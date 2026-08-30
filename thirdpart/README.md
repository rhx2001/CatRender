# CatRender thirdpart

Vulkan SDK headers require both `include/vulkan/` and the sibling `include/vk_video/` directories.
Only the Vulkan loader library and shader compiler are needed from the SDK; its installer,
documentation, samples, and other tools do not need to be copied here.

将第三方库放到本目录。CMake 只从当前项目下的 `thirdpart/`（或兼容拼写的
`thridpart/`）查找，不依赖任何机器上的绝对路径。推荐把所有头文件和库
整理到下面的目录结构：

```text
thirdpart/
├─ include/
│  ├─ GLFW/glfw3.h
│  ├─ glm/glm.hpp
│  ├─ stb_image/stb_image.h
│  ├─ tinyobjloader/tiny_obj_loader.h
│  └─ vulkan/vulkan.h
├─ lib/
│  ├─ glfw3.lib                    # Windows + MSVC
│  └─ vulkan-1.lib
└─ bin/
   ├─ glfw3.dll                    # 仅使用 GLFW DLL 时需要
   └─ glslc.exe                    # 可选；用于自动编译 GLSL
```

也支持直接解压库的常见目录结构，例如 `glfw/include`、
`glfw/lib-vc2022`、`Vulkan/Include`、`Vulkan/Lib`；但最终仍须位于项目的
`thirdpart/` 目录内。

当前源码已经自带 Dear ImGui（`CatRenderer/Include/Gui`），所以不用重复下载
ImGui。`stb_image` 和 `tinyobjloader` 是单头文件库，但仍然需要把头文件
放到这里；CMake 会为现有源码生成兼容 include，并为 tinyobjloader 编译一次
实现代码。

问题中写作 `thridpart` 也可以使用：将 `thirdpart` 改名为 `thridpart`，
CMake 会自动识别。
