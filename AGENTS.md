# AGENTS.md — Reality.exe

This file defines mandatory working rules for Codex and any other coding agent operating inside the **Reality.exe** repository.

Read this file before making changes.

Related project documents:

- `Docs/REALITY_GAME_DESIGN_DOCUMENT_GDD.md`
- `Docs/REALITY_CODEX_PROJECT_PLAN.md`

If a task conflicts with this file, stop and report the conflict before implementing.

---

# 1. Project Summary

**Reality.exe** is a first-person systemic puzzle game built with Unreal Engine 5.

Core gameplay:

- The player can modify selected simulation properties.
- The world evaluates whether those modifications appear plausible.
- Suspicious or impossible actions raise Reality Suspicion.
- The world responds through observation, validation, correction, Inspectors, and Debuggers.

Primary cheat properties:

- Collision
- Scale
- Gravity
- Mass
- Friction
- Time Scale

Target scope:

- 2–3 hour game
- 5 chapters
- 6 core cheat abilities
- Reality Suspicion
- Context / plausibility system
- Witness system
- Reality Inspector
- Debugger
- 3 endings

Do not expand scope without explicit approval.

---

# 2. Core Engineering Principles

Always prefer:

1. Reusable systems over level-specific hacks.
2. Components over deep inheritance.
3. Data-driven configuration over hard-coded branches.
4. Gameplay Tags over string comparisons.
5. Event-driven logic over per-frame scanning.
6. C++ for reusable systems.
7. Blueprint for level scripting and presentation.
8. Clear debug visibility for systemic behavior.
9. Restorable state for editable gameplay properties.
10. Small, reviewable changes over large rewrites.

---

# 3. Unreal Engine Architecture Rules

## Use C++ for

- Interfaces
- Actor Components
- Subsystems
- Core gameplay data structures
- Reality event processing
- Suspicion calculation
- Context evaluation
- Witness report models
- Save data
- Permission logic
- Validation rules
- Reusable AI observation logic

## Use Blueprint for

- Level-specific sequencing
- Puzzle scripting
- Cinematics
- VFX timing
- Audio triggers
- One-off environment reactions
- Designer-authored setup
- Content assembly

Do not move core systemic logic into Level Blueprint.

---

# 4. Required Architectural Direction

Preferred modules / areas:

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
```

Preferred content layout:

```text
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

Do not create random top-level folders unless required.

---

# 5. Core Gameplay Types

The following concepts should remain modular.

## Interaction

Suggested concepts:

```text
IInteractable
UInteractionComponent
UInteractableComponent
```

Responsibilities:

- Focus
- Trace
- Highlight
- Prompt
- Interaction execution

Interaction code must not contain Reality Suspicion logic directly.

---

## Editable Objects

Preferred component:

```cpp
URealityEditableComponent
```

Responsibilities:

- Declare editable properties
- Store original values
- Apply modifications
- Restore modifications
- Expose state to UI
- Emit Reality cheat events
- Provide object tags

Do not create separate duplicated systems such as:

```text
BP_CheatDoor
BP_CheatChair
BP_CheatCrate
BP_CheatVehicle
```

unless the object has truly unique behavior.

---

## Reality Manager

The Reality system is the central authority for:

- Suspicion
- Context
- Plausibility
- Witness reports
- Validation
- Correction
- Escalation state

Do not calculate global Reality Suspicion independently inside arbitrary actors.

---

# 6. Gameplay Tags

Use Gameplay Tags for semantic classification.

Examples:

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

