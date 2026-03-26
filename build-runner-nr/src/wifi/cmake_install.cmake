# Install script for directory: C:/ns3-dev-link/src/wifi

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
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-wifi-relwithdebinfo.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-wifi-relwithdebinfo.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-wifi-relwithdebinfo.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-wifi-relwithdebinfo.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-wifi-relwithdebinfo.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/wifi/helper/athstats-helper.h"
    "C:/ns3-dev-link/src/wifi/helper/spectrum-wifi-helper.h"
    "C:/ns3-dev-link/src/wifi/helper/wifi-co-trace-helper.h"
    "C:/ns3-dev-link/src/wifi/helper/wifi-helper.h"
    "C:/ns3-dev-link/src/wifi/helper/wifi-mac-helper.h"
    "C:/ns3-dev-link/src/wifi/helper/wifi-radio-energy-model-helper.h"
    "C:/ns3-dev-link/src/wifi/helper/yans-wifi-helper.h"
    "C:/ns3-dev-link/src/wifi/helper/wifi-phy-rx-trace-helper.h"
    "C:/ns3-dev-link/src/wifi/helper/wifi-tx-stats-helper.h"
    "C:/ns3-dev-link/src/wifi/model/addba-extension.h"
    "C:/ns3-dev-link/src/wifi/model/adhoc-wifi-mac.h"
    "C:/ns3-dev-link/src/wifi/model/ampdu-subframe-header.h"
    "C:/ns3-dev-link/src/wifi/model/ampdu-tag.h"
    "C:/ns3-dev-link/src/wifi/model/amsdu-subframe-header.h"
    "C:/ns3-dev-link/src/wifi/model/ap-wifi-mac.h"
    "C:/ns3-dev-link/src/wifi/model/block-ack-agreement.h"
    "C:/ns3-dev-link/src/wifi/model/block-ack-manager.h"
    "C:/ns3-dev-link/src/wifi/model/block-ack-type.h"
    "C:/ns3-dev-link/src/wifi/model/block-ack-window.h"
    "C:/ns3-dev-link/src/wifi/model/capability-information.h"
    "C:/ns3-dev-link/src/wifi/model/channel-access-manager.h"
    "C:/ns3-dev-link/src/wifi/model/ctrl-headers.h"
    "C:/ns3-dev-link/src/wifi/model/edca-parameter-set.h"
    "C:/ns3-dev-link/src/wifi/model/eht/advanced-ap-emlsr-manager.h"
    "C:/ns3-dev-link/src/wifi/model/eht/advanced-emlsr-manager.h"
    "C:/ns3-dev-link/src/wifi/model/eht/ap-emlsr-manager.h"
    "C:/ns3-dev-link/src/wifi/model/eht/common-info-basic-mle.h"
    "C:/ns3-dev-link/src/wifi/model/eht/common-info-probe-req-mle.h"
    "C:/ns3-dev-link/src/wifi/model/eht/default-ap-emlsr-manager.h"
    "C:/ns3-dev-link/src/wifi/model/eht/default-emlsr-manager.h"
    "C:/ns3-dev-link/src/wifi/model/eht/eht-capabilities.h"
    "C:/ns3-dev-link/src/wifi/model/eht/eht-configuration.h"
    "C:/ns3-dev-link/src/wifi/model/eht/eht-frame-exchange-manager.h"
    "C:/ns3-dev-link/src/wifi/model/eht/eht-operation.h"
    "C:/ns3-dev-link/src/wifi/model/eht/tid-to-link-mapping-element.h"
    "C:/ns3-dev-link/src/wifi/model/eht/eht-phy.h"
    "C:/ns3-dev-link/src/wifi/model/eht/eht-ppdu.h"
    "C:/ns3-dev-link/src/wifi/model/eht/eht-ru.h"
    "C:/ns3-dev-link/src/wifi/model/eht/emlsr-manager.h"
    "C:/ns3-dev-link/src/wifi/model/eht/multi-link-element.h"
    "C:/ns3-dev-link/src/wifi/model/error-rate-model.h"
    "C:/ns3-dev-link/src/wifi/model/extended-capabilities.h"
    "C:/ns3-dev-link/src/wifi/model/fcfs-wifi-queue-scheduler.h"
    "C:/ns3-dev-link/src/wifi/model/frame-capture-model.h"
    "C:/ns3-dev-link/src/wifi/model/frame-exchange-manager.h"
    "C:/ns3-dev-link/src/wifi/model/gcr-group-address.h"
    "C:/ns3-dev-link/src/wifi/model/gcr-manager.h"
    "C:/ns3-dev-link/src/wifi/model/he/constant-obss-pd-algorithm.h"
    "C:/ns3-dev-link/src/wifi/model/he/he-6ghz-band-capabilities.h"
    "C:/ns3-dev-link/src/wifi/model/he/he-capabilities.h"
    "C:/ns3-dev-link/src/wifi/model/he/he-configuration.h"
    "C:/ns3-dev-link/src/wifi/model/he/he-frame-exchange-manager.h"
    "C:/ns3-dev-link/src/wifi/model/he/he-operation.h"
    "C:/ns3-dev-link/src/wifi/model/he/he-phy.h"
    "C:/ns3-dev-link/src/wifi/model/he/he-ppdu.h"
    "C:/ns3-dev-link/src/wifi/model/he/he-ru.h"
    "C:/ns3-dev-link/src/wifi/model/he/mu-edca-parameter-set.h"
    "C:/ns3-dev-link/src/wifi/model/he/mu-snr-tag.h"
    "C:/ns3-dev-link/src/wifi/model/he/multi-user-scheduler.h"
    "C:/ns3-dev-link/src/wifi/model/he/obss-pd-algorithm.h"
    "C:/ns3-dev-link/src/wifi/model/he/rr-multi-user-scheduler.h"
    "C:/ns3-dev-link/src/wifi/model/ht/ht-capabilities.h"
    "C:/ns3-dev-link/src/wifi/model/ht/ht-configuration.h"
    "C:/ns3-dev-link/src/wifi/model/ht/ht-frame-exchange-manager.h"
    "C:/ns3-dev-link/src/wifi/model/ht/ht-operation.h"
    "C:/ns3-dev-link/src/wifi/model/ht/ht-phy.h"
    "C:/ns3-dev-link/src/wifi/model/ht/ht-ppdu.h"
    "C:/ns3-dev-link/src/wifi/model/interference-helper.h"
    "C:/ns3-dev-link/src/wifi/model/mac-rx-middle.h"
    "C:/ns3-dev-link/src/wifi/model/mac-tx-middle.h"
    "C:/ns3-dev-link/src/wifi/model/mgt-action-headers.h"
    "C:/ns3-dev-link/src/wifi/model/mgt-headers.h"
    "C:/ns3-dev-link/src/wifi/model/mpdu-aggregator.h"
    "C:/ns3-dev-link/src/wifi/model/msdu-aggregator.h"
    "C:/ns3-dev-link/src/wifi/model/nist-error-rate-model.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/dsss-error-rate-model.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/dsss-parameter-set.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/dsss-phy.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/dsss-ppdu.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/erp-information.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/erp-ofdm-phy.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/erp-ofdm-ppdu.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/ofdm-phy.h"
    "C:/ns3-dev-link/src/wifi/model/non-ht/ofdm-ppdu.h"
    "C:/ns3-dev-link/src/wifi/model/non-inheritance.h"
    "C:/ns3-dev-link/src/wifi/model/originator-block-ack-agreement.h"
    "C:/ns3-dev-link/src/wifi/model/phy-entity.h"
    "C:/ns3-dev-link/src/wifi/model/preamble-detection-model.h"
    "C:/ns3-dev-link/src/wifi/model/qos-frame-exchange-manager.h"
    "C:/ns3-dev-link/src/wifi/model/qos-txop.h"
    "C:/ns3-dev-link/src/wifi/model/qos-utils.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/aarf-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/aarfcd-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/amrr-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/aparf-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/arf-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/cara-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/constant-rate-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/ideal-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/minstrel-ht-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/minstrel-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/onoe-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/parf-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/rraa-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/rrpaa-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/rate-control/thompson-sampling-wifi-manager.h"
    "C:/ns3-dev-link/src/wifi/model/recipient-block-ack-agreement.h"
    "C:/ns3-dev-link/src/wifi/model/reduced-neighbor-report.h"
    "C:/ns3-dev-link/src/wifi/model/reference/error-rate-tables.h"
    "C:/ns3-dev-link/src/wifi/model/simple-frame-capture-model.h"
    "C:/ns3-dev-link/src/wifi/model/snr-tag.h"
    "C:/ns3-dev-link/src/wifi/model/spectrum-wifi-phy.h"
    "C:/ns3-dev-link/src/wifi/model/ssid.h"
    "C:/ns3-dev-link/src/wifi/model/sta-wifi-mac.h"
    "C:/ns3-dev-link/src/wifi/model/status-code.h"
    "C:/ns3-dev-link/src/wifi/model/supported-rates.h"
    "C:/ns3-dev-link/src/wifi/model/table-based-error-rate-model.h"
    "C:/ns3-dev-link/src/wifi/model/threshold-preamble-detection-model.h"
    "C:/ns3-dev-link/src/wifi/model/tim.h"
    "C:/ns3-dev-link/src/wifi/model/txop.h"
    "C:/ns3-dev-link/src/wifi/model/vht/vht-capabilities.h"
    "C:/ns3-dev-link/src/wifi/model/vht/vht-configuration.h"
    "C:/ns3-dev-link/src/wifi/model/vht/vht-frame-exchange-manager.h"
    "C:/ns3-dev-link/src/wifi/model/vht/vht-operation.h"
    "C:/ns3-dev-link/src/wifi/model/vht/vht-phy.h"
    "C:/ns3-dev-link/src/wifi/model/vht/vht-ppdu.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-ack-manager.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-acknowledgment.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-assoc-manager.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-bandwidth-filter.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-constants.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-default-ack-manager.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-default-assoc-manager.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-default-gcr-manager.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-default-protection-manager.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-information-element.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mac-header.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mac-queue-container.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mac-queue-elem.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mac-queue-scheduler-impl.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mac-queue-scheduler.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mac-queue.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mac-trailer.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mac.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mgt-header.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mode.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-mpdu.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-net-device.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-opt-field.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-phy-band.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-phy-common.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-phy-listener.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-phy-operating-channel.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-phy-state-helper.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-phy-state.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-phy.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-ppdu.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-protection-manager.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-protection.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-psdu.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-radio-energy-model.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-remote-station-info.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-remote-station-manager.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-ru.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-spectrum-phy-interface.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-spectrum-signal-parameters.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-standards.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-tx-current-model.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-tx-parameters.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-tx-timer.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-tx-vector.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-types.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-units.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-utils.h"
    "C:/ns3-dev-link/src/wifi/model/yans-error-rate-model.h"
    "C:/ns3-dev-link/src/wifi/model/yans-wifi-channel.h"
    "C:/ns3-dev-link/src/wifi/model/yans-wifi-phy.h"
    "C:/ns3-dev-link/src/wifi/model/wifi-spectrum-value-helper.h"
    "C:/ns3-dev-link/src/wifi/test/wifi-mlo-test.h"
    "C:/ns3-dev-link/build/include/ns3/wifi-module.h"
    "C:/ns3-dev-link/build/include/ns3/wifi-export.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/wifi/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
