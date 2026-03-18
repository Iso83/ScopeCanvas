#pragma once

#include <vector>

#include <ScopeCanvas/core/ids/CanvasIds.h>

namespace ScopeCanvas::Core
{
class DiagramModel;

class LayoutEngine
{
public:
    virtual ~LayoutEngine() = default;

    virtual void layout(DiagramModel& model) = 0;
    virtual void layoutNodes(DiagramModel& model, const std::vector<CanvasNodeId>& nodes) = 0;
};
} // namespace ScopeCanvas::Core
