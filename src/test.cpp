#include "stdc++.h"
#include "xml.h"
#include "xmlfile.h"

const int testMax = 3;
void RunUnitTest() {
    using namespace std;
    freopen("/root/XML/test/test.out", "w", stdout);
    XmlDocument doc;
    for (int i = 1; i <= testMax; ++i) {
        string File = "/root/XML/test/datain", File2 = "/root/XML/test/dataout";
        File += to_string(i) + ".xml", File2 += to_string(i) + ".xml";
        doc.loadFile(File);
        doc.parse();
        doc.saveFile(File2);
        // doc.printTree();
        // cout << "=====================Tree==============\n";
        // cout << "\n\n";
        // auto tmp = doc.getXmlFileInfo();
        // cout << "Encoding : " << tmp.encoding << "\nVersion : " <<
        // tmp.version
        //      << "\n";
    }
}
