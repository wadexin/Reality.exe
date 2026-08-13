# Reality.exe Core Ability Review and Demo Ability Set

## Decision status

This document records the Task 015 Prototype Gate decision for the first public Demo. It reviews the six implemented Reality abilities as player-facing mechanics rather than treating technical completion as proof that each deserves equal content.

The recommendation is:

- **Primary Demo abilities:** Collision, Scale, Gravity, Time.
- **Secondary Demo abilities:** Mass and Friction, presented together in one optional or advanced physical-system opportunity rather than taught as equal headline powers.
- **Demo ability budget:** substantially teach four abilities; use Mass and Friction only where their physical consequences are unmistakable.

This is a content-priority decision, not a request to remove or retune any implemented ability. No tuning, capability, Context, UI, input, or P0 changes are authorized by this review.

## Evidence and current constraints

The review is based on the current GDD and project plan, the six implementations and their focused tests, the Developer Console, Reality Manager, Witness and Context systems, prior manual validation, and a read-only inspection of the P0 prototype.

P0 currently demonstrates a narrow prototype vocabulary: Collision-only barriers, Collision/Scale objects, several Collision/Scale/Gravity multi-solution props, Sensor Failure and Explosion Context sources, and Human/Camera Witness roles. It does not provide evidence that Mass, Friction, or Time deserve equal level-design emphasis. That absence is expected because those abilities were completed after P0.

Current contracts that materially affect design are:

- Collision disables owner-local primitive collision and can directly invalidate barriers.
- Scale changes the selected Actor's scale from a captured baseline.
- Gravity supports Normal, Low, and Zero only on eligible simulated primitives. It does not modify Character gravity and does not provide reverse gravity.
- Mass changes effective mass on eligible simulated primitives; its value is usually perceived through interaction and motion rather than appearance.
- Friction applies isolated runtime Physical Materials to eligible simulated primitives; it is perceived mainly during contact and motion.
- Time changes `AActor::CustomTimeDilation`. It strongly affects ordinary tick-driven Actor behavior, but is not local Chaos bullet time, timer virtualization, universal animation control, Niagara control, or audio time scaling.
- Apply events are evaluated at the target Actor by the existing Witness system. Current Context rules exist only for Gravity + Explosion, Scale + Explosion, Collision + Sensor Failure, and Gravity + Fire.

## Rating scale

Scores are decision support, not an additive ranking:

- 1 — weak
- 2 — limited
- 3 — useful
- 4 — strong
- 5 — exceptional

| Ability | Player fantasy | Immediate readability | Puzzle affordance | Systemic combinations | Witness / Context potential | Dominant-strategy safety | Technical reliability | Content efficiency | Visual communication efficiency | Demo value |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| Collision | 5 | 5 | 5 | 3 | 5 | 1 | 5 | 4 | 4 | 5 |
| Scale | 5 | 5 | 5 | 5 | 4 | 3 | 4 | 4 | 5 | 5 |
| Gravity | 4 | 5 | 4 | 5 | 5 | 4 | 4 | 3 | 5 | 5 |
| Mass | 3 | 2 | 3 | 4 | 2 | 5 | 4 | 2 | 2 | 2 |
| Friction | 3 | 2 | 4 | 4 | 2 | 5 | 4 | 2 | 2 | 3 |
| Time | 5 | 4 | 5 | 3 | 4 | 4 | 4 | 3 | 4 | 5 |

“Dominant-strategy safety” scores resistance to becoming a universal answer. “Content efficiency” and “visual communication efficiency” score how cheaply the ability can produce clear, reliable Demo play; higher is better.

## Ability reviews

### Collision — Primary, early, deliberately constrained

**Player fantasy.** Collision is the purest expression of editing the simulation: the player turns solidity off. It instantly communicates forbidden developer access.

**Readability and affordance.** The result is obvious when a barrier stops blocking, and the ability supports doors, barriers, traversal blockers, containment, and physical props. It also supports a satisfying apply-cross-restore cleanup loop.

