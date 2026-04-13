# RedisDesk 项目功能分析

## 项目概述

**RedisDesk** 是一个用 C++17 + Qt Widgets 构建的桌面端 Redis 管理工具，面向开发、调试和运维场景，允许用户通过图形界面可视化地管理和监控 Redis 实例，并可直接打开内置 CLI 执行命令。

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
| 当前版本 | v0.2.2 |

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

- 基于 `SCAN` 命令的增量加载，避免大库阻塞
- 支持分页浏览和全量加载
- 支持通配符搜索
- 支持按数据类型过滤（`string` / `hash` / `list` / `set` / `zset`）
- 以 `:` 为分隔符展示命名空间树

### 3. 内置 CLI

![CLI](images/cli.png)

- 可从当前活动连接的右键菜单直接进入命令行
- 适合临时执行 Redis 原生命令或调试查询
- 与图形界面配合使用，减少在外部终端之间切换

### 4. 数据编辑

支持 5 种 Redis 核心数据类型的查看与编辑：

| 类型 | 操作 |
|---|---|
| `string` | 查看、修改值 |
| `hash` | 查看字段列表、编辑 `field/value` |
| `list` | 查看元素、编辑列表项 |
| `set` | 查看成员、增删元素 |
| `zset` | 查看成员及分数、增删元素 |

### 5. Key 操作

- 新建键（指定类型和初始值）
- 删除键
- 修改 TTL（过期时间）
- 重命名键
- 切换数据库（`db 0` ~ `db 15`）

### 6. 数据概览

![Data Summary](images/data-summary.png)

统计仪表板展示当前 Redis 实例的整体状态：

- Key 总数
- 内存使用量
- 当前连接客户端数
- Redis 版本与实例角色（主 / 从）
- 服务运行时长（Uptime）
- 各数据类型的 Key 数量分布

### 7. 性能监控

![Performance Monitor](images/performance-monitor.png)

实时采样并绘制历史曲线，监控指标包括：

- CPU 使用率
- 命令吞吐量（ops/sec）
- 缓存命中率
- 网络 I/O（读 / 写字节）
- 内存使用趋势

> 实现方式：轮询 `INFO` 命令采样，非 `MONITOR` 命令流。

### 8. 配置管理

![Config Management](images/config-management.png)

- 使用 `CONFIG GET *` 查询所有 Redis 配置项
- 使用 `CONFIG SET` 在线修改配置

---

## 项目结构

```text
redis-desk/
|-- src/
|   |-- main.cpp                       # 应用入口
|   |-- mainwindow.h/cpp               # 主窗口
|   |-- constants/
|   |-- redis/
|   |-- models/
|   |-- dialogs/
|   |-- widgets/
|   |-- delegates/
|   |-- utils/
|   `-- resources/
|-- docs/                              # 文档与截图
|-- CMakeLists.txt
|-- CMakePresets.json
|-- README.md
`-- README.en.md
```

---

## 已知限制

| 限制项 | 说明 |
|---|---|
| 数据类型 | 不支持 `stream` 类型 |
| 认证方式 | 支持密码 `AUTH`，也支持 ACL 用户名 + 密码认证 |
| 监控方式 | 基于轮询 `INFO`，非实时 `MONITOR` 命令流 |
| RESP 实现 | 轻量自实现，非完整第三方 Redis SDK |
