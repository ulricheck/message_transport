
set (MSGT_HEADERS
        ${CMAKE_SOURCE_DIR}/include/message_transport/message_transport_macros.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/message_traits.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/message_transport_types.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/exceptions.h
        )

set(MSGT_COMMON_HEADERS
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher_impl.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher_plugin.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber_impl.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber_plugin.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/serialization/serialization.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/serialization/message_types.h
        )

set(MSGT_COMMON_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/common/publisher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/common/subscriber.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/serialization/serialization.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/serialization/message_types.cpp
        )

set(MSGT_SHAREDMEM_HEADERS
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

message(STATUS ${SCHEMA_HEADERS})

# maybe add static version later ..
add_definitions(-DMSGT_BUILD_SHARED_LIBS)

add_library(msgt SHARED ${MSGT_HEADERS} ${MSGT_COMMON_SOURCES} ${MSGT_COMMON_HEADERS} ${MSGT_SHAREDMEM_SOURCES} ${MSGT_SHAREDMEM_HEADERS})
add_dependencies(msgt message_schema_headers)
target_compile_definitions(msgt PUBLIC -DMSGT_EXPORT)
target_link_libraries(msgt ${requiredLibs})

add_executable(msgt_manager ${MSGT_MANAGER_SOURCES})
target_link_libraries(msgt_manager msgt ${requiredLibs})
