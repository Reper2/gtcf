cmake_minimum_required(VERSION 3.14)
project(GrandTourColdFront CXX C)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Automatically Fetch Raylib if Not Found ---
include(FetchContent)

# Set Raylib build options before fetching
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

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

# Native Desktop Link Options (macOS, Windows, Linux)
if(NOT EMSCRIPTEN)
    # Link native raylib and standard system libraries
    target_link_libraries(GrandTourColdFront PRIVATE raylib)
endif()

# Web / Emscripten Specific Settings
if(EMSCRIPTEN)
    set_target_properties(GrandTourColdFront PROPERTIES 
        OUTPUT_NAME "index"
        SUFFIX ".html"
    )
    
    target_link_options(GrandTourColdFront PRIVATE
        "-sUSE_GLFW=3"
        "-sMAX_WEBGL_VERSION=2"
        "-sMIN_WEBGL_VERSION=1"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sFORCE_FILESYSTEM=1"
        "-lidbfs.js"
        "-sEXPORTED_RUNTIME_METHODS=['FS','IDBFS']"
        "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
        "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/resources@/resources"
    )
endif()