cmake_minimum_required(VERSION 3.16)
project(WebsiteGenerator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror")

set(TARGET_NAME ${PROJECT_NAME})
set(INCLUDE project/include)
set(SOURCE project/src)

set(LIB_NAME WebsiteGeneratorLib)
add_library(
        ${LIB_NAME}
        ${SOURCE}/FSEntryFinder.cpp
)
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE})

add_executable(${TARGET_NAME} project/main.cpp)
target_link_libraries(${TARGET_NAME} ${LIB_NAME})

option(TEST "Enable tests build" OFF)
if (TEST)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")
    set(TEST_DIR tests)

#    target_compile_options(${LIB_NAME} PUBLIC "--coverage" "-O0")
#    target_link_libraries(${LIB_NAME} gcov)
#    target_compile_options(${LIB_NAME} PUBLIC "--coverage" "-O0")
#    target_link_libraries(${LIB_NAME} gcov)

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)

    file(GLOB TestsSrc "${TEST_DIR}/*Tests.cpp")
    foreach (file ${TestsSrc})
        message(${file})
        get_filename_component(target_test ${file} NAME_WE)

        add_executable(${target_test} ${file})
        target_link_libraries(${target_test} ${LIB_NAME})
        target_link_libraries(${target_test} gtest_main)
        gtest_discover_tests(${target_test})

    endforeach ()

    message(STATUS "Tests successfully builded")
endif ()