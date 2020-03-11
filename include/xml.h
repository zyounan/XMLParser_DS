#pragma once
#include "stdc++.h"
// 制作一个包装器，用于 range_based_for_loop 的倒序枚举
template <typename T>
struct reversion_wrapper {
    T& iterable;
};
// range 循环基于 std::begin() 和 std::end();
template <typename T>
inline auto begin(const reversion_wrapper<T>& w) {
    return std::rbegin(w.iterable);
}
template <typename T>
inline auto end(const reversion_wrapper<T>& w) {
    return std::rend(w.iterable);
}
// 容器倒序
template <typename T>
inline auto reverse(T&& iterable) -> reversion_wrapper<T> {
    return {std::forward<T>(iterable)};
}
namespace xmlParser {
enum class XmlSyntax {
    Xmldocument = 1,
    Xmlelement,
    XmlAttribute,
    XmlComment,
    XmlTest,
    XmlDeclaration,
    XmlEmptylabel,
    XmlCDATA,
    XmlUnknown
};
enum class XmlParserInfo{
    Warning,
    Error,
    Info
};
enum class XmlError {
    XML_SUCCESS = 0,
    XML_NO_ATTRIBUTE,
    XML_WRONG_ATTRIBUTE_TYPE,
    XML_ERROR_FILE_NOT_FOUND,
    XML_ERROR_FILE_COULD_NOT_BE_OPENED,
    XML_ERROR_FILE_READ_ERROR,
    XML_ERROR_PARSING_ELEMENT,
    XML_ERROR_PARSING_ATTRIBUTE,
    XML_ERROR_PARSING_TEXT,
    XML_ERROR_PARSING_CDATA,
    XML_ERROR_PARSING_COMMENT,
    XML_ERROR_PARSING_DECLARATION,
    XML_ERROR_PARSING_UNKNOWN,
    XML_ERROR_EMPTY_DOCUMENT,
    XML_ERROR_MISMATCHED_ELEMENT,
    XML_ERROR_PARSING,
    XML_CAN_NOT_CONVERT_TEXT,
    XML_NO_TEXT_NODE,
    XML_ELEMENT_DEPTH_EXCEEDED,
    XML_ERROR_COUNT
};
class XmlException : public std::exception {
   private:
    std::string fuck;
    static constexpr const char* __ErrName[] = {
        "XML_SUCCESS",
        "XML_NO_ATTRIBUTE",
        "XML_WRONG_ATTRIBUTE_TYPE",
        "XML_ERROR_FILE_NOT_FOUND",
        "XML_ERROR_FILE_COULD_NOT_BE_OPENED",
        "XML_ERROR_FILE_READ_ERROR",
        "XML_ERROR_PARSING_ELEMENT",
        "XML_ERROR_PARSING_ATTRIBUTE",
        "XML_ERROR_PARSING_TEXT",
        "XML_ERROR_PARSING_CDATA",
        "XML_ERROR_PARSING_COMMENT",
        "XML_ERROR_PARSING_DECLARATION",
        "XML_ERROR_PARSING_UNKNOWN",
        "XML_ERROR_EMPTY_DOCUMENT",
        "XML_ERROR_MISMATCHED_ELEMENT",
        "XML_ERROR_PARSING",
        "XML_CAN_NOT_CONVERT_TEXT",
        "XML_NO_TEXT_NODE",
        "XML_ELEMENT_DEPTH_EXCEEDED",
        "XML_ERROR_COUNT"};

   public:
    XmlException(const XmlError What)
        : fuck("[Error] : XmlParser got an exception : ") {
        fuck += __ErrName[static_cast<int>(What)];
    }
    virtual const char* what() const noexcept { return fuck.c_str(); }
};
class XmlUtil {
   public:
    static const char* skipWhiteSpace(const char* p, int& curlineNum) {
        if (!p) return nullptr;
        while (*p && isWhite(*p)) {
            if (*(p) == '\n') ++curlineNum;
            ++p;
        }
        return p;
    }
    static char* skipWhiteSpace(char* p, int& curlineNum) {
        return const_cast<char*>(
            skipWhiteSpace(const_cast<const char*>(p), curlineNum));
    }
    static void skipWhiteSpace(std::string& str, int& curlineNum) {
        auto pl = str.begin(), pr = str.end();
        while (pl != pr && isWhite(*pl)) {
            if (*(pl) == '\n') ++curlineNum;
            ++pl;
        }
        str = std::string(pl, pr);
    }
    static void skipWhiteSpace(std::string::iterator& begin,
                               std::string::iterator& end, int& curlineNum) {
        while (begin != end && isWhite(*begin)) {
            if (*(begin) == '\n') ++curlineNum;
            ++begin;
        }
    }
    static void skipchar(std::string::iterator& begin,
                         std::string::iterator& end, char c) {
        while (begin != end && *(begin) == c) ++begin;
    }
    static bool isWhite(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
    static bool isNameStartChar(unsigned char c) {
        if (c >= 128 || isalpha(c)) return 1;
        return c == ':' || c == '_';
    }
    static bool isNameChar(char c) {
        return isNameStartChar(c) || isdigit(c) || c == '.' || c == '-';
    }
    static bool iterEqual(const std::string::iterator& begin,
                          const std::string::iterator& end, const char* dest) {
        std::string w = std::string(begin, end);
        return w == dest;
    }
};
class XmlNode {
    friend class XmlDocument;
    friend class XmlPrinter;
    using Type_KeyValue = std::unordered_map<std::string, std::string>;
    using Type_Son = std::vector<XmlNode*>;

