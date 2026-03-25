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
#include <cmath>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
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

struct NtnTrajectoryTrack
{
    uint32_t cellId{0};
    std::string source{"unknown"};
    std::vector<double> timeSec;
    std::vector<std::array<double, 3>> positionsM;
};

struct BwpEventSpec
{
    std::string eventId;
    std::string eventType;
    double eventTimeSec{0.0};
    uint32_t targetCellId{0};
    uint32_t requestId{0};
    uint32_t targetBwpId{0};
    double targetDlFrequencyHz{0.0};
    double targetUlFrequencyHz{0.0};
    double targetBandwidthHz{0.0};
    bool gateLbPassed{false};
    bool gateDistressPassed{false};
    bool floorApplied{false};
    bool zeroClamped{false};
    bool rateLimited{false};
};

struct BwpTransactionRecord
{
    uint32_t requestId{0};
    uint32_t cellId{0};
    bool overlapGatePassed{false};
    bool gateLbPassed{false};
    bool gateDistressPassed{false};
    bool floorApplied{false};
    bool zeroClamped{false};
    bool rateLimited{false};
    bool success{false};
    std::string failureReason;
    uint32_t oldBwpId{0};
    uint32_t targetBwpId{0};
    double oldDlFrequencyHz{0.0};
    double oldUlFrequencyHz{0.0};
    double oldBandwidthHz{0.0};
    double targetDlFrequencyHz{0.0};
    double targetUlFrequencyHz{0.0};
    double targetBandwidthHz{0.0};
    double requestTimeSec{0.0};
    double scheduledApplyTimeSec{std::numeric_limits<double>::quiet_NaN()};
    double effectiveApplyTimeSec{std::numeric_limits<double>::quiet_NaN()};
    double ackTimeSec{std::numeric_limits<double>::quiet_NaN()};
    double ackTimeoutSec{0.0};
    bool hoBlockedInWindow{false};
};

struct HoEventSpec
{
    double eventTimeSec{0.0};
    std::vector<uint32_t> targetCellIds;
    std::vector<uint32_t> targetUeIds;
};

struct ScenarioPlan
{
    struct LocalHoDiag
    {
        uint32_t evalCount{0};
        uint32_t passCount{0};
        uint32_t tnToNtnEvalCount{0};
        uint32_t tnToNtnServingFailCount{0};
        uint32_t tnToNtnNeighborPassCount{0};
        uint32_t tnToNtnPassCount{0};
        uint32_t tnToTnA3EvalCount{0};
        uint32_t tnToTnA3PassCount{0};
        uint32_t ntnToTnEvalCount{0};
        uint32_t ntnToTnServingFailCount{0};
        uint32_t ntnToTnNeighborPassCount{0};
        uint32_t ntnToTnPassCount{0};
        double tnToNtnCurrRsrqSum{0.0};
        double tnToNtnCandRsrqSum{0.0};
        double tnToNtnCurrRsrpSum{0.0};
        double tnToNtnCandRsrpSum{0.0};
        double tnToNtnCurrRsrqMin{std::numeric_limits<double>::infinity()};
        double tnToNtnCurrRsrqMax{-std::numeric_limits<double>::infinity()};
        double tnToNtnCandRsrqMin{std::numeric_limits<double>::infinity()};
        double tnToNtnCandRsrqMax{-std::numeric_limits<double>::infinity()};
        double ntnToTnCurrRsrqSum{0.0};
        double ntnToTnCandRsrqSum{0.0};
        double ntnToTnCurrRsrpSum{0.0};
        double ntnToTnCandRsrpSum{0.0};
        double ntnToTnCurrRsrqMin{std::numeric_limits<double>::infinity()};
        double ntnToTnCurrRsrqMax{-std::numeric_limits<double>::infinity()};
        double ntnToTnCandRsrqMin{std::numeric_limits<double>::infinity()};
        double ntnToTnCandRsrqMax{-std::numeric_limits<double>::infinity()};
    };

    std::string operation{"unknown"};
    double runDurationSec{0.0};
    uint32_t numCells{0};
    std::vector<NodeSpec> cells;
    std::vector<UeSpec> ues;
    std::vector<uint32_t> requestedRntis;
    std::string topologySourcePath;
    std::string channelSourcePath;
    std::string controlEventsSourcePath;
    std::string ntnTrajectorySourcePath;
    RunnerControlDelayConfig delayConfig;
    std::string ntnTaCapabilityMode{"all_supported"};
    uint32_t ntnTaSupportedUeCount{0};
    uint32_t ntnTaUnsupportedUeCount{0};
    uint32_t controlEventCount{0};
    uint32_t bwpEventCount{0};
    uint32_t hoEventCount{0};
    uint32_t taEventCount{0};
    bool ntnTrajectoryEnabled{false};
    double ntnTrajectoryUpdatePeriodSec{0.0};
    uint32_t ntnTrajectoryTrackCount{0};
    uint32_t ntnTrajectoryWaypointCount{0};
    std::vector<NtnTrajectoryTrack> ntnTrajectoryTracks;
    std::vector<BwpEventSpec> bwpEvents;
    std::vector<HoEventSpec> hoEvents;
    std::vector<double> hoEventTimes;
    double tnAreaRadiusM{0.0};
    double ntnAreaRadiusAtElevation90DegM{0.0};
    double bwpAckTimeoutMinSec{0.020};
    double bwpAckTimeoutPropagationFactor{4.0};
    double bwpAckTimeoutGuardSec{0.005};
    double rrcControlPeriodSec{0.10};
    bool enableCuDistressBandwidthPolicy{false};
    bool enableNtnBootstrapWhenNoAttachedUe{false};
    bool forceZeroNtnBandwidthWhenNoAttachedUe{false};
    double ntnBootstrapDistressThreshold{0.7};
    double ntnBootstrapMinBandwidthHz{5e6};
    bool enableAdaptiveNtnBootstrapThreshold{false};
    double ntnBootstrapDistressThresholdMin{0.0};
    double ntnBootstrapDistressThresholdMax{1.0};
    double ntnBootstrapDistressThresholdNominal{0.7};
    double ntnBootstrapDistressThresholdStepUp{0.05};
    double ntnBootstrapDistressThresholdStepDown{0.05};
    uint32_t ntnBootstrapAttachHoldCount{1};
    uint32_t ntnBootstrapNoAttachHoldCount{1};
    double maxRequestsPerSecond{10.0};
    double minShareChangeForRequest{0.01};
    bool enableNs3LocalBwpController{true};
    std::string ns3LocalBwpPolicyName{"matlab_legacy"};
    bool enableNs3LocalHoController{false};
    bool enableNs3RuntimeControlLoop{true};
    double ns3RuntimeControlPeriodSec{0.10};
    double subcarrierSpacingHz{30000.0};
    double a3OffsetDb{0.5};
    double a3TttMs{100.0};
    double a3RsrpMinDbm{-125.0};
    double a3RsrqMinDb{-20.0};
    std::string a5TriggerMetricTnToNtn{"rsrq"};
    std::string a5TriggerMetricNtnToTn{"rsrp"};
    double a5ServingRsrqThresholdDbTnToNtn{-12.0};
    double a5NeighborRsrqThresholdDbTnToNtn{-11.0};
    double a5ServingRsrqThresholdDbNtnToTn{-12.0};
    double a5NeighborRsrqThresholdDbNtnToTn{-11.0};
    double a5ServingRsrpThresholdDbmTnToNtn{-75.0};
    double a5NeighborRsrpThresholdDbmTnToNtn{-90.0};
    double a5ServingRsrpThresholdDbmNtnToTn{-95.0};
    double a5NeighborRsrpThresholdDbmNtnToTn{-70.0};
    double a5TttMs{100.0};
    uint32_t localHoMaxRequestsPerCycle{1};
    double localHoMinInterRequestSec{0.005};
    bool allowCrossTypeHoReplay{false};
    std::vector<double> txPowerDbmPerCell;
    std::vector<double> gnbTxAntennaGainDbiPerCell;
    std::vector<double> gnbRxAntennaGainDbiPerCell;
    std::vector<double> gnbTxFeederLossDbPerCell;
    std::vector<double> gnbRxFeederLossDbPerCell;
    std::vector<double> gnbAzimuthDegPerCell;
    std::vector<double> gnbDowntiltDegPerCell;
    std::vector<bool> applyDirectionalPatternCellMask;
    bool enableAntennaPattern{false};
    double ueTxAntennaGainDbi{0.0};
    double ueRxAntennaGainDbi{0.0};
    double ueTxFeederLossDb{0.0};
    double ueRxFeederLossDb{0.0};
    double noiseFigureDb{7.0};
    double horizBeamwidthDeg{65.0};
    double vertBeamwidthDeg{10.0};
    double maxAttenuationDb{30.0};
    double sideLobeAttenuationDb{30.0};
    std::vector<double> cuDistressLevelThresholds;
    std::vector<double> cuNtnBandwidthLevelsHz;
    LocalHoDiag localHoDiag;
};

struct MetricsOutput
{
    struct HoTraceEntry
    {
        std::string eventType;
        double timeSec{0.0};
        uint64_t imsi{0};
        uint16_t cellId{0};
        uint16_t rnti{0};
        uint16_t targetCellId{0};
    };

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
    uint32_t debugUeHoStartCount{0};
    uint32_t debugUeHoEndOkCount{0};
    uint32_t debugUeHoEndErrorCount{0};
    uint32_t debugUeRlfCount{0};
    uint32_t debugUeConnectionTimeoutCount{0};
    uint32_t debugHoRequestSubmittedCount{0};
    uint32_t debugHoRequestRejectedPrecheckCount{0};
    uint32_t debugHoRequestRejectUnknownUeCount{0};
    uint32_t debugHoRequestRejectMissingSourceCount{0};
    uint32_t debugHoRequestRejectSameCellCount{0};
    uint32_t debugHoRequestRejectMissingCellTypeCount{0};
    uint32_t debugHoRequestRejectCrossTypeBlockedCount{0};
    uint32_t debugHoRequestRejectMissingGnbCount{0};
    uint32_t debugHoRequestDispatchCount{0};
    uint32_t debugHoRequestAcceptedCount{0};
    uint32_t debugHoRequestRejectedCount{0};
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
    bool ntnTrajectoryEnabled{false};
    double ntnTrajectoryUpdatePeriodSec{0.0};
    uint32_t ntnTrajectoryTrackCount{0};
    uint32_t ntnTrajectoryWaypointCount{0};
    ScenarioPlan::LocalHoDiag localHoDiag;
    std::string dbgA5TriggerMetricTnToNtn;
    std::string dbgA5TriggerMetricNtnToTn;
    double dbgA5ServingRsrqThresholdDbTnToNtn{std::numeric_limits<double>::quiet_NaN()};
    double dbgA5NeighborRsrqThresholdDbTnToNtn{std::numeric_limits<double>::quiet_NaN()};
    double dbgA5ServingRsrqThresholdDbNtnToTn{std::numeric_limits<double>::quiet_NaN()};
    double dbgA5NeighborRsrqThresholdDbNtnToTn{std::numeric_limits<double>::quiet_NaN()};
    double dbgA5ServingRsrpThresholdDbmTnToNtn{std::numeric_limits<double>::quiet_NaN()};
    double dbgA5NeighborRsrpThresholdDbmTnToNtn{std::numeric_limits<double>::quiet_NaN()};
    double dbgA5ServingRsrpThresholdDbmNtnToTn{std::numeric_limits<double>::quiet_NaN()};
    double dbgA5NeighborRsrpThresholdDbmNtnToTn{std::numeric_limits<double>::quiet_NaN()};
    uint32_t bwpTransactionCount{0};
    uint32_t bwpSuccessCount{0};
    uint32_t bwpFailureCount{0};
    uint32_t bwpHoBlockedCount{0};
    double localBwpBootstrapThresholdCurrent{std::numeric_limits<double>::quiet_NaN()};
    uint32_t localBwpBootstrapRaiseCount{0};
    uint32_t localBwpBootstrapSuccCount{0};
    uint32_t localBwpBootstrapFailCount{0};
    uint32_t localBwpBootstrapIdleCount{0};
    uint32_t localBwpBootstrapPoorPersistCount{0};
    double localBwpBootstrapPoorFractionProxy{0.0};
    bool localBwpBootstrapPoorGate{false};
    std::string localBwpPolicyName{"matlab_legacy"};
    std::vector<BwpTransactionRecord> bwpTransactions;
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
    std::vector<double> servingCellIdPerUe;
    std::vector<double> servingRsrpDbmPerUe;
    std::vector<double> servingRsrqDbPerUe;
    std::vector<double> servingSinrDbPerUe;
    std::vector<double> servingRsrpDbmPerUeApprox;
    std::vector<double> servingRsrqDbPerUeApprox;
    std::string localHoDecisionBasis{"approx_rsrp_rsrq_pre_run"};
    std::string localBwpDecisionBasis{"approx_distress_pre_run"};
    std::vector<GrantTraceEntry> grantTrace;
    std::vector<HoTraceEntry> hoTrace;
};

struct MeasurementContext
{
    std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double> sinrDbByCellRntiBwp;
    std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double> rsrpDbmByCellRntiBwp;
    std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double> rsrqDbByCellRntiBwp;
};

double
LookupServingSinrDb(const std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>& sinrDbByCellRntiBwp,
                    uint16_t servingCellId,
                    uint16_t rnti)
{
    if (servingCellId == 0 || rnti == 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }

    bool found = false;
    double selectedSinr = std::numeric_limits<double>::quiet_NaN();
    uint16_t selectedBwpId = 0;
    for (const auto& [key, value] : sinrDbByCellRntiBwp)
    {
        if (std::get<0>(key) != servingCellId || std::get<1>(key) != rnti)
        {
            continue;
        }
        const uint16_t bwpId = std::get<2>(key);
        if (!found || bwpId > selectedBwpId)
        {
            selectedSinr = value;
            selectedBwpId = bwpId;
            found = true;
        }
    }
    return selectedSinr;
}

