# Install script for directory: C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna

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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr/lib/libns3-dev-antenna.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr/lib/libns3-dev-antenna.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-antenna.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-antenna.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-antenna.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/model/angles.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/model/antenna-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/model/cosine-antenna-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/model/isotropic-antenna-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/model/parabolic-antenna-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/model/phased-array-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/model/three-gpp-antenna-model.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/model/uniform-planar-array.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/antenna/utils/symmetric-adjacency-matrix.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr/include/ns3/antenna-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr/src/antenna/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
