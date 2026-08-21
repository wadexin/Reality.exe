@'
# Reality.exe

> **You may cheat. Just don't get caught by reality.**

**Reality.exe** is a first-person systemic puzzle game built with Unreal Engine 5.

Players enter **Developer Mode** and modify the physical rules of selected objects to solve puzzles.  
There is rarely only one solution — unusual systemic solutions are part of the design.

## Playable Demo

**Windows · English / 简体中文**

https://wadexin.itch.io/reality-exe

## Core Gameplay

Players can modify six world parameters:

- Collision
- Scale
- Gravity
- Mass
- Friction
- Local Time

Reality also evaluates whether an action appears plausible through systems such as:

- Suspicion
- Witnesses
- Context

The goal is not simply to break the rules, but to break them without making Reality notice.

## Demo Content

The current demo contains approximately **20–30 minutes** of first-play content, including:

- Developer Mode and Developer Console
- Multi-solution systemic puzzles
- Security and witness mechanics
- Sensor failure / context-based interactions
- Physics-based puzzle spaces
- Local Time scanner puzzle
- Checkpoints, position recovery and restart flow
- English and Simplified Chinese presentation

## Controls

| Input | Action |
| --- | --- |
| WASD | Move |
| Mouse | Look |
| Space | Jump |
| E | Interact |
| F6 | Developer Mode |
| Esc | System Menu |

## Tech Stack

- Unreal Engine 5
- C++
- Blueprint
- UMG
- Gameplay Tags
- Enhanced Input
- Git / GitHub
- Blender

## Development Highlights

- Built a reusable world-parameter editing framework for multiple gameplay abilities.
- Designed puzzle spaces around multiple valid solutions instead of fixed answer paths.
- Implemented Suspicion, Witness and Context systems for evaluating anomalous player actions.
- Developed Developer Mode, target selection, UI feedback and recovery systems.
- Solved packaged-build input and Chinese text rendering issues during development.
- Completed Windows packaging, English / Simplified Chinese builds and public itch.io release.

## Project Structure

```text
Reality/
├─ Source/        C++ gameplay systems
├─ Content/       Unreal Engine assets
├─ Config/        Project configuration
└─ Docs/          Game design and development documentation
