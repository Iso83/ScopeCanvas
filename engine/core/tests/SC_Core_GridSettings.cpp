#include "Layout/GridSettings.h"

#include <ScopeCanvas/core/Vec2.h>
#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    GridSettings grid{};
    grid.enabled = true;
    grid.cellSize = 10.0f;

    SC_TEST((grid.snap(Vec2{12.0f, 26.0f}) == Vec2{10.0f, 30.0f}));

    grid.enabled = false;

    SC_TEST((grid.snap(Vec2{12.0f, 26.0f}) == Vec2{12.0f, 26.0f}));

    return 0;
}