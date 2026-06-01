# Synthetic framework 维护说明

运行时菜单：`gui_cookie.cc` + `SyntheticTab*`。  
演示菜单副本：`framework/gui.cc`（`#if 0`，仅供对照）与 `tools/MenuSandbox/gui_demo.cc`（沙盒编译用）。

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

其余 `functional/*.cpp` 可按需 cherry-pick。

## 独立 UI 调试

使用 **`MenuSandbox`**（`tools/MenuSandbox/`），不依赖 CS2 / `CookieDll`。
