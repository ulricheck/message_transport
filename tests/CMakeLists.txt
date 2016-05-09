
set(TEST_COMMON_HEADERS
        ${CMAKE_SOURCE_DIR}/tests/message_types.h
        )

set(TEST_COMMON_SRCS
        ${CMAKE_SOURCE_DIR}/tests/message_types.cpp
        )

set(SIMPLE_CONSUMER_SRCS
        ${CMAKE_SOURCE_DIR}/tests/simple_consumer.cpp
        )

set(SIMPLE_PRODUCER_SRCS
        ${CMAKE_SOURCE_DIR}/tests/simple_producer.cpp
        )

add_executable(simple_consumer ${TEST_COMMON_HEADERS} ${TEST_COMMON_SRCS} ${SIMPLE_CONSUMER_SRCS})
target_link_libraries(simple_consumer msgt ${requiredLibs})

add_executable(simple_producer ${TEST_COMMON_HEADERS} ${TEST_COMMON_SRCS} ${SIMPLE_PRODUCER_SRCS})
target_link_libraries(simple_producer msgt ${requiredLibs})