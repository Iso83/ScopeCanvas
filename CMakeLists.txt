cmake_minimum_required(VERSION 3.20)

project(
    ScopeCanvas
    VERSION 0.1.0
    LANGUAGES C CXX
)

# =========================================================
# Options
# =========================================================

option(SC_BUILD_TESTS "Build tests" ON)
option(SC_BUILD_DEMOS "Build demos" ON)
if(SC_BUILD_DEMOS)
    option(SC_BUILD_DEMO_BENCHMARK "Enable render benchmarking in demo applications" OFF)
endif()
set(
    SC_THIRDPARTY_FOLDER
    "third_party"
    CACHE STRING
    "Visual Studio folder for ScopeCanvas dependencies"
)
set(
    SC_FOLDER
    ""
    CACHE STRING
    "Visual Studio folder for ScopeCanvas"
)

# =========================================================
# Global Settings
# =========================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# =========================================================
# Testing
# =========================================================

if(SC_BUILD_TESTS)
    enable_testing()
    include(cmake/ScopeCanvasTests.cmake)
endif()


include(cmake/ScopeCanvasDependencies.cmake)
sc_setup_freetype(SC_FREETYPE_TARGET)

# =========================================================
# GLFW Configuration
# =========================================================

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# =========================================================
# Dependencies
# =========================================================

include(FetchContent)

FetchContent_Declare(
    sc_glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)

FetchContent_Declare(
    sc_glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_Declare(
    sc_nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_Declare(
    sc_imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG 60d7fb207eeb46d6363dd4bde10b35991bae0ce7 # ImGui docking (pinned) 2026-03
)

FetchContent_MakeAvailable(
    sc_glfw
    sc_glm
    sc_imgui
    sc_nlohmann_json
)

# =========================================================
# Visual Studio Folder Helper
# =========================================================

function(sc_set_folder target folder)
    if(TARGET ${target})
        set_property(TARGET ${target} PROPERTY FOLDER ${folder})
    endif()
endfunction()

# =========================================================
# Third Party Organization
# =========================================================

sc_set_folder(glfw            "${SC_THIRDPARTY_FOLDER}/glfw")
sc_set_folder(update_mappings "${SC_THIRDPARTY_FOLDER}/glfw")
sc_set_folder(glm             "${SC_THIRDPARTY_FOLDER}/glm")
sc_set_folder(nlohmann_json   "${SC_THIRDPARTY_FOLDER}/json")

# =========================================================
# GLAD
# =========================================================

add_library(glad STATIC
    third_party/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    third_party/glad/include
)

sc_set_folder(glad "${SC_THIRDPARTY_FOLDER}/glad")

# =========================================================
# ImGui
# =========================================================

add_library(SC_ImGui STATIC
    ${sc_imgui_SOURCE_DIR}/imgui.cpp
    ${sc_imgui_SOURCE_DIR}/imgui_draw.cpp
    ${sc_imgui_SOURCE_DIR}/imgui_tables.cpp
    ${sc_imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${sc_imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${sc_imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(SC_ImGui PUBLIC
    ${sc_imgui_SOURCE_DIR}
    ${sc_imgui_SOURCE_DIR}/backends
)

target_link_libraries(SC_ImGui PUBLIC glfw)

sc_set_folder(SC_ImGui "${SC_THIRDPARTY_FOLDER}/imgui")

# =========================================================
# ScopeCanvas Libraries
# =========================================================

add_subdirectory(engine/core)
add_subdirectory(engine/routing)
add_subdirectory(render/core)
add_subdirectory(widget)
add_subdirectory(integration/glfw)

# =========================================================
# ScopeCanvas Demos
# =========================================================

if(SC_BUILD_DEMOS)
    add_subdirectory(demo/common)
    add_subdirectory(demo/gl)
    add_subdirectory(demo/imgui)
    add_subdirectory(demo/flow)
endif()
