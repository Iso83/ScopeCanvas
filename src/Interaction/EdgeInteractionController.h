#pragma once

#include "Engine/DiagramModel.h"

#include <glm/vec2.hpp>

#include <cstdint>
#include <vector>

struct EdgeEndpointHit {
    uint32_t edgeId = 0;
    bool hit = false;
    bool startEndpoint = false;
};

class EdgeInteractionController {
public:
    static Edge* hitTestEdge(DiagramModel& model, const glm::vec2& mouseWorld, float zoom);
    static const Edge* hitTestEdge(const DiagramModel& model, const glm::vec2& mouseWorld, float zoom);

    static const Connector* hitTestConnector(const DiagramModel& model,
                                             const glm::vec2& mouseWorld,
                                             float zoom,
                                             uint32_t* outNodeId = nullptr);

    static EdgeEndpointHit hitTestEdgeEndpoint(const DiagramModel& model,
                                               const glm::vec2& mouseWorld,
                                               float zoom);

private:
    static constexpr int kBezierSegments = 20;
    static constexpr float kHitThresholdPixels = 8.0f;

    static void appendBezierSamples(std::vector<glm::vec2>& points,
                                    const glm::vec2& p0,
                                    const glm::vec2& p1,
                                    const glm::vec2& p2,
                                    const glm::vec2& p3);

    static void computeBezierControls(const glm::vec2& p0,
                                      const glm::vec2& p3,
                                      glm::vec2& p1,
                                      glm::vec2& p2);

    static float distanceToSegment(const glm::vec2& point,
                                   const glm::vec2& segmentStart,
                                   const glm::vec2& segmentEnd);
};
