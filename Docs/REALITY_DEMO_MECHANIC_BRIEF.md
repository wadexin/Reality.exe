# Reality.exe Public Demo Mechanic Brief and Puzzle Grammar

## Document purpose

This document is the level-design contract for the first 20–30 minute public Demo and the direct input to Task 017 Public Demo Graybox. It converts the approved core ability review into reusable puzzle rules, a compact room plan, and playtest gates.

It does not authorize gameplay changes, new abilities, tuning changes, new Context rules, final art, or production beyond the Demo graybox.

## Demo promise

The player discovers that selected physical rules are editable, but every implausible edit can become evidence. The Demo must teach: **“You may cheat. Just don't get caught by reality.”** Success comes from choosing between direct risk, spatial avoidance, environmental plausibility, and unexpected combinations—not from obeying morality or finding one designer-prescribed solution.

By the exit, the player should understand that Reality judges implausibility: a witnessed impossible event is dangerous, a carefully hidden event creates less evidence, and a specific world condition can make some anomalies more believable.

## Audience, length, and ability budget

- **First blind playthrough:** 20–30 minutes.
- **Experienced or replay run:** 10–15 minutes.
- **Primary abilities taught substantially:** Collision, Scale, Gravity, Time.
- **Secondary abilities:** Mass and Friction in one optional advanced opportunity only.
- **Required meaningful spaces:** seven, connected as one compact route with short loops.

The Demo does not teach six abilities equally. Collision introduces the premise, Scale and Gravity establish spatial/physical problem solving, and Time supplies the late reveal. Mass and Friction remain discoverable proof of systemic depth without becoming onboarding requirements.

## Player-facing solution vocabulary

These labels are design and playtest vocabulary, not scores or code states.

### Direct

The fastest obvious edit, usually made in view of a Witness or without cleanup. It should work when the rules support it and carry understandable evidence rather than being blocked by the designer.

### Careful

The player uses cover, timing, target access, Witness facing, or an alternate route to make the same edit outside observation. It trades speed for spatial reasoning.

### Plausible

The player first creates a specific environmental state that explains part of the anomaly, then performs the edit. Context must be a world condition with visible meaning, not merely a Suspicion discount control.

### Emergent

The player combines valid properties, object states, geometry, or routes in an unprompted way. Preserve it unless it causes a softlock, progression corruption, severe instability, or universal trivialization.

A strong main-path room should support at least two categories. A synthesis room should support three where content cost permits.

## Reusable puzzle grammar

Every Demo puzzle must be describable through the following fields:

```text
OBJECTIVE
+ PHYSICAL CONSTRAINT
+ EDITABLE PROPERTIES
+ OBSERVATION
+ PLAUSIBILITY
+ CONSEQUENCE
+ ALTERNATE ROUTE
```

### Objective

A player-facing result such as reach the secure side, obtain access, cross the machinery bay, or reach the exit. The objective must not name the required ability.

### Physical constraint

The world-state obstacle: locked door, blocked opening, height difference, moving scanner, unstable prop, watched route, or timed barrier.

### Editable properties

Only the properties on authored, visually legible targets. Capability assignment defines the available verbs; it must not imply that every similar-looking structural object is editable.

### Observation

Who can see the target when the edit occurs? The player must be able to infer camera direction, Human facing, occlusion, and safe manipulation positions from the space.

### Plausibility

Which active environmental states match the exact edit, if any? A Context is optional. No Context is better than an arbitrary or misleading one.

### Consequence

What changes beyond opening the route: evidence, changed geometry, displaced prop, blocked return path, exposed player, altered machine cadence, or need for cleanup?

### Alternate route

A normal, careful, Context-assisted, or different-property approach that reconnects to the same progression line. Alternate routes should reuse space, not multiply the Demo into separate levels.

### Example grammar card

```text
Objective:
Reach the secure side of the observation corridor.

Physical constraint:
Locked security door.

Editable properties:
Door Collision.

Observation:
One camera clearly watches the doorway; nearby cover breaks LOS to the target.

Plausibility:
SensorFailure source can be activated elsewhere in the loop.

Consequence:
Direct Apply creates high evidence; leaving Collision disabled leaves an obviously invalid route.

Alternate routes:
- Direct: disable Collision in camera view and accept risk.
- Careful: reach the occluded maintenance side and edit from outside camera LOS.
- Plausible: activate SensorFailure, then disable and restore Collision.
- Normal: discover a longer authorized maintenance route if included by the graybox budget.
```

