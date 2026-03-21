# Install script for directory: C:/ns3-dev-link/src/internet

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-internet.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-internet.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-internet.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-internet.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-internet.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES "C:/ns3-dev-link/src/internet/model/win32-internet.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/internet/helper/internet-stack-helper.h"
    "C:/ns3-dev-link/src/internet/helper/internet-trace-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv4-address-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv4-global-routing-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv4-interface-container.h"
    "C:/ns3-dev-link/src/internet/helper/ipv4-list-routing-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv4-routing-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv4-static-routing-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv6-address-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv6-interface-container.h"
    "C:/ns3-dev-link/src/internet/helper/ipv6-list-routing-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv6-routing-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ipv6-static-routing-helper.h"
    "C:/ns3-dev-link/src/internet/helper/neighbor-cache-helper.h"
    "C:/ns3-dev-link/src/internet/helper/rip-helper.h"
    "C:/ns3-dev-link/src/internet/helper/ripng-helper.h"
    "C:/ns3-dev-link/src/internet/model/arp-cache.h"
    "C:/ns3-dev-link/src/internet/model/arp-header.h"
    "C:/ns3-dev-link/src/internet/model/arp-l3-protocol.h"
    "C:/ns3-dev-link/src/internet/model/arp-queue-disc-item.h"
    "C:/ns3-dev-link/src/internet/model/candidate-queue.h"
    "C:/ns3-dev-link/src/internet/model/global-route-manager-impl.h"
    "C:/ns3-dev-link/src/internet/model/global-route-manager.h"
    "C:/ns3-dev-link/src/internet/model/global-router-interface.h"
    "C:/ns3-dev-link/src/internet/model/icmpv4-l4-protocol.h"
    "C:/ns3-dev-link/src/internet/model/icmpv4.h"
    "C:/ns3-dev-link/src/internet/model/icmpv6-header.h"
    "C:/ns3-dev-link/src/internet/model/icmpv6-l4-protocol.h"
    "C:/ns3-dev-link/src/internet/model/ip-l4-protocol.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-address-generator.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-end-point-demux.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-end-point.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-global-routing.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-header.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-interface-address.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-interface.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-l3-protocol.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-list-routing.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-packet-filter.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-packet-info-tag.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-packet-probe.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-queue-disc-item.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-raw-socket-factory.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-raw-socket-impl.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-route.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-routing-protocol.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-routing-table-entry.h"
    "C:/ns3-dev-link/src/internet/model/ipv4-static-routing.h"
    "C:/ns3-dev-link/src/internet/model/ipv4.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-address-generator.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-end-point-demux.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-end-point.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-extension-demux.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-extension-header.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-extension.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-header.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-interface-address.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-interface.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-l3-protocol.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-list-routing.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-option-header.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-option.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-packet-filter.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-packet-info-tag.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-packet-probe.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-pmtu-cache.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-queue-disc-item.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-raw-socket-factory.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-route.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-routing-protocol.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-routing-table-entry.h"
    "C:/ns3-dev-link/src/internet/model/ipv6-static-routing.h"
    "C:/ns3-dev-link/src/internet/model/ipv6.h"
    "C:/ns3-dev-link/src/internet/model/loopback-net-device.h"
    "C:/ns3-dev-link/src/internet/model/ndisc-cache.h"
    "C:/ns3-dev-link/src/internet/model/rip-header.h"
    "C:/ns3-dev-link/src/internet/model/rip.h"
    "C:/ns3-dev-link/src/internet/model/ripng-header.h"
    "C:/ns3-dev-link/src/internet/model/ripng.h"
    "C:/ns3-dev-link/src/internet/model/rtt-estimator.h"
    "C:/ns3-dev-link/src/internet/model/tcp-bbr.h"
    "C:/ns3-dev-link/src/internet/model/tcp-bic.h"
    "C:/ns3-dev-link/src/internet/model/tcp-congestion-ops.h"
    "C:/ns3-dev-link/src/internet/model/tcp-cubic.h"
    "C:/ns3-dev-link/src/internet/model/tcp-dctcp.h"
    "C:/ns3-dev-link/src/internet/model/tcp-header.h"
    "C:/ns3-dev-link/src/internet/model/tcp-highspeed.h"
    "C:/ns3-dev-link/src/internet/model/tcp-htcp.h"
    "C:/ns3-dev-link/src/internet/model/tcp-hybla.h"
    "C:/ns3-dev-link/src/internet/model/tcp-illinois.h"
    "C:/ns3-dev-link/src/internet/model/tcp-l4-protocol.h"
    "C:/ns3-dev-link/src/internet/model/tcp-ledbat.h"
    "C:/ns3-dev-link/src/internet/model/tcp-linux-reno.h"
    "C:/ns3-dev-link/src/internet/model/tcp-lp.h"
    "C:/ns3-dev-link/src/internet/model/tcp-option-rfc793.h"
    "C:/ns3-dev-link/src/internet/model/tcp-option-sack-permitted.h"
    "C:/ns3-dev-link/src/internet/model/tcp-option-sack.h"
    "C:/ns3-dev-link/src/internet/model/tcp-option-ts.h"
    "C:/ns3-dev-link/src/internet/model/tcp-option-winscale.h"
    "C:/ns3-dev-link/src/internet/model/tcp-option.h"
    "C:/ns3-dev-link/src/internet/model/tcp-prr-recovery.h"
    "C:/ns3-dev-link/src/internet/model/tcp-rate-ops.h"
    "C:/ns3-dev-link/src/internet/model/tcp-recovery-ops.h"
    "C:/ns3-dev-link/src/internet/model/tcp-rx-buffer.h"
    "C:/ns3-dev-link/src/internet/model/tcp-scalable.h"
    "C:/ns3-dev-link/src/internet/model/tcp-socket-base.h"
    "C:/ns3-dev-link/src/internet/model/tcp-socket-factory.h"
    "C:/ns3-dev-link/src/internet/model/tcp-socket-state.h"
    "C:/ns3-dev-link/src/internet/model/tcp-socket.h"
    "C:/ns3-dev-link/src/internet/model/tcp-tx-buffer.h"
    "C:/ns3-dev-link/src/internet/model/tcp-tx-item.h"
    "C:/ns3-dev-link/src/internet/model/tcp-vegas.h"
    "C:/ns3-dev-link/src/internet/model/tcp-veno.h"
    "C:/ns3-dev-link/src/internet/model/tcp-westwood-plus.h"
    "C:/ns3-dev-link/src/internet/model/tcp-yeah.h"
    "C:/ns3-dev-link/src/internet/model/udp-header.h"
    "C:/ns3-dev-link/src/internet/model/udp-l4-protocol.h"
    "C:/ns3-dev-link/src/internet/model/udp-socket-factory.h"
    "C:/ns3-dev-link/src/internet/model/udp-socket.h"
    "C:/ns3-dev-link/src/internet/model/windowed-filter.h"
    "C:/ns3-dev-link/build/include/ns3/internet-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/internet/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
