#include "xmlfile.h"
#include <boost/regex.hpp>
#include "stdc++.h"
#include "xml.h"





namespace xmlFile {
std::ifstream file;
const char* xmlHeader = "<?xml";
const char* commentHeader = "<!--";
const char* dtdHeader = "<!";
const char* cdataHeader = "<![CDATA[";
XMLFileInfo FileInfo;

TTreeNode __Treeroot, *Treeroot = &__Treeroot;

// std::string FileName /*{"/root/XML/test/datain2.xml"}*/;
const char
    *reg_ver = R"...((?<=version\=[\"|\'])[\w|\.]+(?=[\"|\']))...",
    *reg_encoding = R"...((?<=encoding\=[\"|\'])[\w|\F.|\-]+(?=[\"|\']))...",
    *reg_standalone =
        R"...((?<=standalone\=[\"|\'])[\w|\.|\-]+(?=[\"|\']))...",
    *reg_label_start = R"...((?<!\")<[^\f\n\r\t\v\/\!]+?(>|\/>))...",
    *reg_label_end =
        R"...((?<!\")(<\/[^\f\n\r\t\v\/\!]+?>|<[^\f\n\r\t\v\/\!]+?\/>))...",
    *reg_label = R"...((?<!\")<[^\f\n\r\t\v\!]+?(>|\/>))...",
    //参数中不带引号的匹配
        *reg_label_args_without_quotes =
            R"...((?<=\ )((?<!("|'))[^=\ ])+=[^"'>\/\ ]+)...",
    //参数中带引号的匹配
            *reg_label_args_with_quotes =
                R"...((?<=\ )((?<!("|'))[^=\ ])+=("|').*?("|'))...",
    //不能有空格
                *reg_label_args = R"...((\S+)=[^(>\/\ )]+)...";

TTreeNode* __getTreeRoot() { return Treeroot; }
void setiostreamMode() { file.exceptions(std::ifstream::failbit); }
void init(const std::string& File) {
    freopen("./output.out", "w", stderr);
    using namespace std;
    if (file.is_open()) file.close();
    // file.exceptions(ifstream::failbit);
    try {
        file.open(File.c_str(), ios_base::in);
    } catch (const ios_base::failure& e) {
        cerr << "In File :" << __FILE__ << endl
             << "At line: " << __LINE__ << endl;
        cerr << e.what() << endl << "\nExiting...\n";
        exit(-1);
    }
}
void printDebugInfo() {
#ifdef DEBUG
    using std::cout;
    using std::endl;
    cout << "\nEncoding: " << FileInfo.encoding
         << "\nXML Version: " << FileInfo.version
         << "\nStandalone: " << FileInfo.isalone << endl;
#endif
}
void getXMLInfo(const std::string& line) {
    using namespace std;
    using namespace boost;
    regex test(reg_ver), test2(reg_standalone), test3(reg_encoding);
    smatch fuck, fuck2, fuck3;
    if (regex_search(line, fuck, test)) {
        FileInfo.version = fuck[0];
    }
    test.set_expression(reg_encoding);
    if (regex_search(line, fuck3, test3)) {
        FileInfo.encoding = fuck3[0];
    }
    test.set_expression(reg_standalone);
    if (regex_search(line, fuck2, test2)) {
        FileInfo.isalone = *fuck2[0].begin() == 'y' ? 1 : 0;
    }
    if (FileInfo.version.empty()) {
        throw system_error(make_error_code(errc::io_error),
                           "Failed to locate XML file info!");
    }
}
void printTree(TTreeNode* now, int depth) {
    for (int i = 1; i <= depth; ++i) std::cout << "--";
    std::cout << "[Start] " << now->tag.TagName << "\n";
    for (auto& x : now->tag.key_value) {
        for (int i = 1; i <= depth; ++i) std::cout << "  ";
        std::cout << x.first << " : " << x.second << std::endl;
    }
    if (now->Content.size()) {
        for (int i = 1; i <= depth; ++i) std::cout << "  ";
        std::cout << "[Content] " << now->Content << "\n";
    }
    for (auto& x : now->Children) {
        printTree(x, depth + 1);
    }
    for (int i = 1; i <= depth; ++i) std::cout << "--";
    std::cout << "[End] " << now->tag.TagName << "\n";
}
void __destroyAll(TTreeNode* now) {
    for (auto& x : now->Children) __destroyAll(x);
    now->Parent = nullptr;
    now->Generation = 1;
    now->Content.clear();
    now->Children.clear();
    now->tag.key_value.clear();
    now->tag.TagName.clear();
    if (now == Treeroot) return;
    delete now;
}
std::string __Trimstring(const std::string& text) {
    using std::string;
    string res = text;
    //左侧空白
    res.erase(res.begin(),
              std::find_if(res.begin(), res.end(),
                           [&](int ch) -> bool { return !std::isspace(ch); }));
    //右侧空白
    res.erase(std::find_if(res.rbegin(), res.rend(),
                           [](int ch) -> bool { return !std::isspace(ch); })
                  .base(),
              res.end());
    return res;
}
auto getKeyValue(const std::string& content) {
    using namespace std;
    auto ll = content.find("\""), l = content.find("'");
    ll = min(ll, l);
    ll = content.substr(0, ll).find("=");
    string lhs = content.substr(0, ll), rhs = content.substr(ll + 1);
    if (rhs.front() == '\'' || rhs.front() == '"') rhs.erase(0, 1);
    if (rhs.back() == '\'' || rhs.back() == '"') rhs.pop_back();
    return make_pair(move(lhs), move(rhs));
}
auto trimTagEnd(const std::string& Tagname) {
    using namespace std;
    string res = Tagname;
    if (Tagname.back() == '>') res.pop_back();
    if (Tagname.front() == '<') res.erase(0, 1);
    if (res.back() == '/')
        res.pop_back();
    else if (res.front() == '/')
        res.erase(0, 1);
    using namespace boost;
    regex reg("\\S+");
    smatch Sres;
    auto l = res.cbegin(), r = res.cend();
    assert(regex_search(l, r, Sres, reg));
    return Sres.str();
}
void parseTagArgv(const std::string& Tag, TTreeNode* const node) {
    auto l = Tag.cbegin(), r = Tag.cend();
    using namespace boost;
    regex reg2(reg_label_args_with_quotes), reg(reg_label_args_without_quotes);
    smatch res;
    while (regex_search(l, r, res, reg)) {
        //没有引号的匹配
        node->tag.key_value.insert(move(getKeyValue(res.str())));
        // std::cout << "[Args] " << res << std::endl;
        l = res[0].second;
    }
    l = Tag.cbegin(), r = Tag.cend();
    while (regex_search(l, r, res, reg2)) {
        node->tag.key_value.insert(move(getKeyValue(res.str())));
        // std::cout << "[Args] " << res << std::endl;
        l = res[0].second;
    }
}
void addTags() {
    using namespace std;
    using namespace boost;
    std::stack<__XMLStackReader> st;
    if (!file.is_open())
        throw system_error(make_error_code(errc::io_error),
                           "Xml file is not open!");
    string buffer;

    file.clear();
    file.seekg(ios_base::beg);

    getline(file, buffer);
    try {
        getXMLInfo(buffer);
    } catch (...) {
#ifdef DEBUG
        cout << "<!!!>This XML doesn't contain headers" << endl;
#endif
        file.clear();
        file.seekg(ios_base::beg);
    }

    regex regS(reg_label_start), regE(reg_label_end), regL(reg_label);
    size_t tot = 0;
    while (1) {
        smatch res;
        try {
            getline(file, buffer);
        } catch (...) {
            break;
        }
        auto l = buffer.cbegin(), r = buffer.cend();
        string ww;  // 用于存放标签之间的文本
        ww.reserve(512);
        while (regex_search(l, r, res, regL)) {
            // 必须先调用str(0)才能获得常量迭代器
            // fuck boost
            auto boost_your_mother_flies_in_the_sky = res.str(0);
            auto p = boost_your_mother_flies_in_the_sky.cbegin(),
                 q = boost_your_mother_flies_in_the_sky.cend();
            smatch ress;
            // 标签开始
            auto pNode = st.empty() ? Treeroot : st.top().pNode;

            if (regex_search(p, q, ress, regS)) {
                TTreeNode::Tag tag;
                //去掉首尾的尖括号
                string&& tmp = ress.str().substr(1);
                tmp.pop_back();
                tag.TagName = std::move(tmp);
                // 加入新节点
                pNode->addNode(tag);

                auto ii = l, jj = p;
                ww.clear();
                while (*ii != *jj) {  //处理新标签之前的文本
                    ww += *(ii++);
                }

                pNode->Content += std::move(__Trimstring(ww));
                parseTagArgv(tag.TagName, pNode->Children.back());

                st.push({++tot, pNode->Children.back()});
            }
            // 标签结束
            if (regex_search(p, q, ress, regE)) {
                if (!st.empty()) {
                    auto ii = l, jj = p;
                    ww.clear();
                    // 处理两个相邻标签之间的文本
                    while (*ii != *jj) {
                        ww += *(ii++);
                    }
                    st.top().pNode->tag.TagName = trimTagEnd(ress.str());

                    st.top().pNode->Content += std::move(__Trimstring(ww));
                    st.pop();
                } else
                    throw system_error(make_error_code(errc::io_error),
                                       "Xml file is not invalid!");
            }
            l = res[0].second;
            tot = st.empty() ? 0 : st.top().tot;
        }  // while(regex)
        auto pNode = st.empty() ? Treeroot : st.top().pNode;
        ww.clear();
        while (l != r) {  //处理标签之后的文本
            ww += *(l++);
        }
        pNode->Content += std::move(__Trimstring(ww));
    }  // while(1)
    if (!st.empty()) {
        throw system_error(make_error_code(errc::io_error),
                           "Xml file is not invalid!");
    }
}
bool hasNext() { return file.is_open() && !file.eof(); }
}  // namespace xmlFile
