# Ashes for a Goddess · Technical Design Document (technical_design.md)

> **Canonical design source:** `gdd.md`. This document turns the GDD into a concrete Blueprint / C++ architecture. **Where this document and the GDD disagree, the GDD wins** and this document must be corrected.
> **Engine target:** Unreal Engine 5.8 (C++ + Blueprints), Windows.
> **Course:** Advanced Game Programming. **Team (12.11.2026):** Merve Özal (level design; wrote the Phase 0–5 code), Tuana Tuna (art), Tala Alsayed (audio, VFX, dance charts and animations, cutscene sequences, credits, playtests), Ebrar Baha Özüpek (C++ systems from Phase 7 on).
> **Companion documents:** `level-design.md` (where things are) and `production_pipeline.md` (build order). Game data texts: the Master Lore document (single source for lore, cards, clues, barks, fallback lines and offline trees).

**Status legend:** `✅ Built` · `🟡 Partial` · `🔲 Not started` · `♻️ Built but must change`

**Source legend:** class names and their jobs come from GDD Figure 6 (Main scripts), translated to Unreal. Items marked **(derived)** are architecture details this document adds to make the GDD buildable; they are not in the GDD and can be changed freely.

**Gameplay (10.11–12.11.2026):** 7 chapters plus a 2D Prologue and Epilogue. Chapters 1–6 are each a day (main quest, 2 "do" objectives, clues into the Rumor Journal while avoiding guards, with hiding spots and eavesdropping) and a night (dance minigame to keep Sahra's cover), then a key conversation and an in-engine ending. Chapter 6 (the trap) has a fixed outcome; Chapter 7 is a spirit walk with 6 soul embers and a 6-piece generated confession. About 10 ambient NPCs give one bark each. Clues, guards and the dance are must-haves. See `gdd.md` 1 Mechanics 1–10.

**Project state (12.11.2026):** the UE 5.8 project exists (`Ashes.uproject`, module `Ashes`). Phases 0–4 are merged (PRs #27, #29, #30), Phase 5 (conversation screen, minimal Scene Director) is merged (#31). The earlier Unity 6.3 skeleton lives only in the Git tag `unity-skeleton`.

---

## 0. Meta & Conventions

### 0.1 How to read this document
Every system section uses the same structure: **GDD basis → Architecture → Class spec → Numbers & formulas → Current state → Required work.**

### 0.2 Naming Conventions
- **C++ classes:** `A` (Actor), `U` (UObject / Component / Subsystem), `F` (struct), `E` (enum), `I` (interface). PascalCase, one class per file pair (`.h` / `.cpp`).
- **Blueprints:** `BP_` (actor), `WBP_` (widget), `GM_` (game mode), `PC_` (player controller), `PS_` (player state), `GI_` (game instance), `AC_` (actor component), `BI_` (interface), `ABP_` (animation Blueprint) **(derived)**, `BT_` / `BB_` (behavior tree / blackboard) **(derived)**.
- **Assets:** `SM_`, `SK_`, `M_`, `MI_`, `T_`, `NS_` (Niagara), `DT_` (DataTable), `DA_` (Data Asset) **(derived)**, `L_` (level), `LS_` (Level Sequence) **(derived)**, `IA_` / `IMC_` (Enhanced Input), `S_` / `MS_` (Sound Wave / MetaSound) **(derived)**.
- **Project prefix / module:** `Ashes` (C++ module `Ashes`, classes such as `AAshesGameMode`, `UAshesGameInstance`).
- **Data row names:** scenes `Ch<N>_<Character><Scene>` (e.g. `Ch1_ZafirKeyScene`); clues `ch<N>_<who>_<what>` (e.g. `ch1_nura_disappears`); beats in snake case (e.g. `uneasy_truce`).
- **AI-assisted header:** every AI-assisted C++ file starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.`; AI-assisted Blueprints get the same text in a Comment node at the top of the Event Graph.

### 0.3 Tools & workflow
- **Blueprint or C++:** C++ for the testable core: LLM HTTP client, prompt builder, response validator, value clamping, Scene Director state machine, clue journal, offline trees, spirit walk logic, data structs (all covered by Automation tests). Blueprints for gameplay glue, level scripting, UI (UMG), animation, guard behavior trees and the dance presentation. Blueprints derive from the C++ classes.
- **AI agent:** Claude Code (or Codex / Antigravity) connected to the editor through MCP + VibeUE; GitHub Copilot in the IDE for C++. The agent never writes `.uasset` / `.umap` as text.
- **Version control:** Git + Git LFS. `.uasset` and `.umap` are binary and cannot be merged: lock them with `git lfs lock` before editing, and only one person edits a given asset or level at a time. `L_Palace` uses **One File Per Actor** (Use External Actors) so several people can change different actors in the same level **(derived)**. `main` always opens and runs; feature branches `feature/<name>-<task>`, pull request, review by another team member, merge only when `L_Palace` plays. Never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `.vs/`.
- **Who touches what (12.11.2026):** Baha owns `Source/Ashes/` from Phase 7 on; Merve owns `L_Palace`, spawn points, routes and data entry (DataTables); Tuana owns `Content/Ashes/Art/` and the widget styles; Tala owns `Content/Ashes/Audio/`, VFX (`NS_`), dance charts, Level Sequences and `WBP_Credits`.

---

## 1. Project Architecture

### 1.1 Folder layout
```
Ashes.uproject
Source/Ashes/
  Core/        AshesGameMode, AshesGameInstance, AshesSaveGame, DayNightSubsystem, SpiritWalkSubsystem
  Player/      AshesCharacter, AshesPlayerController, InteractionComponent, AshesInteractable
  Dialogue/    SceneDirectorSubsystem, NPCBrainComponent, PromptBuilder, ResponseValidator,
               LLMProvider, OpenAICompatibleLLMProvider, OfflineLLMProvider, LocalModelSubsystem, AshesAISettings
  Gameplay/    AshesNPC, ClueJournalSubsystem, SuspicionComponent, HidingSpot, EavesdropZone,
               DanceMinigameComponent, BarkComponent, FollowerComponent, SoulEmber
  AI/          GuardAIController
  UI/          ConversationWidget (C++ base of WBP_Conversation)
  Data/        AshesTypes.h (structs and enums for DataTables)
  Tests/       Automation specs (validator, prompt builder, clamps, Scene Director, clue gate, offline trees, dance grading)
Content/Ashes/
  Core/        GM_Ashes, GI_Ashes, PC_Ashes
  Player/      BP_Sahra, ABP_Sahra
  Characters/  BP_Zafir, BP_Vashir, BP_Nura, BP_Idris, BP_Omar, BP_Ilyas, BP_Tariq, BP_Aisha,
               BP_KingsServant, BP_AmbientNPC, BP_Guard
  Gameplay/    BP_HidingSpot, BP_EavesdropZone, BP_RestrictedVolume, BP_NPCSpawnPoint, BP_SoulEmber, BP_KnifeHidingPlace
  AI/          BT_Guard, BB_Guard
  UI/          WBP_Title, WBP_Settings, WBP_HUD, WBP_Conversation, WBP_RumorJournal, WBP_Dance,
               WBP_ChapterCard, WBP_Pause, WBP_Cutscene, WBP_Credits, WBP_InteractPrompt
  Data/        DT_Characters, DT_Scenes, DT_Clues, DT_Barks, DT_DanceCharts, DT_FallbackLines, DT_OfflineTrees,
               DT_Cutscenes, DA_LoreBible, DA_ExplorationSettings, DA_ConversationSettings
  Input/       IA_*, IMC_Exploration, IMC_Conversation, IMC_Cutscene, IMC_Dance
  Maps/        L_Test, L_Title, L_Palace
  Cinematics/  LS_Prologue, LS_Trap, LS_Epilogue
  Art/         Meshes, Materials, Textures, Portraits, VFX (Niagara)
  Audio/       Music, SFX, MetaSounds
Config/        (committed; never holds an API key)
Docs/
ThirdParty/LLM/  (git-ignored) built-in AI: llama-server + GGUF model, downloaded by Tools/GetLocalLLM.ps1; shipped in the packaged game
Secrets/       (git-ignored) only for an optional external endpoint key; the built-in AI needs no key
```

### 1.2 Core classes
Three layers (GDD Figure 6). Gameplay sends the chapter to Conversation AI; Conversation AI sends what to show to Presentation. Game code reaches the LLM only through `ULLMProvider`.

| Class | Parent | Responsibility |
| :-- | :-- | :-- |
| `AAshesGameMode` → `GM_Ashes` | AGameModeBase | Rules: win (end of epilogue), lose (Suspicion 100 → restart chapter at checkpoint); no lose in Chapters 6 and 7 |
| `UAshesGameInstance` → `GI_Ashes` | UGameInstance | Current chapter, AI mode (Online / Offline), relationship values, memories, Sahra's lines per chapter (for Chapter 7) across level loads; `RequestReply` always answers (model or fallback) |
| `AAshesPlayerController` → `PC_Ashes` | APlayerController | Enhanced Input contexts, UI focus, pause |
| `AAshesCharacter` → `BP_Sahra` | ACharacter | Walk / run, hide state, dance animations, ankle item (cord / anklet / mended cord) |
| `UInteractionComponent` → `AC_Interaction` | UActorComponent | Finds the highlighted character or object; E starts a conversation, a bark, enters a hiding spot or uses an object |
| `IAshesInteractable` | UInterface | Prompt text, can-interact, interact, focus on / off; implemented by talkable characters, ambient NPCs, hiding spots, the knife hiding places, plan-check objects (Chapter 6) and soul embers **(derived)** |
| `AAshesNPC` → `BP_Zafir`, `BP_Vashir`, `BP_Nura`, `BP_Idris`, `BP_Omar`, `BP_Ilyas`, `BP_Tariq`, `BP_Aisha` | ACharacter + `IAshesInteractable` | Display name, `CharacterRowName`, `SceneRowName`, "E: Talk" prompt (`WBP_InteractPrompt` as a screen-space Widget Component); starts the scene on E |
| `BP_KingsServant` | `AAshesNPC` (bUsesLLM false) | Takes the cord in Chapter 4 (in-engine), follows Sahra in Chapter 5 (`UFollowerComponent`), fixed 3-line exchange when cornered |
| `BP_AmbientNPC` | `AAshesNPC` (bUsesLLM false) + `UBarkComponent` | One bark per chapter from `DT_Barks`, then turns away |
| `USceneDirectorSubsystem` | UWorldSubsystem | Conversation FSM: intro, free talk, nudge, resolve, forced close, caught; accepts beats and clues (Phase 5, 11.11.2026: minimal version: StartScene / SubmitLine / PickSuggestion / EndScene, 1 retry then fallback, turn limit, prompt warm-up; delegates OnLinePosted, OnReplyReady, OnThinkingChanged, OnTurnsChanged, OnSceneEnded) |
| `UNPCBrainComponent` | UActorComponent (one per LLM character) | Character card row, relationship values, memories |
| `UPromptBuilder` | UBlueprintFunctionLibrary | Assembles card, lore, scene, state, clue hints and turns into one request |
| `ULLMProvider` | UObject (abstract) | `UOpenAICompatibleLLMProvider` (built-in llama-server, or an optional external OpenAI-compatible endpoint such as Ollama) and `UOfflineLLMProvider` (offline trees and fallback lines) |
| `ULocalModelSubsystem` | UEngineSubsystem | Starts the bundled llama-server hidden on 127.0.0.1 (free port, Windows job object so it dies with the game), polls `/health`, reports Ready / Failed **(derived)** |
| `UAshesAISettings` | UDeveloperSettings (Project Settings > Game > Ashes AI) | Model folder (`ASHES_LLM_DIR` overrides), model file, port, context, device, GPU layers, timeouts, optional external endpoint; per-PC device override: user environment variable `ASHES_LLM_DEVICE` (e.g. `none` = CPU only; never committed) or the console variable `Ashes.LLM.Device` for one session **(derived)** |
| `UResponseValidator` | UBlueprintFunctionLibrary | JSON, lore, spoiler, safety checks; clamps value changes; checks claimed beat and clue ids |
| `UClueJournalSubsystem` | UGameInstanceSubsystem | Found clues per chapter, topic unlocks, key-scene gate (3 of 6); feeds the Rumor Journal |
| `UDayNightSubsystem` | UWorldSubsystem | Chapter phases Day → Night → KeyScene → Ending; nightfall trigger, lighting, NPC spawns per chapter and phase, the palace's look per chapter (moon, lanterns, banners, blossoms, Chapter 7 ghost look) |
| `USpiritWalkSubsystem` (*proposed name*, 12.11) | UWorldSubsystem | Chapter 7: 6 soul embers, memory replay, one confession piece per ember (generated, else pre-written), red → gold at the end |
| `UDanceMinigameComponent` | UActorComponent | Nightly rhythm game timing and grading; result → Favor, Suspicion, reward clue |
| `AHidingSpot` / `AEavesdropZone` → `BP_HidingSpot` / `BP_EavesdropZone` | AActor | Invisible-to-guards spots; timed overheard exchanges that award clues |
| `USuspicionComponent` **(derived)** | UActorComponent (on the player) | One Suspicion value fed by guards, Vashir and the dance; Caught at 100; disabled in Chapters 6 and 7 |
| `UBarkComponent` **(derived)** | UActorComponent | Picks the bark row for the current chapter; shows it as a one-line bubble for 3 s (*proposed*) |
| `UFollowerComponent` **(derived)** | UActorComponent | Chapter 4: Zafir's tail path with look-back points; Chapter 5: the king's servant follows Sahra and loses her when line of sight breaks for about 3 s (*proposed*) |
| `AGuardAIController` → `BP_Guard` + `BT_Guard` | AAIController | Patrol and notice via AI Perception (sight) and a Behavior Tree |
| `ASoulEmber` → `BP_SoulEmber` | AActor + `IAshesInteractable` | Chapter 7 pickup; tells `USpiritWalkSubsystem` which chapter's memory to replay |
| `WBP_HUD`, `WBP_Conversation`, `WBP_RumorJournal`, `WBP_Dance` | UUserWidget | Presentation; portrait swap by emotion inside `WBP_Conversation`. `WBP_Conversation` derives from C++ `UConversationWidget` (BindWidget names: NameText, DialogueScroll, DialogueText, ThinkingText, TurnsText, Reply1–3 + Reply1Text–3, InputBox, SendButton, LeaveButton, PortraitImage, EmotionText) |

---

## 2. Core Types & Data
### 2.1 Enums
| Enum | Values | Used by |
| :-- | :-- | :-- |
| Chapter number **(derived)** | `int32` 0–8 (0 Prologue, 1–7 chapters, 8 Epilogue) in the row structs; an `EChapterId` enum is optional | `UAshesGameInstance`, `USceneDirectorSubsystem`, `DT_*` |
| `EDirectorState` | Intro, FreeTalk, Nudge, Resolve, ForcedClose, Caught, NextChapter | `USceneDirectorSubsystem` |
| Emotions (per card, `TArray<FName>` in `FCharacterCardRow`) | Zafir: neutral, teasing, tender, jealous, cold, angry. Vashir: serene, curious, pleased, suspicious, wrathful. Nura: neutral, amused, sharp, wary, sad. Idris: neutral, stern, wary, troubled. Minor characters: per card (Master Lore, Character Cards) | `UNPCBrainComponent`, `UResponseValidator`, `WBP_Conversation` |
| `EGuardState` | Patrol, Notice (simplified: no chase; Caught comes from the Suspicion value) | `BT_Guard` |
| `EDayPhase` | Day, Night, KeyScene, Ending | `UDayNightSubsystem`, `UAshesGameInstance` |
| `EDanceGrade` | Great (> 80%), Fine (40–80%), Poor (< 40%) | `UDanceMinigameComponent` |
| `EAIMode` | Online, Offline | `UAshesGameInstance`, `WBP_Settings` |
| `ERoomId` **(derived)** | DancersQuarters, GardenOfDusk, GuardPost, CityGate, ThroneHall, KingsChamber, RooftopTerrace | `UDayNightSubsystem`, `BP_RestrictedVolume`, `BP_NPCSpawnPoint` |
| `EClueSource` | Talk, Eavesdrop, Do, DanceReward | `FClueRow`, `UClueJournalSubsystem` |
| `ERoute` **(derived)** | Romance, Comrade (from Zafir's Affection) | closing lines, Chapter 7, Epilogue |

### 2.2 Structs & DataTables
> Row structs live in `Source/Ashes/Data/AshesTypes.h` as `USTRUCT(BlueprintType)` deriving from `FTableRowBase`, so DataTables can be edited in the editor or imported from CSV. Texts come from the Master Lore document; Merve enters them.

| Struct / Table | Fields | Rows | Used by |
| :-- | :-- | :-- | :-- |
| `FCharacterCardRow` / `DT_Characters` | DisplayName, Voice, Wants, Fears, SpeechRules, Knows, MustNotSay, Secrets (`FChapterSecret`: chapter, text), Emotions, FallbackEmotion, TurnLimit, Values (`FRelationshipValueDef`), SpoilerKeywords, bUsesLLM (SpoilerKeywords are `FSpoilerKeyword`: keyword + UnlockChapter, 0 = never; implemented 12.11.2026) | 8 LLM characters (Zafir, Vashir, Nura, Idris, Omar, Ilyas, Tariq, Aisha) + the king's servant (bUsesLLM false) | `UNPCBrainComponent`, `UPromptBuilder`, `UResponseValidator` |
| `FRelationshipValueDef` | Name (Trust / Affection / Favor / Suspicion), start, max increase per turn, max decrease per turn, thresholds with unlock ids | — | `UNPCBrainComponent`, `UResponseValidator` |
| `FSceneCardRow` / `DT_Scenes` | Chapter, Place, Time, GoalText, Beats (`FBeatDef`: id, rule, required), TurnLimit, Characters, Opener, ClosingFull, ClosingWeaker; **proposed new field** `ClosingRomance` (route line) | 8: `Ch1_ZafirKeyScene`, `Ch2_ZafirRehearsal`, `Ch3_VashirAudience`, `Ch3_ZafirFountain`, `Ch4_ZafirKnife`, `Ch5_ZafirRooftop`, `Ch6_VashirTrap`, `Ch7_ZafirConfession`; plus a short general scene per chapter for the side characters (*proposed*) | `USceneDirectorSubsystem`, `UPromptBuilder` |
| `FChapterRow` / `DT_Chapters` **(derived, proposed)** | Chapter, title, verse, question, required clue count (3; 0 in Chapters 6–7), Do 1 and Do 2 text, dance chart row, key scene rows, moon | 7 | `UClueJournalSubsystem`, `UDayNightSubsystem`, `WBP_HUD`, `WBP_ChapterCard` |
| `UAshesLoreBible` / `DA_LoreBible` (Data Asset) | ~600-word text | 1 | `UPromptBuilder` |
| `FLLMResponse` | `reply`, `emotion`, `gesture`, one `<value>_delta` per relationship value of the card (Zafir: `trust_delta`, `affection_delta`; Vashir: `favor_delta`, `suspicion_delta`), `beats[]`, `suggestions[3]`, `off_topic`, `clues[]`; parsed by `UResponseValidator` with `FJsonObject` (generic deltas), stored as `Deltas` map (implemented 11.11.2026) | — | `UResponseValidator`, `USceneDirectorSubsystem` |
| `FMemory` | Character, text, chapter, player line (for Chapter 7) | max 6 per character | `UNPCBrainComponent`, `USpiritWalkSubsystem` |
| `FFallbackLineRow` / `DT_FallbackLines` | Character, scene, beat, emotion, clue, line, AI-assisted flag (implemented 12.11.2026: Character, Type (`Content` = offline answer, `Hesitate` = evasive line for online failures), Scene or "any", Beat, Emotion, Line with [gestures], bAIAssisted; pick = character, scene, pushed beat else "any", current emotion preferred, no repeat in a row; Clue column later. Data: 7 Content rows (Chapter 1) + 54 evasive rows for 8 characters) | 142 (Zafir 53, Vashir 22, side characters 67) | `UOfflineLLMProvider` |
| `FOfflineNodeRow` / `DT_OfflineTrees` | Scene, node id, 3 replies (text, next node, value changes, beats, clues), character line, emotion, stand-in line when a beat rule fails | 7 key-scene trees + 34 side-character nodes (24 clue, 10 small talk) + 12 topic and repeat-visit lines | `UOfflineLLMProvider` |
| `FBarkRow` / `DT_Barks` (new 11.11) | NPC, chapter, line; eavesdrop rows: speakers, lines, clue id; dance gossip rows: line, clue id | 60 barks (10 ambient NPCs × Chapters 1–6) + 7 eavesdrops + 3 dance-reward gossips | `UBarkComponent`, `AEavesdropZone`, `UDanceMinigameComponent` |
| `FCutsceneLineRow` / `DT_Cutscenes` | Part, background, speaker, line, route | Prologue (8 lines), Epilogue (romance and comrade variants), 6 ember captions, the 6-piece fallback confession | `WBP_Cutscene`, `USpiritWalkSubsystem` |
| `FClueRow` / `DT_Clues` | Id, chapter, holder (character row or source actor), source (`EClueSource`), journal text, topic text (suggested reply), prompt hint, requires clue (e.g. `ch3_aisha_one_lie` needs `ch3_guards_sent_away`) (implemented 12.11.2026: Chapter, Holder, Source as text, JournalText, Topic, PromptHint; the holder's prompt gets the hints of its talk clues; `EClueSource` and RequiresClue come in Phase 9) | 36 (6 per palace chapter) | `UClueJournalSubsystem`, `UPromptBuilder`, `UResponseValidator` |
| `FDanceChartRow` / `DT_DanceCharts` | Night, song (MetaSound), BPM, tempo multiplier, length 30–45 s, notes (beat, lane 1–4), reward clue, bFixedOutcome | 6 (30, 33, 36, 39, 42, 45 s) | `UDanceMinigameComponent` |
| `DA_ExplorationSettings` | Walk 220 cm/s, run 450 cm/s, cone 70° / 900 cm, +12/s run, +20/s restricted, −5/s out of view, patrol loop 20–40 s, stops 3–5 s, eavesdrop ~10 s, follower lose time 3 s (proposed) | 1 | `AAshesCharacter`, `BT_Guard`, `AEavesdropZone`, `UFollowerComponent` |
| `DA_ConversationSettings` | 200 chars, 3 suggestions, 80 words, 6 s timeout (30 s in the editor), 0.4 s typing, last 8 turns, 6 memories | 1 | `WBP_Conversation`, `UPromptBuilder`, providers |

---

## 3. Player System
- **GDD basis:** 2 Controls, 2 Camera, 1 Mechanics 5 (Exploration).
- **Architecture:** started from the UE 5.8 **Third Person (C++)** template. `AAshesCharacter` (ACharacter) with a Spring Arm + Camera, `UInteractionComponent`, `USuspicionComponent`; `BP_Sahra` sets the mesh and `ABP_Sahra`. Input through Enhanced Input (`IMC_Exploration`).
- **Class spec:** `AAshesCharacter`: `WalkSpeed = 220`, `RunSpeed = 450` (cm/s on `CharacterMovement->MaxWalkSpeed`), `bIsRunning` (read by guards), `bIsHidden`, `SetGameplayInputEnabled(bool)` (off during conversations, cutscenes, the dance and pause). `UInteractionComponent`: sphere overlap (Pawn + WorldDynamic) on a 0.1 s timer (not Tick), detection radius **200 cm** (decided 10.11.2026, Phase 2), only targets in front of Sahra (dot ≥ 0), nearest wins; current target, delegates `OnFocusChanged` and `OnInteract`; E (`IA_Interact`) calls `TryInteract()`. Interactables implement `IAshesInteractable` (`GetInteractPrompt`, `CanInteract`, `Interact`, `SetInteractionFocus`). Chapter 7: `BP_Sahra` switches to a spirit material (Tala) and the Suspicion component is off.
- **Movement values:** walk 2.2 m/s = 220 cm/s, run 4.5 m/s = 450 cm/s (hold Left Shift). Jump: none (no jump binding; `CanJumpInternal` returns false). Gravity: engine default. Turn rate 540°/s, orient rotation to movement.
- **Camera values (decided 10.11.2026, Phase 1):** Spring Arm length 350 cm, socket offset (0, 50, 60) cm (over the right shoulder), camera lag on at speed 12, probe size 12 cm, FOV 80°; pitch limits −60° (look down) to +25° (look up) on `PC_Ashes` (`CameraPitchMin` / `CameraPitchMax`); mouse Y inverted in `IMC_Exploration` like the template (move the mouse up = look up).
- **Current state:** ✅ Built (Phases 1–2: movement, camera, interaction, "E: Talk").
- **Required work:** hide state (Phase 10), ankle item swap (Phase 13), spirit look (Phase 14).

---

## 4. Conversation System (Free Conversation)
- **GDD basis:** 1 Mechanics 1, 2 Mechanics 1, 8.1.
- **Architecture:** `UInteractionComponent` → `USceneDirectorSubsystem::StartScene(SceneRow)` → per turn: `WBP_Conversation` gets text → `UPromptBuilder::Build(card, lore, scene, state, clue hints, last 8 turns, line)` → `ULLMProvider::SendAsync(request)` (6 s) → `UResponseValidator::Validate` → `USceneDirectorSubsystem::ApplyTurn` → `WBP_Conversation` shows the reply and swaps the portrait.
- **Class spec:** `ULLMProvider::SendAsync(const FLLMRequest&, FOnLLMReply Callback)` (abstract). `UOpenAICompatibleLLMProvider`: `FHttpModule` POST to `<BaseUrl>/chat/completions` (OpenAI-compatible JSON), `SetTimeout(6.0f)` (6 s in the game; soft limit `RequestTimeoutSeconds` = 6 s (hesitation shown, still waiting), hard limit `HardTimeoutSeconds` = 20 s in the game and `EditorHardTimeoutSeconds` = 60 s in the editor and PIE (slow development PCs)), streamed so a timed-out answer stops on the server, `response_format: json_object`, thinking off (`chat_template_kwargs.enable_thinking = false`), completion on the game thread. Built-in model: `BaseUrl = http://127.0.0.1:<port>/v1`, no API key. `UOfflineLLMProvider`: offline tree node or fallback line in the same JSON (see 4.7). `UAshesGameInstance::RequestReply` always answers: model reply, or the offline answer on timeout / error / model not ready. Debug: `Ashes.TestLLM [line]`, `Ashes.LLM.Start`, `Ashes.LLM.Stop`, `Ashes.LLM.Status`, `Ashes.TalkZafir`, `Ashes.Scene.Say / Pick / End`. `UResponseValidator::Validate(const FString& Json, const FAshesValidationContext&) → FAshesValidationResult` (renamed: `FValidationContext` exists in the engine) (parse with `FJsonObjectConverter`, known emotion, ≤ 80 words, off-topic, spoiler keywords, safety word list, clamp deltas, beat and clue ids).
- **Numbers & formulas:** 200 characters input; 3 suggestions; 1–4 sentences / 80 words; first words of the reply on screen within 4 s on average (decided 13.11.2026; measured in the game window, not the editor; the GDD 13.5 early warning uses this number): the provider already receives the answer token by token, and `WBP_Conversation` shows the `reply` field word by word while it arrives (`reply` is the first field the model writes); the finished JSON still passes `UResponseValidator`, and if it fails (spoiler, safety word, broken JSON) the shown words are replaced by the regenerated or fallback line; 6 s timeout; 1 silent retry on broken JSON; regenerate once on spoiler; 3 unsafe inputs close the scene; typing indicator ≥ 0.4 s; portrait change ≤ 1 s. **Turn limits (decided 12.11.2026):** Zafir 11, Vashir 7 (Chapter 3 audience and Chapter 6 trap), Nura, Idris and the minor characters 5 at most, ambient NPCs 1 line (no conversation), Chapter 7 no turns.
- **Current state (11.11.2026):** 🟡 providers done (Phase 3); `UPromptBuilder`, `UResponseValidator`, row structs, `DT_Characters`, `DT_Scenes`, `DA_LoreBible` (placeholders until the Master Lore texts are entered), 8 Automation tests green, `WBP_Conversation` + minimal Scene Director (Phase 5). The code default `TurnLimit = 10` is overridden per row; the rows must be set to 11 / 7 / 5.
- **Required work:** enter the Master Lore cards and scene rows (Merve); turn limits per row; side-character scenes; romance closing line (`ClosingRomance`, Baha).

## 4.1 Relationship Values System
- **GDD basis:** 1 Mechanics 2, 2 Mechanics 2.
- **Architecture:** `UNPCBrainComponent` holds the values; `USceneDirectorSubsystem` applies the validated deltas and broadcasts threshold events. Only Zafir and Vashir have values; the other characters give clues.
- **Class spec:** `ApplyDelta(Name, Delta)`, `GetWord(Name)` (for the prompt, e.g. "Trust: wary"; word bands *proposed*: 0–19 cold, 20–39 wary, 40–59 warming, 60–79 close, 80–100 devoted), delegate `OnThresholdReached(Id)`, `GetRoute()` (Romance when Affection ≥ 60 at the end of Chapter 5, *proposed*).
- **Numbers & formulas:** `New = Clamp(Current + Clamp(Delta, -MaxDown, +MaxUp), 0, 100)`. Zafir: Trust start 20 (±8), Affection start 10 (±8). Vashir: Favor start 30 (±6), Suspicion start 10 (+10 / −5). **Unlocks:** Trust 30 → one true thing (Chapter 2); Trust 50 → admits he hunts Vashir (Chapter 4); Trust 60 on the romance route → his village; Affection 40 → jealousy shows (Chapter 3); Affection 60 → the rooftop confession (Chapter 5); Favor 60 → the invitation (Chapter 3); Suspicion 100 → caught.
- **Current state:** 🟡 clamping and applying done in Phase 4–5; thresholds, words and hints not yet.
- **Required work:** threshold delegates, word bands in the prompt, hint lines (GDD: "Zafir looks at you a moment too long."; list per change in the Master Lore doc), route, Automation tests.

## 4.2 Scene Director System
- **GDD basis:** 1 Mechanics 3, 2 Mechanics 3, Figure 3.
- **Architecture:** FSM in `USceneDirectorSubsystem`: Intro (authored opener) → FreeTalk → Nudge (2 turns left or 3 stalled turns; adds a steering note to the prompt) → FreeTalk; all required beats met → Resolve (full closing line, or the romance closing on that route) → NextChapter (save, in-engine ending); turn limit → ForcedClose (weaker closing line; the Director sets the required beats so the story goes on) → NextChapter; Suspicion 100 → Caught → restart chapter. Chapter 6: `vashir_names_her` and `contract_revealed` are forced by the Director at set turns (fixed outcome) and every player line is saved as Zafir's memory. Chapter 3 below Favor 60: the invitation comes later through Nura (scripted).
- **Class spec:** `StartScene`, `ApplyTurn(const FAshesValidationResult&)`, `AcceptBeat(Id)` (checks the beat's rule), `AcceptClue(Id)` (only if the speaker holds it this chapter), `ForceBeat(Id)`, delegates `OnSceneResolved(bool bFull)`, `OnCaught`.
- **Numbers & formulas:** turn limits Zafir 11, Vashir 7, others 5; nudge at limit − 2 or 3 turns without a new beat; 1–3 beats per scene.
- **Current state:** 🟡 minimal version (Phase 5): start, turns, simple end at the limit.
- **Required work (Phase 7, Baha):** full FSM, beat rules per scene row, nudge notes per scene (Master Lore, Scene Cards), forced beats for Chapter 6, Automation tests for transitions.

## 4.3 Memory System (should have)
- **GDD basis:** 1 Mechanics 4, 2 Mechanics 4.
- **Architecture:** after `OnSceneResolved`, one summary call through `ULLMProvider` writes 1–2 memories per character into `UNPCBrainComponent` (kept in `UAshesGameInstance` across levels); memories go into the prompt state, the Rumor Journal, the Chapter 7 embers (one player line per chapter) and Zafir's confession. The game also keeps one notable player line per chapter without the LLM (the longest line in the key scene, *proposed*) so the embers always have something to replay.
- **Numbers & formulas:** 6 kept per character (oldest dropped); 1–2 per scene; last 8 turns in full.
- **Current state:** 🔲 Not started.
- **Required work (Phase 16, Baha):** summary prompt, storage, journal display, player-line keeper, confession calls. If the summary call fails: keep the stored player line and skip the memory (*proposed*).

## 4.4 Exploration, Guards and Suspicion System (must have)
- **GDD basis:** 1 Mechanics 5 and 8, 2 Mechanics 5, 4 AI.
- **Architecture:** `USuspicionComponent` sums Suspicion from guard sight, Vashir's `suspicion_delta` and a Poor dance. Restricted rooms are `BP_RestrictedVolume` boxes (the King's Chamber, off while Sahra is invited). `AGuardAIController` uses AI Perception (Sight) and `BT_Guard`: patrol a waypoint list (Target Points) with timed waits; while Sahra is perceived, stop and face her (Notice). `AHidingSpot` sets `bIsHidden`, and guards ignore a hidden Sahra. `AEavesdropZone` (box trigger beside a hiding spot) plays an authored exchange from `DT_Barks` (~10 s) and awards a clue if Sahra stays in it unseen until the end. **New stealth (11.11):** Chapter 4 tail (`UFollowerComponent` on Zafir: scripted path, 3 look-back points; being seen restarts the tail), Chapter 5 being followed (`UFollowerComponent` on the king's servant, 6–10 m; loses Sahra after about 3 s out of sight).
- **Numbers & formulas:** sight 70° (`PeripheralVisionAngleDegrees = 35`), radius 900 cm, lose sight 1000 cm **(derived)**; +12/s running in view; +20/s restricted room in view; −5/s out of view (1 s timer, not Tick); +10 for a Poor dance; 100 = caught. Patrol loop 20–40 s, stops 3–5 s (proposed). Weapon seen: +20/s (*proposed*, same as restricted). Guards per chapter: see `level-design.md` 13 (1 garden patrol + Guard Post; extra guards in Chapters 2, 4 and 5; none in Chapter 7).
- **Current state:** 🔲 Not started.
- **Required work (Phase 10, Baha + Merve):** guard Blueprint and routes, perception, restricted volumes, hiding spots, eavesdrop zones, follower component, eye icon events, checkpoint restart, Automation test for the meter.

## 4.4b Clues and Rumor Journal (must have)
- **GDD basis:** 1 Mechanics 6, 2 Mechanics 6.
- **Architecture:** clues come from four sources: LLM replies (`clues[]`, accepted only if the NPC holds that clue this chapter in `DT_Clues`), offline trees, eavesdrop zones and "do" objectives, and dance rewards. `UClueJournalSubsystem` stores them, adds topic replies and prompt hints, checks "requires clue" links and opens the key scene at the required count; delegate `OnClueAdded` drives the toast and `WBP_RumorJournal`.
- **Numbers & formulas:** 6 clues per palace chapter (36 in all), any 3 open the key scene (none needed in Chapters 6 and 7), 1–3 per NPC, toast 2 s (proposed).
- **Current state:** 🔲 Not started (the validator already checks clue ids).
- **Required work (Phase 9):** `DT_Clues` (36 rows), subsystem, journal widget, Automation tests (unknown clue, duplicate, gate, required-clue link).

## 4.5 Nightly Dance Minigame (must have)
- **GDD basis:** 1 Mechanics 7, 2 Mechanics 7.
- **Architecture:** `UDanceMinigameComponent` reads a `DT_DanceCharts` row per night; timing from a **Quartz** clock (`UQuartzSubsystem`) started with the song so notes stay in sync **(derived)**; `IMC_Dance` input; `WBP_Dance` draws the 4 lanes; result → Vashir's Favor, `USuspicionComponent`, `UClueJournalSubsystem` (reward clue). `UDayNightSubsystem` starts it at nightfall: Throne Hall (Chapters 1, 2, 4), King's Chamber (Chapter 3 private dance, Chapter 6 solo dance with a fixed outcome), Throne Hall duet with Zafir (Chapter 5).
- **Numbers & formulas:** 4 lanes ← ↑ ↓ → (proposed), 0.25 s windows, 30 s on night 1 rising by 3 s per night to 45 s on night 6, tempo +10% per night; > 80% → +10 Favor + reward clue; 40–80% → nothing; < 40% → +10 Suspicion. Chapter 6: no reward and no Suspicion.
- **Current state:** 🔲 Not started.
- **Required work (Phase 11, Baha code; Tala songs, charts and dance animations):** component, 6 chart rows, `WBP_Dance`, Sahra's dance montages, Automation test for grading.

## 4.5b Day / Night Chapter Flow (must have)
- **Architecture:** `UDayNightSubsystem` per chapter: Day (main quest, 2 "do" objectives, investigate) → nightfall when the required clues are found or the player chooses "Go to perform" (proposed) → Night (dance) → KeyScene (Scene Director) → in-engine ending → chapter card → next chapter. Switches the lighting (Directional Light / Sky Atmosphere + a night Post Process Volume, Lumen), the moon, the goal line, the `BP_NPCSpawnPoint` set (tags `Ch<N>.Day` / `Ch<N>.Night`) and chapter-tagged dressing (banners from Chapter 4, plum blossoms in Chapter 5, fewer lanterns in Chapter 3). Chapter 7 starts `USpiritWalkSubsystem` instead.
- **Current state:** 🔲 Not started.
- **Required work (Phase 8, Baha code; Merve lighting and spawn points):** phase state, day and night lighting setups, spawn sets, chapter dressing tags, save at chapter start.

## 4.6 Cutscenes and In-Engine Endings
- **GDD basis:** 3 Themes (Prologue, Epilogue), 1 Gameplay Walkthrough.
- **Architecture:** 2D visual-novel cutscenes as `WBP_Cutscene` (2D background, speaker, text, Space to advance) driven by `DT_Cutscenes`, for the Prologue and the Epilogue only. Every chapter ends in-engine with a short moment where the player keeps control (e.g. Chapter 1: walking back with the knife); `LS_Trap` is a Level Sequence in `L_Palace` for the Chapter 6 fall (guards stepping out, the fall, low camera). Epilogue lines change with the route and memories.
- **Current state:** 🔲 Not started.
- **Required work (Phases 14–15, Baha player; Tala sequences):** cutscene widget, `DT_Cutscenes` rows, 3 2D backgrounds (Tuana), `LS_Trap`, `LS_Prologue`, `LS_Epilogue`.

## 4.7 Offline Mode: Trees and Fallback Lines (must have, decided 12.11)
- **GDD basis:** 8.1, Pillar 4 (always playable).
- **Architecture:** `UOfflineLLMProvider` answers whenever the model is not ready, times out, fails twice or the player picks Offline in Settings. In a key scene it walks `DT_OfflineTrees` (each node: character line, emotion, 3 replies with value changes, beats, clues and the next node; a stand-in line when a beat's rule fails, e.g. Trust below 30 for `one_true_thing`). If the player typed free text, the closest of the node's 3 replies is chosen by keyword match (*proposed*); otherwise a fallback line is picked from `DT_FallbackLines`.
- **Fallback pick order:** scene + beat + emotion → clue → scene → any line of that character; never the same line twice in a row.
- **Numbers & formulas:** 142 fallback lines; 7 key-scene trees (Chapter 7 uses the pre-written confession); 34 side-character nodes (24 clue nodes = every talk clue, 10 small talk) + 12 topic and repeat-visit lines.
- **Current state:** 🟡 the provider returns a fixed fallback; the tables are written in the Master Lore doc (12.11) and not yet entered.
- **Required work (Phase 7, Baha; Merve enters data):** row structs, tree walker, pick order, Automation tests (every key-scene path reaches its required beats; no repeat).

## 4.8 Living Palace: Barks and the King's Servant (should have → must for Chapter 5)
- **GDD basis:** 1 Mechanics 9, 4 AI.
- **Architecture:** about 10 `BP_AmbientNPC` at chapter-tagged spawn points; E plays the chapter's bark from `DT_Barks` (one line, then they turn away); some barks point at real characters. `BP_KingsServant` has fixed lines (no LLM): takes the cord in the Chapter 4 morning, follows Sahra in Chapter 5, gives `ch5_servant_cord_stolen` when cornered (3-line exchange), absent in Chapter 6.
- **Current state:** 🔲 Not started.
- **Required work (Phase 13):** `UBarkComponent`, `DT_Barks` (60 + 7 + 3 rows), ambient spawn points, servant logic.

## 4.9 Chapter 7 Spirit Walk (must have)
- **GDD basis:** 1 Mechanics 10, 3 Theme 8.
- **Architecture:** `USpiritWalkSubsystem` (*proposed name*) hides all guards and NPCs, turns on the ghost post process (desaturated, blurred, ash rising; Tala) and the red glow of the Crimson Zenith, and spawns 6 `BP_SoulEmber` (fountain, practice room, Throne Hall, City Gate, Rooftop Terrace, King's Chamber; any order). On pickup: show that chapter's stored player line over a blurred glimpse (`DT_Cutscenes` caption), then request one confession piece from Zafir (scene `Ch7_ZafirConfession`, 1–3 sentences, quoting memories); if the model fails, play the matching piece of the pre-written 6-piece confession. After the 6th ember: the glow turns from red to gold, then the Epilogue.
- **Numbers & formulas:** 6 embers, 6 pieces, no guards, no fail state, no turn limit.
- **Current state:** 🔲 Not started.
- **Required work (Phase 14, Baha; Tala VFX):** subsystem, ember actor, ghost post process, confession calls with fallback, Automation test (6 pieces always play).

---

## 5. Input
> Enhanced Input. One Input Mapping Context per mode; `PC_Ashes` swaps them.

| Input Action | Mapping Context | Bound to |
| :-- | :-- | :-- |
| `IA_Move` (Axis2D) | `IMC_Exploration` | W A S D |
| `IA_Look` (Axis2D) | `IMC_Exploration` | Mouse XY |
| `IA_Run` (Digital, hold) | `IMC_Exploration` | Left Shift |
| `IA_Interact` (Digital) | `IMC_Exploration` | E (talk, bark, interact, enter / leave hiding spot, pick up ember) |
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
- **States:** Guards: Patrol → Notice → Patrol (simplified from the GDD's Patrol → Notice → Investigate → Alert; no chasing). Followers (Zafir's tail, the king's servant): scripted path / follow on the NavMesh. Conversation: Scene Director states (section 4.2). Emotions: per-character emotion tag → portrait in `WBP_Conversation`.
- **Perception:** `UAIPerceptionComponent` with Sight only: 70° total (35° peripheral half-angle), 900 cm radius, line of sight by the perception system; blind to hidden Sahra. No hearing (proposed). Notice duration: the guard stops while he sees Sahra and returns to the route 2 s after losing sight (*proposed*).
- **Classes:** `AGuardAIController`, `BT_Guard` / `BB_Guard`, `BP_Guard` (Character with a patrol route), `UFollowerComponent`, NavMesh Bounds Volume (Recast); `USceneDirectorSubsystem`.
- **Ambient NPCs:** no LLM; stand or walk short loops; one bark each.
- **Current state:** 🔲 Not started.
- **Required work:** guard Blueprint, behavior tree, patrol routes per chapter (`level-design.md`), follower component, suspicion hook.

---

## 7. UI System
- **Widgets:** UMG (follow `design.md` tokens): `WBP_Title`, `WBP_Settings`, `WBP_HUD` (goal line, 2 "do" objectives, eye icon, time-of-day tag, Chapter 7 ember counter), `WBP_Conversation`, `WBP_ChapterCard`, `WBP_RumorJournal`, `WBP_Dance`, `WBP_Pause`, `WBP_Cutscene`, `WBP_Credits`, `WBP_InteractPrompt`, bark bubble.
- **How UI gets data:** dynamic multicast delegates (`BlueprintAssignable`) from `USceneDirectorSubsystem`, `UNPCBrainComponent`, `UClueJournalSubsystem`, `USuspicionComponent`, `UDayNightSubsystem` and `USpiritWalkSubsystem`; no widget property bindings and no Tick polling.
- **Current state:** 🟡 `WBP_InteractPrompt` and `WBP_Conversation` (placeholder style) built; the rest not started.
- **Required work:** screen stack in `PC_Ashes`, final conversation style (Tuana), then HUD, journal, dance and menus.

---

## 8. Save & Progress
- **What is saved:** current chapter (checkpoint), relationship values, found clues, memories, the stored player line per chapter (for Chapter 7), the Chapter 2 hiding place (for the closing line), settings (text speed, volume, AI mode). The full conversation log is kept for the current chapter only (*proposed*).
- **When:** at the start of each chapter (checkpoint, Figure 3 "Next chapter: save").
- **Class:** `UAshesSaveGame` (USaveGame) with `UGameplayStatics::SaveGameToSlot` / `LoadGameFromSlot`; settings in a separate `UAshesSettingsSave` slot **(derived)**.

---

## 9. Audio (Tala)
- **Sound classes / mixes:** Sound Classes Music, SFX, UI under a Master class, one Sound Class Mix for the volume slider (GDD lists "volume"; separate sliders *proposed*) **(derived)**. Music as MetaSounds so the dance tempo can change per night.
- **Who plays which sound:** anim notifies on `ABP_Sahra` and `BP_Guard` for footsteps on stone; fire sounds on lantern and brazier Blueprints; the fountain loop in the garden; the nightfall bell and the midnight bell from `UDayNightSubsystem`; `WBP_*` for UI sounds; `UDayNightSubsystem` switches palace loop (day), dance song (night), romance loop (Zafir key scenes), the ghost palace loop (Chapter 7) and the finale (Epilogue).

---

## 10. Performance Rules
- Avoid Tick for gameplay and UI logic. Use events, timers (`FTimerManager`), delegates and interfaces. Tick is allowed only inside the dance lane rendering if Quartz callbacks are not enough.
- Every HTTP request is async with a 6 s timeout and never blocks the game thread.
- Turn limits keep the number of LLM calls per playthrough fixed (about 11 × 6 Zafir + 7 × 2 Vashir + side talks + 6 confession pieces); measure tokens per playthrough in week 2.
- The built-in model shares the GPU with the game; on 4 GB VRAM PCs it runs on the CPU (`ASHES_LLM_DEVICE=none`).
- Lumen + Virtual Shadow Maps on by default; frame-rate target 60 fps on the stronger PC and 30 fps on the laptop (*proposed*); minimum PC spec MISSING (both team PCs must run it).

---

## 11. Testing
- **Test map:** `L_Test` (every new mechanic is tested here first), then `L_Palace`.
- **How each mechanic is tested:**
  - Validator, JSON parsing, clamping, Scene Director transitions, clue gate, offline trees, dance grading, spirit walk pieces: C++ Automation specs in `Source/Ashes/Tests/`, run from Session Frontend → Automation (filter `Ashes`).
  - LLM characters: test harness (console commands) that sends scripted player lines; red-team 50 lines per character (off-topic, jailbreak, spoiler, rude); LLM judge flags spoilers and broken voice; read every flagged reply + 20 random. Targets: 0 spoiler leaks, 95% valid JSON.
  - No internet: the built-in model needs none; play every chapter offline. Offline mode: stop the model (`Ashes.LLM.Stop`) and check every scene still completes through the trees.
  - Movement, guards, UI: Play In Editor in `L_Test`, then a full run of `L_Palace`.
  - Playtests (led by Tala): 5 outside players (end of week 3), second playtest in week 4.

---

## 12. Open Decisions
| Question | Options | Decision |
| :-- | :-- | :-- |
| Engine | Unity 6.3 HDRP / Unreal Engine 5.8 | **Unreal Engine 5.8** (teacher approved, 10.11.2026) |
| What happens to the Unity skeleton in the repo | Replace in the same repo / new repo / keep for now | Decided 10.11.2026: replace in the same repo; the Unity version is kept as the Git tag `unity-skeleton` |
| C++ vs Blueprint split | All Blueprint / C++ core + Blueprint glue | C++ core + Blueprint glue (section 0.3) |
| Timeout | 6 s (GDD text) / 5 s (Figure 2) | 6 s in the game (text wins); 30 s in the editor only |
| LLM provider and model | Hosted API / free cloud tier / AI model shipped inside the game | **Built-in model shipped with the game** (decided 10.11.2026 with Merve: free, no account, no key, no internet; players download nothing else): llama.cpp `llama-server` b12138 (Vulkan, MIT) + Qwen3.5-4B Q4_K_M GGUF (Apache 2.0, 2.7 GB). Optional external OpenAI-compatible endpoint (Ollama / cloud) as a setting. MISSING: speed check on the stronger PC (laptop: ~20 s per reply on the CPU; target 6 s) |
| Full offline dialogue trees (`DT_OfflineTrees`) | Write a pre-written tree per scene / only fallback lines | **Decided 12.11.2026: full trees** for all 7 key scenes and every side character's clue, plus 142 fallback lines (Master Lore doc) |
| Editor access for the AI agent | MCP + VibeUE / user does editor work | MCP + VibeUE; installed as submodule `Plugins/VibeUE`, working on Merve's PC (10.11.2026); MISSING: Tala's and Baha's PCs |
| Side characters (Nura, Idris, Omar, Ilyas, Tariq, Aisha) as LLM | LLM / pre-written | **Decided 11.11.2026: all LLM characters** (5 turns), with offline nodes for every clue |
| Turn limits | 10 / 8 / 4 (old) / new | **Decided 12.11.2026:** Zafir 11, Vashir 7, others 5, ambient 1 line |
| Dance minigame | Minigame / cutscene | Minigame every night (10.11) |
| Guard patrols | Full FSM / simple patrol + perception | Simple patrol + perception + hiding spots, no chasing (10.11) |
| Nightfall trigger | Required clues found / player chooses / timer | Clues found or player chooses (proposed) |
| Vashir JSON fields | `favor_delta`, `suspicion_delta` | **Decided 11.11.2026:** one `<value>_delta` per value of the card (implemented in Phase 4) |
| Romance closing line | New `ClosingRomance` field / reuse ClosingFull | Proposed 12.11.2026: new field in `FSceneCardRow` (Baha) |
| Chapter 7 class name | `USpiritWalkSubsystem` / other | Proposed 12.11.2026 |

---

## Section Status
| Section | Status |
| :-- | :-- |
| 0. Meta & Conventions | FILLED (12.11: team and ownership) |
| 1. Project Architecture | FILLED (12.11: new classes for barks, followers, embers) |
| 2. Core Types & Data | FILLED (12.11: DT_Barks, DT_OfflineTrees, 142 fallback lines, 36 clues, 6 dance charts) |
| 3. Player System | FILLED |
| 4. Mechanic systems (4–4.9) | FILLED (*proposed* values marked) |
| 5. Input | FILLED (dance keys proposed) |
| 6. AI | FILLED (Notice duration proposed) |
| 7. UI System | FILLED |
| 8. Save & Progress | FILLED (derived; confirm) |
| 9. Audio | FILLED (derived) |
| 10. Performance | FILLED (MISSING: minimum PC spec) |
| 11. Testing | FILLED |
| 12. Open Decisions | FILLED (MISSING: speed on the stronger PC) |
