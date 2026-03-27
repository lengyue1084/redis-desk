# RedisDesk

[中文](#中文说明) | [English](#english)

## 中文说明

RedisDesk 是一个基于 Qt Widgets 开发的桌面端 Redis 管理工具，面向日常开发、调试和运维场景，提供连接管理、键浏览、值编辑、数据概览、性能监控和配置管理等能力。

### 功能特性

- 连接管理：支持新增、编辑、删除、测试、导入和导出 Redis 连接配置
- 自动连接：启动后可自动连接首个已保存连接
- Key 管理：基于 `SCAN` 增量加载 Key，支持分页、全部加载、通配符搜索和按类型筛选
- 命名空间展示：按 `:` 层级树形展示 Key
- 数据编辑：支持查看和编辑 `string`、`hash`、`list`、`set`、`zset`
- Key 操作：支持新增 Key、删除 Key、修改 TTL、切换 `db 0` 到 `db 15`
- 数据概览：展示 Key 总数、内存使用、客户端连接数、Redis 版本、运行时长、实例角色和 Key 类型分布
- 性能监控：展示 CPU、命令处理速率、命中率、网络 I/O、内存指标和最近采样记录
- 配置管理：支持基于 `CONFIG GET *` 查询配置，并通过 `CONFIG SET` 修改配置值

### 技术栈

- C++17
- Qt Widgets
- Qt Network
- CMake 3.16+
- Qt 5 / Qt 6

### 构建说明

#### 环境要求

- CMake 3.16 或更高版本
- 安装包含 `Widgets` 和 `Network` 模块的 Qt
- 支持 C++17 的编译器

#### 配置项目

如果本地已通过环境变量 `QTDIR` 指向 Qt 安装目录，可直接使用仓库中的预设：

```bash
cmake --preset Qt -S . -B build
```

也可以手动指定 Qt 路径：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt
```

#### 编译

```bash
cmake --build build
```

### 目录结构

```text
.
|-- src/
|   |-- constants/
|   |-- delegates/
|   |-- dialogs/
|   |-- models/
|   |-- redis/
|   |-- resources/
|   |-- utils/
|   `-- widgets/
|-- html/
|-- CMakeLists.txt
`-- CMakePresets.json
```

### 当前限制

- 当前 Redis 通信层是项目内置的轻量 RESP 实现，不是第三方完整 SDK
- 当前认证仅支持密码形式的 `AUTH`，还没有用户名 ACL 支持
- 当前主要覆盖常见基础数据类型，不包含 `stream`
- 当前监控页基于 `INFO` 轮询，不是 Redis `MONITOR` 命令流式监听

### 后续方向

- 支持 Redis ACL 用户名
- 支持 TLS / SSH Tunnel
- 支持 `stream` 类型管理
- 优化大 Value 的分页加载和编辑体验
- 增加安装包和发布流程

## English

RedisDesk is a desktop Redis management tool built with Qt Widgets. It is designed for day-to-day development, debugging, and operations workflows, with features for connection management, key browsing, value editing, data overview, monitoring, and configuration management.

### Features

- Connection management: create, edit, delete, test, import, and export Redis connections
- Auto-connect: optionally connect to the first saved connection on startup
- Key management: incremental key loading with `SCAN`, pagination, load-all, wildcard search, and type filters
- Namespace view: display keys in a tree grouped by `:` segments
- Data editing: view and edit `string`, `hash`, `list`, `set`, and `zset`
- Key operations: create keys, delete keys, update TTL, and switch between `db 0` and `db 15`
- Data overview: show total keys, memory usage, connected clients, Redis version, uptime, instance role, and key type distribution
- Performance monitoring: show CPU, command throughput, hit rate, network I/O, memory metrics, and recent samples
- Configuration management: read settings with `CONFIG GET *` and update values with `CONFIG SET`

### Tech Stack

- C++17
- Qt Widgets
- Qt Network
- CMake 3.16+
- Qt 5 / Qt 6

### Build

#### Requirements

- CMake 3.16 or newer
- Qt with the `Widgets` and `Network` modules installed
- A compiler with C++17 support

#### Configure

If `QTDIR` points to your Qt installation, you can use the included preset:

```bash
cmake --preset Qt -S . -B build
```

You can also configure Qt manually:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt
```

#### Build Command

```bash
cmake --build build
```

### Project Structure

```text
.
|-- src/
|   |-- constants/
|   |-- delegates/
|   |-- dialogs/
|   |-- models/
|   |-- redis/
|   |-- resources/
|   |-- utils/
|   `-- widgets/
|-- html/
|-- CMakeLists.txt
`-- CMakePresets.json
```

### Current Limitations

- The Redis communication layer is a lightweight in-project RESP implementation rather than a full external SDK
- Authentication currently supports password-based `AUTH` only, without username ACL support
- The current implementation focuses on common core data types and does not include `stream`
- The monitoring page polls `INFO` data and does not use streaming from the Redis `MONITOR` command

### Roadmap

- Add Redis ACL username support
- Add TLS / SSH tunnel support
- Add `stream` management
- Improve pagination and editing for large values
- Add packaging and release workflows

## License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.
