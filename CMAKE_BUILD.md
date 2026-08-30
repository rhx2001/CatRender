# CMake 构建说明

本分支已经不依赖源码目录中的 Visual Studio `.sln` 或 `.vcxproj` 文件。第三方依赖从项目内的 `thirdpart/` 目录加载，所有路径均相对于当前项目。

在项目根目录执行：

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug --parallel 2
```

可执行文件位于 `build/Debug/CatRender.exe`。构建后会自动复制 shaders、textures、models，以及 `thirdpart/` 中找到的 Vulkan 运行库。

如果需要自动重新编译 GLSL，保留 `thirdpart/bin/glslc.exe`；也可以使用已有的 `CatRenderer/shaders/*.spv` 文件。
