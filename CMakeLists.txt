cmake_minimum_required(VERSION 4.0)

project(
    ScopeCanvas
    VERSION 0.1.1
    LANGUAGES C CXX
)

# =========================================================
# Project configuration
# =========================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# =========================================================
# CppCMake
# =========================================================

include(cmake/Dependencies.cmake)

dependencies_fetch_cppcmake()

cppcmake_project_initialize()

if(CPPCMAKE_PROJECT_ALREADY_REGISTERED)
    return()
endif()

cppcmake_githook_precommit()
cppcmake_version_generate()
cppcmake_test_setup()

# =========================================================
# Dependencies
# =========================================================

dependencies_setup()

# ============================================================================
# Assets
# ============================================================================

include(cmake/Assets.cmake)

assets_setup_fonts()

# =========================================================
# Modules
# =========================================================

#
# Register project modules here.
#
# Examples:
#
#   cppcmake_module_add(engine/core)
#   cppcmake_module_add(engine/math)
#
#   cppcmake_module_add(apps/demo)
#   cppcmake_module_add(apps/editor)
#








#[[ TODO

----------------------------------------------------------------------------------------------------------------------------

function(sc_add_module_test target folder name)
    add_executable(${name} tests/${name}.cpp)

    target_link_libraries(${name} PRIVATE ${target})
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/tests/common
    )

    add_test(NAME ${name} COMMAND ${name})

    set_property(TARGET ${name} PROPERTY FOLDER "${SC_FOLDER}/tests/${folder}")
endfunction()

# =========================================================
# Options
# =========================================================

option(SC_BUILD_TESTS "Build tests" ON)
option(SC_BUILD_DEMOS "Build demos" ON)
if(SC_BUILD_DEMOS)
    option(SC_BUILD_DEMO_BENCHMARK "Enable render benchmarking in demo applications" OFF)
endif()

# =========================================================
# Testing
# =========================================================

if(SC_BUILD_TESTS)
    enable_testing()
    include(cmake/sc_tests.cmake)
endif()


# =========================================================
# ScopeCanvas Libraries
# =========================================================

add_subdirectory(engine/core)
add_subdirectory(engine/routing)
add_subdirectory(render/core)
add_subdirectory(widget)
if(EMSCRIPTEN)
    add_subdirectory(integration/web)
else()
    add_subdirectory(integration/glfw)
endif()

# =========================================================
# ScopeCanvas Demos
# =========================================================

if(SC_BUILD_DEMOS)
    add_subdirectory(demo/common)
    if(EMSCRIPTEN)
        add_subdirectory(demo/web)
    else()
        add_subdirectory(demo/gl)
        add_subdirectory(demo/imgui)
        add_subdirectory(demo/flow)
    endif()
endif()
]]