The card documents possibilities, not a solution checklist. The room passes only if the physical layout and current systems actually support each claimed route.

## Five-beat mechanic progression

### Beat 1 — Collision: editing and restoration

- **Player objective:** reach the workspace beyond a locked maintenance barrier.
- **New concept:** open F6 Developer Console, select an authored target, disable Collision, cross, and explicitly Restore.
- **Old concept reused:** movement, looking, jumping, and E interaction.
- **Obstacle type:** one unmistakable editable barrier inside otherwise non-editable structure.
- **Approaches:** Direct Collision; a short normal interaction/maintenance bypass may exist as a reward for observation, but is not required for the tutorial beat.
- **Suspicion pressure:** no active Witness. One Apply (+25) followed by Restore (-5) is acceptable onboarding evidence and reaches Questioning without punishment.
- **Failure/recovery:** the player cannot fall out of the level or lose access to the target. The barrier remains reacquirable from both sides. Leaving it disabled does not block progression.
- **Exit understanding:** the player knows how to identify a target, edit a physical rule, observe the state, and distinguish 1:1 state cleanup from simply moving on.

This is the only beat permitted to be close to a single-verb tutorial.

### Beat 2 — Scale + Witness: observation creates evidence

- **Player objective:** cross a monitored storage threshold or reach a raised service ledge.
- **New concept:** Scale changes spatial affordance; a Witness adds evidence when it sees an edit at the target.
- **Old concept reused:** Developer Console targeting, Collision as a risky fallback, restoration, and environmental interaction.
- **Obstacle type:** scalable obstruction/step near a clearly oriented Human or Camera observation zone.
- **Approaches:** shrink an obstruction from cover; enlarge a prop into a step when the Witness turns or LOS is blocked; use watched Collision on a designated barrier as the fast Direct route.
- **Suspicion pressure:** one unwitnessed Scale Apply is +20; a Human adds +10 and a Camera +15. The player should be able to compare observed and unobserved outcomes without being forced into Correction.
- **Failure/recovery:** scaled props cannot trap the player against walls or remove the only return path. The original target remains selectable at useful presets.
- **Exit understanding:** the player knows that evidence depends on who sees the target edit, and that physical positioning can be as important as the chosen property.

### Beat 3 — Context/Plausibility: explain the anomaly

- **Player objective:** pass the watched security corridor while managing evidence.
- **New concept:** a specific environmental condition can reduce the plausibility cost of a matching anomaly.
- **Old concept reused:** Collision, Witness LOS, interaction, restoration, and route planning.
- **Obstacle type:** observed Collision-editable security door plus a SensorFailure source reached through a short maintenance loop.
- **Approaches:** Direct watched Collision; Careful edit from an occluded position; activate SensorFailure then use Collision; traverse the maintenance route and avoid the door edit.
- **Suspicion pressure:** Collision is +25, Camera is +15, configured SensorFailure reduction is 15, and Restore is -5. The room should make those qualitative differences visible without requiring arithmetic.
- **Failure/recovery:** Context can be toggled or re-entered safely; the player is never locked between a disabled Context source and a required door. Reckless evidence changes Reality state but does not end the run.
- **Exit understanding:** Context is specific, not universal; it makes one kind of event more explainable rather than legal or invisible.

### Beat 4 — Scale + Gravity: systemic physical room

- **Player objective:** cross a vertical logistics gap and reach the machinery access side.
- **New concept:** two independent physical properties can create multiple arrangements rather than one correct sequence.
- **Old concept reused:** Scale, Witness awareness, restoration, target capability inspection, and route choice.
- **Obstacle type:** recoverable simulated props, an overhead/side opening, a raised ledge, and geometry that supports several physical arrangements.
- **Approaches:** enlarge a prop into a step; use Low/Zero Gravity to reposition a prop through the opening; shrink an obstruction then use the freed route; combine Scale + Gravity to create a safer or less exposed platform arrangement.
- **Suspicion pressure:** quieter experimentation is possible in a partially unobserved setup zone. The final placement or edit may be visible. Current Gravity + Explosion and Scale + Explosion Context relationships may be used only if an actual Explosion state is visibly present.
- **Failure/recovery:** props stay inside a bounded catch volume, cannot enter inaccessible voids, and are not the sole unrecoverable progression keys. A maintenance return route permits reacquisition and reset-by-Restore behavior.
- **Exit understanding:** the player can keep properties active independently, restore in arbitrary order, and invent a physical configuration rather than execute a named answer.

### Beat 5 — Time: local machinery reveal and synthesis

