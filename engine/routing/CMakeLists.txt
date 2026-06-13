file(GLOB_RECURSE SC_ROUTING_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_ROUTING_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

file(GLOB_RECURSE SC_ROUTING_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)

add_library(ScopeCanvasRouting
    ${SC_ROUTING_SRC}
    ${SC_ROUTING_PRIVATE_HEADERS}
    ${SC_ROUTING_PUBLIC_HEADERS}
)

target_link_libraries(ScopeCanvasRouting PUBLIC 
    ScopeCanvasCore
)

target_include_directories(ScopeCanvasRouting PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(ScopeCanvasRouting PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasRouting PROPERTY FOLDER "engine/routing")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ScopeCanvas/routing
    PREFIX "Include"
    FILES ${SC_ROUTING_PUBLIC_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_ROUTING_SRC} ${SC_ROUTING_PRIVATE_HEADERS}
)

if(SC_BUILD_TESTS)
    sc_add_module_test(ScopeCanvasRouting routing SC_Routing_EdgeRouter)
endif()