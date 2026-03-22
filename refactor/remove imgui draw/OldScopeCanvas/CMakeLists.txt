file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    ScopeCanvas/*.cpp
)

file(GLOB_RECURSE ENGINE_HEADERS CONFIGURE_DEPENDS
    ScopeCanvas/*.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvas
             FILES ${ENGINE_SRC} ${ENGINE_HEADERS})

# Lib
add_library(OldScopeCanvasEngine
    ${ENGINE_SRC}
    ${ENGINE_HEADERS}
)

target_include_directories(OldScopeCanvasEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvas
)

target_link_libraries(OldScopeCanvasEngine
    glfw
    glm
    glad
    nlohmann_json::nlohmann_json
)

set_property(TARGET OldScopeCanvasEngine PROPERTY FOLDER "Refactor")


# Exe - Testing
add_executable(OldScopeCanvas
    ${ENGINE_SRC}
    ${ENGINE_HEADERS}
)

target_include_directories(OldScopeCanvas PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvas
)

target_link_libraries(OldScopeCanvas
    glfw
    glm
    glad
    nlohmann_json::nlohmann_json
)

set_property(TARGET OldScopeCanvas PROPERTY FOLDER "Refactor")


# Studio
file(GLOB_RECURSE STUDIO_SRC CONFIGURE_DEPENDS
    ScopeCanvasStudio/*.cpp
)

file(GLOB_RECURSE STUDIO_HEADERS CONFIGURE_DEPENDS
    ScopeCanvasStudio/*.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvasStudio
             FILES ${STUDIO_SRC} ${STUDIO_HEADERS})

add_executable(OldScopeCanvasStudio
    ${STUDIO_SRC}
    ${STUDIO_HEADERS}
)

target_include_directories(OldScopeCanvasStudio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvas
    ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvasStudio
)

target_link_libraries(OldScopeCanvasStudio
    OldScopeCanvasEngine
	SC_ImGui
)

set_target_properties(OldScopeCanvasStudio PROPERTIES
    WIN32_EXECUTABLE TRUE
)

set_property(TARGET OldScopeCanvasStudio PROPERTY FOLDER "Refactor")