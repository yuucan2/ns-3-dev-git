# Install script for directory: C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation

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
    set(CMAKE_INSTALL_CONFIG_NAME "release")
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
  # Include the install script for the subdirectory.
  include("C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr-debug/src/propagation/examples/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build/lib/libns3-dev-propagation.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build/lib/libns3-dev-propagation.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-propagation.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-propagation.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-propagation.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/channel-condition-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/cost231-propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/itu-r-1411-los-propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/itu-r-1411-nlos-over-rooftop-propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/jakes-process.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/jakes-propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/kun-2600-mhz-propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/okumura-hata-propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/probabilistic-v2v-channel-condition-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/propagation-cache.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/propagation-delay-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/propagation-environment.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/three-gpp-propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/propagation/model/three-gpp-v2v-propagation-loss-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build/include/ns3/propagation-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr-debug/src/propagation/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
