file(GLOB_RECURSE SC_GLFW_INTEGRATION_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_GLFW_INTEGRATION_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

file(GLOB_RECURSE SC_GLFW_INTEGRATION_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)

add_library(ScopeCanvasGLFWIntegration
    ${SC_GLFW_INTEGRATION_SRC}
    ${SC_GLFW_INTEGRATION_PRIVATE_HEADERS}
    ${SC_GLFW_INTEGRATION_PUBLIC_HEADERS}
)

target_link_libraries(ScopeCanvasGLFWIntegration PUBLIC
    ScopeCanvasRender
    glfw
)

target_include_directories(ScopeCanvasGLFWIntegration PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(ScopeCanvasGLFWIntegration PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasGLFWIntegration PROPERTY FOLDER "${SC_FOLDER}/integration")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ScopeCanvas/glfw
    PREFIX "Include"
    FILES ${SC_GLFW_INTEGRATION_PUBLIC_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_GLFW_INTEGRATION_SRC} ${SC_GLFW_INTEGRATION_PRIVATE_HEADERS}
)