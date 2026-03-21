# Install script for directory: C:/ns3-dev-link/src/applications

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-applications.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-applications.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-applications.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-applications.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-applications.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/applications/helper/bulk-send-helper.h"
    "C:/ns3-dev-link/src/applications/helper/on-off-helper.h"
    "C:/ns3-dev-link/src/applications/helper/packet-sink-helper.h"
    "C:/ns3-dev-link/src/applications/helper/three-gpp-http-helper.h"
    "C:/ns3-dev-link/src/applications/helper/udp-client-server-helper.h"
    "C:/ns3-dev-link/src/applications/helper/udp-echo-helper.h"
    "C:/ns3-dev-link/src/applications/model/application-packet-probe.h"
    "C:/ns3-dev-link/src/applications/model/bulk-send-application.h"
    "C:/ns3-dev-link/src/applications/model/onoff-application.h"
    "C:/ns3-dev-link/src/applications/model/packet-loss-counter.h"
    "C:/ns3-dev-link/src/applications/model/packet-sink.h"
    "C:/ns3-dev-link/src/applications/model/seq-ts-echo-header.h"
    "C:/ns3-dev-link/src/applications/model/seq-ts-header.h"
    "C:/ns3-dev-link/src/applications/model/seq-ts-size-header.h"
    "C:/ns3-dev-link/src/applications/model/sink-application.h"
    "C:/ns3-dev-link/src/applications/model/source-application.h"
    "C:/ns3-dev-link/src/applications/model/three-gpp-http-client.h"
    "C:/ns3-dev-link/src/applications/model/three-gpp-http-header.h"
    "C:/ns3-dev-link/src/applications/model/three-gpp-http-server.h"
    "C:/ns3-dev-link/src/applications/model/three-gpp-http-variables.h"
    "C:/ns3-dev-link/src/applications/model/udp-client.h"
    "C:/ns3-dev-link/src/applications/model/udp-echo-client.h"
    "C:/ns3-dev-link/src/applications/model/udp-echo-server.h"
    "C:/ns3-dev-link/src/applications/model/udp-server.h"
    "C:/ns3-dev-link/src/applications/model/udp-trace-client.h"
    "C:/ns3-dev-link/build/include/ns3/applications-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/applications/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
