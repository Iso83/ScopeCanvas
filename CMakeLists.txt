cmake_minimum_required(VERSION 3.16)
project(ScopeCanvas)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable IDE folders (Visual Studio)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

option(SCOPECANVAS_BUILD_STUDIO "Build ScopeCanvasStudio (GLFW/ImGui app)" ON)
option(SCOPECANVAS_BUILD_ENGINE_TESTS "Build engine core tests" ON)

if(SCOPECANVAS_BUILD_ENGINE_TESTS)
    enable_testing()
endif()

# ---------- GLFW configuration ----------

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# ---------- Download dependencies ----------

if(SCOPECANVAS_BUILD_STUDIO)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.9
    )

    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
    )

    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )

    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG docking
    )

    FetchContent_MakeAvailable(glfw glm nlohmann_json imgui)
endif()

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

# ---------- GLAD (local loader) ----------

if(SCOPECANVAS_BUILD_STUDIO)
    add_library(glad STATIC
        third_party/glad/src/glad.c
    )

    target_include_directories(glad PUBLIC
        third_party/glad/include
    )

    set_property(TARGET glad PROPERTY FOLDER "third_party/glad")
endif()

# ---------- IMGUI ----------

if(SCOPECANVAS_BUILD_STUDIO)
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp

    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui
    glfw
    glad
    opengl32
)

set_property(TARGET imgui PROPERTY FOLDER "third_party/imgui")
endif()




add_subdirectory(src/engine)
if(SCOPECANVAS_BUILD_STUDIO)
    add_subdirectory(src/studio)
endif()
