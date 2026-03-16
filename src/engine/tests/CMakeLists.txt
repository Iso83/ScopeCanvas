if(NOT SCOPECANVAS_BUILD_ENGINE_TESTS)
    return()
endif()

enable_testing()

add_executable(ScopeCanvasEngineTests
    DiagramModelTests.cpp
)

target_include_directories(ScopeCanvasEngineTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas
)

target_link_libraries(ScopeCanvasEngineTests PRIVATE ScopeCanvasCore)

add_test(NAME ScopeCanvasEngineTests COMMAND ScopeCanvasEngineTests)
set_property(TARGET ScopeCanvasEngineTests PROPERTY FOLDER "engine/tests")
