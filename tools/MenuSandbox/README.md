# MenuSandbox

独立 **Win32 + DX11** 程序，用于在不启动 CS2 / 不注入 `CookieDll` 的情况下调试 Synthetic 演示菜单。

主文档：[../../README.md#菜单沙盒-menusandbox](../../README.md#菜单沙盒-menusandbox)

## 编译

在 Visual Studio 中打开 `Cookie.sln`，选择 **MenuSandbox | Release | x64** 生成。

或：

```bat
msbuild Cookie.sln /p:Configuration=Release /p:Platform=x64 /t:MenuSandbox
```

仅本工程：

```bat
msbuild tools\MenuSandbox\MenuSandbox.vcxproj /p:Configuration=Release /p:Platform=x64 /m:1
```

产物：`bin\MenuSandbox\Release\MenuSandbox.exe`

## 运行

直接运行 **`MenuSandbox.exe`**（无需管理员、无需 CS2）。窗口标题为 *Cookie Menu Sandbox (Synthetic Demo)*。

## 源码说明

| 文件 | 作用 |
|------|------|
| `main.cpp` | 窗口、D3D11 设备、ImGui 初始化、字体/背景纹理加载 |
| `gui_demo.cc` | 完整 Synthetic 演示 UI（含 Config / Lua 等 Tab 壳） |
| `config_demo.cpp` | 演示用配置列表（`config_selectable`），不依赖 `CSettingsJson` |

控件实现与 `CookieDll` 相同，编译 **`SyntheticCompat/*_patched.cpp`** 与 stub（`textfield_stub`、`lua_widgets_stub`、`text_editor_link_stub` 等），通过 `include_shim` 对齐 ImGui 1.91。纹理加载使用 `SyntheticCompat/d3dx11tex.h`（WIC，无需 legacy D3DX SDK）。

游戏内菜单为 `gui_cookie.cc` + `SyntheticTab*`，与此 exe **分离**；对照用演示副本见 `src/base/Client/UI/Synthetic/framework/gui.cc`（`#if 0`）。

## 要求

- 与主工程相同：VS 2022/2026、**MSVC v145**、Windows SDK。
- 不需要 vcpkg **lua**（沙盒不链接 Lua 运行时）。
