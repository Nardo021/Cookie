# MenuSandbox

独立 **Win32 + DX11** 程序，用于在不启动 CS2 / 不注入 `CookieDll` 的情况下调试 Synthetic 演示菜单。

## 编译

在 Visual Studio 中打开 `Cookie.sln`，选择 **MenuSandbox | Release | x64** 生成。

或：

```bat
msbuild Cookie.sln /p:Configuration=Release /p:Platform=x64 /t:MenuSandbox
```

产物：`bin\MenuSandbox\Release\MenuSandbox.exe`

## 说明

- `gui_demo.cc` — 自 `Synthetic/framework/gui.cc` 演示块 vendored，完整 Synthetic 演示 UI（含 Lua 编辑器壳，无脚本运行时）。
- `config_demo.cpp` — 演示用配置列表，不依赖 `CSettingsJson`。
- 控件来自 `src/base/Client/UI/Synthetic/framework/functional/`（**不含** `config.cpp`，避免与 DLL 重复符号）。
- 游戏内菜单为 `gui_cookie.cc` + `SyntheticTab*`，与此 exe 分离。

## 要求

- 与主工程相同：VS 2022/2026、v145、Windows SDK。
- legacy **D3DX11**（`d3dx11.lib`）。
