#include <ScopeCanvas/core/flow/FlowDocument.h>
#include <TestAssert.h>

using namespace ScopeCanvas::Core::Flow;
using namespace ScopeCanvas::Core::Ids;

namespace {
struct Fixture {
    FlowDocument document{};
    NodeId firstId{};
    NodeId secondId{};
    NodeId childId{};
};

Fixture makeFixture() {
    Fixture fixture{};
    FlowGroup& group = fixture.document.createGroup("Root");
    FlowRow& row = fixture.document.createRow(group, "Main");
    fixture.firstId = fixture.document.insertStep(row, 0, NodeTypeId{1}, "First", "Root step").id;
    fixture.secondId = fixture.document.insertStep(row, 1, NodeTypeId{2}, "Second", "Root step").id;
    if (FlowStep* first = fixture.document.getStep(fixture.firstId); first != nullptr)
        fixture.childId = fixture.document.addChildStep(*first, NodeTypeId{3}, "Child", "Nested step").id;
    return fixture;
}

int test_recursive_step_lookup() {
    Fixture fixture = makeFixture();
    const FlowStep* first = fixture.document.getStep(fixture.firstId);
    SC_TEST(first != nullptr);
    SC_TEST(first->title == "First");
    SC_TEST(first->children.size() == 1U);

    const FlowStep* child = fixture.document.getStep(fixture.childId);
    SC_TEST(child != nullptr);
    SC_TEST(child->title == "Child");
    return 0;
}

int test_remove_nested_step() {
    Fixture fixture = makeFixture();
    fixture.document.removeStep(fixture.childId);

    SC_TEST(fixture.document.getStep(fixture.childId) == nullptr);
    const FlowStep* first = fixture.document.getStep(fixture.firstId);
    SC_TEST(first != nullptr);
    SC_TEST(first->children.empty());
    return 0;
}

int test_move_top_level_step() {
    Fixture fixture = makeFixture();
    fixture.document.moveStep(fixture.secondId, -1);

    const FlowGroup& group = fixture.document.groups().front();
    const FlowRow& row = group.rows.front();
    SC_TEST(row.steps.size() == 2U);
    SC_TEST(row.steps[0].id == fixture.secondId);
    SC_TEST(row.steps[1].id == fixture.firstId);
    return 0;
}
} // namespace

int main() {
    SC_RUN_TEST(test_recursive_step_lookup);
    SC_RUN_TEST(test_remove_nested_step);
    SC_RUN_TEST(test_move_top_level_step);
    return 0;
}
