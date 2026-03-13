#pragma once

#include "Engine/CanvasEngine.h"
#include "GridSettings.h"

#include <glm/vec2.hpp>

#include <cstddef>
#include <string>
#include <cstdint>
#include <vector>

struct StudioViewSettings {
    bool curvedEdgeOverlay = false;
    bool shaNodeStyling = true;
    bool connectorStateColors = true;
};

class DiagramBasics {
public:
    DiagramBasics();

    CanvasEngine &engine() { return m_engine; }
    const CanvasEngine &engine() const { return m_engine; }

    GridSettings &gridSettings() { return m_gridSettings; }
    const GridSettings &gridSettings() const { return m_gridSettings; }

    StudioViewSettings &viewSettings() { return m_viewSettings; }
    const StudioViewSettings &viewSettings() const { return m_viewSettings; }

    void seedDefaultDemo();
    void seedSha256Demo();

    void createNode(const std::string &typeId, const glm::vec2 &position);
    void duplicateSelected(const glm::vec2 &offset = { 60.0f, 40.0f });
    void deleteSelected();

    std::vector<uint32_t> selectedNodeIds() const;
    size_t selectedNodeCount() const;

    void createGroupFromSelection(bool collapsed);
    void toggleSelectedGroupsCollapsed();

private:
    CanvasEngine m_engine;
    GridSettings m_gridSettings;
    StudioViewSettings m_viewSettings;
};
