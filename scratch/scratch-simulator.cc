/*
 * Generic bridge scenario for MATLAB -> ns-3 integration.
 *
 * This file is intentionally written as a backend-neutral runner scaffold:
 * - scenario size, node positions, frequencies, and bandwidth are NOT hard-coded
 * - the Python runner materializes JSON state files and passes their paths here
 * - this executable reads the state files and constructs a runtime scenario plan
 *
 * At the moment, the actual NR radio simulation is still a placeholder.
 * The goal of this file is to make the future switch from a fixed demo topology
 * to a JSON-driven scenario explicit, without requiring source edits for
 * changes such as TN / NTN cell count, UE count, positions, bandwidth, etc.
 */

#include "ns3/core-module.h"
#include "runner-control-delay-model.h"
#include "runner-control-delay-model.cc"

#if __has_include("ns3/nr-module.h")
#define SCRATCH_SIM_HAS_NR 1
#include "ns3/antenna-module.h"
#include "ns3/applications-module.h"
#include "ns3/buildings-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/nr-helper.h"
#include "ns3/nr-module.h"
#include "ns3/nr-point-to-point-epc-helper.h"
#include "ns3/point-to-point-helper.h"
#endif

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("ScratchSimulator");

namespace
{

struct NodeSpec
{
    uint32_t id{0};
    std::string cellType{"TN"};
    double x{0.0};
    double y{0.0};
    double z{0.0};
    double carrierFrequencyHz{0.0};
    double bandwidthHz{0.0};
};

struct UeSpec
{
    uint32_t id{0};
    uint32_t servingCellId{0};
    double x{0.0};
    double y{0.0};
    double z{0.0};
};

struct ScenarioPlan
{
    std::string operation{"unknown"};
    double runDurationSec{0.0};
    uint32_t numCells{0};
    std::vector<NodeSpec> cells;
    std::vector<UeSpec> ues;
    std::vector<uint32_t> requestedRntis;
    std::string topologySourcePath;
    std::string channelSourcePath;
    std::string controlEventsSourcePath;
    RunnerControlDelayConfig delayConfig;
    std::string ntnTaCapabilityMode{"all_supported"};
    uint32_t ntnTaSupportedUeCount{0};
    uint32_t ntnTaUnsupportedUeCount{0};
    uint32_t controlEventCount{0};
    uint32_t bwpEventCount{0};
    uint32_t hoEventCount{0};
    uint32_t taEventCount{0};
};

struct MetricsOutput
{
    std::string status{"ok"};
    std::string source{"scratch-simulator"};
    std::string operation{"unknown"};
    std::string requestPath;
    std::string topologyPath;
    std::string channelConfigPath;
    double runDurationSec{0.0};
    double dlThroughputMbps{0.0};
    double ulThroughputMbps{0.0};
    double dlTransmittedBytes{0.0};
    double ulTransmittedBytes{0.0};
    double dlBufferBytes{0.0};
    double ulBufferBytes{0.0};
    uint32_t grantsDl{0};
    uint32_t grantsUl{0};
    double prbUtil{0.0};
    double bler{0.0};
    uint32_t numCells{0};
    uint32_t numUes{0};
    double firstCellBwpDelaySec{0.0};
    double firstHoDelaySec{0.0};
    std::string ntnTaCapabilityMode{"all_supported"};
    uint32_t ntnTaSupportedUeCount{0};
    uint32_t ntnTaUnsupportedUeCount{0};
    uint32_t controlEventCount{0};
    uint32_t bwpEventCount{0};
    uint32_t hoEventCount{0};
    uint32_t taEventCount{0};
    std::vector<double> dlBytesPerCell;
    std::vector<double> ulBytesPerCell;
    std::vector<uint32_t> dlPacketsPerCell;
    std::vector<uint32_t> ulPacketsPerCell;
    std::vector<double> dlBytesPerUe;
    std::vector<double> ulBytesPerUe;
    std::vector<uint32_t> dlPacketsPerUe;
    std::vector<uint32_t> ulPacketsPerUe;
};

std::string
ReadTextFile(const std::string& path)
{
    std::ifstream input(path, std::ios::in);
    if (!input.is_open())
    {
        return "";
    }
    std::ostringstream oss;
    oss << input.rdbuf();
    return oss.str();
}

std::string
EscapeJsonString(const std::string& value)
{
    std::ostringstream oss;
    for (char ch : value)
    {
        switch (ch)
        {
        case '\\':
            oss << "\\\\";
            break;
        case '"':
            oss << "\\\"";
            break;
        case '\b':
            oss << "\\b";
            break;
        case '\f':
            oss << "\\f";
            break;
        case '\n':
            oss << "\\n";
            break;
        case '\r':
            oss << "\\r";
            break;
        case '\t':
            oss << "\\t";
            break;
        default:
            oss << ch;
            break;
        }
    }
    return oss.str();
}

std::string
NormalizePathForJson(const std::string& value)
{
    std::string normalized = value;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    return normalized;
}

std::optional<double>
ExtractFirstNumberAfterKey(const std::string& text, const std::string& key)
{
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(-?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?)");
    std::smatch match;
    if (!std::regex_search(text, match, pattern) || match.size() < 2)
    {
        return std::nullopt;
    }
    return std::stod(match[1].str());
}

std::optional<std::string>
ExtractFirstStringAfterKey(const std::string& text, const std::string& key)
{
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
    std::smatch match;
    if (!std::regex_search(text, match, pattern) || match.size() < 2)
    {
        return std::nullopt;
    }
    return match[1].str();
}

std::optional<bool>
ExtractFirstBoolAfterKey(const std::string& text, const std::string& key)
{
    const std::regex pattern("\"" + key + "\"\\s*:\\s*(true|false)");
    std::smatch match;
    if (!std::regex_search(text, match, pattern) || match.size() < 2)
    {
        return std::nullopt;
    }
    return match[1].str() == "true";
}

std::optional<std::string>
ExtractObjectBodyAfterKey(const std::string& text, const std::string& key)
{
    const auto keyPos = text.find("\"" + key + "\"");
    if (keyPos == std::string::npos)
    {
        return std::nullopt;
    }
    const auto objectStart = text.find('{', keyPos);
    if (objectStart == std::string::npos)
    {
        return std::nullopt;
    }
    int depth = 0;
    size_t objectEnd = std::string::npos;
    for (size_t i = objectStart; i < text.size(); ++i)
    {
        if (text[i] == '{')
        {
            depth++;
        }
        else if (text[i] == '}')
        {
            depth--;
            if (depth == 0)
            {
                objectEnd = i;
                break;
            }
        }
    }
    if (objectEnd == std::string::npos)
    {
        return std::nullopt;
    }
    return text.substr(objectStart, objectEnd - objectStart + 1);
}

std::vector<double>
ExtractNumberArrayAfterKey(const std::string& text, const std::string& key)
{
    std::vector<double> values;
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\\[([^\\]]*)\\]");
    std::smatch match;
    if (!std::regex_search(text, match, pattern) || match.size() < 2)
    {
        return values;
    }