**Systemic value.** Its strongest systemic relationship is not another physical property but Reality itself: target accessibility, Witness coverage, Context, the consequences of leaving geometry non-solid, and restoration. Pairing it with another ability often adds less than simply using Collision alone.

**Dominance risk.** Collision is the clearest critical design risk. If every door, wall, crate, security boundary, and traversal blocker supports it, level geometry stops being a problem space. It bypasses the physical reasoning that Scale, Gravity, Mass, Friction, and Time need in order to matter.

The Demo should therefore treat Collision as:

- **Restricted to authored object categories and specific instances**, not freely assigned to most geometry.
- **An early-game ability** because it explains the premise immediately.
- **Already appropriately high-risk** at the current base Suspicion of +25; no tuning change is recommended without Demo playtest evidence.
- **Powerful but accountable**, using visible Witness placement, target access, restoration pressure, and downstream physical consequences.

Good constraints are legible and diegetic: an editable door panel, a specific modular barrier, a watched security door, or a prop whose loss of collision causes another problem. Bad constraints are invisible blockers or arbitrary refusals on visually identical objects.

**Technical/content assessment.** The implementation is robust and simple to author. Content design must do the balancing work. Collision should receive substantial Demo screen time, but fewer eligible targets than its broad technical capability permits.

### Scale — Primary headline ability

**Player fantasy.** Scale feels like direct spatial editing. It is visually spectacular without requiring final art and makes the Developer Console feel like a genuine property inspector.

**Readability and affordance.** Size change is immediate. Scale can remove an obstacle, create or alter a platform, change cover and occlusion, open spatial routes, and reframe a prop's physical usefulness. It supports multi-solution thinking better than a binary bypass.

**Systemic value.** Scale combines strongly with Gravity and Mass because changing size changes the spatial problem while those properties change behavior. Friction can then govern motion on ramps or surfaces. Scale + Time is useful when a machine's footprint and cycle both matter, although the systems remain mechanically independent.

**Risks.** Large scaling can cause clearance, penetration, physics-body recreation, and navigation/readability problems. The current implementation deliberately reapplies active Mass and Friction after scale changes, which is a strong technical foundation, but authored spaces still need generous tolerances.

**Decision.** Scale is a Primary ability and should be one of the Demo's headline mechanics. Prefer object and environment scaling in the Demo; current code does not establish the GDD's broader player-scaling fantasy.

### Gravity — Primary physical-system ability

**Player fantasy.** Gravity changes what the selected physical object believes “down” means in practice: Low and Zero create floating, repositionable props and altered trajectories.

**Readability and affordance.** Motion is immediately visible. It can clear an object, reposition it, create a temporary traversal aid, or change a timing/trajectory problem. Gravity communicates best when the affected object has room to move and a clear normal-behavior reference.

**Systemic value.** Gravity + Scale is the strongest physical pair because one changes geometry and the other changes movement. Gravity + Mass and Gravity + Friction create different handling profiles. Gravity also has the best current Context support: Explosion and Fire rules already produce interpretable plausibility decisions.

**Constraints.** It applies only to simulated world primitives. It does not modify Character gravity, reverse gravity, or guarantee a platforming solution by itself. Level design must not advertise Gravity on static or constrained objects that cannot visibly respond.

**Decision.** Gravity is Primary. It deserves a room where moving or floating a prop opens several routes, plus a later witnessed/Context variation. It should not be presented as a player jump modifier.

### Mass — Secondary combination modifier

**Player fantasy.** Mass edits the hidden “weight/inertia” truth of a physical object. This is intellectually consistent with the game, but less immediately magical than changing size or gravity.

**Readability and affordance.** Mass is invisible until the player pushes, impacts, launches, balances, or weighs something. It is distinct when a device responds to force or weight, or when identical-looking objects behave differently. Without a clear interaction, it looks like a number changing in a menu.

