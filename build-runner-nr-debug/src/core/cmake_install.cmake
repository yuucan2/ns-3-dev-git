# Install script for directory: C:/ns3-dev-link/src/core

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
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/ns3-dev-link/build/lib/libns3-dev-core-debug.dll.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "C:/ns3-dev-link/build/lib/libns3-dev-core-debug.dll")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.dll" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.dll")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/msys64/ucrt64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libns3-dev-core-debug.dll")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ns3" TYPE FILE FILES
    "C:/ns3-dev-link/build/include/ns3/core-config.h"
    "C:/ns3-dev-link/src/core/model/int64x64-128.h"
    "C:/ns3-dev-link/src/core/helper/csv-reader.h"
    "C:/ns3-dev-link/src/core/helper/event-garbage-collector.h"
    "C:/ns3-dev-link/src/core/helper/random-variable-stream-helper.h"
    "C:/ns3-dev-link/src/core/model/abort.h"
    "C:/ns3-dev-link/src/core/model/ascii-file.h"
    "C:/ns3-dev-link/src/core/model/ascii-test.h"
    "C:/ns3-dev-link/src/core/model/assert.h"
    "C:/ns3-dev-link/src/core/model/attribute-accessor-helper.h"
    "C:/ns3-dev-link/src/core/model/attribute-construction-list.h"
    "C:/ns3-dev-link/src/core/model/attribute-container.h"
    "C:/ns3-dev-link/src/core/model/attribute-helper.h"
    "C:/ns3-dev-link/src/core/model/attribute.h"
    "C:/ns3-dev-link/src/core/model/boolean.h"
    "C:/ns3-dev-link/src/core/model/breakpoint.h"
    "C:/ns3-dev-link/src/core/model/build-profile.h"
    "C:/ns3-dev-link/src/core/model/calendar-scheduler.h"
    "C:/ns3-dev-link/src/core/model/callback.h"
    "C:/ns3-dev-link/src/core/model/command-line.h"
    "C:/ns3-dev-link/src/core/model/config.h"
    "C:/ns3-dev-link/src/core/model/default-deleter.h"
    "C:/ns3-dev-link/src/core/model/default-simulator-impl.h"
    "C:/ns3-dev-link/src/core/model/demangle.h"
    "C:/ns3-dev-link/src/core/model/deprecated.h"
    "C:/ns3-dev-link/src/core/model/des-metrics.h"
    "C:/ns3-dev-link/src/core/model/double.h"
    "C:/ns3-dev-link/src/core/model/enum.h"
    "C:/ns3-dev-link/src/core/model/event-id.h"
    "C:/ns3-dev-link/src/core/model/event-impl.h"
    "C:/ns3-dev-link/src/core/model/fatal-error.h"
    "C:/ns3-dev-link/src/core/model/fatal-impl.h"
    "C:/ns3-dev-link/src/core/model/fd-reader.h"
    "C:/ns3-dev-link/src/core/model/environment-variable.h"
    "C:/ns3-dev-link/src/core/model/global-value.h"
    "C:/ns3-dev-link/src/core/model/hash-fnv.h"
    "C:/ns3-dev-link/src/core/model/hash-function.h"
    "C:/ns3-dev-link/src/core/model/hash-murmur3.h"
    "C:/ns3-dev-link/src/core/model/hash.h"
    "C:/ns3-dev-link/src/core/model/heap-scheduler.h"
    "C:/ns3-dev-link/src/core/model/int64x64-double.h"
    "C:/ns3-dev-link/src/core/model/int64x64.h"
    "C:/ns3-dev-link/src/core/model/integer.h"
    "C:/ns3-dev-link/src/core/model/length.h"
    "C:/ns3-dev-link/src/core/model/list-scheduler.h"
    "C:/ns3-dev-link/src/core/model/log-macros-disabled.h"
    "C:/ns3-dev-link/src/core/model/log-macros-enabled.h"
    "C:/ns3-dev-link/src/core/model/log.h"
    "C:/ns3-dev-link/src/core/model/make-event.h"
    "C:/ns3-dev-link/src/core/model/map-scheduler.h"
    "C:/ns3-dev-link/src/core/model/math.h"
    "C:/ns3-dev-link/src/core/model/names.h"
    "C:/ns3-dev-link/src/core/model/node-printer.h"
    "C:/ns3-dev-link/src/core/model/nstime.h"
    "C:/ns3-dev-link/src/core/model/object-base.h"
    "C:/ns3-dev-link/src/core/model/object-factory.h"
    "C:/ns3-dev-link/src/core/model/object-map.h"
    "C:/ns3-dev-link/src/core/model/object-ptr-container.h"
    "C:/ns3-dev-link/src/core/model/object-vector.h"
    "C:/ns3-dev-link/src/core/model/object.h"
    "C:/ns3-dev-link/src/core/model/pair.h"
    "C:/ns3-dev-link/src/core/model/pointer.h"
    "C:/ns3-dev-link/src/core/model/priority-queue-scheduler.h"
    "C:/ns3-dev-link/src/core/model/ptr.h"
    "C:/ns3-dev-link/src/core/model/random-variable-stream.h"
    "C:/ns3-dev-link/src/core/model/rng-seed-manager.h"
    "C:/ns3-dev-link/src/core/model/rng-stream.h"
    "C:/ns3-dev-link/src/core/model/scheduler.h"
    "C:/ns3-dev-link/src/core/model/show-progress.h"
    "C:/ns3-dev-link/src/core/model/shuffle.h"
    "C:/ns3-dev-link/src/core/model/simple-ref-count.h"
    "C:/ns3-dev-link/src/core/model/simulation-singleton.h"
    "C:/ns3-dev-link/src/core/model/simulator-impl.h"
    "C:/ns3-dev-link/src/core/model/simulator.h"
    "C:/ns3-dev-link/src/core/model/singleton.h"
    "C:/ns3-dev-link/src/core/model/string.h"
    "C:/ns3-dev-link/src/core/model/synchronizer.h"
    "C:/ns3-dev-link/src/core/model/system-path.h"
    "C:/ns3-dev-link/src/core/model/system-wall-clock-ms.h"
    "C:/ns3-dev-link/src/core/model/system-wall-clock-timestamp.h"
    "C:/ns3-dev-link/src/core/model/test.h"
    "C:/ns3-dev-link/src/core/model/time-printer.h"
    "C:/ns3-dev-link/src/core/model/timer-impl.h"
    "C:/ns3-dev-link/src/core/model/timer.h"
    "C:/ns3-dev-link/src/core/model/trace-source-accessor.h"
    "C:/ns3-dev-link/src/core/model/traced-callback.h"
    "C:/ns3-dev-link/src/core/model/traced-value.h"
    "C:/ns3-dev-link/src/core/model/trickle-timer.h"
    "C:/ns3-dev-link/src/core/model/tuple.h"
    "C:/ns3-dev-link/src/core/model/type-id.h"
    "C:/ns3-dev-link/src/core/model/type-name.h"
    "C:/ns3-dev-link/src/core/model/type-traits.h"
    "C:/ns3-dev-link/src/core/model/uinteger.h"
    "C:/ns3-dev-link/src/core/model/uniform-random-bit-generator.h"
    "C:/ns3-dev-link/src/core/model/valgrind.h"
    "C:/ns3-dev-link/src/core/model/vector.h"
    "C:/ns3-dev-link/src/core/model/warnings.h"
    "C:/ns3-dev-link/src/core/model/watchdog.h"
    "C:/ns3-dev-link/src/core/model/realtime-simulator-impl.h"
    "C:/ns3-dev-link/src/core/model/wall-clock-synchronizer.h"
    "C:/ns3-dev-link/src/core/model/val-array.h"
    "C:/ns3-dev-link/src/core/model/matrix-array.h"
    "C:/ns3-dev-link/build/include/ns3/core-module.h"
    "C:/ns3-dev-link/build/include/ns3/core-export.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/ns3-dev-link/build-runner-nr-debug/src/core/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
