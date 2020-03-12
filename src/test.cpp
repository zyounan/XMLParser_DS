#include "stdc++.h"
#include "xml.h"
#include "ui.h"
int main() {
    using namespace std;
    using namespace xmlParser;
    using namespace UI;
    UI::init();
    UI::loop();
    // freopen("/root/XML/test/dataout1.xml", "w", stdout);
    // XmlDocument file;
    // file.open("/root/XML/test/datain1.xml");
    // try {
    //     file.Parse();
    // } catch (...) {
    //     exit(-1);
    // }
    return 0;
}
