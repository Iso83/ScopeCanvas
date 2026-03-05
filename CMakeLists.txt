cmake_minimum_required(VERSION 3.16)
project(NodeCanvasGL)

set(CMAKE_CXX_STANDARD 20)

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