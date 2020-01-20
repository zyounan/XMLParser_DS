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

struct XMLTags {
    XmlSyntax tagType;
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
void addTags() {
    using namespace std;
    using namespace boost;
    if (!file.is_open())
        throw system_error(make_error_code(errc::io_error),
                           "Xml file is not open!");
    string buffer;

    regex regS(reg_label_start), regE(reg_label_end), regL(reg_label);
    stack<int> st;
    int tot = 0;
    while (1) {
        smatch res;
        try {
            getline(file, buffer);
        } catch (...) {
            break;
        }
        auto l = buffer.cbegin(), r = buffer.cend();
        while (regex_search(l, r, res, regL)) {
            auto p = res.str().cbegin(), q = res.str().cend();
            smatch ress;
            if (regex_search(p, q, ress, regS)) {
                st.push(++tot);
#ifdef DEBUG
                cout << "|";
                for (int i = 1; i <= (tot << 1); ++i) cout << "-";
                cout << "Tag start: " << ress.str() << endl;
#endif
            }
            if (regex_search(p, q, ress, regE)) {
#ifdef DEBUG
                cout << "|";
                for (int i = 1; i <= (tot << 1); ++i) cout << "-";
                cout << "Tag end: " << ress.str() << endl;
#endif
                if (!st.empty())
                    st.pop();
                else
                    throw system_error(make_error_code(errc::io_error),
                                       "Xml file is not invalid!");
            }
            l = res[0].second;
            tot = st.empty() ? 0 : st.top();
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
#ifdef DEBUG
    assert(st.empty());
#endif
}
bool hasNext() { return file.is_open() && !file.eof(); }
}  // namespace xmlFile
