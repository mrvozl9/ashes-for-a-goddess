# Ashes for a Goddess · Production Pipeline (State Machine)

> **Canonical design source:** `gdd.md`. Companions: `technical_design.md` (how the code works) and `level-design.md` (where things are). This file is the **order of operations**: small phases, in order, each with a hard check before the next one starts.
>
> **How to use this file:** treat each Phase as a node in a state machine. A phase may not start until its `Depends on` phases are `DONE`. Every phase ends with a **Verification Gate**: the feature is tested in **Play In Editor (PIE)** in Unreal Engine 5.8 and the user confirms before moving on. Never build UI before the gameplay it shows. Never build a level before its mechanics work in the `L_Test` level.
>
> **Time box: 4 weeks in total, Monday 09.11.2026 to Sunday 06.12.2026.** Final build on Saturday 05.12.2026; Sunday 06.12 is buffer only. This compresses the GDD's Weeks 9–14 plan (which ended 20.12.2026). **Team (from 12.11.2026): four people.** **Merve**: level design (grey-box, rooms, spawn points, guard routes, hiding and eavesdrop spots, lighting), story and lines (with Tuana's review), data entry, tracker and board. **Tuana**: art (portraits, character looks, palace kit, UI style, 2D backgrounds). **Tala**: audio, VFX and other work (music, dance songs and charts, SFX, mix, Niagara effects, dance animations, cutscene sequences, credits, licenses, playtests). **Baha**: code (C++ and Blueprint systems from Phase 7 on; Merve wrote Phases 0–5). Four tracks run in parallel; a phase lists who owns it. Before 12.11 the team was Merve and Tuana.
>
> **Engine (10.11.2026):** Unreal Engine 5.8, C++ for the testable core and Blueprints for glue, UI and animation (see `technical_design.md`). The earlier Unity skeleton is kept as the Git tag `unity-skeleton`.
>
> **Updated 10.11.2026 for the agreed gameplay:** each chapter is a day and a night in the open palace. Day = gather clues into the **Rumor Journal** while **avoiding patrolling guards** (hiding spots, eavesdropping). Night = the **nightly dance** minigame to keep Sahra's cover. Then the key conversation and an in-engine ending. Guards and the dance are now must-haves.
>
> **Updated 12.11.2026 for the 7-chapter story (course: Advanced Game Programming):** Prologue (2D) → Chapters 1–6 in the palace (1 The Garden of Ash, 2 The Practice Room, 3 The Private Dance, 4 The King's Favor, 5 Rooftop of Smoke, 6 The Trap) → Chapter 7 The Pyre (3D spirit walk in the ghost palace) → Epilogue (2D). Each palace chapter has a main quest, 2 "do" objectives, 6 clues (any 3 open the key scene), a night dance (30 → 45 s) and an in-engine ending. Turn limits: Zafir 11, Vashir 7, Nura, Idris and the 4 minor characters (Omar, Ilyas, Tariq, Aisha) 5 at most, about 10 ambient NPCs 1 line. All game texts (cards, 36 clues, 60 barks + 7 eavesdrops, 142 fallback lines, full offline trees) are in the Master Lore document. Where Phase 0–6 tasks below mention the old plan (10 turns, fallback lines only), they are kept as written at the time.

**Legend:** `[C++]` C++ code · `[BP]` Blueprint / editor work · `[UE]` project, level or editor setup · `[REPO]` Git · `[TOOLS]` tool setup · `[ART]` assets · `[UI]` interface · `[AUDIO]` sound · `[WRITE]` lore, cards, lines · `⚠️` risk / decision

---

## MACRO-STAGE MAP
```
STAGE 1: PROTOTYPE           STAGE 2: FULL GAME              STAGE 3: POLISH
Week 1: 09.11 – 15.11        Weeks 2–3: 16.11 – 29.11        Week 4: 30.11 – 06.12
Phases 0 to 6                Phases 7 to 17                  Phases 18 to 22
      |                             |                               |
 [PROTOTYPE GATE 15.11] ---> [CONTENT GATE 29.11] ---------> [FINAL BUILD 05.12]
                     (week 2 = all systems, week 3 = all content)
```

**Stage 1 goal:** Sahra walks and talks in the test level; one conversation with Zafir (Chapter 1) works end to end, online through the LLM and offline through pre-written lines, with the validator in between.
**Stage 2 goal:** Week 2 builds every gameplay system and finishes Chapter 1 as a complete day and night (clues and Rumor Journal, guards with hiding spots and eavesdropping, nightly dance, key scene, in-engine ending). Week 3 fills Chapters 2–6, the Chapter 7 spirit walk, the 2D Prologue and Epilogue, HUD and menus, offline mode (trees and fallback lines) for every scene. First playtest with outside players.
**Stage 3 goal:** Art, VFX and audio pass, tuning from playtests (dance difficulty, patrol timing, clue counts, turn limits), red-team pass with 0 spoiler leaks, Windows build that runs the built-in model without a key and falls back to offline mode, credits and AI disclosure.

### Scope for 4 weeks
| Priority | Feature | In the 4-week plan |
| :-- | :-- | :-- |
| Must | Open palace Sama'zar (Garden of Dusk hub + 5 rooms, practice room, servants' passage, City Gate), third-person movement and camera | Yes (grey-box week 2, art week 4) |
| Must | LLM conversations with Zafir (11 turns) and Vashir (7 turns) (prompt builder, validator, clamped values) | Yes (weeks 1–2) |
| Must (decided 11.11) | Nura, Idris and the 4 minor characters (Omar, Ilyas, Tariq, Aisha) as LLM characters, 5 turns at most | Yes (week 3 content); offline nodes for every clue |
| Must | Scene Director with nudge, forced close and forced beats (Chapter 6) | Yes (week 2) |
| Must (new 09.11) | Clues and Rumor Journal; 36 clues, key scene locked until 3 of 6 | Yes (week 2 system, week 3 content) |
| Must (raised 10.11) | Guards on timed routes, Suspicion, hiding spots, eavesdropping (7 eavesdrops) | Yes (week 2), simplified: no chasing |
| Must (new 11.11) | Following (Chapter 4 tail) and being followed (Chapter 5 king's servant) | Yes (week 3) |
| Must (raised 10.11) | Nightly dance minigame, 6 nights (30 → 45 s), rewards and Suspicion | Yes (week 2 system, week 3 songs and charts) |
| Must | Day / night chapter flow; the palace's look per chapter (moon, banners, blossoms) | Yes (weeks 2–3) |
| Must (decided 12.11) | Offline mode: full offline trees for 7 key scenes and every side character's clue + 142 fallback lines | Yes (week 2 code, week 3 data) |
| Must (new 10.11) | Built-in AI model shipped inside the game (llama-server + Qwen3.5-4B, no key, no internet) | Yes (week 1 connection, week 4 packaging) |
| Must | 7 chapters: 6 palace chapters + Chapter 7 spirit walk (6 embers); 2D Prologue and Epilogue; in-engine chapter endings; `LS_Trap` | Yes (week 3) |
| Must | Conversation UI, exploration HUD, title, pause, chapter cards, Rumor Journal | Yes (weeks 1–3) |
| Must | Placeholder portraits, basic fire VFX, 1 music loop + 1 dance song per night (or one song at rising tempo) | Yes |
| Should (new 11.11) | About 10 ambient NPCs with 60 barks | Yes (week 3) if Phase 12 passes; else 5 NPCs |
| Should | Memories + generated Chapter 7 confession (6 pieces; pre-written fallback always there) | Week 3 if the Phase 12 risk check passes |
| Should | Final portraits, 3 music loops, phoenix-rise and ghost-palace VFX | Week 4 if time |
| Nice | TTS for Zafir, varying epilogue lines, optional external AI endpoint setting | ⚠️ Cut (varying epilogue lines only if memories land early) |

### Cut order if a phase slips more than one day
1. Generated memories and the generated Chapter 7 confession (the pre-written 6-piece confession plays).
2. Ambient NPCs from 10 to 5 (barks for the rest are dropped).
3. Dance songs: reuse one song with a faster tempo each night instead of 6 songs.
4. Eavesdrop zones: keep 1 per chapter.
5. Side characters' clue count: Chapters 4–5 required clues from 3 to 2.
6. Final portraits and custom models (placeholders stay).

---

## PROGRESS TRACKER
> Update this table after every work session. Status: `⬜ NOT STARTED` · `🔄 IN PROGRESS` · `✅ DONE` · `⏸️ BLOCKED`.
> When you resume, start from the first row that is not `✅`.
> **GitHub:** every phase is an issue on the project board: Phase N = issue #(N+2) (Phase 0 = #2 … Phase 22 = #24), labelled by week and owner, with the day-by-day tasks as checkboxes. Close the issue when its gate passes (or put `Closes #N` in the PR). The day-by-day view is `Docs/workplan/index.html`.

| # | Phase | Stage | Owner | Target | Status | Last update | Notes |
| :-- | :-- | :-- | :-- | :-- | :-- | :-- | :-- |
| 0 | Project setup & test level | 1 | Merve (+ Tala, Baha setup) | Tue 10.11 | 🔄 IN PROGRESS | 12.11.2026 | UE 5.8 project, modules, `Content/Ashes`, `L_Test`, `L_Palace`, `GM_Ashes`, MCP + VibeUE done on PC 1 (merged to `main`); open: Functional Testing plugin, Git source control setting, Tuana's, Tala's and Baha's PC setup, task board |
| 1 | Player movement & camera | 1 | Merve | Tue 10.11 | ✅ DONE | 10.11.2026 | Walk 220 / run 450 cm/s, no jump, orbit camera; tested by Merve; PR #26 merged |
| 2 | Interaction system (E: Talk) | 1 | Merve | Wed 11.11 | ✅ DONE | 10.11.2026 | `UInteractionComponent` (200 cm, 0.1 s timer), `AAshesNPC`, `WBP_InteractPrompt`, placeholder `BP_Zafir`; tested by Merve; PR #27 merged |
| 3 | LLM provider (built-in model + fallback) | 1 | Merve | Wed 11.11 | ✅ DONE | 11.11.2026 | Built-in model (llama-server + Qwen3.5-4B, no key); streamed requests; 30 s editor / 6 s game timeout; PR #29 merged. Open: speed test on the stronger PC |
| 4 | Prompt builder & validator + lore and Zafir's card | 1 | Merve (code, texts) + Baha (CSV import) | Fri 13.11 | 🔄 IN PROGRESS | 11.11.2026 | C++ done on `feature/merve-phase4-prompt` (8 tests green, `Ashes.TalkZafir` works); waiting for Tuana's lore bible, Zafir's card and Chapter 1 scene card |
| 5 | Conversation UI + Chapter 1 Zafir scene | 1 | Merve + Baha (code), Tuana (UI style, portraits), Tala (UI sounds) | Sat 14.11 | 🔄 IN PROGRESS | 11.11.2026 | Placeholder `WBP_Conversation` + `UConversationWidget` + minimal `USceneDirectorSubsystem` on `feature/merve-phase5-conversation`; E → 10-turn scene works in PIE; open: Tuana's UI style, fallback lines, portraits |
| 6 | 🏁 Prototype gate | 1 | All | Sun 15.11 | ⬜ NOT STARTED | | user sign-off |
| 7 | Scene Director & relationship values | 2 | Baha | Tue 17.11 | ⬜ NOT STARTED | | |
| 8 | Open palace grey-box & day / night flow | 2 | Merve (level) + Baha (code) | Wed 18.11 | ⬜ NOT STARTED | | |
| 9 | Clues & Rumor Journal | 2 | Baha (code), Tuana (UI art), Merve (placement) | Thu 19.11 | ⬜ NOT STARTED | | |
| 10 | Guards, hiding spots & eavesdropping | 2 | Baha (AI), Merve (routes, spots), Tala (SFX, glow) | Sat 21.11 | ⬜ NOT STARTED | | |
| 11 | Nightly dance minigame | 2 | Baha (code), Tala (song, chart, animations), Tuana (UI art) | Sat 21.11 | ⬜ NOT STARTED | | |
| 12 | ⚠️ Chapter 1 complete + risk check | 2 | All | Sun 22.11 | ⬜ NOT STARTED | | decides should-haves |
| 13 | Chapters 2–6 content | 2 | Merve (levels, data), Baha (chapter logic), Tala (dance songs), Tuana (portraits) | Wed 25.11 | ⬜ NOT STARTED | | |
| 14 | Cutscene system, 2D parts + Chapter 7 spirit walk | 2 | Baha (player, spirit walk), Tala (sequences, VFX), Tuana (2D art), Merve (ghost palace) | Wed 25.11 | ⬜ NOT STARTED | | |
| 15 | HUD, menus, chapter cards | 2 | Tuana (art) + Baha (wiring) | Thu 26.11 | ⬜ NOT STARTED | | |
| 16 | Memories + Chapter 7 confession (should) | 2 | Baha | Fri 27.11 | ⬜ NOT STARTED | | |
| 17 | 🏁 Content gate + playtest 1 | 2 | All (Tala runs the playtest) | Sun 29.11 | ⬜ NOT STARTED | | user sign-off |
| 18 | Art pass (palace kit, portraits, VFX) | 3 | Tuana (art), Tala (VFX), Merve (lighting) | Tue 01.12 | ⬜ NOT STARTED | | |
| 19 | Audio pass | 3 | Tala | Tue 01.12 | ⬜ NOT STARTED | | |
| 20 | Tuning, playtest 2, red-team pass | 3 | All | Thu 03.12 | ⬜ NOT STARTED | | |
| 21 | Credits, disclosure, licenses, prompt log | 3 | Tala (+ Merve prompt log, Baha headers) | Fri 04.12 | ⬜ NOT STARTED | | |
| 22 | 🏁 Packaged Windows build | 3 | Baha | Sat 05.12 | ⬜ NOT STARTED | | final |

---

# STAGE 1: PROTOTYPE (Week 1 · 09.11 – 15.11)

## PHASE 0: Project Setup & Test Level
**Objective:** An Unreal Engine 5.8 C++ project every team member opens, with `L_Test` and `L_Palace` levels that run. Replaces the earlier Unity skeleton (decided 10.11.2026: same repository, Unity kept as the tag `unity-skeleton`).
**Depends on:** none.

**Tasks:**
- ✅ [REPO] Remove `Assets/`, `Packages/`, `ProjectSettings/`; Unreal `.gitignore` (Binaries, Intermediate, Saved, DerivedDataCache, `.vs`, `Secrets/`) and `.gitattributes` (Git LFS, `*.uasset` / `*.umap` lockable).
- ✅ [REPO] Merve: tag the last Unity commit on `main` (`2090454`) as `unity-skeleton`: `git tag unity-skeleton 2090454` then `git push origin unity-skeleton` (or GitHub → Releases → Draft a new release → new tag `unity-skeleton` on that commit).
- ✅ [UE] Merve: create the project from the **Third Person** template, **C++**, named `Ashes`, in the repository folder; Starter Content off; Lumen on (default).
- [UE] Enable plugins: Enhanced Input (default), Gameplay Debugger, Functional Testing (Automation) (MISSING: Functional Testing not enabled yet); ✅ add the C++ modules `HTTP`, `Json`, `JsonUtilities`, `UMG`, `AIModule`, `NavigationSystem`, `GameplayTasks` to `Ashes.Build.cs` (derived).
- ✅ [UE] Create `Content/Ashes/` (folders from `design.md` section 10), `L_Test` (floor, walls, a ramp, night sky, Nav Mesh Bounds Volume) and an empty `L_Palace`; set `L_Test` as the editor startup map and `GM_Ashes` as the default game mode.
- [UE] Project Settings → Editor → Source Control: Git with LFS (MISSING: each PC sets this in the editor); ✅ "One File Per Actor" is on for `L_Palace` (World Settings → Use External Actors).
- [UE] Every team member clones with Git LFS, build in Visual Studio 2022 (or Rider), and open the project on the same engine version; `git lfs lock` before editing any `.uasset` / `.umap`.
- [TOOLS] Install MCP + VibeUE (course setup) so the AI agent can reach the editor; confirm every PC has it. ✅ PC 1 (Merve, 10.11.2026); MISSING: Tuana's, Tala's and Baha's PCs.
- [UE] Tala and Baha (joined 12.11.2026): clone with Git LFS, `git submodule update --init`, build in Visual Studio, open on UE 5.8, run `Tools/GetLocalLLM.ps1`; Baha reads `CLAUDE.md`, `SKILL.md` and `technical_design.md` before taking over the code.
- ✅ [WRITE] Task board: GitHub Projects board started (Merve keeps it).

**Deliverable:** `main` opens the Unreal project and `L_Test` plays with no errors on both PCs.
**Verification Gate:** Open the project on every team member's PC, press Play In Editor (PIE) in `L_Test`, the Output Log shows no errors and the C++ build succeeds. ✋ Confirm with the user before the next phase.

---

## PHASE 1: Player Movement & Camera
**Objective:** Sahra walks and runs in third person with an orbit camera.
**Depends on:** Phase 0.

**Tasks:**
- ✅ [BP] Enhanced Input: `IA_Move`, `IA_Look`, `IA_Run`, `IA_Interact`, `IA_OpenJournal`, `IA_Pause` in `IMC_Exploration`; `IMC_Conversation`, `IMC_Cutscene`, `IMC_Dance` (keys in `technical_design.md` section 5); remove the template's jump.
- ✅ [C++] `AAshesCharacter` (from the template character): walk 220 cm/s, run 450 cm/s (Shift), `bIsRunning`, `SetGameplayInputEnabled`; `AAshesPlayerController` adds the mapping contexts.
- ✅ [BP] `BP_Sahra` with the template mannequin or a licensed base model + `ABP_Sahra` (idle, walk, run).
- ✅ [BP] Spring Arm + Camera orbit (arm 350 cm, pitch −60° to +25°; recorded in `technical_design.md` section 3).

**Deliverable:** `BP_Sahra` moving in `L_Test`.
**Verification Gate:** In PIE, W A S D walks at about 2.2 m/s, Shift runs at about 4.5 m/s, mouse orbits the camera, no jump. ✋ Confirm with the user.

---

## PHASE 2: Interaction System (E: Talk)
**Objective:** Pressing E near a highlighted character starts a conversation event.
**Depends on:** Phase 1.

**Tasks:**
- ✅ [C++] `UInteractionComponent` (sphere trace on a 0.1 s timer, not Tick) + `IAshesInteractable` interface; delegate `OnInteract` (radius 200 cm).
- ✅ [UI] "E: Talk" prompt as a Widget Component (`WBP_InteractPrompt`, placeholder style: Old gold frame, Night plum panel, Moon cream text).
- ✅ [BP] Placeholder `BP_Zafir` (mannequin, child of `AAshesNPC`) in `L_Test`.

**Deliverable:** E on the placeholder Zafir prints "Start scene: Zafir" to the Output Log and disables movement.
**Verification Gate:** Walk up to Zafir, prompt appears, E starts the event, walking away hides the prompt. ✋ Confirm with the user.

---

## PHASE 3: LLM Provider (Built-in Model + Fallback)
**Objective:** One request to the built-in AI model is answered in the Output Log; when the model is off or too slow, a fallback line answers instead (decided 10.11.2026: the model ships inside the game, no API key).
**Depends on:** Phase 0 (can run in parallel with Phases 1–2).

**Tasks:**
- ✅ [C++] `ULLMProvider` (abstract), `UOpenAICompatibleLLMProvider` (`FHttpModule`, `SetTimeout(6)`, async callback on the game thread; built-in server or optional external endpoint), `UOfflineLLMProvider` (returns a canned JSON).
- ✅ [C++] `ULocalModelSubsystem` starts / stops the bundled llama-server (hidden, free port, dies with the game), `UAshesAISettings`, AI mode Online / Offline in `UAshesGameInstance` with automatic fallback; console commands `Ashes.TestLLM`, `Ashes.LLM.Start / Stop / Status`.
- ✅ [TOOLS] `Tools/GetLocalLLM.ps1` downloads the pinned llama.cpp b12138 (Vulkan) and Qwen3.5-4B Q4_K_M with SHA-256 checks (git-ignored `ThirdParty/LLM`, or `ASHES_LLM_DIR`).
- ✅ Provider and model chosen (10.11.2026): built-in model, free, no key (GDD 8.4).
- [TEST] Speed on the stronger PC (target: reply under 6 s); this laptop (RTX 3050 Ti 4 GB) gets 7–12 tokens/s = 13–30 s per reply.

**Deliverable:** `Ashes.TestLLM` sends Zafir's test prompt and prints the JSON reply; with the model stopped it prints a fallback reply.
**Verification Gate:** Model running → valid JSON reply (under 6 s on the target PC); `Ashes.LLM.Stop` → fallback reply, no freeze; no internet needed. ✋ Confirm with the user.

---

## PHASE 4: Prompt Builder, Validator, Lore Bible and Zafir's Card
**Objective:** Requests are built from the five parts and every reply is validated and clamped.
**Depends on:** Phase 3.

**Tasks:**
- [WRITE] Merve (Tuana reviews): lore bible (~600 words, hand-written); Zafir's character card (add the line on when he turns cold or angry, from the 03.11 test); Chapter 1 scene card with beats and turn limit 10 (12.11: now 11; all cards drafted in the Master Lore doc, waiting for the team's review and data entry).
- ✅ [C++] Row structs `FCharacterCardRow`, `FSceneCardRow` (`Source/Ashes/Data/AshesTypes.h`) and DataTables `DT_Characters`, `DT_Scenes`, Data Asset `DA_LoreBible` (class `UAshesLoreBible`) in `Content/Ashes/Data`, filled with placeholder text from the GDD until Tuana's hand-written versions; `UPromptBuilder` (card, lore, scene, state words, last 8 turns + new line).
- ✅ [C++] `UResponseValidator`: JSON parse (also inside code fences), 1 silent retry, emotion whitelist with fallback, 80-word limit, `off_topic`, spoiler keywords (regenerate once), safety word list (placeholder), delta clamp, beat / clue ids only.
- ✅ [C++] Automation tests: valid JSON, broken JSON, unknown emotion, +50 Trust clamped to +8, over-long reply, plus free-sentence beats, spoiler / safety and prompt order / secrets / history (8 tests, all green 11.11.2026). Console `Ashes.TalkZafir <line>` talks through the real prompt and validator.

