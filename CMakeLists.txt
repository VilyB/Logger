cmake_minimum_required(VERSION 3.16)
project(logger)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors")

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Include directories
set(IMGUI_DIR "./libraries/imgui")
include_directories(
    src/core
    src/io
    src/net
    src/ui
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${SDL2_INCLUDE_DIRS}
)

# Source files
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)

# Main executable
add_executable(logger ${PROJECT_SOURCES} ${IMGUI_SOURCES})

# Link libraries
target_link_libraries(logger ${SDL2_LIBRARIES})
target_compile_options(logger PRIVATE ${SDL2_CFLAGS_OTHER})

# Set output directory
set_target_properties(logger PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Google Test setup
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # Enable testing
    enable_testing()
    
    # Find or fetch Google Test
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50f33f9b9fd6d7d1c4b0f4b8b8f3b4.zip
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    # Debug: Print what test sources are found
    file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
    message(STATUS "Found test sources: ${TEST_SOURCES}")
    
    if(TEST_SOURCES)
        # Create a library from main project sources (excluding main.cpp if it exists)
        file(GLOB_RECURSE LIB_SOURCES "src/*.cpp")
        list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")
        
        # Always create the test executable, even if no library sources
        add_executable(logger_tests ${TEST_SOURCES})
        
        # Link with project library if it exists
        if(LIB_SOURCES)
            add_library(logger_lib ${LIB_SOURCES} ${IMGUI_SOURCES})
            target_link_libraries(logger_lib ${SDL2_LIBRARIES})
            target_compile_options(logger_lib PRIVATE ${SDL2_CFLAGS_OTHER})
            target_link_libraries(logger_tests logger_lib)
        else()
            # If no library sources, link ImGui directly to tests
            target_sources(logger_tests PRIVATE ${IMGUI_SOURCES})
        endif()
        
        # Link Google Test and other dependencies
        target_link_libraries(logger_tests 
            GTest::gtest_main
            ${SDL2_LIBRARIES}
        )
        target_compile_options(logger_tests PRIVATE ${SDL2_CFLAGS_OTHER})
        
        # Register tests with CTest
        include(GoogleTest)
        gtest_discover_tests(logger_tests)
        
        message(STATUS "Test executable 'logger_tests' will be created")
    else()
        message(WARNING "No test sources found in tests/ directory!")
    endif()
endif()

# Custom targets for convenience
add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/bin/logger
    DEPENDS logger
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if(BUILD_TESTS AND TARGET logger_tests)
    add_custom_target(test_run
        COMMAND ${CMAKE_BINARY_DIR}/logger_tests
        DEPENDS logger_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Valgrind target (if available)
find_program(VALGRIND_EXECUTABLE valgrind)
if(VALGRIND_EXECUTABLE)
    add_custom_target(valgrind
        COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full --log-file=valgrind.log ${CMAKE_BINARY_DIR}/bin/logger
        DEPENDS logger
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()