# Reality.exe Demo Asset Acquisition Research and Import Plan

## Status, scope, and research date

Research was performed on **2026-08-13** for the Reality.exe Unreal Engine **5.7.4** project. Availability, regional access, prices, sales, license choices, download sizes, and supported engine versions can change. Every selected listing must therefore pass the acquisition-day checks in this document before anyone purchases, downloads, or adds it to the project.

This is a research and planning document only. It does not authorize an asset purchase, Fab `Add to Project`, download, migration, Unreal asset generation, Demo-map edit, gameplay change, Hero-asset replacement, or Task 020 implementation.

The primary filters are the approved Task 018 direction: a restrained, believable corporate testing and logistics facility; readable industrial maintenance infrastructure; no neon-cyberpunk, military-sci-fi, horror-first, or recognizable single-pack treatment; and no change to the approved graybox layout.

## Executive decision

Use **strategy B: a Reality.exe-owned simple modular architecture shell plus selectively migrated third-party props**.

Do not reshape the Demo around a marketplace environment. Retain the approved room dimensions, traversal, target sightlines, Witness coverage, Context causality, and Time finale. Build the shell and the six shared Reality materials as original project work, then use a small paid prop set to accelerate maintenance, storage, and office dressing. A large free industrial collection is a staging source only, not a bulk project dependency.

The recommended balanced acquisition, before tax, is:

| Purpose | Candidate | Displayed current price | Recommendation |
|---|---|---:|---|
| Maintenance and structural support | Building and Industrial Props | US$19.99 | Acquire conditionally |
| Storage and logistics | Modular Warehouse Props Pack | US$11.99 | Acquire conditionally |
| Small office dressing | Office Props Pack | US$11.99 | Acquire conditionally |
| Tiling source textures | Selected ambientCG materials | Free | Acquire selectively |
| Large supplemental library | Factory Environment Collection | Free | Optional staging source only |
| Architecture kit | Reality.exe-owned shell | Internal production | Preferred over a purchased kit |

**Estimated balanced total: US$43.97 before tax.** A temporary sale was visible for Modular Warehouse Props Pack, but the budget uses its displayed regular `From US$11.99` price rather than depending on a promotion. All Fab acquisitions remain conditional on an acquisition-day license and compatibility check.

## Evidence policy and unknowns

The research uses official Fab listings/search results, Epic documentation, and original-provider pages. A field is `UNKNOWN` when the accessible official listing did not document it. `UNKNOWN` does not mean unsupported; it means Task 020 must verify it before import. Prices are the Fab values displayed during research and may vary by tier, tax, region, or sale.

The public Fab pages exposed a license selector for the third-party candidates but did not expose the selected license name or full listing-specific license metadata to this research session. Consequently, no third-party Fab candidate is approved merely because it is listed on Fab. Each is explicitly **conditional** on the license Gate below.

## License Gate

### Fab rules relevant to Reality.exe

Fab can offer different licenses, including Creative Commons and the Fab Standard License; Standard-license listings can have Personal and Professional price tiers. Epic's current guidance describes the applicable tiers and a US$100,000 gross-revenue threshold for the Personal tier. The tier chosen at acquisition must match the purchaser's eligibility; this plan does not determine the studio's legal or revenue status. See [Fab licenses and pricing](https://dev.epicgames.com/documentation/fab/licenses-and-pricing-in-fab?lang=en-US).

