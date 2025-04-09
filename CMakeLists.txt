cmake_minimum_required(VERSION 3.10)
project(RedisQueueExample)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)

find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)
find_library(HIREDIS_LIBRARY hiredis)

if (HIREDIS_INCLUDE_DIR AND HIREDIS_LIBRARY)
    add_library(Hiredis::Hiredis INTERFACE IMPORTED)
    set_target_properties(Hiredis::Hiredis PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${HIREDIS_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${HIREDIS_LIBRARY}"
    )
    message(STATUS "Found Hiredis: ${HIREDIS_LIBRARY}")
else()
    message(FATAL_ERROR "Hiredis not found. Install libhiredis-dev")
endif()

add_executable(producer
    src/producer.cpp
    src/redis_queue.cpp
    src/config.cpp
)

add_executable(consumer
    src/consumer.cpp
    src/redis_queue.cpp
    src/config.cpp
)

target_include_directories(producer PRIVATE src)
target_include_directories(consumer PRIVATE src)

target_link_libraries(producer PRIVATE Hiredis::Hiredis)
target_link_libraries(consumer PRIVATE Hiredis::Hiredis ZLIB::ZLIB)