double
LookupServingRsrpDbm(const std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>& rsrpDbmByCellRntiBwp,
                     uint16_t servingCellId,
                     uint16_t rnti)
{
    if (servingCellId == 0 || rnti == 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    bool found = false;
    double selectedRsrp = std::numeric_limits<double>::quiet_NaN();
    uint16_t selectedBwpId = 0;
    for (const auto& [key, value] : rsrpDbmByCellRntiBwp)
    {
        if (std::get<0>(key) != servingCellId || std::get<1>(key) != rnti)
        {
            continue;
        }
        const uint16_t bwpId = std::get<2>(key);
        if (!found || bwpId > selectedBwpId)
        {
            selectedRsrp = value;
            selectedBwpId = bwpId;
            found = true;
        }
    }
    return selectedRsrp;
}

double
LookupCellMetricDb(const std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>& metricByCellRntiBwp,
                   uint16_t cellId,
                   uint16_t rnti)
{
    if (cellId == 0 || rnti == 0)
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    bool found = false;
    double selected = std::numeric_limits<double>::quiet_NaN();
    uint16_t selectedBwpId = 0;
    for (const auto& [key, value] : metricByCellRntiBwp)
    {
        if (std::get<0>(key) != cellId || std::get<1>(key) != rnti)
        {
            continue;
        }
        const uint16_t bwpId = std::get<2>(key);
        if (!found || bwpId > selectedBwpId)
        {
            selected = value;
            selectedBwpId = bwpId;
            found = true;
        }
    }
    return selected;
}

bool EvaluateOverlapGatePass(const ScenarioPlan& plan, uint32_t targetCellId);
std::vector<BwpEventSpec> GenerateLocalBwpEvents(const ScenarioPlan& plan,
                                                 const MeasurementContext* measCtx = nullptr);
void ApplyLocalBwpEventsToPlanCells(ScenarioPlan& plan, const std::vector<BwpEventSpec>& events);
struct UeCellQuality
{
    double rsrpDbm{std::numeric_limits<double>::quiet_NaN()};
    double rsrqDb{std::numeric_limits<double>::quiet_NaN()};
    double sinrDb{std::numeric_limits<double>::quiet_NaN()};
    bool measuredRsrp{false};
    bool measuredRsrq{false};
    bool measuredSinr{false};
};
UeCellQuality ResolveUeCellQuality(const ScenarioPlan& plan,
                                   const UeSpec& ue,
                                   const NodeSpec& cell,
                                   const std::vector<NodeSpec>& activeCells,
                                   const MeasurementContext* measCtx);

double
ComputeSlotDurationSec(double subcarrierSpacingHz)
{
    if (!(subcarrierSpacingHz > 0.0) || !std::isfinite(subcarrierSpacingHz))
    {
        return 1e-3; // default 15kHz-equivalent
    }
    const double muReal = std::log2(subcarrierSpacingHz / 15000.0);
    const double muRounded = std::round(muReal);
    const bool validMu = std::isfinite(muRounded) && std::abs(muReal - muRounded) < 1e-6;
    const double mu = validMu ? std::max(0.0, muRounded) : 1.0; // fallback to 30kHz (0.5ms)
    return 1e-3 / std::pow(2.0, mu);
}

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
RecordServingRsrp(std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>* rsrpDbmByCellRntiBwp,
                  uint16_t cellId,
                  [[maybe_unused]] uint16_t unusedId,
                  uint16_t rnti,
                  double rsrpDbm,
                  uint8_t bwpId)
{
    if (rsrpDbmByCellRntiBwp == nullptr)
    {
        return;
    }
    (*rsrpDbmByCellRntiBwp)[std::make_tuple(cellId, rnti, static_cast<uint16_t>(bwpId))] = rsrpDbm;
}

void
RecordUeMeasurements(std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>* rsrpDbmByCellRntiBwp,
                     std::map<std::tuple<uint16_t, uint16_t, uint16_t>, double>* rsrqDbByCellRntiBwp,
                     uint16_t rnti,
                     uint16_t cellId,
                     double rsrp,
                     double rsrq,
                     [[maybe_unused]] bool isServingCell,
                     uint8_t componentCarrierId)
{
    if (rsrpDbmByCellRntiBwp)
    {
        (*rsrpDbmByCellRntiBwp)[std::make_tuple(cellId, rnti, static_cast<uint16_t>(componentCarrierId))] = rsrp;
    }
    if (rsrqDbByCellRntiBwp)
    {
        (*rsrqDbByCellRntiBwp)[std::make_tuple(cellId, rnti, static_cast<uint16_t>(componentCarrierId))] = rsrq;
    }
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
RecordUeHoStart(std::vector<MetricsOutput::HoTraceEntry>* trace,
                uint32_t* counter,
                uint32_t* acceptedCounter,
                uint64_t imsi,
                uint16_t cellId,
                uint16_t rnti,
                uint16_t targetCellId)
{
    if (counter)
    {
        *counter += 1;
    }
    if (trace)
    {
        MetricsOutput::HoTraceEntry entry;
        entry.eventType = "HandoverStart";
        entry.timeSec = Simulator::Now().GetSeconds();
        entry.imsi = imsi;
        entry.cellId = cellId;
        entry.rnti = rnti;
        entry.targetCellId = targetCellId;
        trace->push_back(entry);
    }
    if (acceptedCounter)
    {
        *acceptedCounter += 1;
    }
    NS_LOG_UNCOND("[NS3LocalHoReq] accepted_rrc imsi=" << imsi << " rnti=" << rnti
                  << " srcCell=" << cellId << " dstCell=" << targetCellId
                  << " t=" << Simulator::Now().GetSeconds());
}

void
RecordUeHoEndOk(std::vector<MetricsOutput::HoTraceEntry>* trace,
                uint32_t* counter,
                uint64_t imsi,
                uint16_t cellId,
                uint16_t rnti)
{
    if (counter)
    {
        *counter += 1;
    }
    if (trace)
    {
        MetricsOutput::HoTraceEntry entry;
        entry.eventType = "HandoverEndOk";
        entry.timeSec = Simulator::Now().GetSeconds();
        entry.imsi = imsi;
        entry.cellId = cellId;
        entry.rnti = rnti;
        trace->push_back(entry);
    }
}

void
RecordUeHoEndError(std::vector<MetricsOutput::HoTraceEntry>* trace,
                   uint32_t* counter,
                   uint32_t* rejectedCounter,
                   uint64_t imsi,
                   uint16_t cellId,
                   uint16_t rnti)
{
    if (counter)
    {
        *counter += 1;
    }
    if (trace)
    {
        MetricsOutput::HoTraceEntry entry;
        entry.eventType = "HandoverEndError";
        entry.timeSec = Simulator::Now().GetSeconds();
        entry.imsi = imsi;
        entry.cellId = cellId;
        entry.rnti = rnti;
        trace->push_back(entry);
    }
    if (rejectedCounter)
    {
        *rejectedCounter += 1;
    }
    NS_LOG_UNCOND("[NS3LocalHoReq] rejected_rrc reason=handover_end_error imsi=" << imsi
                  << " rnti=" << rnti << " cell=" << cellId
                  << " t=" << Simulator::Now().GetSeconds());
}

void
RecordUeRlf(std::vector<MetricsOutput::HoTraceEntry>* trace,
            uint32_t* counter,
            uint32_t* rejectedCounter,
            uint64_t imsi,
            uint16_t cellId,
            uint16_t rnti)
{
    if (counter)
    {
        *counter += 1;
    }
    if (trace)
    {
        MetricsOutput::HoTraceEntry entry;
        entry.eventType = "RadioLinkFailure";
        entry.timeSec = Simulator::Now().GetSeconds();
        entry.imsi = imsi;
        entry.cellId = cellId;
        entry.rnti = rnti;
        trace->push_back(entry);
    }
    if (rejectedCounter)
    {
        *rejectedCounter += 1;
    }
    NS_LOG_UNCOND("[NS3LocalHoReq] rejected_rrc reason=radio_link_failure imsi=" << imsi
                  << " rnti=" << rnti << " cell=" << cellId
                  << " t=" << Simulator::Now().GetSeconds());
}

void
RecordUeConnectionTimeout(std::vector<MetricsOutput::HoTraceEntry>* trace,
                          uint32_t* counter,
                          uint32_t* rejectedCounter,
                          uint64_t imsi,
                          uint16_t cellId,
                          uint16_t rnti,
                          [[maybe_unused]] uint8_t numAttempts)
{
    if (counter)
    {
        *counter += 1;
    }
    if (trace)
    {
        MetricsOutput::HoTraceEntry entry;
        entry.eventType = "ConnectionTimeout";
        entry.timeSec = Simulator::Now().GetSeconds();
        entry.imsi = imsi;
        entry.cellId = cellId;
        entry.rnti = rnti;
        trace->push_back(entry);
    }
    if (rejectedCounter)
    {
        *rejectedCounter += 1;
    }
    NS_LOG_UNCOND("[NS3LocalHoReq] rejected_rrc reason=connection_timeout imsi=" << imsi
                  << " rnti=" << rnti << " cell=" << cellId
                  << " t=" << Simulator::Now().GetSeconds());
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

std::optional<std::string>
ExtractArrayBodyAfterKey(const std::string& text, const std::string& key)
{
    const auto keyPos = text.find("\"" + key + "\"");
    if (keyPos == std::string::npos)
    {
        return std::nullopt;
    }
    const auto arrayStart = text.find('[', keyPos);
    if (arrayStart == std::string::npos)
    {
        return std::nullopt;
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
        return std::nullopt;
    }
    return text.substr(arrayStart, arrayEnd - arrayStart + 1);
}

std::vector<std::string>
ExtractObjectArrayBodiesAfterKey(const std::string& text, const std::string& key)
{
    std::vector<std::string> bodies;
    auto arrayBody = ExtractArrayBodyAfterKey(text, key);
    if (!arrayBody.has_value())
    {
        return bodies;
    }
    const std::string& body = *arrayBody;
    int depth = 0;
    size_t objectStart = std::string::npos;
    for (size_t i = 0; i < body.size(); ++i)
    {
        if (body[i] == '{')
        {
            if (depth == 0)
            {
                objectStart = i;
            }
            depth++;
        }
        else if (body[i] == '}')
        {
            depth--;
            if (depth == 0 && objectStart != std::string::npos)
            {
                bodies.push_back(body.substr(objectStart, i - objectStart + 1));
                objectStart = std::string::npos;
            }
        }
    }
    return bodies;
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

double
GetMetadataNumber(const std::string& eventBody, const std::string& key, double defaultValue)
{
    if (auto metadataBody = ExtractObjectBodyAfterKey(eventBody, "metadata"))
    {
        if (auto value = ExtractFirstNumberAfterKey(*metadataBody, key))
        {
            return *value;
        }
    }
    if (auto value = ExtractFirstNumberAfterKey(eventBody, key))
    {
        return *value;
    }
    return defaultValue;
}

std::vector<uint32_t>
GetTargetCellIds(const std::string& eventBody)
{
    std::vector<uint32_t> out;
    const auto targetCellIds = ExtractNumberArrayAfterKey(eventBody, "target_cell_ids");
    for (double value : targetCellIds)
    {
        const uint32_t cellId = static_cast<uint32_t>(std::max(0.0, value));
        if (cellId > 0)
        {
            out.push_back(cellId);
        }
    }
    if (!out.empty())
    {
        return out;
    }
    const double singleTarget = GetMetadataNumber(eventBody, "target_cell_id", 0.0);
    if (singleTarget > 0.0)
    {
        out.push_back(static_cast<uint32_t>(singleTarget));
    }
    return out;
}

std::vector<uint32_t>
GetTargetUeIds(const std::string& eventBody)
{
    std::vector<uint32_t> out;
    const auto targetUeIds = ExtractNumberArrayAfterKey(eventBody, "target_ue_ids");
    for (double value : targetUeIds)
    {
        const uint32_t ueId = static_cast<uint32_t>(std::max(0.0, value));
        if (ueId > 0)
        {
            out.push_back(ueId);
        }
    }
    return out;
}

std::vector<BwpEventSpec>
ExtractBwpEventsFromControlSource(const std::string& controlJsonSource)
{
    std::vector<BwpEventSpec> out;
    const auto eventBodies = ExtractObjectArrayBodiesAfterKey(controlJsonSource, "events");
    for (const auto& eventBody : eventBodies)
    {
        const auto eventType = ExtractFirstStringAfterKey(eventBody, "event_type").value_or("");
        const bool isBwpRequestType = (eventType == "BwpReconfigRequested") || (eventType == "bwp_reconfig");
        if (!isBwpRequestType)
        {
            continue;
        }

        const std::vector<uint32_t> targetCellIds = GetTargetCellIds(eventBody);
        if (targetCellIds.empty())
        {
            continue;
        }

        const std::string eventId = ExtractFirstStringAfterKey(eventBody, "event_id").value_or("");
        const double eventTimeSec = ExtractFirstNumberAfterKey(eventBody, "event_time_sec").value_or(0.0);
        const uint32_t requestId = static_cast<uint32_t>(
            std::max(0.0, GetMetadataNumber(eventBody, "request_id", 0.0)));
        const uint32_t targetBwpId = static_cast<uint32_t>(
            std::max(0.0, GetMetadataNumber(eventBody, "target_bwp_id", 0.0)));
        const double targetDlFrequencyHz = GetMetadataNumber(eventBody, "target_dl_frequency_hz", 0.0);
        const double targetUlFrequencyHz = GetMetadataNumber(eventBody, "target_ul_frequency_hz", 0.0);
        const double targetBandwidthHz = GetMetadataNumber(eventBody, "target_bandwidth_hz", 0.0);

        for (uint32_t targetCellId : targetCellIds)
        {
            BwpEventSpec spec;
            spec.eventId = eventId;
            spec.eventType = eventType;
            spec.eventTimeSec = eventTimeSec;
            spec.targetCellId = targetCellId;
            spec.requestId = requestId;
            spec.targetBwpId = targetBwpId;
            spec.targetDlFrequencyHz = targetDlFrequencyHz;
            spec.targetUlFrequencyHz = targetUlFrequencyHz;
            spec.targetBandwidthHz = targetBandwidthHz;
            out.push_back(spec);
        }
    }
    return out;
}

std::vector<HoEventSpec>
ExtractHoEventsFromControlSource(const std::string& controlJsonSource)
{
    std::vector<HoEventSpec> out;
    const auto eventBodies = ExtractObjectArrayBodiesAfterKey(controlJsonSource, "events");
    for (const auto& eventBody : eventBodies)
    {
        const auto eventType = ExtractFirstStringAfterKey(eventBody, "event_type").value_or("");
        const bool isHo = (eventType == "HandoverRequested") || (eventType == "HandoverStarted") ||
                          (eventType == "HandoverCompleted") || (eventType == "handover");
        if (!isHo)
        {
            continue;
        }
        HoEventSpec event;
        event.eventTimeSec = ExtractFirstNumberAfterKey(eventBody, "event_time_sec").value_or(0.0);
        event.targetCellIds = GetTargetCellIds(eventBody);
        event.targetUeIds = GetTargetUeIds(eventBody);
        // Require explicit target cell and target UE for event-driven HO execution.
        if (event.targetCellIds.empty() || event.targetUeIds.empty())
        {
            continue;
        }
        out.push_back(event);
    }
    return out;
}

uint32_t
FindPreferredNtnCellId(const ScenarioPlan& plan)
{
    for (const auto& cell : plan.cells)
    {
        if (cell.cellType == "NTN")
        {
            return cell.id;
        }
    }
    return 0;
}

uint32_t
CountUeAttachedToCellType(const ScenarioPlan& plan, const std::string& cellType)
{
    std::map<uint32_t, std::string> cellTypeById;
    for (const auto& cell : plan.cells)
    {
        cellTypeById[cell.id] = cell.cellType;
    }
    uint32_t count = 0;
    for (const auto& ue : plan.ues)
    {
        auto it = cellTypeById.find(ue.servingCellId);
        if (it != cellTypeById.end() && it->second == cellType)
        {
            ++count;
        }
    }
    return count;
}

struct LocalBootstrapAdaptiveState
{
    bool initialized{false};
    double thresholdCurrent{0.7};
    uint32_t attachSuccessCount{0};
    uint32_t noAttachCount{0};
    uint32_t raiseCount{0};
    uint32_t succCount{0};
    uint32_t failCount{0};
    uint32_t idleCount{0};
    uint32_t poorPersistCount{0};
    double poorFractionProxy{0.0};
    bool poorGate{false};
};

struct LocalBwpDecision
{
    bool overlapPass{false};
    bool gateLbPassed{false};
    bool gateDistressPassed{false};
    bool floorApplied{false};
    bool zeroClamped{false};
    bool bootstrapNeeded{false};
    bool distressPass{false};
    bool bootstrapPass{false};
    double distressScore{0.0};
    double targetNtnBwHz{0.0};
    double bootstrapThresholdCurrent{0.0};
};

std::string
NormalizePolicyName(const std::string& name)
{
    std::string out = name;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return std::tolower(c); });
    return out;
}

LocalBootstrapAdaptiveState&
GetLocalBootstrapAdaptiveState()
{
    static LocalBootstrapAdaptiveState s;
    return s;
}

void
ResetLocalBootstrapAdaptiveState()
{
    auto& s = GetLocalBootstrapAdaptiveState();
    s = LocalBootstrapAdaptiveState{};
}

std::vector<BwpEventSpec>
GenerateLocalBwpEvents(const ScenarioPlan& plan, const MeasurementContext* measCtx)
{
    std::vector<BwpEventSpec> out;
    if (!plan.enableNs3LocalBwpController)
    {
        return out;
    }
    if (!(plan.enableCuDistressBandwidthPolicy || plan.enableNtnBootstrapWhenNoAttachedUe))
    {
        return out;
    }
    const uint32_t targetNtnCellId = FindPreferredNtnCellId(plan);
    if (targetNtnCellId == 0)
    {
        return out;
    }
    const uint32_t ntnAttachedCount = CountUeAttachedToCellType(plan, "NTN");

    auto estimateBaseTnPerCellHz = [&plan]() -> double {
        double maxTnBwHz = 0.0;
        for (const auto& cell : plan.cells)
        {
            if (cell.cellType == "TN")
            {
                maxTnBwHz = std::max(maxTnBwHz, cell.bandwidthHz);
            }
        }
        return maxTnBwHz;
    };
    auto estimateSimpleCuDistressScore = [&plan, measCtx]() -> double {
        if (measCtx != nullptr)
        {
            double bad = 0.0;
            double total = 0.0;
            for (const auto& ue : plan.ues)
            {
                const auto servingIt = std::find_if(plan.cells.begin(),
                                                    plan.cells.end(),
                                                    [&ue](const NodeSpec& c) { return c.id == ue.servingCellId; });
                if (servingIt == plan.cells.end() || servingIt->cellType != "TN")
                {
                    continue;
                }
                const auto q = ResolveUeCellQuality(plan, ue, *servingIt, plan.cells, measCtx);
                if (!std::isfinite(q.sinrDb))
                {
                    continue;
                }
                total += 1.0;
                if (q.sinrDb < -5.0)
                {
                    bad += 1.0;
                }
            }
            if (total > 0.0)
            {
                return std::min(1.0, std::max(0.0, bad / total));
            }
        }
        std::map<uint32_t, uint32_t> ueCountPerTnCell;
        for (const auto& cell : plan.cells)
        {
            if (cell.cellType == "TN")
            {
                ueCountPerTnCell[cell.id] = 0;
            }
        }
        for (const auto& ue : plan.ues)
        {
            auto it = ueCountPerTnCell.find(ue.servingCellId);
            if (it != ueCountPerTnCell.end())
            {
                ++it->second;
            }
        }

        double weighted = 0.0;
        double wsum = 0.0;
        for (const auto& [cellId, ueCount] : ueCountPerTnCell)
        {
            (void)cellId;
            // Simple proxy aligned with "low-load -> low distress".
            const double distressCell =
                std::min(1.0, std::max(0.0, (static_cast<double>(ueCount) - 2.0) / 5.0));
            const double w = std::max(1.0, static_cast<double>(ueCount));
            weighted += distressCell * w;
            wsum += w;
        }
        if (wsum <= 0.0)
        {
            return 0.0;
        }
        return std::min(1.0, std::max(0.0, weighted / wsum));
    };
    auto mapDistressToNtnBandwidthHz = [&plan](double distressScore, double baseTnPerCellHz) -> double {
        std::vector<double> thresholds = plan.cuDistressLevelThresholds;
        std::vector<double> levels = plan.cuNtnBandwidthLevelsHz;
        if (levels.empty())
        {
            levels = {0.0, plan.ntnBootstrapMinBandwidthHz};
        }
        if (thresholds.empty() && levels.size() >= 2)
        {
            thresholds.resize(levels.size() - 1, 0.0);
            for (size_t i = 0; i < thresholds.size(); ++i)
            {
                thresholds[i] = 0.40 + (0.95 - 0.40) * (static_cast<double>(i) /
                                                        std::max<size_t>(1, thresholds.size() - 1));
            }
        }

        size_t idx = 0;
        for (double th : thresholds)
        {
            if (distressScore >= th)
            {
                ++idx;
            }
        }
        if (idx >= levels.size())
        {
            idx = levels.size() - 1;
        }
        double bwHz = levels[idx];
        if (baseTnPerCellHz > 0.0)
        {
            bwHz = std::min(std::max(0.0, bwHz), baseTnPerCellHz);
        }
        return bwHz;
    };

    double currentNtnBwHz = 0.0;
    for (const auto& cell : plan.cells)
    {
        if (cell.id == targetNtnCellId)
        {
            currentNtnBwHz = cell.bandwidthHz;
            break;
        }
    }
    const bool overlapPass = EvaluateOverlapGatePass(plan, targetNtnCellId);
    const double baseTnPerCellHz = estimateBaseTnPerCellHz();
    const double minDeltaBwHz = std::max(0.0, plan.minShareChangeForRequest) * std::max(0.0, baseTnPerCellHz);
    const double distressScore = estimateSimpleCuDistressScore();
    LocalBwpDecision decision;
    decision.overlapPass = overlapPass;
    decision.distressScore = distressScore;
    decision.targetNtnBwHz = currentNtnBwHz;
    decision.bootstrapThresholdCurrent = plan.ntnBootstrapDistressThreshold;

    // Adaptive bootstrap threshold state (bridge-local approximation of MATLAB controller).
    auto& adapt = GetLocalBootstrapAdaptiveState();
    if (!adapt.initialized)
    {
        adapt.initialized = true;
        adapt.thresholdCurrent =
            std::min(plan.ntnBootstrapDistressThresholdMax,
                     std::max(plan.ntnBootstrapDistressThresholdMin,
                              plan.ntnBootstrapDistressThresholdNominal));
        adapt.attachSuccessCount = 0;
        adapt.noAttachCount = 0;
    }
    if (!plan.enableAdaptiveNtnBootstrapThreshold)
    {
        adapt.thresholdCurrent = std::min(plan.ntnBootstrapDistressThresholdMax,
                                          std::max(plan.ntnBootstrapDistressThresholdMin,
                                                   plan.ntnBootstrapDistressThreshold));
    }
    const bool hasNtnAttach = (ntnAttachedCount > 0);
    adapt.poorFractionProxy = distressScore;
    adapt.poorGate = (distressScore >= adapt.thresholdCurrent);
    if (hasNtnAttach)
    {
        ++adapt.succCount;
        adapt.poorPersistCount = 0;
    }
    else if (adapt.poorGate)
    {
        ++adapt.failCount;
        ++adapt.poorPersistCount;
    }
    else
    {
        ++adapt.idleCount;
        adapt.poorPersistCount = 0;
    }
    if (plan.enableAdaptiveNtnBootstrapThreshold)
    {
        if (hasNtnAttach)
        {
            adapt.attachSuccessCount += 1;
            adapt.noAttachCount = 0;
        }
        else
        {
            adapt.noAttachCount += 1;
            adapt.attachSuccessCount = 0;
        }
        if (adapt.attachSuccessCount >= std::max<uint32_t>(1, plan.ntnBootstrapAttachHoldCount))
        {
            adapt.thresholdCurrent = std::min(plan.ntnBootstrapDistressThresholdMax,
                                              adapt.thresholdCurrent + std::max(0.0, plan.ntnBootstrapDistressThresholdStepUp));
            adapt.attachSuccessCount = 0;
            ++adapt.raiseCount;
        }
        if (adapt.noAttachCount >= std::max<uint32_t>(1, plan.ntnBootstrapNoAttachHoldCount))
        {
            adapt.thresholdCurrent = std::max(plan.ntnBootstrapDistressThresholdMin,
                                              adapt.thresholdCurrent - std::max(0.0, plan.ntnBootstrapDistressThresholdStepDown));
            adapt.noAttachCount = 0;
        }
    }
    decision.bootstrapPass = distressScore >= adapt.thresholdCurrent;
    decision.bootstrapNeeded =
        plan.enableNtnBootstrapWhenNoAttachedUe &&
        (plan.ntnBootstrapMinBandwidthHz > 0.0) &&
        ((ntnAttachedCount == 0) || (currentNtnBwHz <= 0.0)) &&
        (decision.bootstrapPass || (distressScore >= plan.ntnBootstrapDistressThreshold));
    decision.distressPass = distressScore >= plan.ntnBootstrapDistressThreshold;
    decision.bootstrapThresholdCurrent = adapt.thresholdCurrent;

    const std::string localBwpPolicyName = NormalizePolicyName(plan.ns3LocalBwpPolicyName);
    if (localBwpPolicyName == "fixed_ntn_floor")
    {
        decision.gateLbPassed = overlapPass || decision.bootstrapNeeded;
        decision.gateDistressPassed = true;
        if (decision.bootstrapNeeded)
        {
            decision.floorApplied = true;
            decision.targetNtnBwHz = std::max(currentNtnBwHz, plan.ntnBootstrapMinBandwidthHz);
        }
    }
    else
    {
        // matlab_legacy policy (default):
        //   LB gate -> distress mapping gate -> bootstrap floor -> zero clamp.
        // FloorPolicy exception: if bootstrap is needed, allow bypassing overlap gate.
        decision.gateLbPassed = overlapPass || decision.bootstrapNeeded;
        decision.gateDistressPassed = (!plan.enableCuDistressBandwidthPolicy) || decision.distressPass;
        if (plan.enableCuDistressBandwidthPolicy && decision.gateDistressPassed)
        {
            decision.targetNtnBwHz = mapDistressToNtnBandwidthHz(distressScore, baseTnPerCellHz);
        }
        if (decision.bootstrapNeeded)
        {
            decision.floorApplied = true;
            decision.targetNtnBwHz = std::max(decision.targetNtnBwHz, plan.ntnBootstrapMinBandwidthHz);
        }
    }
    if (plan.forceZeroNtnBandwidthWhenNoAttachedUe && ntnAttachedCount == 0)
    {
        decision.zeroClamped = true;
        decision.targetNtnBwHz = 0.0;
    }
    if (!decision.gateLbPassed)
    {
        return out;
    }
    NS_LOG_UNCOND("[NS3LocalBwpDiag] targetNtnCellId=" << targetNtnCellId
                                                        << " policy=" << localBwpPolicyName
                                                        << " ntnAttached=" << ntnAttachedCount
                                                        << " currentNtnBwHz=" << currentNtnBwHz
                                                        << " bootstrapEnabled="
                                                        << (plan.enableNtnBootstrapWhenNoAttachedUe ? 1 : 0)
                                                        << " adaptiveBootstrapEnabled="
                                                        << (plan.enableAdaptiveNtnBootstrapThreshold ? 1 : 0)
                                                        << " distressScore=" << distressScore
                                                        << " distressPass=" << (decision.distressPass ? 1 : 0)
                                                        << " gateDistressPassed=" << (decision.gateDistressPassed ? 1 : 0)
                                                        << " bootstrapThrCurrent=" << decision.bootstrapThresholdCurrent
                                                        << " bootstrapPass=" << (decision.bootstrapPass ? 1 : 0)
                                                        << " bootstrapMinHz=" << plan.ntnBootstrapMinBandwidthHz
                                                        << " bootstrapNeeded=" << (decision.bootstrapNeeded ? 1 : 0)
                                                        << " overlapPass=" << (overlapPass ? 1 : 0)
                                                        << " gateLbPassed=" << (decision.gateLbPassed ? 1 : 0)
                                                        << " floorApplied=" << (decision.floorApplied ? 1 : 0)
                                                        << " zeroClamped=" << (decision.zeroClamped ? 1 : 0)
                                                        << " targetNtnBwHz=" << decision.targetNtnBwHz
                                                        << " minDeltaBwHz=" << minDeltaBwHz);

    // Keep total allocated bandwidth conserved by redistributing TN budget when
    // NTN target changes. This keeps spectrum accounting stable in NS-3 local mode.
    double totalBwHz = 0.0;
    double totalTnBwHz = 0.0;
    std::vector<uint32_t> tnCellIds;
    std::vector<double> tnCurrentBwHz;
    for (const auto& cell : plan.cells)
    {
        totalBwHz += std::max(0.0, cell.bandwidthHz);
        if (cell.cellType == "TN")
        {
            tnCellIds.push_back(cell.id);
            tnCurrentBwHz.push_back(std::max(0.0, cell.bandwidthHz));
            totalTnBwHz += std::max(0.0, cell.bandwidthHz);
        }
    }
    double targetNtnBwHz = std::max(0.0, std::min(decision.targetNtnBwHz, totalBwHz));
    const double targetTotalTnBwHz = std::max(0.0, totalBwHz - targetNtnBwHz);

    std::unordered_map<uint32_t, double> targetBwByCellId;
    targetBwByCellId[targetNtnCellId] = targetNtnBwHz;
    if (!tnCellIds.empty())
    {
        if (totalTnBwHz > 0.0)
        {
            for (size_t i = 0; i < tnCellIds.size(); ++i)
            {
                const double w = tnCurrentBwHz[i] / totalTnBwHz;
                targetBwByCellId[tnCellIds[i]] = targetTotalTnBwHz * w;
            }
        }
        else
        {
            const double equalBw = targetTotalTnBwHz / static_cast<double>(tnCellIds.size());
            for (uint32_t cellId : tnCellIds)
            {
                targetBwByCellId[cellId] = equalBw;
            }
        }
    }

    const double evalPeriodSec = std::max(plan.rrcControlPeriodSec, 1e-3);
    const double rateLimitGapSec = (plan.maxRequestsPerSecond > 0.0)
                                       ? (1.0 / plan.maxRequestsPerSecond)
                                       : std::numeric_limits<double>::infinity();
    const double firstTimeSec = evalPeriodSec;
    const bool canSchedule = std::isfinite(firstTimeSec) && std::isfinite(rateLimitGapSec);
    if (!canSchedule)
    {
        return out;
    }

    uint32_t reqId = 0;
    uint32_t scheduledOrdinal = 0;
    for (const auto& cell : plan.cells)
    {
        auto targetIt = targetBwByCellId.find(cell.id);
        if (targetIt == targetBwByCellId.end())
        {
            continue;
        }
        const double targetHz = std::max(0.0, targetIt->second);
        if (std::abs(targetHz - cell.bandwidthHz) < minDeltaBwHz)
        {
            continue;
        }
        ++reqId;
        ++scheduledOrdinal;
        BwpEventSpec spec;
        spec.eventId = "ns3_local_bwp_" + std::to_string(reqId);
        spec.eventType = "BwpReconfigRequested";
        const bool rateLimited = (scheduledOrdinal > 1) && std::isfinite(rateLimitGapSec) && (rateLimitGapSec > 0.0);
        spec.eventTimeSec = firstTimeSec + static_cast<double>(scheduledOrdinal - 1) * rateLimitGapSec;
        spec.targetCellId = cell.id;
        spec.requestId = reqId;
        spec.targetBwpId = 0;
        spec.targetBandwidthHz = targetHz;
        spec.targetDlFrequencyHz = cell.carrierFrequencyHz;
        spec.targetUlFrequencyHz = cell.carrierFrequencyHz;
        spec.gateLbPassed = decision.gateLbPassed;
        spec.gateDistressPassed = decision.gateDistressPassed;
        spec.floorApplied = decision.floorApplied;
        spec.zeroClamped = decision.zeroClamped;
        spec.rateLimited = rateLimited;
        out.push_back(spec);
    }
    NS_LOG_UNCOND("[NS3LocalBwpDiag] generatedLocalBwpEvents=" << out.size()
                  << " targetNtnBwHz=" << targetNtnBwHz
                  << " targetTotalTnBwHz=" << targetTotalTnBwHz);
    return out;
}

void
ApplyLocalBwpEventsToPlanCells(ScenarioPlan& plan, const std::vector<BwpEventSpec>& events)
{
    if (events.empty())
    {
        return;
    }
    for (const auto& event : events)
    {
        if (event.targetCellId == 0 || event.targetBandwidthHz < 0.0)
        {
            continue;
        }
        for (auto& cell : plan.cells)
        {
            if (cell.id == event.targetCellId)
            {
                cell.bandwidthHz = event.targetBandwidthHz;
                break;
            }
        }
    }
}

[[maybe_unused]] std::vector<double>
GenerateLocalHoEventTimes(const ScenarioPlan& plan)
{
    std::vector<double> out;
    if (!plan.enableNs3LocalHoController)
    {
        return out;
    }
    const uint32_t targetNtnCellId = FindPreferredNtnCellId(plan);
    if (targetNtnCellId == 0)
    {
        return out;
    }
    // Do not generate HO timing unless NTN target is actually active.
    bool targetNtnActive = false;
    for (const auto& cell : plan.cells)
    {
        if (cell.id == targetNtnCellId)
        {
            targetNtnActive = (cell.bandwidthHz > 0.0);
            break;
        }
    }
    if (!targetNtnActive)
    {
        return out;
    }
    if (!EvaluateOverlapGatePass(plan, targetNtnCellId))
    {
        return out;
    }
    uint32_t attachedTn = 0;
    for (const auto& ue : plan.ues)
    {
        for (const auto& cell : plan.cells)
        {
            if (cell.id == ue.servingCellId && cell.cellType == "TN")
            {
                ++attachedTn;
                break;
            }
        }
    }
    if (attachedTn == 0)
    {
        return out;
    }
    const double tSec = std::max(plan.rrcControlPeriodSec, 1e-3);
    if (std::isfinite(tSec))
    {
        out.push_back(tSec);
    }
    return out;
}

double
ResolvePerCellOrDefault(const std::vector<double>& values, uint32_t cellId, double defaultValue)
{
    if (cellId > 0 && (cellId - 1) < values.size() && std::isfinite(values[cellId - 1]))
    {
        return values[cellId - 1];
    }
    return defaultValue;
}

double
ComputeDirectionalPatternLossDb(const ScenarioPlan& plan, const UeSpec& ue, const NodeSpec& cell)
{
    if (!plan.enableAntennaPattern)
    {
        return 0.0;
    }
    bool applyMask = true;
    if (cell.id > 0 && (cell.id - 1) < plan.applyDirectionalPatternCellMask.size())
    {
        applyMask = plan.applyDirectionalPatternCellMask[cell.id - 1];
    }
    if (!applyMask)
    {
        return 0.0;
    }

    const double azBs = ResolvePerCellOrDefault(plan.gnbAzimuthDegPerCell, cell.id, 0.0);
    const double tiltBs = ResolvePerCellOrDefault(plan.gnbDowntiltDegPerCell, cell.id, 0.0);
    const double dx = ue.x - cell.x;
    const double dy = ue.y - cell.y;
    const double dz = ue.z - cell.z;
    const double horizDist = std::max(1e-6, std::sqrt(dx * dx + dy * dy));
    const double azUe = std::atan2(dy, dx) * 180.0 / M_PI;
    const double elUe = std::atan2(dz, horizDist) * 180.0 / M_PI;

    auto wrap180 = [](double deg) {
        double out = std::fmod(deg + 180.0, 360.0);
        if (out < 0.0)
        {
            out += 360.0;
        }
        return out - 180.0;
    };
    const double azOffset = wrap180(azUe - azBs);
    const double elBoresight = -tiltBs;
    const double elOffset = wrap180(elUe - elBoresight);

    const double hBw = std::max(1e-3, plan.horizBeamwidthDeg);
    const double vBw = std::max(1e-3, plan.vertBeamwidthDeg);
    const double aMax = std::max(0.0, plan.maxAttenuationDb);
    const double sla = std::max(0.0, plan.sideLobeAttenuationDb);
    const double ah = std::min(12.0 * std::pow(azOffset / hBw, 2.0), aMax);
    const double av = std::min(12.0 * std::pow(elOffset / vBw, 2.0), sla);
    return std::min(ah + av, aMax);
}

double
EstimateRsrpDbm(const ScenarioPlan& plan, const UeSpec& ue, const NodeSpec& cell)
{
    const double dx = ue.x - cell.x;
    const double dy = ue.y - cell.y;
    const double dz = ue.z - cell.z;
    const double distM = std::max(1.0, std::sqrt(dx * dx + dy * dy + dz * dz));
    const double freqHz = std::max(1.0, cell.carrierFrequencyHz);
    const double fsplDb = 20.0 * std::log10(distM) + 20.0 * std::log10(freqHz) - 147.55;

    const double txPowerDbm = ResolvePerCellOrDefault(plan.txPowerDbmPerCell, cell.id, 30.0);
    const double gnbTxGainDbi = ResolvePerCellOrDefault(plan.gnbTxAntennaGainDbiPerCell, cell.id, 0.0);
    const double gnbTxFeederLossDb = ResolvePerCellOrDefault(plan.gnbTxFeederLossDbPerCell, cell.id, 0.0);
    const double ueRxGainDbi = plan.ueRxAntennaGainDbi;
    const double ueRxFeederLossDb = plan.ueRxFeederLossDb;
    const double dirLossDb = ComputeDirectionalPatternLossDb(plan, ue, cell);
    return txPowerDbm + gnbTxGainDbi - gnbTxFeederLossDb + ueRxGainDbi - ueRxFeederLossDb - fsplDb - dirLossDb;
}

double
EstimateRsrqDb(const ScenarioPlan& plan,
               const UeSpec& ue,
               const NodeSpec& servingCell,
               const std::vector<NodeSpec>& activeCells)
{
    if (servingCell.bandwidthHz <= 0.0)
    {
        // Zero-bandwidth cells are not schedulable; treat quality as invalid.
        return -150.0;
    }
    auto computeBandwidthOverlapHz = [](const NodeSpec& a, const NodeSpec& b) -> double {
        const double bwA = std::max(0.0, a.bandwidthHz);
        const double bwB = std::max(0.0, b.bandwidthHz);
        if (bwA <= 0.0 || bwB <= 0.0)
        {
            return 0.0;
        }
        const double aLo = a.carrierFrequencyHz - 0.5 * bwA;
        const double aHi = a.carrierFrequencyHz + 0.5 * bwA;
        const double bLo = b.carrierFrequencyHz - 0.5 * bwB;
        const double bHi = b.carrierFrequencyHz + 0.5 * bwB;
        return std::max(0.0, std::min(aHi, bHi) - std::max(aLo, bLo));
    };

    const double serving = EstimateRsrpDbm(plan, ue, servingCell);
    double interfLin = 0.0;
    for (const auto& cell : activeCells)
    {
        if (cell.id == servingCell.id)
        {
            continue;
        }
        const double overlapHz = computeBandwidthOverlapHz(servingCell, cell);
        if (overlapHz <= 0.0)
        {
            // In steady-state TN/NTN split, non-overlapping cells do not interfere.
            continue;
        }
        const double overlapRatio = overlapHz / std::max(1.0, servingCell.bandwidthHz);
        const double rsrpDbm = EstimateRsrpDbm(plan, ue, cell);
        interfLin += std::pow(10.0, rsrpDbm / 10.0) * std::max(0.0, std::min(1.0, overlapRatio));
    }
    const double servingLin = std::pow(10.0, serving / 10.0);
    const double noiseDbm =
        -174.0 + 10.0 * std::log10(std::max(1.0, servingCell.bandwidthHz)) + plan.noiseFigureDb;
    const double noiseLin = std::pow(10.0, noiseDbm / 10.0);
    const double ratio = servingLin / std::max(noiseLin + interfLin, std::pow(10.0, -150.0 / 10.0));
    return 10.0 * std::log10(std::max(ratio, std::pow(10.0, -15.0)));
}

double
ComputeNrbFromBandwidthAndScs(const NodeSpec& cell, double subcarrierSpacingHz)
{
    if (!(cell.bandwidthHz > 0.0) || !(subcarrierSpacingHz > 0.0))
    {
        return 1.0;
    }
    // Approximation: NRB ~= BW / (12 * SCS)
    return std::max(1.0, std::round(cell.bandwidthHz / (12.0 * subcarrierSpacingHz)));
}

double
DeriveRsrqFromRsrpAndSinrDb(double rsrpDbm, double sinrDb, const NodeSpec& cell, double subcarrierSpacingHz)
{
    if (!std::isfinite(rsrpDbm) || !std::isfinite(sinrDb))
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    const double nrb = ComputeNrbFromBandwidthAndScs(cell, subcarrierSpacingHz);
    const double sinrLin = std::pow(10.0, sinrDb / 10.0);
    if (!(sinrLin > 0.0))
    {
        return std::numeric_limits<double>::quiet_NaN();
    }
    // RSRQ = N*RSRP / RSSI, RSSI ~= N*RSRP + (I+N), SINR = RSRP/(I+N)
    // => RSRQ_lin = N / (N + 1/SINR)
    const double rsrqLin = nrb / (nrb + (1.0 / sinrLin));
    return 10.0 * std::log10(std::max(rsrqLin, 1e-15));
}

UeCellQuality
ResolveUeCellQuality(const ScenarioPlan& plan,
                     const UeSpec& ue,
                     const NodeSpec& cell,
                     const std::vector<NodeSpec>& activeCells,
                     const MeasurementContext* measCtx)
{
    UeCellQuality q;
    q.rsrpDbm = EstimateRsrpDbm(plan, ue, cell);
    q.rsrqDb = EstimateRsrqDb(plan, ue, cell, activeCells);
    q.sinrDb = q.rsrpDbm - q.rsrqDb;
    if (measCtx == nullptr)
    {
        return q;
    }

    const size_t ueIdx = (ue.id >= 1) ? (ue.id - 1) : static_cast<size_t>(-1);
    uint16_t rnti = 0;
    if (ueIdx < plan.requestedRntis.size())
    {
        const uint32_t requestedRnti = plan.requestedRntis[ueIdx];
        if (requestedRnti > 0 && requestedRnti <= std::numeric_limits<uint16_t>::max())
        {
            rnti = static_cast<uint16_t>(requestedRnti);
        }
    }
    const uint16_t cellId = static_cast<uint16_t>(cell.id);
    const double measuredRsrp = LookupCellMetricDb(measCtx->rsrpDbmByCellRntiBwp, cellId, rnti);
    const double measuredRsrq = LookupCellMetricDb(measCtx->rsrqDbByCellRntiBwp, cellId, rnti);
    const double measuredSinr = LookupCellMetricDb(measCtx->sinrDbByCellRntiBwp, cellId, rnti);

    if (std::isfinite(measuredRsrp))
    {
        q.rsrpDbm = measuredRsrp;
        q.measuredRsrp = true;
    }
    if (std::isfinite(measuredSinr))
    {
        q.sinrDb = measuredSinr;
        q.measuredSinr = true;
    }
    if (std::isfinite(measuredRsrq) && std::abs(measuredRsrq) > 1e-9)
    {
        q.rsrqDb = measuredRsrq;
        q.measuredRsrq = true;
    }
    else if (std::isfinite(q.rsrpDbm) && std::isfinite(q.sinrDb))
    {
        const double derived = DeriveRsrqFromRsrpAndSinrDb(q.rsrpDbm, q.sinrDb, cell, plan.subcarrierSpacingHz);
        if (std::isfinite(derived))
        {
            q.rsrqDb = derived;
        }
    }
    return q;
}

std::vector<HoEventSpec>
GenerateLocalHoEvents(ScenarioPlan& plan, const MeasurementContext* measCtx)
{
    std::vector<HoEventSpec> out;
    plan.localHoDiag = ScenarioPlan::LocalHoDiag{};
    if (!plan.enableNs3LocalHoController)
    {
        return out;
    }

    // Cells currently contributing to interference / quality.
    std::vector<NodeSpec> activeCells;
    activeCells.reserve(plan.cells.size());
    for (const auto& cell : plan.cells)
    {
        if (cell.bandwidthHz > 0.0)
        {
            activeCells.push_back(cell);
        }
    }
    if (activeCells.empty())
    {
        return out;
    }

    // Candidate set for neighbor search: only active (>0 MHz) cells.
    // This keeps HO sequence consistent with "BWP reallocation first, then HO".
    std::vector<NodeSpec> candidateCells;
    candidateCells.reserve(plan.cells.size());
    for (const auto& cell : plan.cells)
    {
        if (cell.bandwidthHz > 0.0)
        {
            candidateCells.push_back(cell);
        }
    }
    if (candidateCells.empty())
    {
        return out;
    }

    std::unordered_map<uint32_t, NodeSpec> cellById;
    for (const auto& cell : plan.cells)
    {
        cellById[cell.id] = cell;
    }

    std::vector<uint32_t> ueIds;
    std::vector<uint32_t> targetCells;
    uint32_t evalCount = 0;
    uint32_t passCount = 0;
    uint32_t detailCount = 0;
    for (const auto& ue : plan.ues)
    {
        const auto currIt = cellById.find(ue.servingCellId);
        if (currIt == cellById.end())
        {
            continue;
        }
        const NodeSpec& servingCell = currIt->second;
        const bool servingIsTn = (servingCell.cellType == "TN");
        const bool servingIsNtn = (servingCell.cellType == "NTN");

        const NodeSpec* bestTn = nullptr;
        const NodeSpec* bestTnNeighbor = nullptr;
        const NodeSpec* bestNtn = nullptr;
        double bestTnRsrp = -std::numeric_limits<double>::infinity();
        double bestTnNeighborRsrp = -std::numeric_limits<double>::infinity();
        double bestNtnRsrp = -std::numeric_limits<double>::infinity();
        for (const auto& cell : candidateCells)
        {
            const double rsrp = EstimateRsrpDbm(plan, ue, cell);
            if (cell.cellType == "TN" && rsrp > bestTnRsrp)
            {
                bestTnRsrp = rsrp;
                bestTn = &cell;
            }
            else if (cell.cellType == "NTN" && rsrp > bestNtnRsrp)
            {
                bestNtnRsrp = rsrp;
                bestNtn = &cell;
            }
            if (cell.cellType == "TN" && cell.id != servingCell.id && rsrp > bestTnNeighborRsrp)
            {
                bestTnNeighborRsrp = rsrp;
                bestTnNeighbor = &cell;
            }
        }

        // Intra-system TN->TN A3 path for ns-3 local HO authority.
        if (servingIsTn && bestTnNeighbor != nullptr)
        {
            ++evalCount;
            ++plan.localHoDiag.tnToTnA3EvalCount;
            const auto currQ = ResolveUeCellQuality(plan, ue, servingCell, activeCells, measCtx);
            const auto candQ = ResolveUeCellQuality(plan, ue, *bestTnNeighbor, activeCells, measCtx);
            const double currRsrp = currQ.rsrpDbm;
            const double currRsrq = currQ.rsrqDb;
            const double candRsrp = candQ.rsrpDbm;
            const double candRsrq = candQ.rsrqDb;
            const bool passA3 = std::isfinite(currRsrp) && std::isfinite(candRsrp) &&
                                std::isfinite(candRsrq) &&
                                (candRsrp >= currRsrp + plan.a3OffsetDb) &&
                                (candRsrp >= plan.a3RsrpMinDbm) &&
                                (candRsrq >= plan.a3RsrqMinDb);
            if (detailCount < 16)
            {
                NS_LOG_UNCOND("[NS3LocalHoDiag] dir=TN->TN UE-" << ue.id
                              << " currCell=" << servingCell.id
                              << " candCell=" << bestTnNeighbor->id
                              << " trig=a3"
                              << " currRSRP=" << currRsrp
                              << " currRSRQ=" << currRsrq
                              << " candRSRP=" << candRsrp
                              << " candRSRQ=" << candRsrq
                              << " a3Offset=" << plan.a3OffsetDb
                              << " pass=" << (passA3 ? 1 : 0));
                ++detailCount;
            }
            if (passA3)
            {
                ++passCount;
                ++plan.localHoDiag.tnToTnA3PassCount;
                ueIds.push_back(ue.id);
                targetCells.push_back(bestTnNeighbor->id);
                continue;
            }
        }

        if (servingIsTn && bestNtn != nullptr)
        {
            ++evalCount;
            ++plan.localHoDiag.tnToNtnEvalCount;
            const auto currQ = ResolveUeCellQuality(plan, ue, servingCell, activeCells, measCtx);
            const auto candQ = ResolveUeCellQuality(plan, ue, *bestNtn, activeCells, measCtx);
            const double currRsrp = currQ.rsrpDbm;
            const double currRsrq = currQ.rsrqDb;
            const double candRsrp = candQ.rsrpDbm;
            const double candRsrq = candQ.rsrqDb;
            plan.localHoDiag.tnToNtnCurrRsrqSum += currRsrq;
            plan.localHoDiag.tnToNtnCandRsrqSum += candRsrq;
            plan.localHoDiag.tnToNtnCurrRsrpSum += currRsrp;
            plan.localHoDiag.tnToNtnCandRsrpSum += candRsrp;
            plan.localHoDiag.tnToNtnCurrRsrqMin = std::min(plan.localHoDiag.tnToNtnCurrRsrqMin, currRsrq);
            plan.localHoDiag.tnToNtnCurrRsrqMax = std::max(plan.localHoDiag.tnToNtnCurrRsrqMax, currRsrq);
            plan.localHoDiag.tnToNtnCandRsrqMin = std::min(plan.localHoDiag.tnToNtnCandRsrqMin, candRsrq);
            plan.localHoDiag.tnToNtnCandRsrqMax = std::max(plan.localHoDiag.tnToNtnCandRsrqMax, candRsrq);
            bool servingFail = false;
            bool neighPass = false;
            if (plan.a5TriggerMetricTnToNtn == "rsrp")
            {
                servingFail = currRsrp < plan.a5ServingRsrpThresholdDbmTnToNtn;
                neighPass = candRsrp > plan.a5NeighborRsrpThresholdDbmTnToNtn;
            }
            else
            {
                servingFail = currRsrq < plan.a5ServingRsrqThresholdDbTnToNtn;
                neighPass = candRsrq > plan.a5NeighborRsrqThresholdDbTnToNtn;
            }
            if (servingFail)
            {
                ++plan.localHoDiag.tnToNtnServingFailCount;
            }
            if (neighPass)
            {
                ++plan.localHoDiag.tnToNtnNeighborPassCount;
            }
            if (detailCount < 16)
            {
                NS_LOG_UNCOND("[NS3LocalHoDiag] dir=TN->NTN UE-" << ue.id
                              << " currCell=" << servingCell.id
                              << " candCell=" << bestNtn->id
                              << " trig=" << plan.a5TriggerMetricTnToNtn
                              << " currRSRP=" << currRsrp
                              << " currRSRQ=" << currRsrq
                              << " candRSRP=" << candRsrp
                              << " candRSRQ=" << candRsrq
                              << " servingFail=" << (servingFail ? 1 : 0)
                              << " neighPass=" << (neighPass ? 1 : 0));
                ++detailCount;
            }
            if (servingFail && neighPass)
            {
                ++passCount;
                ++plan.localHoDiag.tnToNtnPassCount;
                ueIds.push_back(ue.id);
                targetCells.push_back(bestNtn->id);
            }
        }
        else if (servingIsNtn && bestTn != nullptr)
        {
            ++evalCount;
            ++plan.localHoDiag.ntnToTnEvalCount;
            const auto currQ = ResolveUeCellQuality(plan, ue, servingCell, activeCells, measCtx);
            const auto candQ = ResolveUeCellQuality(plan, ue, *bestTn, activeCells, measCtx);
            const double currRsrp = currQ.rsrpDbm;
            const double currRsrq = currQ.rsrqDb;
            const double candRsrp = candQ.rsrpDbm;
            const double candRsrq = candQ.rsrqDb;
            plan.localHoDiag.ntnToTnCurrRsrqSum += currRsrq;
            plan.localHoDiag.ntnToTnCandRsrqSum += candRsrq;
            plan.localHoDiag.ntnToTnCurrRsrpSum += currRsrp;
            plan.localHoDiag.ntnToTnCandRsrpSum += candRsrp;
            plan.localHoDiag.ntnToTnCurrRsrqMin = std::min(plan.localHoDiag.ntnToTnCurrRsrqMin, currRsrq);
            plan.localHoDiag.ntnToTnCurrRsrqMax = std::max(plan.localHoDiag.ntnToTnCurrRsrqMax, currRsrq);
            plan.localHoDiag.ntnToTnCandRsrqMin = std::min(plan.localHoDiag.ntnToTnCandRsrqMin, candRsrq);
            plan.localHoDiag.ntnToTnCandRsrqMax = std::max(plan.localHoDiag.ntnToTnCandRsrqMax, candRsrq);
            bool servingFail = false;
            bool neighPass = false;
            if (plan.a5TriggerMetricNtnToTn == "rsrq")
            {
                servingFail = currRsrq < plan.a5ServingRsrqThresholdDbNtnToTn;
                neighPass = candRsrq > plan.a5NeighborRsrqThresholdDbNtnToTn;
            }
            else
            {
                servingFail = currRsrp < plan.a5ServingRsrpThresholdDbmNtnToTn;
                neighPass = candRsrp > plan.a5NeighborRsrpThresholdDbmNtnToTn;
            }
            if (servingFail)
            {
                ++plan.localHoDiag.ntnToTnServingFailCount;
            }
            if (neighPass)
            {
                ++plan.localHoDiag.ntnToTnNeighborPassCount;
            }
            if (detailCount < 16)
            {
                NS_LOG_UNCOND("[NS3LocalHoDiag] dir=NTN->TN UE-" << ue.id
                              << " currCell=" << servingCell.id
                              << " candCell=" << bestTn->id
                              << " trig=" << plan.a5TriggerMetricNtnToTn
                              << " currRSRP=" << currRsrp
                              << " currRSRQ=" << currRsrq
                              << " candRSRP=" << candRsrp
                              << " candRSRQ=" << candRsrq
                              << " servingFail=" << (servingFail ? 1 : 0)
                              << " neighPass=" << (neighPass ? 1 : 0));
                ++detailCount;
            }
            if (servingFail && neighPass)
            {
                ++passCount;
                ++plan.localHoDiag.ntnToTnPassCount;
                ueIds.push_back(ue.id);
                targetCells.push_back(bestTn->id);
            }
        }
    }
    plan.localHoDiag.evalCount = evalCount;
    plan.localHoDiag.passCount = passCount;

    NS_LOG_UNCOND("[NS3LocalHoDiag] summary eval=" << evalCount << " pass=" << passCount
                  << " tnToTnA3Eval=" << plan.localHoDiag.tnToTnA3EvalCount
                  << " tnToTnA3Pass=" << plan.localHoDiag.tnToTnA3PassCount
                  << " localHo=" << (plan.enableNs3LocalHoController ? 1 : 0)
                  << " trigTN2NTN=" << plan.a5TriggerMetricTnToNtn
                  << " trigNTN2TN=" << plan.a5TriggerMetricNtnToTn
                  << " a3Offset=" << plan.a3OffsetDb
                  << " tttMs=" << plan.a5TttMs);

    if (ueIds.empty())
    {
        return out;
    }

    if (ueIds.size() > plan.localHoMaxRequestsPerCycle)
    {
        NS_LOG_UNCOND("[NS3LocalHoDiag] admission cap: requested=" << ueIds.size()
                      << " capped=" << plan.localHoMaxRequestsPerCycle);
        ueIds.resize(plan.localHoMaxRequestsPerCycle);
        targetCells.resize(plan.localHoMaxRequestsPerCycle);
    }

    HoEventSpec event;
    const double tttSec = std::max(std::max(0.0, plan.a5TttMs), std::max(0.0, plan.a3TttMs)) / 1000.0;
    const double requestedTimeSec = std::max(plan.rrcControlPeriodSec + tttSec, 1e-3);
    const double earliestHoTimeSec = std::max(0.02, std::min(0.12, 0.25 * std::max(plan.runDurationSec, 0.0)));
    const double latestHoTimeSec = std::max(1e-3, std::max(plan.runDurationSec - 0.05, earliestHoTimeSec));
    event.eventTimeSec = std::min(std::max(requestedTimeSec, earliestHoTimeSec), latestHoTimeSec);
    event.targetUeIds = std::move(ueIds);
    event.targetCellIds = std::move(targetCells);
    out.push_back(std::move(event));
    return out;
}

double
Distance2d(const NodeSpec& lhs, const NodeSpec& rhs)
{
    const double dx = lhs.x - rhs.x;
    const double dy = lhs.y - rhs.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool
EvaluateOverlapGatePass(const ScenarioPlan& plan, uint32_t targetCellId)
{
    if (plan.tnAreaRadiusM <= 0.0 || plan.ntnAreaRadiusAtElevation90DegM <= 0.0)
    {
        return false;
    }
    const NodeSpec* targetCell = nullptr;
    for (const auto& cell : plan.cells)
    {
        if (cell.id == targetCellId)
        {
            targetCell = &cell;
            break;
        }
    }
    if (targetCell == nullptr)
    {
        return false;
    }
    for (const auto& tnCell : plan.cells)
    {
        if (tnCell.cellType != "TN")
        {
            continue;
        }
        for (const auto& ntnCell : plan.cells)
        {
            if (ntnCell.cellType != "NTN")
            {
                continue;
            }
            const double centerDistance = Distance2d(tnCell, ntnCell);
            if (centerDistance > (plan.tnAreaRadiusM + plan.ntnAreaRadiusAtElevation90DegM))
            {
                continue;
            }
            if ((targetCell->id == tnCell.id) || (targetCell->id == ntnCell.id))
            {
                return true;
            }
        }
    }
    return false;
}

double
EstimateOneWayPropagationDelaySec(const ScenarioPlan& plan, uint32_t targetCellId)
{
    if (plan.delayConfig.ntnOneWayPropagationDelaySec > 0.0)
    {
        return plan.delayConfig.ntnOneWayPropagationDelaySec;
    }
    constexpr double SPEED_OF_LIGHT_M_PER_SEC = 299792458.0;
    double fallbackDistanceM = 600000.0;
    for (const auto& cell : plan.cells)
    {
        if (cell.id == targetCellId && cell.cellType == "NTN")
        {
            fallbackDistanceM = std::max(cell.z, fallbackDistanceM);
            break;
        }
    }
    return fallbackDistanceM / SPEED_OF_LIGHT_M_PER_SEC;
}

[[maybe_unused]] std::vector<double>
CollectHoEventTimesSec(const std::string& controlJsonSource)
{
    std::vector<double> out;
    const auto eventBodies = ExtractObjectArrayBodiesAfterKey(controlJsonSource, "events");
    for (const auto& eventBody : eventBodies)
    {
        const auto eventType = ExtractFirstStringAfterKey(eventBody, "event_type").value_or("");
        const bool isHo = (eventType == "HandoverRequested") || (eventType == "HandoverStarted") ||
                          (eventType == "HandoverCompleted") || (eventType == "handover");
        if (!isHo)
        {
            continue;
        }
        out.push_back(ExtractFirstNumberAfterKey(eventBody, "event_time_sec").value_or(0.0));
    }
    return out;
}

std::vector<BwpTransactionRecord>
BuildBwpTransactions(const ScenarioPlan& plan)
{
    std::vector<BwpTransactionRecord> out;
    if (plan.bwpEvents.empty())
    {
        return out;
    }

    RunnerControlDelayModel delayModel(plan.delayConfig);
    const auto& hoEventTimes = plan.hoEventTimes;
    uint32_t generatedRequestId = 1;

    for (const auto& event : plan.bwpEvents)
    {
        BwpTransactionRecord tx;
        tx.requestId = event.requestId > 0 ? event.requestId : generatedRequestId++;
        tx.cellId = event.targetCellId;
        tx.requestTimeSec = std::max(0.0, event.eventTimeSec);
        tx.targetBwpId = event.targetBwpId;
        tx.targetDlFrequencyHz = event.targetDlFrequencyHz;
        tx.targetUlFrequencyHz = event.targetUlFrequencyHz;
        tx.targetBandwidthHz = event.targetBandwidthHz;
        tx.gateLbPassed = event.gateLbPassed;
        tx.gateDistressPassed = event.gateDistressPassed;
        tx.floorApplied = event.floorApplied;
        tx.zeroClamped = event.zeroClamped;
        tx.rateLimited = event.rateLimited;

        for (const auto& cell : plan.cells)
        {
            if (cell.id == tx.cellId)
            {
                tx.oldDlFrequencyHz = cell.carrierFrequencyHz;
                tx.oldUlFrequencyHz = cell.carrierFrequencyHz;
                tx.oldBandwidthHz = cell.bandwidthHz;
                break;
            }
        }
        if (tx.targetDlFrequencyHz <= 0.0)
        {
            tx.targetDlFrequencyHz = tx.oldDlFrequencyHz;
        }
        if (tx.targetUlFrequencyHz <= 0.0)
        {
            tx.targetUlFrequencyHz = tx.oldUlFrequencyHz;
        }
        if (tx.targetBandwidthHz <= 0.0)
        {
            tx.targetBandwidthHz = tx.oldBandwidthHz;
        }

        tx.overlapGatePassed = EvaluateOverlapGatePass(plan, tx.cellId);
        if (!tx.overlapGatePassed)
        {
            tx.success = false;
            tx.failureReason = "overlap_gate_rejected";
            out.push_back(tx);
            continue;
        }

        bool targetIsNtn = false;
        for (const auto& cell : plan.cells)
        {
            if (cell.id == tx.cellId)
            {
                targetIsNtn = (cell.cellType == "NTN");
                break;
            }
        }
        const auto delayDiag = delayModel.EvaluateBwpUpdateDelay(tx.cellId, targetIsNtn);
        tx.scheduledApplyTimeSec = tx.requestTimeSec + std::max(0.0, delayDiag.totalDelaySec);
        tx.effectiveApplyTimeSec = tx.scheduledApplyTimeSec;

        const double oneWayPropagationDelaySec = EstimateOneWayPropagationDelaySec(plan, tx.cellId);
        tx.ackTimeoutSec = std::max(
            plan.bwpAckTimeoutMinSec,
            plan.bwpAckTimeoutPropagationFactor * oneWayPropagationDelaySec + plan.rrcControlPeriodSec +
                plan.bwpAckTimeoutGuardSec);
        tx.ackTimeSec = tx.effectiveApplyTimeSec + std::min(
                                                   tx.ackTimeoutSec * 0.8,
                                                   2.0 * oneWayPropagationDelaySec + plan.rrcControlPeriodSec);
        const double ackDeadlineSec = tx.effectiveApplyTimeSec + tx.ackTimeoutSec;
        if (tx.ackTimeSec > ackDeadlineSec)
        {
            tx.success = false;
            tx.failureReason = "ack_timeout";
        }
        else
        {
            tx.success = true;
            tx.failureReason = "";
        }

        for (double hoTimeSec : hoEventTimes)
        {
            if (hoTimeSec >= tx.requestTimeSec && hoTimeSec <= tx.ackTimeSec)
            {
                tx.hoBlockedInWindow = true;
                break;
            }
        }
        out.push_back(tx);
    }

    return out;
}

ScenarioPlan
BuildScenarioPlanFromState(const std::string& operation,
                           double runDurationSec,
                           const std::string& topologyPath,
                           const std::string& channelPath,
                           const std::string& controlEventsPath,
                           const std::string& ntnTrajectoryPath,
                           const std::string& requestPath)
{
    ScenarioPlan plan;
    plan.operation = operation;
    plan.runDurationSec = runDurationSec;
    plan.topologySourcePath = topologyPath;
    plan.channelSourcePath = channelPath;
    plan.controlEventsSourcePath = controlEventsPath;
    plan.ntnTrajectorySourcePath = ntnTrajectoryPath;

    const std::string topologyJson = ReadTextFile(topologyPath);
    const std::string controlEventsJson = ReadTextFile(controlEventsPath);
    const std::string ntnTrajectoryJson = ReadTextFile(ntnTrajectoryPath);
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
    if (auto profileBody = ExtractObjectBodyAfterKey(topologyJson, "profile"))
    {
        if (operation == "create_and_connect_nodes")
        {
            ResetLocalBootstrapAdaptiveState();
        }
        plan.tnAreaRadiusM = ExtractFirstNumberAfterKey(*profileBody, "tn_area_radius_m").value_or(0.0);
        plan.ntnAreaRadiusAtElevation90DegM =
            ExtractFirstNumberAfterKey(*profileBody, "ntn_area_radius_at_elevation90_deg_m")
                .value_or(0.0);
        plan.bwpAckTimeoutMinSec =
            ExtractFirstNumberAfterKey(*profileBody, "bwp_ack_timeout_min_sec").value_or(0.020);
        plan.bwpAckTimeoutPropagationFactor =
            ExtractFirstNumberAfterKey(*profileBody, "bwp_ack_timeout_propagation_factor")
                .value_or(4.0);
        plan.bwpAckTimeoutGuardSec =
            ExtractFirstNumberAfterKey(*profileBody, "bwp_ack_timeout_guard_sec").value_or(0.005);
        plan.rrcControlPeriodSec =
            ExtractFirstNumberAfterKey(*profileBody, "rrc_control_period_sec").value_or(0.10);
        plan.subcarrierSpacingHz =
            ExtractFirstNumberAfterKey(*profileBody, "subcarrier_spacing_hz").value_or(30000.0);
        plan.enableCuDistressBandwidthPolicy =
            ExtractFirstBoolAfterKey(*profileBody, "enable_cu_distress_bandwidth_policy")
                .value_or(false);
        plan.enableNtnBootstrapWhenNoAttachedUe =
            ExtractFirstBoolAfterKey(*profileBody, "enable_ntn_bootstrap_when_no_attached_ue")
                .value_or(false);
        plan.forceZeroNtnBandwidthWhenNoAttachedUe =
            ExtractFirstBoolAfterKey(*profileBody, "force_zero_ntn_bandwidth_when_no_attached_ue")
                .value_or(false);
        plan.ntnBootstrapDistressThreshold =
            ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_distress_threshold")
                .value_or(0.7);
        plan.ntnBootstrapMinBandwidthHz =
            ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_min_bandwidth_hz")
                .value_or(5e6);
        plan.enableAdaptiveNtnBootstrapThreshold =
            ExtractFirstBoolAfterKey(*profileBody, "enable_adaptive_ntn_bootstrap_threshold")
                .value_or(false);
        plan.ntnBootstrapDistressThresholdMin =
            ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_distress_threshold_min")
                .value_or(0.0);
        plan.ntnBootstrapDistressThresholdMax =
            ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_distress_threshold_max")
                .value_or(1.0);
        plan.ntnBootstrapDistressThresholdNominal =
            ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_distress_threshold_nominal")
                .value_or(plan.ntnBootstrapDistressThreshold);
        plan.ntnBootstrapDistressThresholdStepUp =
            ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_distress_threshold_step_up")
                .value_or(0.05);
        plan.ntnBootstrapDistressThresholdStepDown =
            ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_distress_threshold_step_down")
                .value_or(0.05);
        plan.ntnBootstrapAttachHoldCount = static_cast<uint32_t>(
            std::max(1.0, ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_attach_hold_count")
                              .value_or(1.0)));
        plan.ntnBootstrapNoAttachHoldCount = static_cast<uint32_t>(
            std::max(1.0, ExtractFirstNumberAfterKey(*profileBody, "ntn_bootstrap_no_attach_hold_count")
                              .value_or(1.0)));
        plan.maxRequestsPerSecond =
            ExtractFirstNumberAfterKey(*profileBody, "max_requests_per_second").value_or(10.0);
        plan.minShareChangeForRequest =
            ExtractFirstNumberAfterKey(*profileBody, "min_share_change_for_request").value_or(0.01);
        plan.enableNs3LocalBwpController =
            ExtractFirstBoolAfterKey(*profileBody, "enable_ns3_local_bwp_controller")
                .value_or(true);
        plan.ns3LocalBwpPolicyName =
            ExtractFirstStringAfterKey(*profileBody, "ns3_local_bwp_policy_name")
                .value_or("matlab_legacy");
        plan.enableNs3LocalHoController =
            ExtractFirstBoolAfterKey(*profileBody, "enable_ns3_local_ho_controller")
                .value_or(false);
        plan.enableNs3RuntimeControlLoop =
            ExtractFirstBoolAfterKey(*profileBody, "enable_ns3_runtime_control_loop")
                .value_or(true);
        plan.ns3RuntimeControlPeriodSec =
            std::max(0.0,
                     ExtractFirstNumberAfterKey(*profileBody, "ns3_runtime_control_period_sec")
                         .value_or(plan.rrcControlPeriodSec));
        plan.a3OffsetDb =
            ExtractFirstNumberAfterKey(*profileBody, "a3_offset_db").value_or(0.5);
        plan.a3TttMs =
            ExtractFirstNumberAfterKey(*profileBody, "a3_ttt_ms").value_or(100.0);
        plan.a3RsrpMinDbm =
            ExtractFirstNumberAfterKey(*profileBody, "a3_rsrp_min_dbm").value_or(-125.0);
        plan.a3RsrqMinDb =
            ExtractFirstNumberAfterKey(*profileBody, "a3_rsrq_min_db").value_or(-20.0);
        plan.a5TriggerMetricTnToNtn =
            ExtractFirstStringAfterKey(*profileBody, "a5_trigger_metric_tn_to_ntn")
                .value_or("rsrq");
        plan.a5TriggerMetricNtnToTn =
            ExtractFirstStringAfterKey(*profileBody, "a5_trigger_metric_ntn_to_tn")
                .value_or("rsrp");
        plan.a5ServingRsrqThresholdDbTnToNtn =
            ExtractFirstNumberAfterKey(*profileBody, "a5_serving_rsrq_threshold_db_tn_to_ntn")
                .value_or(-12.0);
        plan.a5NeighborRsrqThresholdDbTnToNtn =
            ExtractFirstNumberAfterKey(*profileBody, "a5_neighbor_rsrq_threshold_db_tn_to_ntn")
                .value_or(-11.0);
        plan.a5ServingRsrqThresholdDbNtnToTn =
            ExtractFirstNumberAfterKey(*profileBody, "a5_serving_rsrq_threshold_db_ntn_to_tn")
                .value_or(-12.0);
        plan.a5NeighborRsrqThresholdDbNtnToTn =
            ExtractFirstNumberAfterKey(*profileBody, "a5_neighbor_rsrq_threshold_db_ntn_to_tn")
                .value_or(-11.0);
        plan.a5ServingRsrpThresholdDbmTnToNtn =
            ExtractFirstNumberAfterKey(*profileBody, "a5_serving_rsrp_threshold_dbm_tn_to_ntn")
                .value_or(-75.0);
        plan.a5NeighborRsrpThresholdDbmTnToNtn =
            ExtractFirstNumberAfterKey(*profileBody, "a5_neighbor_rsrp_threshold_dbm_tn_to_ntn")
                .value_or(-90.0);
        plan.a5ServingRsrpThresholdDbmNtnToTn =
            ExtractFirstNumberAfterKey(*profileBody, "a5_serving_rsrp_threshold_dbm_ntn_to_tn")
                .value_or(-95.0);
        plan.a5NeighborRsrpThresholdDbmNtnToTn =
            ExtractFirstNumberAfterKey(*profileBody, "a5_neighbor_rsrp_threshold_dbm_ntn_to_tn")
                .value_or(-70.0);
        plan.a5TttMs =
            ExtractFirstNumberAfterKey(*profileBody, "a5_ttt_ms").value_or(100.0);
        plan.localHoMaxRequestsPerCycle = static_cast<uint32_t>(
            std::max(1.0, ExtractFirstNumberAfterKey(*profileBody, "local_ho_max_requests_per_cycle")
                              .value_or(static_cast<double>(plan.localHoMaxRequestsPerCycle))));
        plan.localHoMinInterRequestSec =
            std::max(0.0, ExtractFirstNumberAfterKey(*profileBody, "local_ho_min_inter_request_sec")
                              .value_or(plan.localHoMinInterRequestSec));
        plan.allowCrossTypeHoReplay =
            ExtractFirstBoolAfterKey(*profileBody, "allow_cross_type_ho_replay")
                .value_or(false);
        plan.txPowerDbmPerCell =
            ExtractNumberArrayAfterKey(*profileBody, "transmit_power_dbm");
        plan.gnbTxAntennaGainDbiPerCell =
            ExtractNumberArrayAfterKey(*profileBody, "gnb_tx_antenna_gain_dbi");
        plan.gnbRxAntennaGainDbiPerCell =
            ExtractNumberArrayAfterKey(*profileBody, "gnb_rx_antenna_gain_dbi");
        plan.gnbTxFeederLossDbPerCell =
            ExtractNumberArrayAfterKey(*profileBody, "gnb_tx_feeder_loss_db");
        plan.gnbRxFeederLossDbPerCell =
            ExtractNumberArrayAfterKey(*profileBody, "gnb_rx_feeder_loss_db");
        plan.gnbAzimuthDegPerCell =
            ExtractNumberArrayAfterKey(*profileBody, "gnb_azimuth_deg");
        plan.gnbDowntiltDegPerCell =
            ExtractNumberArrayAfterKey(*profileBody, "gnb_downtilt_deg");
        plan.applyDirectionalPatternCellMask =
            ExtractBoolArrayAfterKey(*profileBody, "apply_directional_pattern_cell_mask");
        plan.enableAntennaPattern =
            ExtractFirstBoolAfterKey(*profileBody, "enable_antenna_pattern").value_or(false);
        plan.ueTxAntennaGainDbi =
            ExtractFirstNumberAfterKey(*profileBody, "ue_tx_antenna_gain_dbi").value_or(0.0);
        plan.ueRxAntennaGainDbi =
            ExtractFirstNumberAfterKey(*profileBody, "ue_rx_antenna_gain_dbi").value_or(0.0);
        plan.ueTxFeederLossDb =
            ExtractFirstNumberAfterKey(*profileBody, "ue_tx_feeder_loss_db").value_or(0.0);
        plan.ueRxFeederLossDb =
            ExtractFirstNumberAfterKey(*profileBody, "ue_rx_feeder_loss_db").value_or(0.0);
        plan.noiseFigureDb =
            ExtractFirstNumberAfterKey(*profileBody, "noise_figure_db").value_or(7.0);
        plan.horizBeamwidthDeg =
            ExtractFirstNumberAfterKey(*profileBody, "horiz_beamwidth_deg").value_or(65.0);
        plan.vertBeamwidthDeg =
            ExtractFirstNumberAfterKey(*profileBody, "vert_beamwidth_deg").value_or(10.0);
        plan.maxAttenuationDb =
            ExtractFirstNumberAfterKey(*profileBody, "max_attenuation_db").value_or(30.0);
        plan.sideLobeAttenuationDb =
            ExtractFirstNumberAfterKey(*profileBody, "side_lobe_attenuation_db").value_or(30.0);
        plan.cuDistressLevelThresholds =
            ExtractNumberArrayAfterKey(*profileBody, "cu_distress_level_thresholds");
        plan.cuNtnBandwidthLevelsHz =
            ExtractNumberArrayAfterKey(*profileBody, "cu_ntn_bandwidth_levels_hz");
    }
    plan.controlEventCount = CountMatches(controlJsonSource, std::regex("\"event_type\"\\s*:"));
    plan.bwpEventCount = CountMatches(controlJsonSource,
                                      std::regex("\"event_type\"\\s*:\\s*\"(BwpReconfigRequested|BwpReconfigApplied|bwp_reconfig)\""));
    plan.hoEventCount = CountMatches(controlJsonSource,
                                     std::regex("\"event_type\"\\s*:\\s*\"(HandoverRequested|HandoverStarted|HandoverCompleted|handover)\""));
    plan.taEventCount = CountMatches(controlJsonSource,
                                     std::regex("\"event_type\"\\s*:\\s*\"(TaAdjustmentRequested|TaAdjustmentApplied|timing_advance)\""));
    plan.bwpEvents = ExtractBwpEventsFromControlSource(controlJsonSource);
    if (plan.bwpEvents.empty() && !plan.enableNs3RuntimeControlLoop)
    {
        plan.bwpEvents = GenerateLocalBwpEvents(plan, nullptr);
        // Reflect locally generated BWP decisions into the runtime plan so
        // active cell selection and local HO target discovery use updated BW.
        ApplyLocalBwpEventsToPlanCells(plan, plan.bwpEvents);
    }
    plan.hoEvents = ExtractHoEventsFromControlSource(controlJsonSource);
    if (plan.hoEvents.empty() && !plan.enableNs3RuntimeControlLoop)
    {
        // NS-3 control authority path: generate local HO request events from
        // threshold policy after local BWP plan reflection.
        plan.hoEvents = GenerateLocalHoEvents(plan, nullptr);
    }
    plan.hoEventTimes.clear();
    plan.hoEventTimes.reserve(plan.hoEvents.size());
    for (const auto& hoEvent : plan.hoEvents)
    {
        plan.hoEventTimes.push_back(hoEvent.eventTimeSec);
    }
    if (!plan.hoEventTimes.empty())
    {
        plan.hoEventCount = static_cast<uint32_t>(plan.hoEventTimes.size());
    }

    std::optional<std::string> ntnTrajectoryBody;
    if (!ntnTrajectoryJson.empty())
    {
        ntnTrajectoryBody = ExtractObjectBodyAfterKey(ntnTrajectoryJson, "payload");
    }
    if (!ntnTrajectoryBody.has_value())
    {
        ntnTrajectoryBody = ExtractObjectBodyAfterKey(topologyJson, "ntn_trajectory");
    }
    if (ntnTrajectoryBody.has_value())
    {
        const std::string& body = *ntnTrajectoryBody;
        if (auto enabled = ExtractFirstBoolAfterKey(body, "enabled"))
        {
            plan.ntnTrajectoryEnabled = *enabled;
        }
        if (auto period = ExtractFirstNumberAfterKey(body, "update_period_sec"))
        {
            plan.ntnTrajectoryUpdatePeriodSec = *period;
        }
        const auto trackBodies = ExtractObjectArrayBodiesAfterKey(body, "tracks");
        for (const auto& trackBody : trackBodies)
        {
            NtnTrajectoryTrack track;
            if (auto cellIdx = ExtractFirstNumberAfterKey(trackBody, "cell_idx"))
            {
                track.cellId = static_cast<uint32_t>(std::max(0.0, *cellIdx));
            }
            if (auto source = ExtractFirstStringAfterKey(trackBody, "source"))
            {
                track.source = *source;
            }
            track.timeSec = ExtractNumberArrayAfterKey(trackBody, "time_sec");
            track.positionsM = ExtractMatrix3AfterKey(trackBody, "positions_m");
            if (track.cellId == 0 || track.timeSec.empty() || track.positionsM.empty())
            {
                continue;
            }
            if (track.timeSec.size() != track.positionsM.size())
            {
                continue;
            }
            if (std::abs(track.timeSec.front()) > 1e-6)
            {
                continue;
            }
            bool monotonic = true;
            for (size_t i = 1; i < track.timeSec.size(); ++i)
            {
                if (!(track.timeSec[i] > track.timeSec[i - 1]))
                {
                    monotonic = false;
                    break;
                }
            }
            if (!monotonic)
            {
                continue;
            }
            plan.ntnTrajectoryWaypointCount += static_cast<uint32_t>(track.timeSec.size());
            plan.ntnTrajectoryTracks.push_back(track);
        }
        plan.ntnTrajectoryTrackCount = static_cast<uint32_t>(plan.ntnTrajectoryTracks.size());

        for (const auto& track : plan.ntnTrajectoryTracks)
        {
            if (track.positionsM.empty())
            {
                continue;
            }
            for (auto& cell : plan.cells)
            {
                if (cell.id == track.cellId && cell.cellType == "NTN")
                {
                    cell.x = track.positionsM.front()[0];
                    cell.y = track.positionsM.front()[1];
                    cell.z = track.positionsM.front()[2];
                    break;
                }
            }
        }
    }

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
    metrics.ntnTrajectoryEnabled = plan.ntnTrajectoryEnabled;
    metrics.ntnTrajectoryUpdatePeriodSec = plan.ntnTrajectoryUpdatePeriodSec;
    metrics.ntnTrajectoryTrackCount = plan.ntnTrajectoryTrackCount;
    metrics.ntnTrajectoryWaypointCount = plan.ntnTrajectoryWaypointCount;
    metrics.localHoDiag = plan.localHoDiag;
    metrics.localBwpPolicyName = plan.ns3LocalBwpPolicyName;
    metrics.dbgA5TriggerMetricTnToNtn = plan.a5TriggerMetricTnToNtn;
    metrics.dbgA5TriggerMetricNtnToTn = plan.a5TriggerMetricNtnToTn;
    metrics.dbgA5ServingRsrqThresholdDbTnToNtn = plan.a5ServingRsrqThresholdDbTnToNtn;
    metrics.dbgA5NeighborRsrqThresholdDbTnToNtn = plan.a5NeighborRsrqThresholdDbTnToNtn;
    metrics.dbgA5ServingRsrqThresholdDbNtnToTn = plan.a5ServingRsrqThresholdDbNtnToTn;
    metrics.dbgA5NeighborRsrqThresholdDbNtnToTn = plan.a5NeighborRsrqThresholdDbNtnToTn;
    metrics.dbgA5ServingRsrpThresholdDbmTnToNtn = plan.a5ServingRsrpThresholdDbmTnToNtn;
    metrics.dbgA5NeighborRsrpThresholdDbmTnToNtn = plan.a5NeighborRsrpThresholdDbmTnToNtn;
    metrics.dbgA5ServingRsrpThresholdDbmNtnToTn = plan.a5ServingRsrpThresholdDbmNtnToTn;
    metrics.dbgA5NeighborRsrpThresholdDbmNtnToTn = plan.a5NeighborRsrpThresholdDbmNtnToTn;
    const auto& adapt = GetLocalBootstrapAdaptiveState();
    metrics.localBwpBootstrapThresholdCurrent = adapt.thresholdCurrent;
    metrics.localBwpBootstrapRaiseCount = adapt.raiseCount;
    metrics.localBwpBootstrapSuccCount = adapt.succCount;
    metrics.localBwpBootstrapFailCount = adapt.failCount;
    metrics.localBwpBootstrapIdleCount = adapt.idleCount;
    metrics.localBwpBootstrapPoorPersistCount = adapt.poorPersistCount;
    metrics.localBwpBootstrapPoorFractionProxy = adapt.poorFractionProxy;
    metrics.localBwpBootstrapPoorGate = adapt.poorGate;
    metrics.bwpTransactions = BuildBwpTransactions(plan);
    metrics.bwpTransactionCount = static_cast<uint32_t>(metrics.bwpTransactions.size());
    for (const auto& tx : metrics.bwpTransactions)
    {
        if (tx.success)
        {
            ++metrics.bwpSuccessCount;
        }
        else
        {
            ++metrics.bwpFailureCount;
        }
        if (tx.hoBlockedInWindow)
        {
            ++metrics.bwpHoBlockedCount;
        }
    }
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

    uint32_t activeCellCount = 0;
    std::map<uint32_t, bool> activeCellIds;
    for (const auto& cell : plan.cells)
    {
        // Minimal scenario is a TN radio path. When NTN cells are active,
        // we still run this path for TN subset (instead of placeholder),
        // and simply exclude NTN cells from the simulated set.
        if (cell.bandwidthHz > 0.0 && cell.cellType == "TN")
        {
            ++activeCellCount;
            activeCellIds[cell.id] = true;
        }
    }
    if (activeCellCount == 0)
    {
        return false;
    }

    return std::any_of(plan.ues.begin(), plan.ues.end(), [&activeCellIds](const UeSpec& ue) {
        return activeCellIds.find(ue.servingCellId) != activeCellIds.end();
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
    metrics.servingCellIdPerUe.assign(plan.ues.size(), 0.0);
    metrics.servingRsrpDbmPerUe.assign(plan.ues.size(), std::numeric_limits<double>::quiet_NaN());
    metrics.servingRsrqDbPerUe.assign(plan.ues.size(), std::numeric_limits<double>::quiet_NaN());
    metrics.servingSinrDbPerUe.assign(plan.ues.size(), std::numeric_limits<double>::quiet_NaN());
    metrics.servingRsrpDbmPerUeApprox.assign(plan.ues.size(), std::numeric_limits<double>::quiet_NaN());
    metrics.servingRsrqDbPerUeApprox.assign(plan.ues.size(), std::numeric_limits<double>::quiet_NaN());
    std::vector<double> usedRePerCell(plan.cells.size(), 0.0);
    std::vector<double> availRePerCell(plan.cells.size(), 0.0);
    std::vector<uint32_t> dlTbPerCell(plan.cells.size(), 0);
    std::vector<uint32_t> dlCorruptTbPerCell(plan.cells.size(), 0);
    std::vector<uint32_t> ulTbPerCell(plan.cells.size(), 0);
    std::vector<uint32_t> ulCorruptTbPerCell(plan.cells.size(), 0);
    std::map<uint16_t, uint32_t> traceCellIdToPlanIndex;
    MeasurementContext measCtx;

    Config::SetDefault("ns3::NrRlcUm::MaxTxBufferSize", UintegerValue(999999999));

    std::vector<NodeSpec> activeCells;
    std::map<uint32_t, uint32_t> activeCellIdToLocalIndex;
    for (const auto& cell : plan.cells)
    {
        // Install active cells with non-zero bandwidth (TN and NTN).
        if (cell.bandwidthHz > 0.0)
        {
            activeCellIdToLocalIndex[cell.id] = static_cast<uint32_t>(activeCells.size());
            activeCells.push_back(cell);
        }
    }

    std::vector<UeSpec> attachedUes;
    attachedUes.reserve(plan.ues.size());
    for (const auto& ue : plan.ues)
    {
        if (activeCellIdToLocalIndex.find(ue.servingCellId) != activeCellIdToLocalIndex.end())
        {
            attachedUes.push_back(ue);
        }
    }
    if (activeCells.empty() || attachedUes.empty())
    {
        return metrics;
    }

    // TN->NTN transition emulation is intentionally disabled here.
    // Reason: pre-attach serving-cell rewrites can hide true signaling issues and
    // may collapse traffic onto NTN unexpectedly. Keep initial attachment untouched.

    NodeContainer gnbNodes;
    NodeContainer ueNodes;
    gnbNodes.Create(activeCells.size());
    ueNodes.Create(attachedUes.size());

    MobilityHelper gnbMobility;
    gnbMobility.SetMobilityModel("ns3::WaypointMobilityModel");
    gnbMobility.Install(gnbNodes);

    uint32_t appliedNtnTrackCount = 0;
    uint32_t appliedNtnWaypointCount = 0;
    std::map<uint32_t, NtnTrajectoryTrack> ntnTrackByCellId;
    for (const auto& track : plan.ntnTrajectoryTracks)
    {
        ntnTrackByCellId[track.cellId] = track;
    }
    for (uint32_t idx = 0; idx < gnbNodes.GetN() && idx < activeCells.size(); ++idx)
    {
        const auto& cell = activeCells[idx];
        const auto fallbackZ = cell.z > 0.0 ? cell.z : 25.0;
        Vector initialPos(cell.x, cell.y, fallbackZ);
        bool hasTrajectoryForCell = false;
        auto trackIt = ntnTrackByCellId.find(cell.id);
        if (plan.ntnTrajectoryEnabled && cell.cellType == "NTN" && trackIt != ntnTrackByCellId.end() &&
            !trackIt->second.positionsM.empty())
        {
            initialPos = Vector(trackIt->second.positionsM.front()[0],
                                trackIt->second.positionsM.front()[1],
                                trackIt->second.positionsM.front()[2]);
            hasTrajectoryForCell = true;
        }
        Ptr<WaypointMobilityModel> gnbWpMob =
            gnbNodes.Get(idx)->GetObject<WaypointMobilityModel>();
        if (!gnbWpMob)
        {
            continue;
        }
        gnbWpMob->AddWaypoint(Waypoint(Seconds(0.0), initialPos));
        if (!hasTrajectoryForCell)
        {
            continue;
        }
        const auto& track = trackIt->second;
        ++appliedNtnTrackCount;
        ++appliedNtnWaypointCount;
        for (size_t wpIdx = 1; wpIdx < track.timeSec.size() && wpIdx < track.positionsM.size(); ++wpIdx)
        {
            const double tSec = track.timeSec[wpIdx];
            if (!(tSec > 0.0))
            {
                continue;
            }
            const auto& pos = track.positionsM[wpIdx];
            gnbWpMob->AddWaypoint(Waypoint(Seconds(tSec), Vector(pos[0], pos[1], pos[2])));
            ++appliedNtnWaypointCount;
        }
    }
    if (plan.ntnTrajectoryEnabled)
    {
        metrics.ntnTrajectoryTrackCount = appliedNtnTrackCount;
        metrics.ntnTrajectoryWaypointCount = appliedNtnWaypointCount;
    }

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
    for (const auto& cell : activeCells)
    {
        if (cell.carrierFrequencyHz > 0.0)
        {
            centerFrequencyHz = cell.carrierFrequencyHz;
            break;
        }
    }
    for (const auto& cell : activeCells)
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

    // Explicitly establish X2 adjacency among active gNBs so local HO replay
    // via NrHelper::HandoverRequest has a valid EPC-X2 path.
    if (gnbNodes.GetN() >= 2)
    {
        nrHelper->AddX2Interface(gnbNodes);
    }

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
        traceCellIdToPlanIndex[gnb->GetCellId()] = activeCells[idx].id - 1;
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
            rrc->TraceConnectWithoutContext(
                "HandoverStart",
                MakeBoundCallback(&RecordUeHoStart,
                                  &metrics.hoTrace,
                                  &metrics.debugUeHoStartCount,
                                  &metrics.debugHoRequestAcceptedCount));
            rrc->TraceConnectWithoutContext(
                "HandoverEndOk",
                MakeBoundCallback(&RecordUeHoEndOk,
                                  &metrics.hoTrace,
                                  &metrics.debugUeHoEndOkCount));
            rrc->TraceConnectWithoutContext(
                "HandoverEndError",
                MakeBoundCallback(&RecordUeHoEndError,
                                  &metrics.hoTrace,
                                  &metrics.debugUeHoEndErrorCount,
                                  &metrics.debugHoRequestRejectedCount));
            rrc->TraceConnectWithoutContext(
                "RadioLinkFailure",
                MakeBoundCallback(&RecordUeRlf,
                                  &metrics.hoTrace,
                                  &metrics.debugUeRlfCount,
                                  &metrics.debugHoRequestRejectedCount));
            rrc->TraceConnectWithoutContext(
                "ConnectionTimeout",
                MakeBoundCallback(&RecordUeConnectionTimeout,
                                  &metrics.hoTrace,
                                  &metrics.debugUeConnectionTimeoutCount,
                                  &metrics.debugHoRequestRejectedCount));
        }
        if (phy && phy->GetSpectrumPhy())
        {
            phy->TraceConnectWithoutContext("ReportRsrp",
                                            MakeBoundCallback(&RecordServingRsrp,
                                                              &measCtx.rsrpDbmByCellRntiBwp));
            phy->TraceConnectWithoutContext("ReportUeMeasurements",
                                            MakeBoundCallback(&RecordUeMeasurements,
                                                              &measCtx.rsrpDbmByCellRntiBwp,
                                                              &measCtx.rsrqDbByCellRntiBwp));
            phy->TraceConnectWithoutContext("ReportCurrentCellRsrpSinr",
                                            MakeBoundCallback(&RecordCurrentCellSinr,
                                                              &measCtx.sinrDbByCellRntiBwp));
            phy->GetSpectrumPhy()->TraceConnectWithoutContext(
                "RxPacketTraceUe",
                MakeBoundCallback(&RecordDlRxPacketTrace,
                                  &traceCellIdToPlanIndex,
                                  &measCtx.sinrDbByCellRntiBwp,
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
                                  &measCtx.sinrDbByCellRntiBwp,
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
    for (uint32_t idx = 0; idx < activeCells.size() && idx < gnbDevices.GetN(); ++idx)
    {
        gnbByCellId[activeCells[idx].id] = gnbDevices.Get(idx);
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

    // Replay HO events from control log as explicit ns-3 handover requests.
    // This keeps the initial attach deterministic while still exercising
    // real RRC HO signaling when event targets are resolvable.
    if (!plan.hoEvents.empty())
    {
        std::map<uint32_t, uint32_t> ueIdToDeviceIndex;
        std::map<uint32_t, uint32_t> currentServingCellByUeId;
        std::map<uint32_t, std::string> cellTypeByCellId;
        for (const auto& cell : plan.cells)
        {
            cellTypeByCellId[cell.id] = cell.cellType;
        }
        for (uint32_t idx = 0; idx < attachedUes.size() && idx < ueDevices.GetN(); ++idx)
        {
            ueIdToDeviceIndex[attachedUes[idx].id] = idx;
            currentServingCellByUeId[attachedUes[idx].id] = attachedUes[idx].servingCellId;
        }

        uint32_t hoScheduleOrdinal = 0;
        const double hoStaggerSec = std::max(0.0, plan.localHoMinInterRequestSec);
        for (const auto& hoEvent : plan.hoEvents)
        {
            if (!(hoEvent.eventTimeSec >= 0.0))
            {
                continue;
            }
            if (hoEvent.targetUeIds.empty() || hoEvent.targetCellIds.empty())
            {
                continue;
            }

            uint32_t issuedInEvent = 0;
            for (size_t ueIdx = 0; ueIdx < hoEvent.targetUeIds.size(); ++ueIdx)
            {
                if (issuedInEvent >= plan.localHoMaxRequestsPerCycle)
                {
                    break;
                }
                const uint32_t ueId = hoEvent.targetUeIds[ueIdx];
                const auto ueDevIt = ueIdToDeviceIndex.find(ueId);
                if (ueDevIt == ueIdToDeviceIndex.end() || ueDevIt->second >= ueDevices.GetN())
                {
                    ++metrics.debugHoRequestRejectedPrecheckCount;
                    ++metrics.debugHoRequestRejectUnknownUeCount;
                    NS_LOG_UNCOND("[NS3LocalHoReq] rejected_precheck reason=unknown_ue ueId=" << ueId);
                    continue;
                }

                uint32_t targetCellId = hoEvent.targetCellIds.front();
                if (hoEvent.targetCellIds.size() == hoEvent.targetUeIds.size())
                {
                    targetCellId = hoEvent.targetCellIds[ueIdx];
                }

                const auto srcCellIt = currentServingCellByUeId.find(ueId);
                if (srcCellIt == currentServingCellByUeId.end())
                {
                    ++metrics.debugHoRequestRejectedPrecheckCount;
                    ++metrics.debugHoRequestRejectMissingSourceCount;
                    NS_LOG_UNCOND("[NS3LocalHoReq] rejected_precheck reason=missing_source ueId=" << ueId
                                  << " dstCell=" << targetCellId);
                    continue;
                }
                const uint32_t sourceCellId = srcCellIt->second;
                if (sourceCellId == targetCellId)
                {
                    ++metrics.debugHoRequestRejectedPrecheckCount;
                    ++metrics.debugHoRequestRejectSameCellCount;
                    NS_LOG_UNCOND("[NS3LocalHoReq] rejected_precheck reason=same_cell ueId=" << ueId
                                  << " cell=" << sourceCellId);
                    continue;
                }
                const auto srcTypeIt = cellTypeByCellId.find(sourceCellId);
                const auto dstTypeIt = cellTypeByCellId.find(targetCellId);
                if (srcTypeIt == cellTypeByCellId.end() || dstTypeIt == cellTypeByCellId.end())
                {
                    ++metrics.debugHoRequestRejectedPrecheckCount;
                    ++metrics.debugHoRequestRejectMissingCellTypeCount;
                    NS_LOG_UNCOND("[NS3LocalHoReq] rejected_precheck reason=missing_cell_type ueId=" << ueId
                                  << " srcCell=" << sourceCellId << " dstCell=" << targetCellId);
                    continue;
                }
                if (!plan.allowCrossTypeHoReplay && (srcTypeIt->second != dstTypeIt->second))
                {
                    ++metrics.debugHoRequestRejectedPrecheckCount;
                    ++metrics.debugHoRequestRejectCrossTypeBlockedCount;
                    NS_LOG_UNCOND("[NS3LocalHoDiag] skip cross-type HO replay UE-" << ueId
                                  << " srcCell=" << sourceCellId << "(" << srcTypeIt->second << ")"
                                  << " dstCell=" << targetCellId << "(" << dstTypeIt->second << ")"
                                  << " allowCrossType=" << (plan.allowCrossTypeHoReplay ? 1 : 0));
                    continue;
                }

                const auto srcGnbIt = gnbByCellId.find(sourceCellId);
                const auto dstGnbIt = gnbByCellId.find(targetCellId);
                if (srcGnbIt == gnbByCellId.end() || dstGnbIt == gnbByCellId.end())
                {
                    ++metrics.debugHoRequestRejectedPrecheckCount;
                    ++metrics.debugHoRequestRejectMissingGnbCount;
                    NS_LOG_UNCOND("[NS3LocalHoReq] rejected_precheck reason=missing_gnb ueId=" << ueId
                                  << " srcCell=" << sourceCellId << " dstCell=" << targetCellId);
                    continue;
                }

                Ptr<NetDevice> ueDev = ueDevices.Get(ueDevIt->second);
                Ptr<NetDevice> srcGnb = srcGnbIt->second;
                Ptr<NetDevice> dstGnb = dstGnbIt->second;
                const double scheduledHoTimeSec = std::max(0.0, hoEvent.eventTimeSec) +
                                                  (static_cast<double>(hoScheduleOrdinal) * hoStaggerSec);
                ++hoScheduleOrdinal;
                ++issuedInEvent;
                ++metrics.debugHoRequestSubmittedCount;
                NS_LOG_UNCOND("[NS3LocalHoReq] submit ueId=" << ueId
                              << " srcCell=" << sourceCellId
                              << " dstCell=" << targetCellId
                              << " when=" << scheduledHoTimeSec);
                Simulator::Schedule(Seconds(scheduledHoTimeSec),
                                    [nrHelper, ueDev, srcGnb, dstGnb, ueId, sourceCellId, targetCellId, &metrics]() {
                                        ++metrics.debugHoRequestDispatchCount;
                                        NS_LOG_UNCOND("[NS3LocalHoReq] dispatch ueId=" << ueId
                                                      << " srcCell=" << sourceCellId
                                                      << " dstCell=" << targetCellId
                                                      << " now=" << Simulator::Now().GetSeconds());
                                        nrHelper->HandoverRequest(Seconds(0.0), ueDev, srcGnb, dstGnb);
                                    });

                // Keep local serving map consistent for subsequent events.
                currentServingCellByUeId[ueId] = targetCellId;
            }
        }
        NS_LOG_UNCOND("[NS3LocalHoReq] summary submitted=" << metrics.debugHoRequestSubmittedCount
                      << " dispatched=" << metrics.debugHoRequestDispatchCount
                      << " rejected_precheck=" << metrics.debugHoRequestRejectedPrecheckCount
                      << " rejectUnknownUe=" << metrics.debugHoRequestRejectUnknownUeCount
                      << " rejectMissingSource=" << metrics.debugHoRequestRejectMissingSourceCount
                      << " rejectSameCell=" << metrics.debugHoRequestRejectSameCellCount
                      << " rejectMissingCellType=" << metrics.debugHoRequestRejectMissingCellTypeCount
                      << " rejectCrossTypeBlocked=" << metrics.debugHoRequestRejectCrossTypeBlockedCount
                      << " rejectMissingGnb=" << metrics.debugHoRequestRejectMissingGnbCount
                      << " accepted_rrc=" << metrics.debugHoRequestAcceptedCount
                      << " rejected_rrc=" << metrics.debugHoRequestRejectedCount
                      << " rrcStart=" << metrics.debugUeHoStartCount
                      << " rrcEndOk=" << metrics.debugUeHoEndOkCount
                      << " rrcEndErr=" << metrics.debugUeHoEndErrorCount
                      << " rlf=" << metrics.debugUeRlfCount
                      << " connTimeout=" << metrics.debugUeConnectionTimeoutCount);
    }

    const double simTimeSec = std::max(plan.runDurationSec, 0.6);
    const double appStartSec = 0.1;
    const double appStopSec = std::max(simTimeSec - 0.05, appStartSec + 0.1);
    const double trafficWindowSec = std::max(appStopSec - appStartSec, 0.1);
    const uint32_t packetSizeBytes = 1200;
    const Time packetInterval = MicroSeconds(200);
    // Keep mutable runtime state alive for the whole scenario execution.
    ScenarioPlan runtimePlan = plan;

    if (plan.enableNs3RuntimeControlLoop &&
        (plan.enableNs3LocalHoController || plan.enableNs3LocalBwpController))
    {
        metrics.localHoDecisionBasis = "approx_rsrp_rsrq_runtime_loop";
        metrics.localBwpDecisionBasis = "measured_sinr_runtime_loop";
        metrics.localHoDecisionBasis = "measured_pref_runtime_loop";

        std::map<uint32_t, std::string> cellTypeByCellId;
        for (const auto& cell : plan.cells)
        {
            cellTypeByCellId[cell.id] = cell.cellType;
        }
        std::map<uint32_t, uint32_t> ueIdToDeviceIndex;
        for (uint32_t idx = 0; idx < attachedUes.size() && idx < ueDevices.GetN(); ++idx)
        {
            ueIdToDeviceIndex[attachedUes[idx].id] = idx;
        }

        auto runtimeTick = std::make_shared<std::function<void()>>();
        *runtimeTick = [&, runtimeTick, cellTypeByCellId, ueIdToDeviceIndex]() {
            const double nowSec = Simulator::Now().GetSeconds();
            if (nowSec >= (simTimeSec - 0.02))
            {
                return;
            }

            for (uint32_t idx = 0; idx < attachedUes.size() && idx < ueDevices.GetN(); ++idx)
            {
                Ptr<NrUeNetDevice> ueDev = DynamicCast<NrUeNetDevice>(ueDevices.Get(idx));
                if (!ueDev || !ueDev->GetRrc())
                {
                    continue;
                }
                const uint32_t ueId = attachedUes[idx].id;
                if (ueId < 1 || ueId > runtimePlan.ues.size())
                {
                    continue;
                }
                const uint16_t currentCellId = ueDev->GetRrc()->GetCellId();
                if (currentCellId > 0)
                {
                    runtimePlan.ues[ueId - 1].servingCellId = currentCellId;
                }
            }

            if (runtimePlan.enableNs3LocalBwpController)
            {
                const auto localBwpEvents = GenerateLocalBwpEvents(runtimePlan, &measCtx);
                if (!localBwpEvents.empty())
                {
                    ApplyLocalBwpEventsToPlanCells(runtimePlan, localBwpEvents);
                    runtimePlan.bwpEventCount += static_cast<uint32_t>(localBwpEvents.size());
                    runtimePlan.bwpEvents.insert(runtimePlan.bwpEvents.end(),
                                                 localBwpEvents.begin(),
                                                 localBwpEvents.end());
                }
            }

            if (runtimePlan.enableNs3LocalHoController)
            {
                const auto localHoEvents = GenerateLocalHoEvents(runtimePlan, &measCtx);
                for (const auto& hoEvent : localHoEvents)
                {
                    uint32_t issuedInEvent = 0;
                    for (size_t ueIdx = 0; ueIdx < hoEvent.targetUeIds.size(); ++ueIdx)
                    {
                        if (issuedInEvent >= runtimePlan.localHoMaxRequestsPerCycle)
                        {
                            break;
                        }
                        const uint32_t ueId = hoEvent.targetUeIds[ueIdx];
                        const auto ueDevIt = ueIdToDeviceIndex.find(ueId);
                        if (ueDevIt == ueIdToDeviceIndex.end() || ueDevIt->second >= ueDevices.GetN())
                        {
                            ++metrics.debugHoRequestRejectedPrecheckCount;
                            ++metrics.debugHoRequestRejectUnknownUeCount;
                            continue;
                        }
                        uint32_t targetCellId = hoEvent.targetCellIds.empty() ? 0 : hoEvent.targetCellIds.front();
                        if (hoEvent.targetCellIds.size() == hoEvent.targetUeIds.size())
                        {
                            targetCellId = hoEvent.targetCellIds[ueIdx];
                        }
                        if (targetCellId == 0)
                        {
                            continue;
                        }

                        Ptr<NrUeNetDevice> ueDev = DynamicCast<NrUeNetDevice>(ueDevices.Get(ueDevIt->second));
                        if (!ueDev || !ueDev->GetRrc())
                        {
                            continue;
                        }
                        const uint32_t sourceCellId = static_cast<uint32_t>(ueDev->GetRrc()->GetCellId());
                        if (sourceCellId == 0)
                        {
                            ++metrics.debugHoRequestRejectedPrecheckCount;
                            ++metrics.debugHoRequestRejectMissingSourceCount;
                            continue;
                        }
                        if (sourceCellId == targetCellId)
                        {
                            ++metrics.debugHoRequestRejectedPrecheckCount;
                            ++metrics.debugHoRequestRejectSameCellCount;
                            continue;
                        }
                        const auto srcTypeIt = cellTypeByCellId.find(sourceCellId);
                        const auto dstTypeIt = cellTypeByCellId.find(targetCellId);
                        if (srcTypeIt == cellTypeByCellId.end() || dstTypeIt == cellTypeByCellId.end())
                        {
                            ++metrics.debugHoRequestRejectedPrecheckCount;
                            ++metrics.debugHoRequestRejectMissingCellTypeCount;
                            continue;
                        }
                        if (!runtimePlan.allowCrossTypeHoReplay && (srcTypeIt->second != dstTypeIt->second))
                        {
                            ++metrics.debugHoRequestRejectedPrecheckCount;
                            ++metrics.debugHoRequestRejectCrossTypeBlockedCount;
                            continue;
                        }
                        const auto srcGnbIt = gnbByCellId.find(sourceCellId);
                        const auto dstGnbIt = gnbByCellId.find(targetCellId);
                        if (srcGnbIt == gnbByCellId.end() || dstGnbIt == gnbByCellId.end())
                        {
                            ++metrics.debugHoRequestRejectedPrecheckCount;
                            ++metrics.debugHoRequestRejectMissingGnbCount;
                            continue;
                        }

                        ++issuedInEvent;
                        ++metrics.debugHoRequestSubmittedCount;
                        const double staggerSec = std::max(0.0, runtimePlan.localHoMinInterRequestSec);
                        const double dispatchDelaySec = std::max(1e-3, static_cast<double>(issuedInEvent - 1) * staggerSec);
                        Simulator::Schedule(Seconds(dispatchDelaySec),
                                            [nrHelper,
                                             ueDev,
                                             srcGnb = srcGnbIt->second,
                                             dstGnb = dstGnbIt->second,
                                             ueId,
                                             sourceCellId,
                                             targetCellId,
                                             &metrics]() {
                                                ++metrics.debugHoRequestDispatchCount;
                                                NS_LOG_UNCOND("[NS3LocalHoReq] dispatch(runtime) ueId=" << ueId
                                                              << " srcCell=" << sourceCellId
                                                              << " dstCell=" << targetCellId
                                                              << " now=" << Simulator::Now().GetSeconds());
                                                nrHelper->HandoverRequest(Seconds(0.0), ueDev, srcGnb, dstGnb);
                                            });
                    }
                }
            }

            const double slotSec = ComputeSlotDurationSec(runtimePlan.subcarrierSpacingHz);
            const double nextStepSec = std::max(slotSec, runtimePlan.ns3RuntimeControlPeriodSec);
            if ((nowSec + nextStepSec) < (simTimeSec - 0.01))
            {
                Simulator::Schedule(Seconds(nextStepSec), *runtimeTick);
            }
        };

        const double firstTickSec = std::max(appStartSec, 0.05);
        Simulator::Schedule(Seconds(firstTickSec), *runtimeTick);
        const double slotSec = ComputeSlotDurationSec(runtimePlan.subcarrierSpacingHz);
        NS_LOG_UNCOND("[NS3LocalCtrl] runtime_loop=1 periodSec=" << std::max(slotSec, runtimePlan.ns3RuntimeControlPeriodSec)
                      << " slotSec=" << slotSec
                      << " scsHz=" << runtimePlan.subcarrierSpacingHz
                      << " localHO=" << (runtimePlan.enableNs3LocalHoController ? 1 : 0)
                      << " localBWP=" << (runtimePlan.enableNs3LocalBwpController ? 1 : 0));
    }

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
    std::map<uint32_t, NodeSpec> cellByIdFinal;
    for (const auto& cell : plan.cells)
    {
        cellByIdFinal[cell.id] = cell;
    }
    for (const auto& ue : plan.ues)
    {
        if (ue.id < 1 || ue.id > plan.ues.size())
        {
            continue;
        }
        const size_t ueIdx = ue.id - 1;
        const uint32_t servingCellId = ue.servingCellId;
        metrics.servingCellIdPerUe[ueIdx] = static_cast<double>(servingCellId);
        const auto cellIt = cellByIdFinal.find(servingCellId);
        if (cellIt == cellByIdFinal.end())
        {
            continue;
        }
        const double rsrp = EstimateRsrpDbm(plan, ue, cellIt->second);
        const double rsrq = EstimateRsrqDb(plan, ue, cellIt->second, activeCells);
        metrics.servingRsrpDbmPerUeApprox[ueIdx] = rsrp;
        metrics.servingRsrqDbPerUeApprox[ueIdx] = rsrq;
        metrics.servingRsrpDbmPerUe[ueIdx] = rsrp;
        metrics.servingRsrqDbPerUe[ueIdx] = rsrq;

        uint16_t servingCellIdU16 = static_cast<uint16_t>(servingCellId);
        uint16_t rntiU16 = 0;
        if (ueIdx < plan.requestedRntis.size())
        {
            const uint32_t requestedRnti = plan.requestedRntis[ueIdx];
            if (requestedRnti > 0 && requestedRnti <= std::numeric_limits<uint16_t>::max())
            {
                rntiU16 = static_cast<uint16_t>(requestedRnti);
            }
        }
        const double measuredSinrDb = LookupServingSinrDb(measCtx.sinrDbByCellRntiBwp, servingCellIdU16, rntiU16);
        const double measuredRsrpDbm = LookupServingRsrpDbm(measCtx.rsrpDbmByCellRntiBwp, servingCellIdU16, rntiU16);
        const double measuredRsrqDb = LookupCellMetricDb(measCtx.rsrqDbByCellRntiBwp, servingCellIdU16, rntiU16);
        if (std::isfinite(measuredRsrpDbm))
        {
            metrics.servingRsrpDbmPerUe[ueIdx] = measuredRsrpDbm;
        }
        if (std::isfinite(measuredRsrqDb) && std::abs(measuredRsrqDb) > 1e-9)
        {
            metrics.servingRsrqDbPerUe[ueIdx] = measuredRsrqDb;
        }
        else if (std::isfinite(metrics.servingRsrpDbmPerUe[ueIdx]) && std::isfinite(measuredSinrDb))
        {
            const double derivedRsrq = DeriveRsrqFromRsrpAndSinrDb(metrics.servingRsrpDbmPerUe[ueIdx],
                                                                   measuredSinrDb,
                                                                   cellIt->second,
                                                                   plan.subcarrierSpacingHz);
            if (std::isfinite(derivedRsrq))
            {
                metrics.servingRsrqDbPerUe[ueIdx] = derivedRsrq;
            }
        }
        if (std::isfinite(measuredSinrDb))
        {
            metrics.servingSinrDbPerUe[ueIdx] = measuredSinrDb;
        }
        else
        {
            metrics.servingSinrDbPerUe[ueIdx] = rsrp - rsrq;
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
    output << "  \"debug_ue_ho_start_count\": " << metrics.debugUeHoStartCount << ",\n";
    output << "  \"debug_ue_ho_end_ok_count\": " << metrics.debugUeHoEndOkCount << ",\n";
    output << "  \"debug_ue_ho_end_error_count\": " << metrics.debugUeHoEndErrorCount << ",\n";
    output << "  \"debug_ue_rlf_count\": " << metrics.debugUeRlfCount << ",\n";
    output << "  \"debug_ue_connection_timeout_count\": " << metrics.debugUeConnectionTimeoutCount
           << ",\n";
    output << "  \"debug_ho_request_submitted_count\": " << metrics.debugHoRequestSubmittedCount
           << ",\n";
    output << "  \"debug_ho_request_dispatch_count\": " << metrics.debugHoRequestDispatchCount
           << ",\n";
    output << "  \"debug_ho_request_rejected_precheck_count\": "
           << metrics.debugHoRequestRejectedPrecheckCount << ",\n";
    output << "  \"debug_ho_request_reject_unknown_ue_count\": "
           << metrics.debugHoRequestRejectUnknownUeCount << ",\n";
    output << "  \"debug_ho_request_reject_missing_source_count\": "
           << metrics.debugHoRequestRejectMissingSourceCount << ",\n";
    output << "  \"debug_ho_request_reject_same_cell_count\": "
           << metrics.debugHoRequestRejectSameCellCount << ",\n";
    output << "  \"debug_ho_request_reject_missing_cell_type_count\": "
           << metrics.debugHoRequestRejectMissingCellTypeCount << ",\n";
    output << "  \"debug_ho_request_reject_cross_type_blocked_count\": "
           << metrics.debugHoRequestRejectCrossTypeBlockedCount << ",\n";
    output << "  \"debug_ho_request_reject_missing_gnb_count\": "
           << metrics.debugHoRequestRejectMissingGnbCount << ",\n";
    output << "  \"debug_ho_request_accepted_count\": " << metrics.debugHoRequestAcceptedCount
           << ",\n";
    output << "  \"debug_ho_request_rejected_count\": " << metrics.debugHoRequestRejectedCount
           << ",\n";
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
    output << "  \"bwp_transaction_count\": " << metrics.bwpTransactionCount << ",\n";
    output << "  \"bwp_success_count\": " << metrics.bwpSuccessCount << ",\n";
    output << "  \"bwp_failure_count\": " << metrics.bwpFailureCount << ",\n";
    output << "  \"bwp_ho_blocked_count\": " << metrics.bwpHoBlockedCount << ",\n";
    output << "  \"local_bwp_policy_name\": \"" << EscapeJsonString(metrics.localBwpPolicyName)
           << "\",\n";
    output << "  \"local_ho_decision_basis\": \"" << EscapeJsonString(metrics.localHoDecisionBasis)
           << "\",\n";
    output << "  \"local_bwp_decision_basis\": \"" << EscapeJsonString(metrics.localBwpDecisionBasis)
           << "\",\n";
    output << "  \"local_bwp_bootstrap_threshold_current\": "
           << metrics.localBwpBootstrapThresholdCurrent << ",\n";
    output << "  \"local_bwp_bootstrap_raise_count\": " << metrics.localBwpBootstrapRaiseCount
           << ",\n";
    output << "  \"local_bwp_bootstrap_succ_count\": " << metrics.localBwpBootstrapSuccCount
           << ",\n";
    output << "  \"local_bwp_bootstrap_fail_count\": " << metrics.localBwpBootstrapFailCount
           << ",\n";
    output << "  \"local_bwp_bootstrap_idle_count\": " << metrics.localBwpBootstrapIdleCount
           << ",\n";
    output << "  \"local_bwp_bootstrap_poor_persist_count\": "
           << metrics.localBwpBootstrapPoorPersistCount << ",\n";
    output << "  \"local_bwp_bootstrap_poor_fraction_proxy\": "
           << metrics.localBwpBootstrapPoorFractionProxy << ",\n";
    output << "  \"local_bwp_bootstrap_poor_gate\": "
           << (metrics.localBwpBootstrapPoorGate ? "true" : "false") << ",\n";
    output << "  \"ho_event_count\": " << metrics.hoEventCount << ",\n";
    output << "  \"ta_event_count\": " << metrics.taEventCount << ",\n";
    output << "  \"ntn_trajectory_enabled\": " << (metrics.ntnTrajectoryEnabled ? "true" : "false")
           << ",\n";
    output << "  \"ntn_trajectory_update_period_sec\": " << metrics.ntnTrajectoryUpdatePeriodSec
           << ",\n";
    output << "  \"ntn_trajectory_track_count\": " << metrics.ntnTrajectoryTrackCount << ",\n";
    output << "  \"ntn_trajectory_waypoint_count\": " << metrics.ntnTrajectoryWaypointCount << ",\n";
    output << "  \"local_ho_diag_eval_count\": " << metrics.localHoDiag.evalCount << ",\n";
    output << "  \"local_ho_diag_pass_count\": " << metrics.localHoDiag.passCount << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_eval_count\": " << metrics.localHoDiag.tnToNtnEvalCount
           << ",\n";
    output << "  \"local_ho_diag_tn_to_tn_a3_eval_count\": " << metrics.localHoDiag.tnToTnA3EvalCount
           << ",\n";
    output << "  \"local_ho_diag_tn_to_tn_a3_pass_count\": " << metrics.localHoDiag.tnToTnA3PassCount
           << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_serving_fail_count\": "
           << metrics.localHoDiag.tnToNtnServingFailCount << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_neighbor_pass_count\": "
           << metrics.localHoDiag.tnToNtnNeighborPassCount << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_pass_count\": " << metrics.localHoDiag.tnToNtnPassCount
           << ",\n";
    output << "  \"local_ho_diag_ntn_to_tn_eval_count\": " << metrics.localHoDiag.ntnToTnEvalCount
           << ",\n";
    output << "  \"local_ho_diag_ntn_to_tn_serving_fail_count\": "
           << metrics.localHoDiag.ntnToTnServingFailCount << ",\n";
    output << "  \"local_ho_diag_ntn_to_tn_neighbor_pass_count\": "
           << metrics.localHoDiag.ntnToTnNeighborPassCount << ",\n";
    output << "  \"local_ho_diag_ntn_to_tn_pass_count\": " << metrics.localHoDiag.ntnToTnPassCount
           << ",\n";
    const double tn2ntnEvalDen = std::max(1u, metrics.localHoDiag.tnToNtnEvalCount);
    const double ntn2tnEvalDen = std::max(1u, metrics.localHoDiag.ntnToTnEvalCount);
    const double tn2ntnCurrRsrqMean = metrics.localHoDiag.tnToNtnCurrRsrqSum / tn2ntnEvalDen;
    const double tn2ntnCandRsrqMean = metrics.localHoDiag.tnToNtnCandRsrqSum / tn2ntnEvalDen;
    const double tn2ntnCurrRsrpMean = metrics.localHoDiag.tnToNtnCurrRsrpSum / tn2ntnEvalDen;
    const double tn2ntnCandRsrpMean = metrics.localHoDiag.tnToNtnCandRsrpSum / tn2ntnEvalDen;
    const double ntn2tnCurrRsrqMean = metrics.localHoDiag.ntnToTnCurrRsrqSum / ntn2tnEvalDen;
    const double ntn2tnCandRsrqMean = metrics.localHoDiag.ntnToTnCandRsrqSum / ntn2tnEvalDen;
    const double ntn2tnCurrRsrpMean = metrics.localHoDiag.ntnToTnCurrRsrpSum / ntn2tnEvalDen;
    const double ntn2tnCandRsrpMean = metrics.localHoDiag.ntnToTnCandRsrpSum / ntn2tnEvalDen;
    output << "  \"local_ho_diag_tn_to_ntn_curr_rsrq_mean_db\": " << tn2ntnCurrRsrqMean << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_cand_rsrq_mean_db\": " << tn2ntnCandRsrqMean << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_curr_rsrp_mean_dbm\": " << tn2ntnCurrRsrpMean << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_cand_rsrp_mean_dbm\": " << tn2ntnCandRsrpMean << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_curr_rsrq_min_db\": "
           << (std::isfinite(metrics.localHoDiag.tnToNtnCurrRsrqMin) ? metrics.localHoDiag.tnToNtnCurrRsrqMin : 0.0)
           << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_curr_rsrq_max_db\": "
           << (std::isfinite(metrics.localHoDiag.tnToNtnCurrRsrqMax) ? metrics.localHoDiag.tnToNtnCurrRsrqMax : 0.0)
           << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_cand_rsrq_min_db\": "
           << (std::isfinite(metrics.localHoDiag.tnToNtnCandRsrqMin) ? metrics.localHoDiag.tnToNtnCandRsrqMin : 0.0)
           << ",\n";
    output << "  \"local_ho_diag_tn_to_ntn_cand_rsrq_max_db\": "
           << (std::isfinite(metrics.localHoDiag.tnToNtnCandRsrqMax) ? metrics.localHoDiag.tnToNtnCandRsrqMax : 0.0)
           << ",\n";
    output << "  \"local_ho_diag_ntn_to_tn_curr_rsrq_mean_db\": " << ntn2tnCurrRsrqMean << ",\n";
    output << "  \"local_ho_diag_ntn_to_tn_cand_rsrq_mean_db\": " << ntn2tnCandRsrqMean << ",\n";
    output << "  \"local_ho_diag_ntn_to_tn_curr_rsrp_mean_dbm\": " << ntn2tnCurrRsrpMean << ",\n";
    output << "  \"local_ho_diag_ntn_to_tn_cand_rsrp_mean_dbm\": " << ntn2tnCandRsrpMean << ",\n";
    output << "  \"dbg_a5_trigger_metric_tn_to_ntn\": \"" << EscapeJsonString(metrics.dbgA5TriggerMetricTnToNtn) << "\",\n";
    output << "  \"dbg_a5_trigger_metric_ntn_to_tn\": \"" << EscapeJsonString(metrics.dbgA5TriggerMetricNtnToTn) << "\",\n";
    output << "  \"dbg_a5_serving_rsrq_threshold_db_tn_to_ntn\": "
           << metrics.dbgA5ServingRsrqThresholdDbTnToNtn << ",\n";
    output << "  \"dbg_a5_neighbor_rsrq_threshold_db_tn_to_ntn\": "
           << metrics.dbgA5NeighborRsrqThresholdDbTnToNtn << ",\n";
    output << "  \"dbg_a5_serving_rsrq_threshold_db_ntn_to_tn\": "
           << metrics.dbgA5ServingRsrqThresholdDbNtnToTn << ",\n";
    output << "  \"dbg_a5_neighbor_rsrq_threshold_db_ntn_to_tn\": "
           << metrics.dbgA5NeighborRsrqThresholdDbNtnToTn << ",\n";
    output << "  \"dbg_a5_serving_rsrp_threshold_dbm_tn_to_ntn\": "
           << metrics.dbgA5ServingRsrpThresholdDbmTnToNtn << ",\n";
    output << "  \"dbg_a5_neighbor_rsrp_threshold_dbm_tn_to_ntn\": "
           << metrics.dbgA5NeighborRsrpThresholdDbmTnToNtn << ",\n";
    output << "  \"dbg_a5_serving_rsrp_threshold_dbm_ntn_to_tn\": "
           << metrics.dbgA5ServingRsrpThresholdDbmNtnToTn << ",\n";
    output << "  \"dbg_a5_neighbor_rsrp_threshold_dbm_ntn_to_tn\": "
           << metrics.dbgA5NeighborRsrpThresholdDbmNtnToTn << ",\n";

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
    output << "],\n";
    output << "  \"ho_trace\": [";
    for (size_t idx = 0; idx < metrics.hoTrace.size(); ++idx)
    {
        const auto& trace = metrics.hoTrace[idx];
        if (idx > 0)
        {
            output << ", ";
        }
        output << "{";
        output << "\"event_type\": \"" << EscapeJsonString(trace.eventType) << "\", ";
        output << "\"time_sec\": " << trace.timeSec << ", ";
        output << "\"imsi\": " << trace.imsi << ", ";
        output << "\"cell_id\": " << trace.cellId << ", ";
        output << "\"rnti\": " << trace.rnti << ", ";
        output << "\"target_cell_id\": " << trace.targetCellId;
        output << "}";
    }
    output << "],\n";
    output << "  \"bwp_transactions\": [";
    for (size_t idx = 0; idx < metrics.bwpTransactions.size(); ++idx)
    {
        const auto& tx = metrics.bwpTransactions[idx];
        if (idx > 0)
        {
            output << ", ";
        }
        output << "{";
        output << "\"request_id\": " << tx.requestId << ", ";
        output << "\"cell_id\": " << tx.cellId << ", ";
        output << "\"overlap_gate_passed\": " << (tx.overlapGatePassed ? "true" : "false") << ", ";
        output << "\"gate_lb_passed\": " << (tx.gateLbPassed ? "true" : "false") << ", ";
        output << "\"gate_distress_passed\": " << (tx.gateDistressPassed ? "true" : "false") << ", ";
        output << "\"floor_applied\": " << (tx.floorApplied ? "true" : "false") << ", ";
        output << "\"zero_clamped\": " << (tx.zeroClamped ? "true" : "false") << ", ";
        output << "\"rate_limited\": " << (tx.rateLimited ? "true" : "false") << ", ";
        output << "\"success\": " << (tx.success ? "true" : "false") << ", ";
        output << "\"failure_reason\": \"" << EscapeJsonString(tx.failureReason) << "\", ";
        output << "\"old_bwp_id\": " << tx.oldBwpId << ", ";
        output << "\"target_bwp_id\": " << tx.targetBwpId << ", ";
        output << "\"old_dl_frequency_hz\": " << tx.oldDlFrequencyHz << ", ";
        output << "\"old_ul_frequency_hz\": " << tx.oldUlFrequencyHz << ", ";
        output << "\"old_bandwidth_hz\": " << tx.oldBandwidthHz << ", ";
        output << "\"target_dl_frequency_hz\": " << tx.targetDlFrequencyHz << ", ";
        output << "\"target_ul_frequency_hz\": " << tx.targetUlFrequencyHz << ", ";
        output << "\"target_bandwidth_hz\": " << tx.targetBandwidthHz << ", ";
        output << "\"request_time_sec\": " << tx.requestTimeSec << ", ";
        output << "\"scheduled_apply_time_sec\": " << tx.scheduledApplyTimeSec << ", ";
        output << "\"effective_apply_time_sec\": " << tx.effectiveApplyTimeSec << ", ";
        output << "\"ack_time_sec\": " << tx.ackTimeSec << ", ";
        output << "\"ack_timeout_sec\": " << tx.ackTimeoutSec << ", ";
        output << "\"ho_blocked_in_window\": " << (tx.hoBlockedInWindow ? "true" : "false");
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
    output << ",\n";
    writeNumberArray("serving_cell_id_per_ue", metrics.servingCellIdPerUe);
    output << ",\n";
    writeNumberArray("serving_rsrp_dbm_per_ue", metrics.servingRsrpDbmPerUe);
    output << ",\n";
    writeNumberArray("serving_rsrq_db_per_ue", metrics.servingRsrqDbPerUe);
    output << ",\n";
    writeNumberArray("serving_sinr_db_per_ue", metrics.servingSinrDbPerUe);
    output << ",\n";
    writeNumberArray("serving_rsrp_dbm_per_ue_approx", metrics.servingRsrpDbmPerUeApprox);
    output << ",\n";
    writeNumberArray("serving_rsrq_db_per_ue_approx", metrics.servingRsrqDbPerUeApprox);
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
    std::string ntnTrajectoryPath;
    std::string operation = "unknown";
    double runDurationSec = 0.0;

    CommandLine cmd(__FILE__);
    cmd.AddValue("requestPath", "Path to backend request JSON", requestPath);
    cmd.AddValue("metricsPath", "Path to backend metrics JSON", metricsPath);
    cmd.AddValue("topologyPath", "Path to persisted topology JSON", topologyPath);
    cmd.AddValue("channelConfigPath", "Path to persisted channel-config JSON", channelConfigPath);
    cmd.AddValue("controlEventsPath", "Path to persisted control-events JSON", controlEventsPath);
    cmd.AddValue("ntnTrajectoryPath", "Path to persisted NTN trajectory JSON", ntnTrajectoryPath);
    cmd.AddValue("operation", "Backend operation name", operation);
    cmd.AddValue("runDurationSec", "Requested run duration in seconds", runDurationSec);
    cmd.Parse(argc, argv);

    NS_LOG_UNCOND("Scratch Simulator");
    NS_LOG_UNCOND("Operation=" << operation);
    NS_LOG_UNCOND("RunDurationSec=" << runDurationSec);
    NS_LOG_UNCOND("TopologyPath=" << topologyPath);
    NS_LOG_UNCOND("ChannelConfigPath=" << channelConfigPath);
    NS_LOG_UNCOND("ControlEventsPath=" << controlEventsPath);
    NS_LOG_UNCOND("NtnTrajectoryPath=" << ntnTrajectoryPath);

    const ScenarioPlan plan =
        BuildScenarioPlanFromState(operation,
                                   runDurationSec,
                                   topologyPath,
                                   channelConfigPath,
                                   controlEventsPath,
                                   ntnTrajectoryPath,
                                   requestPath);

    NS_LOG_UNCOND("ScenarioPlan: Cells=" << plan.cells.size() << " UEs=" << plan.ues.size());

    if (!metricsPath.empty())
    {
        MetricsOutput metrics;
        const bool forcePlaceholder = false;
#if SCRATCH_SIM_HAS_NR
        if (!forcePlaceholder && SupportsMinimalTnNrScenario(plan))
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


