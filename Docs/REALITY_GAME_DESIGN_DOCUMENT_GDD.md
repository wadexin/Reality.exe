# Reality.exe — Game Design Document (GDD)

> Working Title: **Reality.exe**  
> Genre: First-Person Systemic Puzzle / Light Stealth / Immersive Sim  
> Engine: Unreal Engine 5  
> Target Platform: PC  
> Target Playtime: 2–3 Hours  
> Target Version: 1.0  
> Development Model: Solo Developer + Codex-Assisted Engineering  
> Core Tagline: **You may cheat. Just don't get caught by reality.**

---

# 1. Document Purpose

This document defines the design vision, player experience, gameplay rules, content structure, level design standards, narrative direction, UX principles, visual language, audio direction, progression, failure states, and success criteria for **Reality.exe**.

This document answers:

- What is the player fantasy?
- What does the player do minute to minute?
- What systems create the game’s unique identity?
- How should puzzles be designed?
- What makes a solution “good”?
- How does the world react to cheating?
- How do chapters escalate?
- How should the player feel at each stage?
- What should never be added if it weakens the core?

This document should be treated as the primary design reference for all gameplay decisions.

For production scheduling, engineering ownership, milestone planning, and Codex implementation rules, refer to:

`REALITY_CODEX_PROJECT_PLAN.md`

---

# 2. High Concept

**Reality.exe** is a first-person puzzle game where the player gains access to a hidden Developer Console that can modify selected properties of the simulated world.

The player can directly alter values such as:

- Collision
- Scale
- Gravity
- Mass
- Friction
- Time Scale

The world, however, contains an automated **Reality Validation System**.

The game does not ask:

> “Can the player cheat?”

The player is explicitly allowed to cheat.

The game asks:

> “Can the player cheat without making reality conclude that something impossible happened?”

The player must therefore:

1. Observe the environment.
2. Understand what the simulation currently considers plausible.
3. Create environmental context.
4. Modify reality.
5. Avoid witnesses or create explanations.
6. Complete objectives.
7. Adapt when Reality begins correcting anomalies.

---

# 3. Elevator Pitch

A locked door blocks your path.

You could find the key.

Or disable its collision.

Or shrink yourself and crawl underneath.

Or reduce gravity and jump through the upper window.

Or trigger a fire alarm so everyone evacuates, cut the lights, disable the door collision, walk through, restore it, then turn the power back on.

The trick is not breaking the rules.

The trick is making the system believe you did not.

---

# 4. Core Player Fantasy

The player fantasy evolves across the game.

## Stage 1 — Power

> “I can modify the game.”

The player experiences immediate joy from breaking obvious restrictions.

---

## Stage 2 — Consequence

> “The world notices impossible things.”

The player learns that raw cheating creates suspicion.

---

## Stage 3 — Deception

> “I can create a plausible explanation.”

The player begins planning around witnesses and environmental context.

---

## Stage 4 — Exploitation

> “I can manipulate the meaning of the rules.”

The player stops thinking only about object parameters and starts exploiting how the validation system interprets events.

---

## Stage 5 — Mastery

> “I am not solving puzzles. I am negotiating with reality.”

Late-game puzzles become arguments against system definitions.

---

# 5. Design Pillars

Every major feature should support at least one of these pillars.

## 5.1 Rule Manipulation

The player modifies world rules directly.

The Developer Console must feel like a real internal tool, not a magical spell wheel.

---

## 5.2 Systemic Problem Solving

Obstacles should support several valid solutions.

The player should frequently ask:

> “Would this weird idea actually work?”

The game should reward experimentation.

---

## 5.3 Plausibility

The game judges whether an event appears explainable within the current world state.

This distinguishes Reality.exe from a standard cheat-power game.

---

## 5.4 Reactive World

NPCs, security systems, Inspectors, and Debuggers respond to anomalies.

The world should feel like it is interpreting the player.

---

## 5.5 Escalating Rule Literacy

The game should become more intellectually complex without constantly adding new mechanics.

Later challenge comes from understanding combinations and constraints.

---

# 6. Player Experience Goals

The ideal player repeatedly experiences the following emotional sequence:

```text
Curiosity
↓
Experimentation
↓
Surprise
↓
Confidence
↓
Suspicion / Pressure
↓
Planning
↓
Clever Exploit
↓
Relief
↓
"I can't believe that worked"
```

The target reaction is not:

> “I found the correct puzzle solution.”

It is:

> “I invented a solution.”

---

# 7. Core Gameplay Loop

```text
Observe
↓
Identify obstacle
↓
Inspect editable objects
↓
Form a plan
↓
Create useful context
↓
Modify one or more properties
↓
Execute
↓
World evaluates anomaly
↓
Player handles consequences
↓
Objective progresses
```

This loop should usually complete in 30 seconds to 5 minutes depending on puzzle scale.

---

# 8. Basic Player Actions

The player can:

