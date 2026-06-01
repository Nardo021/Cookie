# SyntheticCompat

ImGui 1.91 兼容实现，供 CookieDll / MenuSandbox 编译，不修改 `framework/` 上游源文件。

## 命名约定

| 类型 | 规则 | 示例 |
|------|------|------|
| 主头文件 | `Synthetic*.hpp` | `SyntheticFramework.hpp`、`SyntheticVariables.hpp` |
| 替换实现 | `*_patched.cpp` | `begin_patched.cpp`、`base_elements_patched.cpp`（工程唯一链接） |
| UI 类型 | `SyntheticUiTypes.hpp` | `watermark_layout`、`watermark_position` |
| 桩 / 裁剪 | `*_stub.cpp` | `textfield_stub.cpp`、`lua_widgets_stub.cpp` |
| 菜单输入 | `CCookieGUI::OnReopenGUI` | 相对鼠标 / SDL 光标（内联实现） |

## 包含路径

- 工程优先 `include_shim/`，将 `#include <framework/settings/functions.h>` 重定向到 `SyntheticFramework.hpp`。
- 勿再使用 `functions_patched.h.new`、`*_v2.cpp`、`.bak` 等临时文件名。
- `water_mark` 仅 `base_elements_patched.cpp`；勿在 `framework/functional/base_elements.cpp` 恢复。

## 着色器

菜单模糊见 `framework/shader/blur.hpp`（Cookie 构建使用此文件，非 `blur_patched.hpp`）。
