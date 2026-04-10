# RedisDesk 项目功能分析

## 项目概述

**RedisDesk** 是一个用 C++17 + Qt Widgets 构建的桌面端 Redis 管理工具，面向开发、调试和运维场景，允许用户通过图形界面可视化地管理和监控 Redis 实例，无需使用命令行工具。

---

## 技术栈

| 项目 | 技术 |
|---|---|
| 语言 | C++17 |
| GUI 框架 | Qt Widgets（支持 Qt5 / Qt6） |
| 网络通信 | Qt Network（TCP 直连 Redis） |
| 通信协议 | 自实现的 RESP 轻量解析器 |
| 构建系统 | CMake 3.16+ |
| 平台支持 | Windows / macOS / Linux |
| 开源协议 | MIT |
| 当前版本 | v0.2.1 |

---

## 核心功能模块

### 1. 连接管理

- 创建、编辑、删除 Redis 连接配置
- 测试连接可用性
- 导入 / 导出连接配置
- 支持启动时自动连接
- 连接配置持久化（JSON 格式存储于用户配置目录）

### 2. 键值管理

![Key Manager](images/key-manager.png)

- 基于 `SCAN` 命令的增量加载（避免阻塞）
- 分页浏览 / 全量加载模式
- 通配符关键词搜索
- 按数据类型过滤（string / hash / list / set / zset）
- 命名空间树形视图（以 `:` 为分隔符解析层级）

### 3. 数据编辑

支持 5 种 Redis 核心数据类型的查看与编辑：

| 类型 | 操作 |
|---|---|
| `string` | 查看、修改值 |
| `hash` | 查看字段列表、编辑 field/value |
| `list` | 查看元素、编辑列表项 |
| `set` | 查看成员、增删元素 |
| `zset` | 查看成员及分数、增删元素 |

### 4. 键操作

- 新建键（指定类型和初始值）
- 删除键
- 修改键的 TTL（过期时间）
- 重命名键
- 切换数据库（db 0 ~ 15）

### 5. 数据概览

![Data Summary](images/data-summary.png)

统计仪表板，展示当前 Redis 实例的整体状态：

- 键总数量
- 内存使用量
- 当前连接客户端数
- Redis 版本与实例角色（主/从）
- 服务运行时长（Uptime）
- 各数据类型的键数量分布

### 6. 性能监控

![Performance Monitor](images/performance-monitor.png)

实时采样并绘制历史曲线，监控指标包括：

- CPU 使用率
- 命令吞吐量（ops/sec）
- 缓存命中率
- 网络 I/O（读/写字节）
- 内存使用趋势

> 实现方式：轮询 `INFO` 命令采样，非 `MONITOR` 命令流。

### 7. 配置管理

![Config Management](images/config-management.png)

- 使用 `CONFIG GET *` 查询所有 Redis 配置项
- 使用 `CONFIG SET` 在线修改配置

---

## 项目结构

```
redis-desk/
├── src/
│   ├── main.cpp                       # 应用入口
│   ├── mainwindow.h/cpp               # 主窗口（标签页 + 侧边栏）
│   ├── constants/
│   │   ├── constants.h                # 应用常量
│   │   └── enums.h                    # 菜单枚举与类型定义
│   ├── redis/
│   │   └── redisclient.h/cpp          # RESP 协议实现 + 命令队列
│   ├── models/
│   │   └── connectionconfig.h/cpp     # 连接配置模型 + JSON 持久化
│   ├── dialogs/
│   │   ├── connectiondialog.h/cpp     # 新建/编辑连接对话框
│   │   ├── addkeydialog.h/cpp         # 新建键对话框
│   │   └── settingsdialog.h/cpp       # 应用设置对话框
│   ├── widgets/
│   │   ├── connectionpanel.h/cpp      # 连接列表面板
│   │   ├── leftmenupanel.h/cpp        # 左侧导航菜单
│   │   ├── righttopwidget.h/cpp       # 顶部工具栏
│   │   ├── keymanagerpage.h/cpp       # 键值浏览主界面
│   │   ├── datasummarypage.h/cpp      # 数据概览仪表板
│   │   ├── monitormenupage.h/cpp      # 性能监控页面
│   │   └── configmenupage.h/cpp       # Redis 配置管理页面
│   ├── delegates/                     # 自定义列表/表格渲染器
│   ├── utils/
│   │   ├── fontmanager.h/cpp          # 字体管理与缓存
│   │   ├── dpitools.h/cpp             # DPI 缩放工具
│   │   └── commonhelper.h/cpp         # 通用工具函数
│   └── resources/
│       ├── images/                    # 应用图标与图片
│       └── styles/                    # QSS 样式表
├── docs/                              # 文档与截图
├── CMakeLists.txt                     # 根构建配置
├── CMakePresets.json                  # CMake 预设配置
├── README.md                          # 中文文档
└── README.en.md                       # 英文文档
```

---

## 已知限制

| 限制项 | 说明 |
|---|---|
| 数据类型 | 不支持 `stream` 类型 |
| 认证方式 | 支持密码 `AUTH`，也支持 ACL 用户名 + 密码认证 |
| 监控方式 | 基于轮询 `INFO`，非实时 `MONITOR` 命令流 |
| RESP 实现 | 轻量自实现，非完整第三方 Redis SDK |
