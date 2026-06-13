#include <ScopeCanvas/core/ids/IdRegistry.h>
#include <ScopeCanvas/core/ids/StrongId.h>
#include <TestAssert.h>

using namespace ScopeCanvas::Core;
using namespace ScopeCanvas::Core::Ids;

struct TestIdTag;
using TestId = StrongId<TestIdTag>;

int test_create_sequence() {
    GraphRegistry<TestId> reg;

    auto a = reg.createId();
    auto b = reg.createId();
    auto c = reg.createId();

    SC_TEST(a.value() == 1);
    SC_TEST(b.value() == 2);
    SC_TEST(c.value() == 3);

    return 0;
}

int test_contains() {
    GraphRegistry<TestId> reg;

    auto id = reg.createId();

    SC_TEST(reg.contains(id) == true);
    SC_TEST(reg.contains(TestId{999}) == false);

    return 0;
}

int test_remove() {
    GraphRegistry<TestId> reg;

    auto id = reg.createId();

    SC_TEST(reg.contains(id) == true);
    SC_TEST(reg.remove(id) == true);
    SC_TEST(reg.contains(id) == false);

    return 0;
}

int test_remove_invalid() {
    GraphRegistry<TestId> reg;

    SC_TEST(reg.remove(TestId{123}) == false);

    return 0;
}

int test_reuse_id() {
    GraphRegistry<TestId> reg;

    auto a = reg.createId();
    auto b = reg.createId();

    reg.remove(a);

    auto c = reg.createId();

    SC_TEST(c.value() == a.value()); // reuse
    SC_TEST(reg.contains(c) == true);

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

    SC_TEST(d.value() == a.value());
    SC_TEST(e.value() == b.value());

    return 0;
}

int main() {
    SC_RUN_TEST(test_create_sequence);
    SC_RUN_TEST(test_contains);
    SC_RUN_TEST(test_remove);
    SC_RUN_TEST(test_remove_invalid);
    SC_RUN_TEST(test_reuse_id);
    SC_RUN_TEST(test_multiple_reuse_order);

    return 0;
}