**Systemic value.** Mass + Friction is its strongest pairing: one changes resistance to acceleration and momentum, the other changes contact resistance. Mass + Gravity can make a handling puzzle more expressive, although the current systems must not be described as a local physics-time solver or a fully simulated counterweight framework. Scale + Mass is valuable because Scale supplies visible change while Mass allows physical behavior to be tuned independently.

**Witness/Context concern.** A Witness can see the resulting motion but cannot naturally “see mass change.” No current Context rule explains Mass. Suspicion presentation should eventually emphasize the implausible consequence, not assume the player reads the hidden property as visible evidence.

**Decision.** Mass is Secondary, not a headline mechanic. Use it in one optional/advanced physical setup with strong before/after feedback. Do not teach it in isolation through a passive crate.

### Friction — Secondary, potentially valuable but content-expensive

**Player fantasy.** Friction edits the rules of contact: slippery versus grippy. It can enable elegant indirect solutions that feel less like bypassing a lock and more like exploiting a simulation.

**Readability and affordance.** Friction creates genuinely distinct behavior on ramps, moving belts, sliding objects, stopping zones, and pushes. It is weak on flat stationary props with no applied force. The player needs motion, surface cues, and a predictable reference to understand the result.

**Systemic value.** Friction + Gravity can turn a ramp or incline into a controllable transport problem. Friction + Mass gives pushes and impacts meaning. Scale can change clearance or contact presentation, but Scale + Friction is situational rather than universally strong.

**Witness/Context concern.** Like Mass, Friction is inferred through consequence. Existing Witness evaluation works technically, but the fiction of what was noticed is less legible. There is no current Context rule.

**Decision.** Friction is Secondary. It may deserve one memorable optional opportunity if the graybox can make Zero/High behavior obvious within seconds. If that room is not readable without explanatory text, reserve Friction for later production rather than forcing equal Demo coverage.

### Time — Primary, late-Demo reveal

**Player fantasy.** Time is a high-value “I edited the simulation” power. Slowing one scanner while everything else continues normally is distinct from changing geometry or physics properties.

**Readability and affordance.** Moving obstacles, rotating scanners, repeating machinery, and desynchronized cycles make the effect clear. A visible reference at normal speed is important during introduction. Time enables timing windows and coordination problems that none of the other five abilities solve as cleanly.

**Systemic value.** Its best combinations are spatial or observational rather than physical. Time + Scale can change both a mechanism's footprint and cycle. Collision + Time can offer a blunt bypass versus a timed, lower-disruption route. Gravity + Time is only useful when the target has separate tick-driven behavior; Time must not be claimed to slow Chaos motion.

**Witness/Context potential.** A human or camera can notice a scanner or machine suddenly changing cadence. No current Time Context rule exists. A future machine-malfunction or sensor-desynchronization Context could be valuable, but this review does not authorize it.

**Decision.** Time is Primary but should be a late-Demo reveal. Its semantic boundary must be taught through carefully chosen tick-driven machinery, not a simulated falling crate. One strong scanner/machinery room is more valuable than broad but inconsistent capability assignment.

## Ability combination matrix

The matrix is conceptual and constrained to current implemented behavior.

