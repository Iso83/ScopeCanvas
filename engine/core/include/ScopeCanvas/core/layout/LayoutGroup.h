#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <ScopeCanvas/core/ids/TypeIds.h>
#include <vector>

namespace ScopeCanvas::Core {
struct LayoutGroup {
    LayoutGroupId id{};
    std::vector<CanvasNodeId> nodes{};
    bool preserveInternalLayout{};
};
} // namespace ScopeCanvas::Core