**Deliverable:** Output Log conversation with Zafir through the real prompt; all Automation tests green.
**Verification Gate:** Session Frontend → Automation all green; the 5 lines from GDD 12.4 pass again through the API; reply times measured. ✋ Confirm with the user.

---

## PHASE 5: Conversation UI + Chapter 1 Zafir Scene
**Objective:** Talking to Zafir in `L_Test` uses the real conversation screen, online and offline.
**Depends on:** Phases 2 and 4.

**Tasks:**
- [UI] Tuana (style): `WBP_Conversation` per `design.md` (dialogue panel, small portraits, hint line, large portrait, 3 reply buttons, 200-character input, Send, turns left) with placeholder portraits. ✅ Placeholder version built 11.11.2026 (design.md colours; widget names fixed by `UConversationWidget`); Tuana restyles it (keep the widget names).
- ✅ [C++] `WBP_Conversation` logic in `UConversationWidget` (C++ instead of Blueprint): portrait swap (emotion → `T_Portrait_<Name>_<Emotion>` if it exists, else a placeholder colour), typing indicator ≥ 0.4 s, input cut at 200 (MISSING: warning text), 1 / 2 / 3 picks (typed into the empty box or clicked), Send, Leave.
- [WRITE] Merve: Chapter 1 fallback lines for Zafir (11.11: fallback lines only. **Changed 12.11.2026:** full offline trees are back in scope and written in the Master Lore doc together with 142 fallback lines; the tree walker is built in Phase 7).
- ✅ [C++] Turn counter and simple end at 10 turns in `USceneDirectorSubsystem` (minimal; full Director comes in Phase 7); E on an `AAshesNPC` with a card + scene row opens the screen; prompt warm-up when the scene opens; console `Ashes.Scene.Say / Pick / End`.

