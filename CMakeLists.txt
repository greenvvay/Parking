cmake_minimum_required(VERSION 3.10)
project(Parking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/*/*.cpp")

add_library(Parking SHARED 
    ${SOURCES}
)

target_compile_definitions(Parking PRIVATE PARKING_EXPORTS)

target_include_directories(Parking PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if(TESTS)
    add_subdirectory(tests)
endif()

if(WIN)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()