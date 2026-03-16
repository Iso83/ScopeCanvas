#pragma once

#include <functional>
#include <vector>

#include "ScopeCanvasEngineCore/Ids/TypeIds.h"

namespace ScopeCanvas::Engine::Core
{
class NodeBuilderRegistry
{
public:
    using BuilderCallback = std::function<void()>;

    void registerBuilder(NodeTypeId typeId, BuilderCallback builder);
    const BuilderCallback* getBuilder(NodeTypeId typeId) const;

private:
    struct Entry
    {
        NodeTypeId typeId{};
        BuilderCallback builder{};
    };

    std::vector<Entry> m_entries{};
};
} // namespace ScopeCanvas::Engine::Core
