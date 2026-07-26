cmake_minimum_required(VERSION 3.20)
project(GrandTourColdFront CXX)

set(CMAKE_CXX_STANDARD 17)

# Generate compile_commands.json for VS Code IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Fetch Raylib automatically if it's not found locally
find_package(raylib 5.0 QUIET)
if(NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

# 2. Add source files
add_executable(GrandTourColdFront
    src/projectile.cpp
    src/spaceship.cpp
    src/penguin.cpp
    src/main.cpp
    src/key_input.cpp
    src/globals.cpp
    src/colours.cpp
    src/backgrounds.cpp
    src/alien.cpp
)

target_link_libraries(GrandTourColdFront PRIVATE raylib)
target_include_directories(GrandTourColdFront PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
)

# 3. Web/Emscripten settings
if(EMSCRIPTEN)
    # Output file will be index.html (alongside index.js and index.wasm)
    set_target_properties(GrandTourColdFront PROPERTIES OUTPUT_NAME "index")
    
    target_link_options(GrandTourColdFront PRIVATE
        "-sUSE_GLFW=3"
        "-sWASM=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFORCE_FILESYSTEM=1"
        "-sIDBFS=1"
        "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
        "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/resources@/resources"
    )
endif()