- **Player objective:** cross a final security machinery lane and activate the Reality exit/core terminal.
- **New concept:** Time edits one Actor's tick-driven cadence while the player, UI, world, and unrelated Actors remain normal.
- **Old concept reused:** spatial positioning, Witnesses, Context awareness, Collision/Scale/Gravity alternatives, and cleanup.
- **Obstacle type:** synchronized rotating scanners, moving barriers, or periodic machinery with one or more clearly Time-editable Actors and a normal-speed reference.
- **Approaches:** slow a scanner to create a crossing window; accelerate a moving barrier through its blocking phase; use cover and timing without editing; use an earlier spatial property on a separate authored target to create a different route; accept a direct Collision risk on one designated barrier if the route remains legible.
- **Suspicion pressure:** this is the high-risk synthesis. Time Apply is +20 with no current Context reduction; Human/Camera evidence applies normally. A careful player should still finish below Correction, while a reckless player can reach it here.
- **Failure/recovery:** machinery cycles remain non-lethal or resettable in graybox, Time targets remain visible/reacquirable at every preset, and Restore returns exact baseline cadence. The exit cannot depend on a Chaos body being slowed by Time.
- **Exit understanding:** local Time is a machinery/system verb, not global bullet time or local physics slow motion, and earlier abilities remain valid alternate tools.

## Ability-specific authoring grammar

### Collision

Collision is powerful by design. Control dominance through readable capability authorship and consequences, never invisible blockers.

1. **Assign Collision to discrete authored systems.** Common targets are doors, maintenance panels/barriers, removable containment pieces, selected props, and specific security gates.
2. **Do not assign it to structural substrate.** Ordinary walls, floors, ceilings, exterior shells, kill boundaries, and the level's outer containment are not editable because they are presented as world structure rather than discrete simulation devices.
3. **Keep visual equivalence honest.** Objects with the same visual language should follow the same broad eligibility rule. If two identical doors differ, show access tier, hardware, damage, or ownership differences.
4. **Make target access part of the puzzle.** A player may need LOS, cover, a side angle, or a maintenance position to edit the relevant Actor. Do not make the target arbitrarily unreachable.
5. **Author downstream consequences.** Disabling a prop's collision may release support, remove cover, expose a sightline, or complicate return travel. The consequence must follow visible physics/space, not hidden punishment.
6. **Preserve the Apply–cross–Restore loop.** Important barriers must be reacquirable from both sides, and restoring must never trap the player in invalid geometry.
7. **Use Witnesses and Context as choices.** A watched security door can remain editable; the player chooses direct evidence, careful positioning, or SensorFailure.
8. **Do not make Collision the alternate route and main route simultaneously.** At least some objectives must concern moving, timing, or arranging objects rather than merely crossing a surface.

### Scale

Strong Scale patterns:

- Shrink an obstruction to expose clearance.
- Enlarge a sturdy prop into a step, platform, cover object, or LOS blocker.
- Change an opening or mechanism footprint when the editable Actor is clearly distinct from structural geometry.
- Reconfigure cover to create a safe manipulation position.
- Combine spatial footprint with Gravity or Mass so size and behavior remain separate choices.

Safeguards:

- Provide clearance envelopes for every preset the target exposes as meaningful.
- Keep the target selectable at small scale through placement, contrasting silhouette, and nearby access; do not require pixel hunting.
- Prevent enlarged props from sealing all exits or permanently covering essential controls.
- Do not rely on uncontrolled penetration, launch impulses, or clipping as intended solutions.
- Use containment and secondary paths so a scaled simulated prop cannot leave the playable space.
- Avoid “shrink everything” capability assignment. Scale belongs on objects whose changed footprint creates a comprehensible affordance.
- Treat object Scale as the current Demo contract; do not design around player shrinking unless separately implemented and approved.

### Gravity

Strong Gravity patterns:

- Float/reposition an eligible simulated prop through an opening.
- Reduce falling behavior long enough to place or traverse around a prop.
- Remove a vertical obstruction by changing how a loose object settles.
- Build a temporary spatial arrangement with Scale + Gravity.
- Create distinct advanced handling with Mass or Friction where motion is immediately observable.

Safeguards:

- Gravity targets must actually simulate physics and have room to move; never advertise it on static decoration.
- The geometry must include floors, rails, walls, catch pits, or return channels rather than open voids.
- Critical props need redundant instances, a non-prop alternate route, or a recoverable reset path.
- Low and Zero need visually different outcomes within a few seconds.
- Do not imply player gravity, reverse gravity, or automatic lifting; the current presets are Normal, Low, and Zero on eligible world primitives.
- Never require exact unstable stacking or precision Chaos behavior for critical progression.

