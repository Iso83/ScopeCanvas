#pragma once

namespace ScopeCanvas::Core
{
struct Vec2
{
    float x{};
    float y{};

    constexpr Vec2() = default;
    constexpr Vec2(float xValue, float yValue) : x(xValue), y(yValue) {}

    friend constexpr bool operator==(const Vec2& lhs, const Vec2& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    friend constexpr bool operator!=(const Vec2& lhs, const Vec2& rhs)
    {
        return !(lhs == rhs);
    }
};
} // namespace ScopeCanvas::Core
