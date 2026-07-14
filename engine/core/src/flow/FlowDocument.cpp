#include <ScopeCanvas/core/flow/FlowDocument.h>
#include <algorithm>

namespace ScopeCanvas::Core::Flow {
namespace {
FlowStep* findStepIn(std::vector<FlowStep>& steps, Ids::NodeId stepId) {
    for (FlowStep& step : steps) {
        if (step.id == stepId)
            return &step;
        if (FlowStep* child = findStepIn(step.children, stepId); child != nullptr)
            return child;
    }
    return nullptr;
}

const FlowStep* findStepIn(const std::vector<FlowStep>& steps, Ids::NodeId stepId) {
    for (const FlowStep& step : steps) {
        if (step.id == stepId)
            return &step;
        if (const FlowStep* child = findStepIn(step.children, stepId); child != nullptr)
            return child;
    }
    return nullptr;
}

bool eraseStepIn(std::vector<FlowStep>& steps, Ids::NodeId stepId) {
    const auto it = std::remove_if(steps.begin(), steps.end(), [stepId](const FlowStep& step) { return step.id == stepId; });
    const bool erased = it != steps.end();
    steps.erase(it, steps.end());
    if (erased)
        return true;
    for (FlowStep& step : steps)
        if (eraseStepIn(step.children, stepId))
            return true;
    return false;
}

auto findGroupIn(std::vector<FlowGroup>& groups, FlowGroupId groupId) {
    return std::find_if(groups.begin(), groups.end(), [groupId](const FlowGroup& group) { return group.id == groupId; });
}

auto findGroupIn(const std::vector<FlowGroup>& groups, FlowGroupId groupId) {
    return std::find_if(groups.begin(), groups.end(), [groupId](const FlowGroup& group) { return group.id == groupId; });
}

bool moveStepIn(std::vector<FlowStep>& steps, Ids::NodeId stepId, int delta) {
    const auto it = std::find_if(steps.begin(), steps.end(), [stepId](const FlowStep& step) { return step.id == stepId; });
    if (it != steps.end()) {
        const auto oldIndex = static_cast<int>(std::distance(steps.begin(), it));
        const auto newIndex = std::clamp(oldIndex + delta, 0, static_cast<int>(steps.size()) - 1);
        if (oldIndex == newIndex)
            return true;
        FlowStep step = std::move(*it);
        steps.erase(it);
        steps.insert(steps.begin() + newIndex, std::move(step));
        return true;
    }
    for (FlowStep& step : steps)
        if (moveStepIn(step.children, stepId, delta))
            return true;
    return false;
}
} // namespace

FlowGroupId FlowDocument::nextGroupId() { return FlowGroupId{m_nextId++}; }

FlowRowId FlowDocument::nextRowId() { return FlowRowId{m_nextId++}; }

Ids::NodeId FlowDocument::nextStepId() { return Ids::NodeId{m_nextId++}; }

FlowGroup& FlowDocument::createGroup(std::string title, std::string summary) {
    FlowGroup group{};
    group.id = nextGroupId();
    group.title = std::move(title);
    group.summary = std::move(summary);
    m_groups.push_back(std::move(group));
    return m_groups.back();
}

FlowRow& FlowDocument::createRow(FlowGroup& group, std::string title) {
    FlowRow row{};
    row.id = nextRowId();
    row.title = std::move(title);
    group.rows.push_back(std::move(row));
    return group.rows.back();
}

FlowStep& FlowDocument::insertStep(FlowRow& row, std::size_t index, Ids::NodeTypeId typeId, std::string title,
                                   std::string description, std::string status) {
    FlowStep step{};
    step.id = nextStepId();
    step.typeId = typeId;
    step.title = std::move(title);
    step.description = std::move(description);
    step.status = std::move(status);
    index = std::min(index, row.steps.size());
    return *row.steps.insert(row.steps.begin() + static_cast<std::ptrdiff_t>(index), std::move(step));
}

FlowStep& FlowDocument::addChildStep(FlowStep& parent, Ids::NodeTypeId typeId, std::string title,
                                     std::string description, std::string status) {
    FlowStep child{};
    child.id = nextStepId();
    child.typeId = typeId;
    child.title = std::move(title);
    child.description = std::move(description);
    child.status = std::move(status);
    parent.children.push_back(std::move(child));
    return parent.children.back();
}

void FlowDocument::toggleCollapsed(Ids::NodeId stepId) {
    if (FlowStep* step = getStep(stepId); step != nullptr && !step->children.empty())
        step->collapsed = !step->collapsed;
}

void FlowDocument::toggleGroupCollapsed(FlowGroupId groupId) {
    if (FlowGroup* group = getGroup(groupId); group != nullptr)
        group->collapsed = !group->collapsed;
}

FlowGroup* FlowDocument::getGroup(FlowGroupId groupId) {
    const auto it = findGroupIn(m_groups, groupId);
    return it == m_groups.end() ? nullptr : &(*it);
}

const FlowGroup* FlowDocument::getGroup(FlowGroupId groupId) const {
    const auto it = findGroupIn(m_groups, groupId);
    return it == m_groups.end() ? nullptr : &(*it);
}

void FlowDocument::removeStep(Ids::NodeId stepId) {
    for (FlowGroup& group : m_groups)
        for (FlowRow& row : group.rows)
            if (eraseStepIn(row.steps, stepId))
                return;
}

void FlowDocument::moveStep(Ids::NodeId stepId, int delta) {
    if (delta == 0)
        return;
    for (FlowGroup& group : m_groups)
        for (FlowRow& row : group.rows)
            if (moveStepIn(row.steps, stepId, delta))
                return;
}

FlowStep* FlowDocument::getStep(Ids::NodeId stepId) {
    return const_cast<FlowStep*>(static_cast<const FlowDocument*>(this)->getStep(stepId));
}

const FlowStep* FlowDocument::getStep(Ids::NodeId stepId) const {
    for (const FlowGroup& group : m_groups)
        for (const FlowRow& row : group.rows)
            if (const FlowStep* step = findStepIn(row.steps, stepId); step != nullptr)
                return step;
    return nullptr;
}

FlowRow* FlowDocument::rowForStep(Ids::NodeId stepId) {
    return const_cast<FlowRow*>(static_cast<const FlowDocument*>(this)->rowForStep(stepId));
}

const FlowRow* FlowDocument::rowForStep(Ids::NodeId stepId) const {
    for (const FlowGroup& group : m_groups)
        for (const FlowRow& row : group.rows)
            if (findStepIn(row.steps, stepId) != nullptr)
                return &row;
    return nullptr;
}

} // namespace ScopeCanvas::Core::Flow
