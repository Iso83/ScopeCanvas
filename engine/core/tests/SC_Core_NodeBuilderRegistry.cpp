#include "builders/NodeBuilderRegistry.h"

#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    NodeBuilderRegistry registry{};
    bool called = false;

    registry.registerBuilder(NodeTypeId{42}, [&called]() { called = true; });

    const auto* builder = registry.getBuilder(NodeTypeId{42});

    SC_TEST(builder != nullptr);

    (*builder)();

    SC_TEST(called);

    return 0;
}