# Reality.exe Public Demo Art Direction and Environment Production Plan

## Status and scope

This document defines the visual target and production plan for replacing the approved Task 017 public Demo graybox. It treats the gameplay layout, routes, target capabilities, Witness placement, SensorFailure loop, and Time finale as locked design constraints.

The target is a distinctive, readable indie environment assembled from a disciplined modular kit, compatible library assets, shared materials, selective custom hard-surface work, and a small hero-asset budget. It is not an authorization to acquire assets, remodel the level, change gameplay, or pursue AAA photorealism.

## 1. Visual pillars

### 1. Believable facility first

The space initially reads as a mundane corporate testing and logistics facility: economical construction, repeated standards, service access, asset labels, and equipment installed for recognizable operational reasons. The software premise should emerge from details and behavior rather than announce itself as a neon science-fiction set.

### 2. Rules made physical

Architecture and props explain what the facility expects to happen. Door tracks, load ratings, camera mounts, floor lanes, containment rails, machine guards, and maintenance access make normal behavior legible. Reality edits become striking because the legitimate baseline is clear.

### 3. Quiet computational unease

Reality.exe identity appears through serialized markings, diagnostic typography, alignment ticks, suspiciously exact repetition, and occasional impossible seams or registration errors. These elements remain subtle in the normal world and become explicit in Developer Mode.

### 4. Controlled hierarchy over visual density

Objective, obstacle, relevant object, Witness, route branch, and destination must survive every art pass. Detail clusters support room function and leave visual rest around gameplay targets. Lighting and signage establish hierarchy without waypoint spam.

### 5. Instability as loss of certainty

Increasing Reality pressure should eventually affect consistency rather than simply add glow: slight material discontinuities, duplicate markings, alignment drift, intermittent diagnostic artifacts, or incorrect reflections. Task 018 defines this direction only; no state-driven effects are implemented here.

## 2. Demo visual identity

### Overall mood

The facility is clean enough to imply corporate control but old enough to show adaptation, repairs, and budget decisions. It should feel recently occupied, operational, and faintly over-instrumented. The emotional progression moves from orderly intake, through observed and serviced infrastructure, into large industrial systems, then culminates in a machine space whose timing feels unnaturally exact.

### Material language

- Warm-white or pale gray painted wall panels establish the corporate shell.
- Sealed concrete and durable resin floors provide a sober base.
- Charcoal structural steel frames doors, machinery, rails, and service openings.
- Desaturated painted metal distinguishes equipment families without turning rooms into color-coded puzzles.
- Rubber, glass, cable insulation, and molded plastic appear only where function calls for them.
- Wear accumulates at contact, transport, maintenance, and water paths; broad random grunge is avoided.

### Architectural and shape language

- Architecture uses rectilinear, economical modules with visible joints and rational service zones.
- Normal facility geometry favors broad planes, shallow reveals, chamfered safety edges, and repeated 1-m increments.
- Operational equipment uses nested frames and protected cores: an outer housing, readable working part, then service access.
- Reality-specific devices introduce precise split lines, paired brackets, registration marks, and small impossible gaps. They do not adopt a separate alien silhouette language.

### Lighting philosophy

Lighting is motivated by installed fixtures and gameplay function. Neutral ambient readability is preserved throughout. Controlled contrast indicates route priority, observation pressure, maintenance access, machinery cadence, and exit hierarchy. Darkness never conceals a critical target or Witness.

The project currently uses dynamic Lumen GI/reflections and no static lighting. Art production should preserve that baseline while avoiding dependence on hardware ray tracing for basic readability.

### UI in the world

World information uses plausible operational artifacts: room signs, asset plates, equipment indicators, floor lanes, and small screens. A sign names a function or state, not a solution. Numeric evidence and complete capability lists remain in the Developer Console rather than being painted throughout the environment.

### Color principles

The base environment remains neutral. A limited facility warning color marks physical hazard or restricted access; a cool system accent marks legitimate interaction and diagnostics; Developer Mode uses a stronger cool accent; Reality instability uses a separate sparse anomaly accent. Shape, placement, motion, text, and value contrast must carry meaning when color is unavailable.

## 3. Zone visual identities

