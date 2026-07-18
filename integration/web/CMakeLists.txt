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

set_property(TARGET ScopeCanvasWebIntegration PROPERTY FOLDER "${SC_FOLDER}/integration")