### Time

Good targets are rotating scanners, authored sweeping cameras, moving barriers/platforms, periodic hazards, and machinery whose normal movement derives from Actor-local tick delta.

Strong patterns:

1. **Window extension.** Slow a repeating scanner or moving barrier so an existing safe interval becomes traversable. A normal-speed reference establishes that only one Actor changed.
2. **Cycle desynchronization.** Two synchronized machines jointly block a route; changing one Actor's cadence creates recurring openings. The solution can use slow or fast presets rather than one prescribed multiplier.
3. **Phase acceleration.** Speed a mechanism through an obstructing phase, then Restore when it reaches a favorable part of its authored cycle. Critical progression must tolerate different phase timing.
4. **Observation tradeoff.** A Direct Time edit occurs under a camera; a Careful route reaches an occluded control angle or uses physical cover created earlier.

Boundaries:

- Do not design around global bullet time, player slowdown, Chaos rigid-body slow motion, universal timers, audio pitch, or universal particles/animation.
- A Time target must visibly move before the player opens the console.
- Keep a reference rhythm, paired machine, sweep light, or repeated spatial marker visible.
- Avoid frame-perfect timing. Preset differences should be forgiving and obvious.
- Time targets must remain reacquirable at 0.25× through 4× and after the player crosses them.

### Mass and Friction

Use one optional alcove branching from Beat 4 and reconnecting before Beat 5.

- **Mass purpose:** make a simulated prop meaningfully easier/harder to accelerate, push, or use in a force/impact comparison.
- **Friction purpose:** make the same or paired prop slide, grip, travel down a ramp, or stop in a marked zone.
- **Best setup:** a short ramp/transfer bay with a visible force source or reliable initial motion, two outcome zones, and an optional shortcut/reward.
- **Required readability:** baseline attempt must establish behavior before the player edits; Zero/High Friction and light/heavy Mass must produce clearly different outcomes.
- **Progression rule:** failure or skipping the alcove cannot block the main route. No Mass/Friction knowledge is required at the exit.
- **Evaluation gate:** if blind testers need an explanation of why the prop moved differently, cut the opportunity from the public Demo rather than adding tutorial text.

## Witness grammar

Witnesses are spatial constraints, not invisible global multipliers.

### Placement patterns

- **Obvious direct-route camera:** one readable cone/facing direction covers the shortest route and target.
- **Partial-room Human:** watches one work zone while cover and facing leave another safe manipulation position.
- **Overlapping zones:** reserved for synthesis; overlap creates a high-risk center while at least one edge route remains readable.
- **Cover-created safety:** a scalable or movable prop can break LOS, making physical manipulation part of observation planning.
- **Witness-to-target relationship:** place the Witness so the target event location, not merely the player, is clearly inside or outside observation.

### Rules

- Do not fill every room with Witnesses; Beats 1 and the Beat 4 setup need safe experimentation.
- Avoid hidden observers, ambiguous facing, extreme ranges, or LOS through visually solid cover.
- Introduce one Witness type at a time before overlap.
- Every critical observed target needs a readable way to accept, avoid, or mitigate observation.
- Stationary Witnesses are acceptable in the graybox, but routes must not require memorizing invisible numeric cones.
- A Witness noticing Mass/Friction should coincide spatially with the visible consequence; do not imply sophisticated inference at a distant point.

## Context grammar

Context is an environmental event/state with physical, visual, and narrative meaning. Prototype switches may set semantic tags, but the space must communicate what state changed, which systems it concerns, and whether it is active.

### Current implemented rules — safe to design around

- **SensorFailure reduces Collision suspicion by 15.** Present it as a failure in access/security sensing around the relevant corridor.
- **Explosion reduces Gravity suspicion by 15.** Present nearby disturbance and displaced-object plausibility.
- **Explosion reduces Scale suspicion by 10.** Use cautiously on appropriate damaged/deformed objects; it does not make arbitrary scaling normal.
- **Fire reduces Gravity suspicion by 5.** It is only a small specific reduction, not broad permission.
- **Blackout currently has no configured plausibility reduction.** It may exist as environmental presentation but must not be promised as a numeric mitigation.

### Presentation principles

