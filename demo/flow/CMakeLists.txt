file(GLOB_RECURSE SC_DEMOFLOW_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_DEMOFLOW_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

add_executable(ScopeCanvasDemoFlow
    ${SC_DEMOFLOW_SRC}
    ${SC_DEMOFLOW_PRIVATE_HEADERS}
)

target_link_libraries(ScopeCanvasDemoFlow
    ScopeCanvasWidget
    ScopeCanvasGLFWIntegration
    glad
    glm
    glfw
)

target_include_directories(ScopeCanvasDemoFlow PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasDemoFlow PROPERTY FOLDER "${SC_FOLDER}/demo")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_DEMOFLOW_SRC} ${SC_DEMOFLOW_PRIVATE_HEADERS}
)
