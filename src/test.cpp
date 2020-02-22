#include "stdc++.h"
#include "xml.h"
int main() {
    using namespace std;
    using namespace xmlParser;
    freopen("/root/myXmlParser/test/dataout1.xml", "w", stdout);
    XmlDocument file;
    file.open("/root/myXmlParser/test/datain1.xml");
    file.Parse();

    return 0;
}
