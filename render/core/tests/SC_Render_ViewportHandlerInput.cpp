#include <ScopeCanvas/input/InputCodes.h>
#include <ScopeCanvas/render/window/ViewportHandler.h>
#include <TestAssert.h>

using namespace ScopeCanvas::Input;
using namespace ScopeCanvas::Render::Window;

namespace {
int test_key_state_transitions() {
    ViewportHandler handler{};

    SC_TEST(!handler.keyState(Key::Delete).down);
    SC_TEST(!handler.keyState(Key::Delete).pressed());

    handler.processKey(Key::Delete, true);
    SC_TEST(handler.keyState(Key::Delete).down);
    SC_TEST(handler.keyState(Key::Delete).pressed());
    SC_TEST(!handler.keyState(Key::Delete).released());

    handler.updatePrevInteraction();
    SC_TEST(handler.keyState(Key::Delete).down);
    SC_TEST(!handler.keyState(Key::Delete).pressed());

    handler.processKey(Key::Delete, false);
    SC_TEST(!handler.keyState(Key::Delete).down);
    SC_TEST(handler.keyState(Key::Delete).released());
    return 0;
}

int test_unknown_key_is_ignored() {
    ViewportHandler handler{};
    handler.processKey(Key::Unknown, true);
    SC_TEST(!handler.keyState(Key::Unknown).down);
    return 0;
}
} // namespace

int main() {
    SC_RUN_TEST(test_key_state_transitions);
    SC_RUN_TEST(test_unknown_key_is_ignored);
    return 0;
}
