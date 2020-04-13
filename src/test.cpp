// #include "stdc++.h"
// #include "xml.h"
// #include "ui.h"
// int main() {
//     using namespace std;
//     using namespace xmlParser;
//     freopen("/root/XML/test/dataout1.xml", "w", stdout);
//     XmlDocument file;
//     file.open("/root/XML/test/datain1.xml");
//     try {
//         file.Parse();
//     } catch (...) {
//         exit(-1);
//     }
//     return 0;
// }

#include <cppurses/cppurses.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

#include "stdc++.h"
#include "xml.h"
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
    opt::Optional<cppurses::Color> cur_color;

    struct __parse_helper {
        bool isInComment = false;      // 注释 蓝色
        bool isInCDATA = false;        // CDATA 棕色
        bool isIndtd = false;          // dtd 蓝色
        bool isInLabel = false;        // label 红色
        bool isStillKeyValue = false;  // key value 换行
    } parse_flag;

    void __print_xml(xmlParser::XmlNode* root, int d) {
        static auto __printTab = [this](int y) {
            std::stringstream tmp;
            for (int i = 1; i <= y; ++i) {
                //四个空格
                tmp << "    ";
            }
            textbox.append(tmp.str());
        };
        using namespace std;
        using namespace xmlParser;
        if (!root->getDepth() && root->getTag().key_value.size()) {
            //标签颜色为红色
            textbox.append(
                Glyph_string("<?xml ", detail::ForegroundColor::Red));
            //键值对：键: 绿色；等号：白色；值：黄色
            for (auto& x : root->getTag().key_value) {
                // textbox.insert_brush.set_foreground(Color::Green);
                textbox.append(
                    Glyph_string(x.first, detail::ForegroundColor::Green));
                textbox.append(
                    Glyph_string("=", detail::ForegroundColor::White));
                textbox.append(Glyph_string("\"" + x.second + "\" ",
                                            detail::ForegroundColor::Yellow));
                // cout << x.first << "="
                //      << "\"" << x.second << "\" ";
            }
            textbox.append(Glyph_string("?>\n", detail::ForegroundColor::Red));
            // cout << "?>\n";
        }
        for (auto& x : root->getNext()) {
            // textbox.insert_brush.set_foreground(Color::White);
            if (x->getType() == XmlSyntax::XmlUnknown) continue;
            __printTab(d);
            if (x->getType() == XmlSyntax::XmlComment) {
                //注释设置为蓝色
                textbox.append(Glyph_string("<!-- " + x->getContent() + " -->",
                                            detail::ForegroundColor::Blue));
                // cout << "<!-- ";
                // cout << x->getContent() << " -->";
            } else if (x->getType() == XmlSyntax::XmlCDATA) {
                // CDATA 部分设置为棕色
                // textbox.insert_brush.set_foreground(Color::Brown);
                textbox.append(
                    Glyph_string("<![CDATA[ " + x->getContent() + " ]]>",
                                 detail::ForegroundColor::Brown));
                // cout << "<![CDATA["
                //      << " ";
                // cout << x->getContent() << " ]]>";
            } else {
                //标签是红色
                textbox.insert_brush.set_foreground(Color::Red);
                textbox.append(Glyph_string("<" + x->getTagName(),
                                            detail::ForegroundColor::Red));

                if (x->getTagKeyValue().size()) {
                    for (auto& y : x->getTagKeyValue()) {
                        textbox.append(Glyph_string(
                            " " + y.first, detail::ForegroundColor::Green));
                        textbox.append(
                            Glyph_string("=", detail::ForegroundColor::White));
                        textbox.append(
                            Glyph_string("\"" + y.second + "\"",
                                         detail::ForegroundColor::Yellow));
                    }
                }

                if (x->getType() == XmlSyntax::XmlEmptylabel) {
                    textbox.append(
                        Glyph_string("/>", detail::ForegroundColor::Red));
                } else {
                    textbox.append(
                        Glyph_string(">", detail::ForegroundColor::Red));
                    if (x->getContent().size()) {
                        textbox.append("\n");

                        __printTab(d + 1);
                        textbox.append(Glyph_string(
                            x->getContent(), detail::ForegroundColor::White));
                        //正文用白色
                        // cout << x->getContent();
                    }
                }

            }  // end if
            textbox.append("\n");
            // cout << endl;
            __print_xml(x, d + 1);
            //标签结束，红色
            if (x->getType() != XmlSyntax::XmlComment &&
                x->getType() != XmlSyntax::XmlCDATA &&
                x->getType() != XmlSyntax::XmlEmptylabel) {
                __printTab(d);
                textbox.append(Glyph_string("</" + x->getTag().tagName + ">\n",
                                            detail::ForegroundColor::Red));
                // cout << "</" << x->getTag().tagName << ">\n";
            }
        }
    }
    void __on_err_func(const std::string& content) {
        editor_output.set_contents(Glyph_string(content, Attribute::Bold,
                                                detail::ForegroundColor::Red));
    }
    void __load_file(const std::string& filename) {
        using namespace xmlParser;

        XmlDocument file;
        file.open(filename);
        try {
            file.Parse();
            // cur_color = textbox.brush.foreground_color();
            __print_xml(file.getTreeRoot(), 0);

            std::string info = file.getLastInfo();
            if (info.size()) __on_err_func(file.getLastInfo());

        } catch (const XmlException& e) {
            std::string info = file.getLastInfo();
            if (info.size()) __on_err_func(file.getLastInfo());
            file.close();
            std::ifstream ifs(filename);
            if (ifs.fail()) {
                __on_err_func("Failed to open : " + filename + " !");
                return;
            }
            std::ostringstream oss;
            ifs >> oss.rdbuf();
            textbox.set_contents(oss.str());
        }
    }
    void __parse_key_value(Glyph_string& res, std::string::iterator& pl,
                           std::string::iterator& pr) {
        using namespace std;
        using namespace xmlParser;
        string content;
        while (pl != pr && *pl != '>') {
            content.clear();
            // key
            // XmlUtil::skipWhiteSpace(pl, pr);
            while (pl != pr && XmlUtil::isWhite(*pl)) {
                res.append(std::string{*pl}, detail::ForegroundColor::White);
                ++pl;
            }
            while (pl != pr && *pl != '=' && *pl != '>' && *pl != '\'' &&
                   *pl != '\"') {
                content += *(pl++);
            }
            res.append(content, detail::ForegroundColor::Green);
            if (pl == pr) break;

            while (pl != pr && XmlUtil::isWhite(*pl)) {
                res.append(std::string{*pl}, detail::ForegroundColor::White);
                ++pl;
            }

            if (*pl == '=') {
                res.append("=", detail::ForegroundColor::White);
                ++pl;
                parse_flag.isStillKeyValue = true;
            }

            while (pl != pr && XmlUtil::isWhite(*pl)) {
                res.append(std::string{*pl}, detail::ForegroundColor::White);
                ++pl;
            }

            content.clear();
            // value
            char mark = ' ';

            if (pl != pr && *pl == '\'') {
                mark = '\'';
                res.append("\'", detail::ForegroundColor::Yellow);
                ++pl;
            } else if (pl != pr && *pl == '\"') {
                mark = '\"';
                res.append("\"", detail::ForegroundColor::Yellow);
                ++pl;
            }
            while (pl != pr && *pl != '>' && *pl != mark) {
                content += *(pl++);
            }
            res.append(content, detail::ForegroundColor::Yellow);
            if (pl != pr && *pl != '>') {
                ++pl;
                res.append(string{mark}, detail::ForegroundColor::Yellow);
            }
        }
        // xml

        if (pl != pr && *pl == '>') {
            res.append(">", detail::ForegroundColor::Red);
            ++pl;
            parse_flag.isInLabel = false;
            parse_flag.isStillKeyValue = false;
        }
    }
    Glyph_string __parse_line(const cppurses::Glyph_string& line) {
        using namespace xmlParser;
        using namespace std;

        string tmp = line.str();

        Glyph_string res;
        auto pl = tmp.begin(), pr = tmp.end();
        if (parse_flag.isInComment) {
            auto pos = tmp.find("-->");
            if (pos != string::npos) {
                parse_flag.isInComment = false;
                res.append(tmp.substr(0, pos + 3),
                           detail::ForegroundColor::Blue);
                pl = (tmp.begin() + pos + 3);
            } else {
                //整行都是注释
                return {tmp, detail::ForegroundColor::Blue};
            }
        } else if (parse_flag.isInCDATA) {
            auto pos = tmp.find("]]>");
            if (pos != string::npos) {
                parse_flag.isInCDATA = false;
                res.append(tmp.substr(0, pos + 3),
                           detail::ForegroundColor::Brown);
                pl = (tmp.begin() + pos + 3);
            } else {
                //整行都是
                return {tmp, detail::ForegroundColor::Brown};
            }
        } else if (parse_flag.isIndtd) {
            auto pos = tmp.find("]>");
            if (pos != string::npos) {
                parse_flag.isIndtd = false;
                res.append(tmp.substr(0, pos + 2),
                           detail::ForegroundColor::Blue);
                pl = (tmp.begin() + pos + 2);
            } else {
                //整行都是
                return {tmp, detail::ForegroundColor::Blue};
            }
        } else if (parse_flag.isInLabel) {
            string content;
            content.reserve(128);

            auto pos = tmp.find_last_of('<');

            if (pos != string::npos) {
                res.clear();
                for(int i = 0;i <= (int)pos;++i)    res.append(line[i]);                
                pl = tmp.begin() + pos + 1;
            }

            while (pl != pr && XmlUtil::isWhite(*pl)) {
                res.append(std::string{*pl}, detail::ForegroundColor::White);
                ++pl;
            }
            content.clear();
            while (pl != pr && !XmlUtil::isWhite(*pl) && *pl != '>') {
                if (*pl == '\'' || *pl == '\"') break;
                content += *(pl++);
            }
            res.append(content, detail::ForegroundColor::Red);
            __parse_key_value(res, pl, pr);
        }

        while (pl != pr) {
            //空白跳过

            while (pl != pr && XmlUtil::isWhite(*pl)) {
                res.append(std::string{*pl}, detail::ForegroundColor::White);
                ++pl;
            }

            int mark = XmlDocument::Identify(pl, pr);
            switch (mark) {
                case -1: {
                    //空行
                    return tmp;
                } break;
                case 0: {
                    //普通的内容
                    if (parse_flag.isStillKeyValue) {
                        __parse_key_value(res, pl, pr);
                    }
                    string content;
                    //除最后一个标签的前面所有不用管 已经解析过了
                    // auto pos2 = tmp.find_last_of("<");
                    // auto pos = tmp.find_last_of(">",pos2);
                    //

                    res.clear();
                    auto tt = pl - tmp.begin();
                    for (int i = 0; i < (int)tt; ++i) res.append(line[i]);

                    // if (pos != string::npos){
                    //     // pl = tmp.begin() + pos + 1;
                    // }

                    while (pl != pr && *pl != '<') {
                        content += *pl++;
                    }
                    res.append(content, detail::ForegroundColor::White);
                    break;
                }
                case 1:
                case 5: {
                    // 标签 红色
                    // 键: 绿色；等号：白色；值：黄色
                    string content;
                    assert(*pl == '<');
                    parse_flag.isInLabel = true;

                    res.append("<", detail::ForegroundColor::Red);
                    ++pl;
                    content.reserve(128);
                    while (pl != pr && !XmlUtil::isWhite(*pl) && *pl != '>') {
                        content += *(pl++);
                    }
                    res.append(content, detail::ForegroundColor::Red);

                    __parse_key_value(res, pl, pr);

                } break;
                case 2:
                case 3:
                case 4: {
                }

            }  // end switch
        }
        return res;
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
        textbox.clear();

        textbox.contents_modified.disable();
        // textbox.brush.set_foreground(Color::Red);
        __load_file(filename);

        // textbox.set_contents(__load_file(filename));
        size_t pos = filename.find_last_of("/");
        std::string name;

        textbox.contents_modified.enable();

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
        textbox.contents_modified.disable();

        textbox.clear();
        textbox.brush.set_background(Color::Black);

        textbox.contents_modified.enable();

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

        // 实时高亮
        textbox.contents_modified.connect([&, this](const Glyph_string& text) {
            int line = textbox.cursor.y();
            size_t pos = textbox.index_at({(size_t)0, (size_t)line});

            Glyph_string tmp = {text.begin() + pos,
                                text.begin() + pos + textbox.row_length(line)};

            // std::cerr << tmp.str() << std::endl;

            Glyph_string res = __parse_line(tmp);

            textbox.contents_modified.disable();
            textbox.erase(pos, textbox.row_length(line));
            textbox.insert(res, pos);
            textbox.contents_modified.enable();
        });

        textbox.key_pressed.connect([&](Key::Code code) {
            switch (code) {
                case Key::Code::Tab:
                    textbox.insert(Glyph_string("    "),
                                   textbox.index_at(textbox.cursor.position()));
                    break;
                default:
                    break;
            }
            return;
        });

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
#ifndef DEBUG
    if (argc > 1) {
        editor.load_file(argv[1]);
    }
#endif
#ifdef DEBUG
    // editor.load_file("./test/datain3.xml");
#endif
    return sys.run(editor);
}