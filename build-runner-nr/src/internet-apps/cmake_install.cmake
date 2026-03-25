# Install script for directory: C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr/lib/libns3-dev-internet-apps.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr/lib/libns3-dev-internet-apps.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-internet-apps.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-internet-apps.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-internet-apps.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/helper/dhcp-helper.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/helper/dhcp6-helper.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/helper/ping-helper.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/helper/radvd-helper.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/helper/v4traceroute-helper.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/dhcp-client.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/dhcp-header.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/dhcp-server.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/dhcp6-client.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/dhcp6-duid.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/dhcp6-header.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/dhcp6-options.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/dhcp6-server.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/ping.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/radvd-interface.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/radvd-prefix.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/radvd.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/src/internet-apps/model/v4traceroute.h"
    "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr/include/ns3/internet-apps-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/jagar/OneDrive/NTT DOCOMO/SIM評価/TN_NTN周波数共用/周波数共用による全体最適化確認SIM/ns-3-dev/build-runner-nr/src/internet-apps/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