Reality.Stable
Reality.Questioning
Reality.Investigating
Reality.Unstable
Reality.Correction
```

Do not use raw strings like:

```cpp
if (Type == "Door")
```

when a Gameplay Tag or typed enum is appropriate.

---

# 7. Data-Driven Rules

Cheat behavior and suspicion tuning should be data-driven wherever practical.

Prefer:

- Data Assets
- Data Tables
- Curves
- Gameplay Tags
- Config structs

Avoid:

```cpp
if Door then +20
if Car then +30
if Chair then +10
```

inside large hard-coded condition trees.

A new object type should ideally be configurable without changing Reality Manager code.

---

# 8. State Restoration

Any gameplay property that can be modified should support restoration unless explicitly impossible by design.

Examples:

- Collision
- Scale
- Gravity state
- Mass
- Friction
- Time Scale

Store original state safely.

Avoid assuming the original value is a global default.

---

# 9. Runtime Performance Rules

Never use expensive global searches every frame.

Do not use:

```text
GetAllActorsOfClass on Tick
```

for gameplay systems.

Prefer:

- Cached references
- Subsystems
- Events
- Delegates
- Overlap queries
- Perception components
- Timed validation intervals
- Spatially local queries

Tick should be disabled when not required.

---

# 10. Reference Rules

Avoid unnecessary hard Actor references.

Prefer:

- Interfaces
- Components
- Event dispatch
- Weak references where appropriate
- Subsystem lookups
- Gameplay Tags
- Soft references for assets where appropriate

Do not make unrelated actors directly depend on each other.

---

# 11. Blueprint Rules

Blueprints must remain readable.

Avoid:

- Giant graphs
- Deep spaghetti execution chains
- Copy-pasted systemic logic
- Hidden magic values
- Level Blueprint dependencies for reusable systems

Prefer:

- Functions
- Components
- Macro libraries only when justified
- Named variables
- Comment blocks
- Data assets

If Blueprint logic becomes reusable or complex, consider moving it to C++.

---

# 12. C++ Rules

Code should be:

- UE-style
- Readable
- Small in responsibility
- Explicit about ownership
- Safe around null references
- Blueprint-exposed only where useful

Prefer Unreal naming conventions.

Examples:

```cpp
URealityEditableComponent
FRealityCheatEvent
ERealityState
URealityManagerSubsystem
```

Avoid vague names:

```cpp
Manager
Thing
Data
Handler2
TempSystem
```

---

# 13. Public API Documentation

Any reusable public class, component, struct, enum, or function should have concise documentation explaining:

- Purpose
- Ownership
- Expected caller
- Side effects
- Important assumptions

Do not over-comment obvious implementation details.

---

# 14. Debugging Requirements

Core gameplay systems must expose useful debug information.

Required debug visibility should include, where relevant:

- Current Reality Suspicion
- Active Context Tags
- Active Witness Reports
- Cheat event magnitude
- Active Validation Rules
- Editable capabilities
- Original values
- Current modified values
- Reality state
- Permission locks

Debug tools must be easy to disable in shipping builds.

---

# 15. Logging

Use meaningful log categories.

Avoid excessive spam.

Logs should answer:

- What happened?
- Which object?
- Which rule?
- What value changed?
- Why did Reality Suspicion change?
- What correction occurred?

Example conceptual log:

```text
Reality: Door_03 Collision OFF
BaseSuspicion=25
WitnessModifier=0
ContextModifier=-15
RestoreModifier=-5
FinalSuspicion=5
```

---

# 16. Testing Expectations

A change is not complete only because it compiles.

Where practical, verify:

- Apply behavior
- Restore behavior
- Invalid input
- Missing target
- Permission denied
- Repeated modification
- Save/load compatibility
- Suspicion event emission
- Blueprint usage

Use automated or functional tests where they provide value.

---

# 17. Definition of Done

A gameplay task is complete when:

- Code compiles
- Editor loads without new errors
- Runtime behavior is verified
- No obvious level-specific hard dependency exists
- Blueprint exposure is appropriate
- Debug information exists
- Restore/reset behavior works
- Invalid input is handled
- No obvious per-frame performance regression exists
- Public API is documented
- Relevant test scenario passes

---

# 18. Codex Task Execution Procedure

For every task:

## Step 1 — Inspect

Before editing:

- Read relevant files
- Identify existing patterns
- Check adjacent systems
- Avoid duplicating existing functionality

## Step 2 — State Approach

Internally determine:

- Files likely to change
- Dependencies
- Risks
- Testing path

## Step 3 — Implement Narrowly

Only change what is necessary.

Do not opportunistically rewrite unrelated code.

## Step 4 — Verify

At minimum:

- Compile / build if available
- Check obvious references
- Run relevant tests if available
- Inspect changed files

## Step 5 — Report

Summarize:

```text
Changed:
Why:
Validation:
Known limitations:
Follow-up:
```

Do not claim successful runtime behavior unless actually verified.

---

# 19. Change Size

Prefer one coherent task per change.

Good:

```text
Implement Reality cheat event struct.
```

Good:

```text
Add collision editing support to URealityEditableComponent.
```

Bad:

```text
Implement the full Developer Console, Reality Manager, AI system, and Chapter 1.
```

Large tasks should be decomposed before implementation.

---

# 20. When to Stop and Report

Stop and report before proceeding if:

- The requested change conflicts with GDD.
- The requested change significantly expands scope.
- Existing architecture makes the request unsafe.
- Required engine/plugin dependency is missing.
- The change would require destructive migration.
- The task is ambiguous enough to risk implementing the wrong gameplay behavior.
- The task could corrupt save compatibility.

Report:

```text
Problem
Impact
Options
Recommended approach
```

---

# 21. Do Not Invent Design

Codex must not independently decide:

- New cheat abilities
- New chapters
- New endings
- New major NPC types
- New game modes
- Core narrative changes
- Major UX changes
- Difficulty philosophy
- Scope expansion

Codex may propose options, but implementation requires approval.

---

# 22. Explicit v1.0 Non-Goals

Do not implement unless explicitly requested:

- Open world
- Multiplayer
- Guns
- Melee combat system
- Inventory-heavy systems
- RPG levels
- Skill trees
- Loot rarity
- Crafting
- Procedural city generation
- AI-generated NPC dialogue
- Full NPC memory simulation
- Full arbitrary property editor
- Weather simulation
- Material editing as gameplay
- Vehicle-driving gameplay
- User scripting language

---

# 23. Reality Suspicion Rules

Reality Suspicion is not crime.

Never implement it as:

```text
Illegal action = suspicion
```

It measures implausibility.

Examples:

```text
Stealing:
May be illegal but physically plausible.

