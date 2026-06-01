# UI 目录结构

```
UI/
├── Menu/                 共享：配置、特效、资产
│   ├── CookieMenu.hpp    旧菜单（对照用，非运行时）
│   ├── MenuConfig.*
│   ├── MenuAssets.*
│   ├── MenuEffects.*
│   └── ...
└── Synthetic/            运行时菜单（gui_cookie.cc + SyntheticTab*）
    ├── SyntheticCompat/  ImGui 1.91 补丁实现（工程链接 *_patched.cpp）
    ├── include_shim/     头文件重定向
    └── framework/        上游控件源（对照；生产用 SyntheticCompat）
```

## Lua（Tab U）

- 运行时：`SyntheticLuaRuntime`（vcpkg `lua`），脚本目录 `<dll_dir>/lua/*.lua`
- UI：`SyntheticTabLua.cpp` + `text_editor` / `lua` 控件
- 配置：`Menu.activeLuaScript` 记住上次打开的脚本名

## TODO

- [ ] 扩展 `cookie.*` Lua API（见根 [README.md](../../../../README.md)）
