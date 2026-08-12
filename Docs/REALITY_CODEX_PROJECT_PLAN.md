# Reality.exe — Codex Project Plan

> Project type: First-person systemic puzzle / light stealth / immersive sim  
> Engine: Unreal Engine 5  
> Development model: Solo developer + Codex-assisted engineering  
> Target scope: 2–3 hour complete indie game  
> Target production cycle: 20 weeks  
> Core principle: **You may cheat. Just don't get caught by reality.**

---

# 1. Project Vision

## 1.1 High Concept

**Reality.exe** is a first-person systemic puzzle game in which the player can modify selected game-world parameters through a hidden Developer Console.

The player is allowed to "cheat" by changing values such as:

- Collision
- Scale
- Gravity
- Mass
- Friction
- Time Scale

However, the world contains a **Reality Validation System**.

The important question is not:

> Can the player break the rules?

It is:

> Can the player break the rules in a way the world can still explain?

Example:

- Walking through a locked door in front of an NPC → high Reality Suspicion.
- Turning off the lights, distracting witnesses, then disabling door collision → low Reality Suspicion.
- Launching a car into the air for no reason → highly suspicious.
- Launching a car during an explosion → potentially plausible.

The core player fantasy is:

> **Exploit the rules of the game while hiding the fact that you exploited them.**

---

# 2. Product Scope

## Target Version

`Reality.exe 1.0`

Target playtime:

- 2–3 hours
- 5 chapters
- 6 core cheat abilities
- 1 main ending
- 2 alternate endings
- 1 main systemic enemy: Debugger
- 1 intermediate systemic enemy: Reality Inspector

## Explicit Non-Goals

Do NOT add the following to v1.0 unless specifically approved:

- Open world
- Guns
- Complex melee combat
- Inventory-heavy gameplay
- RPG leveling
- Loot rarity
- Skill trees
- Procedural city generation
- Multiplayer
- Complex dialogue trees
- Crafting
- Vehicles as a major gameplay system
- Weather simulation
- Player-editable materials
- NPC memory simulation
- AI-generated dialogue
- Full in-game programming language
- Arbitrary numerical editing for every property

If a new idea does not directly strengthen the core loop, place it in:

`Docs/Reality_2_Ideas.md`

Do not implement it in v1.0.

---

# 3. Core Gameplay Loop

```text
Observe
  ↓
Understand environment rules
  ↓
Create a plausible context
  ↓
Modify reality
  ↓
Avoid / deceive witnesses
  ↓
Complete objective
  ↓
Reality system reacts
  ↓
Adapt strategy
```

The player should gradually evolve from:

```text
"I can walk through walls."
```

to:

```text
"I need to make sure nobody can prove I walked through the wall."
```

to:

```text
"I do not need to walk through the wall. I can manipulate the surrounding rules instead."
```

---

# 4. Core Design Pillars

## 4.1 Systemic Solutions

Major obstacles should support multiple solutions.

For important puzzle spaces, aim for:

- 1 normal solution
- 2 obvious cheat solutions
- 2 systemic combination solutions
- At least 1 emergent solution not explicitly tutorialized

Do not design puzzles around a single required cheat.

Bad:

```text
This door requires Scale = 0.5.
```

Good:

```text
The player must find any way into the room.
```

Possible solutions:

- Find key
- Disable collision
- Shrink player
- Increase vent size
- Reduce gravity and enter through upper window
- Move a heavy object into position
- Trigger an evacuation
- Exploit an NPC opening the door

---

## 4.2 Plausibility Over Punishment

Reality Suspicion should not behave like a traditional crime meter.

The system asks:

> Is this event physically / causally explainable?

Example:

```text
Murder:
Illegal but physically possible.
Reality Suspicion may remain low.

Person becomes 5 cm tall:
Not physically plausible.
Reality Suspicion becomes high.
```

---

## 4.3 Failure Creates New Gameplay

Do not immediately Game Over when Reality Suspicion reaches maximum.

Instead trigger:

`Reality Correction`