- Activating Context changes visible world state: warning lights, machine status, smoke/fire/explosion aftermath, sensor indicators, or powered/unpowered presentation in later art passes.
- Context sources sit in meaningful infrastructure and may require a detour, exposure, or interaction cost.
- The player can infer the relationship before or shortly after testing it; Context is not a hidden tag-matching quiz.
- Do not place a clearly labeled Context button immediately beside its matching obstacle with no tradeoff.
- Context never suppresses Witness notification under current rules; it changes configured plausibility reduction only.

### Future high-value possibilities — not implemented or promised

- Blackout reducing Camera confidence or availability.
- Fire changing Human attention/evacuation and visibility.
- Machine malfunction or sensor desynchronization explaining specific Time events.
- Oil/spill explaining Friction or load-sensor failure explaining Mass.

These are backlog candidates only. Task 017 must use current rules unless a separate implementation task is approved.

## Suspicion pacing with current tuning

Current Apply values are Collision +25, Scale +20, Gravity +15, Mass +10, Friction +10, and Time +20. Human adds +10, Camera adds +15, and explicit Restore applies -5. Reality enters Questioning at 20, Investigating at 40, Unstable at 60, and Correction at 80.

### Pacing target

- **Careful first-time player:** normally exits below Correction, commonly in Investigating or Unstable after several required experiments.
- **Reckless player:** can reach Correction during Beat 5 through repeated observed/direct edits.
- **Experimenting player:** can test each new Primary once in safe or mitigated conditions without being immediately forced out of experimentation.

### Beat allocation

- **Beat 1:** safe mandatory experiment. Questioning is a lesson, not failure.
- **Beat 2:** first observed/unobserved comparison. Provide cover and allow one correction through Restore.
- **Beat 3:** demonstrate that Context changes the outcome of a high-risk event.
- **Beat 4:** safe setup zone supports physical experimentation; observation pressure appears only near the route payoff.
- **Beat 5:** deliberate high-risk synthesis where accumulated choices can reach Correction.

The graybox must not add automatic punishment for crossing a state threshold. Current Correction has no gameplay consequence; its later behavior requires a separate approved task. Until then, states and readout are feedback, not a fail condition.

Avoid requiring repeated Apply/Restore spam merely to discover the meaningful preset. Composition and motion should narrow the likely useful choices while preserving experimentation.

## Restoration grammar

Restore is a planning verb, not only a -5 transaction.

- Restore a door after crossing so an observed corridor returns to its expected state and the return route changes.
- Restore enlarged cover after using it so it no longer blocks a control, route, Witness view, or moving mechanism.
- Restore Gravity after placing a prop so normal settling/support resumes without rewinding its transform.
- Restore Time after crossing so machinery returns to baseline cadence and a later route remains usable.
- Keep one property active while restoring another to prove state independence, especially Scale + Gravity.
- Clean up before entering a newly observed space, creating a decision between speed and evidence/state management.
- Never require restoration from a side where the target cannot be reacquired.

The level must not claim that Restore erases history or all evidence. It is cleanup and a configured -5 reduction, not an undo of the event.

## Seven-space Demo room plan

### Space 1 — Routine Intake

- **Purpose:** establish normal first-person interaction, office/service setting, objective, and a blocked route.
- **Mechanic:** no Reality edit initially; discover F6 and the first editable barrier.
- **Witness:** none.
- **Context:** none.
- **Primary editable Actors:** one Collision-enabled maintenance barrier at the exit of the space.
- **Alternatives:** inspect/interact with ordinary props; optional short normal access clue if cheap to graybox.
- **Transition:** pass and optionally Restore the barrier into the storage threshold.

### Space 2 — Observed Storage

- **Purpose:** teach Scale and show that observation changes evidence.
- **Mechanic:** shrink obstruction or enlarge step/cover.
- **Witness:** one Human or Camera with obvious facing and partial coverage.
- **Context:** none.
- **Primary editable Actors:** scalable crate/step, one deliberately risky Collision barrier, fixed cover.
- **Alternatives:** watched direct Collision; Scale from cover; Scale to create cover/step; careful timing/LOS route.
- **Transition:** upper/service opening and main threshold reconnect into the security corridor hub.

### Space 3 — Security Corridor Hub

- **Purpose:** pose the high-risk direct route and reveal the Context detour.
- **Mechanic:** observed Collision choice.
- **Witness:** one clearly oriented security camera covering the direct door.
- **Context:** SensorFailure is visible as available but activated in Space 4.
- **Primary editable Actors:** security door with Collision, cover object with Scale if needed.
- **Alternatives:** direct watched door; use cover/angle; enter maintenance loop; later return under SensorFailure.
- **Transition:** direct door reaches Space 5 quickly; maintenance entrance leads to Space 4 and reconnects at the same door/landing.

