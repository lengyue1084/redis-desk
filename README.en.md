# RedisDesk

[中文](README.md)

RedisDesk is a desktop Redis management tool built with Qt Widgets. It is designed for day-to-day development, debugging, and operations workflows, with features for connection management, key browsing, value editing, data overview, performance monitoring, and configuration management.

## Release Downloads

- GitHub Release: https://github.com/lengyue1084/redis-desk/releases/tag/v0.2.2
- Gitee Release: https://gitee.com/xiaopangda/redis-desk/releases/tag/v0.2.2

## Features

- Connection management: create, edit, delete, test, import, and export Redis connections
- Auto-connect: optionally connect to the first saved connection on startup
- Key management: incremental key loading with `SCAN`, pagination, load-all, wildcard search, and type filters
- Namespace view: display keys in a tree grouped by `:` segments
- Data editing: view and edit `string`, `hash`, `list`, `set`, and `zset`
- Key operations: create keys, delete keys, update TTL, and switch between `db 0` and `db 15`
- Data overview: show total keys, memory usage, connected clients, Redis version, uptime, instance role, and key type distribution
- Performance monitoring: show CPU, command throughput, hit rate, network I/O, memory metrics, and recent samples
- Configuration management: read settings with `CONFIG GET *` and update values with `CONFIG SET`
- Built-in CLI: open a Redis command line directly from the active connection context menu

## Screenshots

### Key Manager

![Key Manager](docs/images/key-manager.png)

### CLI

![CLI](docs/images/cli.png)

### Data Summary

![Data Summary](docs/images/data-summary.png)

### Performance Monitor

![Performance Monitor](docs/images/performance-monitor.png)

### Config Management

![Config Management](docs/images/config-management.png)

## Tech Stack

- C++17
- Qt Widgets
- Qt Network
- CMake 3.16+
- Qt 5 / Qt 6

## Build

### Requirements

- CMake 3.16 or newer
- Qt with the `Widgets` and `Network` modules installed
- A compiler with C++17 support

### Configure

If `QTDIR` points to your Qt installation, you can use the included preset:

```bash
cmake --preset Qt-Release -S .
```

You can also configure Qt manually:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt -DCMAKE_BUILD_TYPE=Release
```

### Build Command

```bash
cmake --build out/build/release --config Release
```

## Project Structure

```text
.
|-- docs/
|   `-- images/
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
|-- CMakePresets.json
`-- README.md
```

## Current Limitations

- The Redis communication layer is a lightweight in-project RESP implementation rather than a full external SDK
- Authentication supports both password-based `AUTH` and ACL username/password authentication
- The current implementation focuses on common core data types and does not include `stream`
- The monitoring page polls `INFO` data and does not use streaming from the Redis `MONITOR` command

## License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.
