cmake_minimum_required(VERSION 3.3)
SET(PROJECT_NAME "message_transport")
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

IF(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF(UNIX)

IF(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
ENDIF(APPLE)

set(requiredLibs "")

function(compile_flatbuffers_schema_to_cpp SRC_FBS)
    get_filename_component(SRC_FBS_DIR ${SRC_FBS} PATH)
    string(REGEX REPLACE "\\.fbs$" "_generated.h" GEN_HEADER ${SRC_FBS})
    add_custom_command(
            OUTPUT ${GEN_HEADER}
            COMMAND "${FLATBUFFERS_FLATC_EXECUTABLE}" -c --no-includes --gen-mutable -o "${SRC_FBS_DIR}" "${CMAKE_SOURCE_DIR}/${SRC_FBS}")
endfunction()

find_package(FlatBuffers REQUIRED)
if(FLATBUFFERS_FOUND)
    include_directories(${FLATBUFFERS_INCLUDE_DIR})
    compile_flatbuffers_schema_to_cpp("schema/message_schema.fbs")
    add_custom_target(message_schema_headers DEPENDS schema/message_schema_generated.h)
    include_directories("${CMAKE_BINARY_DIR}")
endif(FLATBUFFERS_FOUND)

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

