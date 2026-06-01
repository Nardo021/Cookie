# Synthetic Menu — P0 / P1 状态

## P0 ✅

框架进工程、Present 接线、7 Tab 导航、Config 真 JSON（Save/Load/Delete/列表删除）。

## P1 ✅

所有 Tab 绑定 **真实** `config`（非 `var->c_*` 演示字段）。

| Tab | 文件                       | 绑定模块                                                                                |
| --- | -------------------------- | --------------------------------------------------------------------------------------- |
| R   | `SyntheticTabRage.cpp`     | `Ragebot`, `HitboxData`, `LagComp`, `NoSpread`, `WeaponConfig`                          |
| L   | `SyntheticTabLegit.cpp`    | `Aimbot`, `Triggerbot`, `LegitBot`, `WeaponConfig`                                      |
| A   | `SyntheticTabMovement.cpp` | `AntiAim`, `Bhop`, `Movement`                                                           |
| V   | `SyntheticTabVisuals.cpp`  | `ESP`, `EspOverlay`, `Chams`, `WorldVisuals`, `WorldFov`, `BulletTracer`, `ThirdPerson` |
| S   | `SyntheticTabSkins.cpp`    | `Gloves`, `SkinChanger`（全武器表 `SyntheticSkinWeapons.hpp`）                          |
| C   | `SyntheticConfig.cpp`      | `CSettingsJson` + `MenuConfig`                                                          |
| M   | `SyntheticTabMisc.cpp`     | `PlantBomb`, `MenuEffects`, `MenuSettings`, DPI                                         |

入口：`gui_cookie.cc` → `switch (selection_active)`。

共享：`SyntheticTabCommon.hpp`。

## 验证

1. Release x64 编译 `Cookie.sln`
2. 各 Tab 改选项 → Config Save → Load → 恢复
3. Rage 开时 Legit Tab 显示只读提示
4. Skins：Force Update / Clear All / 手套 Apply

## P2 ✅

体验补齐见 **[P2-STATUS.md](P2-STATUS.md)**（水印、菜单特效、ESP 预览、Skins 完整 UI、Custom Texture）。

## P3 ✅

框架打磨见 **[P3-STATUS.md](P3-STATUS.md)**（拖拽 ESP 预览、Shader 模糊、DPI/通知 JSON、去演示编译）。

## 参考

- `CookieMenu.hpp` — 已不再作运行时入口，仅对照用