    const std::string body = match[1].str();
    const std::regex numberPattern("(-?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?)");
    auto begin = std::sregex_iterator(body.begin(), body.end(), numberPattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it)
    {
        values.push_back(std::stod((*it)[1].str()));
    }
    return values;
}

std::vector<std::string>
ExtractStringArrayAfterKey(const std::string& text, const std::string& key)
{
    std::vector<std::string> values;
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\\[([^\\]]*)\\]");
    std::smatch match;
    if (!std::regex_search(text, match, pattern) || match.size() < 2)
    {
        return values;
    }

    const std::string body = match[1].str();
    const std::regex stringPattern("\"([^\"]*)\"");
    auto begin = std::sregex_iterator(body.begin(), body.end(), stringPattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it)
    {
        values.push_back((*it)[1].str());
    }
    return values;
}

std::vector<bool>
ExtractBoolArrayAfterKey(const std::string& text, const std::string& key)
{
    std::vector<bool> values;
    const std::regex pattern("\"" + key + "\"\\s*:\\s*\\[([^\\]]*)\\]");
    std::smatch match;
    if (!std::regex_search(text, match, pattern) || match.size() < 2)
    {
        return values;
    }

    const std::string body = match[1].str();
    const std::regex boolPattern("(true|false)");
    auto begin = std::sregex_iterator(body.begin(), body.end(), boolPattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it)
    {
        values.push_back((*it)[1].str() == "true");
    }
    return values;
}

uint32_t
CountMatches(const std::string& text, const std::regex& pattern)
{
    return static_cast<uint32_t>(std::distance(std::sregex_iterator(text.begin(), text.end(), pattern),
                                               std::sregex_iterator()));
}

std::vector<std::array<double, 3>>
ExtractMatrix3AfterKey(const std::string& text, const std::string& key)
{
    std::vector<std::array<double, 3>> rows;
    const std::regex rowPattern("\\[\\s*(-?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?)\\s*,\\s*(-?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?)\\s*,\\s*(-?[0-9]+(\\.[0-9]+)?([eE][+-]?[0-9]+)?)\\s*\\]");

    const auto keyPos = text.find("\"" + key + "\"");
    if (keyPos == std::string::npos)
    {
        return rows;
    }
    const auto arrayStart = text.find('[', keyPos);
    if (arrayStart == std::string::npos)
    {
        return rows;
    }

    int depth = 0;
    size_t arrayEnd = std::string::npos;
    for (size_t i = arrayStart; i < text.size(); ++i)
    {
        if (text[i] == '[')
        {
            depth++;
        }
        else if (text[i] == ']')
        {
            depth--;
            if (depth == 0)
            {
                arrayEnd = i;
                break;
            }
        }
    }
    if (arrayEnd == std::string::npos)
    {
        return rows;
    }

    const std::string body = text.substr(arrayStart, arrayEnd - arrayStart + 1);
    auto begin = std::sregex_iterator(body.begin(), body.end(), rowPattern);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it)
    {
        rows.push_back(
            {std::stod((*it)[1].str()), std::stod((*it)[4].str()), std::stod((*it)[7].str())});
    }
    return rows;
}

