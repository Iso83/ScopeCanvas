#pragma once

#include <ScopeCanvas/core/ids/CanvasIds.h>
#include <vector>

namespace ScopeCanvas::Core {
class GraphDocument;

class LayoutEngine {
  public:
    virtual ~LayoutEngine() = default;

    virtual void layout(GraphDocument& model) = 0;
    virtual void layoutNodes(GraphDocument& model, const std::vector<CanvasNodeId>& nodes) = 0;
};
} // namespace ScopeCanvas::Core
