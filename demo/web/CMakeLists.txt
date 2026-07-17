add_executable(ScopeCanvasDemoWeb
    src/main.cpp
    ../gl/src/DemoViewport.cpp
    ../gl/src/DemoViewport.h
)

target_link_libraries(ScopeCanvasDemoWeb PRIVATE
    ScopeCanvasDemoCommon
    ScopeCanvasWebIntegration
)

target_include_directories(ScopeCanvasDemoWeb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../gl/src
)

set(SC_WEB_DIST_DIR "${CMAKE_BINARY_DIR}/dist")
file(MAKE_DIRECTORY "${SC_WEB_DIST_DIR}")
set_target_properties(ScopeCanvasDemoWeb PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SC_WEB_DIST_DIR}"
    OUTPUT_NAME "scopecanvas"
    SUFFIX ".html"
)

target_link_options(ScopeCanvasDemoWeb PRIVATE
    "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
    "-sUSE_GLFW=3"
    "-sMIN_WEBGL_VERSION=2"
    "-sMAX_WEBGL_VERSION=2"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sNO_DISABLE_EXCEPTION_CATCHING=1"
)

sc_setup_web_assets(ScopeCanvasDemoWeb)

add_custom_command(TARGET ScopeCanvasDemoWeb POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:ScopeCanvasDemoWeb>"
        "${SC_WEB_DIST_DIR}/index.html"
    COMMENT "Copying ScopeCanvas Web demo shell to dist/index.html"
)

set_property(TARGET ScopeCanvasDemoWeb PROPERTY FOLDER "${SC_FOLDER}/demo")