ScenarioPlan
BuildScenarioPlanFromState(const std::string& operation,
                           double runDurationSec,
                           const std::string& topologyPath,
                           const std::string& channelPath,
                           const std::string& controlEventsPath,
                           const std::string& requestPath)
{
    ScenarioPlan plan;
    plan.operation = operation;
    plan.runDurationSec = runDurationSec;
    plan.topologySourcePath = topologyPath;
    plan.channelSourcePath = channelPath;
    plan.controlEventsSourcePath = controlEventsPath;

    const std::string topologyJson = ReadTextFile(topologyPath);
    const std::string controlEventsJson = ReadTextFile(controlEventsPath);
    const std::string requestJson = ReadTextFile(requestPath);

    if (topologyJson.empty())
    {
        return plan;
    }

    if (auto numCells = ExtractFirstNumberAfterKey(topologyJson, "num_cells"))
    {
        plan.numCells = static_cast<uint32_t>(std::max(0.0, *numCells));
    }

    const auto carrierFrequencies = ExtractNumberArrayAfterKey(topologyJson, "current_carrier_frequency_per_cell_hz");
    const auto bandwidths = ExtractNumberArrayAfterKey(topologyJson, "current_bandwidth_per_cell_hz");
    const auto gnbPositions = ExtractMatrix3AfterKey(topologyJson, "gnb_positions_m");
    const auto cellTypes = ExtractStringArrayAfterKey(topologyJson, "cell_type");

    for (uint32_t cellIdx = 0; cellIdx < plan.numCells; ++cellIdx)
    {
        NodeSpec cell;
        cell.id = cellIdx + 1;
        if (cellIdx < cellTypes.size())
        {
            cell.cellType = cellTypes[cellIdx];
        }
        if (cellIdx < gnbPositions.size())
        {
            cell.x = gnbPositions[cellIdx][0];
            cell.y = gnbPositions[cellIdx][1];
            cell.z = gnbPositions[cellIdx][2];
        }
        if (cellIdx < carrierFrequencies.size())
        {
            cell.carrierFrequencyHz = carrierFrequencies[cellIdx];
        }
        if (cellIdx < bandwidths.size())
        {
            cell.bandwidthHz = bandwidths[cellIdx];
        }
        plan.cells.push_back(cell);
    }

    const auto uePositions = ExtractMatrix3AfterKey(topologyJson, "ue_positions_m");
    const auto servingCells = ExtractNumberArrayAfterKey(topologyJson, "serving_cell_idx");
    for (uint32_t ueIdx = 0; ueIdx < uePositions.size(); ++ueIdx)
    {
        UeSpec ue;
        ue.id = ueIdx + 1;
        ue.x = uePositions[ueIdx][0];
        ue.y = uePositions[ueIdx][1];
        ue.z = uePositions[ueIdx][2];
        if (ueIdx < servingCells.size())
        {
            ue.servingCellId = static_cast<uint32_t>(std::max(0.0, servingCells[ueIdx]));
        }
        plan.ues.push_back(ue);
    }

    const auto rntiList = ExtractNumberArrayAfterKey(requestJson, "rnti_list");
    for (double rnti : rntiList)
    {
        plan.requestedRntis.push_back(static_cast<uint32_t>(std::max(0.0, rnti)));
    }

    if (auto mode = ExtractFirstStringAfterKey(topologyJson, "ntn_ta_capability_mode"))
    {
        plan.ntnTaCapabilityMode = *mode;
    }
    if (auto supportedCount = ExtractFirstNumberAfterKey(topologyJson, "ntn_ta_supported_ue_count"))
    {
        plan.ntnTaSupportedUeCount = static_cast<uint32_t>(std::max(0.0, *supportedCount));
    }
    if (auto unsupportedCount = ExtractFirstNumberAfterKey(topologyJson, "ntn_ta_unsupported_ue_count"))
    {
        plan.ntnTaUnsupportedUeCount = static_cast<uint32_t>(std::max(0.0, *unsupportedCount));
    }
    if (plan.ntnTaSupportedUeCount == 0 && plan.ntnTaUnsupportedUeCount == 0)
    {
        const auto ueTaCapable = ExtractBoolArrayAfterKey(topologyJson, "ue_ntn_ta_capable");
        for (bool capable : ueTaCapable)
        {
            if (capable)
            {
                ++plan.ntnTaSupportedUeCount;
            }
            else
            {
                ++plan.ntnTaUnsupportedUeCount;
            }
        }
    }

    if (auto delayBody = ExtractObjectBodyAfterKey(topologyJson, "runner_control_delay_model"))
    {
        RunnerControlDelayConfig cfg;
        if (auto value = ExtractFirstStringAfterKey(*delayBody, "schema_version"))
        {
            cfg.schemaVersion = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "base_scheduling_delay_k1"))
        {
            cfg.baseSchedulingDelayK1 = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "base_scheduling_delay_k2"))
        {
            cfg.baseSchedulingDelayK2 = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "delay_offset_k1_multiplier"))
        {
            cfg.delayOffsetK1Multiplier = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "delay_offset_k2_multiplier"))
        {
            cfg.delayOffsetK2Multiplier = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "rrc_lead_time_sec"))
        {
            cfg.rrcLeadTimeSec = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "ntn_simple_distance_m"))
        {
            cfg.ntnSimpleDistanceM = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "ntn_one_way_propagation_delay_sec"))
        {
            cfg.ntnOneWayPropagationDelaySec = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "prb_delay_path_count"))
        {
            cfg.prbDelayPathCount = *value;
        }
        if (auto value = ExtractFirstNumberAfterKey(*delayBody, "ho_delay_path_count"))
        {
            cfg.hoDelayPathCount = *value;
        }
        if (auto value = ExtractFirstStringAfterKey(*delayBody, "control_transport_mode"))
        {
            cfg.controlTransportMode = *value;
        }
        cfg.delayOffsetSlots = ExtractNumberArrayAfterKey(*delayBody, "delay_offset_slots");
        cfg.rrcLeadTimeRangeSec = ExtractNumberArrayAfterKey(*delayBody, "rrc_lead_time_range_sec");
        cfg.cuDuPropagationDelaySec = ExtractNumberArrayAfterKey(*delayBody, "cu_du_propagation_delay_sec");
        const auto unifiedNtn = ExtractFirstBoolAfterKey(*delayBody, "enable_unified_ntn_link_model");
        if (unifiedNtn.has_value())
        {
            cfg.enableUnifiedNtnLinkModel = *unifiedNtn;
        }
        plan.delayConfig = cfg;
    }

    const std::string& controlJsonSource = controlEventsJson.empty() ? topologyJson : controlEventsJson;
    plan.controlEventCount = CountMatches(controlJsonSource, std::regex("\"event_type\"\\s*:"));
    plan.bwpEventCount = CountMatches(controlJsonSource,
                                      std::regex("\"event_type\"\\s*:\\s*\"(BwpReconfigRequested|BwpReconfigApplied|bwp_reconfig)\""));
    plan.hoEventCount = CountMatches(controlJsonSource,
                                     std::regex("\"event_type\"\\s*:\\s*\"(HandoverRequested|HandoverStarted|HandoverCompleted|handover)\""));
    plan.taEventCount = CountMatches(controlJsonSource,
                                     std::regex("\"event_type\"\\s*:\\s*\"(TaAdjustmentRequested|TaAdjustmentApplied|timing_advance)\""));

    return plan;
}