Under the current [Fab Standard License summary](https://www.fab.com/eula), a licensed asset may be used and modified in commercial projects and distributed as part of a packaged project in which it is not offered on a stand-alone basis. Source assets may be shared with collaborators working on the project, including through a private repository, but may not be redistributed as stand-alone assets or used to let third parties extract them. Attribution is not required by the Standard License. Personal and Professional tiers grant the same usage rights, and crossing the revenue threshold later does not by itself require an upgrade for a previously acquired asset. A `Reference Only` offer does not provide source-format files and is unsuitable for the planned Unreal import workflow.

Fab also retains legacy acquisition cases. An asset acquired under the former Unreal Engine Marketplace license may remain governed by that acquisition's license. The acquisition record—not a generic assumption about Fab—must identify the governing terms. Epic explains that a listing exposes its price, formats, and applicable license and requires accepting that license before download in [Purchasing and Downloading Assets in Fab](https://dev.epicgames.com/documentation/fab/purchasing-and-downloading-assets-in-fab?lang=en-US).

This is an operational reading of the cited source, not legal advice.

### Mandatory acquisition-day checklist

For every Fab candidate, record a dated screenshot/PDF or receipt showing:

1. Exact listing URL, pack name, publisher, price, and offer tier.
2. Exact license type and version presented at checkout.
3. That the offer supplies Unreal/FBX source assets, not `Reference Only` access.
4. Current supported Unreal versions and any required plugins/project settings.
5. Commercial packaged-game use and private collaborator/repository treatment under the selected license.
6. Receipt/order identifier and acquiring Epic account or organization owner.
7. Any listing-specific attribution, third-party-content, trademark, AI, or redistribution notice.

**Stop acquisition** if the license remains hidden or ambiguous, the publisher differs from this record, the pack is regionally unavailable, only Reference access is offered, commercial use cannot be confirmed, or required third-party content has separate terms. Do not substitute an archive, mirror, or re-upload.

### Source-control implications

Fab Standard-licensed source files may be available to project collaborators in a private repository when needed to develop the project, according to the EULA summary. That does not permit a public asset dump or stand-alone redistribution. Keep the development repository private, limit access to collaborators working on Reality.exe, and exclude vendor examples and unused source content. Public source releases must omit redistributable third-party source files unless a separately verified license permits them.

ambientCG states that its assets are released under **CC0**, can be used commercially, and do not require attribution; this is documented directly on its asset pages such as [Concrete 003](https://ambientcg.com/view?id=Concrete003). CC0 is materially less restrictive for repository use than the Fab Standard License, but the register should still preserve author/source/provenance. Logos, trademarks, identifiable people, and other third-party rights are separate concerns; none are planned.

## Candidate shortlist

The serious shortlist deliberately contains five sources rather than a large wishlist. The first is an architecture benchmark/contingency, the next three are focused prop sources, and the fifth is an optional free supplemental collection.

### 1. Modern Offices — Dexsoft Games

- **Role:** primary modular/environment candidate for comparison, not the recommended shell.
- **Source and price:** third-party seller on [Fab — Modern Offices](https://www.fab.com/listings/74a9dfb6-4f35-46ed-b394-28615ae08d73?lang=ja); Fab search displayed **From US$29.99** during research.
- **License:** listing-specific offer was not exposed; `UNKNOWN` until the Fab License Gate confirms Standard or another approved commercial license. If Standard, packaged-game use, modification, private collaborator access, no attribution, and no stand-alone redistribution follow the terms summarized above.
- **Format:** native Unreal Engine content.
- **UE compatibility:** listing imagery is explicitly described as updated in **5.7**, which is the strongest direct 5.7 signal among these candidates. Exact minimum/maximum engine-version selector: `UNKNOWN`.
- **Size:** `UNKNOWN`.
- **Contents/modularity:** 120 unique meshes, modular multi-floor office construction, furniture, supplies, kitchen/technology content, and an example level.
- **Materials/textures:** listing says some textures are from ambientCG and poster images from Unsplash; exact resolutions, material counts, and redistribution trail are `UNKNOWN`. Posters can be switched through material settings.
- **Nanite/LODs/collision:** `UNKNOWN`.
- **Dependencies:** no external plugin is documented, but the listing explicitly requires enabling **Virtual Texture Streaming** and **Virtual Textures for Opacity Mask** in project settings. That is a material project-setting dependency.
- **Customization:** modular meshes and switchable posters suggest reasonable flexibility; exact pivot/grid standards and material-instance controls are `UNKNOWN`.
- **Strengths:** corporate vocabulary, explicit 5.7 update, broad ordinary-office coverage.
- **Weaknesses:** far larger than the vertical slice needs; it could dictate the layout and create a recognizable office-pack look; enabling virtual-texture settings solely for a prop source would be an avoidable project change; embedded third-party texture/image provenance requires review.
- **Reality.exe fit:** useful only as a carefully isolated source of a few generic construction/furniture pieces if the custom shell proves insufficient. Do not migrate its example level or use it as the Demo's visual identity.
- **Stock-pack risk:** **high** if used wholesale; medium if a few meshes receive Reality-owned material overrides.
- **Disposition:** **NICE TO HAVE / premium contingency**, not part of the balanced recommendation.

### 2. Building and Industrial Props — 32cm

- **Role:** focused maintenance and industrial structural support source.
- **Source and price:** third-party seller on [Fab — Building and Industrial Props](https://www.fab.com/listings/386b70ff-7d8b-4ba2-8d21-de187f3b8bd3?lang=en); current Fab search displayed **From US$19.99**.
- **License:** listing-specific offer `UNKNOWN`; acquisition is conditional on the Fab License Gate.
- **Format:** native Unreal Engine content.
- **UE compatibility:** exact supported versions and explicit 5.7 support are `UNKNOWN`. The content appears primarily static geometry/materials and therefore is a plausible upgrade candidate, but that is an inference to validate in a staging project—not a compatibility claim.
- **Size:** `UNKNOWN`.
- **Contents/modularity:** modular walls, floors, roof frames, vents; 41 wire pieces; 18 tube pieces; switches, outlets, supports, electrical boxes; industrial fan, shelf and lights; 13 ventilation pieces; 26 water-pipe pieces/valves; 22 industrial-pipe pieces; doors and frames.
- **Materials/textures:** primarily 4K PBR sets with packed Unreal ARM masks. The listing describes controls for color, roughness, normal, specular, metallic values, and replacement seamless wall/floor/roof textures.
- **Nanite/LODs/collision:** `UNKNOWN`.
- **Blueprint/plugin dependencies:** none documented; exact dependency graph `UNKNOWN`.
- **Customization:** strong at the material level and useful as modular service geometry. Reality-owned instances can mute color/grime without destructive source edits.
- **Strengths:** directly fills pipes, vents, conduits, boxes, utility lights, frames, and service-detail gaps with one coherent source.
- **Weaknesses:** broad construction content and 4K maps can add unnecessary weight; tags/presentation lean older industrial/horror in places; collision, grid dimensions, and modern UE setup are undocumented.
- **Reality.exe fit:** strongest paid candidate for Maintenance Loop and restrained Security Hub utility dressing. Use only functional-looking service pieces.
- **Stock-pack risk:** **medium**, reduced to low-medium by limiting the mesh family, overriding materials, and mixing with the custom shell/signage.
- **Disposition:** **MUST HAVE, conditional**.

### 3. Modular Warehouse Props Pack — Daniele Calvi

- **Role:** focused storage/logistics source.
- **Source and price:** third-party seller on [Fab — Modular Warehouse Props Pack](https://www.fab.com/listings/0922c63c-1b37-4301-b0ae-7eee24bc69fb); category listing displayed **From US$11.99**. A US$5.99 sale was also visible during research, but it is not assumed in budgets.
- **License:** listing-specific offer `UNKNOWN`; acquisition is conditional on the Fab License Gate.
- **Format:** native Unreal Engine content.
- **UE compatibility:** exact Unreal versions and 5.7 support `UNKNOWN`. It is described as mesh/material content with no documented gameplay framework; validate conversion in staging.
- **Size:** `UNKNOWN`.
- **Contents/modularity:** six cardboard boxes, three wooden boxes, gas/plastic tanks and barrels, plastic boxes, pallet truck, pallet, stair, cage tank, steel barrels, and a modular shelf with three plane variants.
- **Materials/textures:** Diffuse, Normal, and packed Lightmask (metallic/roughness/AO). Many assets expose 1K/2K/4K material-quality variants. Color can be changed for the shelf, pallet truck, tanks, and several containers by making material instances.
- **Nanite/LODs/collision:** `UNKNOWN`.
- **Blueprint/plugin dependencies:** none documented; exact dependency graph `UNKNOWN`.
- **Customization:** good for neutral color unification and controlled texture resolution. Material-quality switching needs inspection to avoid importing duplicate high-resolution variants unnecessarily.
- **Strengths:** compact, functional storage vocabulary; shelf/pallet/cart/box subset maps directly to Observed Storage and Logistics.
- **Weaknesses:** gas tanks, survival-style props, and barrels are outside the first slice; sparse ratings and undocumented collision/versioning increase validation burden.
- **Reality.exe fit:** good when limited to standardized shelving, pallet, pallet truck, plastic bins, and clean boxes. Avoid survival/military staging.
- **Stock-pack risk:** **medium-low** with custom labels, restricted palette, and deliberate repetition standards.
- **Disposition:** **MUST HAVE, conditional**.

### 4. Office Props Pack — 32cm

- **Role:** generic office clutter source, not an architecture or Hero source.
- **Source and price:** third-party seller on [Fab — Office Props Pack](https://www.fab.com/listings/84ddd217-6215-40bb-ade3-dc7b1b413733); Fab search displayed **From US$11.99**.
- **License:** listing-specific offer `UNKNOWN`; acquisition is conditional on the Fab License Gate.
- **Format:** native Unreal Engine content.
- **UE compatibility:** exact supported versions and explicit 5.7 support `UNKNOWN`; no code dependency is documented.
- **Size:** `UNKNOWN`.
- **Contents/modularity:** 30 distinct pens, notepads, scissors, stapler, ruler, mugs, cutter, suitcases, boxes, bins, archive items, documents, ring binders, clipboards, and folders; two texture sets produce 60 presented variants.
- **Materials/textures:** 4K PBR Albedo, Normal, and packed Unreal ARM maps; 120k vertices and 200k triangles total are documented.
- **Nanite/LODs/collision:** `UNKNOWN`. Small desk clutter generally should not require Nanite, but no assumption is made.
- **Blueprint/plugin dependencies:** none documented; exact dependency graph `UNKNOWN`.
- **Customization:** source has texture variants, but color/tint parameters are not documented. A restrained Reality-owned parent/override or selected original materials should be tested rather than editing vendor materials.
- **Strengths:** compact office storytelling without purchasing an entire office environment; documents, folders, bins, and clipboards reinforce Routine Intake and Security administration.
- **Weaknesses:** 4K maps and two textures per item are excessive for small clutter; the listing has a 1.0 rating from one rating; it lacks desks, chairs, monitors, and keyboards, so existing template/simple custom pieces must cover large furniture.
- **Reality.exe fit:** acceptable for a very small desk/archive subset. It should provide supporting detail, never the target or focal language.
- **Stock-pack risk:** **low-medium** if only 5–8 objects are used and vendor texture variants are culled.
- **Disposition:** **MUST HAVE, conditional**, but Task 020 may defer purchase if the slice reads clearly with existing/simple furniture.

### 5. Factory Environment Collection — Denys Rutkovskyi

- **Role:** optional free supplemental/staging library.
- **Source and price:** third-party seller on [Fab — Factory Environment Collection](https://www.fab.com/listings/2ee66462-8c2b-4303-892c-83f7fc0d9b3e); **Free** during research.
- **License:** listing-specific offer `UNKNOWN`; free price does not prove license type. Acquisition remains conditional on the same Fab License Gate.
- **Format:** native Unreal Engine content.
- **UE compatibility:** exact versions and explicit 5.7 support `UNKNOWN`.
- **Size:** `UNKNOWN`, but scope indicates a high repository/download risk.
- **Contents/modularity:** a large heavy-truck factory collection with assembly, control, painting, storage, office, changing-room, track, train, vehicle, crane, drone, forklift, FX, and ambient-audio content.
- **Materials/textures:** described as optimized; counts/resolutions `UNKNOWN`.
- **Nanite:** `UNKNOWN`.
- **Collision/LODs:** the listing explicitly states optimized collision and LODs, but gives no per-mesh details.
- **Blueprint/plugin dependencies:** functional vehicles, drone, crane, forklift, FX, audio, and example environments imply a substantial dependency graph. Exact Blueprint/plugin requirements are `UNKNOWN`.
- **Customization:** isolated static meshes may be easy to override; wholesale migration would be difficult to audit and normalize.
- **Strengths:** free, broad, mature industrial vocabulary, potentially useful rails/platforms/lights/cabinets.
- **Weaknesses:** severe content and dependency bloat; truck-factory identity; functional systems are irrelevant and could collide with Reality gameplay/input; high recognizable-pack risk.
- **Reality.exe fit:** only as a clean staging project from which a handful of dependency-audited static support meshes might be migrated. Never add the full pack directly to Reality.exe.
- **Stock-pack risk:** **high** wholesale, medium for a few generic meshes.
- **Disposition:** **NICE TO HAVE / free-route source**.

## Style compatibility scores

Scores are 1 (poor) to 5 (strong). `Low stock-pack recognizability` scores high when a source can disappear into the Reality.exe language. Totals are deliberately omitted because technical, legal, and dependency Gates outweigh arithmetic.

| Candidate | Facility credibility | Reality.exe compatibility | Modularity | Customizability | Material compatibility | Gameplay readability | Low stock-pack recognizability |
|---|---:|---:|---:|---:|---:|---:|---:|
| Modern Offices | 4 | 3 | 5 | 4 | 3 | 4 | 2 |
| Building and Industrial Props | 4 | 4 | 5 | 4 | 4 | 4 | 3 |
| Modular Warehouse Props Pack | 4 | 4 | 4 | 4 | 4 | 4 | 4 |
| Office Props Pack | 4 | 4 | 2 | 3 | 3 | 4 | 4 |
| Factory Environment Collection | 5 | 3 | 4 | 3 | 3 | 4 | 2 |

The focused paid sources win despite Modern Offices' higher architecture modularity: they support the approved custom shell instead of replacing it.

## Rejected and reserve candidates

| Candidate | Decision | Reason |
|---|---|---|
| Warehouse Props — Kostya Dubov | **SKIP** | Listing was explicitly unavailable in the research region. It otherwise offered 12 low-poly warehouse meshes, FBX/OBJ and UE5 project formats, and 4K PBR textures, but regional unavailability is a hard acquisition failure. [Fab listing](https://www.fab.com/listings/68d200dd-7038-4a1e-b09c-5e4767109f9f) |
| Modern Office (Modular) | **SKIP** | Listing is organized around baked GPU Lightmass and instructs UE5 users to disable GI, conflicting with the project's dynamic Lumen baseline. [Fab listing](https://www.fab.com/listings/b6a2ad53-c7bd-4fc6-98cf-32969c39c391?lang=en) |
| Warehouse Environment | **SKIP for current slice; premium reserve** | More than 165 assets, Nanite/Lumen, 4K materials and Blueprints are capable but disproportionate; wrapped-plastic presentation references ray tracing and the complete pack would be heavy and recognizable. [Fab listing](https://www.fab.com/listings/ef0311b7-fd62-414a-b2c3-66ba95d8a21d) |
| Office Environment | **SKIP** | More than 200 high-end assets, Nanite/Lumen and 4K materials are excessive for three rooms and create performance, repository, and single-pack-identity risk. [Fab listing](https://www.fab.com/listings/443bbd9f-5e34-45c1-b832-4df40ca6dc98) |
| Quixel Warehouse | **SKIP for Task 020; free reserve** | Free and broad, but the listed UE project versions stop at 5.6, textures run to 8K, and the worn/rusty visual direction conflicts with the controlled facility unless heavily filtered. [Fab listing](https://www.fab.com/listings/a3149fab-3906-4043-b6ee-3937b752a06c) |
| Warehouse_Props_Pack — Stoneware | **Reserve** | Compact 26-mesh/three-master source with packed maps, but price, collision, engine versions, and size are `UNKNOWN`; its chemical/military tank emphasis overlaps only a small part of the Demo. [Fab listing](https://www.fab.com/listings/b8ca363a-8f2d-4cf7-91fb-95db208c9dfa) |
| Horror Office Props Pack | **SKIP** | 106 meshes, seven Blueprints, 84 textures, and a horror/abandoned presentation add needless material/Blueprint complexity and violate the art-direction filter. [Fab listing](https://www.fab.com/listings/0d13be82-33c9-4511-84cd-3d25eb40484b?lang=en) |

## UE 5.7.4 compatibility and technical acceptance

Only Modern Offices makes an explicit 5.7 statement, and even it still requires a staging inspection of the actual acquired version. The other recommended packs are geometry/material-led Unreal products with no documented code plugin, making upgrade plausible but unproven.

Epic's [UE5 migration guidance](https://dev.epicgames.com/documentation/unreal-engine/unreal-engine-5-migration-guide) says older content can generally be opened in newer engine versions while newer content cannot be opened in an older engine, and deprecated Blueprint/API content may require work. Therefore, Task 020 must not use `Add to Project` directly against the production project as the first test.

For each acquired pack:

1. Create or use a disposable **UE 5.7.4 staging project** outside the Reality repository.
2. Add the pack only to staging and record warnings, required plugins, changed project settings, shader compile behavior, and package version.
3. Inspect exact dependency graphs, asset sizes, redirectors, naming collisions, virtual textures, material domains, texture formats, collision, pivots, scale, lightmap UVs, LODs/Nanite, and Blueprint parents.
4. Reject any pack requiring an unrelated gameplay framework, input mapping, GameMode, code plugin, global render change, or deprecated system.
5. Migrate only approved assets and their minimum dependencies through Unreal's Asset Actions workflow. Epic's [Migrating Assets documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/migrating-assets-in-unreal-engine) explains that the Asset Report includes dependencies and that migration should target a project's `Content` folder.
6. Open the production project, resave converted assets, validate maps, run the Reality automation suite, and inspect packaged Development behavior before considering the source accepted.

Do not manually copy, move, rename, or delete `.uasset` files. Use the Content Browser and Unreal-aware operations, consistent with Epic's [asset workflow guidance](https://dev.epicgames.com/documentation/unreal-engine/working-with-assets-in-unreal-engine?lang=en-US).

## Material convergence and source treatment

The Demo must converge on the Task 018 masters rather than display five unrelated vendor shaders:

- `M_FacilitySurface`
- `M_PaintedMetal`
- `M_UtilitySurface`
- `M_Glass`
- `M_IndicatorScreen`
- `M_DecalFacility`

| Source | Default treatment | Rule |
|---|---|---|
| Reality-owned architecture shell | Use Reality master materials directly | Original UVs and modular dimensions are authored for the Demo. |
| Building and Industrial Props | Replace or selectively reuse | Prefer Reality-owned material instances on pipes, cabinets, frames, vents and lights. Retain a vendor material only when its packed maps or special shading materially outperform the shared master and can be parameterized non-destructively. |
| Modular Warehouse Props Pack | Light parameterization / selective replacement | Use clean source normal/ARM textures through Reality-owned instances where practical. Normalize shelf/bin/cart colors. Avoid importing all three resolutions; target 2K for most slice props, with 1K for small items. |
| Office Props Pack | Selectively keep or wrap | Keep only the minimum 1K/2K texture source needed for tiny props; do not carry 4K duplicate variants by default. Use Reality-owned instances for palette cohesion where UV/material layout permits. |
| Factory Environment Collection | Replace for any selected static mesh | The few accepted meshes should be visually absorbed into the shared material language. Do not inherit the collection's environment post process or gameplay materials. |
| Modern Offices | Selective replacement only | Do not enable virtual-texture project settings merely to preserve a nonessential vendor material. Reject any selected asset that cannot be isolated cleanly without those settings. |

Never overwrite vendor source materials or textures. A vendor update should remain inspectable, and a Reality-owned override should be removable without corrupting the source.

## Initial tiling texture sources

### Recommended: ambientCG CC0 source maps

Use a very small set of 2K JPG PBR maps as inputs to the Reality masters, with 4K only where a camera-proximate broad surface proves it necessary. ambientCG states CC0, commercial use, and no attribution requirement on its pages. Candidate sources include:

- Clean concrete: [Concrete 003](https://ambientcg.com/view?id=Concrete003), whose 2K JPG package is documented as 24 MB.
- Smooth sealed-concrete direction: [Concrete 014](https://ambientcg.com/view?id=Concrete014), 2K JPG 26 MB.
- Clean pale wall substrate: [Plaster 002](https://ambientcg.com/view?id=Plaster002), 2K JPG 25 MB.
- Neutral clean metal basis: [Metal 049 A](https://ambientcg.com/view?id=Metal049A), 2K JPG 7 MB.
- Perforated utility metal: [Sheet Metal 002](https://ambientcg.com/view?id=SheetMetal002), 2K JPG 14 MB.
- Restrained local wear/overlay: [Surface Imperfections 002](https://ambientcg.com/view?id=SurfaceImperfections002), 2K JPG 17 MB.

Painted-metal examples such as [Painted Metal 007](https://ambientcg.com/view?id=PaintedMetal007) are rust-heavy and should be reference/reserve only, not the default facility finish. No suitable rubber source was confirmed during this research; author a simple procedural rubber material or research an individual CC0 listing at acquisition time rather than relabeling plastic/fabric as rubber. Glass can start as a Reality-owned shader without a downloaded texture.

### Fab/Quixel materials

Fab currently contains both free and paid material listings; do not assume all Megascans/Fab surface assets are free today. A single free [Quixel Concrete listing](https://www.fab.com/listings/069d6a1b-6f24-47f3-a742-9d8d3e97daeb?lang=en) was observed, but its actual offer license and desired resolution must still pass the Fab Gate. Because ambientCG already provides clear CC0 sources, Quixel is **NICE TO HAVE**, not a dependency.

### Decals

Do not buy a giant decal pack. Generic scratch, smudge, and restrained grime masks can come from individually recorded CC0 sources or be authored in-house. Reality.exe-specific signage remains custom:

- Reality-managed object markers
- Developer Access and Reality Core labels
- Sensor IDs and maintenance continuity marks
- Time-cycle/reference graphics
- Reality Core Exit and corporate/system language

Generic third-party warning decals may be used only if their exact license/source is recorded and they do not replace the custom facility sign system.

## Exact import boundaries

These are maximum boundaries, not automatic import lists. Task 020 should import only assets actually selected after staging.

### Reality-owned architecture shell

**Create later:** wall, floor, ceiling, reveal, doorway, glazing frame, column, beam, baseboard/service strip, simple stair/landing, railing, and utility-light shell modules needed by Routine Intake, Observed Storage, and Security Hub.

**Do not create in Task 019:** any mesh, material, Blueprint, or map edit.

### Building and Industrial Props

**Use candidates:** one coherent pipe family and a few bends/valves; one vent/duct family; one electrical/utility cabinet family; selected conduit/wire support; one ceiling/wall utility fixture; one restrained door/frame or railing piece only if it fits the locked shell dimensions.

**Do not import:** example levels, walls/floors/roof as the Demo shell, all 41 wires, all pipe families, duplicate seamless architecture textures, unrelated doors, horror/grunge variants, or unused 4K maps.

### Modular Warehouse Props Pack

**Use candidates:** modular shelf, one pallet, pallet truck/cart, two or three cardboard/wooden box variants, one clean plastic bin/container.

**Do not import:** gas tanks, cage tanks, survival/military arrangements, stair unless independently required, every barrel color, all quality-level texture duplicates, or any showcase content.

### Office Props Pack

**Use candidates:** ring binders, clipboard, document/folder set, paper tray, bin, notebook, and at most one or two desk-detail clusters.

**Do not import:** all 60 texture variants, every pen/tool, 4K maps for tiny/background props, staged example scenes, or clutter that competes with the first editable target.

### Factory Environment Collection

**Use candidates only after need is proven:** one railing/platform family, one utility light, one generic cabinet, or one non-Hero support machine housing that cannot be produced more cheaply with the custom kit.

**Do not import:** maps, truck factory architecture, vehicles, controllable cargo lift/crane/drone/forklift, Blueprints, input, GameMode, FX, audio, train/test track, assembly line, sample gameplay, or bulk dependency folders.

### Modern Offices

**Use only as premium contingency:** at most a generic chair/desk/cabinet or isolated glazing/trim mesh that survives without project-setting changes.

**Do not import:** example level, whole building, elevator/stairs, kitchen/cafeteria/toilet sets, poster images, Virtual Texture dependencies, or vendor architecture as the Demo layout.

## Content folder, naming, and ownership plan

Respect the repository's established top-level folders. Do not create an unapproved random top-level `Environment` folder. Proposed Task 020 paths:

```text
Content/
  Reality/
    Environment/
      Architecture/
      Materials/
        Masters/
        Instances/
      Decals/
      Hero/
      Blueprints/
  Props/
    Office/
    Storage/
    Maintenance/
    Security/
  ThirdParty/
    32cm_BuildingIndustrial/
    DanieleCalvi_ModularWarehouse/
    32cm_OfficeProps/
    DenysRutkovskyi_Factory/       # only if selected
    Dexsoft_ModernOffices/         # only if premium contingency is selected
    ambientCG/
      Textures/
```

`ThirdParty/<VendorPack>/` is the immutable provenance boundary for approved source meshes, source materials, and their required textures. Reality-owned derived assets belong in `Reality/Environment` or the appropriate existing `Props` category.

Naming rules:

- Preserve vendor source asset names inside the third-party boundary unless an import collision makes a non-destructive namespaced duplicate necessary.
- Prefix source directories with a stable vendor/pack slug; do not scatter vendor dependencies across production folders.
- Reality-owned meshes: `SM_RLT_<Function>_<Variant>`.
- Reality-owned master materials: the six approved names; derived instances: `MI_RLT_<SurfaceOrAsset>_<Variant>`.
- Reality-owned textures/masks: `T_RLT_<Purpose>_<MapType>`.
- Reality-owned decals: `M_DecalFacility`, `MI_RLT_Decal_<Purpose>`, and `T_RLT_Decal_<Purpose>_<MapType>`.
- Reality-owned wrappers: `BP_RLT_<Function>`. A wrapper contains only presentation/assembly behavior; existing core Reality capability components remain the systemic authority.
- Modified mesh duplicate, only when override/wrapper cannot work: `SM_RLT_<Function>_From_<VendorSlug>`, with provenance in its asset description/register.
- Do not rename, overwrite, reparent, or edit third-party source assets destructively. Use material overrides, instances, wrappers, child Blueprints, or explicitly documented duplicates.

## Repository-size controls

The principal risk is texture/dependency weight, not mesh triangle count. Three focused packs still advertise many 4K maps; Modern Offices and Factory Environment Collection can pull whole example environments or functional systems through dependencies.

Controls for Task 020:

1. Measure downloaded pack size in staging and migrated on-disk size before production commit.
2. Use Unreal Reference Viewer/Size Map and the migration Asset Report before moving anything.
3. Import only 1K/2K maps for small/background props; approve 4K individually for close hero-adjacent surfaces. Do not import 8K maps.
4. Do not keep alternate engine/project versions, FBX duplicates, source archives, demo maps, screenshots, documentation media, or unused texture sets inside `Content`.
5. Run `git status` before and after each source migration so unexpected dependencies are attributable.
6. Make one reviewable import commit per approved pack or small coherent batch.
7. Set a provisional Task 020 vertical-slice **soft budget of 500 MB added repository size** and a **hard review Gate at 1 GB**. These are production controls, not claims about current pack sizes.

Git LFS may become necessary as the binary asset set grows, but Task 019 must not change Git configuration. Before enabling it, measure current Git history/repository hosting limits, verify every collaborator/build machine has LFS, define `.uasset`/`.umap`/large-texture policy carefully, and migrate history only with explicit approval. Selective imports should be attempted first.

## License provenance record

Create `Docs/THIRD_PARTY_ASSET_REGISTER.md` in Task 020 only when the first external asset is actually acquired. One row per acquisition/source should include:

- Pack and exact listing/version
- Publisher/legal seller name
- Canonical source URL and listing ID
- Acquisition date, price, currency, tax, and receipt/order ID
- Acquiring account/organization owner
- Exact license name/version/tier and saved evidence path
- Commercial packaged-project and private-repository notes
- Attribution or special restrictions
- Original download/package version and supported UE versions
- Imported source paths and Reality-owned derived paths
- Actual imported asset list/purpose and excluded content
- Any third-party materials embedded by the publisher
- Staging/build/validation notes
- Removal/replacement history

Store receipts/license evidence in a restricted team records location rather than committing personal or billing information. The repository register should point to the record without exposing secrets.

## Custom Hero budget remains locked

The following remain original Reality.exe work and must not be replaced by purchased Hero models:

1. Developer Access / Reality Core Terminal
2. SensorFailure Control Assembly
3. Security Camera / Sensor
4. Security Rotor / Time Machinery

Library pipes, brackets, fasteners, housings, rails, cables, or generic lenses may support these assets only if the final silhouette, interaction surface, state language, labels, and authored assembly remain Reality-owned and provenance is recorded.

## Purchase classification

### MUST HAVE — small conditional set

- **Building and Industrial Props** — US$19.99; conditional on license, staging, collision, and UE 5.7.4 checks.
- **Modular Warehouse Props Pack** — US$11.99; conditional on license, staging, and dependency checks.
- **Office Props Pack** — US$11.99; conditional on license/staging and only if existing/simple office props cannot satisfy the slice.
- **Selected ambientCG CC0 texture maps** — free; use only the named/approved maps and resolutions.

The Fab items are acquisition candidates, not preapproved expenditures. Office Props Pack can be postponed after the initial slice blockout, reducing initial paid spend to **US$31.98**.

### NICE TO HAVE

- **Factory Environment Collection** — free; isolated staging source for at most a few static meshes.
- **Modern Offices** — US$29.99; premium contingency, not the shell.
- A single verified Fab/Quixel surface only if it clearly outperforms the CC0 material inputs.

### SKIP

- Regionally unavailable Warehouse Props.
- Baked-GI Modern Office (Modular).
- Whole Warehouse Environment or Office Environment packs for the first slice.
- Horror Office Props Pack.
- Military/sci-fi/post-apocalyptic warehouse collections.
- AI-generated giant logistics packs or sources with unclear provenance.
- Giant decal libraries and any pack intended to replace the four custom Heroes.

## Budget options

Prices are current displayed US-dollar values before tax and may change.

### A. Free / minimal cost — US$0

- Reality-owned architecture shell.
- Selected CC0 ambientCG maps.
- Conditional free Factory Environment Collection, staged externally and migrated only for a handful of static props.
- Reuse existing project meshes/simple custom blockout props for office and storage.

**Quality:** coherent if the custom shell/material pass is strong, but prop variety may remain visibly prototype-level.

**Customization burden:** high; extensive filtering and Reality-owned material work.

**Download/repository burden:** potentially high if the Factory collection is handled carelessly; low if only a few dependencies are migrated.

### B. Balanced indie — US$43.97 recommended

- Building and Industrial Props: US$19.99.
- Modular Warehouse Props Pack: US$11.99.
- Office Props Pack: US$11.99.
- Free selected ambientCG maps.
- Optional free Factory collection only when a missing mesh is proven.

**Quality:** good functional prop vocabulary while the custom shell and Hero work preserve identity.

**Customization burden:** moderate; material overrides, palette unification, and selective culling are still required.

**Download/repository burden:** moderate and controllable with 1K/2K textures and exact migration boundaries.

**Recommendation:** best balance of speed, cohesion, legal traceability, and low single-pack recognizability.

### C. Premium but still reasonable — US$73.96

- Balanced set: US$43.97.
- Modern Offices: US$29.99 as a contingency source for a few isolated corporate/furniture meshes.
- Free selected texture/supplemental sources.

**Quality:** potentially higher corporate dressing variety, but not automatically more distinctive.

**Customization burden:** moderate-high due to virtual-texture requirements and extra vendor material normalization.

**Download/repository burden:** high relative to a three-room slice.

**Decision:** not recommended until the balanced vertical slice demonstrates a specific gap.

## Task 020 vertical-slice input

Task 020 must art-pass only this representative sequence:

**Routine Intake → Observed Storage → Security Hub**

It must not continue into the Maintenance Loop, Logistics Well, optional Physics alcove, Desynchronization Gate, or exit art pass before approval.

### Preconditions

1. Reconfirm the three selected Fab listing prices/licenses and acquire only approved offers.
2. Create the provenance register and stage each pack in UE 5.7.4 outside the repository.
3. Present measured pack/import sizes and proposed exact asset names before migration.
4. Preserve a before-pass copy/branch of the approved Demo map through normal source control; do not move existing assets.

### Slice deliverables

- A Reality-owned modular shell that preserves the graybox's transforms, gameplay clearances, routes, occlusion, and sightlines.
- First-pass shared masters/instances for facility surface, painted metal, utility surface, glass, indicator screen, and facility decals.
- Routine Intake: restrained corporate finish and a minimal office subset; first editable obstacle remains the focal object.
- Observed Storage: one shelf/pallet/cart/box family; clear scale references; Human Witness silhouette, facing, and line of sight remain legible.
- Security Hub: maintenance source pipes/vent/cabinet/utility fixture; custom Security Camera Hero prototype and coverage remain readable; direct route and maintenance branch hierarchy stay intact.
- Custom temporary/final-authoring decals for room IDs, asset IDs, observation/security state, and route function—never solution instructions.
- Reality-owned material instances/wrappers separated from immutable third-party source folders.

### Proof Gate

Task 020 should prove:

1. Architecture replacement does not alter traversal, E interaction, Reality targeting, or puzzle solutions.
2. The shared material language visually absorbs props from multiple sources.
3. Editable targets read above background detail in normal and Developer modes.
4. Human and Camera Witness locations/facing/occlusion remain understandable.
5. Lighting establishes Routine → Storage → Security hierarchy while preserving Lit-mode gameplay readability.
6. The staging/selective-migration workflow produces no unrelated Blueprint, plugin, config, GameMode, input, demo-level, or giant texture dependency.
7. The added repository size stays within the reviewed budget and the provenance record accounts for every imported source.

Validation should include map load, full Reality automation, Development Editor build, a complete PIE regression through the slice, reference/redirector checks, `git diff --check`, and a source-control inventory. No full-Demo art pass should begin until the slice passes manual review.

## Final recommendation

Approve the **balanced indie plan, capped at US$43.97 before tax**, but execute it in two acquisition decisions:

1. First approve Building and Industrial Props plus Modular Warehouse Props Pack (**US$31.98**) and the free CC0 material inputs.
2. Add Office Props Pack (**US$11.99**) only after the Routine Intake blockout proves that a focused clutter source is needed.

Keep Factory Environment Collection as a free, externally staged fallback and Modern Offices as an unpurchased premium contingency. This preserves the custom shell, shared material language, four custom Heroes, locked gameplay layout, and a low stock-pack signature while giving Task 020 enough production-ready support geometry to test the actual art pipeline.

No external asset is legally or technically cleared until its acquisition-day license evidence and UE 5.7.4 staging result are recorded.
