# Cookie ↔ 参考/cstrike 50 项互相对照

> 对照基准：`参考/cstrike/`（MIT）。状态随 Cookie 源码更新；最后修订：v1.4+（含 Hitbox V1/V2、七档武器、EntCache 生命周期回调）。

## 图例

| 标记 | 含义                                    |
| ---- | --------------------------------------- |
| ✅   | Cookie 已对齐或优于参考                 |
| 🟡   | 部分实现 / 行为接近但细节不同           |
| ❌   | 参考有明确能力，Cookie 仍缺或仅 UI 占位 |
| ⚪   | 参考也为 WIP、注释或空壳，低优先级      |

---

## 1. 菜单 / UX（`core/menu.cpp`、`post_processing.*`）

| #   | 参考能力                                       | Cookie | 说明                           |
| --- | ---------------------------------------------- | ------ | ------------------------------ |
| 1   | `edited::` 自定义控件                          | ❌     | Cookie 使用原生 ImGui + 主题   |
| 2   | Hitbox pointbox 人体点选                       | ❌     | Rage/Legit 仍为 Checkbox 列表  |
| 3   | 菜单 DPI 缩放 125%–200%                        | ❌     | 无 `CalculateDPI`              |
| 4   | 主题色 primtv/accent 多档                      | 🟡     | 金色 accent；无完整主题变量    |
| 5   | 颜色选择器彩虹/复制/粘贴                       | ❌     |                                |
| 6   | D3D11 Gaussian Blur                            | 🟡     | `MenuEffects::blurPlaceholder` |
| 7   | Rage / Antiaim / Visuals / Skins / Misc 分 Tab | 🟡     | Cookie 4 Tab；Rage+Legit 同页  |
| 8   | `multidrop.h` 多选下拉                         | ⚪     | 参考侧实验代码                 |

**Cookie 领先：** ESP 预览、Config JSON Tab、粒子背景、内嵌字体/图标。

---

## 2. 配置基础设施（`core/config.h`、`variables.h`）

| #   | 参考能力                                    | Cookie | 说明                                       |
| --- | ------------------------------------------- | ------ | ------------------------------------------ |
| 9   | `C::` 反射式配置 + FNV1A 序列化             | 🟡     | `MenuConfig` + `ConfigSchema` v2；手写维护 |
| 10  | `TextOverlayVar_t` / `BarOverlayVar_t` 结构 | 🟡     | `EspOverlay` 组件化，配置扁平              |
| 11  | Rage **7 档武器**                           | ✅     | `WeaponConfig::WeaponClass` 七档 + 迁移    |
| 12  | 每档 Rage Key / Trigger / FOV 圈            | 🟡     | 每档 `aimKey`/`trigger`；Rage FOV 圈待补   |
| 13  | `KeyBind_t` Hold/Toggle 完整序列化          | 🟡     | `KeyBindWidget` + 部分 JSON                |

---

## 3. Rage / LagComp / 战斗精度

| #   | 参考能力                            | Cookie | 说明                                           |
| --- | ----------------------------------- | ------ | ---------------------------------------------- |
| 14  | `backtrack_entity` 模型 hitbox 录帧 | ✅     | `HitboxData` V1/V2 + `LagRecord` hitbox 缓存   |
| 15  | `EntCache::RegisterCallback`        | ✅     | `CEntityCache::RegisterLifeCallback` + LagComp |
| 16  | LagComp Render 按 parent 画骨架     | 🟡     | `RenderDebug` 线框；可继续细化                 |
| 17  | 双 record fraction 插值             | ✅     | `BacktrackSelection` + tick 插值               |
| 18  | `HitchanceFast`                     | 🟡     | `RageScan` 散布模拟；未单独 fast 路径          |
| 19  | `AjustRecoil` Rage 压枪             | 🟡     | Legit RCS；Rage 偏 scan                        |
| 20  | Subtick `teleport`                  | ⚪     | 参考亦为空壳                                   |
| 21  | `ScaleDamage2` 精细护甲缩放         | 🟡     | `AutoWall::ScaleDamage` + ConVar               |
| 22  | Rage no scope / per-weapon vis      | 🟡     | `WeaponConfig` 字段；UI 可再暴露               |

**Cookie 领先：** `RageSubTick`、Hitscan 枚举、`NetworkClientService` 刷新、Hitbox API 切换。

---

## 4. Legit（`features/legit/`）

| #   | 参考能力                        | Cookie | 说明                                      |
| --- | ------------------------------- | ------ | ----------------------------------------- |
| 23  | `CalculateHitboxData` capsule   | ✅     | `HitboxData` V2 native + V1 fallback      |
| 24  | 每武器类独立 Aim Key            | ✅     | `WeaponConfig::LegitSettings::aimKey`     |
| 25  | 左右腿分开 hitbox               | 🟡     | 合并 `hitboxLegs`；扫描仍分 Left/RightLeg |
| 26  | Legit no scope                  | ✅     | `LegitBot::config.noScope`                |
| 27  | Per-weapon trigger hitchance UI | 🟡     | 字段在 `WeaponConfig`；菜单已部分接入     |

