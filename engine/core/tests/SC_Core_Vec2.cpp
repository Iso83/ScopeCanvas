#include <ScopeCanvas/core/Vec2.h>
#include <TestAssert.h>

int main() {
    using namespace ScopeCanvas::Core;

    const Vec2 a{};
    const Vec2 b{1.0f, 2.0f};
    const Vec2 c{1.0f, 2.0f};

    SC_TEST(a == Vec2{});
    SC_TEST(b == c);
    SC_TEST(b != a);

    return 0;
}