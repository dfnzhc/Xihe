# 基础设施 Core

Core 是整个项目的基础设施，包含了日志、内存、文件、配置、多线程等最底层的服务，它不依赖于引擎的任何其他部分。


# 结构设计

- Base：最底层的功能宏、类型定义、错误断言等。 (几乎无依赖)
- Debug：调试、性能分析(Tracy)
- Events：事件系统
- Math：数学实现(Eigen)
- Memory：内存相关，分配器等(Mimalloc)
- Services
- Threading：线程相关
- Utils：各种工具组件


# Base

# Events

# Math

# Memory

# Threading

# Utils