**Deliverable:** A playable Zafir conversation from E to the 10th turn.
**Verification Gate:** Play online: type, pick suggestions, portrait changes, fallback on timeout. With the model stopped (`Ashes.LLM.Stop`): every turn answers with a fallback line and the scene can still be left. (Offline trees skipped, see above.) ✋ Confirm with the user.

---

## PHASE 6: 🏁 Prototype Gate (Sun 15.11)
**Objective:** Confirm the core loop works before content.
**Depends on:** Phases 0–5.

**Tasks:**
- All: play `L_Test` end to end, online and offline.
- ⚠️ Early warning check (GDD 13.5): average reply over 4 s, or under 90% valid JSON? If yes → offline mode becomes the default and the side characters and the Chapter 7 confession use their pre-written trees and lines.
- Update this tracker, `PROGRESS.md` and the task board.

**Deliverable:** Signed-off prototype; risk decision recorded in Notes.
**Verification Gate:** User plays the prototype and confirms. ✋

---

# STAGE 2: FULL GAME (Weeks 2–3 · 16.11 – 29.11)

> Week 2 builds every system and proves it on Chapter 1. Week 3 adds content only.

## PHASE 7: Scene Director & Relationship Values
**Objective:** Every scene ends by goal or by turn limit, and values change within their limits.
**Depends on:** Phase 6.

