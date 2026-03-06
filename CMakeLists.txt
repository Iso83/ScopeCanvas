cmake_minimum_required(VERSION 3.16)
project(NodeCanvasGL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(NODECANVAS_FETCH_DEPS "Fetch GLFW/GLAD/GLM with FetchContent when not found" OFF)

find_package(glfw3 QUIET)
find_package(glm QUIET)

if (NOT TARGET glad::glad)
    find_package(glad CONFIG QUIET)
endif()

if (NODECANVAS_FETCH_DEPS AND (NOT glfw3_FOUND OR NOT TARGET glm::glm OR NOT TARGET glad::glad))
    include(FetchContent)

    if (NOT glfw3_FOUND)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.3.9
        )
        FetchContent_MakeAvailable(glfw)
    endif()

    if (NOT TARGET glad::glad)
        FetchContent_Declare(
            glad
            GIT_REPOSITORY https://github.com/Dav1dde/glad.git
            GIT_TAG v2.0.8
        )
        FetchContent_MakeAvailable(glad)
    endif()

    if (NOT TARGET glm::glm)
        FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG 1.0.1
        )
        FetchContent_MakeAvailable(glm)
    endif()
endif()

if (NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW not found. Install glfw3 or configure with -DNODECANVAS_FETCH_DEPS=ON.")
endif()
if (NOT TARGET glad::glad)
    message(FATAL_ERROR "GLAD not found. Install glad or configure with -DNODECANVAS_FETCH_DEPS=ON.")
endif()
if (NOT TARGET glm::glm)
    message(FATAL_ERROR "GLM not found. Install glm or configure with -DNODECANVAS_FETCH_DEPS=ON.")
endif()

add_executable(NodeCanvasGL
    src/main.cpp
    src/App.cpp

    src/Engine/DiagramModel.cpp
    src/Engine/Commands.cpp
    src/Engine/Serializer.cpp

    src/View/Camera2D.cpp
    src/View/Viewport.cpp

    src/Render/Renderer.cpp
    src/Render/GridRenderer.cpp
    src/Render/NodeRenderer.cpp
    src/Render/EdgeRenderer.cpp
    src/Render/Shader.cpp

    src/Interaction/InputState.cpp
    src/Interaction/SelectionController.cpp
    src/Interaction/DragController.cpp
    src/Interaction/ConnectController.cpp
    src/Interaction/GroupController.cpp
    src/Interaction/ArrangeController.cpp

    src/Util/Log.cpp
)

target_include_directories(NodeCanvasGL PRIVATE src)
target_link_libraries(NodeCanvasGL PRIVATE glfw glad::glad glm::glm)
