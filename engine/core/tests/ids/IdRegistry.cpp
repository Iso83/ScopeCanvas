#include "TestAssert.h"

#include <ScopeCanvas/engine/core/ids/IdRegistry.h>
#include <ScopeCanvas/engine/core/ids/StrongId.h>

using namespace ScopeCanvas::Engine::Core;
using namespace ScopeCanvas::Engine::Core::Ids;

struct TestIdTag;
using TestId = StrongId<TestIdTag>;

int test_create_sequence() {
    GraphRegistry<TestId> reg;

    auto a = reg.createId();
    auto b = reg.createId();
    auto c = reg.createId();

    CPPTEST_ASSERT(a.value() == 1);
    CPPTEST_ASSERT(b.value() == 2);
    CPPTEST_ASSERT(c.value() == 3);

    return 0;
}

int test_contains() {
    GraphRegistry<TestId> reg;

    auto id = reg.createId();

    CPPTEST_ASSERT(reg.contains(id) == true);
    CPPTEST_ASSERT(reg.contains(TestId{999}) == false);

    return 0;
}

int test_remove() {
    GraphRegistry<TestId> reg;

    auto id = reg.createId();

    CPPTEST_ASSERT(reg.contains(id) == true);
    CPPTEST_ASSERT(reg.remove(id) == true);
    CPPTEST_ASSERT(reg.contains(id) == false);

    return 0;
}

int test_remove_invalid() {
    GraphRegistry<TestId> reg;

    CPPTEST_ASSERT(reg.remove(TestId{123}) == false);

    return 0;
}

int test_reuse_id() {
    GraphRegistry<TestId> reg;

    auto a = reg.createId();
    auto b = reg.createId();

    reg.remove(a);

    auto c = reg.createId();

    CPPTEST_ASSERT(c.value() == a.value()); // reuse
    CPPTEST_ASSERT(reg.contains(c) == true);

    return 0;
}

int test_multiple_reuse_order() {
    GraphRegistry<TestId> reg;

    auto a = reg.createId(); // 1
    auto b = reg.createId(); // 2
    auto c = reg.createId(); // 3

    reg.remove(b);
    reg.remove(a);

    auto d = reg.createId(); // verwacht a (LIFO free list)
    auto e = reg.createId(); // verwacht b

    CPPTEST_ASSERT(d.value() == a.value());
    CPPTEST_ASSERT(e.value() == b.value());

    return 0;
}

int main() {
    CPPTEST_RUN(test_create_sequence);
    CPPTEST_RUN(test_contains);
    CPPTEST_RUN(test_remove);
    CPPTEST_RUN(test_remove_invalid);
    CPPTEST_RUN(test_reuse_id);
    CPPTEST_RUN(test_multiple_reuse_order);

    return 0;
}