| Zone | Function and architectural character | Materials and prop families | Lighting and signage | Gameplay readability and distinction |
|---|---|---|---|---|
| Routine Intake | Public-facing staff intake transitioning into controlled service access. Lower ceiling, finished wall panels, reception glazing, orderly door portal. | Painted panels, resin floor, glass, laminate desk, chairs, monitor, access reader, small cabinets. | Even neutral office fixtures; exit airlock slightly brighter. Room name, intake direction, access-status plate. | Quiet background around the first editable airlock; ordinary interactables establish the believable baseline. Most finished and least industrial zone. |
| Observed Storage | Compact storage room with a visibly engineered bulkhead and raised service opening. Shelving frames the problem without hiding it. | Sealed concrete, painted block/panels, steel shelving, pallets, standardized load crates, fixed cover. | Cooler utility light with a clear pool on the bulkhead and step; Witness side receives enough key light to read facing. Storage bay, load limit, service-opening markings. | Scale targets share dimensions and markings with nearby normal storage objects. Human silhouette, head direction, and work position clearly establish attention. Denser than Intake but keeps the puzzle axis open. |
| Security Corridor Hub | Deliberate security threshold with a strong central door portal and visible camera mount; maintenance branch reads as subordinate but accessible. | Darker metal door frame, reinforced wall panels, security glazing, bollards, cable trays, access cabinet. | Higher contrast and controlled pools; camera lens/status remains visible. Direct access and maintenance diagnostics signs use operational wording. | Door, camera, cover, and maintenance entrance are readable in one view. Strongest surveillance character; no decorative clutter near LOS decisions. |
| Maintenance Loop | Narrow back-of-house service route exposing the systems behind Security. Lower finish quality, surface-mounted utilities, reachable panels. | Concrete/painted masonry, galvanized trays, pipes, conduits, rubber floor mats, electrical cabinets, cable labels. | Warmer utility fixtures, localized task light at SensorFailure assembly, believable warning indication during failure. | Cable/identifier continuity links the panel back to the Security camera/door. Less polished and more mechanically dense, but the return route stays obvious. |
| Logistics Well | Tall industrial transfer volume organized around elevation, load movement, containment, and the upper deck. | Concrete, steel deck, galvanized rails, painted lifting/transfer equipment, pallets, load props, dock bumpers. | Large overhead pools plus focused light on props, landing, and upper destination. Floor lanes, load ratings, upper-deck identifier. | Strong shadows/contact cues explain physics. The recovery route is visually secondary. Largest vertical scale and broadest spatial rhythm. |
| Physics Service Alcove | Optional instrumented transfer-test bay nested into Logistics, visibly nonessential. | Reuses Logistics shell with rubber strips, replaceable test surface, measurement marks, small control cabinet. | Focused work light and restrained optional-branch accent. Test-lane and mass/load markings rather than tutorial prose. | Straight motion lane, force origin, outcome zone, and retained objects make Mass/Friction effects visible. Compact and deliberately lower hierarchy than the main route. |
| Desynchronization Gate / Exit | Purpose-built security machinery channel leading to the Reality core terminal. Repetition and precise alignment become unusually prominent. | Dark machine housings, guarded steel, durable floor, observation glazing, clean core-terminal surfaces. | Rhythmic fixture/marker sequence makes cadence readable; exit is the clearest destination value. Scanner, baseline reference, service shortcut, and core exit identifiers. | Reference and editable machines are visually paired. Moving arms, directional markers, safe waiting pockets, camera direction, and final terminal form one readable sequence. Most computational and visually intense zone without becoming neon. |

All seven zones reuse the same wall proportions, structural metal, sign grid, typography, fasteners, and facility asset-number convention. Zone identity comes from finish level, equipment density, ceiling/volume, lighting rhythm, and prop family rather than unrelated themes.

## 4. Gameplay readability rules

### Reality-editable targets

- Look like legitimate installed equipment or standardized movable assets in the normal world.
- Carry one small, consistent serialized diagnostic plate at a predictable height or corner.
- Preserve a clean silhouette and a low-clutter halo around the selectable face.
- Use capability-appropriate physical cues: door tracks for Collision, dimensional reference markings for Scale, lifting/containment hardware for Gravity, load plates for Mass, contact surface/lane for Friction, cycle markers for Time.

### Non-editable architecture

- Uses continuous construction systems, repeated structural joints, and no diagnostic plate.
- Structural walls, floors, ceilings, rails, and outer shells must never borrow the editable accent treatment.
- Visual continuity should make it implausible that one arbitrary wall panel is a gameplay target.

### Witnesses and observation

- Camera body, lens hood, mount, cable direction, status light, and physical aim all agree.
- Human posture, head orientation, task surface, and illumination communicate attention naturally.
- Cover looks solid and matches collision; decorative open frames are not used as ambiguous LOS blockers.

### Context and interaction

- Context machinery has a clear normal operating state, a readable control point, and a visibly related equipment network.
- Interactive panels share handle/button/screen placement and a modest interaction indicator.
- Interaction affordance is distinct from Reality editability: a panel can be operated with E without implying editable physics properties.

### Property-specific readability

- Scale props sit beside known-size shelves, openings, rails, or floor grids.
- Gravity props have grounded shadows, lift points, containment, overhead clearance, and an obvious place to reposition them.
- Mass/Friction props occupy a marked motion lane with a visible push source and stopping/arrival reference.
- Time machinery exposes its full repeating path before commitment and retains a nearby baseline reference.
- Exit uses unique destination framing and a stable interaction surface; it never carries editable markings.

## 5. Reality-editable visual language

### Preferred approach: serialized object plate plus Developer Mode registration overlay

The final replacement for the cyan graybox label is a two-layer system.

**Normal world:** eligible assets carry a restrained manufacturer-style plate integrated into the object: a small split-square glyph, serialized `R-X` asset number, registration ticks, and one cool-white status pip. The mark says that the object participates in managed facility systems without exposing capabilities or glowing across the room. It appears on doors, props, and machinery in a consistent mounting zone.

