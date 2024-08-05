#include <vector>
#include <string>

struct MyClass1 {
    std::string value;
};
struct MyClass2 {
    std::vector<MyClass1> menuitem;
};
struct MyClass3 {
    std::string id;
    std::string value;
    MyClass2 popup;
    MyClass2 popdown;
};
struct MyClass4 {
    MyClass3 menu;
    MyClass3 ui;
};
