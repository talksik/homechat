cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(HomechatServer)

find_library(USOCKETS_LIBRARY uSockets.a PATHS /usr/local/lib REQUIRED)
find_package(ZLIB REQUIRED)

set(source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB source_files
    ${source_dir}/*.cpp
)
add_executable(${PROJECT_NAME} ${source_files})

target_include_directories(${PROJECT_NAME} PRIVATE ${source_dir}/include)

target_link_libraries(${PROJECT_NAME} ${USOCKETS_LIBRARY})
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
