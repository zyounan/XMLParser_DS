#pragma once
#include <boost/regex.hpp>
#include "stdc++.h"
#include "xml.h"

namespace xmlFile {
std::ifstream file;
const char* xmlHeader = "<?xml";
const char* commentHeader = "<!--";
const char* dtdHeader = "<!";
const char* cdataHeader = "<![CDATA[";
struct XMLFileInfo {
    std::string encoding, version;
    bool isalone;
} FileInfo;

struct TTreeNode {
    using xiTreeNodeList = std::vector<TTreeNode*>;
    int Generation;
    TTreeNode* Parent = nullptr;
    xiTreeNodeList Children;
    std::string Content;
    struct Tag {
        std::string TagName;
        XmlSyntax tagType;
    };
};
const std::string FileName{"/root/XML/test/datain.xml"};
const char
    *reg_ver = R"...((?<=version\=\")[\w|\.]+(?=\"))...",
    *reg_encoding = R"...((?<=encoding\=\")[\w|\F.|\-]+(?=\"))...",
    *reg_standalone =
        R"...((?<=standalone\=\")[\w|\.|\-]+(?=\"))...",
    *reg_label_start = R"...((?<!\")<[^\f\n\r\t\v\/\!]+?(>|\/>))...",
    *reg_label_end =
        R"...((?<!\")(<\/[^\f\n\r\t\v\/\!]+?>|<[^\f\n\r\t\v\/\!]+?\/>))...",
    *reg_label = R"...((?<!\")<[^\f\n\r\t\v\!]+?(>|\/>))...";
bool hasNext();
void getXMLInfo(const std::string&);
void init() {
    using namespace std;
    file.exceptions(ifstream::failbit);
    try {
        file.open(FileName.c_str(), ios_base::in);
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
         << "\nDependency: " << FileInfo.isalone << endl;
#endif
}
void getXMLInfo(const std::string& line) {
    using namespace std;
    using namespace boost;
    // if (!file.is_open())
    //     throw system_error(make_error_code(errc::io_error),
    //                        "Xml file is not open!");
    regex test(reg_ver), test2(reg_standalone), test3(reg_encoding);
    smatch fuck, fuck2, fuck3;
    // string line;
    // getline(file, line);
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
struct __XMLStackReader {
    size_t tot, begin;
};

std::string __Trimstring(const std::string& text) {
    std::cerr << "[Trimer] ";
    std::cerr << text << std::endl;
    return std::string();
}
void addTags() {
    freopen("./output.out", "w", stderr);
    using namespace std;
    using namespace boost;
    // Reader stack
    std::stack<__XMLStackReader> st;

    if (!file.is_open())
        throw system_error(make_error_code(errc::io_error),
                           "Xml file is not open!");
    string buffer;

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
        size_t now = 0;
        while (regex_search(l, r, res, regL)) {
            auto i = res.prefix().length();
            auto p = res.str().cbegin(), q = res.str().cend();
            smatch ress;
            // start
            if (regex_search(p, q, ress, regS)) {
                st.push({++tot, now + ress.str().length()});
#ifdef DEBUG
                cout << "|";
                for (size_t i = 1; i <= (tot << 1); ++i) cout << "-";
                cout << "Tag start: " << ress.str() << endl;
#endif
            }
            // end
            if (regex_search(p, q, ress, regE)) {
#ifdef DEBUG

                cout << "|";
                for (size_t i = 1; i <= (tot << 1); ++i) cout << "-";
                cout << "Tag end: " << ress.str() << endl;
#endif
                if (!st.empty()) {
                    auto startPos = st.top().begin;
                    auto ii = l, jj = p;
                    string ww;
                    // 这里没办法处理避免文本中的<和标签中的<相同带来的问题。
                    // 需要比较两个迭代器是否相同。Fuck boost::regex
                    // boost::regex傻逼的设计
                    while (*ii != *jj) {
                        ww += *(ii++);
                    }
                    __Trimstring(ww);
                    st.pop();
                } else
                    throw system_error(make_error_code(errc::io_error),
                                       "Xml file is not invalid!");
            }
            l = res[0].second;
            tot = st.empty() ? 0 : st.top().tot;
        }
    }
    if (!st.empty()) {
        throw system_error(make_error_code(errc::io_error),
                           "Xml file is not invalid!");
    }
    file.clear();
    file.seekg(ios_base::beg);

    getline(file, buffer);
    try {
        getXMLInfo(buffer);
    } catch (...) {
#ifdef DEBUG
        cout << "<!!!>This XML doesn't contain headers" << endl;
#endif
        FileInfo = {"UTF-8", "1.0", 1};
    }
    // #ifdef DEBUG
    assert(st.empty());
    // #endif
}
bool hasNext() { return file.is_open() && !file.eof(); }
}  // namespace xmlFile
