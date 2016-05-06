
set(MSGT_COMMON_HEADERS
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/message_transport.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher_impl.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/publisher_plugin.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/raw_publisher.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/raw_subscriber.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/simple_publisher_plugin.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/simple_subscriber_plugin.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/single_subscriber_publisher.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber_impl.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/subscriber_plugin.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/common/transport_hints.h
        )

set(MSGT_COMMON_SOURCES
        ${CMAKE_SOURCE_DIR}/src/common/publisher.cpp
        ${CMAKE_SOURCE_DIR}/src/common/subscriber.cpp
        )

set(MSGT_SHAREDMEM_HEADERS
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/sharedmem_publisher.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/sharedmem_subscriber.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/SharedMemoryBlock.h
        ${CMAKE_SOURCE_DIR}/include/message_transport/sharedmem/SharedMemoryBlockDescriptor.h
        )

set(MSGT_SHAREDMEM_SOURCES
        ${CMAKE_SOURCE_DIR}/src/sharedmem/sharedmem_publisher.cpp
        ${CMAKE_SOURCE_DIR}/src/sharedmem/SharedMemoryBlock.cpp
        ${CMAKE_SOURCE_DIR}/src/sharedmem/SharedMemoryBlockDescriptor.cpp
        )

set(MSGT_MANAGER_SOURCES
        ${CMAKE_SOURCE_DIR}/src/sharedmem/sharedmem_manager.cpp
        )

add_library(msgt_common SHARED ${MSGT_COMMON_SOURCES} ${MSGT_COMMON_HEADERS})
target_link_libraries(msgt_common ${requiredLibs})

add_library(msgt_sharedmem SHARED ${MSGT_SHAREDMEM_SOURCES} ${MSGT_SHAREDMEM_HEADERS})
target_link_libraries(msgt_sharedmem msgt_common ${requiredLibs})

add_executable(msgt_manager ${MSGT_MANAGER_SOURCES})
target_link_libraries(msgt_manager msgt_sharedmem msgt_common ${requiredLibs})