| Pair | Classification | Current design value |
|---|---|---|
| Collision + Scale | Useful | Offers a blunt solidity bypass versus spatially reshaping the target; simultaneous use is usually unnecessary, but the choice supports alternate solutions. |
| Collision + Gravity | Useful | Removing collision can release or pass a prop while Gravity changes how an eligible simulated prop moves; useful when consequences matter, not for a static door. |
| Collision + Mass | Weak / niche | Collision can release a simulated body and Mass can change the resulting impact, but most barriers are solved by Collision alone. |
| Collision + Friction | Weak / niche | A collision-restored moving prop can retain Friction, but few readable puzzles require both rather than one direct action. |
| Collision + Time | Useful | Supports a direct bypass versus manipulating a tick-driven security window; simultaneous application matters only on a target with both meaningful contracts. |
| Scale + Gravity | **Strong** | Changes both spatial footprint and physical movement, enabling floating platforms, moved obstacles, and multiple traversal arrangements. |
| Scale + Mass | **Strong** | Separates visible size from effective physical weight, enabling large-light or small-heavy behavior with clear setup and consequence. |
| Scale + Friction | Useful | Size changes clearance/contact presentation while Friction controls sliding, especially on ramps, but requires careful authoring. |
| Scale + Time | Useful | Changes a mechanism's footprint and tick-driven cadence independently; best on machinery, not generic physics props. |
| Gravity + Mass | **Strong** | Produces distinct handling and force/inertia outcomes on simulated props, provided the player has a clear way to move or observe them. |
| Gravity + Friction | **Strong** | Gravity supplies motion while Friction governs whether an object slides or holds, making ramps and inclined transport highly expressive. |
| Gravity + Time | Weak / niche | `CustomTimeDilation` does not locally slow Chaos, so this is meaningful only when the same Actor also has separate tick-driven machinery behavior. |
| Mass + Friction | **Strong** | Creates the clearest advanced physical manipulation pair: effort, acceleration/momentum, sliding, and stopping can all differ. |
| Mass + Time | Weak / niche | Time does not slow Chaos, so the properties coexist but do not form a reliable shared physical mechanic. |
| Friction + Time | Weak / niche | Time does not scale Chaos contact simulation; a useful pair requires authored tick-driven motion feeding a friction interaction. |

The strongest Demo-ready combination is **Scale + Gravity** because both halves are visible and support traversal. **Gravity + Friction** and **Mass + Friction** have strong systemic potential but higher teaching and content cost. Pairs involving Time and a physics-only property must be treated cautiously because local Actor time does not scale Chaos.

## Witness and Context review

| Ability | Can an observer notice it? | Current Context position | High-value future investigation, not approved here |
|---|---|---|---|
| Collision | Yes when a solid boundary is crossed or an object passes through support; less obvious if changed and restored unseen. | Sensor Failure can reduce Collision suspicion. | Construction/maintenance or access-control failure tied to specific object classes. |
| Scale | Immediately visible during change and persistent afterward. | Explosion can partially explain Scale. | Structural damage or machinery expansion only where the object/result makes sense. |
| Gravity | Floating, falling, or altered motion is highly visible. | Explosion and Fire rules already exist. | Local machinery failure or decompression for appropriate props. |
| Mass | The property is invisible; only implausible force, impact, or handling is observable. | No rule. | Equipment malfunction, counterweight failure, or damaged load sensor tied to explicit mechanisms. |
| Friction | Usually observable only as unexpected sliding or stopping. | No rule. | Spill, ice, oil, damaged brakes, or surface treatment tied to relevant materials/objects. |
| Time | Cadence change is visible on repeating machinery when a reference exists. | No rule. | Machine malfunction or sensor desynchronization; never a universal excuse for local time editing. |

The current Witness system evaluates visibility, range, field of view, and occlusion at the event target. That is sufficient for the Prototype Gate, but Mass and Friction reveal a presentation gap: the event location is known while the precise observable consequence is not modeled. Demo content should make the target and consequence spatially coincide rather than assuming sophisticated witness interpretation.

Context switches should be world actions with costs and consequences, not conspicuous “discount Suspicion” buttons. The current specific rule model is the correct direction; future rules should remain narrow and interpretable.

## Demo ability budget

The first public Demo should substantially teach **four** abilities:

1. Collision — premise/tutorial and cleanup.
2. Scale — headline spatial manipulation.
3. Gravity — headline physical manipulation and first strong combination.
4. Time — late reveal and timing/security climax.

Mass and Friction should remain available only in a clearly authored optional or advanced physical opportunity, ideally as a paired discovery after the player understands the console. They should not receive separate mandatory tutorial rooms in a 20–30 minute Demo. If their test setup is not instantly readable in graybox, omit them from required Demo progression while retaining the production systems for the full game.

