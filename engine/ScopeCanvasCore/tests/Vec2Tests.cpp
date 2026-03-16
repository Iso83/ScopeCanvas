#include "ScopeCanvasEngineCore/Core/Vec2.h"

int main()
{
    using namespace ScopeCanvas::Engine::Core;

    const Vec2 a{};
    const Vec2 b{1.0F, 2.0F};
    const Vec2 c{1.0F, 2.0F};

    return (a == Vec2{} && b == c && b != a) ? 0 : 1;
}
