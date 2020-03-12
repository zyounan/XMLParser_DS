#pragma once
#include "stdc++.h"
#include <cxxabi.h>
namespace Reflect {
//反射类的基类；所有的反射类需要从这个类派生
extern void initReflect();
class ReflectBase {
   public:
    ReflectBase() {}
    virtual ~ReflectBase() {}
    // virtual void Test(){}
    virtual void Work() {}
};
template <typename... Targs>
class ReflectFactory {
   public:
    //用于获得工厂的实例
    static ReflectFactory* Instance() {
        if (!pReflectFactory) pReflectFactory = new ReflectFactory();
        return pReflectFactory;
    }
    //将某个类对应的ReflectCreator注册到map中
    bool Regist(const std::string& strTypeName,
                std::function<ReflectBase*(Targs&&... args)> ReflectCreator) {
        if (!ReflectCreator) return false;
        std::string sTypename = strTypeName;
        return map_func.insert(std::make_pair(sTypename, ReflectCreator))
            .second;
    }
    //创建类的实例；同时调用这个类的构造函数
    ReflectBase* Create(const std::string& strTypeName, Targs&&... args) {
        auto iter = map_func.find(strTypeName);
        if (iter == map_func.end()) return nullptr;
        return iter->second(std::forward<Targs>(args)...);
    }

   private:
    //只能通过Instance()来获得唯一的实例；这也就禁止了外部的实例化
    ReflectFactory() {}
    //工厂类的指针
    static ReflectFactory<Targs...>* pReflectFactory;
    // map
    std::unordered_map<std::string, std::function<ReflectBase*(Targs&&...)>>
        map_func;
};

template <typename... Targs>
ReflectFactory<Targs...>* ReflectFactory<Targs...>::pReflectFactory = nullptr;

template <typename T, typename... Args>
class DynamicCreator {
   public:
    //返回创建的对象
    static T* CreateObject(Args&&... args) {
        return new T(std::forward<Args>(args)...);
    }
    //注册接口
    struct Register {
        Register() {
            const char* TypeName = typeid(T).name();
            std::string tmp(128, '\0');
            size_t len;
            int f;
            abi::__cxa_demangle(TypeName, tmp.data(), &len, &f);
            tmp.resize(strlen(tmp.c_str()));
            std::string typeName = tmp;
            //去掉"class "前缀，针对MSVC特殊优化
            // typeName = typeName.substr(6);
            ReflectFactory<Args...>::Instance()->Regist(typeName, CreateObject);
        }
        void do_nothing() {}
    };
    DynamicCreator() {
        //防止注册函数被优化掉
        mRegister.do_nothing();
    }
    virtual ~DynamicCreator() { mRegister.do_nothing(); };
    // virtual ~DynamicCreator() {}
    static Register mRegister;
};
//静态对象Register的实例化
template <typename T, typename... Args>
typename DynamicCreator<T, Args...>::Register
    DynamicCreator<T, Args...>::mRegister;

// Worker用于获得注册的对象，即外部使用这个反射的接口
class Worker {
   public:
    template <typename... args>
    ReflectBase* Create(const std::string& Typename, args&&... arg) {
        return ReflectFactory<args...>::Instance()->Create(
            Typename, std::forward<args>(arg)...);
    }
};

}  // namespace Reflect
