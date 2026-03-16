#pragma once

namespace glm {

struct vec2 {
    float x;
    float y;

    constexpr vec2() : x(0.0f), y(0.0f) {}
    constexpr explicit vec2(float v) : x(v), y(v) {}
    constexpr vec2(float px, float py) : x(px), y(py) {}

    constexpr vec2 &operator+=(const vec2 &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr vec2 &operator-=(const vec2 &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
};

constexpr inline vec2 operator+(vec2 a, const vec2 &b) {
    a += b;
    return a;
}

constexpr inline vec2 operator-(vec2 a, const vec2 &b) {
    a -= b;
    return a;
}

constexpr inline vec2 operator*(const vec2 &a, float s) {
    return vec2{ a.x * s, a.y * s };
}

constexpr inline vec2 operator*(float s, const vec2 &a) {
    return vec2{ a.x * s, a.y * s };
}

} // namespace glm