**Tasks:**
- [C++] `USceneDirectorSubsystem` state machine: Intro → FreeTalk → Nudge (2 turns left or 3 stalled) → Resolve / ForcedClose → NextChapter; Caught at Suspicion 100.
- [C++] Baha: `UNPCBrainComponent` with Trust 20 / Affection 10 (±8), Favor 30 (±6), Suspicion 10 (+10 / −5); unlocks Trust 30, 50, 60 (romance route), Affection 40, 60, Favor 60, Suspicion 100; value-to-word bands for the prompt; route (romance / comrade).
- [C++] Baha: turn limits per scene row (Zafir 11, Vashir 7, others 5); forced beats for `Ch6_VashirTrap`; `ClosingRomance` field in `FSceneCardRow` (proposed).
- [C++] Baha: offline mode: `FOfflineNodeRow` / `DT_OfflineTrees` tree walker in `UOfflineLLMProvider`, `FFallbackLineRow` / `DT_FallbackLines` with the pick order (scene + beat + emotion → clue → scene → any; never the same line twice in a row).
- [WRITE] Merve: enter the Master Lore cards, scene rows, hint lines, nudge notes, 142 fallback lines and the offline trees into the DataTables (CSV import).
- [C++] Automation tests for transitions, thresholds and offline trees (every key-scene path reaches its required beats).

