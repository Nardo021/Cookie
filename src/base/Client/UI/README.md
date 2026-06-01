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
    └── framework/        Synthetic 控件库（含 lua.cpp / text_editor.cpp，未链接）
```

## Lua（Tab U）

- 运行时：`SyntheticLuaRuntime`（vcpkg `lua`），脚本目录 `<dll_dir>/lua/*.lua`
- UI：`SyntheticTabLua.cpp` + `text_editor` / `lua` 控件
- 配置：`Menu.activeLuaScript` 记住上次打开的脚本名

## TODO

- [ ] 扩展 `cookie.*` Lua API（见根 [README.md](../../../../README.md)）
