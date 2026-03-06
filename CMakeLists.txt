cmake_minimum_required(VERSION 3.16)
project(NodeCanvasGL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------- Download dependencies ----------
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

FetchContent_MakeAvailable(glfw glm)

# ---------- Executable ----------
add_executable(NodeCanvasGL
    src/main.cpp
    src/App.cpp

    glad/src/glad.c

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

target_include_directories(NodeCanvasGL PRIVATE
    src
    glad/include
)

target_link_libraries(NodeCanvasGL
    glfw
    glm
)