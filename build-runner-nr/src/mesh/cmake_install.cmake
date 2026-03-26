# Install script for directory: C:/ns3-dev-link/src/mesh

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-mesh-relwithdebinfo.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-mesh-relwithdebinfo.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-mesh-relwithdebinfo.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-mesh-relwithdebinfo.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-mesh-relwithdebinfo.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/mesh/helper/dot11s/dot11s-installer.h"
    "C:/ns3-dev-link/src/mesh/helper/flame/flame-installer.h"
    "C:/ns3-dev-link/src/mesh/helper/mesh-helper.h"
    "C:/ns3-dev-link/src/mesh/helper/mesh-stack-installer.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/dot11s-mac-header.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/hwmp-protocol.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/hwmp-rtable.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-beacon-timing.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-configuration.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-id.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-metric-report.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-peer-management.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-peering-protocol.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-perr.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-prep.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-preq.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/ie-dot11s-rann.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/peer-link-frame.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/peer-link.h"
    "C:/ns3-dev-link/src/mesh/model/dot11s/peer-management-protocol.h"
    "C:/ns3-dev-link/src/mesh/model/flame/flame-header.h"
    "C:/ns3-dev-link/src/mesh/model/flame/flame-protocol-mac.h"
    "C:/ns3-dev-link/src/mesh/model/flame/flame-protocol.h"
    "C:/ns3-dev-link/src/mesh/model/flame/flame-rtable.h"
    "C:/ns3-dev-link/src/mesh/model/mesh-information-element-vector.h"
    "C:/ns3-dev-link/src/mesh/model/mesh-l2-routing-protocol.h"
    "C:/ns3-dev-link/src/mesh/model/mesh-point-device.h"
    "C:/ns3-dev-link/src/mesh/model/mesh-wifi-beacon.h"
    "C:/ns3-dev-link/src/mesh/model/mesh-wifi-interface-mac-plugin.h"
    "C:/ns3-dev-link/src/mesh/model/mesh-wifi-interface-mac.h"
    "C:/ns3-dev-link/build/include/ns3/mesh-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/mesh/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