- Walk
- Look
- Crouch
- Jump
- Interact
- Pick up selected physical props
- Inspect objects
- Open Developer Console
- Modify permitted properties
- Observe NPC behavior
- Trigger environmental devices
- Use doors, switches, alarms, terminals
- Restore changed properties

No combat system is required for v1.0.

---

# 9. Developer Console

## 9.1 Role

The Developer Console is the game’s central interaction system.

It must communicate:

> “You are accessing forbidden development permissions.”

It should not feel like:

- Magic
- Hacking minigame
- RPG skill menu

It should feel like:

- Debug tooling
- Editor property inspection
- Internal simulation interface

---

## 9.2 Activation

Default input:

`F1`

When activated:

- Player movement slows or pauses depending on context.
- Editable objects become visually readable.
- Focused object displays technical metadata.
- Available properties are shown.
- Locked properties remain visible where useful.
- The world gains subtle debug/editor visualization.

Example:

```text
OBJECT: ManagerDoor_03

TYPE: Object.Door

Transform
Scale:      1.00

Physics
Collision:  ON
Mass:       52.0 kg
Gravity:    DEFAULT

System
Locked:     TRUE       [ACCESS DENIED]
```

The player cannot edit every exposed property.

This helps reinforce that they have limited developer permissions rather than omnipotence.

---

# 10. Editable Property System

Target v1.0 supports six main property families.

---

## 10.1 Collision

Possible values:

```text
ON
OFF
```

### Uses

- Walk through locked doors.
- Walk through barriers.
- Remove collision from props.
- Cause unsupported objects to fall.
- Bypass vents or security obstacles.

### Risk Profile

High.

Directly violating collision is visually obvious.

### Typical Validation

```text
Player crossed solid wall:
High suspicion.

Door temporarily lacked collision with no witness:
Low suspicion.

Prop fell through floor:
Medium / High suspicion.
```

---

## 10.2 Scale

Preset values:

```text
0.25x
0.5x
1.0x
2.0x
4.0x
```

### Uses

- Shrink player.
- Enlarge vents.
- Create platforms from props.
- Shrink obstacles.
- Block routes with enlarged objects.

### Risk Profile

Medium to very high depending on observation.

A static object being discovered at a different size may be suspicious.

A human visibly changing size is extremely suspicious.

---

## 10.3 Gravity

Possible states:

```text
Normal
Low
Zero
Reverse
```

Reverse is late-game.

### Uses

- Reach high locations.
- Move heavy props.
- Float objects.
- Reconfigure rooms.
- Create temporary traversal paths.

### Risk Profile

Medium.

Effects can sometimes be explained by contextual events.

---

## 10.4 Mass

Use preset or constrained range rather than arbitrary values.

### Uses

- Push heavy objects.
- Modify weight sensor behavior.
- Change physical impact.
- Create launch / counterweight setups.

### Risk Profile

Usually low until visibly impossible behavior results.

Mass itself is invisible; consequences are what matter.

---

## 10.5 Friction

Possible presets:

```text
High
Normal
Low
Near-Zero
```

### Uses

- Slide crates.
- Create moving-object routes.
- Alter carts / vehicles.
- Create distractions.
- Redirect objects.

### Risk Profile

Low to medium.

Often highly useful for subtle solutions.

---

## 10.6 Time Scale

Localized where possible.

Example values:

```text
1.0
0.5
0.25
0.1
```

### Uses

- Slow NPCs.
- Extend timing windows.
- Slow moving security systems.
- Manipulate environmental cycles.

### Risk Profile

High if directly observed.

The system should strongly monitor persistent time manipulation.

---

# 11. Property Restoration

A core rule:

**The player should usually be able to restore a modified property.**

This supports:

- Covering evidence.
- Clever temporary manipulation.
- Strategic cleanup.
- Low-suspicion play.

Example:

```text
Door Collision OFF
↓
Player crosses
↓
Door Collision ON
```

This should generally create less long-term suspicion than leaving the door invalid.

---

# 12. Reality Suspicion

## 12.1 Definition

Reality Suspicion represents how strongly the simulation believes that recent events violate expected reality.

Range:

```text
0–100
```

It is not:

- Wanted level
- Morality
- Crime
- Player health

---

## 12.2 States

### 0–20: Stable

World behaves normally.

---

### 20–40: Questioning

Subtle anomalies appear.

Examples:

- NPC glances.
- Lights flicker.
- Monitor glitches.
- Strange ambient audio.

---

### 40–60: Investigating

The system begins active observation.

Examples:

- Cameras turn toward anomalies.
- NPCs inspect nearby changes.
- Validation messages appear.
- Inspector may be introduced.

---

### 60–80: Unstable

Reality visibly struggles.

Examples:

- Repeating NPC animation.
- Material glitches.
- Brief physics anomalies.
- Debug text.
- Audio desynchronization.

---

### 80–100: Correction

The simulation takes corrective action.

Possible consequences:

- Restore an object.
- Lock a property.
- Spawn / activate Debugger.
- Change active validation rules.
- Force alternate route.
- Reset a local puzzle state.

