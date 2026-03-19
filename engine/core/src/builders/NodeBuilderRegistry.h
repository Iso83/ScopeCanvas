#pragma once

#include <ScopeCanvas/core/ids/TypeIds.h>
#include <functional>
#include <vector>

namespace ScopeCanvas::Core {
class NodeBuilderRegistry {
  public:
    using BuilderCallback = std::function<void()>;

    void registerBuilder(NodeTypeId typeId, BuilderCallback builder);
    const BuilderCallback* getBuilder(NodeTypeId typeId) const;

  private:
    struct Entry {
        NodeTypeId typeId{};
        BuilderCallback builder{};
    };

    std::vector<Entry> m_entries{};
};
} // namespace ScopeCanvas::Core
