file(GLOB_RECURSE SC_RENDER_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_RENDER_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

file(GLOB_RECURSE SC_RENDER_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)

add_library(ScopeCanvasRender
    ${SC_RENDER_SRC}
    ${SC_RENDER_PRIVATE_HEADERS}
    ${SC_RENDER_PUBLIC_HEADERS}
)

target_link_libraries(ScopeCanvasRender PUBLIC
    ScopeCanvasDemoCommon
    #opengl32
)

target_include_directories(ScopeCanvasRender PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(ScopeCanvasRender PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasRender PROPERTY FOLDER "render")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ScopeCanvas/render
    PREFIX "Include"
    FILES ${SC_RENDER_PUBLIC_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_RENDER_SRC} ${SC_RENDER_PRIVATE_HEADERS}
)