Do not automatically kill the player.

---

# 13. Suspicion Calculation

Each meaningful cheat creates a Reality Event.

Conceptual data:

```text
CheatType
Magnitude
TargetType
WitnessCount
WitnessType
ContextTags
Duration
Restored
Evidence
CurrentValidationRules
```

Example:

```text
Event:
Player passes through solid door

Base Suspicion:
+25

Human witness:
+25

Camera witness:
+15

Blackout active:
-10

Property restored:
-5

Final:
+50
```

The exact numbers should remain tunable.

---

# 14. Plausibility

Plausibility is the game’s signature concept.

An anomaly becomes less suspicious when the current environment offers a believable explanation.

Example:

```text
Car flies through air
+
Explosion nearby
=
Possible
```

But:

```text
Human instantly shrinks
+
Explosion nearby
=
Still impossible
```

Therefore context relationships must be authored by cheat type.

Avoid universal modifiers such as:

```text
Fire Alarm always reduces suspicion by 30%.
```

Prefer:

```text
Fire Alarm reduces witness quality.
Explosion explains high-velocity physical objects.
Blackout reduces camera reliability.
Flood explains electrical malfunction.
Construction explains moved walls / blocked paths.
```

---

# 15. Context System

Core Context Tags:

```text
Context.Fire
Context.Explosion
Context.Blackout
Context.Flood
Context.Panic
Context.Construction
Context.SensorFailure
Context.Evacuation
Context.PowerFailure
```

Contexts can affect:

- Witness availability
- Sensor accuracy
- NPC attention
- Plausibility
- Route access
- System validation confidence

---

# 16. Witness System

Witnesses do not need human-level reasoning.

They output structured observations.

Example:

```text
WitnessReport

Type:
Human

Event:
Saw.Player.EnterSolidWall

Confidence:
1.0

Distance:
8m

Obstruction:
None
```

Reality Manager interprets the report.

---

# 17. NPC Types

## 17.1 Civilian / Office Worker

Behavior:

- Follows schedule.
- Reacts to nearby disturbances.
- Reports impossible events.
- Evacuates during alarms.

States:

```text
Normal
Concerned
Suspicious
Panicked
```

---

## 17.2 Security NPC

Behavior:

- Patrols.
- Investigates alarms.
- Uses access-controlled doors.
- Reacts faster to anomalies.

No complex combat required.

Security mainly changes space control.

---

## 17.3 Reality Inspector

Introduced mid-game.

Behavior:

- Observes recurring anomaly types.
- Activates validation rules.
- Investigates player-favored strategies.
- Does not fully accept weak contextual explanations.

Fantasy:

> The system has started learning how you cheat.

---

## 17.4 Debugger

Primary systemic antagonist.

Appearance:

- Dark humanoid silhouette.
- Face replaced by display / validation surface.
- Minimal animation.
- Clean, unnatural motion.

Possible display text:

```text
VALIDATING...
```

Capabilities:

- Scan player.
- Restore properties.
- Lock permissions.
- Detect invalid states.
- Enforce validation rules.
- Reconstruct parts of a level.

Debugger should feel procedural rather than emotional.

---

# 18. Active Validation Rules

Late-game challenge is created through explicit temporary rules.

Examples:

```text
Validation.PlayerScale = ACTIVE
Validation.PlayerSpeed = ACTIVE
Validation.Collision = ACTIVE
Validation.Gravity = ACTIVE
Validation.ObjectState = ACTIVE
```

If a monitored value becomes invalid, Debugger immediately reacts.

This forces indirect solutions.

Example:

Forbidden:

```text
Player Scale → 0.25
```

Alternative:

```text
Vent Scale → 4.0
```

---

# 19. Puzzle Design Philosophy

Every puzzle begins with an objective, not a required method.

Bad puzzle description:

> “Use gravity to reach the window.”

Good puzzle description:

> “Enter the manager’s office.”

Possible solutions:

- Find key.
- Disable collision.
- Shrink player.
- Enlarge vent.
- Use low gravity.
- Build a platform.
- Get an NPC to open the door.
- Trigger evacuation.
- Manipulate physical objects.

---

# 20. Puzzle Layers

Strong Reality.exe puzzles contain several layers.

## Layer 1 — Physical Objective

Example:

Enter room.

---

## Layer 2 — Observation

Who is watching?

What objects are editable?

What context can be created?

---

## Layer 3 — Manipulation

Which rule can be changed?

---

## Layer 4 — Plausibility

Will Reality accept what happens?

---

## Layer 5 — Cleanup

Should the player restore modified values?

---

# 21. Puzzle Difficulty Progression

## Early Game

One property at a time.

Examples:

```text
Collision
Scale
Gravity
```

---

## Mid Game

Property + environmental context.

Example:

```text
Blackout
+
Collision
```

---

## Late Mid Game

Two properties + witnesses.

Example:

```text
Mass
+
Friction
+
Security Patrol
```

---

## Late Game

Validation rules + indirect manipulation.

