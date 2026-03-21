#include "Layout/GridSettings.h"

#include <TestAssert.h>
#include <glm/vec2.hpp>

int main() {
    using namespace ScopeCanvas::Core;

    GridSettings grid{};
    grid.enabled = true;
    grid.cellSize = 10.0f;

    SC_TEST((grid.snap(glm::vec2{12.0f, 26.0f}) == glm::vec2{10.0f, 30.0f}));

    grid.enabled = false;

    SC_TEST((grid.snap(glm::vec2{12.0f, 26.0f}) == glm::vec2{12.0f, 26.0f}));

    return 0;
}