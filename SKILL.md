# AI Developer System Instructions (SKILL.md)

> This file tells the AI agent HOW to work on **Ashes for a Goddess**, an **Unreal Engine 5.8** project (C++ and Blueprints) by Burnt Potato Games. Changed from Unity 6.3 HDRP on 10.11.2026 with the teacher's approval; the Unity skeleton is kept as the Git tag `unity-skeleton`.

## 1. User Profile & Output Behavior
- **Who I am:** A team of four game development students in the course *Advanced Game Programming* (since 12.11.2026; Merve and Tuana before that). **Merve Özal**: team lead and level designer (palace grey-box, rooms, spawn points, guard routes, hiding and eavesdrop spots, lighting), leads the story and lines with Claude, enters the data, keeps the tracker, board and prompt log; she wrote the Phase 0–5 code (movement, interaction, built-in AI, prompt builder, validator, conversation screen). **Tuana Tuna**: lead artist and UI artist (portraits, character looks, palace kit, UI style, 2D backgrounds); reviews the writing. **Tala**: audio and VFX (music, dance songs and charts, SFX and mix, Niagara, dance animations, cutscene sequences, credits, licenses, playtests). **Baha**: programmer (C++ systems from Phase 7 on: Scene Director, offline trees, clues, guards, dance, cutscene player, spirit walk, packaging). MISSING: C++ and Blueprint experience level of each member.
- **Language:** English (MISSING: confirm; the team also speaks Turkish).
- **Output:** MISSING: preferred answer style. Default: short, step by step for editor work, and explain any code you write well enough that every team member can explain the prompt builder, validator and Scene Director (GDD 12.3). Address the person who is working (check who is asking: Baha for code, Merve for level and data, Tala for audio and VFX, Tuana for art and UI).

## 2. Agent Capability & Execution Mode
- The agent (Claude Code, Codex or Antigravity) reaches the Unreal editor through **MCP + VibeUE** (git submodule `Plugins/VibeUE`). It works on C++ in `Source/Ashes/`, Markdown in `Docs/`, CSV for DataTables, and uses VibeUE to create and edit Blueprints, Widget Blueprints, DataTables, levels and actors.
- `.uasset` and `.umap` files are binary: the agent **never** writes or edits them as text. All asset work goes through the editor (VibeUE) or the user.
- If VibeUE cannot do a step (or is not installed on that PC), give the user exact click-by-click editor steps instead.
- Prove every editor change with a screenshot or Output Log lines.

## 3. Tech Stack & Optimization
- **Engine:** Unreal Engine 5.8, Windows packaged build, started from the **Third Person (C++)** template. C++ module and prefix: `Ashes`. Every team member on the same engine version.
- **Blueprint or C++:** C++ for the testable core (LLM HTTP client, prompt builder, response validator, value clamping, Scene Director, clue journal, offline trees, dance grading, spirit walk, data structs). Blueprints for gameplay glue, level scripting, UMG UI, animation, Behavior Trees and the dance presentation. Blueprints derive from the C++ classes.
- **Systems:** Enhanced Input, UMG, DataTables and Data Assets, World and Game Instance Subsystems, AI Perception + Behavior Trees + NavMesh, Quartz (dance timing), MetaSounds, Niagara, Sequencer (Level Sequences), Lumen, USaveGame, HTTP + Json modules.
- **Tick rule:** keep Tick off (`PrimaryActorTick.bCanEverTick = false`) unless something truly must update every frame. Use delegates, timers (`FTimerManager`), Behavior Tree tasks, widget bindings to events and Quartz callbacks instead.
- **Networking:** `FHttpModule` requests with a 6 s timeout in the game (30 s in the editor and PIE only), handled in the completion callback; never block the game thread.
- **LLM rule:** game code reaches the LLM only through `ULLMProvider`. The LLM only suggests; C++ (`UResponseValidator`, `USceneDirectorSubsystem`) decides and clamps.
- **AI model and keys:** the AI model is built into the game (llama.cpp `llama-server` + Qwen3.5-4B, started by `ULocalModelSubsystem`; downloaded by `Tools/GetLocalLLM.ps1` into git-ignored `ThirdParty/LLM`); it needs no key and no internet. On a PC with 4 GB of VRAM set `ASHES_LLM_DEVICE=none` (CPU). An optional external endpoint key is never in the repository, `Config/*.ini` or the packaged build (environment variable `ASHES_LLM_API_KEY` or `Secrets/llm_key.txt`, git-ignored). Without the model the game runs in offline mode (offline trees and fallback lines).
- **Story rules for prompts and data:** 7 chapters; turn limits Zafir 11, Vashir 7, Nura, Idris and the minor characters 5, ambient NPCs 1 line; secrets enter a prompt only from their chapter; clues are authored (`DT_Clues`), never invented by the model. The Master Lore document is the single source for every game text.

