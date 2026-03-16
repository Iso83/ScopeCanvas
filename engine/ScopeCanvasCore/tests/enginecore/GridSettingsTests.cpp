#include "ScopeCanvasEngineCore/Core/Vec2.h"
#include "ScopeCanvasEngineCore/Layout/GridSettings.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    GridSettings enabledGrid{};
    enabledGrid.enabled = true;
    enabledGrid.cellSize = 10.0F;

    const Vec2 snapped = enabledGrid.snap(Vec2{12.0F, 26.0F});
    if (!(snapped == Vec2{10.0F, 30.0F}))
    {
        return 1;
    }

    GridSettings disabledGrid{};
    disabledGrid.enabled = false;
    disabledGrid.cellSize = 10.0F;

    const Vec2 unchanged = disabledGrid.snap(Vec2{12.0F, 26.0F});
    if (!(unchanged == Vec2{12.0F, 26.0F}))
    {
        return 1;
    }

    return 0;
}