Possible consequences:

- Lock one cheat permission
- Restore modified objects
- Activate Reality Inspector
- Spawn Debugger
- Increase validation intensity
- Change available paths
- Reset part of the environment

Failure should create pressure and adaptation, not only reloads.

---

# 5. Core Cheat Abilities

Target v1.0 ability set:

## 5.1 Collision

States:

```text
ON
OFF
```

Uses:

- Walk through doors
- Walk through walls
- Drop props through surfaces
- Bypass physical blockers

Risk:

High.

---

## 5.2 Scale

Recommended presets:

```text
0.25x
0.5x
1.0x
2.0x
4.0x
```

Uses:

- Shrink player
- Enlarge vents
- Create platforms
- Remove obstacles indirectly

Avoid unrestricted arbitrary values in v1.0.

---

## 5.3 Gravity

Recommended states:

```text
Normal
Low
Zero
Reverse
```

Reverse may remain locked until late game.

Uses:

- High jumps
- Moving objects
- Floating props
- Environmental reconfiguration

---

## 5.4 Mass

Uses:

- Move otherwise immovable objects
- Change physical interactions
- Create environmental chain reactions

---

## 5.5 Friction

Uses:

- Slide props
- Create moving-floor puzzles
- Manipulate vehicles / carts / crates

---

## 5.6 Time Scale

Prefer localized time manipulation over global slow motion.

Uses:

- Slow NPCs
- Create timing windows
- Manipulate moving systems

---

# 6. Core Technical Architecture

Prefer:

- C++ for reusable systems and rules
- Blueprint for level scripting, presentation, one-off sequences
- Gameplay Tags for semantic state
- Data Assets / Data Tables for tunable rules
- Components over inheritance-heavy actor trees

Avoid:

- Hard references between unrelated Actors
- Level Blueprint as primary gameplay architecture
- Separate cheat logic duplicated across every prop
- Massive Blueprint graphs containing core systemic rules

---

# 7. Proposed UE5 Structure

```text
Source/
  Reality/
    Core/
    Interaction/
    Developer/
    RealitySystem/
    AI/
    Puzzle/
    UI/

Content/
  Core/
  Characters/
  Developer/
  Reality/
  AI/
  Levels/
  Props/
  UI/
  Audio/
  VFX/
  Data/
```

---

# 8. Primary Runtime Systems

## 8.1 Interaction System

Suggested core types:

```text
IInteractable
UInteractionComponent
UInteractableComponent
```

Responsibilities:

- Trace / focus
- Highlight
- Interaction prompt
- Execute interaction
- Send selected object to Developer Console

---

## 8.2 Editable Object System

Suggested component:

```cpp
URealityEditableComponent
```

Responsibilities:

- Declare which properties are editable
- Store original state
- Apply modification
- Restore modification
- Emit cheat events
- Provide UI-readable property data
- Provide Gameplay Tags describing object type

Example capabilities:

```text
Modify.Collision
Modify.Scale
Modify.Gravity
Modify.Mass
Modify.Friction
Modify.Time
```

Do NOT create separate architecture such as:

```text
BP_CheatDoor
BP_CheatChair
BP_CheatCrate
BP_CheatVehicle
```

unless the object has genuinely unique behavior.

---

## 8.3 Developer Console

Input:

```text
F1
```

Flow:

```text
Enter Developer Mode
  ↓
Identify focused editable object
  ↓
Display editable properties
  ↓
Player chooses modification
  ↓
Apply modification
  ↓
Emit Reality Event
```

UI should eventually feel like an internal developer/debugging tool rather than a traditional hacking minigame.

---

## 8.4 Reality Manager

Central authority for:

- Suspicion
- Plausibility
- Context
- Witness reports
- Reality state
- Correction events
- Debugger escalation

Suggested conceptual flow:

```text
Cheat Event
  ↓
Base Suspicion
  ↓
Magnitude
  ↓
Witnesses
  ↓
Environmental Context
  ↓
Evidence
  ↓
Final Suspicion
```

---

