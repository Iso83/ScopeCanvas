#include "TestAssert.h"

#include <ScopeCanvas/engine/core/flow/FlowDocument.h>
#include <ScopeCanvas/engine/routing/flow/FlowLayout.h>

using namespace ScopeCanvas::Engine::Core::Flow;
using namespace ScopeCanvas::Engine::Core::Ids;
using namespace ScopeCanvas::Engine::Routing::Flow;

namespace {
struct Fixture {
    FlowDocument document{};
    FlowGroupId groupId{};
    FlowRowId rowId{};
    NodeId parentId{};
    NodeId childId{};
    NodeId secondId{};
};

Fixture makeFixture() {
    Fixture fixture{};
    FlowGroup& group = fixture.document.createGroup("Root");
    fixture.groupId = group.id;
    FlowRow& row = fixture.document.createRow(group, "Main");
    fixture.rowId = row.id;
    FlowStep& parent = fixture.document.insertStep(row, 0, NodeTypeId{1}, "Parent", "First");
    fixture.parentId = parent.id;
    FlowStep& child = fixture.document.addChildStep(parent, NodeTypeId{2}, "Child", "Nested");
    fixture.childId = child.id;
    fixture.secondId = fixture.document.insertStep(row, 1, NodeTypeId{3}, "Second", "Next").id;
    return fixture;
}

int test_collapsed_parent_excludes_children() {
    Fixture fixture = makeFixture();
    fixture.document.toggleCollapsed(fixture.parentId);

    const FlowLayoutResult layout = FlowLayout{}.build(fixture.document);

    CPPTEST_ASSERT(layout.step(fixture.parentId) != nullptr);
    CPPTEST_ASSERT(layout.step(fixture.childId) == nullptr);
    CPPTEST_ASSERT(layout.step(fixture.secondId) != nullptr);
    return 0;
}

int test_collapsed_group_excludes_rows_and_steps() {
    Fixture fixture = makeFixture();
    fixture.document.toggleGroupCollapsed(fixture.groupId);

    const FlowLayoutResult layout = FlowLayout{}.build(fixture.document);

    CPPTEST_ASSERT(layout.groups.size() == 1U);
    CPPTEST_ASSERT(layout.rows.empty());
    CPPTEST_ASSERT(layout.steps.empty());
    return 0;
}

int test_subtree_bounds_include_visible_children() {
    Fixture fixture = makeFixture();
    const FlowLayoutResult layout = FlowLayout{}.build(fixture.document);

    const FlowStepLayout* parent = layout.step(fixture.parentId);
    const FlowStepLayout* child = layout.step(fixture.childId);
    CPPTEST_ASSERT(parent != nullptr);
    CPPTEST_ASSERT(child != nullptr);
    CPPTEST_ASSERT(parent->subtreePosition.x <= child->position.x);
    CPPTEST_ASSERT(parent->subtreePosition.y <= child->position.y);
    CPPTEST_ASSERT(parent->subtreePosition.x + parent->subtreeSize.x >= child->position.x + child->size.x);
    CPPTEST_ASSERT(parent->subtreePosition.y + parent->subtreeSize.y >= child->position.y + child->size.y);
    return 0;
}

int test_step_width_expands_for_long_labels() {
    FlowDocument document{};
    FlowGroup& group = document.createGroup("Root");
    FlowRow& row = document.createRow(group, "Main");
    const NodeId stepId = document.insertStep(row, 0, NodeTypeId{1}, "A very long customer import title", "Details").id;

    const FlowLayoutResult layout = FlowLayout{}.build(document);

    const FlowStepLayout* step = layout.step(stepId);
    CPPTEST_ASSERT(step != nullptr);
    CPPTEST_ASSERT(step->size.x > FlowLayoutOptions{}.stepSize.x);
    return 0;
}

int test_insertion_index_uses_top_level_steps_for_row() {
    Fixture fixture = makeFixture();
    const FlowLayoutResult layout = FlowLayout{}.build(fixture.document);
    const FlowStepLayout* parent = layout.step(fixture.parentId);
    const FlowStepLayout* second = layout.step(fixture.secondId);
    CPPTEST_ASSERT(parent != nullptr);
    CPPTEST_ASSERT(second != nullptr);

    const float betweenTopLevelSteps = (parent->railAnchor.x + second->railAnchor.x) * 0.5F;
    CPPTEST_ASSERT(FlowLayout{}.insertionIndex(layout, fixture.rowId, betweenTopLevelSteps) == 1U);
    CPPTEST_ASSERT(FlowLayout{}.insertionIndex(layout, FlowRowId{999}, betweenTopLevelSteps) == 0U);
    return 0;
}
} // namespace

int main() {
    CPPTEST_RUN(test_collapsed_parent_excludes_children);
    CPPTEST_RUN(test_collapsed_group_excludes_rows_and_steps);
    CPPTEST_RUN(test_subtree_bounds_include_visible_children);
    CPPTEST_RUN(test_step_width_expands_for_long_labels);
    CPPTEST_RUN(test_insertion_index_uses_top_level_steps_for_row);
    return 0;
}
