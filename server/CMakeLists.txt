cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(HomechatServer)

set(source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB source_files
    ${source_dir}/*.cpp
)
add_executable(${PROJECT_NAME} ${source_files})

target_include_directories(${PROJECT_NAME} PUBLIC ${source_dir}/include)
