cmake_minimum_required(VERSION 3.10)
project(RaylibGameDev C)

# CMake 4 removed compatibility behavior for projects declaring very old
# minimum versions (raylib 5.0 uses 3.0). Set a policy floor so the
# dependency can still configure.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add Raylib
include(FetchContent)
FetchContent_Declare(raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.c
    src/game.c
)

# Create executable
add_executable(game ${SOURCES})

# Link Raylib
target_link_libraries(game raylib)

# Platform-specific configurations
if(MSVC)
    target_compile_options(game PRIVATE /W4)
else()
    target_compile_options(game PRIVATE -Wall -Wextra -Wpedantic)
endif()
