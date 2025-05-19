cmake_minimum_required(VERSION 3.5)
project(tests)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SRC_DIR "${CMAKE_BINARY_DIR}/../src")
file(GLOB SOURCES_FILES "src/*/*.cpp")

add_executable(tests
    payment.cpp
    parking.cpp
    "${CMAKE_BINARY_DIR}/../src/interface/interface_functions.cpp"
    ${SOURCES_FILES}
)

target_include_directories(tests PRIVATE ${SRC_DIR})
target_include_directories(tests PRIVATE "${CMAKE_BINARY_DIR}/Debug")

target_link_libraries(tests PRIVATE Parking gtest gtest_main)

if(WIN)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_custom_command(
    TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:Parking>"
        "$<TARGET_FILE_DIR:tests>"
    VERBATIM
    COMMAND tests
    COMMENT "Running tests..."
)

enable_testing()
add_test(NAME tests COMMAND tests)

# cmake -DWIN=1 ..
# cmake --build . --target tests