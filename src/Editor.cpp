#include <cppurses/cppurses.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/checkbox.hpp>
#include <cppurses/widget/widgets/color_select.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>
#include <cppurses/widget/widgets/textbox.hpp>
// UI headers
// Boost.Regex
#include <boost/regex.hpp>

#include "stdc++.h"
#include "xml.h"

namespace Editor {

//定时器实现
class Timer {
public:
    Timer() : expired_(true), try_to_expire_(false) {
    }

    Timer(const Timer& t) {
        expired_ = t.expired_.load();
        try_to_expire_ = t.try_to_expire_.load();
    }
    ~Timer() {
        Expire();
        //      std::cout << "timer destructed!" << std::endl;
    }

    void StartTimer(int interval, std::function<void()> task) {
        if (!expired_) {
            //          std::cout << "timer is currently running, please expire it first..." << std::endl;
            return;
        }
        expired_ = false;
        std::thread([this, interval, task]() {
            while (!try_to_expire_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                task();
            }
            //          std::cout << "stop task..." << std::endl;
            {
                std::lock_guard<std::mutex> locker(mutex_);
                expired_ = true;
                expired_cond_.notify_one();
            }
        }).detach();
    }

    void Expire() {
        if (expired_) {
            return;
        }

        if (try_to_expire_) {
            //          std::cout << "timer is trying to expire, please wait..." << std::endl;
            return;
        }
        try_to_expire_ = true;
        {
            std::unique_lock<std::mutex> locker(mutex_);
            expired_cond_.wait(locker, [this] { return expired_ == true; });
            if (expired_ == true) {
                //              std::cout << "timer expired!" << std::endl;
                try_to_expire_ = false;
            }
        }
    }

    template <typename callable, class... arguments>
    void SyncWait(int after, callable&& f, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));
        std::this_thread::sleep_for(std::chrono::milliseconds(after));
        task();
    }
    template <typename callable, class... arguments>
    void AsyncWait(int after, callable&& f, arguments&&... args) {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        std::thread([after, task]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }).detach();
    }

private:
    std::atomic<bool> expired_;
    std::atomic<bool> try_to_expire_;
    std::mutex mutex_;
    std::condition_variable expired_cond_;
};

