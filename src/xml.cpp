#include "xml.h"

#include "stdc++.h"
namespace xmlParser {

void XmlDocument::Parse() { parse(root, 0, 1); }
int XmlDocument::identify(std::string::iterator& pl, std::string::iterator& pr,
                          int& lineNum) {
    XmlUtil::skipWhiteSpace(pl, pr, lineNum);
    if (pl == pr) {
        return -1;
    }
    for (int i = 1; i <= 5; ++i) {
        if (pr - pl > xmlRawLen[i - 1] &&
            XmlUtil::iterEqual(pl, pl + xmlRawLen[i - 1], xmlRaw[i - 1]))
            return i;
    }
    return 0;  // nothing
}
int XmlDocument::Identify(std::string::iterator& pl,
                          std::string::iterator& pr) {
    int lineNum = 0;
    // std::string tmp = {l,r};
    // auto pl = tmp.begin(),pr = tmp.end();
    XmlUtil::skipWhiteSpace(pl, pr, lineNum);
    if (pl == pr) {
        return -1;
    }
    for (int i = 1; i <= 5; ++i) {
        if (pr - pl > xmlRawLen[i - 1] &&
            XmlUtil::iterEqual(pl, pl + xmlRawLen[i - 1], xmlRaw[i - 1]))
            return i;
    }
    return 0;  // nothing
}
void XmlDocument::parseKeyValue(std::string& str, It& pl, It& pr,
                                XmlNode& result) {
    __parseKeyValue(str, &result, pl, pr);
}
void XmlDocument::__parseKeyValue(std::string& str, XmlNode* cur, It& pl,
                                  It& pr) {
    using namespace std;
    int line = 0;
    while (pl != pr && *pl != '>') {
        string key, value = "__this_is_empty_mark__";
        XmlUtil::skipWhiteSpace(pl, pr, line);
        if (*pl == '>' || *pl == '?') break;
        auto pos = str.find('=', pl - str.begin());
        if (pos == string::npos) break;
        key = std::string(pl, str.begin() + pos);
        if (key.empty())
            throw XmlException(XmlError::XML_ERROR_PARSING_DECLARATION);
        // 跳过'='
        pl = str.begin() + pos + 1;
        char nxt = ' ';

        // 是否有引号括起来
        if (*pl == '\'')
            XmlUtil::skipchar(pl, pr, '\''), nxt = '\'';
        else if (*pl == '\"')
            XmlUtil::skipchar(pl, pr, '\"'), nxt = '\"';
        if (nxt == '\'' || nxt == '\"') {
            pos = str.find(nxt, pl - str.begin());
            if (pos == string::npos) {
                //引号不匹配
                throw XmlException(XmlError::XML_ERROR_MISMATCHED_ELEMENT);
            }
            value.clear();
            value = std::string(pl, str.begin() + pos);
            pl = str.begin() + pos + 1;  //移动到引号之后
        } else if (pl != pr) {
            value.clear();
            value.reserve(128);
            while (!XmlUtil::isWhite(*pl)) {
                value += *(pl++);
            }
            value.shrink_to_fit();
        }

        cur->Tag.key_value.insert(make_pair(key, value));
        if (pl == pr || *pl == '>' || *pl == '?') break;
    }
}
void XmlDocument::__nextLine(std::string& str, It& pl, It& pr, int& line) {
    getline(in, str);
    if (!in.good()) {
        throw std::ifstream::failure("EOF");
    }
    pl = str.begin(), pr = str.end();
    ++line;
}
void XmlDocument::__checkEnd(std::string& str, It& pl, It& pr, int& res) {}
void XmlDocument::printInfo(const std::string& str, XmlParserInfo x, int line,
                            int which) {
    using std::cerr, std::endl;
    static const char* sss[] = {"[Warning]", "[Error]", "[Info]"};
    std::stringstream ss;
    ss << FileName << ":" << line << ":" << which << ": "
       << sss[static_cast<int>(x)] << " : " << str << std::endl;
    __last_info = ss.str();
}
std::string XmlDocument::getLastInfo() { return this->__last_info; }
void XmlDocument::__parse(XmlNode* cur, int depth, int& line, std::string& str,
                          It& pl, It& pr) {
    using namespace std;
    if (pl == pr) {
        __nextLine(str, pl, pr, line);
    }
    if (depth > maxDepth) {
        printInfo("maxDepth exceeded.", XmlParserInfo::Error, line,
                  static_cast<int>(pl - str.begin() + 1));
        throw XmlException(XmlError::XML_ELEMENT_DEPTH_EXCEEDED);
        return;
    }
    string content;
    while (1) {
        if (__isInComment) {
            XmlUtil::skipWhiteSpace(pl, pr, line);
            auto pos = str.find("-->", pl - str.begin());
            auto nxt = cur->getLastSonByType(XmlSyntax::XmlComment);

            if (!nxt) {
                printInfo("comment format is not valid.", XmlParserInfo::Error,
                          line, static_cast<int>(pl - str.begin() + 1));
                throw XmlException(XmlError::XML_ERROR_PARSING_COMMENT);
            }

            content.clear();
            if (pos == string::npos) {
                __isInComment = true;
                content = string(pl, str.end());
                __nextLine(str, pl, pr, line);
                nxt->content += content;
                continue;
            } else {
                __isInComment = false;
                if (static_cast<int>(pos - 1) >
                    pl - str.begin())  //某一行单独以 --> 结尾
                    content = string(pl, str.begin() + pos);
                pl = str.begin() + pos + commentHeaderLen - 1;

                nxt->setEndPos(pos);
                nxt->setEndLine(line);

                nxt->content += content;
            }
        } else if (__isInCDATA) {
            // CDATA
            XmlUtil::skipWhiteSpace(pl, pr, line);
            auto pos = str.find("]]>", pl - str.begin());
            auto nxt = cur->getLastSonByType(XmlSyntax::XmlCDATA);
            if (!nxt) {
                printInfo("Missing CDATA begining.", XmlParserInfo::Error, line,
                          static_cast<int>(pl - str.begin() + 1));
                throw XmlException(XmlError::XML_ERROR_PARSING_CDATA);
            }
            if (pos == string::npos) {
                __isInCDATA = true;
                content = string(pl, str.end());
                __nextLine(str, pl, pr, line);
                nxt->content += content;
                continue;
            } else {
                __isInCDATA = false;
                if (static_cast<int>(pos - 1) >
                    pl - str.begin())  // 某一行单独以 ]]> 结尾
                    content = string(pl, str.begin() + pos);

                nxt->setEndLine(line);
                nxt->setEndPos(pos);

                pl = str.begin() + pos + 3;
                nxt->content += content;
            }
        } else if (__isIndtd) {
            // dtd
            XmlUtil::skipWhiteSpace(pl, pr, line);
            auto pos = str.find("]>", pl - str.begin());
            auto nxt = cur->getLastSonByType(XmlSyntax::XmlUnknown);
            if (!nxt) {
                printInfo("Missing dtd begining.", XmlParserInfo::Error, line,
                          static_cast<int>(pl - str.begin() + 1));

                throw XmlException(XmlError::XML_ERROR_PARSING_UNKNOWN);
            }
            if (pos == string::npos) {
                __isIndtd = true;
                content = string(pl, str.end());
                __nextLine(str, pl, pr, line);
                nxt->content += content;
                continue;
            } else {
                __isIndtd = false;
                if (static_cast<int>(pos - 1) >
                    pl - str.begin())  // 某一行单独以 ]> 结尾
                    content = string(pl, str.begin() + pos);

                nxt->setEndLine(line);
                nxt->setEndPos(pos);

                pl = str.begin() + pos + 2;
                nxt->content += content;
            }
        } else if (__isInLabel) {
            //上一个Label 尚未结束
            XmlUtil::skipWhiteSpace(pl, pr, line);

            while (pl == pr) {
                __nextLine(str, pl, pr, line);
                XmlUtil::skipWhiteSpace(pl, pr, line);
            }

            // auto nxt = cur->getLastSonByType(XmlSyntax::Xmlelement);
            if (cur->getType() != XmlSyntax::XmlTest) {
                printInfo("Invalid label.", XmlParserInfo::Error, line,
                          static_cast<int>(pl - str.begin() + 1));
                throw XmlException(XmlError::XML_ERROR_PARSING_ELEMENT);
            }
            if (*pl != '>') {
                //处理未结束的键值对
                //单独处理某一个分开的
                // key1=
                // value key2=value
                string lastkey;

                for (auto& x : cur->Tag.key_value) {
                    if (x.second == "__this_is_empty_mark__") {
                        lastkey = x.first;
                        break;
                    }
                }

                if (lastkey.length()) {
                    string tmp_s;
                    tmp_s.reserve(128);
                    int _type = 0;
                    if (pl != pr && *pl == '\'') {
                        _type = 1;
                        ++pl;
                    } else if (pl != pr && *pl == '\"') {
                        _type = 2;
                        ++pl;
                    }
                    bool success = 0;
                    while (pl != pr && *pl != ' ' && *pl != '/') {
                        if (_type == 1 && *pl == '\'') {
                            success = 1;
                            ++pl;   //跳过引号
                            break;
                        } else if (_type == 2 && *pl == '\"') {
                            success = 1;
                            ++pl;
                            break;
                        }
                        tmp_s += *pl++;
                    }
                    if(!_type && !tmp_s.size())   success = 0;
                    if (!success) {
                        printInfo("Invalid label.", XmlParserInfo::Error, line,
                                  static_cast<int>(pl - str.begin() + 1));
                        throw XmlException(XmlError::XML_ERROR_PARSING_ELEMENT);
                    }
                    cur->Tag.key_value[lastkey] = tmp_s;
                }
                // bool _flag_ = 0;
                //上一行没有key1
                // if (!_flag_) {
                //     printInfo("Invalid label.", XmlParserInfo::Error,
                //     line,
                //               static_cast<int>(pl - str.begin() + 1));
                //     throw
                //     XmlException(XmlError::XML_ERROR_PARSING_ELEMENT);
                // }
                //把接下来的干完
                __parseKeyValue(str, cur, pl, pr);

                XmlUtil::skipWhiteSpace(pl, pr, line);
                if (pl == pr) {
                    __nextLine(str, pl, pr, line);
                    continue;
                }
                if (*pl != '>') {
                    printInfo("Invalid label.", XmlParserInfo::Error, line,
                              static_cast<int>(pl - str.begin() + 1));
                    throw XmlException(XmlError::XML_ERROR_PARSING_ELEMENT);
                }
            }
            //跳过 />
            ++pl;
            __isInLabel = false;
        }
        auto res = identify(pl, pr, line);
        switch (res) {
            case -1: {
                //空行
                __nextLine(str, pl, pr, line);
            } break;
            case 0: {
                content.clear();

                if (!cur->getBeginLine()) cur->setBeginLine(line);
                if (cur->getBeginPos() != XmlNode::npos)
                    cur->setBeginPos(pl - str.begin());

                while (*pl != '<' && pl != pr) {
                    content += *pl++;
                }
                if (cur->type == XmlSyntax::XmlEmptylabel && content.size()) {
                    //空标签却不为空
                    printInfo("Missing dtd begining.", XmlParserInfo::Error,
                              line, static_cast<int>(pl - str.begin() + 1));
                    throw XmlException(XmlError::XML_ERROR_PARSING_ATTRIBUTE);
                }

                cur->setEndLine(line);
                cur->setEndPos(pl - str.begin());

                cur->content += content;
            } break;
            case 1: {
                // xml头
                content.clear();
                if (!root) {
                    printInfo("Missing tree root.", XmlParserInfo::Error, line,
                              static_cast<int>(pl - str.begin() + 1));
                    throw XmlException(XmlError::XML_NO_TEXT_NODE);
                }
                root->setBeginLine(line);
                root->setBeginPos(pl - str.begin());

                pl += 2;  //跳过"<?"
                while (!XmlUtil::isWhite(*pl) && XmlUtil::isNameChar(*pl)) {
                    content += *(pl++);
                }
                root->Tag.tagName = content;
                content.clear();
                __parseKeyValue(str, cur, pl, pr);

                root->setEndLine(line);
                root->setEndPos(pl - str.begin());

                pl += 2;  //跳过?>

                if (str.rfind("?>") == string::npos) {
                    printInfo("Broken header.", XmlParserInfo::Error, line,
                              static_cast<int>(pl - str.begin() + 1));
                    throw XmlException(XmlError::XML_ERROR_MISMATCHED_ELEMENT);
                }
            } break;
            case 2: {
                //注释
                XmlNode* node = new XmlNode;
                cur->addNode(node);
                node->type = XmlSyntax::XmlComment;

                content.clear();

                node->setBeginLine(line);
                node->setBeginPos(pl - str.begin());

                pl += commentHeaderLen;
                auto pos = str.find("-->", pl - str.begin());
                if (pos == string::npos) {
                    __isInComment = true;
                    content = string(pl, str.end());
                    __nextLine(str, pl, pr, line);
                } else {
                    content = string(pl, str.begin() + pos);
                    pl = str.begin() + pos + commentHeaderLen - 1;

                    node->setEndLine(line);
                    node->setEndPos(pos);
                }
                node->content = content;

            } break;
            case 3: {
                // CDATA
                content.clear();
                XmlNode* node = new XmlNode;
                cur->addNode(node);
                node->type = XmlSyntax::XmlUnknown;

                node->setBeginLine(line);
                node->setBeginPos(pl - str.begin());

                pl += cdataHeaderLen;
                auto pos = str.find("]>", pl - str.begin());
                if (pos == string::npos) {
                    __isInCDATA = true;
                    content = string(pl, str.end());
                    __nextLine(str, pl, pr, line);
                } else {
                    node->setEndLine(line);
                    node->setEndPos(pos);

                    content = string(pl, str.begin() + pos - 1);
                    pl = str.begin() + pos + 2;  //跳过尾部
                }
                node->content = content;
            } break;
            case 4: {
                // DTD
                printInfo(
                    "xml dtd header(s) detected. This parser "
                    "doesn't support dtd header and it will be "
                    "ignored.",
                    XmlParserInfo::Warning, line,
                    static_cast<int>(pl - str.begin()));

                XmlNode* node = new XmlNode;
                cur->addNode(node);
                node->type = XmlSyntax::XmlUnknown;

                content.clear();
                node->setBeginLine(line);
                node->setBeginPos(pl - str.begin());

                pl += cdataHeaderLen;
                auto pos = str.find("]]>", pl - str.begin());
                if (pos == string::npos) {
                    __isIndtd = true;
                    content = string(pl, str.end());
                    __nextLine(str, pl, pr, line);
                } else {
                    node->setEndLine(line);
                    node->setEndPos(pos);
                    content = string(pl, str.begin() + pos - 1);
                    pl = str.begin() + pos + 3;  //跳过尾部
                }

                node->content = content;
            } break;
            case 5: {
                //普通标签
                ++pl;  //跳过<
                XmlNode* node = nullptr;
                bool isTagEnd = 1;
                if (*pl != '/') {
                    //不是标签结尾
                    node = new XmlNode;
                    cur->addNode(node);

                    node->setBeginLine(line);
                    node->setBeginPos(pl - 1 - str.begin());
                    node->setType(XmlSyntax::Xmlelement);

                    isTagEnd = 0;
                } else {
                    ++pl;  //跳过'/'
                    cur->setEndLine(line);
                    cur->setBeginPos(pl - 2 - str.begin());
                }
                // auto pos = str.find('>', pl - str.begin());
                if (XmlUtil::isNameStartChar(*pl)) {
                    content.clear();
                    while (XmlUtil::isNameChar(*pl) && *pl != '>' && pl != pr) {
                        content += *pl++;
                    }
                    if (isTagEnd) {
                        //标签不匹配
                        if (content != cur->Tag.tagName || *pl != '>') {
                            printInfo("Label doesn't match!",
                                      XmlParserInfo::Error, line,
                                      static_cast<int>(pl - str.begin() + 1));
                            throw XmlException(
                                XmlError::XML_ERROR_PARSING_ELEMENT);
                        }
                        ++pl;    //跳过'>'
                        return;  //标签结束
                    } else {
                        // 标签起始
                        node->Tag.tagName = content;
                        if (XmlUtil::isWhite(*pl)) {
                            __parseKeyValue(str, node, pl, pr);
                        }
                        if (*pl == '/') {
                            //该标签为空标签
                            ++pl;
                            node->type = XmlSyntax::XmlEmptylabel;

                            node->setEndLine(line);
                            node->setEndPos(pl - str.begin());
                        }
                        if (*pl != '>' && !XmlUtil::isNameChar(*pl)) {
                            //结尾在另一行
                            while (pl != pr && XmlUtil::isWhite(*pl)) ++pl;

                            if (pl == pr) {
                                //设置一个特殊的标记
                                node->setType(XmlSyntax::XmlTest);

                                __nextLine(str, pl, pr, line);
                                __isInLabel = true;
                                __parse(node,depth + 1,line,str,pl,pr);

                                continue;
                            }

                            printInfo("Broken header.", XmlParserInfo::Error,
                                      line,
                                      static_cast<int>(pl - str.begin() + 1));
                            throw XmlException(
                                XmlError::XML_ERROR_PARSING_ELEMENT);
                        } else if (*pl == '>') {
                            ++pl;  //跳过>
                            if (node->type != XmlSyntax::XmlEmptylabel)
                                __parse(node, depth + 1, line, str, pl, pr);
                            if (pl == pr) {
                                __nextLine(str, pl, pr, line);
                            }
                            continue;
                        }
                    }
                } else {
                    printInfo("Element label doesn't match.",
                              XmlParserInfo::Error, line,
                              static_cast<int>(pl - str.begin() + 1));
                    throw XmlException(XmlError::XML_ERROR_MISMATCHED_ELEMENT);
                }
            } break;
            default:
                printInfo("Unknown content.", XmlParserInfo::Error, line,
                          static_cast<int>(pl - str.begin() + 1));
                throw XmlException(XmlError::XML_ERROR_PARSING_UNKNOWN);
        }
    }
}
void XmlDocument::printTree(XmlNode* u, int d) {
    static auto __printTab = [](int y) {
        for (int i = 1; i <= y; ++i) std::cout << "\t";
    };
    using namespace std;
    if (!u->depth && u->Tag.key_value.size()) {
        cout << "<?xml ";
        for (auto& x : u->Tag.key_value) {
            cout << x.first << "="
                 << "\"" << x.second << "\" ";
        }
        cout << "?>\n";
    }
    for (auto& x : u->next) {
        if (x->type == XmlSyntax::XmlUnknown) continue;
        __printTab(d);
        if (x->type == XmlSyntax::XmlComment) {
            cout << "<!-- ";
            cout << x->content << " -->";
        } else if (x->type == XmlSyntax::XmlCDATA) {
            cout << cdataHeader << " ";
            cout << x->content << " ]]>";
        } else {
            cout << "<" << x->Tag.tagName;
            if (x->Tag.key_value.size()) {
                for (auto& y : x->Tag.key_value)
                    cout << " " << y.first << "="
                         << "\"" << y.second << "\"";
            }
            cout << ">";
            if (x->content.size()) {
                cout << endl;
                __printTab(d);
                cout << x->content;
            }
        }
        cout << endl;
        printTree(x, d + 1);
        if (x->type != XmlSyntax::XmlComment &&
            x->type != XmlSyntax::XmlCDATA) {
            __printTab(d);
            cout << "</" << x->Tag.tagName << ">\n";
        }
    }
}
void XmlDocument::parse(XmlNode* cur, int depth, int line) {
    using namespace std;
    try {
        string str;
        getline(in, str);
        XmlUtil::skipWhiteSpace(str, line);
        auto pl = str.begin(), pr = str.end();
        __parse(cur, depth, curLine, str, pl, pr);
    } catch (const ifstream::failure& e) {
        // cout << "做完了" << endl;
        return;
        // printTree(root, 0);
    }
}
void XmlDocument::__clearState() {
    if (root) delete root;
    in.close(), out.close();
    in.clear(), out.clear();
    FileName.clear();
}
void XmlDocument::__setOpenstate() {
    using namespace std;
    root = new XmlNode("Virtual root");
    if (FileName.empty())
        throw XmlException(XmlError::XML_ERROR_FILE_NOT_FOUND);
    in.open(FileName, ios_base::in);
    if (!in.good())
        throw XmlException(XmlError::XML_ERROR_FILE_COULD_NOT_BE_OPENED);
}
XmlDocument::XmlDocument(const std::string& filename) : FileName(filename) {
    __setOpenstate();
}
void XmlDocument::open(const std::string& filename) {
    __clearState();
    FileName = filename;
    __setOpenstate();
}
void XmlDocument::close() { __clearState(); }
};  // namespace xmlParser