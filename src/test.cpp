#include "stdc++.h"
#include "xml.h"
#include "xmlfile.h"

const int testMax = 3;
void RunUnitTest() {
    using namespace std;
    freopen("/root/XML/test/test.out", "w", stdout);
    xmlFile::setiostreamMode();
    for (int i = 1; i <= testMax; ++i) {
        string now = "/root/XML/test/datain";
        now += to_string(i) + ".xml";
        xmlFile::__destroyAll(xmlFile::__getTreeRoot());
        xmlFile::init(now);
        xmlFile::addTags();
        cout << "=====================Tree==============\n";
        printTree(xmlFile::__getTreeRoot(), 1);
        cout << "\n\n";
        xmlFile::printDebugInfo();
    }
}
