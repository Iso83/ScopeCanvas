file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    ScopeCanvas/*.cpp
)

file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS
    ScopeCanvas/*.h
)

add_library(ScopeCanvasEngine
    ${ENGINE_SRC}
    ${ENGINE_HEADERS}
)

target_include_directories(ScopeCanvasEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvas
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ScopeCanvasEngine
    glfw
    glm
    glad
    nlohmann_json::nlohmann_json
)

# IDE folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvas
             FILES ${ENGINE_SRC} ${ENGINE_HEADERS})

set_property(TARGET ScopeCanvasEngine PROPERTY FOLDER "engine")



# Exe - Testing
add_executable(ScopeCanvas
    ${ENGINE_SRC}
    ${ENGINE_HEADERS}
)

target_include_directories(ScopeCanvas PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvas
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ScopeCanvas
    glfw
    glm
    glad
    nlohmann_json::nlohmann_json
)

# IDE folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvas
             FILES ${ENGINE_SRC} ${ENGINE_HEADERS})

set_property(TARGET ScopeCanvas PROPERTY FOLDER "engine")

set_target_properties(ScopeCanvas PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ScopeCanvas>"
)