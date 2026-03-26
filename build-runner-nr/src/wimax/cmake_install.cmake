# Install script for directory: C:/ns3-dev-link/src/wimax

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-wimax-relwithdebinfo.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-wimax-relwithdebinfo.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-wimax-relwithdebinfo.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-wimax-relwithdebinfo.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-wimax-relwithdebinfo.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/wimax/helper/wimax-helper.h"
    "C:/ns3-dev-link/src/wimax/model/wimax-channel.h"
    "C:/ns3-dev-link/src/wimax/model/wimax-net-device.h"
    "C:/ns3-dev-link/src/wimax/model/bs-net-device.h"
    "C:/ns3-dev-link/src/wimax/model/ss-net-device.h"
    "C:/ns3-dev-link/src/wimax/model/cid.h"
    "C:/ns3-dev-link/src/wimax/model/cid-factory.h"
    "C:/ns3-dev-link/src/wimax/model/ofdm-downlink-frame-prefix.h"
    "C:/ns3-dev-link/src/wimax/model/wimax-connection.h"
    "C:/ns3-dev-link/src/wimax/model/ss-record.h"
    "C:/ns3-dev-link/src/wimax/model/mac-messages.h"
    "C:/ns3-dev-link/src/wimax/model/dl-mac-messages.h"
    "C:/ns3-dev-link/src/wimax/model/ul-mac-messages.h"
    "C:/ns3-dev-link/src/wimax/model/wimax-phy.h"
    "C:/ns3-dev-link/src/wimax/model/simple-ofdm-wimax-phy.h"
    "C:/ns3-dev-link/src/wimax/model/simple-ofdm-wimax-channel.h"
    "C:/ns3-dev-link/src/wimax/model/send-params.h"
    "C:/ns3-dev-link/src/wimax/model/service-flow.h"
    "C:/ns3-dev-link/src/wimax/model/ss-manager.h"
    "C:/ns3-dev-link/src/wimax/model/connection-manager.h"
    "C:/ns3-dev-link/src/wimax/model/wimax-mac-header.h"
    "C:/ns3-dev-link/src/wimax/model/wimax-mac-queue.h"
    "C:/ns3-dev-link/src/wimax/model/crc8.h"
    "C:/ns3-dev-link/src/wimax/model/service-flow-manager.h"
    "C:/ns3-dev-link/src/wimax/model/bs-uplink-scheduler.h"
    "C:/ns3-dev-link/src/wimax/model/bs-uplink-scheduler-simple.h"
    "C:/ns3-dev-link/src/wimax/model/bs-uplink-scheduler-mbqos.h"
    "C:/ns3-dev-link/src/wimax/model/bs-uplink-scheduler-rtps.h"
    "C:/ns3-dev-link/src/wimax/model/ul-job.h"
    "C:/ns3-dev-link/src/wimax/model/bs-scheduler.h"
    "C:/ns3-dev-link/src/wimax/model/bs-scheduler-simple.h"
    "C:/ns3-dev-link/src/wimax/model/bs-scheduler-rtps.h"
    "C:/ns3-dev-link/src/wimax/model/service-flow-record.h"
    "C:/ns3-dev-link/src/wimax/model/snr-to-block-error-rate-record.h"
    "C:/ns3-dev-link/src/wimax/model/snr-to-block-error-rate-manager.h"
    "C:/ns3-dev-link/src/wimax/model/simple-ofdm-send-param.h"
    "C:/ns3-dev-link/src/wimax/model/ss-service-flow-manager.h"
    "C:/ns3-dev-link/src/wimax/model/bs-service-flow-manager.h"
    "C:/ns3-dev-link/src/wimax/model/cs-parameters.h"
    "C:/ns3-dev-link/src/wimax/model/ipcs-classifier-record.h"
    "C:/ns3-dev-link/src/wimax/model/wimax-tlv.h"
    "C:/ns3-dev-link/src/wimax/model/ipcs-classifier.h"
    "C:/ns3-dev-link/src/wimax/model/bvec.h"
    "C:/ns3-dev-link/src/wimax/model/wimax-mac-to-mac-header.h"
    "C:/ns3-dev-link/build/include/ns3/wimax-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/wimax/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