Example:

```text
Player Gravity monitored
↓
Manipulate platform instead
```

---

## Final Game

Semantic / rule-lawyer puzzles.

Example:

```text
PLAYER MAY NOT CROSS BOUNDARY
```

Solution:

Move the boundary / world across the player.

---

# 22. Normal Solutions

Important:

Not every problem should require cheating.

Normal solutions should exist because they:

- Establish the world’s baseline rules.
- Help players understand what is “plausible.”
- Provide low-risk alternatives.
- Make cheating feel like a deliberate choice.

Examples:

- Find key.
- Wait for NPC.
- Use authorized elevator.
- Trigger legitimate door release.
- Follow maintenance path.

---

# 23. Emergent Solutions

Unexpected solutions are a feature.

If a player combines existing mechanics in a way the designer did not predict, preserve it unless it:

- Breaks progression.
- Causes save corruption.
- Softlocks the game.
- Completely trivializes most future content.
- Creates severe technical instability.

The preferred design response is:

> “That solution is weird, but the rules support it.”

---

# 24. Anti-Exploit Philosophy

This game should not aggressively patch every exploit.

The player is supposed to exploit the simulation.

The goal is to prevent:

- One universal solution.
- Accidental sequence breaking that destroys story state.
- Physics exploits that crash the game.
- Unrecoverable progression.

Do not prevent creativity merely because it was unintended.

---

# 25. Dominant Strategy Prevention

The biggest expected dominant strategy is:

```text
Collision OFF
```

Countermeasures:

- Witnesses.
- Cameras.
- Persistent evidence.
- Collision validation.
- Areas where target object is not editable.
- Paths that require moving objects rather than crossing walls.
- Debugger monitoring.
- High suspicion for direct player-wall violations.

Do not simply disable Collision everywhere.

---

# 26. Level Design Principles

## 26.1 Compact Spaces

Prefer dense, readable environments over large levels.

The player should understand:

- Who is watching.
- Where exits are.
- Which objects matter.
- Where environmental systems connect.

---

## 26.2 Circular Routes

Levels should often reconnect.

This supports:

- Planning.
- Repositioning.
- Multiple approaches.
- Context manipulation.
- Escape after a risky cheat.

---

## 26.3 System Visibility

The player should be able to infer relationships.

Example:

Power panel should visibly connect to:

- Lights.
- Cameras.
- Door sensors.

Avoid invisible puzzle logic.

---

## 26.4 Environmental Storytelling

The world should appear suspiciously organized and controlled.

Examples:

- Repeating office layouts.
- Identical employee desks.
- Perfect schedules.
- Corporate messaging about stability.
- Signs using language like “Consistency,” “Reliability,” and “Validation.”

---

# 27. Chapter Overview

Target total playtime:

2–3 hours.

---

# 28. Chapter 1 — Employee

## Location

NORMAL Systems Office.

## Target Length

25–30 minutes.

## Player Knowledge at Start

None.

## Introduced Mechanics

```text
Interaction
Developer Console
Collision
Scale
Gravity
Witnesses
Reality Suspicion
Basic Context
```

## Primary Objective

Reach the server room and restart a system.

## Story Beat

The player discovers they possess hidden Developer permissions.

## Central Puzzle

Obtain administrator access from the manager’s office.

Possible methods:

- Normal access.
- Door Collision.
- Player Scale.
- Vent Scale.
- Gravity via upper window.
- Fire alarm distraction.

## Chapter End

Server identifies player as:

```text
ENTITY 0427
DEVELOPER PERMISSION DETECTED
```

Then:

```text
PERMISSION REVOCATION FAILED
```

---

# 29. Chapter 2 — Invalid Identity

## Location

Underground facility / parking / logistics floor.

## Target Length

25–35 minutes.

## New Mechanics

```text
Mass
Friction
```

## Core Theme

Physical systems can be manipulated without obvious supernatural effects.

## Typical Puzzle

Vehicle or heavy object blocks route.

Possible solution:

```text
Mass ↓
Friction ↓
Push object
Restore values
```

## Narrative Beat

The player discovers that their employee file is incomplete.

Other employees have:

```text
Birth
Education
Employment
Family
```

Player has:

```text
Creation Date
```

The creation date is recent.

---

# 30. Chapter 3 — Museum

## Location

Public museum containing simulation-related technology.

## Target Length

30–40 minutes.

## New Mechanics

```text
Time Scale
Reality Inspector
```

## Primary Objective

Steal a Reality Core Fragment.

## Systems

- Guards
- Cameras
- Laser grid
- Display case
- Weight sensor
- Power control
- Fire alarm
- Ventilation
- Evacuation system

## Design Goal

This is the game’s flagship systemic sandbox.

The central heist should support at least five clearly different approaches.

## Narrative Beat

The player discovers evidence of previous awakened Developer Agents.

---

# 31. Chapter 4 — Validation

## Location

NORMAL Systems data infrastructure / restricted core facility.

## Target Length

25–35 minutes.

