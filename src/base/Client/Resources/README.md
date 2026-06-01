# Cookie Client Resources

Runtime assets vendored under `Client/Resources/` and wired into the overlay / ESP pipeline (no external reference tree required at build time).

## Runtime files (`xcopy` → `OutDir\Resources\`)

| Path                | Source             | Usage                           |
| ------------------- | ------------------ | ------------------------------- |
| `CS2GunIcons.ttf`   | vendored           | Weapon ESP icon font (primary)  |
| `iconscs2.ttf`      | vendored           | Knife / secondary weapon glyphs |
| `obs_icons.ttf`     | vendored           | Observer / knife fallback font  |
| `icons/*.svg`       | vendored           | SVG icon set (72 files)         |
| `branding/cs2.png`  | vendored           | Menu / watermark branding       |
| `branding/cs2.webp` | vendored           | WebP variant                    |

## Embedded headers (`Resources/embedded/`)

Compiled into the DLL — used when TTF files are missing beside the module.

| File                         | Purpose                                    |
| ---------------------------- | ------------------------------------------ |
| `fa_solid_900.h`             | Font Awesome Solid TTF bytes               |
| `font_awesome_5.h`           | FA icon UTF-8 codepoint macros             |
| `game_icons.h`               | Compressed CS2 gun icon font fallback      |
| `iconscs2_embedded.cpp/.hpp` | Raw `iconscs2.ttf` fallback                |
| `smallest_pixel.h`           | Compressed pixel font (optional ESP)       |
| `Bgs.h`                      | JPEG menu background bytes                 |
| `font.h`                     | Lexend Bold embedded TTF (menu typography) |
| `esp_preview.h`              | PNG player silhouette for ESP menu preview |
| `ui_icons.hpp`               | Sesame UI icon font (compressed)           |

## Code integration

- `WeaponIcons.cpp` — file TTF → embedded `game_icons` / `iconscs2` fallback chain
- `EmbeddedFonts.cpp` — Font Awesome + smallest pixel loaders
- `CNotify.cpp` — FA icons for toast notifications
- `CCookieGUI::InitFont()` — loads weapon + FA fonts at startup

Build copies the whole `Resources/` tree post-build via `CookieDll.vcxproj`.
