cmake_minimum_required(VERSION 3.20)

project(ScopeCanvasCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

file(GLOB_RECURSE ENGINECORE_SRC CONFIGURE_DEPENDS
    src/engine/ScopeCanvasEngineCore/*.cpp
)

file(GLOB_RECURSE ENGINECORE_HEADERS CONFIGURE_DEPENDS
    src/engine/ScopeCanvasEngineCore/*.h
)

add_library(ScopeCanvasEngineCore
    ${ENGINECORE_SRC}
    ${ENGINECORE_HEADERS}
)

target_include_directories(ScopeCanvasEngineCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
)

file(GLOB_RECURSE ROUTING_SRC CONFIGURE_DEPENDS
    src/engine/ScopeCanvasRouting/*.cpp
)

file(GLOB_RECURSE ROUTING_HEADERS CONFIGURE_DEPENDS
    src/engine/ScopeCanvasRouting/*.h
)

add_library(ScopeCanvasRouting
    ${ROUTING_SRC}
    ${ROUTING_HEADERS}
)

target_include_directories(ScopeCanvasRouting PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
)

target_link_libraries(ScopeCanvasRouting PUBLIC
    ScopeCanvasEngineCore
)

add_executable(ScopeCanvasEngineCoreVec2Tests
    tests/Vec2Tests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreVec2Tests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreVec2Tests COMMAND ScopeCanvasEngineCoreVec2Tests)

add_executable(ScopeCanvasEngineCoreIdTests
    tests/IdTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreIdTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreIdTests COMMAND ScopeCanvasEngineCoreIdTests)

add_executable(ScopeCanvasEngineCoreNodeTests
    tests/NodeTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreNodeTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreNodeTests COMMAND ScopeCanvasEngineCoreNodeTests)

add_executable(ScopeCanvasEngineCoreConnectorTests
    tests/ConnectorTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreConnectorTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreConnectorTests COMMAND ScopeCanvasEngineCoreConnectorTests)


add_executable(ScopeCanvasEngineCoreLayoutTests
    tests/LayoutTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreLayoutTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreLayoutTests COMMAND ScopeCanvasEngineCoreLayoutTests)


add_executable(ScopeCanvasEngineCoreDiagramModelTests
    tests/enginecore/DiagramModelTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreDiagramModelTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreDiagramModelTests COMMAND ScopeCanvasEngineCoreDiagramModelTests)


add_executable(ScopeCanvasEngineCoreConnectorLogicTests
    tests/enginecore/ConnectorLogicTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreConnectorLogicTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreConnectorLogicTests COMMAND ScopeCanvasEngineCoreConnectorLogicTests)


add_executable(ScopeCanvasEngineCoreNodeBuilderRegistryTests
    tests/enginecore/NodeBuilderRegistryTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreNodeBuilderRegistryTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreNodeBuilderRegistryTests COMMAND ScopeCanvasEngineCoreNodeBuilderRegistryTests)


add_executable(ScopeCanvasEngineCoreGridSettingsTests
    tests/enginecore/GridSettingsTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreGridSettingsTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreGridSettingsTests COMMAND ScopeCanvasEngineCoreGridSettingsTests)


add_executable(ScopeCanvasEngineCoreLayoutGroupTests
    tests/enginecore/LayoutGroupTests.cpp
)
target_link_libraries(ScopeCanvasEngineCoreLayoutGroupTests PRIVATE
    ScopeCanvasEngineCore
)
add_test(NAME ScopeCanvasEngineCoreLayoutGroupTests COMMAND ScopeCanvasEngineCoreLayoutGroupTests)

add_executable(ScopeCanvasRoutingCompileTests
    tests/routing/RoutingCompileTests.cpp
)
target_link_libraries(ScopeCanvasRoutingCompileTests PRIVATE
    ScopeCanvasRouting
)
add_test(NAME ScopeCanvasRoutingCompileTests COMMAND ScopeCanvasRoutingCompileTests)


add_executable(ScopeCanvasRoutingEdgeRouteTests
    tests/routing/EdgeRouteTests.cpp
)
target_link_libraries(ScopeCanvasRoutingEdgeRouteTests PRIVATE
    ScopeCanvasRouting
)
add_test(NAME ScopeCanvasRoutingEdgeRouteTests COMMAND ScopeCanvasRoutingEdgeRouteTests)

add_executable(ScopeCanvasRoutingEdgeBundleTests
    tests/routing/EdgeBundleTests.cpp
)
target_link_libraries(ScopeCanvasRoutingEdgeBundleTests PRIVATE
    ScopeCanvasRouting
)
add_test(NAME ScopeCanvasRoutingEdgeBundleTests COMMAND ScopeCanvasRoutingEdgeBundleTests)

add_executable(ScopeCanvasRoutingEdgeRouterTests
    tests/routing/EdgeRouterTests.cpp
)
target_link_libraries(ScopeCanvasRoutingEdgeRouterTests PRIVATE
    ScopeCanvasRouting
)
add_test(NAME ScopeCanvasRoutingEdgeRouterTests COMMAND ScopeCanvasRoutingEdgeRouterTests)