## Main Mechanic

Debugger + Active Validation Rules.

## Player Lesson

Direct manipulation is no longer enough.

The player must manipulate surrounding systems.

Examples:

```text
Cannot change Player Speed
→ Move floor instead
```

```text
Cannot change Player Scale
→ Enlarge opening
```

```text
Cannot disable target door collision
→ Disable support structure / move wall
```

## Narrative Beat

Debugger reveals the player is not supposed to understand the simulation.

---

# 32. Chapter 5 — Reality Core

## Location

Abstract simulation kernel.

## Target Length

20–30 minutes.

## Visual Style

- White void.
- Editor grid.
- Untextured meshes.
- Bounding boxes.
- Debug coordinates.
- System logs.
- Floating rule declarations.

## Gameplay

All earlier knowledge is recombined.

Rules become explicit statements.

Example:

```text
RULE:
PLAYER MAY NOT CROSS THIS BOUNDARY
```

The solution is not to violate the rule directly.

Possible approach:

```text
Move boundary across player.
```

Another:

```text
RULE:
DOOR MUST REMAIN CLOSED
```

Possible approach:

```text
Move the wall.
Resize surrounding geometry.
Move destination.
```

---

# 33. Ending Structure

## Ending A — FREE

Player deletes Reality Validation.

Result:

- Full freedom.
- Rules disappear.
- Simulation destabilizes.
- World collapses.

Final concept:

```text
FREEDOM: 100%
REALITY: 0%
```

Theme:

Freedom without structure destroys the world that supports it.

---

## Ending B — STABLE

Player removes their own Developer permissions.

Result:

- World stabilizes.
- Player returns to normal office life.
- F1 no longer works.

Final concept:

```text
REALITY STATUS:
STABLE

ENTITY 0427:
NORMAL
```

Theme:

Safety through surrender.

---

## Ending C — ADMIN

Preferred / hidden ending.

Requirement:

Player demonstrates controlled low-suspicion mastery during the game.

Instead of destroying the system or submitting to it, player changes:

```text
AccessLevel:
Developer Agent
```

to:

```text
Reality Administrator
```

Debugger accepts the new role.

Final concept:

```text
WELCOME, ADMINISTRATOR
```

Theme:

Maturity is not destroying rules; it is understanding and taking responsibility for them.

---

# 34. Narrative Premise

The surface world is a clean, predictable modern city.

The player works for:

**NORMAL Systems**

Corporate slogan:

> Making Reality Reliable.

The environment feels slightly too orderly.

Examples:

- Buses arrive at exactly the same second.
- Employees follow nearly identical routines.
- Weather changes predictably.
- Equipment rarely fails.
- Office spaces repeat.

Eventually the player discovers the truth:

The city is a simulation used to study intelligent agents capable of discovering rule exploits.

The player is:

```text
Developer Agent 0427
```

The player was designed to identify weaknesses in the simulation.

The unexpected development:

The player developed self-awareness.

---

# 35. Narrative Delivery

Avoid long exposition scenes.

Prefer:

- System logs.
- Terminals.
- NPC inconsistencies.
- Environmental repetition.
- Corrupted employee records.
- Debugger behavior.
- Short recorded messages.
- Corporate signage.
- Reality validation messages.

Narrative should be discovered through play.

---

# 36. Tone

Target tone:

- Dry humor.
- Corporate absurdity.
- Existential unease.
- Systemic comedy.
- Occasional surreal horror.

Avoid:

- Constant jokes.
- Heavy horror.
- Grimdark violence.
- Overly emotional melodrama.

The world should become unsettling because it is too rational.

---

# 37. Dialogue Style

NPC dialogue should be short.

Example:

Player passes through a wall.

NPC:

> “...Was that door open?”

Later:

> “No. I definitely saw that.”

Corporate message:

> “Reality inconsistencies should be reported to your supervisor.”

Debugger:

> “Observed state is invalid.”

Avoid verbose lore dumps.

---

# 38. Visual Direction

## 38.1 Normal Reality

Style:

- Clean.
- Minimal.
- Modern.
- Slightly sterile.
- Highly structured.

Materials:

- White.
- Gray.
- Neutral corporate colors.
- Controlled lighting.

The world should initially look believable but unnaturally orderly.

---

## 38.2 Developer Mode

Visual language inspired by:

- UE editor overlays.
- Transform information.
- Wireframes.
- Bounding boxes.
- Collision visualization.
- Property panels.
- Debug labels.

Avoid generic green hacker UI.

---

## 38.3 Reality Instability

As suspicion rises:

### Low

- Tiny light flicker.
- Audio glitch.
- UI timing error.

### Medium

- Texture pop.
- Repeated animation.
- Impossible shadow.
- Brief debug label.

### High

- Meshes lose materials.
- Geometry visibly corrects.
- NPC motion resets.
- Editor-grid surfaces appear.
- Audio fragments loop.

The world itself becomes the suspicion meter.

---

# 39. UI Design

Normal HUD should remain minimal.