# 9. Gameplay Tags

Initial suggested taxonomy:

```text
Cheat.Collision
Cheat.Scale
Cheat.Gravity
Cheat.Mass
Cheat.Friction
Cheat.Time

Context.Fire
Context.Blackout
Context.Explosion
Context.Flood
Context.Panic
Context.Construction
Context.SensorFailure

Witness.Human
Witness.Camera
Witness.Inspector
Witness.Debugger

Object.Door
Object.Prop
Object.Vehicle
Object.Character
Object.Security
Object.Environment

Reality.Stable
Reality.Questioning
Reality.Investigating
Reality.Unstable
Reality.Correction
```

Prefer tags and data-driven rules over giant hard-coded conditional trees.

---

# 10. Reality Suspicion

Range:

```text
0–100
```

Suggested states:

## 0–20: Stable

Normal world.

## 20–40: Questioning

Minor NPC reactions and subtle visual anomalies.

## 40–60: Investigating

Security systems and observers become more attentive.

## 60–80: Unstable

Visible reality glitches.

## 80–100: Correction

Reality actively attempts to fix anomalies.

At 100, do not automatically Game Over.

Trigger a Reality Correction response.

---

# 11. Context / Plausibility System

Environmental events produce Context Tags.

Examples:

```text
Context.Fire
Context.Explosion
Context.Blackout
Context.SensorFailure
Context.Panic
```

Cheat events may have context modifiers.

Example:

```text
Flying vehicle
Base Suspicion: +35

Explosion nearby:
-25

No witnesses:
-10

Final:
0–15 depending on magnitude
```

Important:

A context should not universally excuse all cheats.

Example:

```text
Explosion + flying car
Potentially plausible

Explosion + human shrinks to 5 cm
Still highly implausible
```

---

# 12. NPC Observation

Normal NPCs only need a small number of high-value behaviors.

Recommended states:

```text
Normal
Concerned
Suspicious
Panicked
```

They should detect:

- Player performing impossible actions
- Object state changes
- Environmental anomalies
- Relevant sounds

Output:

```text
Witness Report
```

Example:

```text
Saw.Player.EnterWall
Saw.Object.ScaleChange
Saw.Object.Float
Heard.Explosion
```

Reality Manager interprets these reports.

Do not attempt realistic human reasoning AI in v1.0.

---

# 13. Reality Inspector

Appears mid-game.

Purpose:

- Make repeated strategies less dominant
- Force players to vary methods
- Signal that the system is adapting

Example:

If player frequently uses Collision:

```text
Validation.Collision = Active
```

Future Collision cheats become more suspicious.

The Inspector should not simply be a stronger guard.

It represents systematic observation.

---

# 14. Debugger

Primary systemic antagonist.

The Debugger:

- Detects invalid states
- Restores modified values
- Locks Developer permissions
- Tracks anomalies
- Enforces active rules

It is not primarily a combat enemy.

Core player response:

> Do not modify the monitored value. Modify the surrounding system instead.

Example:

Blocked:

```text
Player Speed = 3000
```

Alternative:

```text
Player Speed = 600
Floor Velocity = -2400
```

Debugger sees:

```text
Player Speed = Valid
```

while the player's effective movement is much faster.

This style of rules-lawyering is a major late-game mechanic.

---

# 15. Chapter Structure

Target: 5 chapters.

---

## Chapter 1 — Office

Target length:

25–30 min

Introduce:

```text
Collision
Scale
Gravity
```

Objectives:

- Reach server room
- Discover Developer Console
- Learn that witnesses matter
- Learn that environmental context can justify cheats
- First Reality anomaly
- Debugger teaser

---

## Chapter 2 — Underground Facility

Target length:

25–35 min

Introduce:

```text
Mass
Friction
```

Focus:

- Combined physical cheats
- Environmental machinery
- Security routes
- More systemic puzzle spaces

Narrative reveal:

Player identity records are inconsistent.

---

## Chapter 3 — Museum

Target length:

30–40 min

Introduce:

