set(SIMPLE_CONSUMER_SRCS
        ${CMAKE_SOURCE_DIR}/tests/simple_consumer.cpp
        )

set(SIMPLE_PRODUCER_SRCS
        ${CMAKE_SOURCE_DIR}/tests/simple_producer.cpp
        )

add_executable(simple_consumer ${SIMPLE_CONSUMER_SRCS})
target_link_libraries(simple_consumer msgt ${requiredLibs})

add_executable(simple_producer ${SIMPLE_PRODUCER_SRCS})
target_link_libraries(simple_producer msgt ${requiredLibs})