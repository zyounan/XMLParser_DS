#ifndef LAYOUT_HPP
#define LAYOUT_HPP
#include <cppurses/cppurses.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/textbox.hpp>
namespace Editor {
using namespace cppurses;
class Editor_menu : public cppurses::layout::Horizontal {
    private:
        Menu_stack& menu_stack = this->make_child<Menu_stack>()
};
class Editor_text_area : public cppurses::layout::Vertical {};
class Editor_output : public cppurses::layout::Vertical {};
class Editor : public cppurses::layout::Vertical {
   private:
    Editor_menu& editor_menu{this->make_child<Editor_menu>()};
    Editor_text_area& editor_text_area{this->make_child<Editor_text_area>()};
    Editor_output& editor_output{this->make_child<Editor_output>()};
   protected:
    bool focus_in_event() override;
   public:
    Editor() { this->initialize(); }
    void initialize() {}
};
}  // namespace Editor
#endif