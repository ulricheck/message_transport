cmake_minimum_required(VERSION 3.3)
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

find_package(Boost COMPONENTS system date_time log thread program_options REQUIRED)
if(Boost_FOUND)
   add_definitions("-DBOOST_ALL_NO_LIB")
   include_directories(${Boost_INCLUDE_DIRS})
   set(requiredLibs ${requiredLibs} ${Boost_LIBRARIES})
endif()

include_directories(include)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