**Developer Mode:** the plate becomes the anchor for a stronger diagnostic overlay: silhouette outline, corner brackets, sparse surface registration lines, target name, capability tags, and current/restorable values in the Console. Only the focused target receives full metadata; other nearby eligible objects receive at most a faint plate/edge response.

This approach is preferred because it remains believable, survives varied asset sources, is inexpensive to add through a decal/material layer, and avoids making every target emissive. It can replace the graybox label incrementally without changing eligibility logic.

Collision OFF should retain the object's normal material and silhouette, adding only a subtle Developer Mode surface discontinuity or dashed boundary. Scale should preserve plate size only if implemented as a separate overlay/decals; the world object itself still scales. No final behavior is authorized in Task 018.

## 6. Developer Mode art language

Developer Mode should feel like a maintenance visualization rendered over reality, not a different fantasy world.

- Cool, high-value accent reserved for selection and diagnostic geometry.
- One-pixel-style outlines and corner brackets rather than thick halos.
- Sparse wireframe fragments only at bounds, pivots, collision surfaces, or changed regions.
- Local axis ticks and measurement marks appear only when relevant to the selected property.
- Terminal typography uses a compact monospaced family with clear hierarchy and generous spacing.
- Screen-space treatment is limited to a subtle exposure shift, edge registration, and brief activation scan; no persistent heavy scanlines.
- Property state uses icon plus text plus geometry, never color alone.
- World readability, Witness facing, and target silhouette remain clearer with the mode active than without it.

Avoid Matrix rain, full-scene wireframe, constant glitching, strong chromatic aberration, rapid noise, and large opaque panels. The existing Console remains the information authority until a separately approved presentation task.

## 7. Witness readability

### Security Camera

- Use a directional housing with a longer lens axis than body width.
- Mount it where its facing direction can be seen before entering coverage.
- Add a small active indicator on the lens side and a matching facility sensor ID.
- A restrained projected gobo, floor scan line, or moving head marker may suggest the watched direction where justified, but normal gameplay should not show a giant LOS cone.
- SensorFailure changes the indicator and associated security fixtures consistently; the physical camera aim remains unchanged.
- Developer Mode may later show approximate range/FOV boundaries and evidence state.

### Human Witness

- Use readable head/torso orientation, a task pose, and a lit work area.
- Avoid idle poses that point feet, torso, and head in conflicting directions.
- Keep the face or profile visible from the decision point.
- Use composition and cover edges to explain safe versus watched positions; do not rely on a floating `WITNESS` label in final presentation.

## 8. SensorFailure presentation

### Normal

- Security sensor cabinet shows a stable status sequence, readable sensor ID, clean green/neutral indicator, and quiet regular equipment sound.
- Cable trays or repeated identifiers visually link it to the Hub camera and security door.
- Camera status and local security fixtures appear synchronized and healthy.

### Failure Active

- Cabinet status changes to an amber fault code with an irregular but restrained indicator pattern.
- One associated security light or camera status indicator changes state, proving the failure affects a real network.
- Screen content reports signal loss, calibration failure, or invalid sensor data—not Suspicion reduction.
- Later audio/VFX should add a relay dropout, interrupted cadence, and short diagnostic tone rather than an arcade power-up effect.

The state presentation must not claim that Witnesses are disabled or that Time is explained. It represents only the existing SensorFailure Context evidence used by current rules.

## 9. Time machinery presentation

The final mechanism should read as a security scanner with a protected drive housing, rotating/sweeping arm, marked travel path, safe waiting pocket, and a clear crossing threshold.

- Moving components use asymmetric silhouettes so direction and phase are visible.
- Alternating physical markers on the arm and housing make angular speed readable without motion blur.
- A restrained chase light or mechanical index lamp follows the cycle but does not replace actual motion.
- Hazard stripes mark the swept volume at floor/rail level.
- The baseline reference uses the same motor family, arm length, marker spacing, and nominal cadence as the editable target.
- The reference is physically adjacent but outside the required path, so comparison is immediate.
- The editable machine carries the serialized Reality plate; the reference does not.
- At 0.25x the arm and index lamps visibly linger; at 4x the cadence visibly compresses. Particle trails and global post-process slow motion are inappropriate.

The housing and guard geometry must keep the selected Actor targetable at every preset and make the local nature of the effect obvious.

## 10. Minimum material library

Use a small number of Substrate-compatible masters where practical, then instances by zone and asset family.

| Family | Minimum instances | Production method |
|---|---|---|
| Architecture | Painted wall/panel, sealed concrete floor, ceiling acoustic/utility panel, structural concrete, charcoal structural metal | Tiling base textures; color/roughness parameters; optional macro mask and vertex/mask variation for broad surfaces. |
| Industrial | Painted metal, raw steel, galvanized utility metal, rubber, molded plastic, safety glass | Shared detail normals and packed masks; painted metal supports edge/wear mask without procedural over-weathering. |
| Detail | Indicator emissive, monitor/screen, warning stripe, diagnostic plate, cable/label surface | Parameterized colors/intensity; atlas or trim sheet for labels/stripes; screen content via small shared atlas. |
| Condition | Clean, handled, service-worn, localized damaged/dirty | Prefer decals and mask parameters over separate full texture sets. Damage remains localized and function-driven. |

