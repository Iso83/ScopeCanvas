#pragma once

#include <cstddef>

namespace ScopeCanvas::Input {
enum class Key : std::size_t {
    Unknown = 0,
    Escape,
    Delete,
    Digit1,
    Digit2,
    Digit3,
    Digit4,
    A,
    D,
    W,
    S,
    G,
    Left,
    Right,
    Up,
    Down,
    Count
};

constexpr std::size_t MaxKeys = static_cast<std::size_t>(Key::Count);
} // namespace ScopeCanvas::Input
