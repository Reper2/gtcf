cmake_minimum_required(VERSION 3.14)

# Force CMake policy compatibility for Raylib
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(GrandTourColdFront CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Fetch Raylib (Shallow Clone) ---
include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0
    GIT_SHALLOW    TRUE
)

# Populate Raylib and explicitly capture its source directory variable
FetchContent_MakeAvailable(raylib)
FetchContent_GetProperties(raylib SOURCE_DIR raylib_SOURCE_DIR)

# --- Define Executable Target ---
add_executable(GrandTourColdFront
    src/alien.cpp
    src/backgrounds.cpp
    src/colours.cpp
    src/globals.cpp
    src/key_input.cpp
    src/main.cpp
    src/penguin.cpp
    src/projectile.cpp
    src/spaceship.cpp
)

# --- Include Directories & Linking ---
target_include_directories(GrandTourColdFront PRIVATE 
    ${raylib_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

# Linking to raylib automatically passes its public include headers down
target_link_libraries(GrandTourColdFront PRIVATE raylib)

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
        "-sASYNCIFY"
        "-sEXPORTED_RUNTIME_METHODS=['FS','IDBFS']"
        "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/web/shell.html"
        "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/resources@/resources"
    )
endif()