Display only:

- Objective.
- Interaction prompt.
- Essential feedback.

Developer Console can be information-dense.

Reality Suspicion should not permanently dominate the screen.

Early game:

A visible meter may be useful for teaching.

Later:

Use increasingly environmental feedback.

---

# 40. UX Principles

## 40.1 Always Explain Editable State

The player should quickly understand:

- What object is selected.
- What can be edited.
- What is locked.
- What changed.
- What the original value was.

---

## 40.2 Preserve Experimentation

Undo / restore should be easy.

Players should not fear testing ideas because resetting is tedious.

---

## 40.3 Avoid Arbitrary Failure

When a solution fails, communicate why.

Examples:

```text
WITNESS CONFIDENCE: HIGH
```

or:

```text
VALIDATION.COLLISION ACTIVE
```

The player should understand the system’s objection.

---

# 41. Audio Direction

## Normal World

- Minimal ambience.
- Office HVAC.
- Computers.
- Fluorescent hum.
- Controlled city noise.

---

## Developer Console

Use:

- Short digital confirmation.
- Scan tones.
- Property toggle sounds.
- Clean system feedback.

Avoid cliché “Hollywood hacking” sounds.

---

## Suspicion Escalation

Music should become structurally wrong rather than simply louder.

Possible techniques:

- Missing beats.
- Repeated bars.
- Reverse fragments.
- Timing drift.
- Buffer-like stutters.

---

## Debugger

Sound design:

- Dry.
- Precise.
- Mechanical.
- Low emotional tone.
- Short validation pulses.

---

# 42. Music Philosophy

Music should support system instability.

It should not constantly tell the player:

> “You are in danger.”

Instead it should make them feel:

> “Something about reality is no longer synchronized.”

---

# 43. Tutorial Philosophy

Avoid large text tutorials.

Teach through constrained scenarios.

Example:

Door locked.

Player gets:

```text
Developer Privileges Detected.
Press F1.
```

Console highlights:

```text
Collision
```

Player learns through action.

Next scenario introduces witness.

The player naturally observes consequence.

---

# 44. Difficulty Philosophy

Difficulty comes from:

- Understanding rules.
- Planning around observers.
- Combining properties.
- Reading context.
- Managing suspicion.
- Working around validation.

Difficulty should NOT come primarily from:

- Fast reflexes.
- Combat aim.
- Precision platforming.
- Long stealth waiting.
- Obscure pixel hunting.

---

# 45. Checkpoints

Use frequent checkpoints around meaningful puzzle spaces.

Recommended:

- Start of major puzzle.
- After major objective.
- Before Debugger sequence.

Allow experimentation without excessive repetition.

---

# 46. Failure States

Possible failures:

## Local Failure

NPC notices anomaly.

Result:

Suspicion increases.

---

## System Failure

Reality determines a state is invalid.

Result:

Correction.

---

## Puzzle Failure

Environment becomes temporarily unusable.

Result:

Provide reset / restore mechanism.

---

## Hard Failure

Rare.

Reserved for:

- Falling out of valid level bounds.
- Critical sequence failure.
- Unrecoverable state.
- Scripted story fail condition.

Avoid punishing experimentation with frequent death.

---

# 47. Save Design

Save:

- Chapter.
- Checkpoint.
- Permissions.
- Major puzzle state.
- Story flags.
- Ending qualification.
- Important persistent consequences.

Do not require saving every physics object’s exact simulation state.

---

# 48. Accessibility / Comfort

Target options:

- FOV.
- Mouse sensitivity.
- Toggle / hold crouch.
- Subtitle size.
- UI scaling.
- Camera shake amount.
- Motion blur toggle.
- Chromatic aberration / glitch intensity option.
- Photosensitive-friendly reduced glitch mode.
- Color-independent important feedback.

---

# 49. Replayability

Replayability comes from:

- Alternate puzzle solutions.
- Low-suspicion challenge.
- Finding unintended systemic combinations.
- Alternate endings.
- Faster route discovery.

Do not build a separate roguelike or score system for v1.0.

---

# 50. Optional Challenge Metrics

Post-level statistics may include:

```text
Reality Suspicion Peak
Total Reality Modifications
Witnessed Modifications
Properties Restored
Reality Corrections Triggered
Unique Solution Tags
```

Do not rank creativity with a rigid numeric score.

---

# 51. Hidden Low-Suspicion Path

The ADMIN ending can use hidden criteria based on player behavior.

Potential signals:

- Low average suspicion.
- Few corrections.
- Frequent property restoration.
- Context-aware solutions.
- Reduced direct player-state manipulation.
- Diverse cheat usage.

Avoid requiring perfection.

The player should earn ADMIN through demonstrated understanding, not save-scumming.

---

# 52. Game Economy

There is no traditional currency economy.

The closest equivalents are:

## Permission

Which properties can be edited.

## Suspicion

How much systemic pressure the player has accumulated.

## Context

Temporary opportunities.

This keeps the game focused.

---

