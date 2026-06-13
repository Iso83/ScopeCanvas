#include <ScopeCanvas/core/Connector.h>
#include <ScopeCanvas/core/Edge.h>
#include <ScopeCanvas/core/Node.h>
#include <ScopeCanvas/routing/EdgeRouter.h>
#include <ScopeCanvas/routing/IGraphView.h>
#include <algorithm>
#include <glm/geometric.hpp>

namespace ScopeCanvas::Routing {
std::vector<EdgeRoute> EdgeRouter::routeAll(const IGraphView* view) const {
    std::vector<EdgeRoute> routes;
    if (view == nullptr)
        return routes;

    view->forEachEdgeId([&](auto edgeId) {
        const auto* edge = view->getEdge(edgeId);
        if (edge == nullptr)
            return;

        const ScopeCanvas::Core::Connector* from = view->getConnector(edge->fromConnector);
        const ScopeCanvas::Core::Connector* to = view->getConnector(edge->toConnector);
        if (from == nullptr || to == nullptr)
            return;

        const ScopeCanvas::Core::Node* fromNode = view->getNode(from->nodeId);
        const ScopeCanvas::Core::Node* toNode = view->getNode(to->nodeId);
        if (fromNode == nullptr || toNode == nullptr || fromNode->connectors.empty() || toNode->connectors.empty())
            return;

        std::size_t fromIndex = 0;
        while (fromIndex < fromNode->connectors.size() && fromNode->connectors[fromIndex] != edge->fromConnector)
            ++fromIndex;

        std::size_t toIndex = 0;
        while (toIndex < toNode->connectors.size() && toNode->connectors[toIndex] != edge->toConnector)
            ++toIndex;

        const std::size_t safeFromIndex = std::min(fromIndex, fromNode->connectors.size() - 1U);
        const std::size_t safeToIndex = std::min(toIndex, toNode->connectors.size() - 1U);
        const glm::vec2 start = view->connectorWorld(*fromNode, safeFromIndex);
        const glm::vec2 end = view->connectorWorld(*toNode, safeToIndex);
        const glm::vec2 startNormal = (safeFromIndex % 2U) == 1U ? glm::vec2(1.0F, 0.0F) : glm::vec2(-1.0F, 0.0F);
        const glm::vec2 endNormal = (safeToIndex % 2U) == 1U ? glm::vec2(1.0F, 0.0F) : glm::vec2(-1.0F, 0.0F);
        constexpr float kBreakout = 48.0F;
        constexpr float kDetourMargin = 36.0F;
        EdgeRoute route{};
        route.edgeId = edge->id;
        const glm::vec2 startStub = start + startNormal * kBreakout;
        const glm::vec2 endStub = end + endNormal * kBreakout;
        if (startNormal.x == endNormal.x) {
            route.points = {start, startStub, {startStub.x, end.y}, end};
            routes.push_back(route);
            return;
        }
        const bool needsDetour = startNormal.x > 0.0F && endNormal.x < 0.0F && end.x <= start.x + kBreakout * 1.5F;
        if (needsDetour) {
            const float fromTop = fromNode->position.y;
            const float fromBottom = fromNode->position.y + fromNode->size.y;
            const float toTop = toNode->position.y;
            const float toBottom = toNode->position.y + toNode->size.y;
            const float detourY = end.y >= start.y ? std::max(fromBottom, toBottom) + kDetourMargin
                                                   : std::min(fromTop, toTop) - kDetourMargin;
            route.points = {start, startStub, {startStub.x, detourY}, {endStub.x, detourY}, endStub, end};
        } else if (glm::dot(end - start, startNormal) < kBreakout)
            route.points = {start, startStub, {startStub.x, end.y}, endStub, end};
        else
            route.points = {start, end};
        routes.push_back(route);
    });

    return routes;
}
} // namespace ScopeCanvas::Routing