MetricsOutput
RunPlaceholderGenericScenario(const ScenarioPlan& plan, const std::string& requestPath)
{
    MetricsOutput metrics;
    metrics.operation = plan.operation;
    metrics.requestPath = requestPath;
    metrics.topologyPath = plan.topologySourcePath;
    metrics.channelConfigPath = plan.channelSourcePath;
    metrics.runDurationSec = plan.runDurationSec;
    metrics.numCells = static_cast<uint32_t>(plan.cells.size());
    metrics.numUes = static_cast<uint32_t>(plan.ues.size());
    metrics.ntnTaCapabilityMode = plan.ntnTaCapabilityMode;
    metrics.ntnTaSupportedUeCount = plan.ntnTaSupportedUeCount;
    metrics.ntnTaUnsupportedUeCount = plan.ntnTaUnsupportedUeCount;
    metrics.controlEventCount = plan.controlEventCount;
    metrics.bwpEventCount = plan.bwpEventCount;
    metrics.hoEventCount = plan.hoEventCount;
    metrics.taEventCount = plan.taEventCount;
    RunnerControlDelayModel delayModel(plan.delayConfig);
    if (!plan.cells.empty())
    {
        const bool firstIsNtn = plan.cells.front().cellType == "NTN";
        const auto diag = delayModel.EvaluateBwpUpdateDelay(plan.cells.front().id, firstIsNtn);
        metrics.firstCellBwpDelaySec = diag.totalDelaySec;
    }
    if (plan.cells.size() >= 2)
    {
        const bool involvesNtn = plan.cells.front().cellType == "NTN" || plan.cells[1].cellType == "NTN";
        const auto hoDiag = delayModel.EvaluateHandoverDelay(plan.cells.front().id, plan.cells[1].id, involvesNtn);
        metrics.firstHoDelaySec = hoDiag.totalDelaySec;
    }

    /*
     * Placeholder phase:
     * We intentionally return zeros until the actual 5G-LENA / NR device
     * installation, traffic creation, and statistics collection are wired in.
     *
     * The important part for now is that the topology is generic:
     * changing TN / NTN cell count or UE layout should only change the JSON
     * input and must not require editing this source file.
     */
    return metrics;
}

#if SCRATCH_SIM_HAS_NR
bool
SupportsMinimalTnNrScenario(const ScenarioPlan& plan)
{
    if (plan.cells.empty() || plan.ues.empty())
    {
        return false;
    }

    uint32_t tnCellCount = 0;
    std::map<uint32_t, bool> activeTnCellIds;
    for (const auto& cell : plan.cells)
    {
        if (cell.cellType == "TN" && cell.bandwidthHz > 0.0)
        {
            ++tnCellCount;
            activeTnCellIds[cell.id] = true;
        }
    }
    if (tnCellCount == 0)
    {
        return false;
    }

    return std::any_of(plan.ues.begin(), plan.ues.end(), [&activeTnCellIds](const UeSpec& ue) {
        return activeTnCellIds.find(ue.servingCellId) != activeTnCellIds.end();
    });
}

