#pragma once

#include <ScopeCanvas/engine/core/ids/CanvasIds.h>
#include <cstddef>
#include <functional>
#include <glm/vec2.hpp>

namespace ScopeCanvas::Engine::Core {
class Node;
class Connector;
class Edge;
} // namespace ScopeCanvas::Engine::Core

namespace ScopeCanvas::Engine::Routing {
class IGraphView {
public:
    virtual ~IGraphView() = default;

    virtual void forEachEdgeId(const std::function<void(Core::Ids::EdgeId)>& f) const = 0;
    virtual glm::vec2 connectorWorld(const Core::Node& node, std::size_t index) const = 0;

    virtual const Core::Edge* getEdge(Core::Ids::EdgeId id) const = 0;
    virtual const Core::Connector* getConnector(Core::Ids::ConnectorId id) const = 0;
    virtual const Core::Node* getNode(Core::Ids::NodeId id) const = 0;
};
} // namespace ScopeCanvas::Engine::Routing
