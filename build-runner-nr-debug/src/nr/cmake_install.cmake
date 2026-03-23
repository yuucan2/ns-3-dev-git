# Install script for directory: C:/ns3-dev-link/src/nr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/NS3")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/msys64/ucrt64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-nr-debug.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-nr-debug.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-nr-debug.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-nr-debug.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-nr-debug.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/nr/helper/beamforming-helper-base.h"
    "C:/ns3-dev-link/src/nr/helper/cc-bwp-helper.h"
    "C:/ns3-dev-link/src/nr/helper/file-scenario-helper.h"
    "C:/ns3-dev-link/src/nr/helper/grid-scenario-helper.h"
    "C:/ns3-dev-link/src/nr/helper/hexagonal-grid-scenario-helper.h"
    "C:/ns3-dev-link/src/nr/helper/ideal-beamforming-helper.h"
    "C:/ns3-dev-link/src/nr/helper/node-distribution-scenario-interface.h"
    "C:/ns3-dev-link/src/nr/helper/nr-bearer-stats-calculator.h"
    "C:/ns3-dev-link/src/nr/helper/nr-bearer-stats-connector.h"
    "C:/ns3-dev-link/src/nr/helper/nr-bearer-stats-simple.h"
    "C:/ns3-dev-link/src/nr/helper/nr-epc-helper.h"
    "C:/ns3-dev-link/src/nr/helper/nr-helper.h"
    "C:/ns3-dev-link/src/nr/helper/nr-mac-rx-trace.h"
    "C:/ns3-dev-link/src/nr/helper/nr-mac-scheduling-stats.h"
    "C:/ns3-dev-link/src/nr/helper/nr-no-backhaul-epc-helper.h"
    "C:/ns3-dev-link/src/nr/helper/nr-phy-rx-trace.h"
    "C:/ns3-dev-link/src/nr/helper/nr-point-to-point-epc-helper-base.h"
    "C:/ns3-dev-link/src/nr/helper/nr-point-to-point-epc-helper.h"
    "C:/ns3-dev-link/src/nr/helper/nr-radio-environment-map-helper.h"
    "C:/ns3-dev-link/src/nr/helper/nr-spectrum-value-helper.h"
    "C:/ns3-dev-link/src/nr/helper/nr-stats-calculator.h"
    "C:/ns3-dev-link/src/nr/helper/realistic-beamforming-helper.h"
    "C:/ns3-dev-link/src/nr/helper/scenario-parameters.h"
    "C:/ns3-dev-link/src/nr/helper/three-gpp-ftp-m1-helper.h"
    "C:/ns3-dev-link/src/nr/model/bandwidth-part-gnb.h"
    "C:/ns3-dev-link/src/nr/model/bandwidth-part-ue.h"
    "C:/ns3-dev-link/src/nr/model/beam-id.h"
    "C:/ns3-dev-link/src/nr/model/beam-manager.h"
    "C:/ns3-dev-link/src/nr/model/beamforming-vector.h"
    "C:/ns3-dev-link/src/nr/model/bwp-manager-algorithm.h"
    "C:/ns3-dev-link/src/nr/model/bwp-manager-gnb.h"
    "C:/ns3-dev-link/src/nr/model/bwp-manager-ue.h"
    "C:/ns3-dev-link/src/nr/model/ideal-beamforming-algorithm.h"
    "C:/ns3-dev-link/src/nr/model/lena-error-model.h"
    "C:/ns3-dev-link/src/nr/model/nr-a2-a4-rsrq-handover-algorithm.h"
    "C:/ns3-dev-link/src/nr/model/nr-a3-rsrp-handover-algorithm.h"
    "C:/ns3-dev-link/src/nr/model/nr-tn-ntn-handover-algorithm.h"
    "C:/ns3-dev-link/src/nr/model/nr-amc.h"
    "C:/ns3-dev-link/src/nr/model/nr-anr.h"
    "C:/ns3-dev-link/src/nr/model/nr-anr-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-as-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-asn1-header.h"
    "C:/ns3-dev-link/src/nr/model/nr-cb-two-port.h"
    "C:/ns3-dev-link/src/nr/model/nr-cb-type-one-sp.h"
    "C:/ns3-dev-link/src/nr/model/nr-cb-type-one.h"
    "C:/ns3-dev-link/src/nr/model/nr-ccm-mac-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-ccm-rrc-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-ch-access-manager.h"
    "C:/ns3-dev-link/src/nr/model/nr-chunk-processor.h"
    "C:/ns3-dev-link/src/nr/model/nr-common.h"
    "C:/ns3-dev-link/src/nr/model/nr-component-carrier.h"
    "C:/ns3-dev-link/src/nr/model/nr-control-messages.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-cc-t1.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-cc-t2.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-cc.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-error-model.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-ir-t1.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-ir-t2.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-ir.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-t1.h"
    "C:/ns3-dev-link/src/nr/model/nr-eesm-t2.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-gnb-application.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-gnb-s1-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-gtpc-header.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-gtpu-header.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-mme-application.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-pgw-application.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-s11-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-s1ap-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-sgw-application.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-tft-classifier.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-tft.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-ue-nas.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-x2-header.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-x2-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-epc-x2.h"
    "C:/ns3-dev-link/src/nr/model/nr-eps-bearer-tag.h"
    "C:/ns3-dev-link/src/nr/model/nr-eps-bearer.h"
    "C:/ns3-dev-link/src/nr/model/nr-error-model.h"
    "C:/ns3-dev-link/src/nr/model/nr-fh-control.h"
    "C:/ns3-dev-link/src/nr/model/nr-fh-sched-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-fh-phy-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-gnb-cmac-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-gnb-component-carrier-manager.h"
    "C:/ns3-dev-link/src/nr/model/nr-gnb-cphy-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-gnb-mac.h"
    "C:/ns3-dev-link/src/nr/model/nr-gnb-net-device.h"
    "C:/ns3-dev-link/src/nr/model/nr-gnb-phy.h"
    "C:/ns3-dev-link/src/nr/model/nr-gnb-rrc.h"
    "C:/ns3-dev-link/src/nr/model/nr-handover-algorithm.h"
    "C:/ns3-dev-link/src/nr/model/nr-handover-management-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-harq-phy.h"
    "C:/ns3-dev-link/src/nr/model/nr-interference-base.h"
    "C:/ns3-dev-link/src/nr/model/nr-interference.h"
    "C:/ns3-dev-link/src/nr/model/nr-lte-amc.h"
    "C:/ns3-dev-link/src/nr/model/nr-lte-mi-error-model.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-csched-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-harq-process.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-harq-vector.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-header-fs-dl.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-header-fs-ul.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-header-fs.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-header-vs-dl.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-header-vs-ul.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-header-vs.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-pdu-info.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-sched-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-cqi-management.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-harq-rr.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-lc-alg.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-lc-qos.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-lc-rr.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-lcg.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ns3.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ofdma-mr.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ofdma-pf.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ofdma-qos.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ofdma-rr.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ofdma.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-srs-default.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-srs.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-tdma-mr.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-tdma-pf.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-tdma-qos.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-tdma-rr.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-tdma.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ue-info-mr.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ue-info-pf.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ue-info-qos.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ue-info-rr.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler-ue-info.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-scheduler.h"
    "C:/ns3-dev-link/src/nr/model/nr-mac-short-bsr-ce.h"
    "C:/ns3-dev-link/src/nr/model/nr-mimo-chunk-processor.h"
    "C:/ns3-dev-link/src/nr/model/nr-mimo-matrices.h"
    "C:/ns3-dev-link/src/nr/model/nr-mimo-signal.h"
    "C:/ns3-dev-link/src/nr/model/nr-net-device.h"
    "C:/ns3-dev-link/src/nr/model/nr-no-op-component-carrier-manager.h"
    "C:/ns3-dev-link/src/nr/model/nr-no-op-handover-algorithm.h"
    "C:/ns3-dev-link/src/nr/model/nr-pdcp-header.h"
    "C:/ns3-dev-link/src/nr/model/nr-pdcp-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-pdcp-tag.h"
    "C:/ns3-dev-link/src/nr/model/nr-pdcp.h"
    "C:/ns3-dev-link/src/nr/model/nr-phy-mac-common.h"
    "C:/ns3-dev-link/src/nr/model/nr-phy-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-phy-tag.h"
    "C:/ns3-dev-link/src/nr/model/nr-phy.h"
    "C:/ns3-dev-link/src/nr/model/nr-pm-search-full.h"
    "C:/ns3-dev-link/src/nr/model/nr-pm-search.h"
    "C:/ns3-dev-link/src/nr/model/nr-radio-bearer-info.h"
    "C:/ns3-dev-link/src/nr/model/nr-radio-bearer-tag.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-am-header.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-am.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-header.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-sdu-status-tag.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-sequence-number.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-tag.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-tm.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc-um.h"
    "C:/ns3-dev-link/src/nr/model/nr-rlc.h"
    "C:/ns3-dev-link/src/nr/model/nr-rrc-header.h"
    "C:/ns3-dev-link/src/nr/model/nr-rrc-protocol-ideal.h"
    "C:/ns3-dev-link/src/nr/model/nr-rrc-protocol-real.h"
    "C:/ns3-dev-link/src/nr/model/nr-rrc-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-simple-ue-component-carrier-manager.h"
    "C:/ns3-dev-link/src/nr/model/nr-spectrum-phy.h"
    "C:/ns3-dev-link/src/nr/model/nr-spectrum-signal-parameters.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-ccm-rrc-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-cmac-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-component-carrier-manager.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-cphy-sap.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-mac.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-net-device.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-phy.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-power-control.h"
    "C:/ns3-dev-link/src/nr/model/nr-ue-rrc.h"
    "C:/ns3-dev-link/src/nr/model/nr-vendor-specific-parameters.h"
    "C:/ns3-dev-link/src/nr/model/realistic-beamforming-algorithm.h"
    "C:/ns3-dev-link/src/nr/model/realistic-bf-manager.h"
    "C:/ns3-dev-link/src/nr/model/sfnsf.h"
    "C:/ns3-dev-link/src/nr/utils/distance-based-three-gpp-spectrum-propagation-loss-model.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/helper/traffic-generator-helper.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/helper/xr-traffic-mixer-helper.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator-3gpp-audio-data.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator-3gpp-generic-video.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator-3gpp-pose-control.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator-ftp-single.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator-ngmn-ftp-multi.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator-ngmn-gaming.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator-ngmn-video.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator-ngmn-voip.h"
    "C:/ns3-dev-link/src/nr/utils/traffic-generators/model/traffic-generator.h"
    "C:/ns3-dev-link/build/include/ns3/nr-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr-debug/src/nr/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
