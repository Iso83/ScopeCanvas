#include "diagram/DiagramBasics.h"

#include "Interaction/Commands/GraphCommands.h"
#include "demo/Sha256Demo.h"

#include <memory>
#include <string>
#include <unordered_set>

DiagramBasics::DiagramBasics() = default;

void DiagramBasics::seedDefaultDemo() {
    m_engine.graph().clear();

    Node *node1 = m_engine.graph().createNodeOfType("Number", { -280.0f, -40.0f }, { 180.0f, 100.0f });
    Node *node2 = m_engine.graph().createNodeOfType("Add", { 0.0f, 90.0f }, { 220.0f, 120.0f });
    Node *node3 = m_engine.graph().createNodeOfType("Output", { 260.0f, -150.0f }, { 200.0f, 110.0f });

    if (node1 != nullptr && node2 != nullptr && !node1->connectors.empty() && !node2->connectors.empty()) {
        m_engine.graph().createEdge(node1->id, node1->connectors[0].id, node2->id, node2->connectors[0].id);
    }
    if (node2 != nullptr && node3 != nullptr && !node2->connectors.empty() && !node3->connectors.empty()) {
        m_engine.graph().createEdge(node2->id, node2->connectors.back().id, node3->id, node3->connectors[0].id);
    }

    m_engine.graph().syncIdCounters();
    m_engine.commands().clear();
}

void DiagramBasics::seedSha256Demo() {
    Sha256Demo::Build(m_engine.graph());
    m_engine.commands().clear();
}

void DiagramBasics::createNode(const std::string &typeId, const glm::vec2 &position) {
    auto command = std::make_unique<CreateNodeCommand>(m_engine.graph(), typeId, position, glm::vec2(220.0f, 120.0f));
    m_engine.commands().execute(std::move(command));
}

void DiagramBasics::duplicateSelected(const glm::vec2 &offset) {
    std::vector<const Node *> sources;
    for (const Node &node : m_engine.graph().nodes()) {
        if (node.selected) {
            sources.push_back(&node);
        }
    }

    for (const Node *source : sources) {
        if (source == nullptr) {
            continue;
        }

        auto createCmd = std::make_unique<CreateNodeCommand>(
            m_engine.graph(), source->nodeTypeId, source->position + offset, source->size);
        m_engine.commands().execute(std::move(createCmd));
    }
}

void DiagramBasics::deleteSelected() {
    std::vector<uint32_t> ids;
    for (const Node &node : m_engine.graph().nodes()) {
        if (node.selected) {
            ids.push_back(node.id);
        }
    }

    for (uint32_t id : ids) {
        auto deleteCmd = std::make_unique<DeleteNodeCommand>(m_engine.graph(), id);
        m_engine.commands().execute(std::move(deleteCmd));
    }
}

std::vector<uint32_t> DiagramBasics::selectedNodeIds() const {
    std::vector<uint32_t> ids;
    for (const Node &node : m_engine.graph().nodes()) {
        if (node.selected) {
            ids.push_back(node.id);
        }
    }
    return ids;
}

size_t DiagramBasics::selectedNodeCount() const {
    size_t count = 0;
    for (const Node &node : m_engine.graph().nodes()) {
        if (node.selected) {
            ++count;
        }
    }
    return count;
}

void DiagramBasics::createGroupFromSelection(bool collapsed) {
    Group *group = m_engine.graph().createGroup();
    if (group == nullptr) {
        return;
    }

    for (const Node &node : m_engine.graph().nodes()) {
        if (node.selected) {
            m_engine.graph().addNodeToGroup(node.id, group->id);
        }
    }

    if (collapsed) {
        m_engine.graph().collapseGroup(group->id);
    }
}

void DiagramBasics::toggleSelectedGroupsCollapsed() {
    std::unordered_set<uint32_t> groupIds;
    for (const Node &node : m_engine.graph().nodes()) {
        if (node.selected && node.groupId != 0) {
            groupIds.insert(node.groupId);
        }
    }

    for (uint32_t groupId : groupIds) {
        for (const Group &group : m_engine.graph().groups()) {
            if (group.id != groupId) {
                continue;
            }

            if (group.collapsed) {
                m_engine.graph().expandGroup(group.id);
            }
            else {
                m_engine.graph().collapseGroup(group.id);
            }
            break;
        }
    }
}
