# Synthetic Menu — P3 状态

框架打磨：交互式 ESP 预览、DPI/模糊/通知持久化，移除演示编译项。

## P3 ✅

| 项 | 实现 |
|----|------|
| 交互式 `esp_preview` | `SyntheticEspPreview.cpp` → 拖拽 NAME/DISTANCE/FLAGS/HEALTH/AMMO，写回 `EspOverlay::config.*Side` |
| DPI 持久化 | `MenuSettings::menuDpiPercent` + Config **Menu** 段；`SyntheticMenu::ApplyPersistedUiSettings()` |
| 面板暗层 | `MenuEffects::DrawPanelBlurOverlay`；`blur.hpp` 委托同一实现（Synthetic 菜单默认不调用） |
| CNotify 桥接 | `SyntheticNotifyBridge`：Config 操作同时 `GetNotify` + 菜单内 Synthetic toast |
| 移除演示编译 | `BaseSources.props` 去掉 `lua.cpp`、`text_editor.cpp`；`framework/gui.cc` 保持 `#if 0` |

## 工程

- `SyntheticEspPreview.cpp`
- `SyntheticNotifyBridge.cpp`

## 验证（Windows Release x64）

1. Visuals → **ESP Layout Preview**：拖拽标签到四边 → Save Config → Load → 布局恢复
2. CookieMenu 路径仍可用 `MenuEffects` 模糊/粒子；Synthetic 菜单无背景层（见 P2-STATUS）
3. Misc → DPI 150% → Save/Load → 菜单缩放恢复
4. Config Save/Load → 游戏内 CNotify + 菜单 Synthetic 通知
5. 编译无 `lua.cpp` / `text_editor.cpp` 链接

## 与 P2 关系

- P2 静态 `MenuAssets` 预览 → P3 改为 Synthetic 框架 **可拖拽** 预览（与 Overlay 布局联动）
