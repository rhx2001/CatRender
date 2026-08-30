# CatRender

> 一个从零实现的 Vulkan 实时渲染器（学习 / 科研向），用于实践现代图形 API、渲染管线与引擎资源管理。

CatRender 是我基于 Vulkan API 从零搭建的实时渲染器，目标是逐步实践现代图形引擎的核心模块：渲染管线、GPU 资源管理、材质系统与调试 GUI。当前已实现完整的 Vulkan 绘制链路，可加载多模型 / 多材质并实时渲染；后续计划重构为更清晰的引擎架构。

## 已实现功能

**渲染管线（VulkanCore）**
- Vulkan 实例创建、校验层（Validation Layers）与 Debug Messenger 接入
- 基于 GLFW 的窗口与 Surface；物理设备选择（队列族 / Swapchain 支持检测）
- 逻辑设备与 Graphics / Present 队列、Swapchain 及 Image Views
- Render Pass（含深度附件）、光栅化 Graphics Pipeline
- 双缓冲帧同步（Semaphore + Fence，`MAX_FRAMES_IN_FLIGHT = 2`），窗口 Resize 时 Swapchain 重建
- 深度资源（Depth Image / `findDepthFormat`）

**资源与材质（BufferManager / MaterialManager / ModelManager）**
- BufferManager：Vertex / Index Buffer 的 Staging + 拷贝创建；Image 创建、布局迁移、Mipmap 生成（`generateMipmaps`）、ImageView
- 纹理加载与 Mipmap 生成，多纹理支持（`TEXTURE_NUM = 10`）
- MaterialManager：PBR 风格材质块（`baseColor` / `metallic` / `roughness` / `occlusion`，预留扩展槽位）写入大块 Uniform Buffer，按 Dynamic Offset 索引
- MaterialViewer 缓存：同一纹理图像共享 Sampler 与 DescriptorSet，避免重复分配
- ModelManager：ModelInstance 绑定 Mesh + Material，支持多实例（`MAX_NUM_OBJECT = 100`）、带名称 / 变换矩阵创建；多模型加载（viking_room、african_head）

**相机与调试**
- Camera：View / Projection 矩阵
- ImGui 集成（Vulkan 后端，GUIManager）：帧循环内 GUI 绘制，便于参数调试
- 着色器：`.vert` / `.frag` + 编译脚本（`shaders/compile.bat`）

## 架构概览

```
CatRender (App)
  ├─ VulkanCore          渲染后端：实例 / 设备 / Swapchain / 管线 / 同步
  ├─ Window (GLFW)       窗口与 Surface
  ├─ Camera              视图与投影
  ├─ ModelManager        模型实例 = Mesh + Material 绑定
  │    ├─ BufferManager  顶点 / 索引 / 图像 Buffer 与 Mipmap
  │    └─ MaterialManager 材质块 + MaterialViewer 缓存
  └─ GUIManager          ImGui（Vulkan 后端）调试界面
```

资源管理采用 **Model-Instance** 模式：每个 `ModelInstance` 挂载一个 Mesh 与一个 Material；Material 通过 `MaterialViewer` 复用同一纹理的 Sampler / DescriptorSet，材质参数集中写入大块 UBO 并按 Offset 索引。设计思路见 [引擎学习说明 / 资源管理](引擎学习说明/资源管理.md)。

## 构建与运行

**依赖**
- Windows + Visual Studio 2019 / 2022，或其他支持 C++17 的编译器
- Vulkan SDK（提供 Vulkan headers、loader 和 `glslc`）
- GLFW、GLM、stb_image、tinyobjloader
- Dear ImGui 已随仓库提供，不需要另行下载

第三方依赖统一从当前项目下的 `thirdpart/` 载入，不再使用任何机器绝对路径。
依赖目录布局见 [thirdpart/README.md](thirdpart/README.md)。CMake 同时兼容按
问题中拼写的 `thridpart/` 目录。

**步骤**
1. 将第三方库整理到项目下的 `thirdpart/`，目录结构参考
   [thirdpart/README.md](thirdpart/README.md)。
2. 配置：`cmake -S . -B build -G "Visual Studio 17 2022" -A x64`。
3. 编译：`cmake --build build --config Debug`。
4. 运行 `build/Debug/CatRender.exe`（CMake 会自动复制 shaders 和 textures）。

代码默认加载 `CatRenderer/models/viking_room.obj` 与
`CatRenderer/models/african_head.obj`；仓库未包含这些模型，需要自行下载并放置。
如果 `glslc` 可用，CMake 会自动重新编译 `CatRenderer/shaders/Shader.vert/.frag`；
否则会使用仓库现有的 `.spv` 文件。

> 注：本项目为学习 / 科研用途，模型资源和第三方库不随仓库分发。

## 运行截图

（建议补充一张运行截图：渲染 viking_room / african_head 模型，直观展示成果。）

## 当前状态与后续计划

- ✅ 完整 Vulkan 绘制链路、多模型 / 多材质、ImGui 调试界面
- 🚧 TODO（代码内已标记）：动态添加 / 更新材质参数、Descriptor 工厂化、GUI 资源管理面板
- 🔭 计划：引擎架构重构（更清晰的模块边界与场景管理）

## 文档
- [引擎学习说明](引擎学习说明/) —— 资源管理、材质与描述符设计笔记

---

License：暂未指定（默认保留所有权利）。欢迎就图形 / 引擎实现交流；如需用于其他用途请先联系作者。
