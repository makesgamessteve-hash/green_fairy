cmake_minimum_required(VERSION 3.24)
project(GreenFairy VERSION 0.1.0 LANGUAGES C)

include(CTest)
include(FetchContent)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
option(GF_BUILD_APP "Build the SDL/OpenGL application" ON)
option(GF_COVERAGE "Instrument the first-party C synth engine and Lua loader with gcov" OFF)

FetchContent_Declare(lua
    URL https://www.lua.org/ftp/lua-5.4.8.tar.gz
    URL_HASH SHA256=4f18ddae154e793e46eeab727c59ef1c0c0c2b744e7b94219710d76f530629ae)
FetchContent_MakeAvailable(lua)
file(GLOB lua_sources CONFIGURE_DEPENDS "${lua_SOURCE_DIR}/src/*.c")
list(REMOVE_ITEM lua_sources "${lua_SOURCE_DIR}/src/lua.c" "${lua_SOURCE_DIR}/src/luac.c")
add_library(gf_lua STATIC ${lua_sources})
target_include_directories(gf_lua PUBLIC "${lua_SOURCE_DIR}/src")

add_library(gf_core STATIC src/synth.c src/bank.c)
set_target_properties(gf_core PROPERTIES C_EXTENSIONS OFF)
target_include_directories(gf_core PUBLIC src)
target_link_libraries(gf_core PRIVATE gf_lua)
if(NOT WIN32)
    target_link_libraries(gf_core PUBLIC m)
endif()
if(MSVC)
    target_compile_options(gf_core PRIVATE /W4)
else()
    target_compile_options(gf_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(GF_COVERAGE)
    if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU" OR EMSCRIPTEN)
        message(FATAL_ERROR "GF_COVERAGE requires native GCC")
    endif()
    target_compile_options(gf_core PRIVATE --coverage -O0 -g)
    target_link_options(gf_core INTERFACE --coverage)
endif()

if(BUILD_TESTING AND NOT EMSCRIPTEN)
    add_executable(gf_core_tests tests/core.c)
    target_link_libraries(gf_core_tests PRIVATE gf_core)
    add_test(NAME core COMMAND gf_core_tests
        "${PROJECT_SOURCE_DIR}/assets/scripts/bank.lua")
endif()

if(GF_BUILD_APP)
    set(SDL_SHARED OFF CACHE BOOL "Build SDL as a shared library")
    set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library")
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "Build SDL test helpers")
    set(SDL_TESTS OFF CACHE BOOL "Build SDL tests")
    FetchContent_Declare(SDL3
        URL https://codeload.github.com/libsdl-org/SDL/tar.gz/refs/tags/release-3.2.26
        URL_HASH SHA256=73eeeec0d3235ef75f310bd2bb758622157abfa05da65771656579a7dfeeba05)
    FetchContent_MakeAvailable(SDL3)

    add_executable(green-fairy src/app.c src/draw.c src/panel.c)
    set_target_properties(green-fairy PROPERTIES C_EXTENSIONS OFF)
    target_link_libraries(green-fairy PRIVATE gf_core SDL3::SDL3)
    if(MSVC)
        target_compile_options(green-fairy PRIVATE /W4)
    else()
        target_compile_options(green-fairy PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    if(EMSCRIPTEN)
        set_target_properties(green-fairy PROPERTIES
            OUTPUT_NAME "instrument" SUFFIX ".js" RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/web")
        target_link_options(green-fairy PRIVATE
            "-sMIN_WEBGL_VERSION=2" "-sMAX_WEBGL_VERSION=2" "-sALLOW_MEMORY_GROWTH=1"
            "-sFORCE_FILESYSTEM=1" "-sEXPORTED_RUNTIME_METHODS=FS,HEAPF32,addRunDependency,removeRunDependency"
            "-sINCOMING_MODULE_JS_API=canvas,preRun,onRuntimeInitialized,onAbort,printErr")
        set(asset_destination "${PROJECT_BINARY_DIR}/web")
    elseif(APPLE)
        set_target_properties(green-fairy PROPERTIES MACOSX_BUNDLE ON
            MACOSX_BUNDLE_BUNDLE_NAME "Green Fairy"
            MACOSX_BUNDLE_GUI_IDENTIFIER "studio.greenfairy.instrument")
        set(asset_destination "$<TARGET_BUNDLE_DIR:green-fairy>/Contents/Resources")
    else()
        set(asset_destination "$<TARGET_FILE_DIR:green-fairy>")
    endif()

    add_custom_target(gf_assets ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${asset_destination}/assets"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/assets" "${asset_destination}/assets"
        VERBATIM)
    add_dependencies(green-fairy gf_assets)
    if(EMSCRIPTEN)
        add_custom_command(TARGET gf_assets POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/web" "${asset_destination}"
            VERBATIM)
        install(DIRECTORY "${PROJECT_BINARY_DIR}/web/" DESTINATION .)
    elseif(APPLE)
        install(TARGETS green-fairy BUNDLE DESTINATION .)
    else()
        install(TARGETS green-fairy RUNTIME DESTINATION .)
        install(DIRECTORY assets DESTINATION .)
    endif()
endif()
