file(GLOB_RECURSE SC_STUDIO_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_STUDIO_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

add_executable(ScopeCanvasStudio
    ${SC_STUDIO_SRC}
    ${SC_STUDIO_PRIVATE_HEADERS}
)
set_target_properties(ScopeCanvasStudio PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(ScopeCanvasStudio
    ScopeCanvasWidget
    SC_ImGui
)

target_include_directories(ScopeCanvasStudio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasStudio PROPERTY FOLDER "studio")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_STUDIO_SRC} ${SC_STUDIO_PRIVATE_HEADERS}
)