# 53. Progression

Progression comes from three sources.

## Mechanical Progression

New Developer permissions.

---

## Cognitive Progression

Player understands more combinations.

---

## System Opposition

Reality becomes better at validating the player.

The most important progression is cognitive.

---

# 54. Ability Unlock Order

Recommended:

```text
Chapter 1
Collision
Scale
Gravity

Chapter 2
Mass
Friction

Chapter 3
Time Scale

Chapter 4
No major new ability
Focus on validation

Chapter 5
Temporary Full Access
```

Avoid introducing new abilities in every late-game encounter.

---

# 55. Content Budget

Target 1.0:

```text
5 Chapters
6 Cheat Abilities
2 Main Systemic Enemy Types
10–15 Major Puzzle Spaces
20–30 Smaller Interactions
3 Endings
2–3 Hours
```

This scope should remain intentionally compact.

---

# 56. Puzzle Content Template

Every major puzzle should be documented using:

```text
Puzzle Name:

Narrative Objective:

Physical Objective:

Primary Obstacles:

Relevant NPCs:

Editable Objects:

Available Contexts:

Expected Normal Solution:

Expected Cheat Solution A:

Expected Cheat Solution B:

Expected Combination Solution:

Possible Emergent Solutions:

Dominant Strategy Risk:

Failure / Recovery:

Narrative Beat:

Required Assets:
```

---

# 57. Example Puzzle — Manager Office

## Narrative Objective

Retrieve administrator access card.

## Physical Objective

Enter locked manager office.

## Obstacles

- Locked door.
- One office worker witness.
- Elevated side window.
- Vent.
- Fire alarm system.

## Editable Objects

- Door Collision.
- Player Scale.
- Vent Scale.
- Gravity.
- Nearby crate.

## Context

- Fire alarm.
- Blackout.

## Solutions

### Normal

Find temporary access key.

### Cheat A

Disable door Collision when no witness is present.

### Cheat B

Shrink player and enter through gap / vent.

### Combination

Low Gravity + crate → reach upper window.

### Contextual

Trigger alarm → NPC evacuates → disable collision → restore.

## Dominant Strategy Risk

Collision OFF is simplest.

Mitigation:

Witness route and camera angle.

---

# 58. Example Puzzle — Museum Exhibit

## Narrative Objective

Steal Reality Core Fragment.

## Obstacles

- Guard.
- Camera.
- Weight sensor.
- Locked display case.
- Laser grid.

## Editable Objects

- Display collision.
- Exhibit mass.
- Camera time scale.
- Vent scale.
- Nearby prop mass/friction.

## Context

- Blackout.
- Fire alarm.
- Sensor failure.
- Evacuation.

## Solution Examples

- Trigger blackout, bypass display, restore.
- Match weight using manipulated prop.
- Slow camera and guard timing.
- Reduce exhibit mass and use ventilation route.
- Cause controlled sensor malfunction.

This puzzle should be the strongest expression of the game's systemic promise.

---

# 59. Debugger Encounter Rules

Debugger should:

- Teleport sparingly.
- Be predictable enough to reason around.
- Display what it is validating.
- Avoid invisible arbitrary detection.
- Create urgency without forcing combat.

Example UI:

```text
ACTIVE VALIDATION

Player.Scale
Player.Collision
```

The player knows what not to touch.

---

# 60. Reality Correction Rules

Corrections should be readable.

Examples:

```text
Restoring Door_03 Collision...
```

```text
Permission Scale temporarily revoked.
```

```text
Local geometry validation initiated.
```

This converts punishment into information.

---

# 61. Humor

Humor should emerge from dry system logic.

Examples:

NPC sees impossible event:

> “That is probably against policy.”

Corporate sign:

> “Unexpected reality should be reported within 24 hours.”

Debugger after restoring an obviously broken room:

> “Issue resolved.”

Avoid meme-heavy writing that will date quickly.

---

# 62. Story Themes

Primary themes:

- Freedom vs structure.
- Rules vs interpretation.
- Agency.
- System legitimacy.
- Responsibility.
- Creativity inside constraints.

The game should not preach one ending as objectively correct.

---

# 63. Ending Theme Matrix

```text
FREE
Freedom without stability.

STABLE
Stability without freedom.

ADMIN
Responsibility through understanding.
```

ADMIN should feel like synthesis, not simply “best because secret.”

---

# 64. Visual Iconography

Repeated visual motifs:

- Checkmarks.
- Validation boxes.
- Error triangles.
- Property panels.
- Grids.
- Bounding boxes.
- Identity numbers.
- Repeated corporate geometry.

These should unify the game’s identity.

---

# 65. Reality.exe Brand Identity

The title should often appear as:

```text
REALITY.EXE
```

Possible system language:

```text
VALIDATING...
PLAUSIBILITY ACCEPTED
STATE MISMATCH
REALITY CORRECTION
PERMISSION DENIED
ENTITY UNRESOLVED
```

Keep terminology consistent across UI, narrative, and world signage.

---

