# Install script for directory: C:/ns3-dev-link/src/network

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-network-relwithdebinfo.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-network-relwithdebinfo.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-network-relwithdebinfo.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-network-relwithdebinfo.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-network-relwithdebinfo.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/network/helper/application-container.h"
    "C:/ns3-dev-link/src/network/helper/application-helper.h"
    "C:/ns3-dev-link/src/network/helper/delay-jitter-estimation.h"
    "C:/ns3-dev-link/src/network/helper/net-device-container.h"
    "C:/ns3-dev-link/src/network/helper/node-container.h"
    "C:/ns3-dev-link/src/network/helper/packet-socket-helper.h"
    "C:/ns3-dev-link/src/network/helper/simple-net-device-helper.h"
    "C:/ns3-dev-link/src/network/helper/trace-helper.h"
    "C:/ns3-dev-link/src/network/model/address.h"
    "C:/ns3-dev-link/src/network/model/application.h"
    "C:/ns3-dev-link/src/network/model/buffer.h"
    "C:/ns3-dev-link/src/network/model/byte-tag-list.h"
    "C:/ns3-dev-link/src/network/model/channel-list.h"
    "C:/ns3-dev-link/src/network/model/channel.h"
    "C:/ns3-dev-link/src/network/model/chunk.h"
    "C:/ns3-dev-link/src/network/model/header.h"
    "C:/ns3-dev-link/src/network/model/net-device.h"
    "C:/ns3-dev-link/src/network/model/nix-vector.h"
    "C:/ns3-dev-link/src/network/model/node-list.h"
    "C:/ns3-dev-link/src/network/model/node.h"
    "C:/ns3-dev-link/src/network/model/packet-metadata.h"
    "C:/ns3-dev-link/src/network/model/packet-tag-list.h"
    "C:/ns3-dev-link/src/network/model/packet.h"
    "C:/ns3-dev-link/src/network/model/socket-factory.h"
    "C:/ns3-dev-link/src/network/model/socket.h"
    "C:/ns3-dev-link/src/network/model/tag-buffer.h"
    "C:/ns3-dev-link/src/network/model/tag.h"
    "C:/ns3-dev-link/src/network/model/trailer.h"
    "C:/ns3-dev-link/src/network/utils/address-utils.h"
    "C:/ns3-dev-link/src/network/utils/bit-deserializer.h"
    "C:/ns3-dev-link/src/network/utils/bit-serializer.h"
    "C:/ns3-dev-link/src/network/utils/crc32.h"
    "C:/ns3-dev-link/src/network/utils/data-rate.h"
    "C:/ns3-dev-link/src/network/utils/drop-tail-queue.h"
    "C:/ns3-dev-link/src/network/utils/dynamic-queue-limits.h"
    "C:/ns3-dev-link/src/network/utils/error-channel.h"
    "C:/ns3-dev-link/src/network/utils/error-model.h"
    "C:/ns3-dev-link/src/network/utils/ethernet-header.h"
    "C:/ns3-dev-link/src/network/utils/ethernet-trailer.h"
    "C:/ns3-dev-link/src/network/utils/flow-id-tag.h"
    "C:/ns3-dev-link/src/network/utils/generic-phy.h"
    "C:/ns3-dev-link/src/network/utils/header-serialization-test.h"
    "C:/ns3-dev-link/src/network/utils/inet-socket-address.h"
    "C:/ns3-dev-link/src/network/utils/inet6-socket-address.h"
    "C:/ns3-dev-link/src/network/utils/ipv4-address.h"
    "C:/ns3-dev-link/src/network/utils/ipv6-address.h"
    "C:/ns3-dev-link/src/network/utils/llc-snap-header.h"
    "C:/ns3-dev-link/src/network/utils/lollipop-counter.h"
    "C:/ns3-dev-link/src/network/utils/mac16-address.h"
    "C:/ns3-dev-link/src/network/utils/mac48-address.h"
    "C:/ns3-dev-link/src/network/utils/mac64-address.h"
    "C:/ns3-dev-link/src/network/utils/mac8-address.h"
    "C:/ns3-dev-link/src/network/utils/net-device-queue-interface.h"
    "C:/ns3-dev-link/src/network/utils/output-stream-wrapper.h"
    "C:/ns3-dev-link/src/network/utils/packet-burst.h"
    "C:/ns3-dev-link/src/network/utils/packet-data-calculators.h"
    "C:/ns3-dev-link/src/network/utils/packet-probe.h"
    "C:/ns3-dev-link/src/network/utils/packet-socket-address.h"
    "C:/ns3-dev-link/src/network/utils/packet-socket-client.h"
    "C:/ns3-dev-link/src/network/utils/packet-socket-factory.h"
    "C:/ns3-dev-link/src/network/utils/packet-socket-server.h"
    "C:/ns3-dev-link/src/network/utils/packet-socket.h"
    "C:/ns3-dev-link/src/network/utils/packetbb.h"
    "C:/ns3-dev-link/src/network/utils/pcap-file-wrapper.h"
    "C:/ns3-dev-link/src/network/utils/pcap-file.h"
    "C:/ns3-dev-link/src/network/utils/pcap-test.h"
    "C:/ns3-dev-link/src/network/utils/queue-fwd.h"
    "C:/ns3-dev-link/src/network/utils/queue-item.h"
    "C:/ns3-dev-link/src/network/utils/queue-limits.h"
    "C:/ns3-dev-link/src/network/utils/queue-size.h"
    "C:/ns3-dev-link/src/network/utils/queue.h"
    "C:/ns3-dev-link/src/network/utils/radiotap-header.h"
    "C:/ns3-dev-link/src/network/utils/sequence-number.h"
    "C:/ns3-dev-link/src/network/utils/simple-channel.h"
    "C:/ns3-dev-link/src/network/utils/simple-net-device.h"
    "C:/ns3-dev-link/src/network/utils/sll-header.h"
    "C:/ns3-dev-link/src/network/utils/timestamp-tag.h"
    "C:/ns3-dev-link/build/include/ns3/network-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/network/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