This budget preserves the fantasy of a broad internal tool without asking the player to learn six equal vocabularies, five preset sets, Witness logic, Context logic, restoration, and Suspicion within half an hour.

## Proposed 20–30 minute mechanic progression

### 1. Baseline and forbidden access — 0–5 minutes

- Establish normal interaction and a normal route attempt.
- Reveal the Developer Console on one unmistakably editable barrier.
- Teach Collision OFF, crossing, and explicit Restore with no Witness pressure.
- Immediately show that not every wall or piece of world geometry is an editable target.

### 2. Spatial alternatives and observation — 5–11 minutes

- Introduce Scale on an object where shrinking removes an obstruction and enlarging creates a different route or platform.
- Add one Human or Camera Witness with clearly readable view coverage.
- Let Collision remain an option but make it the conspicuous/high-evidence route rather than silently forbidding it.

### 3. Plausibility as play — 11–16 minutes

- Present a watched security obstacle and a specific Context source.
- Let the player compare direct Collision with a Context-supported approach.
- Make Context activation alter the space or observers visibly, so it reads as world manipulation rather than a numeric discount.

### 4. Physical combination — 16–23 minutes

- Introduce Gravity on an eligible simulated prop with obvious space to float or fall.
- Combine Gravity + Scale in one compact multi-route room: reshape, reposition, or use both.
- Optionally place a side challenge using Mass + Friction with a ramp or pushable prop; do not gate completion on understanding both.

### 5. Time reveal and synthesis — 23–30 minutes

- Introduce Time using two visibly synchronized tick-driven scanners or moving mechanisms, one editable and one reference.
- Build a timing/security objective where slowing, speeding, or desynchronizing machinery creates a route.
- Reuse Witness, Context awareness, restoration, and one earlier physical ability without requiring all six.
- End on a systemic choice rather than a single prescribed sequence.

## Developer Console assessment

The current single scrolling console remains acceptable for the prototype because unsupported sections are fully hidden and a standard ScrollBox prevents hard clipping. It is not yet an ideal six-ability learning surface.

Recommendations for later UI work:

- Continue hiding unsupported abilities completely; capability-driven relevance is the strongest clutter control already present.
- Preserve the current Primary ordering near the top: Collision, Scale, Gravity, then Time. Mass and Friction can later sit under an **Advanced Physics** grouping when a target supports them.
- Keep target identity, capabilities, current state, and Reality evidence visible or easily recoverable while scrolling.
- Prefer compact horizontal preset controls or collapsed inactive sections before considering tabs; tabs could hide cross-property state and weaken the “property inspector” fantasy.
- Distinguish “baseline value selected but cycle still active” from “restored and inactive” consistently for Scale, Gravity, Mass, Friction, and Time.
- Do not redesign until Demo target mixes and normal-resolution usability are tested; current hidden sections mean most real targets will not display all six groups.

## Minimum visual and readability requirements

### Collision

- A consistent indication that the specific object—not arbitrary world geometry—is editable.
- Immediate state feedback in the console and a subtle world-state cue for Collision OFF.
- Clear physical consequence when traversed, plus readable Restore confirmation.
- Witness view/coverage cues around high-risk barriers.

### Scale

- Unmistakable transform interpolation or state transition, with enough clearance to avoid ambiguous clipping.
- Stable visual reference for original size.
- Environment composition that makes each useful preset's spatial consequence legible.
- Feedback that 1.0x-in-cycle and explicit Restore are not the same logical state.

### Gravity

- An object that visibly responds immediately; never teach it on an apparently static or constrained prop.
- Clear falling/floating motion, shadow/contact cues, and sufficient vertical space.
- Distinct indication of Low versus Zero behavior.
- Context presentation that visually connects Explosion/Fire to the affected area when used.

### Time

