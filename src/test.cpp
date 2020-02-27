#include "stdc++.h"
#include "xml.h"
int main() {
    using namespace std;
    using namespace xmlParser;
    freopen("/root/XML/test/dataout1.xml", "w", stdout);
    XmlDocument file;
    file.open("/root/XML/test/datain1.xml");
    file.Parse();

    return 0;
}
