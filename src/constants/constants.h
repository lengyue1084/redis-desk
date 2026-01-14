#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {
// 窗口尺寸
const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 768;
const double DIALOG_COEFFICIENT = 0.1;

const int WINDOW_DIALOG_WIDTH = WINDOW_WIDTH * ( 1 - DIALOG_COEFFICIENT );
const int WINDOW_DIALOG_HEIGHT = WINDOW_HEIGHT * ( 1 - DIALOG_COEFFICIENT );

// 布局尺寸
constexpr int TOP_WIDGET_HEIGHT = 40;
constexpr int TOOLBAR_ICON_SIZE = 24;
constexpr int SPLITTER_HANDLE_WIDTH = 1;
constexpr int LEFT_MENU_MIN_WIDTH = 200;
constexpr int RIGHT_CONTENT_MIN_WIDTH = 20;

// 字体大小常量
constexpr int BASE_FONT_SIZE = 14;
constexpr int TITLE_FONT_SIZE = 18;
constexpr int TEXT_FONT_SIZE = 12;
constexpr int SMALL_FONT_SIZE = 10;

// 布局比例常量
constexpr double LEFT_PANEL_RATIO = 0.3;
constexpr double RIGHT_PANEL_RATIO = 0.7;

// 使用内联函数替代QString常量
inline const QString& styleFilePath() {
    static const QString path = ":/styles/style.qss";
    return path;
}

namespace Icons {
inline const QString& client() { static const QString icon = ":/images/icons/client.png"; return icon; }
inline const QString& query() { static const QString icon = ":/images/icons/query.png"; return icon; }
inline const QString& about() { static const QString icon = ":/images/icons/about.png"; return icon; }
}

// 菜单文本
namespace MenuText {
inline const QString& file() { static const QString text = "文件"; return text; }
inline const QString& newConnection() { static const QString text = "新建连接"; return text; }
inline const QString& importConfig() { static const QString text = "导入配置"; return text; }
inline const QString& exit() { static const QString text = "退出"; return text; }
inline const QString& edit() { static const QString text = "编辑"; return text; }
inline const QString& find() { static const QString text = "查找"; return text; }
inline const QString& replace() { static const QString text = "替换"; return text; }
inline const QString& help() { static const QString text = "帮助"; return text; }
inline const QString& about() { static const QString text = "关于"; return text; }
}

// 其他命名空间类似修改...
namespace ToolBarText {
inline const QString& connect() { static const QString text = "连接"; return text; }
inline const QString& query() { static const QString text = "查询"; return text; }
inline const QString& about() { static const QString text = "关于"; return text; }
}

namespace StatusBarText {
inline const QString& ready() { static const QString text = "就绪"; return text; }
}

namespace LeftMenuText {
inline const QString& redis() { static const QString text = "Redis"; return text; }
inline const QString& tools() { static const QString text = "常用工具"; return text; }
inline const QString& localhost() { static const QString text = "127.0.0.1"; return text; }
inline const QString& remoteHost() { static const QString text = "192.168.1.1"; return text; }
inline const QString& base64Encode() { static const QString text = "base64编码"; return text; }
inline const QString& jsonFormat() { static const QString text = "json格式化"; return text; }
}

namespace Style {
inline const QString& topWidgetBackground() { static const QString style = "background-color: #f0f0f0;"; return style; }
inline const QString& splitterHandle() { static const QString style = "QSplitter::handle { background-color: #cccccc; }"; return style; }
}

namespace Ttile {
inline const QString& projectTitle(){ static const QString title = "RedisAdmin";return title;}

}

} // namespace Constants

#endif // CONSTANTS_H
