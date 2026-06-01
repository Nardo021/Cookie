# Features 目录结构

```
Features/
├── Init/           模块初始化（FeatureInit）
├── Events/         游戏事件分发（GameEvents）
├── Combat/         战斗相关
│   ├── Rage/       Ragebot、RageScan、RageSubTick
│   ├── Legit/      Aimbot、LegitBot、Triggerbot、WeaponConfig
│   ├── AutoWall.*  穿墙伤害
│   ├── EnginePred.* 引擎预测
│   ├── LagComp.*   延迟补偿 / Backtrack
│   └── NoSpread.hpp
├── Visuals/        ESP、Chams、WorldFov、ThirdPerson、Tracers
├── Movement/       Movement、Bhop
├── Inventory/      皮肤、手套、武器图标、TextureOverride
└── Misc/           AntiAim、PlantBomb
```

UI 菜单相关代码位于 `Client/UI/Menu/`（CookieMenu、MenuConfig、MenuAssets、MenuEffects）。
