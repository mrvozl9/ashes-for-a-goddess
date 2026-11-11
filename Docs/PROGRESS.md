# Ashes for a Goddess · Progress Log (PROGRESS.md)

> Newest entry on top. Write one entry at the end of every work session.

## 11.11.2026 (Phase 4: prompt builder and validator)
- **Done:** Phase 3 merged (#29). C++: row structs and conversation types (`Source/Ashes/Data/AshesTypes.h`: `FCharacterCardRow`, `FSceneCardRow`, `FBeatDef`, `FRelationshipValueDef`, `FChapterSecret`, `UAshesLoreBible`, `FLLMResponse`, `FPromptState`, `FConversationTurn`), `UPromptBuilder` (GDD 8.1 order, secrets only from their chapter, values as words, last 8 turns, 200-char input), `UResponseValidator` (JSON also inside code fences, retry flag, emotion whitelist + fallback, 80 words, delta clamp, beat / clue ids only, spoiler and safety → regenerate), console `Ashes.TalkZafir <line>` / `Ashes.TalkZafir.Reset` (real prompt + validator, 1 retry, then fallback). Data: `DT_Characters` (Zafir), `DT_Scenes` (`Ch1_ZafirKeyScene`), `DA_LoreBible` in `Content/Ashes/Data` with placeholder text taken from the GDD. Tests: 8 Automation tests green (`Ashes.Validator.*`, `Ashes.PromptBuilder.*`). Conversation in the Output Log: Zafir answers in character with emotion, gesture, 3 suggestions; values clamped and applied; "Tell me about your phone" flagged off-topic; "Did Vashir hire you?" answered without the keywords "contract" / "hired". Fixed Bug 2 (cold-start request garbage).
- **Next (Tuana, hand-written, not AI):** in the editor, `Content/Ashes/Data`: (1) `DA_LoreBible` Text: the ~600-word lore bible (now a 45-word placeholder from the GDD premise); (2) `DT_Characters` row `Zafir`: Voice, Wants, Fears, Speech rules (add when he turns cold or angry, 03.11 test), Knows, Must not say, the Chapter 4 secret text, spoiler keywords (now GDD placeholders; Fears and Knows empty); (3) `DT_Scenes` row `Ch1_ZafirKeyScene`: goal, the two beat rules, closing lines full / weaker (empty). Merve: test `Ashes.TalkZafir`, then the 5 lines from the 03.11 test (GDD 8.2) and measure reply times. Then PR.
- **Problems:** On the laptop replies take 15–29 s at ~9 tok/s (fine for the 30 s editor limit). In one reply Zafir said "I am here for Vashir, yes" – not a leak by keyword, but Tuana's card should make his cover story clear. Safety word list is a placeholder (MISSING: team list).
- **Commit:** `Phase 4: prompt builder, response validator and data tables (placeholders)`

## 11.11.2026 (Phase 3: editor-only AI timeout)
- **Done:** New setting `EditorRequestTimeoutSeconds` (30 s) used only in the editor and Play-in-Editor, so real replies show up on the slow laptop while developing; packaged builds always use `RequestTimeoutSeconds` (6 s), nothing to switch back before packaging. Tested: two `Ashes.TestLLM` runs in the editor answered in 8.7 s and 7.7 s (26–28 tok/s). GDD 8.1 and technical_design updated.
- **Next:** Speed test on the stronger PC; then Phase 4.
- **Problems:** None.
- **Commit:** `[phase 3]: 30 s AI timeout in the editor, 6 s in the game`

## 11.11.2026 (Phase 3 fix: cancel timed-out AI requests)
- **Done:** Fixed the second-request delay (debug.md Bug 1): AI requests are now streamed, so when the game gives up after the timeout the server stops writing that answer. Verified: two back-to-back `Ashes.TestLLM` runs with 6 s both fall back after ~6 s with the server idle right after; with 30 s Zafir's reply came in 9.0 s (221 tokens, 26 tok/s on a freshly opened editor). Closing the editor also stops llama-server (job object).
- **Next:** Merve decides on compact answers (fix B); word-by-word display planned for Phase 5; speed test on the stronger PC.
- **Problems:** The model fills `beats` with free text instead of beat ids; the prompt builder and validator (Phase 4) handle that.
- **Commit:** `[phase 3]: cancel timed-out AI requests`

## 10.11.2026 (Phase 3: built-in AI model)
- **Done:** Decision with Merve: the AI model ships inside the game (free, no account, no key, no internet; players download nothing else). Chosen: llama.cpp `llama-server` b12138 (Windows, Vulkan, MIT) + Qwen3.5-4B Q4_K_M GGUF (Apache 2.0, 2.7 GB); `Tools/GetLocalLLM.ps1` downloads both pinned with SHA-256 checks into git-ignored `ThirdParty/LLM` (or `ASHES_LLM_DIR`; on PC 1: `C:\Users\aliio\Projects\AshesLLM`). C++: `ULLMProvider`, `UOpenAICompatibleLLMProvider` (FHttpModule, 6 s timeout, JSON output, thinking off), `UOfflineLLMProvider` (fallback JSON), `ULocalModelSubsystem` (hidden llama-server on a free 127.0.0.1 port, `/health` polling, Windows job object), `UAshesAISettings` (Project Settings > Game > Ashes AI), `UAshesGameInstance` (AI mode, `RequestReply` with automatic fallback; set as GameInstanceClass), console commands `Ashes.TestLLM`, `Ashes.LLM.Start/Stop/Status`. Docs: GDD 8.1–8.4, technical_design (classes, section 12 decision), pipeline (Phase 3, scope), Licenses.md.
- **Tests (PC 1, laptop RTX 3050 Ti 4 GB + Intel UHD):** server ready in 13.6 s; `Ashes.TestLLM` → valid in-character JSON from Zafir in 9.7 s (154 tokens, 18.1 tok/s); 6 s timeout → fallback after 6.3 s, no freeze; model file missing in PIE → game instance answers with a fallback at once. Manual runs: 7–12 tok/s with the editor open; CPU-only 11.8 tok/s faster than the laptop GPU.
- **Next:** Speed test on Merve's stronger PC (target: reply under 6 s). Team confirms dropping the full offline dialogue trees. Then Phase 4 (prompt builder, validator, lore + Zafir's card).
- **Problems:** Too slow on the laptop for the 6 s target (10–30 s per reply). If the stronger PC is also slow: smaller model, shorter replies or word-by-word streaming (decide at the Phase 12 risk check). Not yet tested: the server being killed when the game crashes (job object).
- **Commit:** `Phase 3: built-in AI model (llama.cpp + Qwen3.5-4B) with fallback`