Recommended masters:

1. `M_FacilitySurface` for walls, concrete, floors, and ceilings.
2. `M_PaintedMetal` for doors, machines, rails, cabinets, and props.
3. `M_UtilitySurface` for rubber, plastic, galvanized/raw metal variants.
4. `M_Glass` with minimal translucency and an opaque/masked fallback where possible.
5. `M_IndicatorScreen` for emissive indicators and simple displays.
6. `M_DecalFacility` for signage, IDs, wear, and Reality-specific overlays.

Do not create a master per room or a unique material per prop. Parameter names, channel packing, texel density, and instance naming should be standardized before broad replacement.

## 11. Decal library

### Generic facility decals

- Room and bay numbers
- Directional arrows and route names
- Restricted-access and staff-only markings
- Floor lanes, stop lines, load zones, and pallet footprints
- Warning stripes and electrical/mechanical hazards
- Maintenance inspection labels and service dates
- Security camera/sensor identifiers
- Electrical cabinet and cable-run identifiers
- Small contact grime, wheel scuffs, edge handling, leaks, and repair patches

### Custom Reality.exe decals

- Facility corporate wordmark and restrained system glyph family
- Reality-managed serialized object plate
- Developer access authorization mark
- Sensor network/failure identifiers
- Security rotor cycle/index graphics
- Reality core exit identity
- Sparse registration errors or duplicate asset IDs for later instability presentation

Decals should cluster at decisions, equipment, and plausible wear points. Large blank planes are useful visual rest and should not be filled automatically.

## 12. Modular environment kit

Use a 1-m base grid consistent with the existing 100-cm UE cube basis. Preserve the approved graybox clearances by snapping replacement surfaces to the graybox envelope rather than reinterpreting room dimensions.

### Core modules

- Walls: 1 m, 2 m, and 4 m widths; 3 m nominal height; solid, service-panel, window, and utility variants
- Corners: internal/external 90-degree pieces and simple end caps
- Floors/ceilings: 1 m, 2 m, and 4 m squares; clean corporate and industrial/service variants
- Door frames: 1 m single/service, 2 m secure/bulkhead, and wide logistics opening
- Door panels: normal, security, maintenance, and editable-compatible faces using shared frame dimensions
- Structural: 0.25/0.5 m beams, columns, shallow pilasters, deck supports
- Traversal: railings in 1 m/2 m sections, corners, gates, stairs, landings, ramps, raised-deck edges
- Service openings: low/high pass-through frames, sill, vent/service hatch surrounds
- Utilities: vents, cable tray straights/corners/T-junctions, pipe brackets, conduit runs, maintenance panel recesses
- Finishing: base trim, ceiling trim, wall reveals, threshold plates, bumper strips

### Kit rules

- Collision is simple, explicit, and matches visible solids.
- Pivot and snap conventions are identical across packs and custom modules.
- Door and service-opening modules preserve target reacquisition and authored alternate routes.
- The kit supports corporate-facing and back-of-house finishes through materials and add-on layers, not duplicated geometry sets.
- Graybox remains available in a comparison layer/map copy until every replacement is verified; Task 018 does not create that copy.

## 13. Generic versus custom assets

### Generic/library candidates

- Desks, task chairs, visitor chairs, monitors, keyboards, and office cabinets
- Storage shelves, pallets, barrels, totes, standard crates, carts, and safety bollards
- Pipes, valves, vents, cable trays, conduit, cables, ducts, and generic electrical boxes
- Industrial and office light fixtures
- Server racks and generic control cabinets
- Railings, stairs, ladders, structural beams, and basic modular shell pieces if their dimensions fit
- Generic Human placeholder or appropriately licensed worker character

### Custom/simple candidates

- Door face inserts and mounting adapters that fit locked gameplay bounds
- Reality-managed object plate, glyph plate, and capability-neutral diagnostic hardware
- Facility signage frames, sensor-ID plates, and Reality.exe decal atlas
- Scale/Gravity load props if library crates cannot provide clean silhouettes and correct pivots
- Test-lane fixtures and Time cycle floor markers

### Custom/hero candidates

- Developer access / Reality core terminal family
- SensorFailure control assembly
- Security camera / sensor housing family
- Security Rotor / Time machinery family

Generic clutter should not become custom merely to appear original. Identity is concentrated in the four hero families and the shared marking system.

## 14. Hero asset budget

Limit the Demo to four custom hero families.

