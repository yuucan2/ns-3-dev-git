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
#include <limits>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
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
    struct GrantTraceEntry
    {
        uint32_t cellId{0};
        std::string grantType;
        uint32_t frame{0};
        uint32_t slot{0};
        uint32_t startSymbol{0};
        uint32_t numSymbols{0};
        uint32_t rnti{0};
        uint32_t mcs{0};
        uint32_t tbSize{0};
        uint32_t rv{0};
        uint32_t ndi{0};
        uint32_t bwpId{0};
        uint32_t numRb{0};
        uint32_t cqi{0};
        double rawSinr{std::numeric_limits<double>::quiet_NaN()};
        double effectiveSinr{std::numeric_limits<double>::quiet_NaN()};
        std::string txType;
    };

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
    double dlOfferedBytes{0.0};
    double ulOfferedBytes{0.0};
    double debugRemoteHostIpv4TxBytes{0.0};
    double debugPgwIpv4RxBytes{0.0};
    double debugPgwIpv4TxBytes{0.0};
    double debugUeIpv4RxBytes{0.0};
    double debugPgwAppRxFromTunBytes{0.0};
    double debugPgwAppRxFromS1uBytes{0.0};
    double debugGnbAppRxFromS1uBytes{0.0};
    double debugGnbAppRxFromGnbBytes{0.0};
    double debugGnbPdcpTxBytes{0.0};
    double debugGnbPdcpRxBytes{0.0};
    double debugUePdcpTxBytes{0.0};
    double debugUePdcpRxBytes{0.0};
    double debugGnbRlcTxBytes{0.0};
    double debugGnbRlcRxBytes{0.0};
    double debugUeRlcTxBytes{0.0};
    double debugUeRlcRxBytes{0.0};
    double debugGnbRlcBufferReportBytes{0.0};
    uint32_t debugGnbRlcBufferReportCount{0};
    double debugGnbMacBufferStatusBytes{0.0};
    uint32_t debugGnbMacBufferStatusCount{0};
    uint32_t debugDlActiveUeCount{0};
    double debugDlActiveUeBytes{0.0};
    uint32_t debugDlDciFailureCount{0};
    uint32_t debugDlActiveHarqCount{0};
    uint32_t debugDlSymbolsAvailable{0};
    double debugDlScheduledBytes{0.0};
    uint32_t debugDlSchedulingCount{0};
    uint32_t debugUeConnectionEstablishedCount{0};
    uint32_t debugUeDrbCreatedCount{0};
    double dlBufferBytes{0.0};
    double ulBufferBytes{0.0};
    uint32_t grantsDl{0};
    uint32_t grantsUl{0};
    uint32_t dlOfferedPackets{0};
    uint32_t ulOfferedPackets{0};
    double prbUtil{0.0};
    double bler{0.0};
    double ulBler{0.0};
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
    std::vector<double> prbUtilPerCell;
    std::vector<double> dlBlerPerCell;
    std::vector<double> ulBlerPerCell;
    std::vector<uint32_t> dlPacketsPerCell;
    std::vector<uint32_t> ulPacketsPerCell;
    std::vector<double> dlBytesPerUe;
    std::vector<double> ulBytesPerUe;
    std::vector<uint32_t> dlPacketsPerUe;
    std::vector<uint32_t> ulPacketsPerUe;
    std::vector<GrantTraceEntry> grantTrace;
};

void
RecordCurrentCellSinr(std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>* sinrDbByCellRntiBwp,
                      uint16_t cellId,
                      uint16_t rnti,
                      [[maybe_unused]] double power,
                      double avgSinr,
                      uint8_t bwpId)
{
    if (sinrDbByCellRntiBwp == nullptr)
    {
        return;
    }
    double sinrDb = std::numeric_limits<double>::quiet_NaN();
    if (std::isfinite(avgSinr) && avgSinr > 0.0)
    {
        sinrDb = 10.0 * std::log10(avgSinr);
    }
    (*sinrDbByCellRntiBwp)[std::make_tuple(cellId, rnti, static_cast<uint16_t>(bwpId))] = sinrDb;
}

void
RecordIpv4L3Bytes(double* counter,
                  Ptr<const Packet> packet,
                  [[maybe_unused]] Ptr<Ipv4> ipv4,
                  [[maybe_unused]] uint32_t interface)
{
    if (counter && packet)
    {
        *counter += static_cast<double>(packet->GetSize());
    }
}

void
RecordPacketBytes(double* counter, Ptr<Packet> packet)
{
    if (counter && packet)
    {
        *counter += static_cast<double>(packet->GetSize());
    }
}

void
RecordPdcpTxBytes(double* counter,
                  [[maybe_unused]] uint16_t rnti,
                  [[maybe_unused]] uint8_t lcid,
                  uint32_t size)
{
    if (counter)
    {
        *counter += static_cast<double>(size);
    }
}

void
RecordPdcpRxBytes(double* counter,
                  [[maybe_unused]] uint16_t rnti,
                  [[maybe_unused]] uint8_t lcid,
                  uint32_t size,
                  [[maybe_unused]] uint64_t delay)
{
    if (counter)
    {
        *counter += static_cast<double>(size);
    }
}

