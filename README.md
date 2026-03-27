# RedisDesk

RedisDesk 是一个基于 Qt Widgets 开发的桌面端 Redis 管理工具，面向日常开发、调试和运维场景，提供连接管理、键浏览、值编辑、数据概览、性能监控和配置管理等能力。

## 仓库简介

轻量级 Qt 桌面版 Redis 管理工具，支持连接配置、Key 浏览与编辑、运行监控和配置项管理。

## 项目概述

项目使用 Qt Widgets 构建图形界面，底层基于 `QTcpSocket` 实现了一个轻量级 Redis RESP 客户端，当前版本已经覆盖常见 Redis 桌面管理需求：

- 多连接配置的新增、编辑、删除与持久化保存
- 连接前测试可用性
- 启动后自动连接首个已保存连接
- 连接配置的 JSON 导入与导出
- 基于 `SCAN` 的键浏览、分页加载与全部加载
- 支持通配符搜索和按类型筛选
- 支持 `db 0` 到 `db 15` 的数据库切换
- 按 `:` 命名空间层级展示 Key
- 支持查看和编辑 `string`、`hash`、`list`、`set`、`zset`
- 支持新增 Key、删除 Key、设置 TTL
- 支持 Redis 数据概览、性能监控和配置项管理

## 功能特性

### 连接管理

- 保存连接名称、主机、端口、密码和数据库编号
- 支持编辑已有连接
- 支持删除连接
- 支持导入和导出连接配置 JSON
- 连接配置保存在应用数据目录中

### Key 管理

- 使用 `SCAN` 增量加载 Key，避免一次性全量阻塞
- 支持关键字/通配符搜索
- 支持按 `string`、`hash`、`list`、`set`、`zset` 类型筛选
- 支持数据库切换
- 支持按命名空间树形展示 Key
- 支持详情抽屉查看和编辑内容
- 支持修改 Key TTL
- 支持删除 Key

### 已支持的数据类型

- `string`
- `hash`
- `list`
- `set`
- `zset`

### 数据概览

- 总 Key 数
- 内存使用量
- 客户端连接数
- Redis 版本
- 运行时长
- 实例角色
- 操作系统信息
- 每秒命令数
- Key 类型分布

### 性能监控

- CPU 使用信息
- 命令处理速率
- 命中率统计
- 网络 I/O
- 客户端连接信息
- 内存峰值、碎片率、淘汰键统计
- 最近采样记录表

### 配置管理

- 基于 `CONFIG GET *` 读取配置项
- 支持关键字搜索配置项
- 支持按分类筛选配置项
- 支持通过 `CONFIG SET` 修改配置值

## 技术栈

- C++17
- Qt Widgets
- Qt Network
- CMake 3.16+
- Qt 5 / Qt 6

## 构建说明

### 环境要求

- CMake 3.16 或更高版本
- 安装包含 `Widgets` 和 `Network` 模块的 Qt
- 支持 C++17 的编译器

### 配置项目

如果本地已经通过环境变量 `QTDIR` 指向 Qt 安装目录，可以直接使用仓库里的预设：

```bash
cmake --preset Qt -S . -B build
```

也可以手动指定 Qt 路径：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt
```

### 编译

```bash
cmake --build build
```

## 目录结构

```text
.
├─ src/
│  ├─ constants/      # 常量与枚举
│  ├─ delegates/      # 自定义委托
│  ├─ dialogs/        # 连接、新增 Key、设置等弹窗
│  ├─ models/         # 连接配置持久化
│  ├─ redis/          # Redis 客户端与 RESP 解析
│  ├─ resources/      # 图标、图片、QSS 资源
│  ├─ utils/          # DPI、字体、通用工具
│  └─ widgets/        # 主界面页面与面板组件
├─ html/              # 静态页面或演示资源
├─ CMakeLists.txt
└─ CMakePresets.json
```

## 当前限制

- 当前 Redis 通信层是项目内置的轻量 RESP 实现，不是第三方完整 SDK
- 当前连接认证仅包含密码形式的 `AUTH`，还没有用户名 ACL 支持
- 当前主要覆盖常见基础数据类型，不包含 `stream`
- 当前监控页基于 `INFO` 轮询，不是 Redis `MONITOR` 命令流式监听

## 后续可扩展方向

- 支持 Redis ACL 用户名
- 支持 TLS / SSH Tunnel
- 支持 Stream 类型管理
- 支持大 Value 的分页加载与更完整编辑
- 支持更安全的配置项编辑提示
- 增加安装包与发布流程

## License

本项目基于 MIT License 开源，详见 [LICENSE](LICENSE)。