| Hero family | Gameplay and visual purpose | Complexity | Primitive construction | Unique textures? |
|---|---|---|---|---|
| Developer access + Reality core terminals | Frames first authorization and final completion; bookends the Demo identity. | Medium | Yes: panels, bevelled frame, recessed screen, cable/stand variants. | One shared small screen/label atlas; shell should use shared painted-metal materials. |
| SensorFailure assembly | Makes Context a believable security-system failure with readable normal/fault states. | Medium | Yes: cabinet, relay modules, indicators, conduit, small screen. | No full unique PBR set; custom screen/status atlas and decals are sufficient. |
| Security camera/sensor family | Makes Witness facing and network state readable throughout. | Low–medium | Yes: mount, directional body, lens hood, indicator, cable. | No; shared metal/plastic/glass plus custom ID decal. |
| Security Rotor / Time machinery | Carries the late Demo reveal and local-time comparison. | Medium–high | Yes: motor housing, bearings, arm, guard, index lights, service panel. | Selective unique trim/label atlas only; broad surfaces use shared masters. |

No sculpt-heavy asset is required. These families should be designed around animation pivots, collision, selection bounds, state indicators, and variant reuse before surface detail.

## 15. 3D modeling strategy

### Tier A — licensed library assets

Use for modular shell candidates, furniture, shelves, logistics clutter, pipes, vents, lights, generic electrical equipment, and non-hero workplace props. Adapt materials and collision; do not inherit unrelated Blueprint/gameplay frameworks.

### Tier B — simple custom hard surface

Use boxes, cylinders, bevels, booleans, mirrored parts, simple weighted normals, and straightforward UVs for door inserts, signage frames, object plates, test fixtures, mounting adapters, bumpers, load props, service-opening trims, and route-specific guards.

### Tier C — hero custom assets

Reserve for the four hero families. Build modular variants from shared parts, prioritize readable silhouette and state changes, and reuse the Demo material/trim library. High-poly sculpting, complex organic modeling, and character-art specialization are out of scope.

## 16. Texture strategy

- Establish one texel-density target for first-person environment surfaces and a lower target for distant/ceiling elements.
- Use tiling architecture materials for all large surfaces.
- Use one industrial trim sheet for edges, rails, machine frames, door tracks, and repeated hardware where useful.
- Reuse shared detail normals for painted metal, fine plastic, and utility metal.
- Pack wear, material selection, and dirt masks to support instances without multiplying texture sets.
- Use decals for IDs, hazards, floor lanes, localized wear, repairs, and Reality-specific language.
- Keep monitor/status graphics in one or two small atlases.
- Allow bespoke texture content only for terminal screens, SensorFailure status graphics, security IDs, rotor index markings, and the Reality.exe corporate/diagnostic atlas.

Library asset textures may be retained when technically sound, but their exposed colors/roughness should be normalized through material instances so the Demo does not look assembled from unrelated packs.

## 17. Prop-dressing strategy

| Zone | Density | Dressing rule |
|---|---|---|
| Routine Intake | Low–medium | Orderly furniture clusters at edges; keep airlock and F6 discovery sightline clear. |
| Observed Storage | Medium–high at perimeter | Shelves and pallets communicate function and create intentional cover; puzzle bulkhead, Scale targets, Human, and both approaches retain visual breathing room. |
| Security Hub | Low | Only security-relevant props, bollards, cabinet, and controlled cover. Preserve clean LOS and maintenance-branch visibility. |
| Maintenance Loop | Medium–high | Utilities layer walls/ceiling, leaving floor circulation and SensorFailure interaction face clear. Repeated IDs link systems. |
| Logistics Well | Medium | Large props in bounded clusters; preserve physical trajectories, landing visibility, rails, and recovery path collision. |
| Physics Alcove | Low–medium | Measurement equipment and contained test props define a clean experiment lane; avoid incidental physics clutter. |
| Time Gate / Exit | Low | Machinery, guards, camera, and terminal dominate. Decorative clutter must not obscure motion phase, safe waiting pockets, or shortcut risk. |

Every prop receives one of four purposes: function, navigation, LOS/cover, or narrative evidence. Props without a purpose should be omitted. Decorative collision defaults to simple or disabled where contact is unnecessary, and dressing must be regression-tested against all authored Scale presets and physics paths.

## 18. Lighting direction

- **Routine Intake:** soft neutral overhead panels, high facial/object readability, modest destination emphasis on the airlock.
- **Observed Storage:** cooler utility ambience with a defined Witness work light and separate highlight on the bulkhead/service opening. Cover must read by value as well as silhouette.
- **Security Hub:** controlled contrast, visible camera lens/status, brighter secure doorway, warmer subordinate maintenance entry.
- **Maintenance Loop:** practical wall/ceiling utility fixtures and task light at the sensor cabinet. Failure state may alter existing indicators or one linked fixture, not plunge the space into darkness.
- **Logistics Well:** broad high-bay pools, strong contact shadows, upper-deck destination light, and enough fill to track airborne props.
- **Physics Alcove:** focused service-bench/test-lane lighting that makes speed and stopping distance legible.
- **Time Finale:** strongest rhythmic composition; repeated fixtures/index lights support cadence while the exit remains a stable visual anchor beyond the motion.
- **Exit:** clean, calm destination light that is visually separate from warning and Developer Mode accents.