### Space 4 — Maintenance Loop

- **Purpose:** teach Context as a world state and provide the required maintenance route.
- **Mechanic:** E interaction with SensorFailure infrastructure; optional normal bypass back to the corridor.
- **Witness:** none or one Human watching only the approach, never the Context control and exit simultaneously.
- **Context:** SensorFailure source and clear active-state presentation placeholder.
- **Primary editable Actors:** maintenance panel/door; no new ability required.
- **Alternatives:** activate SensorFailure and return; traverse the longer maintenance bypass without using the watched door; ignore it and accept direct risk.
- **Transition:** reconnect at the secure side/corridor hub, controlling content cost.

### Space 5 — Logistics Well

- **Purpose:** teach Gravity and combine it with Scale in a multi-solution physical room.
- **Mechanic:** reposition and resize recoverable simulated props.
- **Witness:** safe setup area; one partial Witness near the final ledge only.
- **Context:** optional visible Explosion source only if Task 017 can present it as an environmental state using existing prototype components; otherwise none.
- **Primary editable Actors:** two bounded simulated props supporting Scale + Gravity, one scalable obstruction, raised ledge/side opening.
- **Alternatives:** enlarge step; float and place prop; shrink obstruction; combine Scale + Gravity; use the maintenance edge route.
- **Transition:** all routes reconnect at the machinery access landing.

### Space 6 — Physics Service Alcove (Optional)

- **Purpose:** offer advanced Mass/Friction experimentation without blocking completion.
- **Mechanic:** alter push/inertia and slide/grip on a short ramp or transfer lane.
- **Witness:** none.
- **Context:** none.
- **Primary editable Actors:** one or two contained simulated props supporting Mass/Friction; reliable initial force/push affordance.
- **Alternatives:** skip entirely; use Mass, Friction, or both to reach an optional shortcut, observation point, or nonessential reward.
- **Transition:** reconnect to the machinery access route before Space 7.

### Space 7 — Desynchronization Gate

- **Purpose:** reveal Time, synthesize the Demo vocabulary, and deliver the exit/core terminal.
- **Mechanic:** local Time presets on visibly repeating security machinery.
- **Witness:** one Camera covering the direct center plus a partial Human or second Camera only if overlap remains readable. Maximum recommended active Witness count in this space: two.
- **Context:** reuse currently active SensorFailure only for Collision events if its spatial fiction remains credible; do not claim it reduces Time. No new Time Context.
- **Primary editable Actors:** two synchronized tick-driven scanners/moving barriers (at least one Time-capable), one optional Scale/Collision alternate target, Reality exit/core terminal.
- **Alternatives:** slow scanner; accelerate barrier phase; use cover/timing; earlier spatial ability route; risky direct Collision on a designated barrier.
- **Transition:** terminal interaction completes the Demo. It remains reachable and valid regardless of prior edits.

### Route topology

The risky direct route passes from Space 3 to Space 5. The maintenance and Context route loops through Space 4 and reconnects at the same hub/secure side. Space 6 branches from Space 5 and rejoins before Space 7. This produces alternatives without building three separate content tracks.

## Softlock and recovery rules

1. Required physics objects cannot fall into voids, leave streaming bounds, or become permanently unreachable.
2. No single movable prop is the only unrecoverable progression key; provide redundancy, containment, or an alternate route.
3. Critical traversal has a non-physics recovery route back to the manipulation side.
4. Scale clearances support authored presets and cannot permanently trap the player between geometry and an enlarged Actor.
5. Tiny Scale targets remain selectable; giant targets cannot cover every exit/control.
6. Collision targets required for cleanup are reacquirable from both sides, including while collision is disabled.
7. Gravity rooms use catch floors/rails and avoid precision stacking as a mandatory solution.
8. Time machinery is non-lethal or resettable in graybox, remains targetable at every preset, and cannot stop progression because of phase.
9. The Demo exit/core terminal has no Reality-editable capability that can invalidate completion.
10. Optional Mass/Friction objects never gate the main path.
11. High Suspicion/Correction does not hard-fail or lock the run under current implementation.
12. Every branch reconnects before the final gate, and no route permanently closes the others before the player commits safely.

## Player guidance

### Explicitly communicate