void
RecordRlcTxBytes(double* counter,
                 [[maybe_unused]] uint16_t rnti,
                 [[maybe_unused]] uint8_t lcid,
                 uint32_t size)
{
    if (counter)
    {
        *counter += static_cast<double>(size);
    }
}

void
RecordRlcRxBytes(double* counter,
                 [[maybe_unused]] uint16_t rnti,
                 [[maybe_unused]] uint8_t lcid,
                 uint32_t size,
                 [[maybe_unused]] uint64_t delay)
{
    if (counter)
    {
        *counter += static_cast<double>(size);
    }
}

void
RecordBufferReport(double* bytesCounter,
                   uint32_t* countCounter,
                   [[maybe_unused]] uint16_t rnti,
                   [[maybe_unused]] uint8_t lcid,
                   uint32_t txQueueBytes,
                   uint32_t retxQueueBytes,
                   uint16_t statusPduBytes)
{
    if (bytesCounter)
    {
        *bytesCounter += static_cast<double>(txQueueBytes) +
                         static_cast<double>(retxQueueBytes) +
                         static_cast<double>(statusPduBytes);
    }
    if (countCounter)
    {
        *countCounter += 1;
    }
}

void
RecordDlScheduling(double* bytesCounter,
                   uint32_t* countCounter,
                   NrSchedulingCallbackInfo info)
{
    if (bytesCounter)
    {
        *bytesCounter += static_cast<double>(info.m_tbSize);
    }
    if (countCounter)
    {
        *countCounter += 1;
    }
}

void
RecordDlActiveUe(uint32_t* countCounter, double* bytesCounter, uint32_t count, uint32_t bytes)
{
    if (countCounter)
    {
        *countCounter = count;
    }
    if (bytesCounter)
    {
        *bytesCounter = static_cast<double>(bytes);
    }
}

void
RecordDlDciFailure(uint32_t* countCounter,
                   [[maybe_unused]] uint16_t rnti,
                   [[maybe_unused]] uint32_t rbg,
                   [[maybe_unused]] uint32_t tbSize)
{
    if (countCounter)
    {
        *countCounter += 1;
    }
}

void
RecordDlScheduleContext(uint32_t* harqCounter,
                        uint32_t* ueCounter,
                        uint32_t* symCounter,
                        uint32_t activeHarqCount,
                        uint32_t activeUeCount,
                        uint32_t dlSymbolsAvailable)
{
    if (harqCounter)
    {
        *harqCounter = activeHarqCount;
    }
    if (ueCounter)
    {
        *ueCounter = activeUeCount;
    }
    if (symCounter)
    {
        *symCounter = dlSymbolsAvailable;
    }
}

void
RecordCount3(uint32_t* counter,
             [[maybe_unused]] uint64_t imsi,
             [[maybe_unused]] uint16_t cellId,
             [[maybe_unused]] uint16_t rnti)
{
    if (counter)
    {
        *counter += 1;
    }
}

void
RecordCount4(uint32_t* counter,
             [[maybe_unused]] uint64_t imsi,
             [[maybe_unused]] uint16_t cellId,
             [[maybe_unused]] uint16_t rnti,
             [[maybe_unused]] uint8_t lcid)
{
    if (counter)
    {
        *counter += 1;
    }
}

