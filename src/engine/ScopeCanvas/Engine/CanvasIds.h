#pragma once

#include <cstdint>

struct CanvasNodeId {
    uint32_t value = 0;
    constexpr CanvasNodeId() = default;
    constexpr CanvasNodeId(uint32_t v) : value(v) {}
    constexpr explicit operator bool() const { return value != 0; }
    constexpr operator uint32_t() const { return value; }
    constexpr bool operator==(const CanvasNodeId &) const = default;
};

struct CanvasConnectorId {
    uint32_t value = 0;
    constexpr CanvasConnectorId() = default;
    constexpr CanvasConnectorId(uint32_t v) : value(v) {}
    constexpr explicit operator bool() const { return value != 0; }
    constexpr operator uint32_t() const { return value; }
    constexpr bool operator==(const CanvasConnectorId &) const = default;
};

struct CanvasEdgeId {
    uint32_t value = 0;
    constexpr CanvasEdgeId() = default;
    constexpr CanvasEdgeId(uint32_t v) : value(v) {}
    constexpr explicit operator bool() const { return value != 0; }
    constexpr operator uint32_t() const { return value; }
    constexpr bool operator==(const CanvasEdgeId &) const = default;
};

struct CanvasViewId {
    uint32_t value = 0;
    constexpr CanvasViewId() = default;
    constexpr CanvasViewId(uint32_t v) : value(v) {}
    constexpr explicit operator bool() const { return value != 0; }
    constexpr operator uint32_t() const { return value; }
    constexpr bool operator==(const CanvasViewId &) const = default;
};

