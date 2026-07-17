file(GLOB_RECURSE SC_WEB_INTEGRATION_SRC CONFIGURE_DEPENDS
    src/*.cpp
)
file(GLOB_RECURSE SC_WEB_INTEGRATION_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)
add_library(ScopeCanvasWebIntegration
    ${SC_WEB_INTEGRATION_SRC}
    ${SC_WEB_INTEGRATION_PUBLIC_HEADERS}
)
target_link_libraries(ScopeCanvasWebIntegration PUBLIC
    ScopeCanvasRender
)
target_include_directories(ScopeCanvasWebIntegration PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_options(ScopeCanvasWebIntegration PRIVATE
    "-sUSE_GLFW=3"
)

target_link_options(ScopeCanvasWebIntegration PUBLIC
    $<$<BOOL:${EMSCRIPTEN}>:-sUSE_GLFW=3>
    $<$<BOOL:${EMSCRIPTEN}>:-sMIN_WEBGL_VERSION=2>
    $<$<BOOL:${EMSCRIPTEN}>:-sMAX_WEBGL_VERSION=2>
)
set_property(TARGET ScopeCanvasWebIntegration PROPERTY FOLDER "${SC_FOLDER}/integration")
