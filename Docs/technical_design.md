# Ashes for a Goddess · Technical Design Document (technical_design.md)

> **Canonical design source:** `gdd.md`. This document turns the GDD into a concrete Blueprint / C++ architecture. **Where this document and the GDD disagree, the GDD wins** and this document must be corrected.
> **Engine target:** Unreal Engine 5.8 (C++ + Blueprints), Windows. Changed from Unity 6.3 HDRP on 10.11.2026 with the teacher's approval.
> **Companion documents:** `level-design.md` (where things are) and `production_pipeline.md` (build order).

**Status legend:** `✅ Built` · `🟡 Partial` · `🔲 Not started` · `♻️ Built but must change`

**Source legend:** class names and their jobs come from GDD Figure 6 (Main scripts), translated to Unreal. Items marked **(derived)** are architecture details this document adds to make the GDD buildable; they are not in the GDD and can be changed freely.

**Gameplay (10.11.2026):** each chapter is a day (gather clues into the Rumor Journal while avoiding guards, with hiding spots and eavesdropping) and a night (dance minigame to keep Sahra's cover), then a key conversation and a cutscene. Clues, guards and the dance are must-haves. See `gdd.md` 1 Mechanics 5–8.

**Project state (10.11.2026):** no Unreal project exists yet. The earlier Unity 6.3 skeleton was removed from the repository on 10.11.2026 and kept as the Git tag `unity-skeleton`; the UE 5.8 project is created in Phase 0 (`production_pipeline.md`).

---

## 0. Meta & Conventions

### 0.1 How to read this document
Every system section uses the same structure: **GDD basis → Architecture → Class spec → Current state → Required work.**

### 0.2 Naming Conventions
- **C++ classes:** `A` (Actor), `U` (UObject / Component / Subsystem), `F` (struct), `E` (enum), `I` (interface). PascalCase, one class per file pair (`.h` / `.cpp`).
- **Blueprints:** `BP_` (actor), `WBP_` (widget), `GM_` (game mode), `PC_` (player controller), `PS_` (player state), `GI_` (game instance), `AC_` (actor component), `BI_` (interface), `ABP_` (animation Blueprint) **(derived)**, `BT_` / `BB_` (behavior tree / blackboard) **(derived)**.
- **Assets:** `SM_`, `SK_`, `M_`, `MI_`, `T_`, `NS_` (Niagara), `DT_` (DataTable), `DA_` (Data Asset) **(derived)**, `L_` (level), `LS_` (Level Sequence) **(derived)**, `IA_` / `IMC_` (Enhanced Input), `S_` / `MS_` (Sound Wave / MetaSound) **(derived)**.
- **Project prefix / module:** `Ashes` (C++ module `Ashes`, classes such as `AAshesGameMode`, `UAshesGameInstance`).
- **AI-assisted header:** every AI-assisted C++ file starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.`; AI-assisted Blueprints get the same text in a Comment node at the top of the Event Graph.

### 0.3 Tools & workflow
- **Blueprint or C++:** C++ for the testable core: LLM HTTP client, prompt builder, response validator, value clamping, Scene Director state machine, clue journal, data structs (all covered by Automation tests). Blueprints for gameplay glue, level scripting, UI (UMG), animation, guard behavior trees and the dance presentation. Blueprints derive from the C++ classes.
- **AI agent:** Claude Code (or Codex / Antigravity) connected to the editor through MCP + VibeUE, as the course template assumes; GitHub Copilot in the IDE for C++. The agent never writes `.uasset` / `.umap` as text.
- **Version control:** Git + Git LFS. `.uasset` and `.umap` are binary and cannot be merged: lock them with `git lfs lock` before editing, and only one person edits a given asset or level at a time. `L_Palace` uses **One File Per Actor** (Use External Actors) so both members can change different actors in the same level **(derived)**. `main` always opens and runs; feature branches `feature/<name>-<task>`, pull request, review by the other member, merge only when `L_Palace` plays. Never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `.vs/`.

---

## 1. Project Architecture

### 1.1 Folder layout
```
Ashes.uproject
Source/Ashes/
  Core/        AshesGameMode, AshesGameInstance, AshesSaveGame, DayNightSubsystem
  Player/      AshesCharacter, AshesPlayerController, InteractionComponent
  Dialogue/    SceneDirectorSubsystem, NPCBrainComponent, PromptBuilder, ResponseValidator,
               LLMProvider, CloudLLMProvider, OfflineLLMProvider
  Gameplay/    ClueJournalSubsystem, SuspicionComponent, HidingSpot, EavesdropZone, DanceMinigameComponent
  AI/          GuardAIController
  Data/        AshesTypes.h (structs and enums for DataTables)
  Tests/       Automation specs (validator, clamps, Scene Director, clue gate, dance grading)
Content/Ashes/
  Core/        GM_Ashes, GI_Ashes, PC_Ashes
  Player/      BP_Sahra, ABP_Sahra
  Characters/  BP_Zafir, BP_Vashir, BP_Nura, BP_Idris, BP_CourtNPC, BP_Guard
  Gameplay/    BP_HidingSpot, BP_EavesdropZone, BP_RestrictedVolume, BP_NPCSpawnPoint
  AI/          BT_Guard, BB_Guard
  UI/          WBP_Title, WBP_Settings, WBP_HUD, WBP_Conversation, WBP_RumorJournal, WBP_Dance,
               WBP_ChapterCard, WBP_Pause, WBP_Cutscene, WBP_Credits
  Data/        DT_Characters, DT_Scenes, DT_Clues, DT_DanceCharts, DT_FallbackLines, DT_OfflineTrees,
               DT_Cutscenes, DA_LoreBible, DA_ExplorationSettings, DA_ConversationSettings
  Input/       IA_*, IMC_Exploration, IMC_Conversation, IMC_Cutscene, IMC_Dance
  Maps/        L_Test, L_Title, L_Palace
  Cinematics/  LS_Prologue, LS_Trap, LS_Pyre, LS_Epilogue
  Art/         Meshes, Materials, Textures, Portraits, VFX (Niagara)
  Audio/       Music, SFX, MetaSounds
Config/        (committed; never holds the API key)
Docs/
Secrets/       (git-ignored) API key file, if not using the environment variable
```

### 1.2 Core classes
Three layers (GDD Figure 6). Gameplay sends the chapter to Conversation AI; Conversation AI sends what to show to Presentation. Game code reaches the LLM only through `ULLMProvider`.

| Class | Parent | Responsibility |
| :-- | :-- | :-- |
| `AAshesGameMode` → `GM_Ashes` | AGameModeBase | Rules: win (end of epilogue), lose (Suspicion 100 → restart chapter at checkpoint) |
| `UAshesGameInstance` → `GI_Ashes` | UGameInstance | Current chapter, AI mode (Online / Offline), relationship values and memories across level loads |
| `AAshesPlayerController` → `PC_Ashes` | APlayerController | Enhanced Input contexts, UI focus, pause |
| `AAshesCharacter` → `BP_Sahra` | ACharacter | Walk / run, hide state, dance animations |
| `UInteractionComponent` → `AC_Interaction` | UActorComponent | Finds the highlighted character or object; E starts a conversation or enters a hiding spot |
| `USceneDirectorSubsystem` | UWorldSubsystem | Conversation FSM: intro, free talk, nudge, resolve, forced close, caught; accepts beats and clues |
| `UNPCBrainComponent` | UActorComponent (one per character) | Character card row, relationship values, memories |
| `UPromptBuilder` | UBlueprintFunctionLibrary | Assembles card, lore, scene, state and turns into one request |
| `ULLMProvider` | UObject (abstract) | `Cloud` / `Local` / `Offline script` implementations |
| `UResponseValidator` | UBlueprintFunctionLibrary | JSON, lore, spoiler, safety checks; clamps value changes; checks claimed clue ids |
| `UClueJournalSubsystem` | UGameInstanceSubsystem | Found clues per chapter, topic unlocks, key-scene gate; feeds the Rumor Journal |
| `UDayNightSubsystem` | UWorldSubsystem | Chapter phases Day → Night → KeyScene; nightfall trigger, lighting and NPC spawns per phase |
| `UDanceMinigameComponent` | UActorComponent | Nightly rhythm game timing and grading; result → Favor, Suspicion, reward |
| `AHidingSpot` / `AEavesdropZone` → `BP_HidingSpot` / `BP_EavesdropZone` | AActor | Invisible-to-guards spots; timed overheard exchanges that award clues |
| `USuspicionComponent` **(derived)** | UActorComponent (on the player) | One Suspicion value fed by guards, Vashir and the dance; Caught at 100 |
| `AGuardAIController` → `BP_Guard` + `BT_Guard` | AAIController | Patrol and notice via AI Perception (sight) and a Behavior Tree |
| `WBP_HUD`, `WBP_Conversation`, `WBP_RumorJournal`, `WBP_Dance` | UUserWidget | Presentation; portrait swap by emotion inside `WBP_Conversation` |

---

## 2. Core Types & Data
### 2.1 Enums
| Enum | Values | Used by |
| :-- | :-- | :-- |
| `EChapterId` **(derived)** | Prologue, GardenOfAsh, PrivateDance, RooftopOfSmoke, TheTrap, ThePyre, Epilogue | `UAshesGameInstance`, `USceneDirectorSubsystem` |
| `EDirectorState` | Intro, FreeTalk, Nudge, Resolve, ForcedClose, Caught, NextChapter | `USceneDirectorSubsystem` |
| `EZafirEmotion` | Neutral, Teasing, Tender, Jealous, Cold, Angry (fallback Neutral) | `UNPCBrainComponent`, `WBP_Conversation` |
| `EVashirEmotion` | Serene, Curious, Pleased, Suspicious, Wrathful (fallback Serene) | `UNPCBrainComponent`, `WBP_Conversation` |
| Nura / Idris emotions | MISSING: their 4–6 emotion names | `WBP_Conversation` |
| `EGuardState` | Patrol, Notice (simplified: no chase; Caught comes from the Suspicion value) | `BT_Guard` |
| `EDayPhase` | Day, Night, KeyScene | `UDayNightSubsystem`, `UAshesGameInstance` |
| `EDanceGrade` | Great (> 80%), Fine (40–80%), Poor (< 40%) | `UDanceMinigameComponent` |
| `EAIMode` | Online, Offline | `UAshesGameInstance`, `WBP_Settings` |
| `ERoomId` **(derived)** | DancersQuarters, GardenOfAsh, GuardPost, ThroneHall, KingsChamber, RooftopTerrace | `UDayNightSubsystem`, `BP_RestrictedVolume` |

### 2.2 Structs & DataTables
> Row structs live in `Source/Ashes/Data/AshesTypes.h` as `USTRUCT(BlueprintType)` deriving from `FTableRowBase`, so DataTables can be edited in the editor or imported from CSV.

| Struct / Table | Fields | Used by |
| :-- | :-- | :-- |
| `FCharacterCardRow` / `DT_Characters` | Name, voice, wants, fears, speech rules, knows, must not say, secrets per chapter, emotion list, fallback emotion, turn limit, relationship value definitions, uses LLM (bool) | `UNPCBrainComponent`, `UPromptBuilder` |
| `FRelationshipValue` | Name (Trust / Affection / Favor / Suspicion), start, current 0–100, max increase per turn, max decrease per turn, thresholds with unlock ids | `UNPCBrainComponent`, `UResponseValidator` |
| `FSceneCardRow` / `DT_Scenes` | Chapter, place, time, goal text, beats (id, rule, required), turn limit, characters, authored opener, closing lines (full / weaker) | `USceneDirectorSubsystem`, `UPromptBuilder` |
| `DA_LoreBible` (Data Asset) | ~600-word text | `UPromptBuilder` |
| `FLLMResponse` | `reply`, `emotion`, `gesture`, `trust_delta`, `affection_delta`, `beats[]`, `suggestions[3]`, `off_topic`, `clues[]` (MISSING: Vashir's field names, presumably `favor_delta`, `suspicion_delta`); parsed with `FJsonObjectConverter` | `UResponseValidator`, `USceneDirectorSubsystem` |
| `FMemory` | Character, text, chapter | `UNPCBrainComponent` (max 6) |
| `FFallbackLineRow` / `DT_FallbackLines` | Scene, beat, emotion, line, AI-assisted flag (~120) | `UOfflineLLMProvider` |
| `FOfflineNodeRow` / `DT_OfflineTrees` | Scene, node id, 3 replies → character line, value changes, beats, clues | `UOfflineLLMProvider` |
| `FCutsceneLineRow` / `DT_Cutscenes` | Part, background, speaker, line (~150 total) | `WBP_Cutscene`, Level Sequences |
| `DA_ExplorationSettings` | Walk 220 cm/s, run 450 cm/s, cone 70° / 900 cm, +12/s run, +20/s restricted, −5/s out of view, patrol loop 20–40 s, stops 3–5 s, eavesdrop ~10 s (proposed) | `AAshesCharacter`, `BT_Guard`, `AEavesdropZone` |
| `FClueRow` / `DT_Clues` | Id, chapter, holder NPC, journal text, topic text, prompt hint, required flag, source (talk / eavesdrop / dance reward) | `UClueJournalSubsystem`, `UPromptBuilder`, `UResponseValidator` |
| `FChapterQuestionRow` (in `DT_Scenes`) | Chapter, question text, required clue count (3, proposed), key-scene character | `UClueJournalSubsystem`, `WBP_HUD` |
| `FDanceChartRow` / `DT_DanceCharts` | Night, song (MetaSound), BPM, tempo multiplier, length 30–45 s, notes (beat, lane 1–4), reward clue | `UDanceMinigameComponent` |
| `DA_ConversationSettings` | 200 chars, 3 suggestions, 80 words, 6 s timeout, 0.4 s typing, last 8 turns, 6 memories | `WBP_Conversation`, `UPromptBuilder`, providers |

---

## 3. Player System
- **GDD basis:** 2 Controls, 2 Camera, 2 Mechanics 5 (Exploration).
- **Architecture:** start from the UE 5.8 **Third Person (C++)** template. `AAshesCharacter` (ACharacter) with a Spring Arm + Camera, `UInteractionComponent`, `USuspicionComponent`; `BP_Sahra` sets the mesh and `ABP_Sahra`. Input through Enhanced Input (`IMC_Exploration`).
- **Class spec:** `AAshesCharacter`: `WalkSpeed = 220`, `RunSpeed = 450` (cm/s on `CharacterMovement->MaxWalkSpeed`), `bIsRunning` (read by guards), `bIsHidden`, `SetGameplayInputEnabled(bool)` (off during conversations, cutscenes, the dance and pause). `UInteractionComponent`: sphere trace each 0.1 s timer (not Tick), detection radius (MISSING), current target, delegate `OnInteract`.
- **Movement values:** walk 2.2 m/s = 220 cm/s, run 4.5 m/s = 450 cm/s. Jump: none (unbind the template's jump). Gravity: engine default. Spring Arm length, pitch limits and camera lag: MISSING.
- **Current state:** 🔲 Not started.
- **Required work:** project from template, input actions, character, camera, interaction prompt, test in `L_Test`.

---

## 4. Conversation System (Free Conversation)
- **GDD basis:** 1 Mechanics 1, 2 Mechanics 1, 8.1.
- **Architecture:** `UInteractionComponent` → `USceneDirectorSubsystem::StartScene(SceneRow)` → per turn: `WBP_Conversation` gets text → `UPromptBuilder::Build(card, lore, scene, state, last 8 turns, line)` → `ULLMProvider::SendAsync(request)` (6 s) → `UResponseValidator::Validate` → `USceneDirectorSubsystem::ApplyTurn` → `WBP_Conversation` shows the reply and swaps the portrait.
- **Class spec:** `ULLMProvider::SendAsync(const FLLMRequest&, FOnLLMReply Callback)` (abstract). `UCloudLLMProvider`: `FHttpModule` request, `SetTimeout(6.0f)`, JSON body, key from the environment variable `ASHES_LLM_API_KEY` or `Secrets/llm_key.txt` (never in `Config/*.ini`). `UOfflineLLMProvider`: answers from `DT_OfflineTrees` and `DT_FallbackLines`. `UResponseValidator::Validate(const FString& Json, const FValidationContext&) → FValidationResult` (parse with `FJsonObjectConverter`, known emotion, ≤ 80 words, off-topic, spoiler keywords, safety word list, clamp deltas, clue ids).
- **Numbers & formulas:** 200 characters input; 3 suggestions; 1–4 sentences / 80 words; 6 s timeout; 1 silent retry on broken JSON; regenerate once on spoiler; 3 unsafe inputs close the scene; typing indicator ≥ 0.4 s; portrait change ≤ 1 s.
- **Current state:** 🔲 Not started.
- **Required work:** provider base + cloud + offline providers; prompt builder; validator with Automation tests; `WBP_Conversation`; choose the LLM provider and model (GDD: a fast low-cost hosted model, for example Claude Haiku 5.5).

## 4.1 Relationship Values System
- **GDD basis:** 1 Mechanics 2, 2 Mechanics 2.
- **Architecture:** `UNPCBrainComponent` holds `FRelationshipValue`s; `USceneDirectorSubsystem` applies the validated deltas and broadcasts threshold events.
- **Class spec:** `ApplyDelta(Name, Delta)`, `GetWord(Name)` (for the prompt, for example "Trust: wary"; MISSING: the word bands), delegate `OnThresholdReached(Id)`.
- **Numbers & formulas:** `New = Clamp(Current + Clamp(Delta, -MaxDown, +MaxUp), 0, 100)`. Zafir Trust 20 (±8; 50, 70), Affection 10 (±8; 60). Vashir Favor 30 (±6; 60), Suspicion 10 (+10 / −5; 100 = caught).
- **Current state:** 🔲 Not started.
- **Required work:** values, clamping, threshold delegates, hint lines (MISSING: list per change), Automation tests.

## 4.2 Scene Director System
- **GDD basis:** 1 Mechanics 3, 2 Mechanics 3, Figure 3.
- **Architecture:** FSM in `USceneDirectorSubsystem`: Intro (authored opener) → FreeTalk → Nudge (2 turns left or 3 stalled turns; adds a steering note to the prompt) → FreeTalk; all beats met → Resolve (closing line) → NextChapter (save, cutscene); turn limit → ForcedClose (weaker beat) → NextChapter; Suspicion 100 → Caught → restart chapter.
- **Class spec:** `StartScene`, `ApplyTurn(const FValidationResult&)`, `AcceptBeat(Id)` (checks the beat's rule), delegates `OnSceneResolved(bool bFull)`, `OnCaught`.
- **Numbers & formulas:** turn limits 10 / 8 (4 in Ch4) / 4; nudge at limit − 2 or 3 turns without a new beat; 2–3 beats per scene.
- **Current state:** 🔲 Not started.
- **Required work:** FSM, beat rules per scene row, nudge notes (MISSING: text per scene), Automation tests for transitions.

## 4.3 Memory System (should have)
- **GDD basis:** 1 Mechanics 4, 2 Mechanics 4.
- **Architecture:** after `OnSceneResolved`, one summary call through `ULLMProvider` writes 1–2 memories per character into `UNPCBrainComponent` (kept in `UAshesGameInstance` across levels); memories go into the prompt state, the Rumor Journal and the pyre confession.
- **Numbers & formulas:** 6 kept per character (oldest dropped); 1–2 per scene; last 8 turns in full.
- **Current state:** 🔲 Not started.
- **Required work:** summary prompt, storage, journal display, pyre confession call with 2 memories. MISSING: behaviour when the summary call fails.

## 4.4 Exploration, Guards and Suspicion System (must have)
- **GDD basis:** 1 Mechanics 5 and 8, 2 Mechanics 5, 4 AI.
- **Architecture:** `USuspicionComponent` sums Suspicion from guard sight, Vashir's `suspicion_delta` and a Poor dance. Restricted rooms are `BP_RestrictedVolume` boxes. `AGuardAIController` uses AI Perception (Sight) and `BT_Guard`: patrol a spline or waypoint list with timed waits; while Sahra is perceived, stop and face her (Notice). `AHidingSpot` sets `bIsHidden`, and guards ignore a hidden Sahra. `AEavesdropZone` (box trigger beside a hiding spot) plays an authored exchange (~10 s) and awards a clue if Sahra stays in it unseen until the end.
- **Numbers & formulas:** sight 70° (`PeripheralVisionAngleDegrees = 35`), radius 900 cm, lose sight 1000 cm **(derived)**; +12/s running in view; +20/s restricted room in view; −5/s out of view (1 s timer, not Tick); +10 for a Poor dance; 100 = caught. Patrol loop 20–40 s, stops 3–5 s (proposed). MISSING: weapon-seen rate.
- **Current state:** 🔲 Not started.
- **Required work:** guard Blueprint and routes, perception, restricted volumes, hiding spots, eavesdrop zones, eye icon events, checkpoint restart, Automation test for the meter.

## 4.4b Clues and Rumor Journal (must have)
- **GDD basis:** 1 Mechanics 6, 2 Mechanics 6.
- **Architecture:** clues come from three sources: LLM replies (`clues[]`, accepted only if the NPC holds that clue this chapter in `DT_Clues`), offline / pre-written NPC trees, and eavesdrop / dance rewards. `UClueJournalSubsystem` stores them, adds topic replies and prompt hints, and opens the key scene at the required count; delegate `OnClueAdded` drives the toast and `WBP_RumorJournal`.
- **Numbers & formulas:** 5–6 clues per chapter, 3 required, 1–3 per NPC, toast 2 s (proposed).
- **Current state:** 🔲 Not started.
- **Required work:** `DT_Clues`, subsystem, journal widget, validator change, Automation tests (unknown clue, duplicate, gate).

## 4.5 Nightly Dance Minigame (must have)
- **GDD basis:** 1 Mechanics 7, 2 Mechanics 7.
- **Architecture:** `UDanceMinigameComponent` reads a `DT_DanceCharts` row per night; timing from a **Quartz** clock (`UQuartzSubsystem`) started with the song so notes stay in sync **(derived)**; `IMC_Dance` input; `WBP_Dance` draws the 4 lanes; result → Vashir's Favor, `USuspicionComponent`, `UClueJournalSubsystem` (reward clue). `UDayNightSubsystem` starts it at nightfall in the Throne Hall (Chapter 2: King's Chamber).
- **Numbers & formulas:** 4 lanes ← ↑ ↓ → (proposed), 0.25 s windows, 30 s on night 1 rising to 45 s, tempo +10% per night; > 80% → +10 Favor + reward; 40–80% → nothing; < 40% → +10 Suspicion.
- **Current state:** 🔲 Not started.
- **Required work:** component, 4 chart rows, `WBP_Dance`, Sahra's dance montages, Automation test for grading.

## 4.5b Day / Night Chapter Flow (must have)
- **Architecture:** `UDayNightSubsystem` per chapter: Day (investigate) → nightfall when the required clues are found or the player chooses "Go to perform" (proposed) → Night (dance) → KeyScene (Scene Director) → cutscene → next chapter. Switches the lighting (Directional Light / Sky Atmosphere + a night Post Process Volume, Lumen), the goal line and the `BP_NPCSpawnPoint` set.
- **Current state:** 🔲 Not started.
- **Required work:** phase state, day and night lighting setups, spawn sets, save at chapter start.

## 4.6 Cutscene System
- **GDD basis:** 3 Themes (Prologue, Pyre, Epilogue), 1 Gameplay Walkthrough.
- **Architecture:** visual-novel cutscenes as `WBP_Cutscene` (2D background, speaker, text, Space to advance) driven by `DT_Cutscenes`; Level Sequences (`LS_`) only where camera work in the palace is needed (the Trap) **(derived)**; the pyre uses generated lines with memories (should have; pre-written fallback).
- **Current state:** 🔲 Not started.
- **Required work:** cutscene widget, scripts (~150 lines), backgrounds, `LS_Trap`.

---

## 5. Input
> Enhanced Input. One Input Mapping Context per mode; `PC_Ashes` swaps them.

| Input Action | Mapping Context | Bound to |
| :-- | :-- | :-- |
| `IA_Move` (Axis2D) | `IMC_Exploration` | W A S D |
| `IA_Look` (Axis2D) | `IMC_Exploration` | Mouse XY |
| `IA_Run` (Digital, hold) | `IMC_Exploration` | Left Shift |
| `IA_Interact` (Digital) | `IMC_Exploration` | E (talk, interact, enter / leave hiding spot) |
| `IA_OpenJournal` (Digital) | `IMC_Exploration`, `IMC_Conversation` | Tab |
| `IA_Pause` (Digital) | all contexts | Esc |
| `IA_Submit` (Digital) | `IMC_Conversation` | Enter |
| `IA_PickReply1` / `2` / `3` (Digital) | `IMC_Conversation` | 1 / 2 / 3 |
| `IA_Advance` (Digital) | `IMC_Cutscene` | Space |
| `IA_DanceLane1–4` (Digital) | `IMC_Dance` | ← ↑ ↓ → (proposed) |
| `IA_Jump` | none | Removed from the template; not in the game |

Text entry uses a UMG Editable Text box; `IMC_Exploration` is removed while typing so W A S D do not move Sahra **(derived)**.

---

## 6. AI
- **States:** Guards: Patrol → Notice → Patrol (simplified from the GDD's Patrol → Notice → Investigate → Alert; no chasing). Conversation: Scene Director states (section 4.2). Emotions: per-character emotion tag → portrait in `WBP_Conversation`.
- **Perception:** `UAIPerceptionComponent` with Sight only: 70° total (35° peripheral half-angle), 900 cm radius, line of sight by the perception system; blind to hidden Sahra. No hearing (proposed). MISSING: Notice duration.
- **Classes:** `AGuardAIController`, `BT_Guard` / `BB_Guard`, `BP_Guard` (Character with a patrol route), NavMesh Bounds Volume (Recast); `USceneDirectorSubsystem`.
- **Current state:** 🔲 Not started.
- **Required work:** guard Blueprint, behavior tree, patrol routes in the garden (2 guards Ch1, 3 guards Ch2), suspicion hook.

---

## 7. UI System
- **Widgets:** UMG (follow `design.md` tokens): `WBP_Title`, `WBP_Settings`, `WBP_HUD`, `WBP_Conversation`, `WBP_ChapterCard`, `WBP_RumorJournal`, `WBP_Dance`, `WBP_Pause`, `WBP_Cutscene`, `WBP_Credits`.
- **How UI gets data:** dynamic multicast delegates (`BlueprintAssignable`) from `USceneDirectorSubsystem`, `UNPCBrainComponent`, `UClueJournalSubsystem`, `USuspicionComponent` and `UDayNightSubsystem`; no widget property bindings and no Tick polling.
- **Current state:** 🔲 Not started.
- **Required work:** screen stack in `PC_Ashes`, conversation screen first (Figure 5), then HUD, journal, dance and menus.

---

## 8. Save & Progress
- **What is saved:** current chapter (checkpoint), relationship values, found clues, memories, settings (text speed, volume, AI mode). MISSING: whether the conversation log is saved.
- **When:** at the start of each chapter (checkpoint, Figure 3 "Next chapter: save").
- **Class:** `UAshesSaveGame` (USaveGame) with `UGameplayStatics::SaveGameToSlot` / `LoadGameFromSlot`; settings in a separate `UAshesSettingsSave` slot **(derived)**.

---

## 9. Audio
- **Sound classes / mixes:** Sound Classes Music, SFX, UI under a Master class, one Sound Class Mix for the volume slider (GDD lists "volume"; MISSING: separate sliders) **(derived)**. Music as MetaSounds so the dance tempo can change per night.
- **Who plays which sound:** anim notifies on `ABP_Sahra` and `BP_Guard` for footsteps on stone; fire sounds on lantern and brazier Blueprints; `WBP_*` for UI sounds (MISSING in `design.md`); `UDayNightSubsystem` switches palace loop (day), dance song (night), romance loop (Zafir key scenes), finale (pyre, epilogue).

---

## 10. Performance Rules
- Avoid Tick for gameplay and UI logic. Use events, timers (`FTimerManager`), delegates and interfaces. Tick is allowed only inside the dance lane rendering if Quartz callbacks are not enough.
- Every HTTP request is async with a 6 s timeout and never blocks the game thread.
- Turn limits keep the number of LLM calls per playthrough fixed; measure tokens per playthrough in week 2.
- Lumen + Virtual Shadow Maps on by default; MISSING: frame-rate target and minimum PC spec (both team PCs must run it).

---

## 11. Testing
- **Test map:** `L_Test` (every new mechanic is tested here first), then `L_Palace`.
- **How each mechanic is tested:**
  - Validator, JSON parsing, clamping, Scene Director transitions, clue gate, dance grading: C++ Automation specs in `Source/Ashes/Tests/`, run from Session Frontend → Automation (filter `Ashes`).
  - LLM characters: test harness (editor utility or console command) that sends scripted player lines; red-team 50 lines per character (off-topic, jailbreak, spoiler, rude); LLM judge flags spoilers and broken voice; read every flagged reply + 20 random. Targets: 0 spoiler leaks, 95% valid JSON.
  - Offline mode: play every chapter with no key and no internet.
  - Movement, guards, UI: Play In Editor in `L_Test`, then a full run of `L_Palace`.
  - Playtests: 5 outside players (end of week 3), second playtest in week 4.

---

## 12. Open Decisions
| Question | Options | Decision |
| :-- | :-- | :-- |
| Engine | Unity 6.3 HDRP / Unreal Engine 5.8 | **Unreal Engine 5.8** (teacher approved, 10.11.2026) |
| What happens to the Unity skeleton in the repo | Replace in the same repo / new repo / keep for now | Decided 10.11.2026: replace in the same repo; the Unity version is kept as the Git tag `unity-skeleton` |
| C++ vs Blueprint split | All Blueprint / C++ core + Blueprint glue | C++ core + Blueprint glue (section 0.3) |
| Timeout | 6 s (GDD text) / 5 s (Figure 2) | 6 s (text wins) |
| LLM provider and model | Hosted low-cost model (for example Claude Haiku 5.5) | MISSING: final choice |
| Editor access for the AI agent | MCP + VibeUE / user does editor work | MCP + VibeUE (course template); MISSING: confirm both PCs have it installed |
| Court NPCs (Nura, Idris) as LLM | LLM / pre-written | Pre-written first; LLM if the week-2 risk check passes |
| Dance minigame | Minigame / cutscene | Minigame every night (10.11) |
| Guard patrols | Full FSM / simple patrol + perception | Simple patrol + perception + hiding spots, no chasing (10.11) |
| Nightfall trigger | Required clues found / player chooses / timer | Clues found or player chooses (proposed) |
| Vashir JSON fields | `favor_delta`, `suspicion_delta` | MISSING |

---

## Section Status
| Section | Status |
| :-- | :-- |
| 0. Meta & Conventions | FILLED |
| 1. Project Architecture | FILLED |
| 2. Core Types & Data | FILLED (MISSING: Nura/Idris emotions, Vashir JSON fields) |
| 3. Player System | FILLED (MISSING: camera values, interaction radius) |
| 4. Mechanic systems (4–4.6) | FILLED (MISSING items listed per system) |
| 5. Input | FILLED (dance keys proposed) |
| 6. AI | FILLED (MISSING: Notice duration) |
| 7. UI System | FILLED |
| 8. Save & Progress | FILLED (derived; confirm) |
| 9. Audio | FILLED (derived) |
| 10. Performance | FILLED (MISSING: fps target) |
| 11. Testing | FILLED |
| 12. Open Decisions | FILLED (MISSING: LLM provider) |
