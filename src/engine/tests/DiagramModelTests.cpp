#include "Engine/DiagramModel.h"
#include "Engine/NodeTypes.h"

#include <cassert>

int main() {
    DiagramModel model;

    Node *container = model.createNodeWithConnectors({0,0}, {300,200}, createConnectorTemplatesForType(1,1), "Container", "Container");
    assert(container != nullptr);
    const CanvasNodeId containerId = container->id;
    container->allowChildren = true;

    Node *child = model.createNode({20,20}, {100,60});
    assert(child != nullptr);
    const CanvasNodeId childId = child->id;
    assert(model.addChildNode(containerId, childId));
    child = model.findNode(childId);
    assert(child != nullptr);
    assert(child->parentId == containerId);
    assert(model.isNodeHiddenByCollapsedAncestor(childId) == false);

    model.setNodeCollapsed(containerId, true);
    assert(model.isNodeHiddenByCollapsedAncestor(childId) == true);
    model.setNodeCollapsed(containerId, false);

    container = model.findNode(containerId);
    assert(container != nullptr);

    Connector *containerOut = nullptr;
    Connector *childIn = nullptr;
    for (Connector &c : container->connectors) {
        if (c.direction == ConnectorDirection::Output) containerOut = &c;
    }
    for (Connector &c : child->connectors) {
        if (c.direction == ConnectorDirection::Input) { childIn = &c; break; }
    }
    assert(containerOut != nullptr && childIn != nullptr);

    assert(model.createEdge(containerId, containerOut->id, childId, childIn->id));
    assert(!model.createEdge(containerId, containerOut->id, childId, childIn->id)); // duplicate

    Node *n2 = model.createNode({260,30}, {120,80});
    assert(n2 != nullptr);
    Connector *n2in = nullptr;
    Connector *n2out = nullptr;
    for (Connector &c : n2->connectors) {
        if (c.direction == ConnectorDirection::Input && n2in == nullptr) n2in = &c;
        if (c.direction == ConnectorDirection::Output) n2out = &c;
    }
    assert(n2in != nullptr && n2out != nullptr);

    std::string reason;
    assert(!model.isValidConnection(n2->id, n2out->id, childId, childIn->id, &reason));
    assert(!model.createEdge(n2->id, n2out->id, childId, childIn->id)); // child input maxConnections=1

    GraphView v1{CanvasViewId{1}, {0,0}, 1.0f, CanvasNodeId{}};
    GraphView v2{CanvasViewId{2}, {20,10}, 2.0f, containerId};
    assert(v1.id != v2.id);
    assert(v1.zoom != v2.zoom);

    return 0;
}
