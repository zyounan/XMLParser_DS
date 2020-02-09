#pragma once
// #include <boost/regex.hpp>
#include "stdc++.h"
#include "xml.h"

class XmlTreeNode {
    using xiTreeNodeList = std::vector<XmlTreeNode*>;
    friend class XmlDocument;

   public:
    struct Tag {
        std::string TagName = "Virtual root";
        std::map<std::string, std::string> key_value;
        XmlSyntax tagType = XmlSyntax::XmlDocumentSyntax;
    } tag;
    XmlTreeNode() = default;
    XmlTreeNode(const XmlTreeNode& rhs) = delete;

   private:
    int Generation = 1;
    XmlTreeNode* Parent = nullptr;
    xiTreeNodeList Children;
    std::string Content;
    auto __makeTreeNode(const XmlTreeNode::Tag& rhs) {
        auto nxt = new XmlTreeNode();
        nxt->Generation = this->Generation + 1;
        nxt->tag = rhs;
        nxt->Parent = this;
        return nxt;
    }

   public:
    virtual ~XmlTreeNode() {
        tag.key_value.clear();
        tag.TagName.clear();
        Parent = nullptr, Generation = 1;
        for (auto& x : Children) delete x;
        Children.clear();
        Content.clear();
    }
    bool addNode(const XmlTreeNode::Tag& rhs) {
        auto nxt = __makeTreeNode(rhs);
        this->Children.emplace_back(nxt);
        return nxt;
    }

    XmlTreeNode(XmlTreeNode&& rhs) {
        using std::move;
        tag = move(rhs.tag);
        Generation = rhs.Generation;
        Parent = rhs.Parent;
        Children = move(rhs.Children);
        Content = move(rhs.Content);
    }
    auto getParent() const { return Parent; }
    // 深复制
    XmlTreeNode* copy() {
        auto res = new XmlTreeNode();
        res->Content = Content;
        res->Generation = Generation;
        res->Parent = Parent;
        res->tag = tag;
        for (auto& x : Children) {
            res->Children.push_back(x->copy());
        }
        return res;
    }
};

class XmlDocument {
   public:
    XmlDocument();                             //新建一个文件
    XmlDocument(const std::string&);           //从指定文件名打开
    XmlDocument(const XmlDocument&) = delete;  //复制XML对象
    XmlDocument(XmlDocument&&);
    XmlDocument& operator=(const XmlDocument&) = delete;
    virtual ~XmlDocument();
    bool loadFile(const std::string&);
    bool saveFile(const std::string&);
    //使用当前的文件名
    bool loadFile();
    bool saveFile();
    void parse();

    void printTree();
    struct XmlFileInfo {
        std::string encoding = "UTF-8", version;
        bool isalone = 1;
    };
    XmlFileInfo getXmlFileInfo();

   private:
    std::string Filename;
    std::ifstream file_in;
    std::ofstream file_out;
    XmlFileInfo FileInfo;
    struct __XMLStackReader {
        size_t tot;
        XmlTreeNode* pNode;
    };
    XmlTreeNode __Treeroot, *Treeroot = &__Treeroot;
    void __init();
    void __getXMLFileInfo(const std::string&);
    void __clearIOstate();
    void __printTreeDebug(XmlTreeNode*, int)const;
    void __printTree(XmlTreeNode*const,short);
    void __printTabs(short);
    void __writeHeader();
    void __destroyAll(XmlTreeNode*);
    //去除两侧空白
    std::string __trimString(const std::string&);
    std::pair<std::string, std::string> __getLabelKeyValue(const std::string&);
    // trimTagEnd
    std::string __getTagName(const std::string&);
    void __parseTagAttr(const std::string&, XmlTreeNode* const);
    int __ignore(std::string&, std::string::const_iterator&,
                 std::string::const_iterator&, const char*, const char*, bool&);

   public:
    XmlTreeNode* getTreeRoot() const { return Treeroot; }
    //以下为解析用正则表达式
    static constexpr const char *
        reg_ver = R"...((?<=version\=[\"|\'])[\w|\.]+(?=[\"|\']))...",
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
                   *reg_label_args = R"...((\S+)=[^(>\/\ )]+)...",
       *xmlHeader = "<?xml", *commentHeader = "<!--", *commentEnd = "-->",
       *dtdHeader = "<!", *cdataHeader = "<![CDATA[", *cdataEnd = "]]>";
};

namespace xmlFile {}  // namespace xmlFile
