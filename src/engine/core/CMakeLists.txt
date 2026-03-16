set(ENGINE_CORE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/CanvasEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/DiagramModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/LayoutEngine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/NodeTypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Interaction/Commands/CommandManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Interaction/Commands/GraphCommands.cpp
)

set(ENGINE_CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/CanvasEngine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/CanvasExtensionPoints.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/CanvasIds.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/ConnectorDirection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/DiagramModel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/LayoutEngine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/NodeTypes.h
)

if(TARGET nlohmann_json::nlohmann_json)
    list(APPEND ENGINE_CORE_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/Persistence/GraphSerializer.cpp
    )
    list(APPEND ENGINE_CORE_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas/Engine/Persistence/GraphSerializer.h
    )
endif()

add_library(ScopeCanvasEngineCore
    ${ENGINE_CORE_SRC}
    ${ENGINE_CORE_HEADERS}
)

target_include_directories(ScopeCanvasEngineCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../ScopeCanvas
    ${CMAKE_SOURCE_DIR}/include
)

if(NOT TARGET glm)
    target_include_directories(ScopeCanvasEngineCore PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/fallback
    )
endif()

if(TARGET glm)
    target_link_libraries(ScopeCanvasEngineCore PUBLIC glm)
endif()
if(TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(ScopeCanvasEngineCore PUBLIC nlohmann_json::nlohmann_json)
endif()

set_property(TARGET ScopeCanvasEngineCore PROPERTY FOLDER "engine/core")
