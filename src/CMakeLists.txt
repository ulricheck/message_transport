
set (MSGT_HEADERS
        ${CMAKE_SOURCE_DIR}/include/message_transport/message_transport_macros.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/message_transport_traits.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/message_transport_types.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/message_transport_util.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/exceptions.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/logging.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/init_logging.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/tracing.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/platform/clean_windows.h
        )

set(MSGT_COMMON_HEADERS
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher.inl
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher_impl.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher_plugin.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber_impl.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber_plugin.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/serialization/serialization.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/serialization/message_types.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/timer.h
        )

set(MSGT_COMMON_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/common/publisher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common/subscriber.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/serialization/serialization.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/serialization/message_types.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/message_transport_util.cpp
        ${tracing_src_files}
        )

set(MSGT_SHAREDMEM_HEADERS
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/release_pool.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/sharedmem_publisher.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/sharedmem_subscriber.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/SharedMemoryBlock.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/SharedMemoryBlockDescriptor.h
        )

set(MSGT_SHAREDMEM_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/sharedmem/sharedmem_publisher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sharedmem/SharedMemoryBlock.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/sharedmem/SharedMemoryBlockDescriptor.cpp
        )

set(MSGT_MANAGER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/sharedmem/sharedmem_manager.cpp
        )


# main library: msgt
add_definitions(-DMSGT_BUILD_SHARED_LIBS)

add_library(msgt SHARED ${MSGT_HEADERS} ${MSGT_COMMON_SOURCES} ${MSGT_COMMON_HEADERS} ${MSGT_SHAREDMEM_SOURCES} ${MSGT_SHAREDMEM_HEADERS})
target_compile_definitions(msgt PUBLIC -DMSGT_EXPORTS)
target_link_libraries(msgt ${requiredLibs})

# tracing
IF(MSGT_ENABLE_DTRACE)
    DTRACE_INSTRUMENT(msgt)
ENDIF(MSGT_ENABLE_DTRACE)

IF(MSGT_ENABLE_ETW)
    ETW_INSTRUMENT(msgt)
ENDIF(MSGT_ENABLE_ETW)

# intermediate tool for shm management: msgt_manager
add_executable(msgt_manager ${MSGT_MANAGER_SOURCES})
target_link_libraries(msgt_manager msgt ${requiredLibs})
