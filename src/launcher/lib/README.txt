Place BlackBone static library here as `BlackBone.lib` (x64).

Normally you do NOT copy it manually: `msbuild Cookie.sln` builds `BlackBoneLib` first and copies the lib here.

First-time setup (if external/BlackBone is missing):
  git clone https://github.com/DarthTon/BlackBone external/BlackBone

Requirements (must match Cookie.vcxproj):
- Runtime: /MT (Release) or /MTd (Debug)
- Toolset: v145

Cookie applies MSVC compatibility patches in external/BlackBone:
  - CookieBuild.props (OutDir + BLACKBONE_NO_NET when cor.h / NETFX SDK is absent)
  - PE/ImageNET.h stub for injector builds without .NET metadata
  - ProcessModules.cpp typename fix (if present in your tree)
