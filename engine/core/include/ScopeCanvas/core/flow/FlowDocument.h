#pragma once

#include <ScopeCanvas/core/flow/FlowIds.h>
#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <string>
#include <vector>

namespace ScopeCanvas::Core::Flow {

struct FlowStep {
    Ids::NodeId id{};
    Ids::NodeTypeId typeId{};
    std::string title{};
    std::string description{};
    std::string status{};
    bool collapsed{false};
    std::vector<FlowStep> children{};
};

struct FlowRow {
    FlowRowId id{};
    std::string title{};
    std::vector<FlowStep> steps{};
};

struct FlowGroup {
    FlowGroupId id{};
    std::string title{};
    std::string summary{};
    bool collapsed{false};
    std::vector<FlowRow> rows{};
};

class FlowDocument {
  public:
    FlowGroup& createGroup(std::string title, std::string summary = {});
    FlowRow& createRow(FlowGroup& group, std::string title);
    FlowStep& insertStep(FlowRow& row, std::size_t index, Ids::NodeTypeId typeId, std::string title,
                         std::string description, std::string status = {});
    FlowStep& addChildStep(FlowStep& parent, Ids::NodeTypeId typeId, std::string title, std::string description,
                           std::string status = {});

    void toggleCollapsed(Ids::NodeId stepId);
    void toggleGroupCollapsed(FlowGroupId groupId);
    FlowGroup* getGroup(FlowGroupId groupId);
    const FlowGroup* getGroup(FlowGroupId groupId) const;
    void removeStep(Ids::NodeId stepId);
    void moveStep(Ids::NodeId stepId, int delta);

    FlowStep* getStep(Ids::NodeId stepId);
    const FlowStep* getStep(Ids::NodeId stepId) const;
    FlowRow* rowForStep(Ids::NodeId stepId);
    const FlowRow* rowForStep(Ids::NodeId stepId) const;

    std::vector<FlowGroup>& groups() { return m_groups; }
    const std::vector<FlowGroup>& groups() const { return m_groups; }

  private:
    std::vector<FlowGroup> m_groups{};
    std::uint32_t m_nextId{1};

    FlowGroupId nextGroupId();
    FlowRowId nextRowId();
    Ids::NodeId nextStepId();
};

} // namespace ScopeCanvas::Core::Flow