**Deliverable:** The Zafir scene resolves with full or weaker outcome, online and offline; hints instead of numbers.
**Verification Gate:** Reach the goal → Resolve; stall → Nudge; hit the limit → Forced close; with the model stopped the Chapter 1 tree reaches `uneasy_truce`; tests green. ✋ Confirm with the user.

---

## PHASE 8: Open Palace Grey-Box & Day / Night Flow
**Objective:** The whole palace is explorable and a chapter runs Day → Night → Key scene → Next day.
**Depends on:** Phase 7.

**Tasks:**
- [UE] Merve: grey-box the Garden of Dusk hub (running fountain) + 5 rooms at about 6000 × 3000 cm in `L_Palace` (see `level-design.md` sections 3 and 13), the practice room, the servants' passage behind the King's Chamber curtain and the locked City Gate; stairs to the Rooftop; Player Start and checkpoint in the Dancers' Quarters; King's Chamber as a `BP_RestrictedVolume`; Nav Mesh Bounds Volume (check with `P`).
- [UE] Merve: mark hiding spots (curtains, lattice screens, alcoves), eavesdrop stops and chapter spawn points (`Ch<N>.Day` / `Ch<N>.Night`) in the grey-box, even before they work.
- [C++] Baha: `UDayNightSubsystem` and `AAshesGameMode`: chapters 1–7, phases of the day (Day, Night, KeyScene, Ending), checkpoint save at chapter start (`UAshesSaveGame`, USaveGame), `BP_NPCSpawnPoint` per chapter and per phase, chapter-tagged dressing (moon, lanterns, banners, blossoms); Continue loads the save.
- [C++] Baha: nightfall trigger: when the required clues are found or the player chooses "Go to perform" (*proposed*), a bell rings and the goal line switches to "Perform in the Throne Hall"; lighting switches to night (Directional Light, Sky Atmosphere and Post Process Volume; Merve sets the lights).

**Deliverable:** Walk the whole palace; Chapter 1 phases advance with placeholder triggers.
**Verification Gate:** Every room except the King's Chamber and the City Gate is reachable; Day → Night → Key scene → in-engine ending → Chapter 2 card works; quit and Continue returns to the checkpoint. ✋ Confirm with the user.

---

## PHASE 9: Clues & Rumor Journal
**Objective:** Talking gives clues, clues open topics and the key scene.
**Depends on:** Phase 8.