MetricsOutput
RunMinimalTnNrScenario(const ScenarioPlan& plan, const std::string& requestPath)
{
    MetricsOutput metrics = RunPlaceholderGenericScenario(plan, requestPath);
    metrics.source = "scratch-simulator-tn-subset-nr";
    metrics.dlBytesPerCell.assign(plan.cells.size(), 0.0);
    metrics.ulBytesPerCell.assign(plan.cells.size(), 0.0);
    metrics.dlPacketsPerCell.assign(plan.cells.size(), 0);
    metrics.ulPacketsPerCell.assign(plan.cells.size(), 0);
    metrics.dlBytesPerUe.assign(plan.ues.size(), 0.0);
    metrics.ulBytesPerUe.assign(plan.ues.size(), 0.0);
    metrics.dlPacketsPerUe.assign(plan.ues.size(), 0);
    metrics.ulPacketsPerUe.assign(plan.ues.size(), 0);

    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    std::vector<NodeSpec> tnCells;
    std::map<uint32_t, uint32_t> tnCellIdToLocalIndex;
    for (const auto& cell : plan.cells)
    {
        if (cell.cellType == "TN" && cell.bandwidthHz > 0.0)
        {
            tnCellIdToLocalIndex[cell.id] = static_cast<uint32_t>(tnCells.size());
            tnCells.push_back(cell);
        }
    }

    std::vector<UeSpec> attachedUes;
    attachedUes.reserve(plan.ues.size());
    for (const auto& ue : plan.ues)
    {
        if (tnCellIdToLocalIndex.find(ue.servingCellId) != tnCellIdToLocalIndex.end())
        {
            attachedUes.push_back(ue);
        }
    }
    if (tnCells.empty() || attachedUes.empty())
    {
        return metrics;
    }

    NodeContainer gnbNodes;
    NodeContainer ueNodes;
    gnbNodes.Create(tnCells.size());
    ueNodes.Create(attachedUes.size());

    Ptr<ListPositionAllocator> gnbPositions = CreateObject<ListPositionAllocator>();
    for (const auto& cell : tnCells)
    {
        gnbPositions->Add(Vector(cell.x, cell.y, cell.z > 0.0 ? cell.z : 25.0));
    }
    MobilityHelper gnbMobility;
    gnbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    gnbMobility.SetPositionAllocator(gnbPositions);
    gnbMobility.Install(gnbNodes);

    Ptr<ListPositionAllocator> uePositions = CreateObject<ListPositionAllocator>();
    for (const auto& ue : attachedUes)
    {
        uePositions->Add(Vector(ue.x, ue.y, ue.z > 0.0 ? ue.z : 1.5));
    }
    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    ueMobility.SetPositionAllocator(uePositions);
    ueMobility.Install(ueNodes);

    Ptr<NrPointToPointEpcHelper> nrEpcHelper = CreateObject<NrPointToPointEpcHelper>();
    Ptr<IdealBeamformingHelper> beamformingHelper = CreateObject<IdealBeamformingHelper>();
    Ptr<NrHelper> nrHelper = CreateObject<NrHelper>();
    nrHelper->SetBeamformingHelper(beamformingHelper);
    nrHelper->SetEpcHelper(nrEpcHelper);

    double centerFrequencyHz = 3.5e9;
    double maxBandwidthHz = 20e6;
    for (const auto& cell : tnCells)
    {
        if (cell.carrierFrequencyHz > 0.0)
        {
            centerFrequencyHz = cell.carrierFrequencyHz;
            break;
        }
    }
    for (const auto& cell : tnCells)
    {
        if (cell.bandwidthHz > maxBandwidthHz)
        {
            maxBandwidthHz = cell.bandwidthHz;
        }
    }

    CcBwpCreator ccBwpCreator;
    CcBwpCreator::SimpleOperationBandConf bandConf(centerFrequencyHz,
                                                   maxBandwidthHz,
                                                   1,
                                                   BandwidthPartInfo::UMa);
    OperationBandInfo band = ccBwpCreator.CreateOperationBandContiguousCc(bandConf);
    nrHelper->InitializeOperationBand(&band);
    BandwidthPartInfoPtrVector allBwps = CcBwpCreator::GetAllBwps({band});

    beamformingHelper->SetAttribute("BeamformingMethod",
                                    TypeIdValue(DirectPathBeamforming::GetTypeId()));
    nrHelper->SetSchedulerTypeId(NrMacSchedulerTdmaRR::GetTypeId());
    nrHelper->SetUeAntennaAttribute("NumRows", UintegerValue(2));
    nrHelper->SetUeAntennaAttribute("NumColumns", UintegerValue(2));
    nrHelper->SetGnbAntennaAttribute("NumRows", UintegerValue(4));
    nrHelper->SetGnbAntennaAttribute("NumColumns", UintegerValue(4));
    nrHelper->SetUeAntennaAttribute("AntennaElement",
                                    PointerValue(CreateObject<IsotropicAntennaModel>()));
    nrHelper->SetGnbAntennaAttribute("AntennaElement",
                                     PointerValue(CreateObject<IsotropicAntennaModel>()));

    NetDeviceContainer gnbDevices = nrHelper->InstallGnbDevice(gnbNodes, allBwps);
    NetDeviceContainer ueDevices = nrHelper->InstallUeDevice(ueNodes, allBwps);

    for (uint32_t idx = 0; idx < gnbDevices.GetN(); ++idx)
    {
        DynamicCast<NrGnbNetDevice>(gnbDevices.Get(idx))->UpdateConfig();
    }
    for (uint32_t idx = 0; idx < ueDevices.GetN(); ++idx)
    {
        DynamicCast<NrUeNetDevice>(ueDevices.Get(idx))->UpdateConfig();
    }

    std::map<uint32_t, Ptr<NetDevice>> gnbByCellId;
    for (uint32_t idx = 0; idx < tnCells.size() && idx < gnbDevices.GetN(); ++idx)
    {
        gnbByCellId[tnCells[idx].id] = gnbDevices.Get(idx);
    }
    for (uint32_t idx = 0; idx < attachedUes.size() && idx < ueDevices.GetN(); ++idx)
    {
        const auto found = gnbByCellId.find(attachedUes[idx].servingCellId);
        if (found != gnbByCellId.end())
        {
            nrHelper->AttachToGnb(ueDevices.Get(idx), found->second);
        }
        else
        {
            NetDeviceContainer singleUe;
            singleUe.Add(ueDevices.Get(idx));
            nrHelper->AttachToClosestGnb(singleUe, gnbDevices);
        }
    }

    Ptr<Node> pgw = nrEpcHelper->GetPgwNode();
    NodeContainer remoteHostContainer;
    remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);

    InternetStackHelper internet;
    internet.Install(remoteHostContainer);
    internet.Install(ueNodes);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", DataRateValue(DataRate("10Gb/s")));
    p2p.SetDeviceAttribute("Mtu", UintegerValue(2500));
    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(10)));
    NetDeviceContainer internetDevices = p2p.Install(pgw, remoteHost);

    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIfaces = ipv4h.Assign(internetDevices);
    Ipv4StaticRoutingHelper routingHelper;
    Ptr<Ipv4StaticRouting> remoteHostStaticRouting =
        routingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteHostStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"),
                                               Ipv4Mask("255.0.0.0"),
                                               1);

    Ipv4InterfaceContainer ueIfaces = nrEpcHelper->AssignUeIpv4Address(ueDevices);
    for (uint32_t idx = 0; idx < ueNodes.GetN(); ++idx)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            routingHelper.GetStaticRouting(ueNodes.Get(idx)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);
    }
    NrEpsBearer defaultBearer(NrEpsBearer::NGBR_LOW_LAT_EMBB);
    nrHelper->ActivateDataRadioBearer(ueDevices, defaultBearer);

    const double simTimeSec = std::max(plan.runDurationSec, 0.6);
    const double appStartSec = 0.1;
    const double appStopSec = std::max(simTimeSec - 0.05, appStartSec + 0.1);
    const double trafficWindowSec = std::max(appStopSec - appStartSec, 0.1);
    const uint32_t packetSizeBytes = 1200;
    const Time packetInterval = MicroSeconds(200);

    ApplicationContainer dlServerApps;
    ApplicationContainer dlClientApps;
    ApplicationContainer ulServerApps;
    ApplicationContainer ulClientApps;
    std::vector<uint16_t> dlPorts;
    std::vector<uint16_t> ulPorts;
    for (uint32_t idx = 0; idx < ueNodes.GetN(); ++idx)
    {
        const uint16_t dlPort = static_cast<uint16_t>(4000 + idx);
        const uint16_t ulPort = static_cast<uint16_t>(5000 + idx);
        dlPorts.push_back(dlPort);
        ulPorts.push_back(ulPort);

        UdpServerHelper dlServer(dlPort);
        dlServerApps.Add(dlServer.Install(ueNodes.Get(idx)));

        UdpClientHelper dlClient(ueIfaces.GetAddress(idx), dlPort);
        dlClient.SetAttribute("Interval", TimeValue(packetInterval));
        dlClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        dlClient.SetAttribute("PacketSize", UintegerValue(packetSizeBytes));
        dlClientApps.Add(dlClient.Install(remoteHost));

        UdpServerHelper ulServer(ulPort);
        ulServerApps.Add(ulServer.Install(remoteHost));

        UdpClientHelper ulClient(internetIfaces.GetAddress(1), ulPort);
        ulClient.SetAttribute("Interval", TimeValue(packetInterval));
        ulClient.SetAttribute("MaxPackets", UintegerValue(0xFFFFFFFF));
        ulClient.SetAttribute("PacketSize", UintegerValue(packetSizeBytes));
        ulClientApps.Add(ulClient.Install(ueNodes.Get(idx)));
    }

    dlServerApps.Start(Seconds(appStartSec));
    dlClientApps.Start(Seconds(appStartSec));
    ulServerApps.Start(Seconds(appStartSec));
    ulClientApps.Start(Seconds(appStartSec));

    dlServerApps.Stop(Seconds(appStopSec));
    dlClientApps.Stop(Seconds(appStopSec));
    ulServerApps.Stop(Seconds(appStopSec));
    ulClientApps.Stop(Seconds(appStopSec));

    Simulator::Stop(Seconds(simTimeSec));
    Simulator::Run();

    uint64_t totalDlPackets = 0;
    uint64_t totalUlPackets = 0;
    for (uint32_t idx = 0; idx < dlServerApps.GetN(); ++idx)
    {
        const uint32_t received = dlServerApps.Get(idx)->GetObject<UdpServer>()->GetReceived();
        totalDlPackets += received;
        if (idx < attachedUes.size())
        {
            const uint32_t ueId = attachedUes[idx].id;
            if (ueId >= 1 && ueId <= metrics.dlPacketsPerUe.size())
            {
                metrics.dlPacketsPerUe[ueId - 1] = received;
                metrics.dlBytesPerUe[ueId - 1] = static_cast<double>(received) * packetSizeBytes;
            }
            const uint32_t cellId = attachedUes[idx].servingCellId;
            if (cellId >= 1 && cellId <= metrics.dlPacketsPerCell.size())
            {
                metrics.dlPacketsPerCell[cellId - 1] += received;
                metrics.dlBytesPerCell[cellId - 1] += static_cast<double>(received) * packetSizeBytes;
            }
        }
    }
    for (uint32_t idx = 0; idx < ulServerApps.GetN(); ++idx)
    {
        const uint32_t received = ulServerApps.Get(idx)->GetObject<UdpServer>()->GetReceived();
        totalUlPackets += received;
        if (idx < attachedUes.size())
        {
            const uint32_t ueId = attachedUes[idx].id;
            if (ueId >= 1 && ueId <= metrics.ulPacketsPerUe.size())
            {
                metrics.ulPacketsPerUe[ueId - 1] = received;
                metrics.ulBytesPerUe[ueId - 1] = static_cast<double>(received) * packetSizeBytes;
            }
            const uint32_t cellId = attachedUes[idx].servingCellId;
            if (cellId >= 1 && cellId <= metrics.ulPacketsPerCell.size())
            {
                metrics.ulPacketsPerCell[cellId - 1] += received;
                metrics.ulBytesPerCell[cellId - 1] += static_cast<double>(received) * packetSizeBytes;
            }
        }
    }

    Simulator::Destroy();

    metrics.dlTransmittedBytes = static_cast<double>(totalDlPackets) * packetSizeBytes;
    metrics.ulTransmittedBytes = static_cast<double>(totalUlPackets) * packetSizeBytes;
    metrics.dlThroughputMbps = (metrics.dlTransmittedBytes * 8.0) / (trafficWindowSec * 1e6);
    metrics.ulThroughputMbps = (metrics.ulTransmittedBytes * 8.0) / (trafficWindowSec * 1e6);
    metrics.grantsDl = static_cast<uint32_t>(totalDlPackets);
    metrics.grantsUl = static_cast<uint32_t>(totalUlPackets);
    return metrics;
}
#endif