Walking through wall:
Highly implausible.

Car flying during explosion:
Potentially plausible.

Human shrinking instantly:
Highly implausible.
```

Preserve this distinction.

---

# 24. Context Rules

Context must not universally excuse cheating.

Bad:

```text
Context.Fire always -30 Suspicion.
```

Better:

```text
Fire reduces witness quality.
Explosion may explain high-velocity objects.
Blackout reduces camera confidence.
Sensor failure reduces security evidence.
```

Context effects should be specific and interpretable.

---

# 25. Failure Philosophy

Avoid automatic Game Over for high Suspicion.

Prefer Reality Correction:

- Restore invalid state
- Revoke permission
- Activate Inspector
- Spawn Debugger
- Change validation rules
- Reset local environment

Failure should create gameplay.

---

# 26. Dominant Strategy Warning

Collision OFF is expected to become the easiest exploit.

Do not solve this by disabling Collision everywhere.

Use:

- Witnesses
- Cameras
- Validation
- Environmental constraints
- Suspicion
- Geometry
- Alternate objectives

The goal is to make Collision one tool among several.

---

# 27. Puzzle Rule

Do not encode puzzles around one required answer unless explicitly designed as tutorial onboarding.

Prefer:

```text
Objective:
Enter room.
```

Not:

```text
Required solution:
Use Gravity.
```

Systems should enable multiple valid approaches.

---

# 28. Save System Rules

Save only state that needs to persist.

Prefer storing:

- Chapter
- Checkpoint
- Developer permissions
- Major puzzle flags
- Narrative flags
- Ending qualification
- Critical persistent object state

Avoid serializing every physics object's live state unless required.

---

# 29. Error Handling

Gameplay systems should fail safely.

Examples:

- Invalid editable target → no modification
- Missing Reality Manager → log error, avoid crash
- Unsupported property → reject clearly
- Missing data asset → use safe fallback if possible
- Invalid saved state → recover to checkpoint

Never silently corrupt state.

---

# 30. Feature Priority

If tradeoffs are required, preserve features in this order:

1. Developer Console
2. Collision / Scale / Gravity
3. Reality Suspicion
4. Witness system
5. Context / Plausibility
6. Multi-solution puzzles
7. Mass / Friction / Time
8. Debugger
9. Story
10. Extra polish

The game can survive less content.

It cannot survive a weak systemic core.

---

# 31. Scope Reduction Rule

If schedule pressure appears:

Cut:

- Optional rooms
- Extra dialogue
- Extra cinematics
- Cosmetic interactions
- NPC variants

Then shorten chapters.

Do not respond to schedule pressure by adding technical shortcuts that destroy the systemic architecture.

---

# 32. Code Review Checklist

Before considering a change ready, check:

- Is this reusable?
- Is this data-driven where appropriate?
- Is this coupled to one level?
- Is this using Gameplay Tags correctly?
- Does it restore state?
- Does it emit useful debug info?
- Does it run every frame unnecessarily?
- Does it create hard references?
- Does it duplicate existing logic?
- Is this actually in scope?

---

# 33. Design Review Checklist for Code Changes

Before implementing gameplay behavior, verify:

- Does this support rule manipulation?
- Does this preserve multiple solutions?
- Does this make Reality behavior understandable?
- Does this create arbitrary restrictions?
- Does this accidentally create one universal solution?
- Does this align with the GDD?

---

# 34. Preferred Task Template

When creating or receiving an implementation task, use:

```text
TASK:
[Single clear task]