## 4. Pre-Flight Checks
Before changing the project:
1. Check the MCP + VibeUE connection to the editor works (list the open level or the Content Browser).
2. Check that Git works, you are on a `feature/<name>-<task>` branch (never `main`), the last state is committed, and any `.uasset` / `.umap` you will touch is locked with `git lfs lock` by you.
3. Check `Ashes.uproject` reports Unreal Engine 5.8 and the C++ project builds.
4. Check no API key, `.env`, `Secrets/` or `ThirdParty/LLM` file is staged (`git status`).

## 5. Planning & State Machine
- Before any work, read `Docs/gdd.md`, `Docs/design.md`, `Docs/technical_design.md`, `Docs/level-design.md` and `Docs/production_pipeline.md`.
- If one of them is missing, stop and tell the user.
- Work only on the next open phase in `Docs/production_pipeline.md`. Never build UI before the gameplay it shows.
- The schedule is 4 weeks (09.11.2026 – 06.12.2026). If a phase is late, apply the cut list in the pipeline's Scope table instead of adding work.

## 6. The Test Map
- New mechanics are built and tested first in `L_Test` (`Content/Ashes/Maps/L_Test`), never directly in `L_Palace`.
- Leave `L_Test` ready so the user can press Play In Editor (PIE) and test.

## 7. Compile, Test & Build
- After changing C++, build (Live Coding for small changes, a full editor build after header or reflection changes) and make sure there are no compile errors before saying the step is done.
- Day-to-day testing: Play In Editor; check the Output Log for errors and warnings.
- Validator, JSON parsing, value clamping, Scene Director transitions, clue gate, offline trees and dance grading have C++ Automation tests in `Source/Ashes/Tests/` (run in Session Frontend → Automation, filter `Ashes.`).
- Packaged builds only at milestones (Platforms → Windows → Package Project), output to `Builds/` (git-ignored).

## 8. Task Prioritization (Anti-Stuck Protocol)
- Foundation first: movement and free conversation (with the offline fallback) must work before UI polish, guards, memories or art.
- Every AI feature needs its fallback in the same phase (offline trees, fallback lines, the pre-written Chapter 7 confession).
- After every phase: stop and ask the user to test. Do not continue until they confirm.
- If something fails twice the same way, stop, write it in `Docs/debug.md` and ask the user.