void
WriteMetricsJson(const std::string& path, const MetricsOutput& metrics)
{
    std::ofstream output(path, std::ios::out | std::ios::trunc);
    if (!output.is_open())
    {
        NS_FATAL_ERROR("Failed to open metricsPath=" << path);
    }

    output << "{\n";
    output << "  \"status\": \"" << EscapeJsonString(metrics.status) << "\",\n";
    output << "  \"source\": \"" << EscapeJsonString(metrics.source) << "\",\n";
    output << "  \"operation\": \"" << EscapeJsonString(metrics.operation) << "\",\n";
    output << "  \"request_path\": \"" << EscapeJsonString(NormalizePathForJson(metrics.requestPath)) << "\",\n";
    output << "  \"topology_path\": \"" << EscapeJsonString(NormalizePathForJson(metrics.topologyPath)) << "\",\n";
    output << "  \"channel_config_path\": \"" << EscapeJsonString(NormalizePathForJson(metrics.channelConfigPath)) << "\",\n";
    output << "  \"run_duration_sec\": " << metrics.runDurationSec << ",\n";
    output << "  \"num_cells\": " << metrics.numCells << ",\n";
    output << "  \"num_ues\": " << metrics.numUes << ",\n";
    output << "  \"dl_throughput_mbps\": " << metrics.dlThroughputMbps << ",\n";
    output << "  \"ul_throughput_mbps\": " << metrics.ulThroughputMbps << ",\n";
    output << "  \"dl_transmitted_bytes\": " << metrics.dlTransmittedBytes << ",\n";
    output << "  \"ul_transmitted_bytes\": " << metrics.ulTransmittedBytes << ",\n";
    output << "  \"dl_buffer_bytes\": " << metrics.dlBufferBytes << ",\n";
    output << "  \"ul_buffer_bytes\": " << metrics.ulBufferBytes << ",\n";
    output << "  \"grants_dl\": " << metrics.grantsDl << ",\n";
    output << "  \"grants_ul\": " << metrics.grantsUl << ",\n";
    output << "  \"prb_util\": " << metrics.prbUtil << ",\n";
    output << "  \"bler\": " << metrics.bler << ",\n";
    output << "  \"first_cell_bwp_delay_sec\": " << metrics.firstCellBwpDelaySec << ",\n";
    output << "  \"first_ho_delay_sec\": " << metrics.firstHoDelaySec << ",\n";
    output << "  \"ntn_ta_capability_mode\": \"" << EscapeJsonString(metrics.ntnTaCapabilityMode) << "\",\n";
    output << "  \"ntn_ta_supported_ue_count\": " << metrics.ntnTaSupportedUeCount << ",\n";
    output << "  \"ntn_ta_unsupported_ue_count\": " << metrics.ntnTaUnsupportedUeCount << ",\n";
    output << "  \"control_event_count\": " << metrics.controlEventCount << ",\n";
    output << "  \"bwp_event_count\": " << metrics.bwpEventCount << ",\n";
    output << "  \"ho_event_count\": " << metrics.hoEventCount << ",\n";
    output << "  \"ta_event_count\": " << metrics.taEventCount << ",\n";

    auto writeNumberArray = [&output](const std::string& key, const auto& values) {
        output << "  \"" << EscapeJsonString(key) << "\": [";
        for (size_t idx = 0; idx < values.size(); ++idx)
        {
            if (idx > 0)
            {
                output << ", ";
            }
            output << values[idx];
        }
        output << "]";
    };

    writeNumberArray("dl_bytes_per_cell", metrics.dlBytesPerCell);
    output << ",\n";
    writeNumberArray("ul_bytes_per_cell", metrics.ulBytesPerCell);
    output << ",\n";
    writeNumberArray("dl_packets_per_cell", metrics.dlPacketsPerCell);
    output << ",\n";
    writeNumberArray("ul_packets_per_cell", metrics.ulPacketsPerCell);
    output << ",\n";
    writeNumberArray("dl_bytes_per_ue", metrics.dlBytesPerUe);
    output << ",\n";
    writeNumberArray("ul_bytes_per_ue", metrics.ulBytesPerUe);
    output << ",\n";
    writeNumberArray("dl_packets_per_ue", metrics.dlPacketsPerUe);
    output << ",\n";
    writeNumberArray("ul_packets_per_ue", metrics.ulPacketsPerUe);
    output << "\n";
    output << "}\n";
}

} // namespace

