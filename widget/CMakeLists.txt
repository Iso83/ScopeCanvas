file(GLOB_RECURSE SC_WIDGET_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_WIDGET_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

file(GLOB_RECURSE SC_WIDGET_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)

add_library(ScopeCanvasWidget
    ${SC_WIDGET_SRC}
    ${SC_WIDGET_PRIVATE_HEADERS}
    ${SC_WIDGET_PUBLIC_HEADERS}
)

target_link_libraries(ScopeCanvasWidget PUBLIC
    ScopeCanvasRender
)

target_include_directories(ScopeCanvasWidget PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(ScopeCanvasWidget PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasWidget PROPERTY FOLDER "${SC_FOLDER}/widget")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ScopeCanvas/widget
    PREFIX "Include"
    FILES ${SC_WIDGET_PUBLIC_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_WIDGET_SRC} ${SC_WIDGET_PRIVATE_HEADERS}
)