   private:
    int depth;
    XmlNode* fa;
    mutable Type_Son next;
    struct tag {
        std::string tagName;
        Type_KeyValue key_value;
    } Tag;
    std::string content;
    XmlSyntax type;

   public:
    XmlNode() = default;
    void setContent(const std::string& s) { content = s; }
    void setType(XmlSyntax t) { type = t; }
    void setTagName(const std::string& s) { Tag.tagName = s; }
    void setTagKeyValue(Type_KeyValue&& mp) { Tag.key_value = mp; }
    void setFather(XmlNode* f) {
        fa = f;
        depth = f->depth + 1;
    }
    auto getFather() { return fa; }
    auto getTagKeyValue() { return Tag.key_value; }
    auto getTagName() { return Tag.tagName; }
    auto getType() { return type; }
    auto getContent() { return content; }
    XmlNode* getLastSonByType(XmlSyntax Type) {
        for (auto& x : reverse(next)) {
            if (x->type == Type) return x;
        }
        return nullptr;
    }
    XmlNode(const std::string& Content, XmlSyntax Type = XmlSyntax::Xmlelement)
        : content(Content), type(Type) {
        next.reserve(3);
    }
    XmlNode(const std::string& Content, Type_KeyValue&& Key_value,
            const std::string& tagName = "",
            XmlSyntax Type = XmlSyntax::Xmlelement)
        : XmlNode(Content, Type) {
        Tag.key_value = std::move(Key_value);
        Tag.tagName = tagName;
    }
    void addNode(XmlNode* addThis) {
        addThis->depth = depth + 1;
        addThis->fa = this;
        next.push_back(addThis);
    }
    static auto makeNode(std::string&& Content, std::string&& TagName,
                         Type_KeyValue*&& pKeyvalue) {
        using std::move;
        auto res = new XmlNode();
        res->type = XmlSyntax::Xmlelement;
        if (!pKeyvalue) res->Tag.key_value = move(*pKeyvalue);
        res->Tag.tagName = move(TagName);
        res->content = move(Content);
        return res;
    }
};
class XmlPrinter {};
class XmlDocument {
   private:
    XmlNode* root = nullptr;
    std::string FileName;
    std::ifstream in;
    std::ofstream out;
    bool __isInComment = false;
    bool __isInCDATA = false;
    bool __isIndtd = false;
    int curLine = 1;
    using It = std::string::iterator;
    static constexpr const char* xmlRaw[] = {
        "<?", "<!--", "<![CDATA[", "<!", "<",
    };
    static constexpr const int xmlRawLen[] = {2, 4, 9, 2, 1};
    static constexpr const char* xmlHeader = {"<?"};           // 1
    static constexpr const char* commentHeader = {"<!--"};     // 2
    static constexpr const char* cdataHeader = {"<![CDATA["};  // 3
    static constexpr const char* dtdHeader = {"<!"};           // 4
    static constexpr const char* elementHeader = {"<"};        // 5
    static const int maxDepth = 20;
    static const int xmlHeaderLen = 2;
    static const int commentHeaderLen = 4;
    static const int cdataHeaderLen = 9;
    static const int dtdHeaderLen = 2;
    static const int elementHeaderLen = 1;
    void printInfo(const std::string&,XmlParserInfo,int,int);
    void printTree(XmlNode*, int);
    void parse(XmlNode*, int, int);
    void __parse(XmlNode*, int, int&, std::string&, It&, It&);
    void __clearState();
    void __setOpenstate();
    void __parseKeyValue(std::string&, XmlNode*, It&, It&);
    void __nextLine(std::string&, It&, It&, int&);
    void __checkEnd(std::string&, It&, It&, int&);
    int identify(std::string::iterator&, std::string::iterator&, int&);

   public:
    XmlDocument(const std::string&);
    // XmlDocument(const std::string& filecontent);
    XmlDocument() = default;
    void open(const std::string&);
    void save(const std::string& filename);
    void Parse();
};

};  // namespace xmlParser