# 66. Marketing Promise

The game should be easy to explain in one short clip.

Example clip:

```text
Locked door
↓
Developer Console
↓
Collision ON → OFF
↓
Player walks through
↓
NPC stares
↓
REALITY SUSPICION +37
```

The concept must be understandable without narration.

---

# 67. Trailer Moments

Prioritize capturing:

- First wall walk.
- Player shrinking.
- Gravity manipulation.
- Car / crate behaving impossibly.
- Fire alarm used to hide cheating.
- Reality glitch escalation.
- Inspector observation.
- Debugger validation.
- Late-game rule exploit.

---

# 68. Player Testing Questions

After each test ask:

1. What did you think the game wanted you to do?
2. When did you first understand Reality Suspicion?
3. Which cheat did you use most?
4. Did you discover a solution you were not sure would work?
5. Did any solution feel arbitrarily blocked?
6. Did you understand why Reality reacted?
7. Did you feel clever?
8. Which puzzle would you tell a friend about?

---

# 69. Core Analytics Concepts

If analytics are implemented, useful events include:

```text
CheatUsed
CheatWitnessed
SuspicionChanged
RealityCorrection
PuzzleCompleted
SolutionTagUsed
PropertyRestored
DebuggerDetected
CheckpointRestarted
```

Avoid invasive telemetry.

---

# 70. Primary Design Risks

## Risk 1 — Collision Dominates Everything

Mitigation:

- Witnesses.
- Validation.
- Alternative geometry.
- Context systems.
- Direct consequences.

---

## Risk 2 — Reality Suspicion Feels Arbitrary

Mitigation:

- Clear reason feedback.
- Predictable context relationships.
- Debug-friendly tuning.
- Visible witness behavior.

---

## Risk 3 — Too Many Properties

Mitigation:

Keep v1.0 to six major cheat families.

---

## Risk 4 — Systemic Bugs Become Unmanageable

Mitigation:

- Restore/reset support.
- Controlled editable objects.
- Preset values.
- Strong debug tooling.

---

## Risk 5 — Game Becomes Traditional Stealth

Mitigation:

Stealth should support rule manipulation, not replace it.

---

## Risk 6 — Game Becomes a Simple Power Fantasy

Mitigation:

Reality must adapt and introduce validation pressure.

---

## Risk 7 — Puzzle Solutions Feel Scripted

Mitigation:

Design spaces and rules, not answer sequences.

---

# 71. Design Review Checklist

Before approving a puzzle, ask:

- Is the objective clear?
- Are at least three approaches possible?
- Is there a normal-world solution?
- Are editable objects readable?
- Can the player predict witness risk?
- Can context meaningfully change plausibility?
- Is Collision OFF the obvious universal answer?
- Can the player recover from experimentation?
- Does the puzzle teach or test a meaningful rule?
- Does the puzzle create a memorable “this actually works” possibility?

If most answers are no, redesign the puzzle.

---

# 72. Feature Review Checklist

Before adding a feature:

1. Does it strengthen rule manipulation?
2. Does it interact with at least two existing systems?
3. Does it create new player decisions?
4. Can it be used in more than one level?
5. Does it increase systemic depth more than production cost?
6. Does it risk creating a universal solution?
7. Does it fit the 2–3 hour scope?

If it only adds spectacle or complexity, do not add it.

---

# 73. Definition of a Successful Reality.exe Moment

A successful moment follows this pattern:

```text
Player notices a rule.
↓
Player forms an unusual idea.
↓
Player tests it.
↓
The systems respond consistently.
↓
The plan works, partially works, or creates an understandable consequence.
↓
Player learns something new.
```

The ideal reaction:

> “Wait... that counts?”

or:

> “I can't believe the game let me do that.”

---

# 74. Vertical Slice Quality Bar

The Museum or late Office section should demonstrate final-quality:

- Developer Console interaction.
- Reality Suspicion feedback.
- Context use.
- Multi-solution puzzle.
- NPC reaction.
- Reality visual instability.
- Audio identity.
- At least one systemic surprise.

If the vertical slice cannot sell the concept, adding more chapters will not solve the problem.

---

# 75. Final Scope Lock

For Reality.exe 1.0, the design is considered complete when the following exists:

```text
Developer Console
6 cheat abilities
Reality Suspicion
Context / plausibility
Witness system
Reality Inspector
Debugger
5 chapters
10–15 major puzzle spaces
3 endings
2–3 hour campaign
```

Anything beyond this requires explicit scope approval.

---

# 76. Final Design Principle

Whenever a design decision is unclear, prioritize this question:

> **Does this let the player reason creatively about the rules of the world?**

If yes, it likely belongs in Reality.exe.

If it merely gives the player more content, more power, more numbers, or more spectacle without creating new systemic reasoning:

**Do not add it.**

---

# 77. One-Sentence Definition

**Reality.exe is a first-person systemic puzzle game where the player can modify the simulation’s rules, but must make every impossible action look plausible enough that reality itself does not reject them.**