```text
Time Scale
Reality Inspector
```

Core set piece:

Steal a Reality Core Fragment from a secured exhibit.

Possible systems:

- Guards
- Cameras
- Laser security
- Weight sensor
- Ventilation
- Power control
- Evacuation
- Display case

This chapter should be the strongest marketing / trailer level.

---

## Chapter 4 — Debugger

Target length:

25–35 min

Focus:

- Active validation rules
- Permission locking
- Indirect manipulation
- Exploiting system definitions

Major conceptual shift:

```text
Early game:
Modify the target.

Late game:
Modify the target's environment.
```

---

## Chapter 5 — Reality Core

Target length:

20–30 min

Visual direction:

- Debug space
- White void
- Grid
- Untextured geometry
- Editor-like visualization
- System logs

Final rule puzzles:

```text
PLAYER MAY NOT CROSS BOUNDARY
```

Solution concept:

Move the world across the player.

```text
DOOR MUST REMAIN CLOSED
```

Solution concept:

Do not open the door. Modify surrounding geometry.

Endings:

```text
FREE
STABLE
ADMIN
```

---

# 16. Production Schedule — 20 Weeks

## Week 1 — Project Setup

Deliverables:

- UE project
- Git repository
- Folder structure
- Coding rules
- AGENTS.md
- Basic first-person controller
- Build verification

Exit criteria:

Project builds cleanly and Codex has clear repository instructions.

---

## Weeks 2–3 — Core Systems

Implement:

- Interaction System
- Editable Object Component
- Developer Console prototype
- Collision cheat
- Scale cheat
- Gravity cheat
- Reality Event model
- Reality Manager prototype
- Gameplay Tags
- Debug logging

Exit criteria:

A graybox room supports interacting with editable objects and modifying at least three properties.

---

## Week 4 — Prototype Gate

Build:

- 1 room
- 1 corridor
- 2 NPCs
- 5 small puzzles

Required mechanics:

```text
Collision
Scale
Gravity
Witness detection
Suspicion
Basic context
```

Critical question:

> Does systemic cheating generate spontaneous player ideas?

Do not continue full production until the answer is yes.

---

## Weeks 5–6 — Public Demo

Build a polished 20–30 minute office demo.

Required flow:

```text
Office
↓
Server Room blocked
↓
Find administrator card
↓
Discover Developer Console
↓
First cheat
↓
Witnessed cheat
↓
Learn suspicion
↓
Create plausible distraction
↓
Manager office
↓
Debugger appears
↓
Server room
↓
Reality.exe reveal
```

Exit criteria:

A standalone packaged build can be given to external testers with no developer explanation.

---

## Week 7 — Production Refactor

Refactor prototype systems into scalable architecture.

Tasks:

- Data-driven cheat definitions
- Data-driven suspicion modifiers
- Context rule system
- Component cleanup
- Reduce hard references
- Add automated / functional tests where practical
- Improve debug output
- Document architecture

Do not build new content unless required for refactor verification.

---

## Weeks 8–9 — Chapter 1

Produce final-quality Office chapter.

Target:

25–30 min

Exit criteria:

Chapter is fully playable with final gameplay systems and near-final pacing.

---

## Weeks 10–11 — Chapter 2

Produce Underground Facility.

Add:

```text
Mass
Friction
```

Exit criteria:

Players must use at least one combined systemic solution.

---

## Weeks 12–13 — Chapter 3

Produce Museum.

Add:

```text
Time Scale
Reality Inspector
```

Target multiple meaningful solutions.

Exit criteria:

At least 5 clearly distinct viable approaches exist for the central heist objective.

---

## Weeks 14–15 — Chapter 4

Produce Debugger chapter.

Add:

- Active validation rules
- Permission locking
- Indirect manipulation puzzles
- Debugger encounters

Exit criteria:

The player is forced to reason about rule definitions rather than only raw parameter modification.

---

## Week 16 — Chapter 5

Produce Reality Core and ending flow.

Add:

```text
FREE
STABLE
ADMIN
```

