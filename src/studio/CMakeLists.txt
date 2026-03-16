file(GLOB_RECURSE STUDIO_SRC CONFIGURE_DEPENDS
    ScopeCanvasStudio/*.cpp
)

file(GLOB_RECURSE STUDIO_HEADERS CONFIGURE_DEPENDS
    ScopeCanvasStudio/*.h
)

add_executable(ScopeCanvasStudio
    ${STUDIO_SRC}
    ${STUDIO_HEADERS}
)

target_include_directories(ScopeCanvasStudio PRIVATE
    ${CMAKE_SOURCE_DIR}/src/engine/ScopeCanvas
    ${CMAKE_SOURCE_DIR}/src/studio/ScopeCanvasStudio
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(ScopeCanvasStudio
    ScopeCanvasCore
    ScopeCanvasRender
	imgui
)

set_target_properties(ScopeCanvasStudio PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# IDE folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ScopeCanvasStudio
             FILES ${STUDIO_SRC} ${STUDIO_HEADERS})

set_property(TARGET ScopeCanvasStudio PROPERTY FOLDER "studio")