static constexpr const char *
    reg_ver = R"...((?<=version\=[\"|\'])[\w|\.]+(?=[\"|\']))...",
   *reg_encoding = R"...((?<=encoding\=[\"|\'])[\w|\F.|\-]+(?=[\"|\']))...",
   *reg_standalone =
       R"...((?<=standalone\=[\"|\'])[\w|\.|\-]+(?=[\"|\']))...",
   *reg_label_start = R"...((?<!\")<[^\f\n\r\t\v\/\!]+?(>|\/>))...",
   *reg_label = R"...((?<!\")<[^\f\t\v\!]+?(>|\/>))...",
   *reg_label_end =
       R"...((?<!\")(<\/[^\f\n\r\t\v\/\!]+?>|<[^\f\n\r\t\v\/\!]+?\/>))...",
   *reg_label_args_without_quotes =
       R"...((?<=\ )((?<!("|'))[^=\ ])+=[^"'>\/\ ]+)...",
   *reg_label_args_with_quotes =
       R"...((?<=\ )((?<!("|'))[^=\ ])+=("|').*?("|'))...",
   *reg_label_args = R"...((\S+)=[^(>\/\ )]+)...",
   *reg_comment = R"...((?<!\")<!--[^\f\n\r\t\v\!]+)...";

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
    Timer highlight_timer;
    std::mutex lock;

    enum class __parse_state {
        clear,
        comment,
        cdata,
        dtd,
        label,
        key_value,
        content
    };
    struct __parse_helper {
        bool isInComment = false;  // 注释 蓝色
        bool isInCDATA = false;    // CDATA 棕色
        bool isIndtd = false;      // dtd 蓝色
        bool isInLabel = false;    // label 红色
        int sta_sum = 0;
        bool isStillKeyValue = false;                        // key value 换行
        __parse_state current_state = __parse_state::clear;  //初始化为空
    } parse_flag;
    boost::regex Reg_label{reg_label}, Reg_label_start{reg_label_start}, Reg_label_end{reg_label_end},
        Reg_comment{reg_comment};

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
    void __parse_key_value(Glyph_string& res, std::string::const_iterator& pl,
                           std::string::const_iterator& pr) {
        using namespace std;
        using namespace xmlParser;
        string content;
        while (pl != pr && *pl != '>') {
            parse_flag.current_state = __parse_state::label;

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
                // parse_flag.isStillKeyValue = true;
                parse_flag.current_state = __parse_state::key_value;
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
            parse_flag.current_state = __parse_state::clear;
            // parse_flag.sta_sum--;
            // parse_flag.isInLabel = false;
            // parse_flag.isStillKeyValue = false;
        }
    }

    Glyph_string __parse_line(const cppurses::Glyph_string& line) {
        using namespace xmlParser;
        using namespace boost;
        using std::string;

        string tmp = line.str();

        Glyph_string res;
        smatch what;
        auto pl = tmp.cbegin(), pr = tmp.cend();
        if (pl == pr) return {};

        while (pl != pr) {
            //不是特殊标记起始
            if (*pl != '<') {
                //普通文本
                if (parse_flag.current_state == __parse_state::clear) {
                    string content;
                    content.reserve(128);
                    size_t nxt = tmp.find('<', pl - tmp.cbegin());
                    content = tmp.substr(pl - tmp.cbegin(), nxt - (pl - tmp.cbegin()));
                    res.append(content, cppurses::detail::ForegroundColor::White);

                    if (nxt == string::npos)
                        pl = pr;
                    else
                        pl = tmp.cbegin() + nxt;
                }
                //其他情况……
                else if (parse_flag.current_state == __parse_state::comment) {
                    string content;
                    content.reserve(128);
                    size_t nxt = tmp.find("-->", pl - tmp.cbegin());
                    content = tmp.substr(pl - tmp.cbegin(), nxt - (pl - tmp.cbegin()));
                    if (nxt == string::npos) {
                        pl = pr;
                    } else {
                        pl = tmp.begin() + nxt + 3;
                        content += "-->";
                        parse_flag.current_state = __parse_state::clear;
                    }
                    res.append(content, cppurses::detail::ForegroundColor::Blue);
                } else if (parse_flag.current_state == __parse_state::key_value || parse_flag.current_state == __parse_state::label) {
                    size_t nxt = tmp.find(">", pl - tmp.cbegin());
                    __parse_key_value(res, pl, pr);
                    if (nxt == string::npos) {
                        pl = pr;
                    } else
                        pl = tmp.begin() + nxt;
                }
            } else {
                //解析标签
                //必须是以pl开头的一个前缀
                if (regex_search(pl, pr, what, Reg_label) && !what.empty() && what.position(0ul) == 0) {
                    parse_flag.current_state = __parse_state::label;

                    auto label_start = what.str(0);
                    auto l = label_start.cbegin(), r = label_start.cend();
                    //找到括号开头<
                    string tag_name;
                    tag_name.reserve(128);
                    while (l != r && *l != ' ' && *l != '>') {
                        tag_name += *l++;
                    }
                    res.append(tag_name, cppurses::detail::ForegroundColor::Red);
                    if (l != r && *l == '>') {
                        parse_flag.current_state = __parse_state::clear;
                        res.append(">", cppurses::detail::ForegroundColor::Red);
                    } else if (l != r && *l == ' ') {
                        //解析 Key-Value
                        __parse_key_value(res, l, r);
                    }
                    pl = what[0].second;

                } else if (regex_search(pl, pr, what, Reg_comment) && !what.empty() && what.position(0ul) == 0) {
                    parse_flag.current_state = __parse_state::comment;
                    string content;
                    content.reserve(128);
                    size_t nxt = tmp.find("-->", pl - tmp.cbegin());
                    content = tmp.substr(pl - tmp.cbegin(), nxt - (pl - tmp.cbegin()));

                    if (nxt == string::npos) {
                        pl = pr;
                    } else {
                        pl = tmp.begin() + nxt + 3;
                        content += "-->";
                        parse_flag.current_state = __parse_state::clear;
                    }
                    res.append(content, cppurses::detail::ForegroundColor::Blue);
                } else {
                    //刚刚输入进来一个< ?，
                    res.append(string{*pl}, cppurses::detail::ForegroundColor::White);
                    ++pl;
                }
            }
        }
        return res;
    }

public:
    bool is_open_file = false;
    std::string cur_path;
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
        cur_path = filename;
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
    void save_file(const std::string& path) {
        std::ofstream ofs(path, std::ios_base::out | std::ios_base::trunc);
        if (ofs.is_open()) {
            editor_output.set_contents("Saving...");
            ofs << this->textbox.contents().str();
        } else {
            editor_output.set_contents("Failed to save!!");
        }
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

        static auto _on_enter_save = [&, this]() {
            Glyph_string gs;
            gs = this->editor_output.contents();
            if (gs.size()) {
                this->save_file(gs.str());
            }
            editor_output.set_contents(glyph);
            Focus::set_focus_to(textbox);
            editor_output.disable_input();
            Shortcuts::remove_shortcut(Key::Code::Enter);
        };

        // 打开文件 Ctrl + O
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
                cur_path = "";
            }
            Focus::set_focus_to(this->textbox);
        });
        // 保存文件 Ctrl + x
        Shortcuts::add_shortcut(Key::Code::Ctrl_x).connect([&, this]() {
            this->editor_output.set_contents("");
            if (this->is_open_file) {
                this->editor_output.set_contents(cur_path);
            }
            this->editor_output.enable_input();
            Focus::set_focus_to(this->editor_output);
            Shortcuts::add_shortcut(Key::Code::Enter).connect(_on_enter_save);
        });

        glyph = Glyph_string("Ready. Press ", Attribute::Bold) +
                Glyph_string("[F1]", Attribute::Bold,
                             detail::ForegroundColor::Red) +
                Glyph_string(" for help.", Attribute::Bold);

        // Syntax Highlighting A
        textbox.contents_modified.connect([&, this](const Glyph_string& text) {
            if (!text.size())
                return;
            //锁上去
            lock.lock();
            int line = textbox.cursor.y();
            size_t pos = textbox.index_at({0ul, (size_t)line});
            int back = (int)pos + (int)textbox.row_length(line) - 1;
            for (; back >= (int)pos; --back) {
                auto x = text[back].brush.foreground_color().get();
                if (x != cppurses::Color::Black && x != cppurses::Color::White)
                    break;
            }
            Glyph_string res = {text.begin() + pos, text.begin() + back + 1};

            Glyph_string tmp = {text.begin() + back + 1,
                                text.begin() + pos + textbox.row_length(line)};

            textbox.contents_modified.disable();
            res = res + __parse_line(tmp);
            textbox.erase(pos, textbox.row_length(line));
            textbox.insert(res, pos);
            textbox.contents_modified.enable();

            lock.unlock();
        });
        //Syntax Highlighting B
        highlight_timer.StartTimer(3000, [&, this] {
            // size_t pos = textbox.index_at({0ul, textbox.bottom_line()});
            auto& text = textbox.contents();
            if (!text.size()) return;
            //锁上去
            lock.lock();

            size_t y = textbox.cursor.y();
            size_t old = textbox.index_at(textbox.cursor.position());
            textbox.contents_modified.disable();
            Glyph_string res = __parse_line(text);
            textbox.erase(0);
            textbox.insert(res, 0);
            textbox.scroll_down(y);
            textbox.set_cursor(old);
            textbox.contents_modified.enable();

            lock.unlock();
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