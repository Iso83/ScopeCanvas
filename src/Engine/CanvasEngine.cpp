#include "Engine/CanvasEngine.h"

GraphDocument& CanvasEngine::graph() {
    return m_graph;
}

const GraphDocument& CanvasEngine::graph() const {
    return m_graph;
}

CommandStack& CanvasEngine::commands() {
    return m_commands;
}

const CommandStack& CanvasEngine::commands() const {
    return m_commands;
}