## 10.11.2026 (Phase 2: interaction system)
- **Done:** C++ `IAshesInteractable` (prompt text, can-interact, interact, focus), `UInteractionComponent` on Sahra (sphere overlap every 0.1 s on a timer, radius 200 cm, nearest target in front, `OnFocusChanged` / `OnInteract`; E = `IA_Interact` → `TryInteract`), `AAshesNPC` (talkable character base with a screen-space "E: Talk" Widget Component; Phase 2 placeholder: E logs "Start scene: <name>" and freezes Sahra, E again logs "End scene" and unfreezes). Through VibeUE: `WBP_InteractPrompt` (Old gold frame, Night plum panel, Moon cream text), `BP_Zafir` (Manny mannequin, idle) placed in `L_Test` at (−900, 0); `BP_Sahra` got `IA_Interact`. Radius recorded in `technical_design.md` section 3; `IAshesInteractable` and `AAshesNPC` added to the class table (derived). Scripted PIE test in `L_Test`: no prompt at 600 cm, prompt at 180 cm, E → "Start scene: Zafir" and W no longer moves Sahra, E again → "End scene: Zafir" and movement back, prompt hidden at 700 cm; no errors in the Output Log.
- **Next:** Merve tests the Phase 2 gate in PIE by hand, then pull request → review → merge. Then Phase 3 (LLM provider, cloud + offline): needs the provider / model choice and a spending cap.
- **Problems:** UE 5.8 generates default bodies for interface events, so `AshesInteractable.cpp` must not define them (build error C2084 on the first try, fixed). `L_Test` and `BP_Sahra` are locked with `git lfs lock` until the merge.
- **Commit:** `Phase 2: interaction system (E: Talk)`

