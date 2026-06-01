# Synthetic Menu — P2 状态

体验与旧 `CookieMenu.hpp` 的功能对齐（非 P1 的 config 绑定）。

## P2 ✅

| 项                            | 实现                                                                                 |
| ----------------------------- | ------------------------------------------------------------------------------------ |
| 实时水印（FPS + 日期）        | `SyntheticWatermark::Render()` — 菜单内 `gui_cookie.cc`，关菜单 `CCookieClient`      |
| 菜单粒子 / 背景图 / 模糊占位  | `gui_cookie.cc` 内容区调用 `MenuEffects::RenderMenuBackground`（带面板 origin 偏移） |
| Misc 特效开关                 | `SyntheticTabMisc.cpp`：Synthetic 水印、位置、粒子、背景图 Alpha、模糊               |
| Visuals ESP 预览              | `SyntheticTabVisuals.cpp` → `MenuAssets::RenderEspPreviewPanel`                      |
| Skins Paint Kit 搜索 + 稀有度 | `SyntheticTabSkins.cpp` → `g_PaintKits` 过滤 + Combo                                 |
| Randomize All / Clear All     | Skins Tab 按钮                                                                       |
| Custom Texture                | Skins Tab 独立子面板：Load、筛选、Start/Stop Browse、Prev/Next/Lock、Unlock、Reset   |
| 菜单关闭 Synthetic 水印       | `CCookieClient`：`var->c_watermark` 时 `SyntheticWatermark::Render()`                |

## 工程

- `BaseSources.props`：`SyntheticWatermark.cpp`

## 验证（Windows Release x64）

1. 打开菜单 → 右侧内容区粒子/背景图与旧菜单一致（仅主面板，不含左侧 Tab 栏）
2. Misc → 开关 Synthetic 水印、改位置 → 四角显示 `CHEAT_NAME` + FPS + 日期
3. Visuals → ESP 预览图（需 `MenuAssets` 纹理已加载）
4. Skins → 搜索/稀有度筛选 Paint Kit → Randomize All → Force Update
5. Custom Texture → Load Image → Browse（F10/F11/F12 与旧菜单相同）

## 与旧菜单差异（可接受）

- Paint Kit 同时保留 **Combo** 与 **ID 滑条**（旧菜单为 InputInt）
- 关闭菜单：优先 `Synthetic Watermark`（`var->c_watermark`），否则回退 `MenuEffects::config.watermark`
- 静态 `MenuAssets` ESP 图 → P3 已改为可拖拽 **ESP Layout Preview**（见 [P3-STATUS.md](P3-STATUS.md)）
