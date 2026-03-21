# Install script for directory: C:/ns3-dev-link/src

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/ns3-dev-link/build-runner-nr/src/antenna/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/applications/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/buildings/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/config-store/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/core/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/flow-monitor/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/internet/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/mobility/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/network/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/nr/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/point-to-point/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/propagation/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/spectrum/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/stats/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/bridge/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/traffic-control/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/internet-apps/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/virtual-net-device/cmake_install.cmake")
  include("C:/ns3-dev-link/build-runner-nr/src/csma/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