- The current objective in short world/UI language.
- `F6` opens/closes the Developer Console when it first becomes available.
- `E` interaction prompt when a required control or terminal is focused.
- Restore as a distinct action during the first Collision edit.

Do not explicitly list puzzle solutions, preferred presets, Witness equations, or every alternate route.

### Communicate through the space

- **Composition:** frame objective, obstacle, editable target, and at least one alternate path in the same visual problem.
- **Motion:** establish Gravity and Time targets before editing; repeated movement teaches cadence.
- **Lighting:** separate main route, maintenance route, cover, Context infrastructure, and exit without turning them into colored key/lock codes.
- **Labels:** use restrained operational language—maintenance, sensor status, access, machinery cycle—not tutorial prose.
- **Witness direction:** camera body/lens, sweep direction, Human facing, and visible cover establish LOS.
- **Environmental storytelling:** Context sources belong to plausible systems and visibly affect their local environment.
- **Comparison:** paired props or machines show baseline versus edited behavior for hidden/temporal properties.

## Art and readability requirements from gameplay

- Editable targets need a consistent Reality-specific marking/state language distinguishable from ordinary interactables and structural geometry.
- Capability differences must be readable through console metadata and, where practical, object category/hardware—not arbitrary identical-object exceptions.
- Collision OFF needs subtle world-state feedback without making the Actor visually disappear.
- Scale needs clear original-size references and safe silhouettes at useful presets.
- Gravity props need shadows, contact cues, room to float/fall, and containment geometry.
- Time machinery needs visible cycles, sweep lights/arms/barriers, and a baseline reference.
- Cameras need unmistakable facing direction and later a readable observation/sensor state.
- Human Witnesses need readable facing/attention even with minimal animation.
- LOS blockers and cover must look solid and match collision.
- Context sources need recognizable infrastructure and active/inactive state presentation.
- SensorFailure must visually relate to the security systems whose event it can help explain.
- Explosion/Fire presentation, if used, must read as a world event rather than a labeled toggle.
- Route hierarchy, maintenance access, reconnect points, and the final terminal must be legible without waypoint spam.
- Mass/Friction setups require visible force, motion paths, ramp markings, and outcome zones.

These are requirements for later art direction and asset acquisition, not authorization to select or create final assets now.

## Minimum asset needs

### Generic — suitable for asset libraries or existing engine/project content

- Modular office/service walls, floors, ceilings, doors, windows, and trim.
- Storage/logistics props, crates, pallets, railings, ramps, pipes, vents, and maintenance clutter.
- Generic lights, warning fixtures, cables, electrical cabinets, and industrial machinery shells.
- Basic Human placeholder and generic environmental audio/VFX placeholders.
- Cover objects and structural LOS blockers.

### Custom — likely needs original Reality.exe language

- Developer access terminal/panel and the final Reality exit/core terminal.
- Reality-editable target marking and active-property state language.
- Developer Console presentation assets when the prototype UI advances.
- Security camera/scanner with explicit facing, observation, and sensor-failure states.
- SensorFailure Context panel/machinery with clear system linkage.
- Tick-driven Time scanner/moving security mechanism designed around the current local-time contract.
- Reality Suspicion/evidence feedback that communicates source and consequence spatially.
- Minimal Context state presentation for any Explosion/Fire condition used in the Demo.

“Custom” means identity/readability likely benefits from original design. It does not require every mesh to be bespoke; modular geometry can remain generic while overlays, motion language, signage, and states carry the game's identity.

## First blind playtest criteria

A successful first blind test does not require every tester to use every ability. Record route, edits, Context states, Witness exposure, completion time, confusion points, and any designer intervention.

Success criteria:

- Tester understands the current physical objective in each major beat without verbal explanation.
- Tester discovers and uses F6 without external coaching after the intended reveal.
- Tester completes the first edit and understands explicit Restore.
- Tester uses at least one Primary ability other than Collision to make progress.
- Tester notices that a Witness changed the evidence outcome or deliberately reasons about LOS.
- Tester intentionally activates and uses a matching Context at least once, or can accurately explain why it would help after observing the result.
- Tester identifies at least one alternate route or produces one legitimate emergent solution.
- Tester understands Time as local machinery cadence, not global slow motion.
- Tester reaches the exit without designer-only knowledge or an unrecoverable state.
- Tester is never softlocked by lost/scaled physics props or machine phase.
- First run completes in 20–30 minutes; replay/experienced route plausibly fits 10–15 minutes.
- Console operation supports decisions rather than consuming more attention than the rooms themselves.
- Careful play normally stays below Correction; reckless repeated observed edits can reach it by the final beat.