GOAL:
[Why the task exists]

REQUIREMENTS:
- ...
- ...
- ...

NON-GOALS:
- ...
- ...

FILES / SYSTEMS:
- ...

DONE WHEN:
- ...
- ...
- ...

VALIDATION:
- Build
- Runtime check
- Functional test
```

---

# 35. Example Task

```text
TASK:
Implement Collision modification in URealityEditableComponent.

GOAL:
Allow permitted world objects to toggle collision through Developer Console.

REQUIREMENTS:
- Respect Modify.Collision capability.
- Store original collision state.
- Support Apply and Restore.
- Emit FRealityCheatEvent.
- No UI dependency.
- Blueprint callable.
- Log invalid requests.

NON-GOALS:
- Developer Console UI.
- Suspicion calculation.
- NPC witness detection.

DONE WHEN:
- Project compiles.
- Component can be attached to a test StaticMeshActor.
- Collision toggles at runtime.
- Actor restores original collision.
- Cheat event is emitted.
```

---

# 36. Repository Hygiene

Do not commit:

- Generated build artifacts unless repository policy requires them
- Temporary debug files
- Local editor settings
- IDE cache
- Large unrelated binaries
- Test screenshots unless requested

Respect existing `.gitignore`.

---

# 37. Asset Changes

Do not rename or move Unreal assets casually.

Asset moves can break references.

If moving assets is required:

- Verify references
- Prefer Unreal-aware asset operations
- Report migration risk
- Avoid bulk renames without approval

---

# 38. Plugin Dependencies

Do not add new Unreal plugins without explicit approval.

If a feature requires a plugin:

Report:

```text
Plugin:
Reason:
Engine compatibility:
Project impact:
Alternative without plugin:
```

---

# 39. Engine Version

Do not silently upgrade Unreal Engine version.

If code requires a newer engine API, report it.

Maintain compatibility with the project’s configured engine version.

---

# 40. Final Rule

When uncertain, optimize for:

> **Clear systemic interactions that let the player creatively reason about the rules of the world.**

Do not optimize for:

- More features
- More code
- More abstraction
- More spectacle
- More content

unless it directly strengthens the core experience.

Reality.exe succeeds when players think:

> “Wait… would this work?”

and the game responds consistently.
