#pragma once

#include "Engine/DiagramModel.h"

#include <glm/vec2.hpp>

class SelectionController {
public:
    void onMouseDown(DiagramModel& model, const glm::vec2& mouseWorld, bool shiftDown);
    void onMouseDrag(DiagramModel& model, const glm::vec2& mouseWorld);
    void onMouseUp(DiagramModel& model, const glm::vec2& mouseWorld);

    bool isBoxSelecting() const { return m_boxSelecting; }
    glm::vec2 boxStart() const { return m_boxStart; }
    glm::vec2 boxEnd() const { return m_boxEnd; }

private:
    static Node* hitTest(DiagramModel& model, const glm::vec2& mouseWorld);
    static bool intersectsSelectionRect(const Node& node, const glm::vec2& boxStart, const glm::vec2& boxEnd);

    void updateBoxSelection(DiagramModel& model);

    bool m_boxSelecting = false;
    glm::vec2 m_boxStart = glm::vec2(0.0f);
    glm::vec2 m_boxEnd = glm::vec2(0.0f);
};
