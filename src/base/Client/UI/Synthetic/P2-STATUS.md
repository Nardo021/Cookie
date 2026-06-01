# Synthetic Menu — P2 状态

体验与旧 `CookieMenu.hpp` 的功能对齐（非 P1 的 config 绑定）。

## P2 ✅

| 项 | 实现 |
| --- | --- |
| 游戏内 HUD（地图名 / FPS / Ping / 时间） | `SyntheticWatermark::RenderOverlay()` — 菜单开/关均显示；可拖动，位置写入 config |
| HUD 绘制 | 委托 `c_gui::water_mark`（`base_elements_patched.cpp`） |
| Misc HUD 开关 | `SyntheticTabMisc.cpp`：HUD、默认角落、通知位置 |
| Visuals ESP 预览 | `SyntheticTabVisuals.cpp` → `SyntheticEspPreview` |
| Skins Paint Kit 搜索 + 稀有度 | `SyntheticTabSkins.cpp` |
| Randomize All / Clear All | Skins Tab |
| Custom Texture | Skins Tab 子面板 |

## 已移除（Synthetic 菜单）

- 菜单内全屏背景 / 内容区粒子 / 模糊（`gui_cookie.cc` 不再调用 `MenuEffects` 背景 API）
- Misc 中已无上述 dead 开关（仅 CookieMenu 路径仍可用 `MenuEffects::config`）

## 工程

- `BaseSources.props`：`SyntheticWatermark.cpp`、`begin_patched.cpp`

## 验证（Windows Release x64）

1. 进局 → 右上角 HUD 显示地图名、FPS、Ping、时间
2. 拖动 HUD → 保存 config → 重载后位置保持
3. INSERT 开菜单 → 无全屏模糊/粒子，仅 Synthetic 面板 + 侧栏分类
4. Misc → 改 HUD 默认角落 → 清除自定义坐标，回到预设角

## 与旧菜单差异（可接受）

- 关闭菜单时不再在 `gui_cookie` 内重复画 HUD（统一 `CCookieClient`）
- 未初始化 Synthetic 时回退 `MenuEffects::RenderWatermark`（需在别处启用 legacy 配置）
