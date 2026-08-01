#pragma once

#include <ScopeCanvas/engine/core/ids/StrongId.h>

namespace ScopeCanvas::Engine::Core::Flow {
struct FlowGroupIdTag;
struct FlowRowIdTag;

using FlowGroupId = Ids::StrongId<FlowGroupIdTag>;
using FlowRowId = Ids::StrongId<FlowRowIdTag>;
} // namespace ScopeCanvas::Core::Flow