void
ConnectPdcpTraceSinks(NetDeviceContainer devices, bool isUe, MetricsOutput* metrics)
{
    if (metrics == nullptr)
    {
        return;
    }

    for (uint32_t idx = 0; idx < devices.GetN(); ++idx)
    {
        Ptr<Object> rrcObject;
        if (isUe)
        {
            Ptr<NrUeNetDevice> ue = DynamicCast<NrUeNetDevice>(devices.Get(idx));
            if (!ue)
            {
                continue;
            }
            rrcObject = ue->GetRrc();
        }
        else
        {
            Ptr<NrGnbNetDevice> gnb = DynamicCast<NrGnbNetDevice>(devices.Get(idx));
            if (!gnb)
            {
                continue;
            }
            rrcObject = gnb->GetRrc();
        }

        if (!rrcObject)
        {
            continue;
        }

        if (isUe)
        {
            ObjectMapValue drbMapValue;
            if (!rrcObject->GetAttributeFailSafe("DataRadioBearerMap", drbMapValue))
            {
                continue;
            }
            for (auto drbIt = drbMapValue.Begin(); drbIt != drbMapValue.End(); ++drbIt)
            {
                Ptr<NrDataRadioBearerInfo> drbInfo =
                    drbIt->second->GetObject<NrDataRadioBearerInfo>();
                if (!drbInfo || !drbInfo->m_pdcp)
                {
                    continue;
                }
                drbInfo->m_pdcp->TraceConnectWithoutContext(
                    "TxPDU",
                    MakeBoundCallback(&RecordPdcpTxBytes, &metrics->debugUePdcpTxBytes));
                drbInfo->m_pdcp->TraceConnectWithoutContext(
                    "RxPDU",
                    MakeBoundCallback(&RecordPdcpRxBytes, &metrics->debugUePdcpRxBytes));
                if (drbInfo->m_rlc)
                {
                    drbInfo->m_rlc->TraceConnectWithoutContext(
                        "TxPDU",
                        MakeBoundCallback(&RecordRlcTxBytes, &metrics->debugUeRlcTxBytes));
                    drbInfo->m_rlc->TraceConnectWithoutContext(
                        "RxPDU",
                        MakeBoundCallback(&RecordRlcRxBytes, &metrics->debugUeRlcRxBytes));
                    drbInfo->m_rlc->TraceConnectWithoutContext(
                        "BufferReport",
                        MakeBoundCallback(&RecordBufferReport,
                                          &metrics->debugGnbRlcBufferReportBytes,
                                          &metrics->debugGnbRlcBufferReportCount));
                }
            }
            continue;
        }

        ObjectMapValue ueMapValue;
        if (!rrcObject->GetAttributeFailSafe("UeMap", ueMapValue))
        {
            continue;
        }
        for (auto ueIt = ueMapValue.Begin(); ueIt != ueMapValue.End(); ++ueIt)
        {
            Ptr<Object> ueManager = ueIt->second;
            if (!ueManager)
            {
                continue;
            }
            ObjectMapValue drbMapValue;
            if (!ueManager->GetAttributeFailSafe("DataRadioBearerMap", drbMapValue))
            {
                continue;
            }
            for (auto drbIt = drbMapValue.Begin(); drbIt != drbMapValue.End(); ++drbIt)
            {
                Ptr<NrDataRadioBearerInfo> drbInfo =
                    drbIt->second->GetObject<NrDataRadioBearerInfo>();
                if (!drbInfo || !drbInfo->m_pdcp)
                {
                    continue;
                }
                drbInfo->m_pdcp->TraceConnectWithoutContext(
                    "TxPDU",
                    MakeBoundCallback(&RecordPdcpTxBytes, &metrics->debugGnbPdcpTxBytes));
                drbInfo->m_pdcp->TraceConnectWithoutContext(
                    "RxPDU",
                    MakeBoundCallback(&RecordPdcpRxBytes, &metrics->debugGnbPdcpRxBytes));
                if (drbInfo->m_rlc)
                {
                    drbInfo->m_rlc->TraceConnectWithoutContext(
                        "TxPDU",
                        MakeBoundCallback(&RecordRlcTxBytes, &metrics->debugGnbRlcTxBytes));
                    drbInfo->m_rlc->TraceConnectWithoutContext(
                        "RxPDU",
                        MakeBoundCallback(&RecordRlcRxBytes, &metrics->debugGnbRlcRxBytes));
                    drbInfo->m_rlc->TraceConnectWithoutContext(
                        "BufferReport",
                        MakeBoundCallback(&RecordBufferReport,
                                          &metrics->debugGnbRlcBufferReportBytes,
                                          &metrics->debugGnbRlcBufferReportCount));
                }
            }
        }
    }
}

void
RecordSlotDataStats(const std::map<uint16_t, uint32_t>* traceCellIdToPlanIndex,
                    std::vector<double>* usedRePerCell,
                    std::vector<double>* availRePerCell,
                    [[maybe_unused]] const SfnSf& sfn,
                    [[maybe_unused]] uint32_t activeUe,
                    uint32_t usedRe,
                    [[maybe_unused]] uint32_t usedSymbols,
                    uint32_t availableRb,
                    uint32_t availableSymbols,
                    [[maybe_unused]] uint16_t bwpId,
                    uint16_t cellId)
{
    if (traceCellIdToPlanIndex == nullptr || usedRePerCell == nullptr || availRePerCell == nullptr)
    {
        return;
    }
    const auto found = traceCellIdToPlanIndex->find(cellId);
    if (found == traceCellIdToPlanIndex->end())
    {
        return;
    }
    const uint32_t planIndex = found->second;
    if (planIndex >= usedRePerCell->size() || planIndex >= availRePerCell->size())
    {
        return;
    }
    (*usedRePerCell)[planIndex] += static_cast<double>(usedRe);
    (*availRePerCell)[planIndex] += static_cast<double>(availableRb) *
                                     static_cast<double>(availableSymbols) * 12.0;
}