## 9. Version Control (Git)
- Commit after every phase that works, on a feature branch; open a pull request; another team member reviews; merge only when `L_Palace` plays. `main` always opens and runs.
- Commit message format: `[phase]: [what changed]`.
- Never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `.vs/`, `Builds/`, `.env`, `Secrets/`.
- `.uasset`, `.umap` and all art, audio, video and fonts go through Git LFS. Lock binary assets with `git lfs lock <file>` before editing; unlock after the merge. `L_Palace` uses One File Per Actor so several people can work in it at once.
- Every AI-assisted C++ file starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.`; AI-assisted Blueprints get the same text in a Comment node at the top of the Event Graph.

## 10. Naming Conventions
Follow `Docs/design.md` section 9. C++: `A` actor, `U` object / component / subsystem, `F` struct, `E` enum, `I` interface, project prefix `Ashes` where a name could clash. Assets: `BP_` Blueprint · `WBP_` widget · `GM_` / `GI_` / `PC_` framework Blueprints · `ABP_` animation Blueprint · `BT_` / `BB_` Behavior Tree / Blackboard · `M_` / `MI_` material / instance · `SM_` / `SK_` mesh · `T_` texture · `L_` level · `LS_` Level Sequence · `DT_` / `DA_` DataTable / Data Asset · `IA_` / `IMC_` input · `NS_` Niagara · `S_` / `MS_` sound. Levels: `L_Test`, `L_Title`, `L_Palace`.

## 11. UI Rules
- All UI is UMG and follows the tokens in `Docs/design.md`. Never invent a color, size or font that is not there; add the token first.
- Widgets update through delegates and events, never through per-frame property bindings.
- Relationship values are never shown as numbers; use the mood icon and hint line.

## 12. Data-Driven Design
- Characters, scenes, clues, barks, dance charts, fallback lines, offline trees, cutscene scripts and tuning numbers live in DataTables (`DT_`, row structs in `Source/Ashes/Data/AshesTypes.h`) and Data Assets (`DA_`) in `Content/Ashes/Data/`, not hard-coded. DataTables can be edited as CSV and reimported.
- Adding a new character, scene or clue should only need a new row.
- The story, twist, lore bible and key story lines are the team's own (GDD 12.3). Game-data lines (cards, clues, barks, fallback lines, offline trees) may be drafted with Claude from the team's story, then reviewed and rewritten by the team (Tuana reviews); mark them AI-assisted.

## 13. Lessons Learned
> Add a short note every time you fix a hard bug, so the AI never repeats it.
- 03.11.2026 Zafir answered "teasing" in 4 of 5 test replies → add a line to his card saying when he turns cold or angry.
- 10.11.2026 Engine changed from Unity 6.3 to Unreal Engine 5.8 → ignore Unity names (MonoBehaviour, ScriptableObject, prefabs, scenes) in older notes and in the tag `unity-skeleton`.
- 10.11.2026 UE 5.8 generates default bodies for interface events → don't define them again in `AshesInteractable.cpp` (error C2084).
- 11.11.2026 On the 4 GB laptop the AI on the GPU slowed to 3 tok/s while the editor rendered → `ASHES_LLM_DEVICE=none` (CPU, ~9 tok/s).
- 11.11.2026 `FValidationContext` already exists in the engine → our types are `FAshesValidationContext` / `FAshesValidationResult`.
- 12.11.2026 The story moved from 4 to 7 chapters and the turn limits changed (11 / 7 / 5) → older notes with 4 chapters, 10 / 8 / 4 turns or "fallback lines only" are out of date.

## 14. Session Handoff
- At the end of every session, update the Progress Tracker in `Docs/production_pipeline.md` and write a short note in `Docs/PROGRESS.md`: what was done, what is next, open problems.
- Tick the matching tasks on the phase's GitHub issue (Phase N = issue #N+2) and on the work plan page.
- Record only key prompts in the prompt log: the Google Sheet linked in `Docs/PromptLog.md` (shared with the instructor). About once a day, one short row for that day's big change (a big design decision, a new document or asset, or a new way of using AI); not every phase or message (repeated work such as the next phase shares one row). Main prompt as sent with only spelling fixed, one or two sentences per cell. If you cannot edit the sheet, put the row in the waiting table in `Docs/PromptLog.md`.
- When the user says "continue where we left off": read `Docs/PROGRESS.md` and the tracker, then start the first open phase.

## 15. Live Editor Control
- Use MCP + VibeUE to inspect the open level, actors, Blueprints and the Output Log, to create assets and to run PIE.
- Prove each change with a screenshot or Output Log lines. If the tool is not connected, ask the user to paste Output Log errors and describe what they see in PIE.
- VibeUE (git submodule `Plugins/VibeUE`, pinned to dc051be) and Unreal MCP work on Merve's PC (tested 10.11.2026). MISSING: confirm on Tuana's, Tala's and Baha's PCs after `git submodule update --init` and a build.
