# Install script for directory: C:/ns3-dev-link/src/stats

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-stats-relwithdebinfo.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-stats-relwithdebinfo.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-stats-relwithdebinfo.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-stats-relwithdebinfo.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-stats-relwithdebinfo.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES "C:/ns3-dev-link/src/stats/model/sqlite-output.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/src/stats/model/sqlite-data-output.h"
    "C:/ns3-dev-link/src/stats/helper/file-helper.h"
    "C:/ns3-dev-link/src/stats/helper/gnuplot-helper.h"
    "C:/ns3-dev-link/src/stats/model/average.h"
    "C:/ns3-dev-link/src/stats/model/basic-data-calculators.h"
    "C:/ns3-dev-link/src/stats/model/boolean-probe.h"
    "C:/ns3-dev-link/src/stats/model/data-calculator.h"
    "C:/ns3-dev-link/src/stats/model/data-collection-object.h"
    "C:/ns3-dev-link/src/stats/model/data-collector.h"
    "C:/ns3-dev-link/src/stats/model/data-output-interface.h"
    "C:/ns3-dev-link/src/stats/model/double-probe.h"
    "C:/ns3-dev-link/src/stats/model/file-aggregator.h"
    "C:/ns3-dev-link/src/stats/model/get-wildcard-matches.h"
    "C:/ns3-dev-link/src/stats/model/gnuplot-aggregator.h"
    "C:/ns3-dev-link/src/stats/model/gnuplot.h"
    "C:/ns3-dev-link/src/stats/model/histogram.h"
    "C:/ns3-dev-link/src/stats/model/omnet-data-output.h"
    "C:/ns3-dev-link/src/stats/model/probe.h"
    "C:/ns3-dev-link/src/stats/model/stats.h"
    "C:/ns3-dev-link/src/stats/model/time-data-calculators.h"
    "C:/ns3-dev-link/src/stats/model/time-probe.h"
    "C:/ns3-dev-link/src/stats/model/time-series-adaptor.h"
    "C:/ns3-dev-link/src/stats/model/uinteger-16-probe.h"
    "C:/ns3-dev-link/src/stats/model/uinteger-32-probe.h"
    "C:/ns3-dev-link/src/stats/model/uinteger-8-probe.h"
    "C:/ns3-dev-link/build/include/ns3/stats-module.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr/src/stats/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
