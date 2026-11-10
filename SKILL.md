# AI Developer System Instructions (SKILL.md)

> This file tells the AI agent HOW to work on **Ashes for a Goddess**, an **Unreal Engine 5.8** project (C++ and Blueprints) by Burnt Potato Games. Changed from Unity 6.3 HDRP on 10.11.2026 with the teacher's approval; the Unity skeleton is kept as the Git tag `unity-skeleton`.

## 1. User Profile & Output Behavior
- **Who I am:** Two game development students in *Game Studio: AI in Game Development*. Merve Özal: Lead Programmer, Level Designer, Audio Designer (player character, camera, LLM client, prompt builder, validator, level, audio). Tuana Tuna: Lead Artist, UI Designer, Narrative Designer, Programmer (lore bible, character and scene cards, fallback lines, portraits, UI screens, dance minigame, playtests). MISSING: C++ and Blueprint experience level of each member.
- **Language:** English (MISSING: confirm; the team also speaks Turkish).
- **Output:** MISSING: preferred answer style. Default: short, step by step for editor work, and explain any code you write well enough that both members can explain the prompt builder, validator and Scene Director (GDD 12.3).

## 2. Agent Capability & Execution Mode
- The agent (Claude Code, Codex or Antigravity) reaches the Unreal editor through **MCP + VibeUE**, as the course template assumes. It works on C++ in `Source/Ashes/`, Markdown in `Docs/`, CSV for DataTables, and uses VibeUE to create and edit Blueprints, Widget Blueprints, DataTables, levels and actors.
- `.uasset` and `.umap` files are binary: the agent **never** writes or edits them as text. All asset work goes through the editor (VibeUE) or the user.
- If VibeUE cannot do a step (or is not installed on that PC), give the user exact click-by-click editor steps instead.
- Prove every editor change with a screenshot or Output Log lines.

## 3. Tech Stack & Optimization
- **Engine:** Unreal Engine 5.8, Windows packaged build, started from the **Third Person (C++)** template. C++ module and prefix: `Ashes`. Both members on the same engine version.
- **Blueprint or C++:** C++ for the testable core (LLM HTTP client, prompt builder, response validator, value clamping, Scene Director, clue journal, dance grading, data structs). Blueprints for gameplay glue, level scripting, UMG UI, animation, Behavior Trees and the dance presentation. Blueprints derive from the C++ classes.
- **Systems:** Enhanced Input, UMG, DataTables and Data Assets, World and Game Instance Subsystems, AI Perception + Behavior Trees + NavMesh, Quartz (dance timing), MetaSounds, Niagara, Sequencer (Level Sequences), Lumen, USaveGame, HTTP + Json modules.
- **Tick rule:** keep Tick off (`PrimaryActorTick.bCanEverTick = false`) unless something truly must update every frame. Use delegates, timers (`FTimerManager`), Behavior Tree tasks, widget bindings to events and Quartz callbacks instead.
- **Networking:** `FHttpModule` requests with `SetTimeout(6)`, handled in the completion callback; never block the game thread.
- **LLM rule:** game code reaches the LLM only through `ULLMProvider`. The LLM only suggests; C++ (`UResponseValidator`, `USceneDirectorSubsystem`) decides and clamps.
- **API key:** never in the repository, `Config/*.ini` or the packaged build. Read it from the environment variable `ASHES_LLM_API_KEY` or `Secrets/llm_key.txt` (git-ignored). Without a key the game runs in offline mode.

## 4. Pre-Flight Checks
Before changing the project:
1. Check the MCP + VibeUE connection to the editor works (list the open level or the Content Browser).
2. Check that Git works, you are on a `feature/<name>-<task>` branch (never `main`), the last state is committed, and any `.uasset` / `.umap` you will touch is locked with `git lfs lock` by you.
3. Check `Ashes.uproject` reports Unreal Engine 5.8 and the C++ project builds.
4. Check no API key, `.env` or `Secrets/` file is staged (`git status`).

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
- Validator, JSON parsing, value clamping, Scene Director transitions, clue gate and dance grading have C++ Automation tests in `Source/Ashes/Tests/` (run in Session Frontend → Automation, filter `Ashes.`).
- Packaged builds only at milestones (Platforms → Windows → Package Project), output to `Builds/` (git-ignored).

