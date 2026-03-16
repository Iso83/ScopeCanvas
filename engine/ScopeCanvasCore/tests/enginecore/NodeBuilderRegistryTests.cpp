#include "ScopeCanvasEngineCore/Builders/NodeBuilderRegistry.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    NodeBuilderRegistry registry{};
    bool called = false;

    registry.registerBuilder(NodeTypeId{42}, [&called]() {
        called = true;
    });

    const NodeBuilderRegistry::BuilderCallback* builder = registry.getBuilder(NodeTypeId{42});
    if (builder == nullptr)
    {
        return 1;
    }

    (*builder)();
    return called ? 0 : 1;
}
