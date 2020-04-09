#include <bits/stdc++.h>
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
class Editor_menu_help : public cppurses::layout::Vertical {
   public:
    Editor_menu_help() {}
};
class Editor_menu : public cppurses::layout::Horizontal {
   public:
    Menu_stack& menu_stack = this->make_child<Menu_stack>();
    Editor_menu_help& menu_help =
        menu_stack.make_page<Editor_menu_help>("Help");

    Editor_menu() { this->height_policy.fixed(1); }
};

class Editor : public cppurses::layout::Vertical {
   private:
    Glyph_string glyph;
    std::string __load_file(const std::string& filename) {
        std::ifstream ifs(filename);
        if (ifs.fail()) return "";
        std::ostringstream oss;
        ifs >> oss.rdbuf();
        if (ifs.fail() && !ifs.eof()) return "";
        return oss.str();
    }
   public:
    bool is_open_file = false;
    Titlebar& title_bar = this->make_child<Titlebar>("Xml Parser");
    Textbox& textbox = this->make_child<Textbox>();
    Textbox& editor_output = this->make_child<
        Textbox>();  //如果这个地方放一个layout，layout中只有一个widget会占很大的地方

    Editor() { this->initialize(); }
    void load_file(const std::string& filename) {
        // Clear current
        
        textbox.set_contents(__load_file(filename));
        size_t pos = filename.find_last_of("/");
        std::string name;
        if (pos != std::string::npos) {
            name = filename.substr(pos + 1);
        } else
            name = filename;
        this->title_bar.title.set_contents("Xml Parser - [ " + name + " ]");

        is_open_file = true;
    }
    void close_file() {
        if (!is_open_file) return;
        is_open_file = false;
        textbox.set_contents("");
        textbox.erase(0);
        textbox.clear();
        textbox.brush.set_background(Color::Black);
        System::send_event(Paint_event(textbox));
        this->title_bar.title.set_contents("Xml Parser");
        
        
        editor_output.set_contents(glyph);
        editor_output.disable_input();
    }
    void initialize() {
        this->focus_policy = Focus_policy::Strong;
        // 打开文件 Ctrl + O
        static auto _on_enter = [&, this]() {
            Glyph_string gs;
            gs = this->editor_output.contents();
            if (gs.size()) {
                this->load_file(gs.str());
            }
            editor_output.set_contents(glyph);
            Focus::set_focus_to(textbox);
            editor_output.disable_input();
            Shortcuts::remove_shortcut(Key::Code::Enter);
        };

        Shortcuts::add_shortcut(Key::Code::Ctrl_o).connect([&, this]() {
            this->editor_output.set_contents("");
            this->editor_output.enable_input();
            Focus::set_focus_to(this->editor_output);
            Shortcuts::add_shortcut(Key::Code::Enter).connect(_on_enter);
        });

        // 关闭文件 Ctrl + W
        Shortcuts::add_shortcut(Key::Code::Ctrl_w).connect([&, this]() {
            if (is_open_file) {
                // 检查是否没有保存
                this->close_file();
            }
            Focus::set_focus_to(this->textbox);
        });

        glyph = Glyph_string("Ready. Press ", Attribute::Bold) +
                Glyph_string("[F1]", Attribute::Bold,
                             detail::ForegroundColor::Red) +
                Glyph_string(" for help.", Attribute::Bold);
        textbox.border.enable();

        textbox.border.segments.north.disable();
        textbox.border.segments.south.disable();
        textbox.border.segments.south_east.disable();
        textbox.border.segments.south_west.disable();
        textbox.border.segments.north_east.disable();
        textbox.border.segments.north_west.disable();

        textbox.border.segments.east.enable();
        textbox.border.segments.west.enable();
        textbox.border.segments.east.symbol =
            textbox.border.segments.west.symbol = L'|';

        editor_output.set_contents(glyph);
        editor_output.height_policy.fixed(1);
        editor_output.disable_input();
    }

   protected:
    bool focus_in_event() override {
        Focus::set_focus_to(this->textbox);
        return true;
    }
};
}  // namespace Editor

using namespace cppurses;
int main(int argc, char* argv[]) {
    System sys;
    Editor::Editor editor;
    System::set_initial_focus(&editor.textbox);
    if (argc > 1) {
        editor.load_file(argv[1]);
    }
    return sys.run(editor);
}