#pragma once
// #include <boost/regex.hpp>
#include "stdc++.h"
#include "xml.h"

namespace xmlFile {
// const char* xmlHeader = "<?xml";
// const char* commentHeader = "<!--";
// const char* dtdHeader = "<!";
// const char* cdataHeader = "<![CDATA[";
struct XMLFileInfo {
    std::string encoding = "UTF-8", version;
    bool isalone = 1;
};
struct TTreeNode {
    using xiTreeNodeList = std::vector<TTreeNode*>;
    int Generation = 1;
    TTreeNode* Parent = nullptr;
    xiTreeNodeList Children;
    std::string Content;
    struct Tag {
        std::string TagName = "Virtual root";
        std::map<std::string, std::string> key_value;
        XmlSyntax tagType = XmlSyntax::XmlDocumentSyntax;
    } tag;
    auto __makeTreeNode(const TTreeNode::Tag& rhs) {
        auto nxt = new TTreeNode();
        nxt->Generation = this->Generation + 1;
        nxt->tag = rhs;
        nxt->Parent = this;
        return nxt;
    }
    bool addNode(const TTreeNode::Tag& rhs) {
        auto nxt = __makeTreeNode(rhs);
        this->Children.emplace_back(nxt);
        return nxt;
    }
};
struct __XMLStackReader {
    size_t tot;
    TTreeNode* pNode;
};
bool hasNext();
void setiostreamMode();
void getXMLInfo(const std::string&);
void init(const std::string&);
TTreeNode* __getTreeRoot();
void printDebugInfo();
void getXMLInfo(const std::string&);
void printTree(TTreeNode*, int);
void __destroyAll(TTreeNode*);
std::string __Trimstring(const std::string&);
auto getKeyValue(const std::string& );
auto trimTagEnd(const std::string& );
void parseTagArgv(const std::string& , TTreeNode* const );
void addTags();
bool hasNext();
}  // namespace xmlFile