Prefer a small number of meaningful movable/stationary dynamic fixtures, emissive materials that do not pretend to light large spaces, and local fill only where Lumen readability requires it. Validate exposure when moving between zones; avoid large brightness shocks.

## 19. Color hierarchy

1. **Neutral facility base:** warm white, pale gray, concrete, charcoal, and desaturated metal colors.
2. **Safety/security:** one warm warning family for hazard, restricted access, and active surveillance—not generic interaction.
3. **Operational system:** restrained cool-white/cool accent for valid controls, asset IDs, and healthy diagnostics.
4. **Developer Mode:** a clearer cyan/blue diagnostic accent used only while the mode is active or on tiny normal-world status pips.
5. **Reality anomaly:** a sparse contrasting accent reserved for inconsistency/instability, not assigned one color per ability.

Ability identity should be communicated by icon, label, geometry, and state. Do not create six saturated property colors or depend on red/green alone for camera and SensorFailure state.

## 20. Later sound and VFX dependencies

Task 018 does not implement these dependencies.

- Developer Mode: short activation/deactivation scan, focused-target registration sound, restrained outline/metadata reveal.
- Apply/Restore: one shared systemic transient plus property-specific physical response; Restore sounds like normalization, not history erasure.
- Witness observation: subtle directional acknowledgement and Developer readout cue, not an alarm for every event.
- SensorFailure: relay dropout, interrupted equipment loop, fault indicator sequence, and linked camera/security status response.
- Time machinery: mechanically synchronized baseline motor/index cadence whose pitch/rhythm follows Actor-local time naturally.
- Exit completion: stable system confirmation, terminal state change, and restrained completion emphasis.

Effects should be attached to systemic state/events so reused assets respond consistently rather than being scripted per room.

## 21. Performance budget

- Reuse modular geometry and instances; avoid dozens of tiny unique Static Mesh assets that could be modules or decals.
- Target a small shared material set and instances; monitor material-slot count on library assets and merge unnecessary slots during preparation.
- Prefer opaque/masked materials. Reserve translucency for limited glass/screens where it materially helps.
- Use decals selectively at decisions and wear points; atlas repeated signs and avoid stacked full-room decals.
- Keep large numbers of movable shadow-casting lights out of dressing. Use Lumen-compatible motivated fixtures and validate scalability settings.
- Use Nanite for sufficiently detailed static environment/library meshes where it simplifies LOD management, not for tiny simple primitives or moving gameplay mechanisms without need.
- Keep hero textures selective; avoid multiple unique 4K sets for small cabinets, signs, or props.
- Preserve simple custom collision on gameplay actors and verify library collision rather than relying blindly on complex-as-simple.
- Avoid making hardware ray tracing a readability requirement despite current project support.
- Profile the dressed Demo at the representative target hardware after architecture, dressing, and lighting passes; optimization should follow measured cost.

## 22. Task 019 external asset search specification

Do not acquire assets until licensing, technical compatibility, and fit are reviewed.

| Search category | Required style and quality | Modularity / UE requirements | Avoid |
|---|---|---|---|
| Modular office/laboratory | Restrained contemporary corporate/service architecture; realistic but not luxury or futuristic. Mid/high-quality first-person surfaces. | 1-m-compatible pieces preferred; UE5 materials, clean pivots, configurable colors, usable collision. | Neon labs, curved spaceship corridors, branded real-world offices, monolithic demo maps. |
| Industrial facility | Practical steel/concrete service architecture, decks, rails, stairs, guards, doors. | Modular dimensions, separable pieces, Nanite/LOD support where appropriate, simple collision. | Heavy post-apocalypse damage, oil-refinery-only megastructures, gameplay framework dependencies. |
| Maintenance props | Electrical cabinets, conduits, cable trays, pipes, valves, service tools. | Reusable straight/corner/junction pieces, material instances, sensible texel density. | Overly rusty/grimy sets, baked unreadable labels, excessive unique materials. |
| Storage/logistics | Shelves, pallets, totes, crates, carts, dock fixtures, bumpers. | Clean pivots, physics-safe collision candidates, material variants. | Military loot styling, destructible dependencies, excessive clutter bundles. |
| Security props | Cameras, mounts, access readers, bollards, generic monitoring hardware. | Directionally readable forms, separated moving pieces if possible, customizable status materials. | Weapons/turrets, hidden proprietary Blueprint logic, fixed emissive colors. |
| Generic furniture | Office desks, chairs, cabinets, monitors, break/service furniture. | Coherent contemporary style, moderate detail, configurable materials. | Hero designer furniture that dominates the setting, huge texture footprint. |
| Pipe/vent kits | Contemporary institutional utility systems. | Snap-friendly straights/corners/transitions, brackets, vents, shared materials. | Victorian/steampunk language, overly complex industrial simulation assets. |
| Lighting fixtures | Office panels, utility strips, high-bay industrial lights, warning fixtures. | Meshes separated from light Actors where possible; Lumen-friendly emissive setup. | Packs requiring many dynamic lights, ornate decorative fixtures, fixed blue neon. |

