# Install script for directory: C:/ns3-dev-link/src/propagation

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-propagation.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-propagation.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-propagation.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-propagation.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-propagation.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/propagation/model/channel-condition-model.h"
    "C:/ns3-dev-link/src/propagation/model/cost231-propagation-loss-model.h"
    "C:/ns3-dev-link/src/propagation/model/itu-r-1411-los-propagation-loss-model.h"
    "C:/ns3-dev-link/src/propagation/model/itu-r-1411-nlos-over-rooftop-propagation-loss-model.h"
    "C:/ns3-dev-link/src/propagation/model/jakes-process.h"
    "C:/ns3-dev-link/src/propagation/model/jakes-propagation-loss-model.h"
    "C:/ns3-dev-link/src/propagation/model/kun-2600-mhz-propagation-loss-model.h"
    "C:/ns3-dev-link/src/propagation/model/okumura-hata-propagation-loss-model.h"
    "C:/ns3-dev-link/src/propagation/model/probabilistic-v2v-channel-condition-model.h"
    "C:/ns3-dev-link/src/propagation/model/propagation-cache.h"
    "C:/ns3-dev-link/src/propagation/model/propagation-delay-model.h"
    "C:/ns3-dev-link/src/propagation/model/propagation-environment.h"
    "C:/ns3-dev-link/src/propagation/model/propagation-loss-model.h"
    "C:/ns3-dev-link/src/propagation/model/three-gpp-propagation-loss-model.h"
    "C:/ns3-dev-link/src/propagation/model/three-gpp-v2v-propagation-loss-model.h"
    "C:/ns3-dev-link/build/include/ns3/propagation-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/propagation/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