## 10.11.2026 (Phase 1: player movement and camera)
- **Done:** `AAshesCharacter`: walk 220 cm/s, run 450 cm/s while Left Shift is held (`bIsRunning`), `SetGameplayInputEnabled(bool)`, no jump (`CanJumpInternal` false; template `DoJumpStart`/`DoJumpEnd` kept as empty stubs so template Blueprints still compile), Tick off, orbit camera (arm 350 cm, over-the-right-shoulder offset, lag 12, FOV 80). `AAshesPlayerController`: `ExplorationMappingContext` slot, pitch limits −60° / +25°. Through VibeUE: `IA_Move`, `IA_Look`, `IA_Run`, `IA_Interact`, `IA_OpenJournal`, `IA_Pause`, `IMC_Exploration` (W A S D, mouse, Left Shift, E, Tab, Esc) and empty `IMC_Conversation`, `IMC_Cutscene`, `IMC_Dance`; `ABP_Sahra` (copy of the template `ABP_Unarmed`: idle / walk / run), `BP_Sahra` (Quinn mannequin), `PC_Ashes`; `GM_Ashes` now uses `BP_Sahra` and `PC_Ashes`. Camera values recorded in `technical_design.md` section 3. Scripted PIE test in `L_Test`: W = 220 cm/s, Shift + W = 450 cm/s, Space does not jump, pitch limits and arm applied, no errors in the Output Log.
- **Next:** Merve tests the Phase 1 gate in PIE by hand (W A S D, Shift, mouse orbit, Space), then pull request → Tuana reviews → merge. Then Phase 2 (interaction system).
- **Problems:** None. The template Blueprints (`BP_ThirdPersonCharacter`) still reference the old `JumpAction`; harmless, they are not used by the game.
- **Commit:** `Phase 1: player movement and camera`

## 10.11.2026 (Phase 0: Unreal project)
- **Done:** Tagged the last Unity commit as `unity-skeleton`. Created the UE 5.8 Third Person C++ project `Ashes` (Project Browser) and added the modules `HTTP`, `Json`, `JsonUtilities`, `NavigationSystem`, `GameplayTasks`; `AshesEditor` builds (Visual Studio 2026, MSVC 14.51). Set up Unreal MCP (auto-starts on `http://127.0.0.1:8000/mcp`) and VibeUE (git submodule `Plugins/VibeUE`, pinned to dc051be), `.mcp.json` for Claude Code. Through VibeUE: `Content/Ashes/` folders, `L_Test` (40 × 40 m floor, 4 m walls, ramp to a 2 m platform, moonlit night sky, Nav Mesh Bounds Volume; navmesh path up the ramp checked), empty `L_Palace` with One File Per Actor, `GM_Ashes` (copy of the template game mode) as default game mode, `L_Test` as editor startup map. PIE in `L_Test`: `GM_Ashes` runs, the mannequin spawns, no errors in the Output Log.
- **Next:** Tuana: clone, `git submodule update --init`, open and build, PIE in `L_Test` (Phase 0 gate on PC 2), approve the `unreal-mcp` server in Claude Code. Then review and merge the pull request into `main` and start Phase 1.
- **Problems:** The repository lives in OneDrive (`Documents/Unreal Projects/Ashes`); OneDrive sync can lock files or conflict with Git, so moving it outside OneDrive is recommended. Functional Testing plugin and the editor's Git source control setting are still open. Both PCs need Visual Studio with the C++ game workload (2022 or 2026).
- **Commit:** `Phase 0: Content/Ashes folders, L_Test, L_Palace and GM_Ashes`

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