int
main(int argc, char* argv[])
{
    std::string requestPath;
    std::string metricsPath;
    std::string topologyPath;
    std::string channelConfigPath;
    std::string controlEventsPath;
    std::string operation = "unknown";
    double runDurationSec = 0.0;

    CommandLine cmd(__FILE__);
    cmd.AddValue("requestPath", "Path to backend request JSON", requestPath);
    cmd.AddValue("metricsPath", "Path to backend metrics JSON", metricsPath);
    cmd.AddValue("topologyPath", "Path to persisted topology JSON", topologyPath);
    cmd.AddValue("channelConfigPath", "Path to persisted channel-config JSON", channelConfigPath);
    cmd.AddValue("controlEventsPath", "Path to persisted control-events JSON", controlEventsPath);
    cmd.AddValue("operation", "Backend operation name", operation);
    cmd.AddValue("runDurationSec", "Requested run duration in seconds", runDurationSec);
    cmd.Parse(argc, argv);

    NS_LOG_UNCOND("Scratch Simulator");
    NS_LOG_UNCOND("Operation=" << operation);
    NS_LOG_UNCOND("RunDurationSec=" << runDurationSec);
    NS_LOG_UNCOND("TopologyPath=" << topologyPath);
    NS_LOG_UNCOND("ChannelConfigPath=" << channelConfigPath);
    NS_LOG_UNCOND("ControlEventsPath=" << controlEventsPath);

    const ScenarioPlan plan =
        BuildScenarioPlanFromState(operation,
                                   runDurationSec,
                                   topologyPath,
                                   channelConfigPath,
                                   controlEventsPath,
                                   requestPath);

    NS_LOG_UNCOND("ScenarioPlan: Cells=" << plan.cells.size() << " UEs=" << plan.ues.size());

    if (!metricsPath.empty())
    {
        MetricsOutput metrics;
#if SCRATCH_SIM_HAS_NR
        if (SupportsMinimalTnNrScenario(plan))
        {
            NS_LOG_UNCOND("ScenarioMode=minimal-tn-nr");
            metrics = RunMinimalTnNrScenario(plan, requestPath);
        }
        else
#endif
        {
            NS_LOG_UNCOND("ScenarioMode=placeholder-generic");
            metrics = RunPlaceholderGenericScenario(plan, requestPath);
        }
        WriteMetricsJson(metricsPath, metrics);
    }

    return 0;
}