void
RecordGrantTrace(const std::map<uint16_t, uint32_t>* traceCellIdToPlanIndex,
                 const std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>* sinrDbByCellRntiBwp,
                 std::vector<uint32_t>* tbPerCell,
                 std::vector<uint32_t>* corruptTbPerCell,
                 std::vector<MetricsOutput::GrantTraceEntry>* grantTrace,
                 const std::string& grantType,
                 RxPacketTraceParams params)
{
    if (traceCellIdToPlanIndex == nullptr || tbPerCell == nullptr || corruptTbPerCell == nullptr)
    {
        return;
    }
    const auto found = traceCellIdToPlanIndex->find(static_cast<uint16_t>(params.m_cellId));
    if (found == traceCellIdToPlanIndex->end())
    {
        return;
    }
    const uint32_t planIndex = found->second;
    if (planIndex >= tbPerCell->size() || planIndex >= corruptTbPerCell->size())
    {
        return;
    }
    (*tbPerCell)[planIndex] += 1;
    if (params.m_corrupt)
    {
        (*corruptTbPerCell)[planIndex] += 1;
    }
    if (grantTrace != nullptr)
    {
        MetricsOutput::GrantTraceEntry entry;
        entry.cellId = planIndex + 1;
        entry.grantType = grantType;
        entry.frame = params.m_frameNum;
        entry.slot = params.m_slotNum;
        entry.startSymbol = params.m_symStart;
        entry.numSymbols = params.m_numSym;
        entry.rnti = params.m_rnti;
        entry.mcs = params.m_mcs;
        entry.tbSize = params.m_tbSize;
        entry.rv = params.m_rv;
        entry.ndi = 0;
        entry.bwpId = params.m_bwpId;
        entry.numRb = params.m_rbAssignedNum;
        entry.cqi = params.m_cqi;
        if (entry.cqi > 15U && entry.cqi % 16U == 0U)
        {
            entry.cqi /= 16U;
        }
        entry.rawSinr = params.m_sinr;
        entry.effectiveSinr = params.m_sinrEff;
        if (!std::isfinite(entry.rawSinr) || entry.rawSinr < 0.0)
        {
            if (sinrDbByCellRntiBwp != nullptr)
            {
                auto sinrIt = sinrDbByCellRntiBwp->find(
                    std::make_tuple(static_cast<uint16_t>(params.m_cellId),
                                    params.m_rnti,
                                    params.m_bwpId));
                if (sinrIt != sinrDbByCellRntiBwp->end())
                {
                    entry.rawSinr = sinrIt->second;
                }
            }
        }
        entry.txType = params.m_rv > 0 ? "reTx" : "newTx";
        grantTrace->push_back(entry);
    }
}

void
RecordDlRxPacketTrace(const std::map<uint16_t, uint32_t>* traceCellIdToPlanIndex,
                      const std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>* sinrDbByCellRntiBwp,
                      std::vector<uint32_t>* dlTbPerCell,
                      std::vector<uint32_t>* dlCorruptTbPerCell,
                      std::vector<MetricsOutput::GrantTraceEntry>* grantTrace,
                      RxPacketTraceParams params)
{
    RecordGrantTrace(traceCellIdToPlanIndex,
                     sinrDbByCellRntiBwp,
                     dlTbPerCell,
                     dlCorruptTbPerCell,
                     grantTrace,
                     "DL",
                     params);
}

