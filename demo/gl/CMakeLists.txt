file(GLOB_RECURSE SC_DEMOGL_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_DEMOGL_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

add_executable(ScopeCanvasDemoGL
    ${SC_DEMOGL_SRC}
    ${SC_DEMOGL_PRIVATE_HEADERS}
)

#set_target_properties(ScopeCanvasDemoGL PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(ScopeCanvasDemoGL
    ScopeCanvasRender
    ScopeCanvasWidget
)

target_include_directories(ScopeCanvasDemoGL PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasDemoGL PROPERTY FOLDER "demo")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_DEMOGL_SRC} ${SC_DEMOGL_PRIVATE_HEADERS}
)