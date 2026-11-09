# AI Developer System Instructions (SKILL.md)

> This file tells the AI agent HOW to work on **Ashes for a Goddess**, a Unity 6.3 LTS (6000.3.27f2) HDRP project by Burnt Potato Games. The course template was written for Unreal Engine 5.8 with MCP + VibeUE; every rule below is adapted to Unity.

## 1. User Profile & Output Behavior
- **Who I am:** Two game development students in *Game Studio: AI in Game Development*. Merve Özal: Lead Programmer, Level Designer, Audio Designer (player controller, camera, LLM client, prompt builder, validator, level, audio). Tuana Tuna: Lead Artist, UI Designer, Narrative Designer, Programmer (lore bible, character and scene cards, fallback lines, portraits, UI screens, playtests). MISSING: C# experience level of each member.
- **Language:** English (MISSING: confirm; the team also speaks Turkish).
- **Output:** MISSING: preferred answer style. Default: short, step by step for editor work, and explain any code you write well enough that both members can explain the prompt builder, validator and Scene Director (GDD 12.3).

## 2. Agent Capability & Execution Mode
- The agent (Claude Code, GitHub Copilot) works on files in the repository: C# scripts, ScriptableObject text, Markdown docs.
- Unity scenes, prefabs and assets are YAML text (Force Text serialization), but the agent must **not** hand-write or hand-edit `.unity`, `.prefab`, `.asset` or `.meta` files beyond trivial value changes. Scene and prefab work is done in the Unity editor by the user, or through a Unity editor bridge if one is set up (MISSING: whether a Unity MCP bridge is used).
- When editor work is needed, give the user exact click-by-click steps.

## 3. Tech Stack & Optimization
- **Engine:** Unity 6.3 LTS (6000.3.27f2), HDRP, Windows standalone. Both members on the same version.
- **Main logic in:** C# (MonoBehaviours, ScriptableObjects, plain C# classes). Packages: Input System, Cinemachine, AI Navigation, Timeline, uGUI + TextMeshPro, Newtonsoft Json, VFX Graph, Unity Test Framework.
- **No Update rule:** do not use `Update` for gameplay or UI logic that can be event-driven. Use C# events, `UnityEvent`s, coroutines, async/await and timers.
- **Networking:** UnityWebRequest with async/await, 6 s timeout; never block the main thread.
- **LLM rule:** game code reaches the LLM only through `ILLMProvider`. The LLM only suggests; C# (`ResponseValidator`, `SceneDirector`) decides and clamps.
- **API key:** never in the repository or the build. Read it from an environment variable or `Secrets/` (git-ignored). Without a key the game runs in offline mode.

## 4. Pre-Flight Checks
Before changing the project:
1. Check the Unity version in `ProjectSettings/ProjectVersion.txt` is 6000.3.27f2.
2. Check that Git works, you are on a `feature/<name>-<task>` branch (never `main`), and the last state is committed.
3. Check no API key or `.env` file is staged (`git status`).

## 5. Planning & State Machine
- Before any work, read `Docs/gdd.md`, `Docs/design.md`, `Docs/technical_design.md`, `Docs/level-design.md` and `Docs/production_pipeline.md`.
- If one of them is missing, stop and tell the user.
- Work only on the next open phase in `Docs/production_pipeline.md`. Never build UI before the gameplay it shows.
- The schedule is 4 weeks (09.11.2026 – 06.12.2026). If a phase is late, apply the cut list in the pipeline's Scope table instead of adding work.

## 6. The Test Map
- New mechanics are built and tested first in the `Test` scene (`Assets/_Project/Scenes/Test.unity`), never directly in `Palace`.
- Leave the `Test` scene ready so the user can press Play and test.

## 7. Compile, Test & Build
- Day-to-day testing: Unity Play Mode and Edit Mode tests (Test Runner).
- After changing C#, make sure the Console has no compile errors before saying the step is done.
- Validator, JSON parsing, value clamping and Scene Director transitions must have Edit Mode tests in `Assets/_Project/Tests/EditMode`.
- Packaged builds only at milestones, output to `Builds/` (git-ignored).

## 8. Task Prioritization (Anti-Stuck Protocol)
- Foundation first: movement and free conversation (with the offline fallback) must work before UI polish, guards, memories or art.
- Every AI feature needs its fallback in the same phase (fallback lines, offline mode).
- After every phase: stop and ask the user to test. Do not continue until they confirm.

## 9. Version Control (Git)
- Commit after every phase that works, on a feature branch; open a pull request; the other member reviews; merge only when the `Palace` scene runs. `main` always opens and runs.
- Commit message format: `[phase]: [what changed]`.
- Never commit `Library/`, `Temp/`, `Obj/`, `Logs/`, `UserSettings/`, `Build/`, `Builds/`, `.env`, `Secrets/`.
- Art, audio, models, fonts and video go through Git LFS.
- Every AI-assisted script starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.`

## 10. Naming Conventions
Follow `Docs/design.md` section 9. PascalCase for classes and files, one class per file. Prefixes: `P_` prefab · `M_` material · `T_` texture · `SO_` ScriptableObject. MISSING: prefixes for UI prefabs, meshes and material variants. Scenes: `Boot`, `Title`, `Palace`, `Cutscenes`, `Test`.

## 11. UI Rules
- All UI follows the tokens in `Docs/design.md`. Never invent a color, size or font that is not there; add the token first.
- UI updates through events, never by checking values every frame.
- Relationship values are never shown as numbers; use the mood icon and hint line.

## 12. Data-Driven Design
- Characters, scenes, fallback lines, cutscene scripts and tuning numbers live in ScriptableObjects (`Assets/_Project/Data/`) or text files, not hard-coded.
- Adding a new character or scene should only need a new card asset.
- The lore bible, character cards and key story lines are written by the team, never generated (GDD 12.3).

## 13. Lessons Learned
> Add a short note every time you fix a hard bug, so the AI never repeats it.
- 03.11.2026 Zafir answered "teasing" in 4 of 5 test replies → add a line to his card saying when he turns cold or angry.

## 14. Session Handoff
- At the end of every session, update the Progress Tracker in `Docs/production_pipeline.md` and write a short note in `Docs/PROGRESS.md`: what was done, what is next, open problems.
- Record every AI chat that produced code, text or art in `Docs/PromptLog.md`.
- When the user says "continue where we left off": read `Docs/PROGRESS.md` and the tracker, then start the first open phase.

## 15. Live Editor Control
- MISSING: the template's MCP + VibeUE is Unreal-only. If the team installs a Unity editor bridge (MCP), use it to inspect scenes and read the Console, and prove each change with a screenshot or Console output. Without one, ask the user to paste Console errors and describe what they see in Play Mode.