## Failure signals

The graybox requires revision if any of these patterns recur:

- Collision solves nearly every obstacle with no meaningful tradeoff.
- Players assume all walls/floors are editable or cannot predict target eligibility.
- Players never notice Witnesses, cannot read facing/LOS, or attribute evidence changes randomly.
- Context is described as a score-reduction button rather than a world explanation.
- Players cannot see Scale/Gravity affordances until told the intended solution.
- Players expect Gravity to affect themselves or expect reverse gravity.
- Players interpret Time as global slow motion or test it primarily on Chaos props.
- Players spend more time opening, scrolling, and reacquiring the Console than reasoning about space.
- A single lost or badly scaled prop blocks completion.
- Mass/Friction confuse onboarding, appear mandatory, or cannot be understood from visible consequences.
- Players follow one obvious corridor and never perceive the maintenance/Context loop.
- Restoration reads as event undo rather than state cleanup.
- Careful first-time players routinely enter Correction well before the synthesis room.
- Testers need designer coaching to find F6, the objective, or the exit.

## Demo scope lock

The public Demo does not require:

- All five chapters or a condensed full-game story.
- Reality Inspector.
- Debugger appearance or behavior, despite the older project-plan flow sketch.
- Final endings or ending qualification.
- Full NPC AI, schedules, patrol simulation, combat, or stealth takedowns.
- Mass or Friction tutorials or mandatory progression.
- New Reality abilities, arbitrary property editing, or player-property editing.
- New Context rules or retuned Suspicion values.
- Correction gameplay consequences unless separately approved.
- Final Developer Console redesign.
- Final art quality, cinematics, final VFX/audio, or broad asset acquisition.
- Full-game save persistence, inventory, crafting, weapons, or combat.
- Separate levels for every alternate route.
- Changes to P0 as a substitute for building the dedicated Demo graybox.

If schedule pressure appears, remove the optional physics alcove, decorative beats, and extra route dressing before weakening the four-Primary progression or systemic clarity.

## Task 017 Public Demo Graybox input

Task 017 should build one dedicated compact graybox map from this specification:

- **Seven meaningful spaces:** Routine Intake, Observed Storage, Security Corridor Hub, Maintenance Loop, Logistics Well, optional Physics Service Alcove, and Desynchronization Gate.
- **Mechanic order:** Collision → Scale + Witness → SensorFailure/Collision plausibility → Scale + Gravity multi-solution room → local Time synthesis.
- **Expected playtime:** 20–30 minutes blind; 10–15 minutes replay.
- **Required player systems:** existing First Person character, E interaction, F6 Developer Console, Reality Manager/readout, restoration, and Prototype Exit/core interaction.
- **Required editable prototypes:** two-sided Collision barrier/door; scalable obstruction/step/cover; two bounded simulated Scale + Gravity props; optional Mass/Friction ramp props; tick-driven Time scanner/moving barrier; non-editable structural geometry.
- **Witness budget:** three core Witness placements across the route—one in Observed Storage, one Camera at the Security Corridor, and one at the final gate—with at most one additional overlapping final Witness if LOS remains obvious. Beats 1 and the Beat 4 setup remain safe.
- **Context sources:** one required SensorFailure source in the maintenance loop. At most one optional Explosion source in the logistics room if it can be presented honestly with current rules. Do not claim Blackout affects evidence and do not add a Time Context.
- **Time machinery:** at least two visibly synchronized tick-driven mechanisms, one normal reference and at least one `Cheat.Time` target; no Chaos-dependent Time puzzle.
- **Route structure:** risky direct corridor and Context-assisted maintenance loop reconnect before the Logistics Well; optional Mass/Friction alcove reconnects before the final gate.
- **Alternate-solution requirement:** each post-tutorial main space supports at least two valid approaches; the final gate supports Direct, Careful, and at least one non-Time or no-edit timing alternative where practical.
- **Recovery requirements:** contained physics, redundant/alternate traversal, reacquirable targets, non-invalidatable exit, and no hard failure at Correction.
- **Assets:** graybox primitives and existing prototype actors only unless Task 017 explicitly approves narrowly required reusable prototype glue. No final-art acquisition.
- **Validation focus:** blind discoverability, Collision dominance, Witness/Context comprehension, local-Time semantics, console operation cost, route reconnection, and softlock resistance.

Task 017 should not silently expand the scope. If the current reusable actors cannot support a required graybox behavior, report the exact missing prototype glue before implementing unrelated systems.
