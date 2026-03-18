file(GLOB_RECURSE SC_RENDERGL_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_RENDERGL_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

file(GLOB_RECURSE SC_RENDERGL_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)

add_library(ScopeCanvasRenderGL
    ${SC_RENDERGL_SRC}
    ${SC_RENDERGL_PRIVATE_HEADERS}
    ${SC_RENDERGL_PUBLIC_HEADERS}
)

target_link_libraries(ScopeCanvasRenderGL PUBLIC
    ScopeCanvasRouting
    glad
    glm
)

target_link_libraries(ScopeCanvasRenderGL PRIVATE
    #opengl32
)

target_include_directories(ScopeCanvasRenderGL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(ScopeCanvasRenderGL PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasRenderGL PROPERTY FOLDER "render")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ScopeCanvas/render
    PREFIX "Include"
    FILES ${SC_RENDERGL_PUBLIC_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_RENDERGL_SRC} ${SC_RENDERGL_PRIVATE_HEADERS}
)