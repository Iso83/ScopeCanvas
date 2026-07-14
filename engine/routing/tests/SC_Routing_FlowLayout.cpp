#include <ScopeCanvas/core/flow/FlowDocument.h>
#include <ScopeCanvas/routing/flow/FlowLayout.h>
#include <TestAssert.h>

using namespace ScopeCanvas::Core::Flow;
using namespace ScopeCanvas::Core::Ids;
using namespace ScopeCanvas::Routing::Flow;

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

    SC_TEST(layout.step(fixture.parentId) != nullptr);
    SC_TEST(layout.step(fixture.childId) == nullptr);
    SC_TEST(layout.step(fixture.secondId) != nullptr);
    return 0;
}

int test_collapsed_group_excludes_rows_and_steps() {
    Fixture fixture = makeFixture();
    fixture.document.toggleGroupCollapsed(fixture.groupId);

    const FlowLayoutResult layout = FlowLayout{}.build(fixture.document);

    SC_TEST(layout.groups.size() == 1U);
    SC_TEST(layout.rows.empty());
    SC_TEST(layout.steps.empty());
    return 0;
}

int test_subtree_bounds_include_visible_children() {
    Fixture fixture = makeFixture();
    const FlowLayoutResult layout = FlowLayout{}.build(fixture.document);

    const FlowStepLayout* parent = layout.step(fixture.parentId);
    const FlowStepLayout* child = layout.step(fixture.childId);
    SC_TEST(parent != nullptr);
    SC_TEST(child != nullptr);
    SC_TEST(parent->subtreePosition.x <= child->position.x);
    SC_TEST(parent->subtreePosition.y <= child->position.y);
    SC_TEST(parent->subtreePosition.x + parent->subtreeSize.x >= child->position.x + child->size.x);
    SC_TEST(parent->subtreePosition.y + parent->subtreeSize.y >= child->position.y + child->size.y);
    return 0;
}

int test_insertion_index_uses_top_level_steps_for_row() {
    Fixture fixture = makeFixture();
    const FlowLayoutResult layout = FlowLayout{}.build(fixture.document);
    const FlowStepLayout* parent = layout.step(fixture.parentId);
    const FlowStepLayout* second = layout.step(fixture.secondId);
    SC_TEST(parent != nullptr);
    SC_TEST(second != nullptr);

    const float betweenTopLevelSteps = (parent->railAnchor.x + second->railAnchor.x) * 0.5F;
    SC_TEST(FlowLayout{}.insertionIndex(layout, fixture.rowId, betweenTopLevelSteps) == 1U);
    SC_TEST(FlowLayout{}.insertionIndex(layout, FlowRowId{999}, betweenTopLevelSteps) == 0U);
    return 0;
}
} // namespace

int main() {
    SC_RUN_TEST(test_collapsed_parent_excludes_children);
    SC_RUN_TEST(test_collapsed_group_excludes_rows_and_steps);
    SC_RUN_TEST(test_subtree_bounds_include_visible_children);
    SC_RUN_TEST(test_insertion_index_uses_top_level_steps_for_row);
    return 0;
}
