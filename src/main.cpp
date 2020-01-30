#include "stdc++.h"
#include "xml.h"
#include "xmlfile.h"
int main() {



    xmlFile::init();
#ifdef DEBUG
    xmlFile::addTags();
    xmlFile::printDebugInfo();
#endif
    return 0;
}