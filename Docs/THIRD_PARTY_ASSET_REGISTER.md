# Reality.exe Third-Party Asset Register

## Purpose

This register records external source content that actually enters the Reality.exe repository. It is an operational provenance record, not legal advice. Each future acquisition must preserve the exact license and receipt evidence available on its acquisition date.

## Task 020 vertical slice — 2026-08-13

No third-party files entered the repository during the initial Task 020 implementation.

The recommended paid Fab packs were not present in the local Epic/Fab Vault, and Task 020 did not authorize purchasing them. No unrelated Vault content was substituted. The first vertical slice therefore uses only Reality.exe-owned Unreal assets created from Unreal Engine basic shape content already available to the project. No ambientCG texture was downloaded or imported; the first shared materials are procedural color/roughness/metallic materials without external texture inputs.

| Asset/source | Publisher | Source | Acquisition date | License | Original package | Imported Reality paths | Purpose | Modifications/derived assets | Notes |
|---|---|---|---|---|---|---|---|---|---|
| None for Task 020 initial pass | N/A | N/A | N/A | N/A | N/A | N/A | N/A | N/A | Paid-pack import portion deferred; no external source files are present. |

## Required fields for future entries

Every external source must add a row before its first production commit with:

- Exact pack/asset name and version
- Publisher or original author
- Canonical listing/source URL and listing ID
- Acquisition date, displayed price, currency, and receipt/order identifier location
- Exact license name, version, tier, and retained evidence location
- Original package name and declared Unreal compatibility
- Imported source paths and precise purpose
- Reality-owned derived paths, wrappers, instances, or modified duplicates
- Attribution, redistribution, repository-access, trademark, or embedded-source notes
- UE staging results, excluded dependencies, and later removal/replacement history

Receipts and account details belong in restricted team records, not in the repository.

## Task 021 readability revision — 2026-08-13

Four ambientCG surface sources were acquired directly from their official asset pages as the 2K-JPG packages. ambientCG identifies its downloadable assets as Creative Commons CC0 1.0 Universal. Only Base Color, DirectX Normal, and Roughness maps entered the repository; archives, previews, displacement, OpenGL normals, DCC files, and package metadata were excluded.

| Asset/source | Publisher | Source | Acquisition date | License | Original package | Imported Reality paths | Purpose | Modifications/derived assets | Notes |
|---|---|---|---|---|---|---|---|---|---|
| Plaster 002 (`Plaster002`) | ambientCG | https://ambientcg.com/view?id=Plaster002 | 2026-08-13 | Creative Commons CC0 1.0 Universal, as stated by ambientCG | `Plaster002_2K-JPG.zip` | `/Game/ThirdParty/ambientCG/Surfaces/Plaster002/` | Painted/plaster facility walls | Texture parameters in `M_FacilitySurface`; used by warm-white and cool-gray facility instances | Imported Color, NormalDX, and Roughness only; 2048 maximum texture size. |
| Concrete 010 (`Concrete010`) | ambientCG | https://ambientcg.com/view?id=Concrete010 | 2026-08-13 | Creative Commons CC0 1.0 Universal, as stated by ambientCG | `Concrete010_2K-JPG.zip` | `/Game/ThirdParty/ambientCG/Surfaces/Concrete010/` | Sealed concrete and controlled test-floor surfaces | Texture parameters in `M_FacilitySurface`; selected by sealed-concrete and Physics/test instances | Imported Color, NormalDX, and Roughness only; 2048 maximum texture size. |
| Painted Metal 007 (`PaintedMetal007`) | ambientCG | https://ambientcg.com/view?id=PaintedMetal007 | 2026-08-13 | Creative Commons CC0 1.0 Universal, as stated by ambientCG | `PaintedMetal007_2K-JPG.zip` | `/Game/ThirdParty/ambientCG/Surfaces/PaintedMetal007/` | Painted and dark structural metal surface detail | Default texture set in `M_PaintedMetal`; color remains controlled by existing Material Instances | Imported Color, NormalDX, and Roughness only; source blue is restrained by project tinting. |
| Metal 010 (`Metal010`) | ambientCG | https://ambientcg.com/view?id=Metal010 | 2026-08-13 | Creative Commons CC0 1.0 Universal, as stated by ambientCG | `Metal010_2K-JPG.zip` | `/Game/ThirdParty/ambientCG/Surfaces/Metal010/` | Galvanized and utility-metal surface detail | Default texture set in `M_UtilitySurface`; project instances control tone and roughness | Imported Color, NormalDX, and Roughness only; displacement and duplicate NormalGL excluded. |
