#include "builders/NodeBuilderRegistry.h"

#include <algorithm>

namespace ScopeCanvas::Core
{
void NodeBuilderRegistry::registerBuilder(NodeTypeId typeId, BuilderCallback builder)
{
    auto it = std::find_if(m_entries.begin(), m_entries.end(), [typeId](const Entry& entry) {
        return entry.typeId == typeId;
    });

    if (it != m_entries.end())
    {
        it->builder = std::move(builder);
        return;
    }

    m_entries.push_back(Entry{typeId, std::move(builder)});
}

const NodeBuilderRegistry::BuilderCallback* NodeBuilderRegistry::getBuilder(NodeTypeId typeId) const
{
    const auto it = std::find_if(m_entries.begin(), m_entries.end(), [typeId](const Entry& entry) {
        return entry.typeId == typeId;
    });

    return it == m_entries.end() ? nullptr : &it->builder;
}
} // namespace ScopeCanvas::Core
