if(NOT SCOPECANVAS_BUILD_STUDIO)
    return()
endif()

set(ENGINE_RENDER_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Render/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Render/GridRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Render/NodeRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Render/EdgeRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Render/SelectionRectRenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Render/Shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/View/Camera2D.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/View/Viewport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Interaction/InputState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Interaction/DragController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Interaction/SelectionController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Interaction/ConnectController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Interaction/EdgeInteractionController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Interaction/CameraController.cpp
)

add_library(ScopeCanvasEngineRender ${ENGINE_RENDER_SRC})

target_include_directories(ScopeCanvasEngineRender PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ScopeCanvasEngineRender
    PUBLIC ScopeCanvasEngineCore
    PUBLIC glfw glad
)
if(TARGET glm)
    target_link_libraries(ScopeCanvasEngineRender PUBLIC glm)
endif()

set_property(TARGET ScopeCanvasEngineRender PROPERTY FOLDER "engine/render")
