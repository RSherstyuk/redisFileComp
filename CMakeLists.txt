cmake_minimum_required(VERSION 3.10)
project(redisFileComp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
find_package(Hiredis REQUIRED)

add_executable(producer
    src/producer.cpp
    src/redis_queue.cpp
)

add_executable(consumer
    src/consumer.cpp
    src/redis_queue.cpp
)

target_include_directories(producer PRIVATE src)
target_link_libraries(producer PRIVATE ZLIB::ZLIB Hiredis::Hiredis)

target_include_directories(consumer PRIVATE src)
target_link_libraries(consumer PRIVATE ZLIB::ZLIB Hiredis::Hiredis)
