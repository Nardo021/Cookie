# Cookie

CS2 internal 工具（x64）。编译后在 **`bin\Release\`** 得到 **`Cookie.exe`**（内嵌 payload，运行时不需要同目录 `Cookie.dll`）。

## 要求

- Windows 10/11 x64
- Visual Studio 2022 / 2026 + **MSVC v145** + Windows SDK
- vcpkg manifest（**protobuf** 3.21.12、**lua**）
- [BlackBone](https://github.com/DarthTon/BlackBone) 源码 → `external/BlackBone`

## 首次准备

```bat
cd /d path\to\Cookie
git clone https://github.com/DarthTon/BlackBone external/BlackBone
vcpkg install --triplet x64-windows-static
```

## 编译

```bat
msbuild Cookie.sln /p:Configuration=Release /p:Platform=x64 /m
```

或在 Visual Studio 中打开 `Cookie.sln`，选 **Release | x64** 生成解决方案。

构建顺序：`BlackBoneLib` → `CookieDll` → `Cookie`（由解决方案依赖自动处理）。可选目标 **`MenuSandbox`**（独立菜单沙盒，见下方）。

### 产物

| 文件 | 位置 | 说明 |
|------|------|------|
| **Cookie.exe** | `bin\Release\` | 唯一需要运行的文件 |
| Cookie.dll | `obj\Release\` | 中间产物，已嵌入 exe |
| MenuSandbox.exe | `bin\MenuSandbox\Release\` | 可选：无 CS2 的 Synthetic UI 沙盒 |
| BlackBone.lib | `src\launcher\lib\` | 由 BlackBoneLib 自动生成 |

### 可忽略的目录

若存在仓库根下的 `x64\Release\`、`Cookie\x64\Release\` 或根目录 `Cookie.exe`，那是旧输出路径遗留，可删除后重新编译。

## 运行

以**管理员**运行 **`bin\Release\Cookie.exe`**，会打开 Cookie Injector 窗口和独立的 **Log** 窗口：

1. 先启动 CS2（可选：点击 **Patch VAC3** 处理 VAC）
2. 确认 CS2 状态为 **[RUNNING]**
3. 点击 **Inject** 注入内嵌的 `Cookie.dll`（释放到临时目录后 LoadLibrary，再调用 `CookieBootstrap` 初始化）

| 按键 | 作用 |
|------|------|
| INSERT | 菜单 |
| END | 卸载 |

## 工程结构

```
Cookie.sln              解决方案（Cookie + CookieDll + BlackBoneLib + MenuSandbox）
Cookie.vcxproj          启动器 exe，内嵌 dll 资源
CookieDll.vcxproj       内部 payload dll
BlackBoneLib.vcxproj    编译 BlackBone 静态库
bin/                    最终可执行文件（Release / Debug）
src/launcher/           注入 UI、injector、ImGui、BlackBone 链接
src/base/               SDK、Hook、功能、Synthetic 菜单
tools/MenuSandbox/      独立 DX11 菜单沙盒（不注入 DLL）
src/base/Client/Resources/  字体、SVG、内嵌资产（构建时复制到 OutDir）
external/BlackBone/     BlackBone 源码（git clone）
obj/                    dll 与编译中间文件
sdk/                    cs2-dumper 偏移
patterns/               签名
```

**入口链**：`Cookie.exe` → LoadLibrary → `CookieBootstrap` → `CCookieClient`

功能与模块说明见 **[Features.md](./Features.md)**。

## 资源资产

构建时 `CookieDll` 会将 `src/base/Client/Resources/` 整目录复制到输出目录的 `Resources\`（PostBuild `xcopy`），供运行时加载字体与图标：

| 类型 | 路径 | 用途 |
|------|------|------|
| 字体 | `CS2GunIcons.ttf`、`iconscs2.ttf`、`obs_icons.ttf` | ESP 武器图标 |
| SVG | `icons/*.svg` | 参考图标集（73 个） |
| 品牌 | `branding/cs2.png` / `.webp` | 水印 / 品牌图 |
| 内嵌 | `Resources/embedded/` | FA 字体、game_icons、Bgs 等编译进 DLL 的 fallback |

详细清单与集成点见 `src/base/Client/Resources/README.md`。

## 游戏更新后

1. `sdk/offsets.hpp`、`sdk/buttons.hpp`
2. `src/base/Client/Game/Offsets.hpp`
3. `patterns/pattern.txt` → `src/base/Client/Game/Patterns.hpp`
4. `src/base/CS2/SDK/Update/Offsets.hpp`、FunctionList 签名（若失效）
5. `SpreadHooks` / `GetSpread` / `GetInaccuracy` 签名（NoSpread 零散布 detour）
6. 验证 `Resources\` 是否随构建正常复制（字体缺失时走内嵌 fallback）

## 菜单（Synthetic）

游戏内运行时菜单在 **`src/base/Client/UI/Synthetic/`**（`gui_cookie.cc` + `SyntheticTab*`），旧版 ImGui 菜单 `CookieMenu.hpp` 仅作对照，不参与注入。

| Tab | 图标 | 内容 |
|-----|------|------|
| Rage | R | Ragebot、Hitbox API、激活键绑定 |
| Legit | L | Aimbot / Triggerbot（`checkbox_with_key`） |
| Anti-Aim | A | Anti-Aim、Movement 高级 |
| Visuals | V | ESP、Overlay、Chams、世界/相机、**ESP 布局预览**（可拖拽） |
| Skins | S | 手套、PaintKit、自定义纹理 |
| Config | C | JSON 配置 Save / Load / Delete / Create / 排序 |
| Lua | U | 脚本列表、编辑器、运行（`<dll_dir>/lua/*.lua`） |
| Misc | M | 菜单特效、DPI、水印/通知位置 |

| 入口 | 说明 |
|------|------|
| 游戏内 | `CookieDll` → `SyntheticMenu` + `gui_cookie.cc` |
| 独立沙盒 | **`MenuSandbox`** — [tools/MenuSandbox/README.md](tools/MenuSandbox/README.md)（演示 UI，无 Lua 运行时） |
| UI 结构 | [src/base/Client/UI/README.md](src/base/Client/UI/README.md) |
| framework 同步 | [src/base/Client/UI/Synthetic/FRAMEWORK-SYNC.md](src/base/Client/UI/Synthetic/FRAMEWORK-SYNC.md) |

**Lua（vcpkg `lua`）**：`SyntheticLuaRuntime` 提供 `cookie.print`、`set_aimbot`、`set_rage`、`set_esp`；配置项 `Menu.activeLuaScript` 记住上次脚本。

## TODO

- [ ] **Lua API 扩展**：Trigger、Movement、配置读写等更多 `cookie.*` 绑定。