## 8. Task Prioritization (Anti-Stuck Protocol)
- Foundation first: movement and free conversation (with the offline fallback) must work before UI polish, guards, memories or art.
- Every AI feature needs its fallback in the same phase (fallback lines, offline mode).
- After every phase: stop and ask the user to test. Do not continue until they confirm.
- If something fails twice the same way, stop, write it in `Docs/debug.md` and ask the user.

## 9. Version Control (Git)
- Commit after every phase that works, on a feature branch; open a pull request; the other member reviews; merge only when `L_Palace` plays. `main` always opens and runs.
- Commit message format: `[phase]: [what changed]`.
- Never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `.vs/`, `Builds/`, `.env`, `Secrets/`.
- `.uasset`, `.umap` and all art, audio, video and fonts go through Git LFS. Lock binary assets with `git lfs lock <file>` before editing; unlock after the merge. `L_Palace` uses One File Per Actor so both members can work in it at once.
- Every AI-assisted C++ file starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.`; AI-assisted Blueprints get the same text in a Comment node at the top of the Event Graph.

## 10. Naming Conventions
Follow `Docs/design.md` section 9. C++: `A` actor, `U` object / component / subsystem, `F` struct, `E` enum, `I` interface, project prefix `Ashes` where a name could clash. Assets: `BP_` Blueprint · `WBP_` widget · `GM_` / `GI_` / `PC_` framework Blueprints · `ABP_` animation Blueprint · `BT_` / `BB_` Behavior Tree / Blackboard · `M_` / `MI_` material / instance · `SM_` / `SK_` mesh · `T_` texture · `L_` level · `LS_` Level Sequence · `DT_` / `DA_` DataTable / Data Asset · `IA_` / `IMC_` input · `NS_` Niagara · `S_` / `MS_` sound. Levels: `L_Test`, `L_Title`, `L_Palace`.

## 11. UI Rules
- All UI is UMG and follows the tokens in `Docs/design.md`. Never invent a color, size or font that is not there; add the token first.
- Widgets update through delegates and events, never through per-frame property bindings.
- Relationship values are never shown as numbers; use the mood icon and hint line.

## 12. Data-Driven Design
- Characters, scenes, clues, dance charts, fallback lines, offline trees, cutscene scripts and tuning numbers live in DataTables (`DT_`, row structs in `Source/Ashes/Data/AshesTypes.h`) and Data Assets (`DA_`) in `Content/Ashes/Data/`, not hard-coded. DataTables can be edited as CSV and reimported.
- Adding a new character, scene or clue should only need a new row.
- The lore bible, character cards and key story lines are written by the team, never generated (GDD 12.3).

## 13. Lessons Learned
> Add a short note every time you fix a hard bug, so the AI never repeats it.
- 03.11.2026 Zafir answered "teasing" in 4 of 5 test replies → add a line to his card saying when he turns cold or angry.
- 10.11.2026 Engine changed from Unity 6.3 to Unreal Engine 5.8 → ignore Unity names (MonoBehaviour, ScriptableObject, prefabs, scenes) in older notes and in the tag `unity-skeleton`.

## 14. Session Handoff
- At the end of every session, update the Progress Tracker in `Docs/production_pipeline.md` and write a short note in `Docs/PROGRESS.md`: what was done, what is next, open problems.
- Tick the matching tasks on the phase's GitHub issue (Phase N = issue #N+2) and on the work plan page.
- Record only key prompts in the prompt log: the Google Sheet linked in `Docs/PromptLog.md` (shared with the instructor). A short row for a big design decision, a new document or asset, or a new way of using AI; not every phase or message (repeated work such as the next phase shares one row). Main prompt as sent with only spelling fixed, one or two sentences per cell. If you cannot edit the sheet, put the row in the waiting table in `Docs/PromptLog.md`.
- When the user says "continue where we left off": read `Docs/PROGRESS.md` and the tracker, then start the first open phase.

## 15. Live Editor Control
- Use MCP + VibeUE to inspect the open level, actors, Blueprints and the Output Log, to create assets and to run PIE.
- Prove each change with a screenshot or Output Log lines. If the tool is not connected, ask the user to paste Output Log errors and describe what they see in PIE.
- VibeUE (git submodule `Plugins/VibeUE`, pinned to dc051be) and Unreal MCP work on Merve's PC (tested 10.11.2026). MISSING: confirm on the second PC after `git submodule update --init` and a build.
