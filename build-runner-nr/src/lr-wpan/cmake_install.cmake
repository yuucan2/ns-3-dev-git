# Install script for directory: C:/ns3-dev-link/src/lr-wpan

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-lr-wpan-relwithdebinfo.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-lr-wpan-relwithdebinfo.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-lr-wpan-relwithdebinfo.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-lr-wpan-relwithdebinfo.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-lr-wpan-relwithdebinfo.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/lr-wpan/helper/lr-wpan-helper.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-constants.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-csmaca.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-error-model.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-fields.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-interference-helper.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-lqi-tag.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-mac-header.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-mac-pl-headers.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-mac-trailer.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-mac-base.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-mac.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-net-device.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-phy.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-spectrum-signal-parameters.h"
    "C:/ns3-dev-link/src/lr-wpan/model/lr-wpan-spectrum-value-helper.h"
    "C:/ns3-dev-link/build/include/ns3/lr-wpan-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/lr-wpan/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
