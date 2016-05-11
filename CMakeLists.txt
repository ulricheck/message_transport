cmake_minimum_required(VERSION 3.3)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

SET(PROJECT_NAME "message_transport")
project(${PROJECT_NAME})

IF(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF(UNIX)

IF(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
ENDIF(APPLE)

set(requiredLibs "")

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)

find_package(Boost COMPONENTS system chrono date_time log thread program_options REQUIRED)
if(Boost_FOUND)
   add_definitions("-DBOOST_ALL_NO_LIB")
   include_directories(${Boost_INCLUDE_DIRS})
   set(requiredLibs ${requiredLibs} ${Boost_LIBRARIES})
endif()

# Event Tracing Integration
set(MSGT_ENABLE_DTRACE_DEFAULT OFF)
set(MSGT_ENABLE_ETW_DEFAULT OFF)
IF(APPLE)
  set(MSGT_ENABLE_DTRACE_DEFAULT ON)
ENDIF(APPLE)
IF(WIN32)
  set(MSGT_ENABLE_ETW_DEFAULT ON)
ENDIF(WIN32)

OPTION(MSGT_ENABLE_DTRACE "Enable DTrace tracing of message_transport activity?" ${MSGT_ENABLE_DTRACE_DEFAULT})
OPTION(MSGT_ENABLE_ETW "Enable Event Tracing of message_transport activity?" ${MSGT_ENABLE_ETW_DEFAULT})

set(tracing_hdr_files "")
set(tracing_src_files "")

IF(MSGT_ENABLE_DTRACE)
    MESSAGE( STATUS "Enabled DTrace.")
    INCLUDE(dtrace)
    add_definitions("-DHAVE_DTRACE")
    add_definitions("-DENABLE_MSGT_TRACING")
    include_directories(${CMAKE_BINARY_DIR})
    set(tracing_hdr_files ${tracing_hdr_files} "${CMAKE_BINARY_DIR}/tracing/probes_message_transport_dtrace.h")
ENDIF(MSGT_ENABLE_DTRACE)

IF(MSGT_ENABLE_ETW)
    MESSAGE( STATUS "Enabled Event Tracing for Windows.")
    INCLUDE(etw)
    add_definitions("-DHAVE_ETW")
    add_definitions("-DENABLE_MSGT_TRACING")
    include_directories(${CMAKE_BINARY_DIR})
    set(tracing_hdr_files ${tracing_hdr_files} "${CMAKE_BINARY_DIR}/tracing/etw/message_transport_etw.h")
    set(tracing_src_files ${tracing_src_files} "${CMAKE_BINARY_DIR}/tracing/etw/message_transport_etw.rc")
    MESSAGE(STATUS "${tracing_src_files}")
ENDIF(MSGT_ENABLE_ETW)




include_directories(include)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

