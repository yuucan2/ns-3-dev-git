# Install script for directory: C:/ns3-dev-link/src/uan

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-uan-relwithdebinfo.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-uan-relwithdebinfo.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-uan-relwithdebinfo.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-uan-relwithdebinfo.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-uan-relwithdebinfo.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/uan/helper/acoustic-modem-energy-model-helper.h"
    "C:/ns3-dev-link/src/uan/helper/uan-helper.h"
    "C:/ns3-dev-link/src/uan/model/acoustic-modem-energy-model.h"
    "C:/ns3-dev-link/src/uan/model/uan-channel.h"
    "C:/ns3-dev-link/src/uan/model/uan-header-common.h"
    "C:/ns3-dev-link/src/uan/model/uan-header-rc.h"
    "C:/ns3-dev-link/src/uan/model/uan-mac-aloha.h"
    "C:/ns3-dev-link/src/uan/model/uan-mac-cw.h"
    "C:/ns3-dev-link/src/uan/model/uan-mac-rc-gw.h"
    "C:/ns3-dev-link/src/uan/model/uan-mac-rc.h"
    "C:/ns3-dev-link/src/uan/model/uan-mac.h"
    "C:/ns3-dev-link/src/uan/model/uan-net-device.h"
    "C:/ns3-dev-link/src/uan/model/uan-noise-model-default.h"
    "C:/ns3-dev-link/src/uan/model/uan-noise-model.h"
    "C:/ns3-dev-link/src/uan/model/uan-phy-dual.h"
    "C:/ns3-dev-link/src/uan/model/uan-phy-gen.h"
    "C:/ns3-dev-link/src/uan/model/uan-phy.h"
    "C:/ns3-dev-link/src/uan/model/uan-prop-model-ideal.h"
    "C:/ns3-dev-link/src/uan/model/uan-prop-model-thorp.h"
    "C:/ns3-dev-link/src/uan/model/uan-prop-model.h"
    "C:/ns3-dev-link/src/uan/model/uan-transducer-hd.h"
    "C:/ns3-dev-link/src/uan/model/uan-transducer.h"
    "C:/ns3-dev-link/src/uan/model/uan-tx-mode.h"
    "C:/ns3-dev-link/build/include/ns3/uan-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/uan/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
