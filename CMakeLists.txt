cmake_minimum_required(VERSION 3.20)
project(ScopeCanvas LANGUAGES C CXX)

option(SC_BUILD_TESTS "Build tests" ON)
if(SC_BUILD_TESTS)
    enable_testing()
    include(cmake/ScopeCanvasTests.cmake)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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
    #GIT_TAG docking
    GIT_TAG 60d7fb207eeb46d6363dd4bde10b35991bae0ce7 # ImGui docking (pinned) 2026-03
)

FetchContent_MakeAvailable(sc_glfw sc_glm sc_imgui sc_nlohmann_json)

# ---------- Organize third party targets ----------

if(TARGET glfw)
    set_property(TARGET glfw PROPERTY FOLDER "third_party/glfw")
endif()

if(TARGET update_mappings)
    set_property(TARGET update_mappings PROPERTY FOLDER "third_party/glfw")
endif()

if(TARGET glm)
    set_property(TARGET glm PROPERTY FOLDER "third_party/glm")
endif()

if(TARGET nlohmann_json)
    set_property(TARGET nlohmann_json PROPERTY FOLDER "third_party/json")
endif()

# =========================================================
# GLFW
# =========================================================
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# =========================================================
# GLAD (local loader)
# =========================================================

add_library(glad STATIC
    third_party/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    third_party/glad/include
)

set_property(TARGET glad PROPERTY FOLDER "third_party/glad")

# =========================================================
# IMGUI
# =========================================================

add_library(SC_ImGui STATIC
    ${sc_imgui_SOURCE_DIR}/imgui.cpp
    ${sc_imgui_SOURCE_DIR}/imgui_draw.cpp
    ${sc_imgui_SOURCE_DIR}/imgui_tables.cpp
    ${sc_imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${sc_imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${sc_imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(SC_ImGui  PUBLIC
    ${sc_imgui_SOURCE_DIR}
    ${sc_imgui_SOURCE_DIR}/backends
)

target_link_libraries(SC_ImGui PUBLIC glfw)

set_property(TARGET SC_ImGui PROPERTY FOLDER "third_party/imgui")

# =========================================================
# PROJECT COMPONENTS
# =========================================================

add_subdirectory(engine/core)
add_subdirectory(engine/routing)
add_subdirectory(render/core)
add_subdirectory(widget)
add_subdirectory(demo/common)
add_subdirectory(demo/gl)
add_subdirectory(demo/imgui)