#ifndef ENUMS_H
#define ENUMS_H
#include <QString>

namespace Menu {
enum class MenuType{
    Close,
    Max,
    Min
};
inline QString toString(MenuType menuType){
    switch (menuType) {
    case MenuType::Close:
        return "menuClose";
        break;
    case MenuType::Max:
        return "menuMax";
        break;
    case MenuType::Min:
        return "menuMin";
        break;
    default:
        break;
    }
}

inline QString intToString(int menuType){
    return toString(static_cast<MenuType>(menuType));
}

enum class LeftMenu{
    KeyManager,
    DataSummar,
    MonitorMenu,
    ConfigMenu,
    CommandLine
};

};

#endif // ENUMS_H
