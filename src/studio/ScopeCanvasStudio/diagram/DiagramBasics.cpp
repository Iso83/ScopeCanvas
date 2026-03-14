#include "diagram/DiagramBasics.h"

#include "Interaction/Commands/GraphCommands.h"
#include "demo/Sha256Demo.h"

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_set>

DiagramBasics::DiagramBasics() = default;

void DiagramBasics::seedDefaultDemo() {
    m_engine.graph().clear();
    m_parentLayouts.clear();

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

    m_parentLayouts.clear();
    Node *parent = nullptr;
    for (Node &node : m_engine.graph().nodes()) {
        if (node.title.find("Bits Container") != std::string::npos) {
            parent = &node;
            break;
        }
    }
    if (parent != nullptr) {
        registerParentLayout(parent->id, 8);
    }

    applyParentLayouts();
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

void DiagramBasics::alignSelectedConnectors() {
    for (Node &node : m_engine.graph().nodes()) {
        if (!node.selected || node.connectors.empty()) {
            continue;
        }

        int inputIx = 0;
        int outputIx = 0;
        int inputCount = 0;
        int outputCount = 0;
        for (const Connector &c : node.connectors) {
            if (c.direction == ConnectorDirection::Input) {
                ++inputCount;
            }
            else {
                ++outputCount;
            }
        }

        for (Connector &connector : node.connectors) {
            if (connector.direction == ConnectorDirection::Input) {
                connector.side = ConnectorSide::Left;
                connector.offset = static_cast<float>(inputIx + 1) / static_cast<float>(std::max(1, inputCount + 1));
                ++inputIx;
            }
            else {
                connector.side = ConnectorSide::Right;
                connector.offset = static_cast<float>(outputIx + 1) / static_cast<float>(std::max(1, outputCount + 1));
                ++outputIx;
            }
        }
    }
}

void DiagramBasics::registerParentLayout(uint32_t parentNodeId, int slotCount) {
    ParentLayout layout;
    layout.parentNodeId = parentNodeId;
    layout.slotCount = slotCount;
    layout.slots.resize(static_cast<size_t>(std::max(slotCount, 0)));

    for (Node &node : m_engine.graph().nodes()) {
        const std::string key = "Bit[";
        const size_t ix = node.title.find(key);
        if (ix == std::string::npos) {
            continue;
        }
        const size_t end = node.title.find(']', ix + key.size());
        if (end == std::string::npos) {
            continue;
        }

        const std::string number = node.title.substr(ix + key.size(), end - (ix + key.size()));
        int slot = -1;
        try {
            slot = std::stoi(number);
        }
        catch (...) {
            slot = -1;
        }

        if (slot >= 0 && slot < slotCount) {
            layout.slots[static_cast<size_t>(slot)].childNodeId = node.id;
        }
    }

    m_parentLayouts[parentNodeId] = layout;
}

void DiagramBasics::applyParentLayouts() {
    for (auto &entry : m_parentLayouts) {
        ParentLayout &layout = entry.second;
        Node *parent = m_engine.graph().findNode(layout.parentNodeId);
        if (parent == nullptr) {
            continue;
        }

        for (size_t i = 0; i < layout.slots.size(); ++i) {
            ParentLayoutSlot &slot = layout.slots[i];
            if (slot.childNodeId == 0) {
                continue;
            }

            Node *child = m_engine.graph().findNode(slot.childNodeId);
            if (child == nullptr) {
                slot.childNodeId = 0;
                continue;
            }

            const float targetY = parent->position.y + layout.topPadding + static_cast<float>(i) * (layout.slotHeight + layout.slotGap);
            child->position.x = parent->position.x + 40.0f;
            child->position.y = targetY;
            child->size.x = parent->size.x - 80.0f;
            child->size.y = layout.slotHeight;
            child->groupId = parent->groupId;
            m_engine.graph().recomputeRoutesForNode(child->id);
        }
    }
}