All candidates require explicit commercial/project-use licensing records, UE 5 compatibility, source/version information, and an estimated import footprint. Task 019 should produce a shortlist and compatibility assessment before any import.

## 23. Third-party asset acceptance criteria

An asset or pack is accepted only when:

- Its license permits the intended commercial project use and redistribution only as cooked game content; proof/source is recorded.
- It works in the project's UE 5 version or has a low-risk documented migration path.
- It has no required unrelated gameplay framework, plugin, input system, or project-settings takeover.
- Mesh scale, pivots, normals, UVs, and texel density can conform to the Demo standards.
- Collision is usable or inexpensive to replace and matches visible geometry.
- Material complexity, slot count, shader dependencies, and texture sizes are reasonable.
- Style, wear, proportions, and construction fit the chosen facility language after feasible material adjustment.
- Core colors, labels, and materials are customizable enough to integrate with Reality.exe.
- The pack does not impose a recognizable visual identity stronger than the project's own direction.
- Import can be scoped to needed assets without excessive unused dependencies or repository bloat.
- Performance is appropriate for first-person proximity and representative target hardware.
- The asset does not introduce broken references, naming conflicts, or opaque Blueprint dependencies.

Reject technically impressive assets that require redesigning the approved layout or visual pillars to justify their use.

## 24. Originality strategy

Reality.exe should look authored through combination and direction rather than total bespoke production.

- Preserve the approved room composition and systemic sequence instead of adopting marketplace demonstration layouts.
- Normalize library assets through the shared facility materials, controlled palette, texel density, and wear logic.
- Replace baked generic signs with the Reality.exe sign grid, room naming, sensor IDs, and diagnostic plate system.
- Concentrate bespoke form in the four gameplay hero families.
- Remix prop families across zones by function and avoid placing recognizable pack showcase clusters intact.
- Use lighting hierarchy to serve Witness, Context, physics, and Time decisions rather than merely showcase assets.
- Carry one consistent serialized system language from airlock to exit, becoming more explicit near the core.
- Add sparse, deliberate reality-registration anomalies only where narrative/system pressure supports them.

The resulting identity should survive even if individual chairs, shelves, pipes, and walls are replaced with comparable legal assets.

## 25. Zone-by-zone preliminary asset list

### Routine Intake

- `LIBRARY`: desk, task/visitor chairs, monitors, office cabinets, restrained glass modules, ceiling panel lights
- `CUSTOM SIMPLE`: intake sign frame, airlock frame adapter, access reader mount, base/threshold trim
- `CUSTOM HERO`: Developer access terminal variant from the shared terminal family
- `MATERIAL/DECAL ONLY`: room ID, visitor/staff routes, intake instructions, airlock asset plate, light handling wear

### Observed Storage

- `LIBRARY`: steel shelving, pallets, totes, storage clutter, fixed cover, utility lights
- `CUSTOM SIMPLE`: standardized Scale obstruction shell, service step, raised-opening trim, load reference marks
- `CUSTOM HERO`: none; Human Witness may remain a licensed placeholder/character solution
- `MATERIAL/DECAL ONLY`: bay numbers, inventory labels, floor pallet footprints, load limits, Reality object plates

### Security Corridor Hub

- `LIBRARY`: reinforced wall/door-shell candidates, bollards, glazing, cable trays, security cabinet, corridor lights
- `CUSTOM SIMPLE`: secure door face/track fitted to gameplay bounds, camera mount, maintenance-route sign hardware
- `CUSTOM HERO`: security camera/sensor family
- `MATERIAL/DECAL ONLY`: restricted access, camera/sensor IDs, watched-door identifier, floor threshold, asset plate

### Maintenance Loop

- `LIBRARY`: pipes, conduit, cable trays, vents, electrical boxes, rubber mats, utility fixtures
- `CUSTOM SIMPLE`: network-link brackets, label carriers, panel surround, cable transitions matching the Hub
- `CUSTOM HERO`: SensorFailure control assembly
- `MATERIAL/DECAL ONLY`: circuit/sensor IDs, diagnostic warnings, service dates, localized leaks/repairs, fault-screen graphics

### Logistics Well

- `LIBRARY`: industrial deck/rail/stair candidates, pallets, carts, shelves, dock bumpers, high-bay lights, structural beams
- `CUSTOM SIMPLE`: physics-safe load props, transfer obstruction shell, recovery-route guards, containment details
- `CUSTOM HERO`: none
- `MATERIAL/DECAL ONLY`: load lanes, upper-deck destination, weight limits, stopping zones, object plates, wheel/contact wear

### Physics Service Alcove

- `LIBRARY`: small control cabinets, generic instruments, utility shelving, cables
- `CUSTOM SIMPLE`: test-lane rails, push/force fixture, result markers, protected prop variants
- `CUSTOM HERO`: none
- `MATERIAL/DECAL ONLY`: mass/load scale, friction/contact strip, measurement grid, optional-service identifier

### Desynchronization Gate / Exit

