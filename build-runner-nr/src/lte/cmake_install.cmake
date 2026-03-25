# Install script for directory: C:/ns3-dev-link/src/lte

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
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build-runner-nr/lib/libns3-dev-lte.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build-runner-nr/lib/libns3-dev-lte.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-lte.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-lte.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-lte.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/lte/helper/cc-helper.h"
    "C:/ns3-dev-link/src/lte/helper/epc-helper.h"
    "C:/ns3-dev-link/src/lte/helper/lte-global-pathloss-database.h"
    "C:/ns3-dev-link/src/lte/helper/lte-helper.h"
    "C:/ns3-dev-link/src/lte/helper/lte-hex-grid-enb-topology-helper.h"
    "C:/ns3-dev-link/src/lte/helper/lte-stats-calculator.h"
    "C:/ns3-dev-link/src/lte/helper/mac-stats-calculator.h"
    "C:/ns3-dev-link/src/lte/helper/no-backhaul-epc-helper.h"
    "C:/ns3-dev-link/src/lte/helper/phy-rx-stats-calculator.h"
    "C:/ns3-dev-link/src/lte/helper/phy-stats-calculator.h"
    "C:/ns3-dev-link/src/lte/helper/phy-tx-stats-calculator.h"
    "C:/ns3-dev-link/src/lte/helper/point-to-point-epc-helper.h"
    "C:/ns3-dev-link/src/lte/helper/radio-bearer-stats-calculator.h"
    "C:/ns3-dev-link/src/lte/helper/radio-bearer-stats-connector.h"
    "C:/ns3-dev-link/src/lte/helper/radio-environment-map-helper.h"
    "C:/ns3-dev-link/src/lte/model/a2-a4-rsrq-handover-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/a3-rsrp-handover-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/component-carrier-enb.h"
    "C:/ns3-dev-link/src/lte/model/component-carrier-ue.h"
    "C:/ns3-dev-link/src/lte/model/component-carrier.h"
    "C:/ns3-dev-link/src/lte/model/cqa-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/epc-enb-application.h"
    "C:/ns3-dev-link/src/lte/model/epc-enb-s1-sap.h"
    "C:/ns3-dev-link/src/lte/model/epc-gtpc-header.h"
    "C:/ns3-dev-link/src/lte/model/epc-gtpu-header.h"
    "C:/ns3-dev-link/src/lte/model/epc-mme-application.h"
    "C:/ns3-dev-link/src/lte/model/epc-pgw-application.h"
    "C:/ns3-dev-link/src/lte/model/epc-s11-sap.h"
    "C:/ns3-dev-link/src/lte/model/epc-s1ap-sap.h"
    "C:/ns3-dev-link/src/lte/model/epc-sgw-application.h"
    "C:/ns3-dev-link/src/lte/model/epc-tft-classifier.h"
    "C:/ns3-dev-link/src/lte/model/epc-tft.h"
    "C:/ns3-dev-link/src/lte/model/epc-ue-nas.h"
    "C:/ns3-dev-link/src/lte/model/epc-x2-header.h"
    "C:/ns3-dev-link/src/lte/model/epc-x2-sap.h"
    "C:/ns3-dev-link/src/lte/model/epc-x2.h"
    "C:/ns3-dev-link/src/lte/model/eps-bearer-tag.h"
    "C:/ns3-dev-link/src/lte/model/eps-bearer.h"
    "C:/ns3-dev-link/src/lte/model/fdbet-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/fdmt-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/fdtbfq-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/ff-mac-common.h"
    "C:/ns3-dev-link/src/lte/model/ff-mac-csched-sap.h"
    "C:/ns3-dev-link/src/lte/model/ff-mac-sched-sap.h"
    "C:/ns3-dev-link/src/lte/model/ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/lte-amc.h"
    "C:/ns3-dev-link/src/lte/model/lte-anr-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-anr.h"
    "C:/ns3-dev-link/src/lte/model/lte-as-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-asn1-header.h"
    "C:/ns3-dev-link/src/lte/model/lte-ccm-mac-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-ccm-rrc-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-chunk-processor.h"
    "C:/ns3-dev-link/src/lte/model/lte-common.h"
    "C:/ns3-dev-link/src/lte/model/lte-control-messages.h"
    "C:/ns3-dev-link/src/lte/model/lte-enb-cmac-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-enb-component-carrier-manager.h"
    "C:/ns3-dev-link/src/lte/model/lte-enb-cphy-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-enb-mac.h"
    "C:/ns3-dev-link/src/lte/model/lte-enb-net-device.h"
    "C:/ns3-dev-link/src/lte/model/lte-enb-phy-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-enb-phy.h"
    "C:/ns3-dev-link/src/lte/model/lte-enb-rrc.h"
    "C:/ns3-dev-link/src/lte/model/lte-ffr-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-ffr-distributed-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-ffr-enhanced-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-ffr-rrc-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-ffr-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-ffr-soft-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-fr-hard-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-fr-no-op-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-fr-soft-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-fr-strict-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-handover-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/lte-handover-management-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-harq-phy.h"
    "C:/ns3-dev-link/src/lte/model/lte-interference.h"
    "C:/ns3-dev-link/src/lte/model/lte-mac-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-mi-error-model.h"
    "C:/ns3-dev-link/src/lte/model/lte-net-device.h"
    "C:/ns3-dev-link/src/lte/model/lte-pdcp-header.h"
    "C:/ns3-dev-link/src/lte/model/lte-pdcp-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-pdcp-tag.h"
    "C:/ns3-dev-link/src/lte/model/lte-pdcp.h"
    "C:/ns3-dev-link/src/lte/model/lte-phy-tag.h"
    "C:/ns3-dev-link/src/lte/model/lte-phy.h"
    "C:/ns3-dev-link/src/lte/model/lte-radio-bearer-info.h"
    "C:/ns3-dev-link/src/lte/model/lte-radio-bearer-tag.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-am-header.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-am.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-header.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-sdu-status-tag.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-sequence-number.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-tag.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-tm.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc-um.h"
    "C:/ns3-dev-link/src/lte/model/lte-rlc.h"
    "C:/ns3-dev-link/src/lte/model/lte-rrc-header.h"
    "C:/ns3-dev-link/src/lte/model/lte-rrc-protocol-ideal.h"
    "C:/ns3-dev-link/src/lte/model/lte-rrc-protocol-real.h"
    "C:/ns3-dev-link/src/lte/model/lte-rrc-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-spectrum-phy.h"
    "C:/ns3-dev-link/src/lte/model/lte-spectrum-signal-parameters.h"
    "C:/ns3-dev-link/src/lte/model/lte-spectrum-value-helper.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-ccm-rrc-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-cmac-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-component-carrier-manager.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-cphy-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-mac.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-net-device.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-phy-sap.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-phy.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-power-control.h"
    "C:/ns3-dev-link/src/lte/model/lte-ue-rrc.h"
    "C:/ns3-dev-link/src/lte/model/lte-vendor-specific-parameters.h"
    "C:/ns3-dev-link/src/lte/model/no-op-component-carrier-manager.h"
    "C:/ns3-dev-link/src/lte/model/no-op-handover-algorithm.h"
    "C:/ns3-dev-link/src/lte/model/pf-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/pss-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/rem-spectrum-phy.h"
    "C:/ns3-dev-link/src/lte/model/rr-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/simple-ue-component-carrier-manager.h"
    "C:/ns3-dev-link/src/lte/model/tdbet-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/tdmt-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/tdtbfq-ff-mac-scheduler.h"
    "C:/ns3-dev-link/src/lte/model/tta-ff-mac-scheduler.h"
    "C:/ns3-dev-link/build-runner-nr/include/ns3/lte-module.h"
    "C:/ns3-dev-link/build-runner-nr/include/ns3/lte-export.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/lte/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
