file(GLOB_RECURSE SC_DEMOIMGUI_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE SC_DEMOIMGUI_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

add_executable(ScopeCanvasDemoImGui
    ${SC_DEMOIMGUI_SRC}
    ${SC_DEMOIMGUI_PRIVATE_HEADERS}
)

set_target_properties(ScopeCanvasDemoImGui PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(ScopeCanvasDemoImGui
    ScopeCanvasDemoCommon
    SC_ImGui
)

target_include_directories(ScopeCanvasDemoImGui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_property(TARGET ScopeCanvasDemoImGui PROPERTY FOLDER "demo")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src
    PREFIX "Source"
    FILES ${SC_DEMOIMGUI_SRC} ${SC_DEMOIMGUI_PRIVATE_HEADERS}
)