- `LIBRARY`: industrial guards, structural frames, observation glazing, cable trays, warning fixtures
- `CUSTOM SIMPLE`: safe-pocket rails, cycle floor marks, service-shortcut door face, core-terminal mounting frame
- `CUSTOM HERO`: Security Rotor/Time machinery family; Reality core terminal variant; security camera family reuse
- `MATERIAL/DECAL ONLY`: scanner swept-volume marks, baseline reference index, cycle ticks, shortcut warning, Reality core identity, exit status graphics

## 26. Production order

1. **Standards and blockout lock:** record graybox dimensions, gameplay bounds, pivots, collision, naming, texel density, material parameters, and a screenshot baseline.
2. **Modular architecture kit:** replace shell surfaces zone by zone while preserving the graybox envelope and retesting routes after each zone.
3. **Base material library:** establish the six shared masters/instances before importing broad prop dressing.
4. **Functional doors and gameplay props:** replace every editable target with correct pivots, bounds, collision, and markings before decorative work.
5. **Hero gameplay assets:** camera, SensorFailure assembly, Time machinery, and terminal family; validate state/readability immediately.
6. **Generic prop dressing:** add library assets according to the zone density budget and rerun LOS/physics/Scale tests.
7. **Decals and signage:** install the facility and Reality.exe language after architecture/props stabilize.
8. **Lighting pass:** establish route hierarchy, Witness readability, Context state, physics contact, and Time cadence under Lumen.
9. **Developer Mode presentation:** replace graybox labels using the approved plate/overlay language in a separately scoped task.
10. **Audio/VFX polish and final optimization:** implement minimal systemic cues, profile, fix readability regressions, and conduct blind playtests.

Architecture and materials should be proven in a representative Intake-to-Security vertical slice before replacing all seven spaces. Gameplay validation occurs after every major replacement, not only at the end.

## 27. Definition of art-pass success

The Demo art pass is sufficiently complete when:

- No major player-facing graybox primitives remain except explicitly approved temporary debug/validation elements.
- All seven zones are distinguishable by function while unmistakably belonging to one facility.
- Approved routes, alternatives, timings, target reacquisition, and softlock recovery remain intact.
- Editable targets are predictable without persistent floating debug labels.
- Non-editable architecture cannot reasonably be mistaken for a target.
- Human and Camera facing/attention are understandable from normal gameplay views.
- SensorFailure reads as a linked environmental system fault rather than a score-control device.
- Scale references, Gravity containment, Mass/Friction motion paths, and Time cadence are visually legible.
- The baseline and editable Time mechanisms can be compared without reading the numeric multiplier.
- Lighting guides the player and preserves critical targets in every required state.
- The exit reads immediately and completion presentation remains clear.
- Materials, signs, lighting, and hero assets prevent a recognizable marketplace-pack collage.
- Collision and navigation match visible geometry; dressing introduces no gameplay blocker or ambiguous LOS.
- Material, texture, light, decal, and mesh costs remain stable on representative target hardware.
- A blind playtest meets the approved gameplay/readability gates without art-related coaching.

AAA polish, bespoke clutter, cinematics, sculpted characters, and unique textures for every asset are not success requirements.

## 28. Recommended next production tasks

### Task 019 — Asset acquisition research and import specification

Research current legal candidates against Section 22, record license/UE compatibility/footprint, and recommend a minimal shortlist. Do not bulk import before approval.

### Task 020 — Modular architecture and base-material vertical slice

Create the module standards, shared base materials, and replace Routine Intake through Security Hub as a representative slice without changing gameplay bounds.

### Task 021 — Full Demo architecture, materials, decals, and signage

Extend the approved kit across all zones; establish the facility sign grid, Reality object plates, floor markings, and controlled condition variation.

### Task 022 — Custom gameplay hero assets

Build and integrate the terminal, SensorFailure, security camera, and Time machinery families with correct gameplay pivots, state surfaces, and collision.

### Task 023 — Environment dressing and lighting pass

Dress each zone to its density budget, establish Lumen lighting hierarchy, verify Witness/Context/physics/Time readability, and profile the result.

### Task 024 — Developer Mode and Reality visual feedback

Replace graybox editable labels with the serialized plate and diagnostic overlay language; add restrained systemic Apply/Restore, observation, and instability presentation as separately approved.

Each task must preserve the Task 017 gameplay Gate and include a manual regression route. Task 018 does not start any of these tasks.

## 29. Art production review checklist

Before accepting any later art change, verify:

- Does it preserve the approved route, bounds, collision, Witness LOS, and recovery path?
- Does it make the local objective, obstacle, relevant object, and destination clearer?
- Does normal-world presentation remain believable without Developer Mode?
- Does it use the shared material, sign, plate, module, and color standards?
- Is it generic enough for a library asset, simple enough for Tier B, or valuable enough for the hero budget?
- Does it introduce unnecessary material slots, textures, translucency, dynamic lights, or dependencies?
- Can a player still distinguish interaction, editability, observation, Context, and destination?
- Does it strengthen Reality.exe identity without becoming generic neon cyberpunk?
