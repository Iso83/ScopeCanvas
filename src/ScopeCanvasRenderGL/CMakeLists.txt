file(GLOB_RECURSE RENDERGL_SRC CONFIGURE_DEPENDS
    *.cpp
)

file(GLOB_RECURSE RENDERGL_HEADERS CONFIGURE_DEPENDS
    *.h
)

add_library(ScopeCanvasRenderGL
    ${RENDERGL_SRC}
    ${RENDERGL_HEADERS}
)

target_include_directories(ScopeCanvasRenderGL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ScopeCanvasRenderGL PUBLIC
    ScopeCanvasEngineCore
    ScopeCanvasRouting
    glad
    glm
    opengl32
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDERGL_SRC} ${RENDERGL_HEADERS})
set_property(TARGET ScopeCanvasRenderGL PROPERTY FOLDER "render")