- Repeating tick-driven machinery with obvious motion and no reliance on Chaos behavior.
- A normal-speed reference, synchronized neighbors, or readable rhythm during introduction.
- Clear local targeting so the player sees that the world and player remain at normal speed.
- Persistent state feedback because the effect can become subtle at 1.0x.

### Secondary: Mass and Friction

- Mass requires a visible force source, pressure/weight response, impact comparison, or push interaction; a numeric kilogram readout is insufficient.
- Friction requires motion, a ramp/surface, and clear sliding/stopping distance; a stationary object is insufficient.
- Both need paired baseline comparison and sound/animation feedback eventually, but no assets are authorized by this review.

## Ranked design risks

### Critical

1. **Collision becomes the universal solution.** Broad capability assignment would erase geometry, traversal, and the need for other properties. Mitigate through authored eligibility, target access, witnesses, consequences, and alternate-object design—not invisible blockers.

### High

2. **Six equal abilities overwhelm a short Demo.** The console, preset vocabularies, restoration states, Suspicion, Witnesses, and Context already create a large learning load. Teach four; make two optional.

3. **Mass and Friction are technically valid but perceptually weak.** Without dedicated force, ramp, and comparison setups, they read as menu numbers and dilute confidence in the systemic promise.

4. **Developer Console interruption cost breaks flow.** Repeated opening, targeting, scrolling, choosing, closing, and reacquiring can turn experimentation into menu labor. Capability filtering helps, but later interaction-cost testing is essential.

### Medium

5. **Time's semantic boundary is misunderstood.** Players may expect local bullet time for falling physics, particles, animation, timers, and audio. Only assign Time to behavior that visibly respects the supported Actor tick contract.

6. **Suspicion remains numerical rather than spatially intuitive.** Players can see a delta without necessarily understanding who observed it, from where, or what evidence remains.

7. **Stationary Witnesses become solved geometry.** Once sight cones and positions are memorized, observation may become routing arithmetic rather than a reactive system.

8. **Context switches feel like Suspicion coupons.** If activation has no spatial, social, or operational consequence, plausibility becomes a matching-tag exercise.

### Low for the current Prototype Gate

9. **Preset breadth creates false choices.** Five presets are useful systemically, but many authored targets may have only two meaningful outcomes. Content should validate useful ranges rather than displaying every preset as equally valuable in puzzle design.

## Recommended next tasks

Do not begin these as part of Task 015. Recommended sequence:

1. **Demo mechanic brief and puzzle grammar.** Convert the four-Primary budget into room objectives, eligible target categories, alternate-solution requirements, and explicit anti-dominance checks without building the level.
2. **Public Demo graybox.** Build a compact 20–30 minute route using Collision, Scale, Gravity, and a late Time reveal, with one optional Mass/Friction side opportunity.
3. **Gameplay readability and environment-art pipeline.** Define editable-object language, state feedback, Witness coverage cues, Context causality, and minimum visual/audio feedback before environment production scales up.
4. **Witness presentation and behavior pass.** Make observation/evidence spatially understandable and test a small amount of movement or state change without prematurely building full AI complexity.
5. **Reality Correction consequence prototype.** Add one understandable, recoverable response to high Suspicion so the Demo's risk system produces gameplay rather than only a number/state label.

## Gate conclusion

The six-property foundation is worth retaining, but the Demo should not advertise six co-equal pillars. **Collision, Scale, Gravity, and Time** form the clearest public-facing set: bypass, reshape, reposition, and retime. They are visually distinct and collectively express the game's central fantasy.

**Mass and Friction** are not redundant implementations; they are advanced physical modifiers whose value depends on specialized content. Their best role is Secondary and combinatorial. Treating that as a deliberate content budget protects their future potential while keeping the first Demo readable.

Collision should remain powerful, early, and highly legible, but capability assignment must be selective. The goal is not to weaken the cheat. The goal is to make choosing it an accountable systemic decision rather than the answer to every wall.
