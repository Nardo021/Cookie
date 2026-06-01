# Cookie — 完整功能清单

> CS2 内部客户端（Cookie v1.4+）。本文档按模块列出当前已实现的功能、配置项、依赖 Hook 与调用关系，便于查阅与维护。

---

## 目录

1. [架构概览](#架构概览)
2. [菜单与配置](#菜单与配置)
3. [战斗 — Ragebot](#战斗--ragebot)
4. [战斗 — Legit Aimbot / Triggerbot](#战斗--legit-aimbot--triggerbot)
5. [战斗 — WeaponConfig（按武器配置）](#战斗--weaponconfig按武器配置)
6. [战斗 — RageSubTick（Silent / RapidFire）](#战斗--ragesubticksilent--rapidfire)
7. [穿墙与伤害 — AutoWall](#穿墙与伤害--autowall)
8. [散布补偿 — NoSpread & SpreadHooks](#散布补偿--nospread--spreadhooks)
9. [延迟补偿 — LagComp](#延迟补偿--lagcomp)
10. [反瞄准 — AntiAim](#反瞄准--antiaim)
11. [移动 — Bhop & Movement](#移动--bhop--movement)
12. [引擎预测 — EnginePred](#引擎预测--enginepred)
13. [视觉 — ESP](#视觉--esp)
14. [视觉 — ESP Overlay 组件系统](#视觉--esp-overlay-组件系统)
15. [视觉 — Chams](#视觉--chams)
16. [视觉 — 子弹轨迹 / 第三人称 / World FOV / 菜单特效](#视觉--子弹轨迹--第三人称--world-fov--菜单特效)
17. [换肤 — SkinChanger（库存注入）](#换肤--skinchanger库存注入)
18. [换肤 — 手套 Gloves](#换肤--手套-gloves)
19. [换肤 — 自定义纹理 TextureOverride](#换肤--自定义纹理-textureoverride)
20. [杂项 — Plant Bomb Anywhere](#杂项--plant-bomb-anywhere)
21. [事件 — GameEvents](#事件--gameevents)
22. [资源资产 — Resources / 字体](#资源资产--resources--字体)
23. [底层基础设施](#底层基础设施)
24. [Hook 清单](#hook-清单)
25. [模块互斥与优先级](#模块互斥与优先级)

---

## 架构概览

### 生命周期

| 阶段         | 入口                                            | 说明                                                                                            |
| ------------ | ----------------------------------------------- | ----------------------------------------------------------------------------------------------- |
| 初始化       | `CCookieClient::OnInit`                         | 加载 client/engine、Trace、ConVar、SigScan、SpreadHooks detour、Chams、RageSubTick 种子表、配置 |
| 每帧渲染     | `CCookieClient::OnRender`                       | SkinChanger（库存路径跳过 fallback tick）、菜单、EspOverlay、FOV 圈、Watermark、子弹轨迹        |
| 每 tick 输入 | `CCookieClient::OnCreateMove`                   | **NetworkFullUpdate**（皮肤）、战斗 → AntiAim → Movement → PlantBomb                            |
| 帧阶段       | `Hook_FrameStageNotify`                         | 手套 + **SkinChangerInventory**（stage 6）、PlantBomb、LagComp 录制（stage 3）                  |
| 游戏事件     | `Hook_HandleGameEvents` / `FireEventClientSide` | round_start、player_death → GameEvents                                                          |
| Subtick      | `Hook_InputParser`                              | Silent / RapidFire 写 input_history                                                             |
| 卸载         | `CCookieClient::OnShutdown`                     | Chams、ThirdPerson、Gloves、**SkinChangerInventory**、AntiAim Hook 等                           |

### CreateMove 调用顺序

```
EnginePred::Start（前：SkinChangerInventory::ProcessNetworkFullUpdate）
  → ThirdPerson::ApplyInput
  → Ragebot::OnCreateMove  OR  LegitBot::OnCreateMove（二选一）
  → AntiAim::Process（战斗未占用视角时）
  → Movement::Process（EdgeBug → AutoStrafe → Bhop → MovementFix → input_history）
  → PlantBomb::Process
  → AntiAim::FixMovementPost / CancelMovementFix
EnginePred::End
```

### 核心文件位置

| 类别               | 路径                                                                    |
| ------------------ | ----------------------------------------------------------------------- |
| 功能模块           | `src/base/Client/Features/`（见 `Features/README.md`）                  |
| 皮肤库存           | `Inventory/SkinChangerInventory.*`、`Gloves.*`、`SkinChangerSetModel.*` |
| 按武器配置         | `Combat/Legit/WeaponConfig.*`                                           |
| Subtick 战斗       | `Combat/Rage/RageSubTick.*`                                             |
| 资源 / 字体        | `src/base/Client/Resources/`、`Fonts/EmbeddedFonts.*`                   |
| 运行时菜单 UI      | `src/base/Client/UI/Synthetic/gui_cookie.cc`、`SyntheticTab*`         |
| 菜单共享层         | `src/base/Client/UI/Menu/MenuConfig.cpp`、`MenuEffects.*`               |
| 旧菜单（对照）     | `src/base/Client/UI/Menu/CookieMenu.hpp`                                |
| Lua 运行时         | `src/base/Client/UI/Synthetic/SyntheticLuaRuntime.*`                    |
| Hook               | `src/base/CS2/Hook/`                                                    |
| 输入绕过 / Subtick | `src/base/GameClient/CL_Bypass.hpp`                                     |
| 客户端入口         | `src/base/Client/CCookieClient.cpp`                                     |

---

## 菜单与配置

### Synthetic 菜单（运行时）

**入口：** `gui_cookie.cc` → `SyntheticTabs::*` / `SyntheticConfig::RenderConfigTab`  
**初始化：** `SyntheticMenu::Init`（字体/纹理/DPI/Lua）  
**水印：** `SyntheticWatermark`（菜单开/关均可绘制）

| Tab | 图标 | 主要模块 |
| --- | ---- | -------- |
| **Rage** | R | Ragebot、Hitbox V1/V2、激活键（Hold/Toggle + Active Binds） |
| **Legit** | L | Aimbot、Triggerbot（合并按键 UI） |
| **Anti-Aim** | A | AntiAim、Movement（EdgeBug / Strafe 等） |
| **Visuals** | V | ESP、EspOverlay、Chams、World/ThirdPerson/FOV、**ESP Layout Preview** |
| **Skins** | S | Gloves、SkinChanger、PaintKit、CustomTexture |
| **Config** | C | JSON Save/Load/Delete、Create 弹窗、列表排序 |
| **Lua** | U | 脚本 CRUD、内嵌编辑器、Run |
| **Misc** | M | MenuEffects、菜单 DPI、Synthetic 水印/通知位置 |

独立调试：**`MenuSandbox.exe`**（`tools/MenuSandbox/`，无 `CookieDll` / 无 Lua VM）。

### 配置持久化

- 格式：JSON（`*.json`，DLL 同目录）
- 序列化：`MenuConfig.cpp`（`CSettingsJson` 读写）
- 覆盖：战斗、视觉、移动、皮肤、**Menu**（`toggleKey`、`dpiPercent`、`syntheticWatermark*`、`activeLuaScript`）、**MenuEffects**（含 `shaderBlur`）、EspOverlay **布局边**（`nameSide` 等）
- 兼容：旧版 Bhop `autoStrafe` → `Movement.strafeMode = Legit`
- Save/Load：`CSettingsJson` 推送 `CNotify`；菜单内部分操作经 `SyntheticNotifyBridge` 同步 Synthetic toast

### 快捷键

| 按键 | 功能 |
| ---- | ---- |
| `INSERT`（默认，可配置） | 开关菜单 |
| `END` | 卸载 DLL |
| `F10` / `F11` / `F12` | 自定义纹理 Browse（上一张 / 下一张 / 锁定） |

### KeyBind 系统

| 组件 | 说明 |
| ---- | ---- |
| `widget->checkbox_with_key` | 功能开关 + 键位 + Hold/Toggle + 「显示于 Active Binds」 |
| `SyntheticBinds` | 每帧收集绑定，右上角 **Active Binds** 浮层 |
| `KeyBindUtils::KeyBindSlot` | Hold/Toggle 与 `CInputSystem` 状态缓存 |
| `KeyBindWidget`（CookieMenu） | 旧菜单 Hold 绑定（对照用） |

战斗键位持久化字段示例：`Ragebot.activationKey*`、`Aimbot.aimKey*`、`Triggerbot.key` / `keyHold` / `showInBinds`。

### Lua 脚本

| 项 | 说明 |
| --- | ---- |
| 依赖 | vcpkg **lua** |
| 目录 | `<dll_dir>/lua/*.lua`（自动创建） |
| 运行时 | `SyntheticLuaRuntime`：`cookie.print`、`set_aimbot`、`set_rage`、`set_esp` |
| UI | `SyntheticTabLua` + framework `text_editor` |
| 配置 | `Menu.activeLuaScript` 记住上次打开的脚本名 |

---

## 战斗 — Ragebot

**文件：** `Ragebot.hpp` / `Ragebot.cpp`，扫描逻辑在 `RageScan.hpp` / `RageScan.cpp`

Rage 启用时自动关闭 Legit Aimbot 与 Triggerbot。

### 功能

| 功能            | 说明                                                             |
| --------------- | ---------------------------------------------------------------- |
| 多 Hitbox 扫描  | Head / Neck / Chest / Pelvis / Stomach / Arms / Legs 可独立开关  |
| Multipoint      | 自适应 multipoint 采样，缩放由 `multipointScale` 控制            |
| Hitchance       | 基于散布模拟的命中率过滤                                         |
| Auto Stop       | 开火前减速；支持 Slow（反向移动）与 Early（硬停）                |
| Early Auto Stop | 扫描阶段提前停步（`earlyAutoStop`）                              |
| Penetration     | 调用 AutoWall 评估穿墙伤害                                       |
| Safe Point      | 要求命中点通过额外安全校验                                       |
| Adaptive Weapon | 按当前武器类型自动调整扫描参数                                   |
| Backtrack       | 结合 LagComp **骨骼 tick 插值**（优先）+ origin 回退             |
| Sticky Target   | 锁定上一帧目标，减少 flick                                       |
| Rapid Fire      | RageSubTick 连发                                                 |
| Delay Aim       | 开火前延迟（ms）                                                 |
| Auto Scope      | 狙击自动开镜                                                     |
| Per-Weapon      | WeaponConfig 按武器类型覆盖 hitbox / hitchance / removeSpread 等 |
| Round Events    | round_start / player_death 重置目标与 tick                       |
| Auto Shoot      | 自动 `+attack`                                                   |
| Silent Aim      | 仅写入 input_history，不改客户端可见视角                         |
| No Spread       | 与 NoSpread 模块联动                                             |
| Target Select   | 最高伤害 / 最低 FOV / 最近距离                                   |
| Hitscan Mode    | Normal / Lethal / Lethal+ / Prefer Lethal                        |
| Scan Mode       | Single Point / Adaptive Multipoint                               |
| Team Check      | 跳过队友                                                         |

### 配置项

| 字段                        | 类型 | 默认     | 说明              |
| --------------------------- | ---- | -------- | ----------------- |
| `enabled`                   | bool | false    | 总开关            |
| `activationKey`             | int  | 0        | 激活键 VK         |
| `activationUseKey`          | bool | false    | 是否要求按键激活  |
| `activationKeyHold`         | bool | true     | Hold / Toggle     |
| `activationShowInBinds`     | bool | true     | Active Binds 面板 |
| `minDamage`                 | int  | 1        | 最小伤害阈值      |
| `hitchance`                 | int  | 50       | 命中率 %          |
| `multipointScale`           | int  | 70       | Multipoint 范围 % |
| `autoStop`                  | bool | true     | 开火前停步        |
| `earlyAutoStop`             | bool | true     | 扫描阶段提前停步  |
| `penetration`               | bool | true     | 穿墙              |
| `safePoint`                 | bool | false    | 安全点            |
| `adaptiveWeapon`            | bool | true     | 武器自适应        |
| `autoShoot`                 | bool | true     | 自动开火          |
| `silentAim`                 | bool | false    | 静默瞄准          |
| `backtrack`                 | bool | true     | 回溯              |
| `teamCheck`                 | bool | true     | 队伍过滤          |
| `hitscanMode`               | enum | Normal   | 命中扫描策略      |
| `scanMode`                  | enum | Adaptive | 单点 / 自适应     |
| `stopMode`                  | enum | Slow     | Slow / Early      |
| `targetSelect`              | enum | Damage   | 目标优先级        |
| `hitboxHead` … `hitboxLegs` | bool | 见源码   | 各部位开关        |

### 依赖

- `AutoWall::CanPenetrate` / `FireBullet`
- `LagComp::GetBacktrackBoneInterpolated` / `GetBacktrackOriginInterpolated`
- `RageSubTick::SetSilentAim` / `SetRapidFire`
- `WeaponConfig::GetRageSettingsForActiveWeapon`
- `Movement::ApplyAutoStop`
- `EnginePred`（Rage 内部二次 Start/End）
- `CL_Bypass::SetViewAngles` / `SetAttack` / `SetDontAttack`

---

## 战斗 — Legit Aimbot / Triggerbot

**文件：** `Aimbot.hpp`（内联实现）、`LegitBot.hpp`（配置同步包装）、`Triggerbot.hpp` / `Triggerbot.cpp`

Rage 未启用时使用 Legit 管线；`LegitBot::SyncToAimbot` 保持配置一致。

### Legit Aimbot

| 功能           | 说明                                                    |
| -------------- | ------------------------------------------------------- |
| FOV 限制       | Angle（角度）或 Screen（像素距准星）两种模式            |
| Hitbox         | Head / Neck / Chest / Pelvis                            |
| Aim Key        | `checkbox_with_key`：Hold/Toggle；`autoShoot` 时可免按键 |
| Visible Only   | `TraceShape` 视线检测                                   |
| Penetration    | visCheck 失败时用 AutoWall 评估是否可穿墙命中           |
| Recoil Control | 补偿 aim punch（2×）                                    |
| Silent Aim     | input_history 静默写角                                  |
| No Spread      | 散布补偿 + SpreadHooks 零 spread detour                 |
| Per-Weapon     | WeaponConfig 覆盖 smooth / RCS / trigger / removeSpread |
| Delay Aim      | 瞄准延迟（ms）                                          |
| In Smoke       | 配置项（enum 已预留，逻辑待补）                         |
| Auto Shoot     | 有目标时自动开火                                        |
| Team Check     | 跳过队友                                                |
| FOV 圈         | 背景 ImGui 绘制当前 FOV 范围                            |
| Menu Block     | 菜单打开 / 鼠标捕获时暂停战斗逻辑                       |

### Triggerbot

| 功能               | 说明                     |
| ------------------ | ------------------------ |
| 准星下敌人自动开火 | 基于 crosshair entity    |
| Trigger Key        | 与开关合并 UI；Hold/Toggle + Active Binds |
| Team Check         | 跳过队友                 |
| Delay              | 0–200 ms 延迟            |

### 配置项（Aimbot）

| 字段            | 类型  | 默认       |
| --------------- | ----- | ---------- |
| `enabled`       | bool  | false      |
| `fovType`       | int   | 0（Angle） |
| `fov`           | float | 5.0        |
| `screenFov`     | float | 100.0      |
| `targetHitbox`  | int   | 0          |
| `aimKey`        | int   | VK_LBUTTON |
| `aimKeyHold`    | bool  | true       |
| `aimUseKey`     | bool  | true       |
| `aimShowInBinds`| bool  | true       |
| `autoShoot`     | bool  | false      |
| `silentAim`     | bool  | true       |
| `teamCheck`     | bool  | true       |
| `visCheck`      | bool  | true       |
| `penetration`   | bool  | false      |
| `recoilControl` | bool  | false      |

### 配置项（Triggerbot）

| 字段        | 类型 | 默认    |
| ----------- | ---- | ------- |
| `enabled`   | bool | false   |
| `key`       | int  | VK_MENU |
| `useKey`    | bool | true    |
| `keyHold`   | bool | true    |
| `showInBinds` | bool | true  |
| `teamCheck` | bool | true    |
| `delayMs`   | int  | 15      |

---

## 战斗 — WeaponConfig（按武器配置）

**文件：** `WeaponConfig.hpp` / `WeaponConfig.cpp`

按 CS2 武器类型（Pistol / SMG / Rifle / Sniper / Heavy）维护独立的 Legit 与 Rage 配置块，供 Ragebot / LegitBot 在 `usePerWeapon` 开启时读取。

### Rage 每类武器字段（节选）

`hitboxes`、`multipointScale`、`hitchance`、`minDamage`、`safePoint`、`earlyAutoStop`、`removeSpread`、`delayAimMs` 等。

### Legit 每类武器字段（节选）

`smoothX` / `smoothY`、`rcsShots`、`triggerHitchance`、`removeSpread`、`noScope` 等。

---

## 战斗 — RageSubTick（Silent / RapidFire）

**文件：** `RageSubTick.hpp` / `RageSubTick.cpp`  
**Hook：** `Hook_InputParser`

| 功能       | 说明                                                |
| ---------- | --------------------------------------------------- |
| Silent Aim | 写 `InputHistoryEntry` 视角，不改客户端可见角       |
| Rapid Fire | Subtick 连发序列                                    |
| Seed Table | 启动时 `BuildSeedTable()` 预建 255 组 spread 随机对 |

Ragebot 在瞄准后调用 `SetSilentAim` / `SetRapidFire`；`ResetTick` 在 round_start / player_death 触发。

---

## 穿墙与伤害 — AutoWall

**文件：** `AutoWall.hpp` / `AutoWall.cpp`，Trace 辅助在 `AutoWallTrace.hpp`

### 功能

| API            | 说明                                                  |
| -------------- | ----------------------------------------------------- |
| `FireBullet`   | 模拟弹道穿透，输出伤害与有效性                        |
| `ScaleDamage`  | 按 HitGroup、护甲、ConVar（`mp_damage_scale_*`）缩放  |
| `CanPenetrate` | 简化接口，返回 `PenetrationResult { canHit, damage }` |

### HitGroup

Generic、Head、Chest、Stomach、LeftArm、RightArm、LeftLeg、RightLeg、Neck

### 使用方

- Legit Aimbot（`penetration` 开关）
- Ragebot / RageScan（`settings.penetration`）

---

## 散布补偿 — NoSpread & SpreadHooks

**文件：** `NoSpread.hpp`（内联 SHA1 + 散布种子算法）、`SpreadHooks.hpp` / `SpreadHooks.cpp`

### NoSpread（角度补偿）

- 根据 inaccuracy / spread 网格搜索补偿角
- `CompensateAngles(desired, cmd, out)` 供 Aimbot / Ragebot 调用
- 菜单中与 Legit / Rage 共用 `NoSpread::config.enabled`；Legit 可按武器 `removeSpread` 临时启用

### SpreadHooks（零散布 Detour）

| 阶段    | 行为                                                                         |
| ------- | ---------------------------------------------------------------------------- |
| SigScan | 扫描 `GetSpread` / `GetInaccuracy`（`Patterns.hpp`）                         |
| Install | 签名就绪后 MinHook detour，`FeatureInit::Init` 自动安装                      |
| 运行时  | `NoSpread::config.enabled` 或 `SetZeroSpreadActive(true)` 时 hook 返回 `0.f` |

> 参考项目中的 `WeaponAccuracySpreadClientSide` 无可用 pattern；Cookie 采用已验证的 GetSpread/GetInaccuracy 路径。游戏更新后若 detour 失败，仍回退到 NoSpread 角度补偿。

### 相关底层

- `CFunctionList` SDK 直调（Hitchance 模拟等）
- 与 `CL_Bypass` Subtick 写角配合

---

## 延迟补偿 — LagComp

**文件：** `LagComp.hpp` / `LagComp.cpp`

### 功能

| API                                    | 说明                                                      |
| -------------------------------------- | --------------------------------------------------------- |
| `RecordPlayers`                        | FrameStage 3：录制 origin、**128 骨骼**、`simulationTime` |
| `GetBacktrackTicks`                    | 根据 interp + latency 计算回溯 tick                       |
| `GetBacktrackOriginInterpolated`       | 按 tick 在两条记录间 **线性插值** origin                  |
| `GetBacktrackBoneInterpolated`         | 按 tick 在两条记录间 **线性插值** 单骨位置                |
| `GetBacktrackPos` / `GetBacktrackBone` | 对外查询（bone 优先走插值 API）                           |
| `Clear`                                | round_start / 换图清空                                    |

### 扫描侧用法

RageScan 在 backtrack 开启时：先解析 hitbox 对应 bone index → `GetBacktrackBoneInterpolated`；失败则 origin delta 回退。

### 参数

- 最大记录数：`kMaxRecords = 12`
- 最大 unlag 时间：`kMaxUnlagTime = 0.2s`
- ConVar：`cl_interp`、`cl_interp_ratio`、`cl_updaterate`

### 使用方

- Ragebot / RageScan（`backtrack` 开关）

---

## 反瞄准 — AntiAim

**文件：** `AntiAim.hpp`（内联）

### 模式

| 模式   | 说明                           |
| ------ | ------------------------------ |
| Static | 固定 pitch + 相对 yaw 偏移     |
| Spin   | pitch 固定 + 持续旋转 fake yaw |

### 功能

- 分离 real / fake yaw，写入 cmd 与 input_history
- `FixMovement` / `FixMovementPost`：WASD 相对相机意图修正移动方向
- 战斗模块占用视角时（`blockAntiAim`）跳过 AntiAim
- ThirdPerson 启用时延迟初始化 Camera Hook

### 配置项

| 字段        | 类型  | 默认   |
| ----------- | ----- | ------ |
| `enabled`   | bool  | false  |
| `mode`      | int   | Static |
| `pitch`     | float | 89     |
| `yaw`       | float | 180    |
| `spinSpeed` | float | 50     |

---

## 移动 — Bhop & Movement

### Bhop

**文件：** `Bhop.hpp` / `Bhop.cpp`

| 功能         | 说明                                                         |
| ------------ | ------------------------------------------------------------ |
| Bunny Hop    | 地面跳起、空中松 jump；仅操纵 usercmd，不 force +jump 内存   |
| Hold Space   | `requireSpace`：必须按住空格才 bhop                          |
| Edge Jump    | 离开平台边缘自动跳                                           |
| Jump Bug     | Subtick 落地帧 duck + jump 时序（`CL_Bypass::ApplyJumpBug`） |
| Hop Chance   | 概率跳（非 100% 触发 bhop）                                  |
| Auto Forward | 无 W/S 输入时自动 +forward                                   |
| 地面检测     | `EnginePred::IsOnGround()`，预测激活时用 predicted flags     |

| 配置项         | 默认  |
| -------------- | ----- |
| `enabled`      | false |
| `edgeJump`     | false |
| `jumpBug`      | true  |
| `autoForward`  | true  |
| `requireSpace` | true  |

> Bhop **不再** 自带 Auto Strafe；空中转向统一由 Movement 处理。

---

### Movement（高级）

**文件：** `Movement.hpp` / `Movement.cpp`

#### 处理流程

```
跳过 noclip / ladder / observer / 水中
  → EdgeBug
  → AutoStrafe（唯一入口）
  → Bhop::Process
  → MatrixMovementFix（AntiAim 视角备份时）
  → ProcessInputHistory（MovementCorrection + ValidateUserCommand）
```

#### Edge Bug

| 项      | 说明                                                        |
| ------- | ----------------------------------------------------------- |
| Keybind | `edgeBugKey`（默认侧键1），`edgeBugUseKey` 可关闭           |
| 预测    | `m_hPredictedPawn`、`m_bInLanding`、128 点环形向下 Trace    |
| 检测    | 64-tick 环检测 `z_velocity == -7`                           |
| Cvar    | 激活时 `sv_min_jump_landing_sound = 63464578`，否则恢复 260 |
| Duck    | 命中时清零 move、强制 duck、地面时松 jump                   |

#### Auto Strafe

| 模式  | 说明                                        |
| ----- | ------------------------------------------- |
| Off   | 关闭                                        |
| Legit | WASD assist + 速度角 ideal 修正             |
| Rage  | Legit 基础上 smoothing × 0.5，偏视角 strafe |

| 配置项         | 说明                  |
| -------------- | --------------------- |
| `strafeSmooth` | 0–100 平滑度          |
| `strafeAssist` | WASD 按键历史辅助方向 |

#### Movement Fix / Correction / Validate

| 功能                | 说明                                                                            |
| ------------------- | ------------------------------------------------------------------------------- |
| Movement Fix        | AntiAim 后矩阵移动方向修正（`movment_fix` 对齐参考）                            |
| Movement Correction | 逐条 input_history 按 base 视角修正 forward/left/up（原始 move 快照，避免累积） |
| Validate Angles     | clamp pitch/yaw、同步 move 按钮、反算 mousedx/mousedy                           |
| Auto Stop           | Rage 专用：`Slow` 反向减速 / `Early` 硬停                                       |

#### 配置项

| 字段                 | 默认        |
| -------------------- | ----------- |
| `movementFix`        | true        |
| `movementCorrection` | true        |
| `validateAngles`     | true        |
| `edgeBug`            | false       |
| `edgeBugUseKey`      | true        |
| `edgeBugKey`         | VK_XBUTTON1 |
| `strafeMode`         | Off         |
| `strafeSmooth`       | 50          |
| `strafeAssist`       | true        |

---

## 引擎预测 — EnginePred

**文件：** `EnginePred.hpp` / `EnginePred.cpp`

### 功能

- `Start`：保存并改写 `IGlobalVars`（currentTime、frameTime、tickCount）为 tickbase 预测时间
- 从 `m_hPredictedPawn` 读取并缓存 `s_predictedFlags`
- `End`：恢复 GlobalVars
- 公共 API：
  - `GetPredictedFlags()`
  - `GetPawnFlags(pawn)` — 预测激活时返回 predicted flags
  - `IsOnGround(pawn)`

### 使用方

- Movement（EdgeBug、AutoStrafe、ApplyAutoStop）
- Bhop（地面判定）
- Ragebot（内部嵌套预测）

### 相关 Hook

- `Hook_PredictionSimulation`（可选，签名缺失时跳过）

---

## 视觉 — ESP

**文件：** `Esp.hpp`（内联渲染）

### 功能

| 功能           | 说明                             |
| -------------- | -------------------------------- |
| Box            | Normal / Corners 两种框          |
| Skeleton       | 骨骼连线                         |
| Health Bar     | 左侧渐变血条                     |
| Name           | 玩家名                           |
| Distance       | 米制距离                         |
| Team Check     | 仅敌人                           |
| Max Distance   | 最大绘制距离                     |
| Glow ESP       | 写入 entity glow 属性（outline） |
| Bomb Timer     | 已 plant C4 倒计时               |
| Spectator List | 观战本地玩家的名单               |

### 配置项

| 字段                                       | 默认   |
| ------------------------------------------ | ------ |
| `enabled`                                  | true   |
| `bBox`                                     | true   |
| `boxMode`                                  | 0      |
| `bSkeleton`                                | false  |
| `bHealthBar`                               | true   |
| `bName`                                    | true   |
| `bDistance`                                | true   |
| `teamCheck`                                | true   |
| `maxDistance`                              | 3000   |
| `boxColor` / `skeletonColor` / `glowColor` | 见源码 |
| `bBombTimer`                               | false  |
| `bGlow`                                    | false  |
| `bSpectators`                              | false  |

---

## 视觉 — ESP Overlay 组件系统

**文件：** `EspOverlay.hpp` / `EspOverlay.cpp`

组件化 ESP 渲染管线，参考项目 overlay 架构。

### 组件

| 组件             | 说明                   |
| ---------------- | ---------------------- |
| `BoxComponent`   | 框 + 描边 + 可选填充   |
| `BarComponent`   | 四边对齐血条           |
| `TextComponent`  | 四边 + 方向对齐文字    |
| `OverlayContext` | 自动 side padding 布局 |

### 功能

- `RenderAll`：遍历实体，构建组件并绘制
- **弹药条**、**HK / KIT** 等 overlay 组件
- `WeaponIcons`：CS2GunIcons / iconscs2 / 内嵌 `game_icons` fallback
- 与 legacy `Esp.hpp` 可并存（`skipPlayerRendering` 避免重复）
- 使用 `CDraw::ProjectPlayerBounds` 统一 WorldToScreen 包围盒
- **布局边**：`nameSide` / `distanceSide` / `healthBarSide` / `ammoBarSide` / `flagsSide`（`AlignSide` 四边）
- **菜单预览**：`SyntheticEspPreview` 拖拽 NAME/DISTANCE/FLAGS/HEALTH/AMMO → 写回 `EspOverlay::config.*Side`，随 JSON 持久化

### 配置项

| 字段                 | 默认   |
| -------------------- | ------ |
| `enabled`            | true   |
| `useComponents`      | true   |
| `showHealthBar`      | true   |
| `showName`           | true   |
| `showDistance`       | true   |
| `showWeaponIcon`     | true   |
| `showAmmoBar`        | 见菜单 |
| `showHK` / `showKIT` | 见菜单 |
| `nameSide` … `flagsSide` | Top/Bottom/Left/Right |

---

## 视觉 — Chams

**文件：** `Chams.hpp` / `Chams.cpp`  
**Hook：** `Hook_DrawObject`（scenesystem.dll，签名缺失时优雅降级）

### 功能

- KV3 / VMAT 动态创建 Material2（Flat / Glow 两套，各含 visible + ignoreZ 变体）
- 按实体 owner 分类：Enemy / Local / Teammate / Weapon / Hands(ViewModel)
- `OnDrawObject` 替换材质与颜色，跳过默认绘制

### 配置项

| 字段            | 默认            |
| --------------- | --------------- |
| `enabled`       | false           |
| `enemy`         | true            |
| `local`         | false           |
| `teammate`      | false           |
| `weapon`        | false           |
| `hands`         | false           |
| `materialStyle` | Glow            |
| `ignoreZ`       | false           |
| `ignoreZColor`  | 独立 X-Ray 颜色 |
| `color`         | RGBA            |

### 状态提示

- DrawObject Hook 或材质初始化失败时，菜单显示警告并禁用 Chams 控件
- `FeatureInit::VerifyHooks` + `CNotify` 推送 Hook 就绪状态

---

## 视觉 — 子弹轨迹 / 第三人称 / World FOV / 菜单特效

### Bullet Tracers

**文件：** `Tracers.hpp`

- 检测 `m_iShotsFired` 增量触发轨迹
- 分段渐变拖尾 + 弹头亮点 + 落点标记
- 可配置：寿命、视觉弹速、粗细、射线长度

### Third Person

**文件：** `ThirdPerson.hpp` / `ThirdPerson.cpp`  
**Hook：** `OverrideView`、`GetRenderFov`、`SetViewModelFov`

| 功能      | 说明                                |
| --------- | ----------------------------------- |
| 第三人称  | 设置 `CCSGOInput::m_bInThirdPerson` |
| 相机距离  | 40–400，带碰撞 Trace 缩短           |
| FOV       | 60–140，Hook 渲染 FOV               |
| ViewModel | 第三人称时调整 viewmodel FOV        |

### World FOV

**文件：** `WorldFov.hpp` / `WorldFov.cpp`  
**Hook：** `Hook_GetRenderFov`

- 菜单可调世界 FOV（60–140）
- 开镜时可选保留原始 scoped FOV

### Menu Effects

**文件：** `MenuEffects.hpp` / `MenuEffects.cpp`  
**Synthetic 集成：** `gui_cookie.cc`（`shaderBlur` → `draw_background_blur`）、`SyntheticWatermark`、`var->c_notify`

| 功能 | 说明 |
| ---- | ---- |
| Watermark | 关闭菜单时的 ImGui 水印（`MenuEffects::config.watermark`） |
| Synthetic Watermark | Synthetic 框架水印（位置可配置，Misc Tab） |
| Particles | 菜单背景粒子连线 |
| Shader Blur | 菜单面板 DX11 背景模糊（`shaderBlur`） |
| Blur Fallback | 无 shader 时的暗层占位（`blurPlaceholder`） |
| Menu Background Image | 面板背景图 + Alpha |
| Notify Position | Synthetic 菜单内 toast 四角位置 |
| Menu DPI | 100–200%，写入 Config **Menu** 段 |

---

## 换肤 — SkinChanger（库存注入）

**文件：** `SkinChanger.hpp`（配置 + fallback）、`SkinChangerInventory.*`（主路径）、`SkinChangerSetModel.*`

### 主路径：Loadout / Inventory 注入（对齐参考）

| 步骤          | 说明                                                                                  |
| ------------- | ------------------------------------------------------------------------------------- |
| 创建物品      | `CEconItem::Create` → 填 defIndex / paint / wear / seed / StatTrak                    |
| SOC 注入      | `CCSPlayerInventory::AddEconItem`                                                     |
| 装备          | `EquipItemInLoadout`（CT/T 双队）                                                     |
| 运行时同步    | **FSN stage 6** 遍历 owned 武器，复制 loadout item view → 实体、SetModel、MeshGroup 2 |
| ViewModel     | `Hook_SetModel` 按 managed loadout 模型覆盖（防高 ping 刀闪）                         |
| Killfeed 刀名 | `player_death` 时本地刀杀改写 event `weapon` 字符串                                   |
| 回合刷新      | `round_start` / Force Update → `NetworkClientService::Update()`（deltaTick = -1）     |
| 追踪          | `s_addedItemIds` 白名单；`Shutdown` 移除假物品                                        |

`SkinChanger::Tick` 在库存路径激活时**不再**走 RegenerateWeaponSkins fallback；仅在无 inventory 时保留旧内存覆写逻辑。

### 菜单功能

| 功能              | 说明                                                     |
| ----------------- | -------------------------------------------------------- |
| 全武器 PaintKit   | 按 defIndex 配置 paintKit / wear / seed / StatTrak       |
| PaintKit 浏览器   | 稀有度过滤 + 名称搜索 + 自定义 ID                        |
| Knife Changer     | 20+ 刀型 + 库存 equip                                    |
| Force Update      | `forceUpdate` → `SyncFromConfig` + **NetworkFullUpdate** |
| Clear / Randomize | 批量清除或随机皮肤                                       |

### SkinConfig 字段

`paintKit`、`wear`、`seed`、`statTrak`（-1 关闭）、`enabled`

### 相关 Hook

- `Hook_FrameStageNotify` → `SkinChangerInventory::OnFrameStageNotify(6)`
- `Hook_EquipItemInLoadout` → 武器/刀 SOC 刷新（手套之前）
- `Hook_SetModel` → loadout 模型名
- `Hook_IsLoadoutAllowed` → 允许 loadout 操作
- `Hook_SOCacheSubscribed`（透传）

---

## 换肤 — 手套 Gloves

**文件：** `Gloves.hpp` / `Gloves.cpp`

### 功能

| 功能                   | 说明                                                  |
| ---------------------- | ----------------------------------------------------- |
| 手套模型               | 从 `g_GlovesNames` 列表选择                           |
| PaintKit / Wear / Seed | 完整 econ 属性                                        |
| Loadout Team           | T（2）/ CT（3）                                       |
| Apply Gloves           | 手动触发应用                                          |
| 材质刷新               | `material_magic_number` + `invalidate_glove_material` |
| 帧更新                 | `OnGlove` @ FrameStage                                |
| 回合重置               | `ResetRound`                                          |

### 配置项

| 字段         | 默认  |
| ------------ | ----- |
| `enabled`    | false |
| `modelIndex` | 0     |
| `paintKit`   | 0     |
| `wear`       | 0.001 |
| `seed`       | 0     |
| `team`       | 3     |

---

## 换肤 — 自定义纹理 TextureOverride

**文件：** `TextureOverride.hpp`（菜单内 `CustomTexture` 命名空间）

### 功能

- 从磁盘加载图片替换武器 SRV
- Browse 模式：F10/F11 循环候选纹理，F12 锁定
- 尺寸过滤 / No Filter 调试模式
- Hook 统计：hookCalls、unique SRVs

---

## 杂项 — Plant Bomb Anywhere

**文件：** `PlantBomb.hpp` / `PlantBomb.cpp`

- 持 C4 时将 `m_bInBombZone` spoof 为 true
- CreateMove + FrameStage（NET_UPDATE_END）双路径应用

---

## 事件 — GameEvents

**文件：** `GameEvents.hpp` / `GameEvents.cpp`  
**Hook：** `Hook_HandleGameEvents`、`Hook_FireEventClientSide`

| 事件           | 行为                                                                                                                              |
| -------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| `round_start`  | 清空 LagComp、重置 Gloves 回合状态、Rage/Legit round 回调、**SkinChangerInventory::SyncFromConfig**、**RequestNetworkFullUpdate** |
| `player_death` | **Killfeed 刀名**、Rage/Legit 死亡回调、RageSubTick reset                                                                         |

---

## 资源资产 — Resources / 字体

**目录：** `src/base/Client/Resources/`（构建时 xcopy 到 `OutDir\Resources\`）

| 组件              | 说明                                                         |
| ----------------- | ------------------------------------------------------------ |
| **WeaponIcons**   | 磁盘 TTF → 内嵌 `game_icons` / `iconscs2_embedded` fallback  |
| **EmbeddedFonts** | Font Awesome Solid（`CNotify` 图标）、smallest_pixel（可选） |
| **icons/\*.svg**  | 73 个参考 SVG                                                |
| **branding/**     | cs2.png / cs2.webp                                           |
| **embedded/**     | fa_solid_900、Bgs.h、ui_icons、font.h 等                     |

详见 `src/base/Client/Resources/README.md`。

---

## 底层基础设施

### ConVar 管理（CConVars）

启动时缓存：`mp_teammates_are_enemies`、`cl_interp*`、`sv_autobunnyhopping`、第三人称相关、`m_pitch`/`m_yaw`/`sensitivity`、`sv_min_jump_landing_sound`、`sv_standable_normal`、`mp_damage_scale_*` 等。

### 签名扫描（CSigScan / CSigScanManager）

批量扫描 + 回调解析；Spread、Hitbox、DrawObject 等依赖此框架。

### Schema v2（CSchemaV2）

运行时 schema offset 解析，减少硬编码。

### 日志（CLog / COOKIE_LOG）

分级彩色日志，开发调试用。

### 通知（CNotify）

Toast 队列：Info / Success / Warning / Error，带动画进出场；类型图标使用 **Font Awesome**（`EmbeddedFonts`）。

### 绘制工具（CDraw）

WorldToScreen、BBox 8 顶点、Rect/Circle/Text 渲染 flags、Glow 描边。

### 输入系统（CInputSystem）

WndProc 键位数组、Hold/Toggle 模式。

### CL_Bypass（Subtick 操控）

| API                           | 用途                         |
| ----------------------------- | ---------------------------- |
| `SetViewAngles`               | 写 base / input_history 视角 |
| `SetAttack` / `SetDontAttack` | 开火控制 + Subtick           |
| `SetButton`                   | 任意按钮 + 可选 Subtick when |
| `ApplyJumpBug`                | Jump bug subtick 序列        |
| `AddSubtickStrafeStep`        | Subtick 视角步进             |
| `SpoofCrc`                    | CRC 同步                     |

### Protobuf

集成 `cs_usercmd.pb` / `usercmd.pb`；`Hook_MessageLite_SerializePartialToArray` 拦截序列化。

### Call Stack Spoof（CSpoofCall）

ConVar Setup 等敏感调用可选 spoof。

### 武器图标（WeaponIcons）

加载顺序：`Resources/*.ttf` → 内嵌压缩字体；刀 glyph 可用 `iconscs2` / `obs_icons`。

### 内嵌字体（EmbeddedFonts）

`InitFontAwesome` / `InitSmallestPixel`；供通知与可选 UI 使用。

---

## Hook 清单

| Hook                                      | DLL                   | 用途                                                  |
| ----------------------------------------- | --------------------- | ----------------------------------------------------- |
| Present / ResizeBuffers / CreateSwapChain | gameoverlayrenderer64 | ImGui 渲染                                            |
| CreateMove                                | client                | 功能主循环                                            |
| FrameStageNotify                          | client                | 手套 / **SkinChangerInventory** / LagComp / PlantBomb |
| InputParser                               | client                | **RageSubTick** Silent / RapidFire                    |
| HandleGameEvents                          | client                | **GameEvents** round_start / player_death             |
| OverrideView                              | client                | 第三人称相机                                          |
| GetRenderFov / SetViewModelFov            | client                | FOV / **WorldFov**                                    |
| DrawObject                                | scenesystem           | **Chams 核心**                                        |
| DrawGlow                                  | client                | Glow 相关                                             |
| SetModel                                  | client                | **库存皮肤** viewmodel / 武器模型                     |
| PredictionSimulation                      | client                | 引擎预测                                              |
| SOCacheSubscribed                         | client                | 库存（透传）                                          |
| EquipItemInLoadout / IsLoadoutAllowed     | client                | Loadout / 皮肤 equip                                  |
| LevelInit / LevelShutdown                 | client                | 地图切换清理                                          |
| OnAddEntity / OnRemoveEntity              | client                | 实体生命周期                                          |
| GetMatricesForView                        | client                | 视图矩阵                                              |
| FireEventClientSide                       | client                | 游戏事件                                              |
| MouseInputEnabled / IsRelativeMouseMode   | client / inputsystem  | 菜单鼠标                                              |
| OnClientOutput / CDemoRecorder            | engine2               | 输出 / Demo                                           |
| SerializePartialToArray                   | client                | Protobuf 输入                                         |
| AntiTamper                                | client                | 反篡改绕过                                            |

> 标记 `SkipIfNotFound` 的 Hook（DrawObject、SetModel、LevelInit 等）签名失效时跳过，不影响其他功能。

---

## 模块互斥与优先级

| 规则            | 行为                                                                       |
| --------------- | -------------------------------------------------------------------------- |
| Ragebot ↔ Legit | Rage 开启时禁用 Aimbot / Triggerbot UI 与逻辑                              |
| 战斗 ↔ AntiAim  | `blockAntiAim == true` 时跳过 AntiAim                                      |
| Movement Fix    | AntiAim 启用且战斗未占用视角时，`FixMovementPost` 修正移动                 |
| EnginePred      | CreateMove 外层 Start/End 包裹 Movement；Rage 内部独立嵌套                 |
| Chams           | 依赖 DrawObject + 材质双向就绪，否则仅 Glow/ESP 可用                       |
| 菜单打开        | Aimbot 战斗阻塞（`IsMenuBlockingCombat`）                                  |
| 皮肤            | 库存注入（SkinChangerInventory）为主；无 inventory 时 SkinChanger fallback |
| Spread          | Sig 失效时 SpreadHooks detour 跳过，NoSpread 角度补偿仍可用                |

---

## 版本说明

- 菜单标题：**Cookie v1.4**（Synthetic 8 Tab + Lua 运行时）
- 文档基准：当前 `src/base/Client/` 源码树（Synthetic UI、库存皮肤、LagComp 骨骼插值、SpreadHooks、ESP 布局预览）
- 平台：Windows CS2 编译与注入；macOS 可编辑源码，游戏内验证需在 Windows **Release x64** 进行

### 实现状态摘要

| 类别 | 状态 |
| ---- | ---- |
| **核心战斗 / 移动 / 视觉** | Rage/Legit、AutoWall、NoSpread/SpreadHooks、LagComp、Movement、ESP + Overlay、Chams、ThirdPerson/FOV |
| **皮肤** | SkinChangerInventory、Gloves、CustomTexture |
| **菜单** | Synthetic 运行时（`gui_cookie.cc`）；`CookieMenu.hpp` 仅对照 |
| **菜单打磨** | ESP 拖拽预览、DPI/模糊/通知持久化、Active Binds、Config Create/排序 |
| **Lua** | 基础 VM + 4 个 API；扩展绑定待办见根 [README.md](./README.md) |
| **本地工具** | `MenuSandbox`（UI 沙盒，无脚本执行） |

> Windows 端需验证：`SpreadHooks` / `HitboxNative` 签名、vcpkg `lua` 链接、`NetworkClientService`、FSN stage 6 皮肤同步。

---

_完整构建与资源说明见根目录 [README.md](./README.md)。_
