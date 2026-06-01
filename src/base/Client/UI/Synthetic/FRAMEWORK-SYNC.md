# Synthetic framework 维护说明

运行时菜单：`gui_cookie.cc` + `SyntheticTab*`。  
演示菜单副本：`framework/gui.cc`（`#if 0`，仅供对照）与 `tools/MenuSandbox/gui_demo.cc`（沙盒编译用）。

## `begin_patched.cpp` 体量（勿再堆逻辑）

文件仍为整份 ImGui `begin` 分叉（约 1.5k 行）。**禁止**往此文件追加新功能；仅允许：

- 调用 `SyntheticScrollPatch.hpp` 等已外提模块
- 与上游 `framework/functional/begin.cpp` 同步缺陷修复

新窗口行为请外提头文件/`.cpp`，或更新本说明中的 patch checklist。

## 滚动逻辑（必须同步）

生产：`SyntheticCompat/begin_patched.cpp`  
沙盒 / 对照：`framework/functional/begin.cpp`

二者均调用 **`SyntheticCompat/SyntheticScrollPatch.hpp`** 中的 `SyntheticScroll::ApplyAnimatedWindowScroll`。  
修改滚动行为时只改该头文件，然后验证 CookieDll 与 MenuSandbox。

## 水印 / HUD

- 类型定义：`SyntheticCompat/SyntheticUiTypes.hpp`（`watermark_position`、`watermark_layout`）
- 绘制实现：`SyntheticCompat/base_elements_patched.cpp` → `c_gui::water_mark(..., watermark_layout*)`（勿在 `framework/functional/base_elements.cpp` 恢复）
- 游戏 HUD：`SyntheticWatermark::Render()` → 委托 `water_mark`；入口 `SyntheticWatermark::RenderOverlay()`（`CCookieClient`）
- 状态：`var->c_watermark`（`use_custom_position` / `custom_position`）；持久化经 `MenuSettings` ↔ `SyntheticMenu::ApplyPersistedUiSettings` / `SyncUiSettingsToMenu`

## 与外部 Synthetic 上游

若从上游 Imgui Menu Synthetic 获取更新，对比：

```bat
diff -rq <upstream>\framework src\base\Client\UI\Synthetic\framework
```

**勿覆盖**（Cookie 定制）：

| 文件 | 原因 |
|------|------|
| `functional/config.cpp` | 已接 `CSettingsJson` + `SyntheticConfig` |
| `functional/keybind.cpp` | 含 `key_name_from_vk()` |

其余 `functional/*.cpp` 可按需 cherry-pick；`begin.cpp` 滚动段勿单独改，改 `SyntheticScrollPatch.hpp`。

## 独立 UI 调试

使用 **`MenuSandbox`**（`tools/MenuSandbox/`），不依赖 CS2 / `CookieDll`。
