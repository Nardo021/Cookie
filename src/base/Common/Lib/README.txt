Cookie 链接依赖说明
====================

已通过 vcpkg manifest 自动获取：
- libprotobuf 3.21.12（`vcpkg.json`）

已内置源码编译：
- MinHook（`minhook/`）
- ImGui（`src/base/Common/Include/ImGui`）

已改为 ImGui 实现（无需外部 .lib）：
- FreeType / imgui_freetype
- FW1FontWrapper

已禁用（无需 VMProtectSDK64.lib）：
- VMProtect（Config.hpp → DISABLE_VMPROTECT 1）