**Tasks:**
- [C++] Baha: `FClueRow` / `DT_Clues` and `UClueJournalSubsystem`: accept a clue only if the NPC holds it this chapter; ignore duplicates; "requires clue" links (e.g. Aisha in Chapter 3); gate for the key scene (3 of 6; none in Chapters 6–7); clue → extra suggested reply and prompt hint for later NPCs.
- [C++] Baha: `clues` in the LLM JSON is already checked by the validator (Phase 4); offline nodes give clues directly.
- [UI] Tuana (art) + Baha (logic): `WBP_RumorJournal` (Tab): chapter question, found clues with speaker, "?" slots, conversation log, memories; "New clue" toast.
- [WRITE] Merve: enter the 36 clues from the Master Lore doc (ids `ch<N>_<who>_<what>`, journal text, topic text, prompt hint); Chapter 1 cards for Nura, Aisha, Omar, Tariq, Idris and Ilyas.
- [C++] Automation tests: rejected unknown clue, duplicate, gate opens at the required count.

**Deliverable:** Chapter 1 day: gather 3 clues, Zafir opens up.
**Verification Gate:** Zafir refuses with 2 clues and talks with 3; journal shows the right entries online and offline. ✋ Confirm with the user.

---

## PHASE 10: Guards, Hiding Spots & Eavesdropping
**Objective:** Patrols make moving around tense, and sneaking earns information.
**Depends on:** Phase 8 (built first in `L_Test`, then placed in `L_Palace`).

**Tasks:**
- [C++] / [BP] `AGuardAIController` + `BT_Guard` / `BB_Guard`: Move To fixed Target Points with Wait stops (loop 20–40 s, stops 3–5 s, *proposed*); Patrol → Notice → Patrol; AI Perception Sight 70° (35° half-angle) / 900 cm with line of sight; no chasing.
- [C++] `USuspicionComponent`: +12/s running in view, +20/s restricted room in view, −5/s out of view; dance and Vashir add to the same meter; Caught at 100 → restart at checkpoint.
- [C++] `AHidingSpot` (E to enter / leave; Sahra is not sensed by guards) and `AEavesdropZone` (hear the whole ~10 s exchange unseen → authored clue; seen → exchange stops).
- [UI] Eye icon in `WBP_HUD`, hiding-spot glow (custom depth outline or emissive material) and prompt.
- [BP] `BP_Guard`, `BP_HidingSpot`, `BP_EavesdropZone`; Chapter 1: the garden patrol guard and the Guard Post guard who meets him at the lattice screen stop (eavesdrop `ch1_guards_blade`). Merve places routes and spots; Tala adds footsteps and the hiding-spot glow.

**Deliverable:** Chapter 1 day with guards, one hiding route and one eavesdrop clue.
**Verification Gate:** Run past a guard → Suspicion rises → restart at 100; hide → the guard walks past; eavesdrop → clue added. ✋ Confirm with the user.

---

## PHASE 11: Nightly Dance Minigame
**Objective:** The night performance is a short, readable rhythm game that feeds Favor, Suspicion and clues.
**Depends on:** Phase 6 (built in `L_Test` in parallel with Phases 7–10; Baha writes the code, Tala makes the song, chart and dance animations, Tuana draws `WBP_Dance`).

**Tasks:**
- [C++] `UDanceMinigameComponent`: note chart from `DT_DanceCharts` (row `Night1`...), timing from a Quartz clock synced to the song, `IMC_Dance` active, 4 lanes (← ↑ ↓ →, *proposed*), 0.25 s windows, accuracy; > 80% → +10 Favor + reward; 40–80% → nothing; < 40% → +10 Suspicion; pause support.
- [UI] `WBP_Dance`: lanes, hit feedback, end result with reward or warning.
- [BP] Sahra's dance loop and 4 move animations (montages in `ABP_Sahra`); Throne Hall audience placement.
- [AUDIO] Tala: Night 1 song (royalty-free) as a MetaSound; tempo as a parameter; chart row `Night1` (30 s).
- [WRITE] Night 1 reward clue `ch1_nobles_doors` (from `DT_Barks` dance gossip).

**Deliverable:** Night 1 dance in the Throne Hall.
**Verification Gate:** Play well → Favor hint + reward clue; play badly → Suspicion; notes stay in sync with the music. ✋ Confirm with the user.

---

## PHASE 12: ⚠️ Chapter 1 Complete + Risk Check (Sun 22.11)
**Objective:** Chapter 1 is the full loop, and real numbers decide what stays.
**Depends on:** Phases 9, 10 and 11.

**Tasks:**
- Play Chapter 1 end to end: Day (clues, guards, eavesdrop, Do 1) → Night (dance) → Zafir key scene (Do 2: the knife) → in-engine ending.
- [C++] Test harness (commandlet or editor utility) and first red-team run (50 lines per character: off-topic, jailbreak, spoiler, rude); LLM judge flags spoilers and broken voice.
- Measure tokens per playthrough and average reply time.
- Decide: memories + generated Chapter 7 confession in or out; 10 or 5 ambient NPCs.
- ⚠️ Early warnings (GDD 13.5): average reply over 4 s on the stronger PC or under 90% valid JSON → smaller model, shorter replies or word-by-word streaming; side characters fall back to their offline nodes more often. Chapter 2–6 data not entered → apply the cut order. Palace still grey-box → custom models cut.

**Deliverable:** Playable Chapter 1, red-team report, decisions in Notes and `PROGRESS.md`.
**Verification Gate:** User plays Chapter 1 and confirms the loop is fun; decisions confirmed. ✋

---

## PHASE 13: Chapters 2–6 Content
> Rewritten 12.11.2026 for the 7-chapter story. Day-by-day tasks are in `Docs/workplan/index.html`; who is where per chapter is in `level-design.md` sections 4–9.
**Objective:** Chapters 2–6 each have their day (main quest, 2 "do" objectives, 6 clues), night and key scene, and end in-engine.
**Depends on:** Phase 12.