Exit criteria:

All endings function and the entire game has a valid narrative ending.

---

## Week 17 — Alpha / Feature Freeze

Absolute rule:

**No new gameplay systems.**

Required flow:

```text
New Game
↓
Chapter 1
↓
Chapter 2
↓
Chapter 3
↓
Chapter 4
↓
Chapter 5
↓
Ending
```

Exit criteria:

The full game can be completed from start to finish.

---

## Week 18 — Playtest

Target:

10+ external players.

Measure:

### Collision Dependency

If most players solve most obstacles with:

```text
Collision OFF
```

the systemic design is failing.

### Creative Solution Count

Record moments when testers find a solution not explicitly expected.

Desired result:

At least one "I didn't know that would work" moment per player.

### Frustration Source

Determine whether players feel:

```text
"The world rules prevented me."
```

or:

```text
"The developer arbitrarily prevented me."
```

The first is acceptable.

The second requires redesign.

---

## Week 19 — Polish

Focus:

- Developer Console UX
- VFX
- Audio
- Animation
- Lighting
- Camera
- Suspicion feedback
- Reality glitch effects
- Debugger presentation

Property modification should feel satisfying.

Example:

```text
Collision ON → OFF
```

should involve:

```text
Input
↓
UI response
↓
Object scan
↓
Collision visualization changes
↓
Audio confirmation
↓
Reality reaction
```

not only a text toggle.

---

## Week 20 — Release Candidate

Only:

- Bug fixing
- Crash fixing
- Performance
- Save / Load
- Settings
- Input
- Gamepad
- Resolution
- Packaging
- Localization checks
- Achievements if already scoped
- Release build verification

No new mechanics.

Target:

```text
Reality.exe
v1.0 RC
```

---

# 17. Development Gates

## Gate A — Week 4

Question:

> Is modifying reality actually fun?

If no:

Do not scale production.

---

## Gate B — Week 6

Question:

> Can a new player understand and enjoy the concept without developer explanation?

If no:

Fix onboarding and core loop before adding chapters.

---

## Gate C — Week 13

Question:

> Does the systemic gameplay still produce multiple interesting solutions?

If no:

Reduce content scope and improve systemic depth.

---

## Gate D — Week 17

Question:

> Can the game be completed from start to finish?

If no:

Cut content instead of extending scope.

---

# 18. Codex Working Rules

Codex should be treated as an engineering collaborator, not an autonomous game designer.

## Codex SHOULD

- Implement clearly scoped systems
- Refactor existing code
- Write reusable UE C++ components
- Add tests
- Diagnose compile errors
- Fix bugs
- Review architecture
- Improve logs
- Reduce duplication
- Generate boilerplate
- Add editor-exposed configuration
- Document code
- Convert repeated logic into data-driven systems

---

## Codex SHOULD NOT Decide Without Approval

- Core game mechanics
- New cheat abilities
- Major narrative changes
- Puzzle solutions
- Chapter structure
- Visual identity
- Difficulty philosophy
- Whether a feature belongs in v1.0
- Scope expansion

If implementation reveals a design problem, Codex should report:

```text
Problem
Impact
Possible options
Recommended technical option
```

and wait for a design decision.

---

# 19. Codex Task Format

Prefer small tasks.

Good:

```text
TASK:
Implement URealityEditableComponent.

REQUIREMENTS:
- Actor Component
- Stores editable property flags
- Stores original values
- Supports collision, scale, gravity
- Emits FRealityCheatEvent
- Blueprint-callable Apply/Restore methods
- No hard dependency on UI
- Must compile in UE5 project

DONE WHEN:
- Component compiles
- Can be attached to a static mesh actor
- Collision can be toggled at runtime
- Original state can be restored
- Cheat event is emitted
```

Bad:

```text
Build the cheating system.
```

---

# 20. Recommended Issue Breakdown

Use issues approximately at this size:

```text
Interaction trace
Interactable interface
Interaction prompt
Editable component
Collision modifier
Scale modifier
Gravity modifier
Cheat event struct
Reality Manager
Suspicion calculation
Context tags
Witness component
NPC observer
Developer Console object panel
Developer Console property widget
Reality debug overlay
Reality Correction event
Inspector monitoring
Debugger validation
Permission locking
Save state
```

Each issue should ideally be independently testable.

---

# 21. Definition of Done

A gameplay system is not complete merely because it compiles.

For each system require:

- Compiles
- Runtime behavior verified
- No obvious hard-coded level dependency
- Blueprint exposure where appropriate
- Debug information available
- Restore/reset behavior supported
- Invalid inputs handled
- No obvious performance issue
- Documented public API
- Tested in at least one graybox scenario

---

# 22. Debug Requirements

Every core systemic action should have optional debug visualization.

Examples:

```text
Show Reality Suspicion
Show current Context Tags
Show active Witness Reports
Show cheat event magnitude
Show active Validation Rules
Show editable object capabilities
Show original/current property values
```

Debug features must be easy to disable for shipping.

---

# 23. Save / Load Requirements

At minimum save:

- Current chapter
- Checkpoint
- Current Developer Permissions
- Puzzle-critical persistent state
- Narrative flags
- Ending qualification state

Avoid trying to serialize every physics object's live transform unless necessary.

Prefer restoring level state from authored state plus explicit persistent flags.

---

# 24. Performance Philosophy

Systemic interactions must not depend on scanning the entire level every frame.

Prefer:

- Event-driven updates
- Local overlap / perception queries
- Cached subsystem references
- Gameplay Tags
- Timed validation intervals where appropriate

Avoid:

```text
GetAllActorsOfClass every Tick
```

for core gameplay systems.

---

# 25. Blueprint / C++ Boundary

Prefer C++ for:

- Core interfaces
- Components
- Data structures
- Reality Manager
- Rule evaluation
- Cheat event handling
- Save model
- Reusable AI observation logic

Prefer Blueprint for:

- Level-specific events
- Puzzle sequencing
- Cinematics
- VFX timing
- Audio triggers
- One-off scripted reactions
- Environment setup

---

# 26. Prototype Priorities

If time becomes constrained, preserve in this order:

1. Developer Console
2. Collision / Scale / Gravity
3. Reality Suspicion
4. Witness system
5. Context / plausibility
6. Multi-solution puzzle design
7. Debugger
8. Story
9. Visual polish
10. Extra abilities

The game can survive reduced narrative scope.

It cannot survive a weak core interaction loop.

---

# 27. Scope Reduction Plan

If the project slips, reduce in this order:

### First cut

- Extra NPC variations
- Extra cinematics
- Optional side spaces
- Cosmetic interactions

### Second cut

Reduce Chapter 2 and Chapter 4 length.

### Third cut

Reduce endings from 3 to 2.

### Last resort

Release a polished 60–90 minute version.

Do NOT cut:

- Developer Console
- Reality Suspicion
- Context system
- At least 4 cheat abilities
- Debugger concept
- Multiple-solution level design

---

# 28. Milestones

```text
P0 — Week 4
Reality Prototype
10-minute graybox
Core fun validated
```

```text
P1 — Week 6
Reality Demo
20–30 minute standalone build
External test ready
```

```text
P2 — Week 13
Reality Vertical Production Check
3 chapters
Core systemic depth proven
```

```text
P3 — Week 17
Reality Alpha
Full game playable
Feature freeze
```

```text
P4 — Week 20
Reality.exe 1.0 Release Candidate
```

---

# 29. Success Criteria

The project is succeeding if players regularly say things equivalent to:

> "Wait, can I do this?"

followed by:

> "It actually worked."

The strongest success signal is not simply completion rate.

It is:

**Players discovering solutions the designer did not explicitly teach them.**

---

# 30. Final Development Rule

Whenever deciding whether to implement a feature, ask:

> Does this create more interesting interactions between rules?

If yes, consider it.

If it only creates more content, complexity, or spectacle without improving systemic interaction:

**Do not add it to v1.0.**