void
RecordUlRxPacketTrace(const std::map<uint16_t, uint32_t>* traceCellIdToPlanIndex,
                      const std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>* sinrDbByCellRntiBwp,
                      std::vector<uint32_t>* ulTbPerCell,
                      std::vector<uint32_t>* ulCorruptTbPerCell,
                      std::vector<MetricsOutput::GrantTraceEntry>* grantTrace,
                      RxPacketTraceParams params)
{
    RecordGrantTrace(traceCellIdToPlanIndex,
                     sinrDbByCellRntiBwp,
                     ulTbPerCell,
                     ulCorruptTbPerCell,
                     grantTrace,
                     "UL",
                     params);
}

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
    metrics.prbUtilPerCell.assign(plan.cells.size(), 0.0);
    metrics.dlBlerPerCell.assign(plan.cells.size(), 0.0);
    metrics.ulBlerPerCell.assign(plan.cells.size(), 0.0);
    metrics.dlPacketsPerCell.assign(plan.cells.size(), 0);
    metrics.ulPacketsPerCell.assign(plan.cells.size(), 0);
    metrics.dlBytesPerUe.assign(plan.ues.size(), 0.0);
    metrics.ulBytesPerUe.assign(plan.ues.size(), 0.0);
    metrics.dlPacketsPerUe.assign(plan.ues.size(), 0);
    metrics.ulPacketsPerUe.assign(plan.ues.size(), 0);
    std::vector<double> usedRePerCell(plan.cells.size(), 0.0);
    std::vector<double> availRePerCell(plan.cells.size(), 0.0);
    std::vector<uint32_t> dlTbPerCell(plan.cells.size(), 0);
    std::vector<uint32_t> dlCorruptTbPerCell(plan.cells.size(), 0);
    std::vector<uint32_t> ulTbPerCell(plan.cells.size(), 0);
    std::vector<uint32_t> ulCorruptTbPerCell(plan.cells.size(), 0);
    std::map<uint16_t, uint32_t> traceCellIdToPlanIndex;
    std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double> sinrDbByCellRntiBwp;

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

    // Avoid starving DL when both directions are backlogged: with the default
    // all-flexible pattern, the UL scheduler can consume every data symbol.
    const std::string tddPattern = "DL|DL|S|UL|UL|DL|DL|S|UL|UL|";
    for (uint32_t idx = 0; idx < gnbDevices.GetN(); ++idx)
    {
        Ptr<NrGnbNetDevice> gnb = DynamicCast<NrGnbNetDevice>(gnbDevices.Get(idx));
        if (gnb && gnb->GetPhy(0))
        {
            gnb->GetPhy(0)->SetPattern(tddPattern);
        }
    }
    for (uint32_t idx = 0; idx < ueDevices.GetN(); ++idx)
    {
        Ptr<NrUeNetDevice> ue = DynamicCast<NrUeNetDevice>(ueDevices.Get(idx));
        if (ue && ue->GetPhy(0))
        {
            ue->GetPhy(0)->SetPattern(tddPattern);
        }
    }

    for (uint32_t idx = 0; idx < gnbDevices.GetN(); ++idx)
    {
        DynamicCast<NrGnbNetDevice>(gnbDevices.Get(idx))->UpdateConfig();
    }
    for (uint32_t idx = 0; idx < ueDevices.GetN(); ++idx)
    {
        DynamicCast<NrUeNetDevice>(ueDevices.Get(idx))->UpdateConfig();
    }

    for (uint32_t idx = 0; idx < gnbDevices.GetN(); ++idx)
    {
        Ptr<NrGnbNetDevice> gnb = DynamicCast<NrGnbNetDevice>(gnbDevices.Get(idx));
        if (!gnb)
        {
            continue;
        }
        Ptr<NrGnbPhy> phy = gnb->GetPhy(0);
        traceCellIdToPlanIndex[gnb->GetCellId()] = tnCells[idx].id - 1;
        Ptr<NrGnbMac> mac = gnb->GetMac(0);
        Ptr<NrMacSchedulerNs3> scheduler =
            DynamicCast<NrMacSchedulerNs3>(NrHelper::GetScheduler(gnbDevices.Get(idx), 0));
        if (phy)
        {
            phy->TraceConnectWithoutContext("SlotDataStats",
                                            MakeBoundCallback(&RecordSlotDataStats,
                                                              &traceCellIdToPlanIndex,
                                                              &usedRePerCell,
                                                              &availRePerCell));
        }
        if (mac)
        {
            mac->TraceConnectWithoutContext("DlScheduling",
                                            MakeBoundCallback(&RecordDlScheduling,
                                                              &metrics.debugDlScheduledBytes,
                                                              &metrics.debugDlSchedulingCount));
            mac->TraceConnectWithoutContext("DlBufferStatus",
                                            MakeBoundCallback(&RecordBufferReport,
                                                              &metrics.debugGnbMacBufferStatusBytes,
                                                              &metrics.debugGnbMacBufferStatusCount));
        }
        if (scheduler)
        {
            scheduler->TraceConnectWithoutContext("DlActiveUe",
                                                  MakeBoundCallback(&RecordDlActiveUe,
                                                                    &metrics.debugDlActiveUeCount,
                                                                    &metrics.debugDlActiveUeBytes));
            scheduler->TraceConnectWithoutContext("DlDciFailure",
                                                  MakeBoundCallback(&RecordDlDciFailure,
                                                                    &metrics.debugDlDciFailureCount));
            scheduler->TraceConnectWithoutContext("DlScheduleContext",
                                                  MakeBoundCallback(&RecordDlScheduleContext,
                                                                    &metrics.debugDlActiveHarqCount,
                                                                    &metrics.debugDlActiveUeCount,
                                                                    &metrics.debugDlSymbolsAvailable));
        }
    }
    for (uint32_t idx = 0; idx < ueDevices.GetN(); ++idx)
    {
        Ptr<NrUeNetDevice> ue = DynamicCast<NrUeNetDevice>(ueDevices.Get(idx));
        if (!ue)
        {
            continue;
        }
        Ptr<NrUeRrc> rrc = ue->GetRrc();
        Ptr<NrUePhy> phy = ue->GetPhy(0);
        if (rrc)
        {
            rrc->TraceConnectWithoutContext(
                "ConnectionEstablished",
                MakeBoundCallback(&RecordCount3, &metrics.debugUeConnectionEstablishedCount));
            rrc->TraceConnectWithoutContext("DrbCreated",
                                            MakeBoundCallback(&RecordCount4,
                                                              &metrics.debugUeDrbCreatedCount));
        }
        if (phy && phy->GetSpectrumPhy())
        {
            phy->TraceConnectWithoutContext("ReportCurrentCellRsrpSinr",
                                            MakeBoundCallback(&RecordCurrentCellSinr,
                                                              &sinrDbByCellRntiBwp));
            phy->GetSpectrumPhy()->TraceConnectWithoutContext(
                "RxPacketTraceUe",
                MakeBoundCallback(&RecordDlRxPacketTrace,
                                  &traceCellIdToPlanIndex,
                                  &sinrDbByCellRntiBwp,
                                  &dlTbPerCell,
                                  &dlCorruptTbPerCell,
                                  &metrics.grantTrace));
        }
    }
    for (uint32_t idx = 0; idx < gnbDevices.GetN(); ++idx)
    {
        Ptr<NrGnbNetDevice> gnb = DynamicCast<NrGnbNetDevice>(gnbDevices.Get(idx));
        if (!gnb)
        {
            continue;
        }
        Ptr<NrGnbPhy> phy = gnb->GetPhy(0);
        if (phy && phy->GetSpectrumPhy())
        {
            phy->GetSpectrumPhy()->TraceConnectWithoutContext(
                "RxPacketTraceGnb",
                MakeBoundCallback(&RecordUlRxPacketTrace,
                                  &traceCellIdToPlanIndex,
                                  &sinrDbByCellRntiBwp,
                                  &ulTbPerCell,
                                  &ulCorruptTbPerCell,
                                  &metrics.grantTrace));
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
    Ptr<Ipv4L3Protocol> remoteHostIpv4 = remoteHost->GetObject<Ipv4>()->GetObject<Ipv4L3Protocol>();
    Ptr<Ipv4L3Protocol> pgwIpv4 = pgw->GetObject<Ipv4>()->GetObject<Ipv4L3Protocol>();
    if (remoteHostIpv4)
    {
        remoteHostIpv4->TraceConnectWithoutContext("Tx",
                                                   MakeBoundCallback(&RecordIpv4L3Bytes,
                                                                     &metrics.debugRemoteHostIpv4TxBytes));
    }
    if (pgwIpv4)
    {
        pgwIpv4->TraceConnectWithoutContext("Rx",
                                            MakeBoundCallback(&RecordIpv4L3Bytes,
                                                              &metrics.debugPgwIpv4RxBytes));
        pgwIpv4->TraceConnectWithoutContext("Tx",
                                            MakeBoundCallback(&RecordIpv4L3Bytes,
                                                              &metrics.debugPgwIpv4TxBytes));
    }
    for (uint32_t idx = 0; idx < ueNodes.GetN(); ++idx)
    {
        Ptr<Ipv4L3Protocol> ueIpv4 = ueNodes.Get(idx)->GetObject<Ipv4>()->GetObject<Ipv4L3Protocol>();
        if (ueIpv4)
        {
            ueIpv4->TraceConnectWithoutContext("Rx",
                                               MakeBoundCallback(&RecordIpv4L3Bytes,
                                                                 &metrics.debugUeIpv4RxBytes));
        }
    }
    if (pgw->GetNApplications() > 0)
    {
        Ptr<NrEpcPgwApplication> pgwApp =
            pgw->GetApplication(0)->GetObject<NrEpcPgwApplication>();
        if (pgwApp)
        {
            pgwApp->TraceConnectWithoutContext("RxFromTun",
                                               MakeBoundCallback(&RecordPacketBytes,
                                                                 &metrics.debugPgwAppRxFromTunBytes));
            pgwApp->TraceConnectWithoutContext("RxFromS1u",
                                               MakeBoundCallback(&RecordPacketBytes,
                                                                 &metrics.debugPgwAppRxFromS1uBytes));
        }
    }
    for (uint32_t idx = 0; idx < gnbNodes.GetN(); ++idx)
    {
        if (gnbNodes.Get(idx)->GetNApplications() == 0)
        {
            continue;
        }
        Ptr<NrEpcGnbApplication> gnbApp =
            gnbNodes.Get(idx)->GetApplication(0)->GetObject<NrEpcGnbApplication>();
        if (!gnbApp)
        {
            continue;
        }
        gnbApp->TraceConnectWithoutContext("RxFromS1u",
                                           MakeBoundCallback(&RecordPacketBytes,
                                                             &metrics.debugGnbAppRxFromS1uBytes));
        gnbApp->TraceConnectWithoutContext("RxFromGnb",
                                           MakeBoundCallback(&RecordPacketBytes,
                                                             &metrics.debugGnbAppRxFromGnbBytes));
    }
    for (uint32_t idx = 0; idx < ueNodes.GetN(); ++idx)
    {
        Ptr<Ipv4StaticRouting> ueStaticRouting =
            routingHelper.GetStaticRouting(ueNodes.Get(idx)->GetObject<Ipv4>());
        ueStaticRouting->SetDefaultRoute(nrEpcHelper->GetUeDefaultGatewayAddress(), 1);
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

    // DRBs are created during early RRC setup, so connect PDCP traces shortly
    // before sustained traffic starts flowing.
    Simulator::Schedule(Seconds(std::min(0.2, appStopSec - 0.01)),
                        &ConnectPdcpTraceSinks,
                        gnbDevices,
                        false,
                        &metrics);
    Simulator::Schedule(Seconds(std::min(0.2, appStopSec - 0.01)),
                        &ConnectPdcpTraceSinks,
                        ueDevices,
                        true,
                        &metrics);

    Simulator::Stop(Seconds(simTimeSec));
    Simulator::Run();

    uint64_t totalDlPackets = 0;
    uint64_t totalUlPackets = 0;
    uint64_t totalDlOfferedPackets = 0;
    uint64_t totalUlOfferedPackets = 0;
    for (uint32_t idx = 0; idx < dlClientApps.GetN(); ++idx)
    {
        Ptr<UdpClient> dlClient = dlClientApps.Get(idx)->GetObject<UdpClient>();
        if (dlClient)
        {
            totalDlOfferedPackets += dlClient->GetTotalTx();
        }
    }
    for (uint32_t idx = 0; idx < ulClientApps.GetN(); ++idx)
    {
        Ptr<UdpClient> ulClient = ulClientApps.Get(idx)->GetObject<UdpClient>();
        if (ulClient)
        {
            totalUlOfferedPackets += ulClient->GetTotalTx();
        }
    }
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

    metrics.dlOfferedBytes = static_cast<double>(totalDlOfferedPackets);
    metrics.ulOfferedBytes = static_cast<double>(totalUlOfferedPackets);
    metrics.dlOfferedPackets =
        static_cast<uint32_t>(totalDlOfferedPackets / std::max<uint32_t>(packetSizeBytes, 1u));
    metrics.ulOfferedPackets =
        static_cast<uint32_t>(totalUlOfferedPackets / std::max<uint32_t>(packetSizeBytes, 1u));
    metrics.dlTransmittedBytes = static_cast<double>(totalDlPackets) * packetSizeBytes;
    metrics.ulTransmittedBytes = static_cast<double>(totalUlPackets) * packetSizeBytes;
    metrics.dlThroughputMbps = (metrics.dlTransmittedBytes * 8.0) / (trafficWindowSec * 1e6);
    metrics.ulThroughputMbps = (metrics.ulTransmittedBytes * 8.0) / (trafficWindowSec * 1e6);
    metrics.grantsDl = static_cast<uint32_t>(totalDlPackets);
    metrics.grantsUl = static_cast<uint32_t>(totalUlPackets);
    double totalUsedRe = 0.0;
    double totalAvailRe = 0.0;
    uint64_t totalDlTb = 0;
    uint64_t totalDlCorruptTb = 0;
    uint64_t totalUlTb = 0;
    uint64_t totalUlCorruptTb = 0;
    for (size_t cellIdx = 0; cellIdx < plan.cells.size(); ++cellIdx)
    {
        totalUsedRe += usedRePerCell[cellIdx];
        totalAvailRe += availRePerCell[cellIdx];
        totalDlTb += dlTbPerCell[cellIdx];
        totalDlCorruptTb += dlCorruptTbPerCell[cellIdx];
        totalUlTb += ulTbPerCell[cellIdx];
        totalUlCorruptTb += ulCorruptTbPerCell[cellIdx];
        if (availRePerCell[cellIdx] > 0.0)
        {
            metrics.prbUtilPerCell[cellIdx] =
                std::min(std::max(usedRePerCell[cellIdx] / availRePerCell[cellIdx], 0.0), 1.0);
        }
        if (dlTbPerCell[cellIdx] > 0)
        {
            metrics.dlBlerPerCell[cellIdx] =
                static_cast<double>(dlCorruptTbPerCell[cellIdx]) / static_cast<double>(dlTbPerCell[cellIdx]);
        }
        if (ulTbPerCell[cellIdx] > 0)
        {
            metrics.ulBlerPerCell[cellIdx] =
                static_cast<double>(ulCorruptTbPerCell[cellIdx]) / static_cast<double>(ulTbPerCell[cellIdx]);
        }
    }
    if (totalAvailRe > 0.0)
    {
        metrics.prbUtil = std::min(std::max(totalUsedRe / totalAvailRe, 0.0), 1.0);
    }
    if (totalDlTb > 0)
    {
        metrics.bler = static_cast<double>(totalDlCorruptTb) / static_cast<double>(totalDlTb);
    }
    if (totalUlTb > 0)
    {
        metrics.ulBler = static_cast<double>(totalUlCorruptTb) / static_cast<double>(totalUlTb);
    }
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
    output << "  \"dl_offered_bytes\": " << metrics.dlOfferedBytes << ",\n";
    output << "  \"ul_offered_bytes\": " << metrics.ulOfferedBytes << ",\n";
    output << "  \"debug_remotehost_ipv4_tx_bytes\": " << metrics.debugRemoteHostIpv4TxBytes
           << ",\n";
    output << "  \"debug_pgw_ipv4_rx_bytes\": " << metrics.debugPgwIpv4RxBytes << ",\n";
    output << "  \"debug_pgw_ipv4_tx_bytes\": " << metrics.debugPgwIpv4TxBytes << ",\n";
    output << "  \"debug_ue_ipv4_rx_bytes\": " << metrics.debugUeIpv4RxBytes << ",\n";
    output << "  \"debug_pgw_app_rx_from_tun_bytes\": " << metrics.debugPgwAppRxFromTunBytes
           << ",\n";
    output << "  \"debug_pgw_app_rx_from_s1u_bytes\": " << metrics.debugPgwAppRxFromS1uBytes
           << ",\n";
    output << "  \"debug_gnb_app_rx_from_s1u_bytes\": " << metrics.debugGnbAppRxFromS1uBytes
           << ",\n";
    output << "  \"debug_gnb_app_rx_from_gnb_bytes\": " << metrics.debugGnbAppRxFromGnbBytes
           << ",\n";
    output << "  \"debug_gnb_pdcp_tx_bytes\": " << metrics.debugGnbPdcpTxBytes << ",\n";
    output << "  \"debug_gnb_pdcp_rx_bytes\": " << metrics.debugGnbPdcpRxBytes << ",\n";
    output << "  \"debug_ue_pdcp_tx_bytes\": " << metrics.debugUePdcpTxBytes << ",\n";
    output << "  \"debug_ue_pdcp_rx_bytes\": " << metrics.debugUePdcpRxBytes << ",\n";
    output << "  \"debug_gnb_rlc_tx_bytes\": " << metrics.debugGnbRlcTxBytes << ",\n";
    output << "  \"debug_gnb_rlc_rx_bytes\": " << metrics.debugGnbRlcRxBytes << ",\n";
    output << "  \"debug_ue_rlc_tx_bytes\": " << metrics.debugUeRlcTxBytes << ",\n";
    output << "  \"debug_ue_rlc_rx_bytes\": " << metrics.debugUeRlcRxBytes << ",\n";
    output << "  \"debug_gnb_rlc_buffer_report_bytes\": " << metrics.debugGnbRlcBufferReportBytes
           << ",\n";
    output << "  \"debug_gnb_rlc_buffer_report_count\": " << metrics.debugGnbRlcBufferReportCount
           << ",\n";
    output << "  \"debug_gnb_mac_buffer_status_bytes\": " << metrics.debugGnbMacBufferStatusBytes
           << ",\n";
    output << "  \"debug_gnb_mac_buffer_status_count\": " << metrics.debugGnbMacBufferStatusCount
           << ",\n";
    output << "  \"debug_dl_active_ue_count\": " << metrics.debugDlActiveUeCount << ",\n";
    output << "  \"debug_dl_active_ue_bytes\": " << metrics.debugDlActiveUeBytes << ",\n";
    output << "  \"debug_dl_dci_failure_count\": " << metrics.debugDlDciFailureCount << ",\n";
    output << "  \"debug_dl_active_harq_count\": " << metrics.debugDlActiveHarqCount << ",\n";
    output << "  \"debug_dl_symbols_available\": " << metrics.debugDlSymbolsAvailable << ",\n";
    output << "  \"debug_dl_scheduled_bytes\": " << metrics.debugDlScheduledBytes << ",\n";
    output << "  \"debug_dl_scheduling_count\": " << metrics.debugDlSchedulingCount << ",\n";
    output << "  \"debug_ue_connection_established_count\": "
           << metrics.debugUeConnectionEstablishedCount << ",\n";
    output << "  \"debug_ue_drb_created_count\": " << metrics.debugUeDrbCreatedCount << ",\n";
    output << "  \"dl_buffer_bytes\": " << metrics.dlBufferBytes << ",\n";
    output << "  \"ul_buffer_bytes\": " << metrics.ulBufferBytes << ",\n";
    output << "  \"grants_dl\": " << metrics.grantsDl << ",\n";
    output << "  \"grants_ul\": " << metrics.grantsUl << ",\n";
    output << "  \"dl_offered_packets\": " << metrics.dlOfferedPackets << ",\n";
    output << "  \"ul_offered_packets\": " << metrics.ulOfferedPackets << ",\n";
    output << "  \"prb_util\": " << metrics.prbUtil << ",\n";
    output << "  \"bler\": " << metrics.bler << ",\n";
    output << "  \"ul_bler\": " << metrics.ulBler << ",\n";
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
    writeNumberArray("prb_util_per_cell", metrics.prbUtilPerCell);
    output << ",\n";
    writeNumberArray("dl_bler_per_cell", metrics.dlBlerPerCell);
    output << ",\n";
    writeNumberArray("ul_bler_per_cell", metrics.ulBlerPerCell);
    output << ",\n";
    writeNumberArray("dl_packets_per_cell", metrics.dlPacketsPerCell);
    output << ",\n";
    writeNumberArray("ul_packets_per_cell", metrics.ulPacketsPerCell);
    output << ",\n";
    output << "  \"grant_trace\": [";
    for (size_t idx = 0; idx < metrics.grantTrace.size(); ++idx)
    {
        const auto& grant = metrics.grantTrace[idx];
        if (idx > 0)
        {
            output << ", ";
        }
        output << "{";
        output << "\"cell_id\": " << grant.cellId << ", ";
        output << "\"grant_type\": \"" << EscapeJsonString(grant.grantType) << "\", ";
        output << "\"frame\": " << grant.frame << ", ";
        output << "\"slot\": " << grant.slot << ", ";
        output << "\"start_symbol\": " << grant.startSymbol << ", ";
        output << "\"num_symbols\": " << grant.numSymbols << ", ";
        output << "\"rnti\": " << grant.rnti << ", ";
        output << "\"mcs\": " << grant.mcs << ", ";
        output << "\"tb_size\": " << grant.tbSize << ", ";
        output << "\"rv\": " << grant.rv << ", ";
        output << "\"ndi\": " << grant.ndi << ", ";
        output << "\"bwp_id\": " << grant.bwpId << ", ";
        output << "\"num_rb\": " << grant.numRb << ", ";
        output << "\"cqi\": " << grant.cqi << ", ";
        output << "\"raw_sinr\": " << grant.rawSinr << ", ";
        output << "\"effective_sinr\": " << grant.effectiveSinr << ", ";
        output << "\"tx_type\": \"" << EscapeJsonString(grant.txType) << "\"";
        output << "}";
    }
    output << "]\n";
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