**Tasks:**
- [WRITE] Merve: enter Vashir's card and the minor characters' cards; scene rows `Ch2_ZafirRehearsal`, `Ch3_VashirAudience`, `Ch3_ZafirFountain`, `Ch4_ZafirKnife`, `Ch5_ZafirRooftop`, `Ch6_VashirTrap`; clues, barks and offline trees for Chapters 2–6 (all in the Master Lore doc).
- [UE] Merve: NPC spawn points per chapter and phase; guard routes per chapter (extra guard at the Dancers' Quarters in Chapter 2, King's Chamber door guard in Chapter 3, City Gate guard at night in Chapter 4, doubled guard in Chapter 5, emptied corridors in Chapter 6); eavesdrop stops; ambient NPC spots; chapter dressing (moon, fewer lanterns in Chapter 3, banners from Chapter 4, plum blossoms in Chapter 5).
- [C++] / [BP] Baha, Chapter 2: the search at the afternoon bell; 3 knife hiding places (`BP_KnifeHidingPlace`, Do 1); Zafir lifts the knife; the closing line depends on the hiding place; Guard Post eavesdrop (Do 2).
- [C++] / [BP] Baha, Chapter 3: King's Chamber door scouting with Zafir's scripted save (Do 1); Ilyas leaves at the dusk bell (Do 2); Vashir's audience (7 turns, Favor 60 invitation, Suspicion from conversation; the late invitation through Nura below 60); private dance in the King's Chamber; Zafir at the fountain (jealousy beat).
- [C++] / [BP] Baha, Chapter 4: the anklet replaces the cord on Sahra's model (morning, the king's servant); tail Zafir to the City Gate at the midnight bell (`UFollowerComponent`, Do 1); find Omar (Do 2); key scene in the practice room with the knife at his throat (Trust 50).
- [C++] / [BP] Baha, Chapter 5: the king's servant follows Sahra (shake off, corner, fixed 3-line exchange, Do 1); nobles' eavesdrop in the Throne Hall (Do 2); the duet; rooftop key scene (Affection 60 confession); the mended cord replaces the anklet.
- [C++] / [BP] Baha, Chapter 6: plan checks (servants' passage, City Gate, guard change, Do 1); no key-scene gate; solo dance in the King's Chamber (fixed outcome, Do 2); the trap (`Ch6_VashirTrap`, 7 turns, forced beats, lines saved as Zafir's memories); `LS_Trap` (Tala) for the fall.
- [C++] / [BP] Baha: `UBarkComponent` and `BP_AmbientNPC` (about 10, `DT_Barks`).
- [AUDIO] Tala: dance songs and charts for nights 2–6 (33, 36, 39, 42, 45 s), or one song at rising tempo (cut order 3); bells (nightfall, afternoon, dusk, midnight).
- [ART] Tuana: portraits for Vashir, Nura, Idris and the 4 minor characters (placeholders first); the anklet and the mended cord.

**Deliverable:** Chapters 1–6 in sequence.
**Verification Gate:** Play Chapters 1–6 online and offline; get caught once on purpose to see the restart; Chapter 6 cannot be lost. ✋ Confirm with the user.

---

## PHASE 14: Cutscene System, 2D Parts + Chapter 7 Spirit Walk
**Objective:** The story plays from Prologue to Epilogue, including the Chapter 7 spirit walk.
**Depends on:** Phase 12 (in parallel with Phase 13).

**Tasks:**
- [BP] Baha: visual-novel cutscene player `WBP_Cutscene` (background, speaker, text, Space to advance via `IMC_Cutscene`) reading `DT_Cutscenes`, for the Prologue and the Epilogue (romance and comrade variants); Tala makes `LS_Prologue` and `LS_Epilogue` for camera moves where needed.
- [C++] Baha: Chapter 7 `USpiritWalkSubsystem` (*proposed name*): hide guards and NPCs, ghost post process, 6 `BP_SoulEmber` (fountain, practice room, Throne Hall, City Gate, Rooftop Terrace, King's Chamber; any order), replay the stored player line per chapter, then one confession piece (generated, else pre-written); red → gold after the 6th ember; then the Epilogue.
- [UE] Merve: the ghost palace (Chapter 7 dressing, ember placement, the red glow of the Crimson Zenith over the walls).
- [VFX] Tala: ghost post process, ash rising, ember VFX, the red → gold fire.
- [WRITE] Merve: enter the Prologue narration (8 lines), Epilogue lines (both routes), 6 ember captions and the 6-piece fallback confession (Master Lore doc).
- [ART] Tuana: 2D backgrounds (Burning Tamra, the palace gate, the Crimson Zenith), AI-assisted concepts finished by hand.

**Deliverable:** Full story path.
**Verification Gate:** Play New Game to Credits offline; Chapter 7 plays all 6 pieces of the pre-written confession with the model stopped. ✋ Confirm with the user.

---

## PHASE 15: HUD, Menus, Chapter Cards
**Objective:** Every screen in `design.md` section 7 exists.
**Depends on:** Phases 13 and 14 (UI after the gameplay it shows).

**Tasks:**
- [UI] `L_Title` + `WBP_Title` (Niagara embers over the dying sun), Settings (text speed, volume, AI mode + status line), Pause (Resume, Settings, Restart Chapter, Quit), Chapter card with the chapter question (3 s), Credits.
- [UI] Exploration HUD: goal line with the 2 "do" objectives, day / night tag, eye icon, "E: Talk", hide prompt, "New clue" toast, bark bubble, Chapter 7 ember counter.
- [C++] / [BP] Screen stack in `AAshesPlayerController` (or a HUD class), widgets bound to delegates, not Tick.

**Deliverable:** All screens reachable through the flow.
**Verification Gate:** Walk every arrow in the `design.md` flow chart. ✋ Confirm with the user.

---

## PHASE 16: Memories + Chapter 7 Confession (should have)
**Objective:** Characters remember, and Zafir's Chapter 7 confession quotes the player's own moments.
**Depends on:** Phase 12 decision = keep; Phase 14 (spirit walk).

**Tasks:**
- [C++] Baha: summary call after each key scene (1–2 memories, max 6 per character); memories in the prompt and the Rumor Journal; the Chapter 6 lines saved as Zafir's memories.
- [C++] Baha: generated confession, one piece (1–3 sentences) per soul ember, quoting memories; pre-written 6-piece fallback.

**Deliverable:** Memories visible in the journal; Chapter 7 lines change between runs.
**Verification Gate:** Two different playthroughs give different confession pieces; offline still plays the pre-written confession. ✋ Confirm with the user.

---

## PHASE 17: 🏁 Content Gate + Playtest 1 (Sun 29.11)
**Objective:** Content complete and tested by outsiders.
**Depends on:** Phases 13–15 (16 if kept).

**Tasks:**
- Playtest with 5 outside players (Tala leads); watch for: lost players (which clue next? where is the next character?), patrols that feel unfair, the tail and being followed, dance difficulty, conversation pacing, broken replies, how Chapters 6 and 7 land.
- Feature freeze from here: no new features in Stage 3.

**Deliverable:** Playtest notes and a fix list.
**Verification Gate:** Full playthrough online and offline with no blocker. ✋ User sign-off.

---

# STAGE 3: POLISH (Week 4 · 30.11 – 06.12)

## PHASE 18: Art Pass
**Objective:** The palace and portraits look like the GDD.
**Depends on:** Phase 17.

**Tasks:**
- [ART] Tuana: licensed modular palace kit (Fab) assembled (Merve lights it with Lumen: moonlight, lanterns); license check for educational and portfolio use.
- [ART] Tuana: final portraits if time (4–6 emotions each, 8 LLM characters), otherwise placeholders stay.
- [VFX] Tala: Niagara fire and ember VFX; ghost-palace look polish; phoenix-rise VFX for the epilogue (most important effect).
- [UE] Merve: lighting pass per chapter (moon, lanterns, the Rose red King's Chamber).

**Deliverable:** Art-complete build.
**Verification Gate:** Play the palace; frame rate acceptable on the team's PCs (`stat fps`, `stat unit`). ✋ Confirm with the user.

## PHASE 19: Audio Pass
**Objective:** Music and SFX in place.
**Depends on:** Phase 17.

**Tasks:**
- [AUDIO] Tala: palace loop (must); romance, ghost-palace and finale loops (should); loop seams checked.
- [AUDIO] Footsteps on stone, fire, cloth, blades, crowd (CC0 / CC-BY); licenses in `Docs/Licenses.md`.
- [AUDIO] Volume setting wired to a Sound Class Mix (Master, Music, SFX, UI sound classes).

**Deliverable:** Sound-complete build.
**Verification Gate:** Listen through every scene; volume slider works. ✋ Confirm with the user.

## PHASE 20: Tuning, Playtest 2, Red-Team Pass
**Objective:** The game feels right and leaks nothing.
**Depends on:** Phases 18–19.

**Tasks:**
- Tune values, turn limits (Zafir 11, Vashir 7, others 5), hints, clue counts, patrol timing, tail distance and dance difficulty from playtest 1.
- Second playtest.
- Red-team pass: 0 spoiler leaks, at least 95% valid JSON.

**Deliverable:** Tuned build and red-team report.
**Verification Gate:** Targets met. ✋ Confirm with the user.

## PHASE 21: Credits, Disclosure, Licenses, Prompt Log
**Objective:** Course paperwork complete.
**Depends on:** Phase 20.

**Tasks:**
- Tala: credits screen: team (Merve, Tuana, Tala, Baha), licensed assets, AI-assisted assets and tools; course: Advanced Game Programming.
- Tala: `Docs/Licenses.md` complete; Merve: prompt log sheet final (`Docs/PromptLog.md` links it); Baha: AI-assisted headers on every C++ file and a header Comment node in AI-assisted Blueprints.

**Deliverable:** Paperwork complete.
**Verification Gate:** Reviewer checklist from the pull request template passes. ✋ Confirm with the user.

## PHASE 22: 🏁 Packaged Windows Build (Sat 05.12)
**Objective:** A Windows build that always runs.
**Depends on:** Phase 21.

**Tasks:**
- Baha: Platforms → Windows → Package Project (Shipping) to `Builds/` (git-ignored); stage `ThirdParty/LLM` (llama-server + model) as non-UFS files into the build; no API key anywhere; check the model starts and the fallback works if it cannot.
- Test on a PC without Unreal Engine installed and without internet.

**Deliverable:** Final Windows build, tagged release on `main`.
**Verification Gate:** Clean-machine run (no internet) from Title to Credits with the built-in model, including all 6 nights and the Chapter 7 spirit walk; once more with the model folder removed (offline trees and fallback lines). ✋ Final sign-off.

---

# APPENDIX: State-Machine Rules
1. One phase at a time per owner. Finish and verify before starting the next.
2. Commit with Git after every phase that passes its gate (feature branch → pull request → review → merge when `L_Palace` plays). Lock `.uasset` / `.umap` files with `git lfs lock` before editing and unlock after merge.
3. If a phase is blocked, mark it `⏸️ BLOCKED`, write why in Notes, and ask the user.
4. Never change finished phases without writing the reason in Notes.
5. If a phase misses its target date by more than one day, apply the cut list in the Scope table before starting the next phase.

---

## Section Status
| Section | Status |
| :-- | :-- |
| Macro-stage map and goals | FILLED |
| Scope for 4 weeks | FILLED (12.11: 7 chapters, offline trees, side characters as LLM) |
| Progress tracker | FILLED |
| Stage 1 phases 0–6 | FILLED |
| Stage 2 phases 7–17 | FILLED (12.11: Phases 13, 14 and 16 rewritten for 7 chapters) |
| Stage 3 phases 18–22 | FILLED |
| Appendix | FILLED |
