file(GLOB_RECURSE SC_CORE_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_CORE_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

file(GLOB_RECURSE SC_CORE_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)

add_library(ScopeCanvasCore
    ${SC_CORE_SRC}
    ${SC_CORE_PRIVATE_HEADERS}
    ${SC_CORE_PUBLIC_HEADERS}
)

target_include_directories(ScopeCanvasCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${sc_glm_SOURCE_DIR}
)

target_include_directories(ScopeCanvasCore PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasCore PROPERTY FOLDER "${SC_FOLDER}/engine/core")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/ScopeCanvas/core
    PREFIX "Include"
    FILES ${SC_CORE_PUBLIC_HEADERS}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_CORE_SRC} ${SC_CORE_PRIVATE_HEADERS}
)

if(SC_BUILD_TESTS)
    sc_add_module_test(ScopeCanvasCore core SC_Core_IdRegistry)
endif()