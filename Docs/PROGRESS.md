# Ashes for a Goddess · Progress Log (PROGRESS.md)

> Newest entry on top. Write one entry at the end of every work session.

## 10.11.2026 (engine switch)
- **Done:** The teacher approved Unreal Engine 5.8, so the project moves from Unity 6.3 HDRP to UE 5.8 (C++ and Blueprints) in the same repository. Rewrote `technical_design.md` for Unreal (C++ core classes, DataTables, subsystems, Enhanced Input, AI Perception + Behavior Trees, Quartz dance timing, UMG, Automation tests). Updated `gdd.md` (engine, controls, Development tables, AI tool plan), `design.md` (Unreal prefixes, `Content/Ashes/` folders, UMG), `level-design.md` (`L_Palace`, One File Per Actor, NavMesh in cm), `production_pipeline.md` (all phases in Unreal terms, new Phase 0), `SKILL.md`, `CLAUDE.md`, `debug.md`, `README.md` and the PR template. Removed the Unity `Assets/`, `Packages/`, `ProjectSettings/`; new Unreal `.gitignore` and `.gitattributes` (LFS, `.uasset` / `.umap` lockable). Work plan page and GitHub issues #2–#24 switched to Unreal task text.
- **Next:** Tag the last Unity commit `2090454` as `unity-skeleton`. Then Phase 0: create the UE 5.8 Third Person C++ project `Ashes` in the repository folder, `L_Test` and `L_Palace`, MCP + VibeUE on both PCs.
- **Problems:** About one day of week 1 went to the engine switch; Phase 0 is now due Tue 10.11. Both PCs need Visual Studio 2022 with the C++ game workload. The GDD's Unity prefixes (`P_`, `SO_`) are replaced by Unreal prefixes.
- **Commit:** `docs: switch planning docs and repo from Unity 6.3 to Unreal Engine 5.8`

## 10.11.2026
- **Done:** Game concept pinned down with the team: "Court of Whispers". The palace is open; each chapter is a day and a night. Day: gather clues into the Rumor Journal while avoiding patrolling guards (hiding spots, eavesdropping). Night: the dance minigame every night to keep Sahra's cover (good dance = Favor + reward, bad dance = Suspicion). Then the key conversation and a cutscene. Updated `gdd.md`, `design.md`, `technical_design.md`, `level-design.md` and `production_pipeline.md` (Stage 2 now: week 2 all systems on Chapter 1, week 3 all content; phases renumbered 0–22).
- **Next:** Phase 0 in `production_pipeline.md` (scenes, assembly definitions, task board), then Phase 1.
- **Problems:** Week 2 is the heaviest week (Scene Director, palace, clues, guards, dance); the cut order is in the pipeline. Proposed numbers (clue counts, patrol timing, dance keys ← ↑ ↓ →, nightfall trigger) need team confirmation. Play time grows to about 25–30 minutes.
- **Commit:** `docs: open-palace day/night loop with clues, guards and nightly dance`

## 09.11.2026
- **Done:** Planning docs filled from the midterm GDD: `Docs/gdd.md`, `design.md`, `technical_design.md`, `level-design.md`, `production_pipeline.md` (4-week plan, 09.11 – 06.12.2026), `PROGRESS.md`, `debug.md`, plus `CLAUDE.md` and `SKILL.md` at the repo root. All docs adapted from the Unreal templates to Unity 6.3 HDRP.
- **Next:** Phase 0 in `production_pipeline.md`: create `Boot`, `Test` and `Palace` scenes and add them to Build Profiles; assembly definitions for scripts and Edit Mode tests; task board.
- **Problems:** Open `MISSING:` items in the docs need team decisions (design pillars, typography, UI sounds, camera values, dance keys, LLM provider choice, Unity editor bridge). The GDD still says Unity 6000.0.x; the project uses 6000.3.27f2. GDD Figure 2 says a 5 s timeout, the text says 6 s (6 s used).
- **Commit:** `docs: fill planning docs from GDD with 4-week plan`

## 03.11.2026
- **Done:** Repository created with the Unity 6.3 HDRP skeleton, packages, folder layout, `.gitignore`, `.gitattributes` (LFS), README; HDRP Wizard default resources committed.
- **Next:** Planning docs.
- **Problems:** None recorded.
- **Commit:** `2090454 graph-set`