---

## 5. 视觉（`features/visuals/`）

| #   | 参考能力                        | Cookie | 说明                                      |
| --- | ------------------------------- | ------ | ----------------------------------------- |
| 28  | ESP 条渐变 + Glow Shadow        | ❌     | `EspOverlay` 无 glow shadow               |
| 29  | 血条/弹药条独立背景条配置       | 🟡     | 有 Bar；样式项较少                        |
| 30  | Chams Remove teammate / occlude | ✅     | `hideTeammateVanilla` / `skipOccludePass` |
| 31  | 骨架 ESP 独立开关 + 描边        | 🟡     | `Esp.hpp` skeleton                        |
| 32  | Nightmode / colModulate         | ✅     | `WorldVisuals`                            |
| 33  | `full_update` 菜单按钮          | ✅     | Skins 页 Force Update                     |
| 34  | ViewModel FOV 菜单              | ✅     | `g_viewModelFovConfig` + Hook             |
| 35  | 第三人称 No Interp              | ✅     | `ThirdPerson::config.noInterp`            |
| 36  | 动态 VMAT `CreateMaterial`      | 🟡     | Cookie KV3/VMAT 五套材质                  |

**Cookie 领先：** 五材质 Chams、WorldFov、EspOverlay 组件、`GetMatricesForView` bbox 缓存。

---

## 6. 移动 / 杂项

| #   | 参考能力           | Cookie | 说明                                   |
| --- | ------------------ | ------ | -------------------------------------- |
| 37  | `bAntiUntrusted`   | ✅     | `Movement::validateAngles`（菜单标注） |
| 38  | OverlayKey 独立键  | ❌     | 可用 INSERT 菜单键替代                 |
| 39  | Autostrafe 模式 UI | ✅     | Off / Legit / Rage                     |
| 40  | Edge Bug KeyBind   | ✅     |                                        |

**Cookie 领先：** EdgeBug 128 点 Trace、Jump Bug、AA movement fix、grenade guard。

---

## 7. 底层 / Hook / 工程

| #   | 参考能力                     | Cookie | 说明                   |
| --- | ---------------------------- | ------ | ---------------------- |
| 41  | Spoofcall + lazy_importer    | 🟡     | `CSpoofCall`；未全覆盖 |
| 42  | EntCache 事件总线            | ✅     | `RegisterLifeCallback` |
| 43  | 统一 `D::` 绘制层            | 🟡     | `CDraw` + ImGui        |
| 44  | `AnimationHandler_t` UI 动画 | ❌     |                        |
| 45  | `CameraInput` hook           | ⚪     | 参考已注释             |
| 46  | `IDebugOverlay`              | ❌     |                        |
| 47  | ShadowVMT + InlineHook       | 🟡     | MinHook + `CShadowVMT` |

**Cookie 领先：** `FeatureInit::VerifyHooks`、`SpreadHooks`、VAC inhibitor、Hook SkipIfNotFound。

---

## 8. 皮肤 / 库存

| #   | 参考能力                | Cookie         | 说明                    |
| --- | ----------------------- | -------------- | ----------------------- |
| 48  | 内存写皮肤旧路径        | ✅ Cookie 更强 | Inventory + SetModel    |
| 49  | Killfeed 刀名改写       | ✅             | `GameEvents`            |
| 50  | 手套 ViewModel 材质刷新 | ✅             | `Gloves` + magic number |

---

## 汇总（2026-06）

| 类别          | ✅     | 🟡     | ❌    | ⚪    |
| ------------- | ------ | ------ | ----- | ----- |
| 菜单 UX (1–8) | 0      | 2      | 5     | 1     |
| 配置 (9–13)   | 1      | 4      | 0     | 0     |
| 战斗 (14–22)  | 3      | 5      | 0     | 1     |
| Legit (23–27) | 3      | 2      | 0     | 0     |
| 视觉 (28–36)  | 5      | 3      | 1     | 0     |
| 移动 (37–40)  | 3      | 0      | 1     | 0     |
| 底层 (41–47)  | 1      | 3      | 2     | 1     |
| 皮肤 (48–50)  | 3      | 0      | 0     | 0     |
| **合计**      | **19** | **19** | **9** | **3** |

**结论：** 战斗主链、库存皮肤、Hook 工程化已对齐；剩余差距集中在 **菜单控件库（#1–2）、ESP 样式（#28）、反射配置（#9）** 等体验/工程项，非阻塞核心功能。

**关联文档：** [Features.md](./Features.md)（功能清单）、[1.md](./1.md)（迁移清单 1–24）。
