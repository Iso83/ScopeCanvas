#pragma once

#include <ScopeCanvas/core/ids/StrongId.h>

namespace ScopeCanvas::Core::Flow {
struct FlowGroupIdTag;
struct FlowRowIdTag;

using FlowGroupId = Ids::StrongId<FlowGroupIdTag>;
using FlowRowId = Ids::StrongId<FlowRowIdTag>;
} // namespace ScopeCanvas::Core::Flow
