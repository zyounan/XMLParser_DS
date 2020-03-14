#include "stdc++.h"
#include "reflect.h"

namespace Reflect {
class Test : public ReflectBase, DynamicCreator<Test> {
   public:
    Test() {
        // std::cout << "Reflect ok" << std::endl;
    }
    virtual void Work() {
        // std::cout << "114514" << std::endl;
        return;
    }
};
void initReflect() {
    Worker worker;
    ReflectBase* p = worker.Create("Reflect::Test");
    p->Work();
}
}  // namespace Reflect