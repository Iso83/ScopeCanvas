file(GLOB_RECURSE SC_DEMOCOMMON_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_DEMOCOMMON_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

file(GLOB_RECURSE SC_DEMOCOMMON_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)

add_library(ScopeCanvasDemoCommon
    ${SC_DEMOCOMMON_SRC}
    ${SC_DEMOCOMMON_PRIVATE_HEADERS}
    ${SC_DEMOCOMMON_PUBLIC_HEADERS}
)

target_link_libraries(ScopeCanvasDemoCommon PUBLIC
    ScopeCanvasWidget
    glad
    glm
)

if(SC_BUILD_DEMO_BENCHMARK)
    target_compile_definitions(ScopeCanvasDemoCommon PUBLIC SC_BUILD_DEMO_BENCHMARK=1)
endif()

target_include_directories(ScopeCanvasDemoCommon PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(ScopeCanvasDemoCommon PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasDemoCommon PROPERTY FOLDER "${SC_FOLDER}/demo")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ScopeCanvas/demo
    PREFIX "Include"
    FILES ${SC_DEMOCOMMON_PUBLIC_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_DEMOCOMMON_SRC} ${SC_DEMOCOMMON_PRIVATE_HEADERS}
)