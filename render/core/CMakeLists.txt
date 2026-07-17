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
    ScopeCanvasRouting
    ${SC_FREETYPE_TARGET}
    #opengl32
)
if(NOT SC_BUILD_WEB)
    target_link_libraries(ScopeCanvasRender PUBLIC glad)
else()
    target_compile_options(ScopeCanvasRender PUBLIC "-sUSE_FREETYPE=1")
    target_link_options(ScopeCanvasRender PUBLIC "-sUSE_FREETYPE=1")
endif()

target_include_directories(ScopeCanvasRender PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(ScopeCanvasRender PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasRender PROPERTY FOLDER "${SC_FOLDER}/render")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ScopeCanvas
    PREFIX "Include"
    FILES ${SC_RENDER_PUBLIC_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_RENDER_SRC} ${SC_RENDER_PRIVATE_HEADERS}
)

if(SC_BUILD_TESTS)
    sc_add_module_test(ScopeCanvasRender render SC_Render_ViewportHandlerInput)
endif()
