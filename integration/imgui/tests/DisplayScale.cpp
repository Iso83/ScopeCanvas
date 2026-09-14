#include "TestAssert.h"

#include <ScopeCanvas/integration/imgui/DisplayScale.h>
#include <cmath>
#include <limits>

using ScopeCanvas::Integration::ImGui::DisplayMetrics;
using ScopeCanvas::Integration::ImGui::DisplayScale;
using ScopeCanvas::Integration::ImGui::effectiveDisplayScale;

namespace {
bool close(float left, float right) {
    return std::abs(left - right) < 0.0001F;
}
} // namespace

int test_effective_scale_uses_larger_valid_source() {
    DisplayMetrics metrics{1.25F, 1.5F, 800, 600, 1600, 900};
    CPPTEST_ASSERT(close(effectiveDisplayScale(metrics), 2.0F));

    metrics = {2.0F, 2.0F, 800, 600, 1200, 900};
    CPPTEST_ASSERT(close(effectiveDisplayScale(metrics), 2.0F));
    return 0;
}

int test_invalid_or_minimized_dimensions_are_safe() {
    const auto nan = std::numeric_limits<float>::quiet_NaN();
    const DisplayMetrics metrics{nan, -1.0F, 0, 0, 1920, 1080};
    CPPTEST_ASSERT(close(effectiveDisplayScale(metrics), 1.0F));
    return 0;
}

int test_tolerance_and_non_cumulative_scaling() {
    auto* context = ::ImGui::CreateContext();
    auto baseline = ::ImGui::GetStyle();
    baseline.WindowPadding = {10.0F, 12.0F};
    ::ImGui::GetStyle() = baseline;
    DisplayScale displayScale{baseline};

    const DisplayMetrics scale150{1.5F, 1.5F, 800, 600, 1200, 900};
    CPPTEST_ASSERT(displayScale.update(scale150, ::ImGui::GetStyle(), ::ImGui::GetIO()));
    CPPTEST_ASSERT(close(::ImGui::GetStyle().WindowPadding.x, 15.0F));
    CPPTEST_ASSERT(close(::ImGui::GetIO().FontGlobalScale, 1.5F));
    CPPTEST_ASSERT(!displayScale.update(scale150, ::ImGui::GetStyle(), ::ImGui::GetIO()));
    CPPTEST_ASSERT(close(::ImGui::GetStyle().WindowPadding.x, 15.0F));

    const DisplayMetrics withinTolerance{1.505F, 1.505F, 800, 600, 1204, 903};
    CPPTEST_ASSERT(!displayScale.update(withinTolerance, ::ImGui::GetStyle(), ::ImGui::GetIO()));

    const DisplayMetrics scale100{1.0F, 1.0F, 800, 600, 800, 600};
    CPPTEST_ASSERT(displayScale.update(scale100, ::ImGui::GetStyle(), ::ImGui::GetIO()));
    CPPTEST_ASSERT(close(::ImGui::GetStyle().WindowPadding.x, 10.0F));
    CPPTEST_ASSERT(close(::ImGui::GetStyle().WindowPadding.y, 12.0F));
    CPPTEST_ASSERT(close(::ImGui::GetIO().FontGlobalScale, 1.0F));

    ::ImGui::DestroyContext(context);
    return 0;
}

int main() {
    CPPTEST_RUN(test_effective_scale_uses_larger_valid_source);
    CPPTEST_RUN(test_invalid_or_minimized_dimensions_are_safe);
    CPPTEST_RUN(test_tolerance_and_non_cumulative_scaling);
    return 0;
}
