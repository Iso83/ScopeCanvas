cmake_minimum_required(VERSION 4.0)

project(
    ScopeCanvas
    VERSION 0.3.0
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

# =========================================================
# Assets
# =========================================================

include(cmake/Assets.cmake)

assets_setup_fonts()

# =========================================================
# Engine
# =========================================================

cppcmake_module_add(engine/core)
cppcmake_module_add(engine/routing)
cppcmake_module_add(engine/render)

# =========================================================
# Widgets
# =========================================================

cppcmake_module_add(widget)

# =========================================================
# Integration
# =========================================================

if(EMSCRIPTEN)
    cppcmake_module_add(integration/web)
else()
    cppcmake_module_add(integration/glfw)
endif()

# =========================================================
# Demos
# =========================================================

set(build_demo_option "${PROJECT_NAME}_BUILD_DEMOS")

option(
    ${build_demo_option}
    "Build demo applications."
    ${PROJECT_IS_TOP_LEVEL}
)

if(${build_demo_option})
    option(${PROJECT_NAME}_DEMO_BENCHMARK "Enable render benchmarking in demo applications" OFF)

    cppcmake_module_add(demo/common)

    if(EMSCRIPTEN)
        cppcmake_module_add(demo/web)
    else()
        cppcmake_module_add(demo/gl)
        cppcmake_module_add(demo/imgui)
        cppcmake_module_add(demo/flow)
    endif()

endif()
