# 高性能 WebServer 项目

一个基于 **epoll + 线程池** 的高性能 HTTP Web 服务器实现，支持静态资源访问和多客户端并发连接。

## ✨ 特性

- 🚀 **高性能 I/O**：使用 epoll 边缘触发（ET）模式 + 非阻塞 IO
- 🧵 **并发处理**：线程池模型，支持多客户端并发访问
- 📄 **HTTP 解析**：基于状态机的 HTTP/1.1 请求解析
- 🔄 **长连接支持**：支持 HTTP keep-alive
- 📦 **零拷贝**：使用 mmap + writev 减少数据拷贝
- ⚡ **错误处理**：完善的 HTTP 错误响应（400/403/404/500）

## 🏗️ 架构

```
客户端
  │
  ▼
主线程 (main.cpp)
  ├─ epoll 事件循环
  ├─ 接受新连接
  └─ 分发任务到线程池
       │
       ▼
工作线程池 (threadpool.h)
  └─ 处理 HTTP 请求
       │
       ▼
HTTP 连接处理 (http_conn.cpp)
  ├─ 解析请求（状态机）
  ├─ 定位资源文件
  └─ 生成响应
```

## 📁 项目结构

```
web_server/
├── main.cpp              # 主程序，事件循环
├── http_conn.h/cpp       # HTTP 连接处理类
├── threadpool.h          # 线程池模板类
├── locker.h              # 同步机制封装（互斥锁、信号量）
├── resources/            # 静态资源目录
│   ├── index.html
│   └── images/
└── noactive/             # 定时器示例（非活动连接管理）
    ├── lst_timer.h
    └── nonactive_conn.cpp
```

## 🚀 快速开始

### 编译

```bash
g++ -o webserver main.cpp http_conn.cpp -lpthread
```

### 运行

```bash
./webserver 8080
```

### 测试

在浏览器中访问：
- `http://localhost:8080/index.html`
- `http://localhost:8080/images/image1.jpg`

或使用 curl：

```bash
curl http://localhost:8080/index.html
```

## 🔧 技术栈

- **网络编程**：socket、epoll、非阻塞 IO
- **并发模型**：pthread 线程池
- **协议解析**：HTTP/1.1 状态机解析
- **系统优化**：mmap、writev（零拷贝）
- **同步机制**：互斥锁、信号量

## 📖 核心模块说明

### 1. main.cpp - 主事件循环
- 初始化网络（socket、bind、listen）
- 创建 epoll 对象监听事件
- 创建线程池处理任务
- 事件循环：处理连接、读写、错误

### 2. http_conn - HTTP 连接处理
- **主状态机**：解析请求行 → 请求头 → 请求体
- **从状态机**：按 `\r\n` 切分 HTTP 行
- **文件服务**：使用 mmap 映射文件，writev 发送

### 3. threadpool - 线程池
- 管理工作线程和任务队列
- 使用信号量实现生产者-消费者模型
- 模板设计，可复用

### 4. locker - 同步机制
- 封装 pthread 互斥锁、条件变量、信号量
- RAII 风格，异常安全

## 🎯 关键技术点

### epoll 边缘触发（ET）
- 只在状态变化时触发，减少系统调用
- 必须配合非阻塞 IO，循环读取直到 EAGAIN

### EPOLLONESHOT
- 防止多个线程同时处理同一个 fd
- 处理完事件后需要重新注册

### 零拷贝技术
- `mmap()`：文件直接映射到内存，无需 read() 拷贝
- `writev()`：一次系统调用发送多个缓冲区

### HTTP 状态机解析
- 主状态机控制解析阶段（请求行/头/体）
- 从状态机控制行解析（完整/不完整/错误）

## 📊 性能优化

- ✅ epoll ET 模式（减少系统调用）
- ✅ 非阻塞 IO（避免线程阻塞）
- ✅ 线程池（避免频繁创建线程）
- ✅ mmap + writev（零拷贝）
- ✅ 端口复用 SO_REUSEADDR

## 🐛 已知问题

1. `doc_root` 路径硬编码在代码中
2. Content-Type 固定为 `text/html`，未根据文件类型设置
3. 错误处理不够完善，缺少日志系统
4. 线程池析构时未等待所有线程退出

## 🔮 改进方向

- [ ] 配置化（从配置文件读取参数）
- [ ] 路由功能（支持动态路由）
- [ ] 日志系统（异步日志）
- [ ] 连接超时管理（参考 noactive 目录）
- [ ] 支持更多 HTTP 方法（POST、PUT、DELETE）
- [ ] 支持 HTTPS

## 📚 学习资源

- 《Linux 高性能服务器编程》- 游双
- 《UNIX 网络编程 卷1》- Stevens
- RFC 7230-7237（HTTP/1.1 协议规范）

## 📝 复盘文档

详细的技术分析和学习总结请参考：[复盘文档.md](./复盘文档.md)

## 📄 许可证

本项目仅用于学习目的。

