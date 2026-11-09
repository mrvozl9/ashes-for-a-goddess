# Ashes for a Goddess · Technical Design Document (technical_design.md)

> **Canonical design source:** `gdd.md`. This document turns the GDD into a concrete Unity C# architecture. **Where this document and the GDD disagree, the GDD wins** and this document must be corrected.
> **Engine target:** Unity 6.3 LTS (6000.3.27f2), HDRP, C#. (The course template targets Unreal Engine 5.8; this project is Unity, see `gdd.md` header.)
> **Companion documents:** `level-design.md` (where things are) and `production_pipeline.md` (build order).

**Status legend:** `✅ Built` · `🟡 Partial` · `🔲 Not started` · `♻️ Built but must change`

**Source legend:** class names and their jobs come from GDD Figure 6 (Main scripts). Items marked **(derived)** are architecture details this document adds to make the GDD buildable; they are not in the GDD and can be changed freely.

Repository state checked on 09.11.2026 (`main` at `2090454`): Unity 6.3 project, HDRP default resources created, packages installed, `Assets/_Project/` folders created, Active Input Handling = Input System. No scenes and no scripts yet.

---

## 0. Meta & Conventions

### 0.1 How to read this document
Every system section uses the same structure: **GDD basis → Architecture → Class spec → Current state → Required work.**

### 0.2 Naming Conventions
- **C# classes:** PascalCase, one class per file, file name = class name (GDD 13.1). Interfaces start with `I` (`ILLMProvider`). Enums and structs use PascalCase without prefix **(derived)**.
- **Assets (GDD 13.1):** `P_` prefabs, `M_` materials, `T_` textures, `SO_` ScriptableObjects. Other prefixes: see `design.md` section 9 (MISSING there).
- **Scenes:** `Boot`, `Title`, `Palace`, `Cutscenes` (GDD 13.1) plus a test scene `Test` **(derived, Unity version of the template's `L_Test`)**.
- **Namespaces:** MISSING: namespace rule (suggested: `Ashes.Core`, `Ashes.Dialogue`, ...).
- **AI-assisted header:** every AI-assisted script starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.`
- **Project prefix:** none in the GDD; MISSING if wanted.

### 0.3 Tools & workflow
- **C# or visual scripting:** all game logic in C# (GDD 1, 13). Visuals in the Animator, Timeline (cutscenes), VFX Graph / particle systems and uGUI + TextMeshPro.
- **AI agent:** GitHub Copilot in the IDE; Claude for design questions, error messages and these docs (GDD 12.2). Claude Code works on C# scripts and text files in the repo. MISSING: whether a Unity editor bridge (MCP) is used; without one, the user does scene and prefab work in the editor (the template's VibeUE is Unreal-only).
- **Version control:** Git + Git LFS (art, audio, models, fonts, video). `main` always opens and runs. Feature branches `feature/<name>-<task>`, pull request, review by the other member, merge only when the Palace scene runs. Asset Serialization = Force Text, visible meta files.

---

## 1. Project Architecture

### 1.1 Folder layout
```
Assets/_Project/
  Art/        Models, Materials, Portraits, VFX
  Audio/      Music, SFX
  Data/       Characters, Scenes, Lore (ScriptableObjects and text)
  Prefabs/
  Scenes/     Boot, Title, Palace, Cutscenes (+ Test)
  Scripts/
    Core/       GameManager, SaveSystem, Settings
    Player/     PlayerController, InteractionSystem
    Dialogue/   SceneDirector, NPCBrain, PromptBuilder, ResponseValidator, providers
    AI/         emotion state machine, memory summarizer
    UI/         UIManager, ConversationUI, HUD, PortraitAnimator, menus
    Guards/     GuardFSM, SuspicionMeter
  Tests/      EditMode (validator, parser, clamping)
Docs/
Secrets/      (ignored by git) API key file
```

### 1.2 Core classes
Three layers (GDD Figure 6). Gameplay sends the chapter to Conversation AI; Conversation AI sends what to show to Presentation. Game code reaches the LLM only through `ILLMProvider`.

| Class | Parent | Responsibility |
| :-- | :-- | :-- |
| `GameManager` | MonoBehaviour (persists from `Boot`) | Chapters, checkpoints, room unlocking, online / offline mode, win (end of epilogue) and lose (Suspicion 100 → restart chapter) |
| `PlayerController` | MonoBehaviour | Movement (walk / run) and Cinemachine camera |
| `InteractionSystem` | MonoBehaviour | Highlights characters and objects; E starts a conversation scene |
| `GuardFSM` | MonoBehaviour | Patrol, notice, investigate, alert (should have) |
| `SceneDirector` | MonoBehaviour | Conversation FSM: intro, free talk, nudge, resolve, forced close, caught |
| `NPCBrain` | MonoBehaviour (one per character) | Character profile, relationship values, memories |
| `PromptBuilder` | Plain C# class | Assembles card, lore, scene, state and turns into one request |
| `ILLMProvider` | C# interface | `Cloud` / `Local` / `Offline script` implementations |
| `ResponseValidator` | Plain C# class | JSON, lore, spoiler, safety checks; clamps value changes |
| `UIManager` | MonoBehaviour | Owns every screen |
| `ConversationUI` | MonoBehaviour | Text, input, 3 replies |
| `HUD` | MonoBehaviour | Goal, eye icon, prompts |
| `PortraitAnimator` | MonoBehaviour | Emotion tag to portrait |

---

## 2. Core Types & Data
### 2.1 Enums
| Enum | Values | Used by |
| :-- | :-- | :-- |
| `ChapterId` **(derived)** | Prologue, GardenOfAsh, PrivateDance, RooftopOfSmoke, TheTrap, ThePyre, Epilogue | `GameManager`, `SceneDirector` |
| `DirectorState` | Intro, FreeTalk, Nudge, Resolve, ForcedClose, Caught, NextChapter | `SceneDirector` |
| `ZafirEmotion` | Neutral, Teasing, Tender, Jealous, Cold, Angry (fallback Neutral) | `NPCBrain`, `PortraitAnimator` |
| `VashirEmotion` | Serene, Curious, Pleased, Suspicious, Wrathful (fallback Serene) | `NPCBrain`, `PortraitAnimator` |
| Nura / Idris emotions | MISSING: their 4–6 emotion names | `PortraitAnimator` |
| `GuardState` | Patrol, Notice, Investigate, Alert | `GuardFSM` |
| `AIMode` | Online, Offline | `GameManager`, Settings |
| `RoomId` **(derived)** | DancersQuarters, GardenOfAsh, GuardPost, ThroneHall, KingsChamber, RooftopTerrace | `GameManager`, `GuardFSM` |

### 2.2 Structs & DataTables
> Unity ScriptableObjects (`SO_`) replace Unreal DataTables.

| Struct / Table | Fields | Used by |
| :-- | :-- | :-- |
| `CharacterCard` (SO) | Name, voice, wants, fears, speech rules, knows, must not say, secrets per chapter, emotion list, fallback emotion, turn limit, relationship value definitions | `NPCBrain`, `PromptBuilder` |
| `RelationshipValue` (struct) | Name (Trust / Affection / Favor / Suspicion), start, current 0–100, max increase per turn, max decrease per turn, thresholds with unlock ids | `NPCBrain`, `ResponseValidator` |
| `SceneCard` (SO) | Chapter, place, time, goal text, beats (id, rule, required), turn limit, characters, authored opener, closing lines (full / weaker) | `SceneDirector`, `PromptBuilder` |
| `LoreBible` (TextAsset) | ~600-word text | `PromptBuilder` |
| `LLMResponse` (C# class for Newtonsoft) | `reply`, `emotion`, `gesture`, `trust_delta`, `affection_delta`, `beats[]`, `suggestions[3]`, `off_topic` (MISSING: Vashir's field names, presumably `favor_delta`, `suspicion_delta`) | `ResponseValidator`, `SceneDirector` |
| `Memory` (struct) | Character, text, chapter | `NPCBrain` (max 6) |
| `FallbackLine` (SO table) | Scene, beat, emotion, line, AI-assisted flag (~120) | `OfflineScriptProvider` |
| `OfflineDialogueTree` (SO) | Scene, nodes (3 replies → character line, value changes, beats) | `OfflineScriptProvider` |
| `CutsceneScript` (SO / Timeline) | Part, background, speaker, lines (~150 total) | Cutscene player |
| `ExplorationSettings` (SO) | Walk 2.2, run 4.5, cone 70° / 9 m, +12/s run, +20/s restricted, −5/s out of view | `PlayerController`, `GuardFSM` |
| `ConversationSettings` (SO) | 200 chars, 3 suggestions, 80 words, 6 s timeout, 0.4 s typing, last 8 turns, 6 memories | `ConversationUI`, `PromptBuilder`, providers |

---

## 3. Player System
- **GDD basis:** 2 Controls, 2 Camera, 2 Mechanics 5 (Exploration).
- **Architecture:** `P_Sahra` prefab with `CharacterController` **(derived)**, `PlayerController`, `InteractionSystem`, Animator; Cinemachine 3 orbital follow camera; input from the Input System action asset.
- **Class spec:** `PlayerController`: `walkSpeed = 2.2f`, `runSpeed = 4.5f`, `IsRunning` (read by guards), `SetInputEnabled(bool)` (off during conversations, cutscenes, pause). `InteractionSystem`: detection radius (MISSING), current target, event `OnInteract(Interactable)`; `Interactable` component on characters and objects **(derived)**.
- **Movement values:** walk 2.2 m/s, run 4.5 m/s. Jump: none. Gravity: Unity default (MISSING: confirm). Camera distance and limits: MISSING.
- **Current state:** 🔲 Not started.
- **Required work:** input actions, controller, camera rig, interaction prompt, test in `Test` scene.

---

## 4. Conversation System (Free Conversation)
- **GDD basis:** 1 Mechanics 1, 2 Mechanics 1, 8.1.
- **Architecture:** `InteractionSystem` → `SceneDirector.StartScene(SceneCard)` → per turn: `ConversationUI` gets text → `PromptBuilder.Build(card, lore, scene, state, last 8 turns, line)` → `ILLMProvider.SendAsync(request, 6 s)` → `ResponseValidator.Validate` → `SceneDirector.Apply` → `ConversationUI.Show` + `PortraitAnimator.SetEmotion`.
- **Class spec:** `ILLMProvider { Task<string> SendAsync(LLMRequest, CancellationToken) }`. `CloudProvider`: UnityWebRequest with async/await, 6 s timeout, key from environment variable or `Secrets/` file. `OfflineScriptProvider`: returns pre-written trees and fallback lines. `ResponseValidator.Validate(string json) → ValidationResult` (parse, emotion known, ≤ 80 words, off-topic, spoiler keywords, safety word list, clamp deltas).
- **Numbers & formulas:** 200 characters input; 3 suggestions; 1–4 sentences / 80 words; 6 s timeout; 1 silent retry on broken JSON; regenerate once on spoiler; 3 unsafe inputs close the scene; typing indicator ≥ 0.4 s; portrait change ≤ 1 s.
- **Current state:** 🔲 Not started.
- **Required work:** provider interface + cloud + offline providers; prompt builder; validator with Edit Mode tests; conversation UI; LLM provider choice and model (GDD: a fast low-cost hosted model, for example Claude Haiku 5.5).

## 4.1 Relationship Values System
- **GDD basis:** 1 Mechanics 2, 2 Mechanics 2.
- **Architecture:** `NPCBrain` holds `RelationshipValue`s; `SceneDirector` applies the validated deltas and fires threshold events.
- **Class spec:** `NPCBrain.ApplyDelta(valueName, delta)`, `GetWord(valueName)` (for the prompt, for example "Trust: wary"; MISSING: the word bands), event `OnThreshold(id)`.
- **Numbers & formulas:** `new = clamp(current + clamp(delta, -maxDown, +maxUp), 0, 100)`. Zafir Trust 20 (±8; 50, 70), Affection 10 (±8; 60). Vashir Favor 30 (±6; 60), Suspicion 10 (+10 / −5; 100 = caught).
- **Current state:** 🔲 Not started.
- **Required work:** values, clamping, threshold events, hint lines (MISSING: list of hint lines per change), Edit Mode tests.

## 4.2 Scene Director System
- **GDD basis:** 1 Mechanics 3, 2 Mechanics 3, Figure 3.
- **Architecture:** FSM: Intro (authored opener) → FreeTalk → Nudge (2 turns left or 3 stalled turns; adds a steering note to the prompt) → FreeTalk; all beats met → Resolve (closing line) → NextChapter (save, cutscene); turn limit → ForcedClose (weaker beat) → NextChapter; Suspicion 100 → Caught → restart chapter.
- **Class spec:** `StartScene`, `OnTurn(ValidationResult)`, `AcceptBeat(id)` (checks the beat's rule), events `OnSceneResolved(bool full)`, `OnCaught`.
- **Numbers & formulas:** turn limits 10 / 8 (4 in Ch4) / 4; nudge at limit − 2 or 3 turns without a new beat; 2–3 beats per scene.
- **Current state:** 🔲 Not started.
- **Required work:** FSM, beat rules per scene card, nudge notes (MISSING: nudge text per scene), Edit Mode tests for transitions.

## 4.3 Memory System (should have)
- **GDD basis:** 1 Mechanics 4, 2 Mechanics 4.
- **Architecture:** after `OnSceneResolved`, one summary call through `ILLMProvider` writes 1–2 memories per character into `NPCBrain`; memories go into the prompt state and the pyre confession.
- **Numbers & formulas:** 6 kept per character (oldest dropped); 1–2 per scene; last 8 turns in full.
- **Current state:** 🔲 Not started.
- **Required work:** summary prompt, storage, Log screen display, pyre confession call with 2 memories. MISSING: behaviour when the summary call fails.

## 4.4 Exploration and Suspicion System (should have for guards)
- **GDD basis:** 1 Mechanics 5, 2 Mechanics 5, 4 AI.
- **Architecture:** `SuspicionMeter` **(derived)** on `GameManager` sums Suspicion from `GuardFSM` sight checks and Vashir's `suspicion_delta`; restricted rooms are trigger volumes **(derived)**.
- **Numbers & formulas:** +12/s running in view; +20/s restricted room in view; −5/s out of view; 100 = caught. MISSING: weapon-seen rate.
- **Current state:** 🔲 Not started.
- **Required work:** view cone check, restricted volumes, eye icon events, checkpoint restart.

## 4.5 Dance Minigame (should have; cut first)
- **GDD basis:** 1 Mechanics 6, 2 Mechanics 6.
- **Architecture:** standalone UI minigame **(derived)**; result → `NPCBrain(Vashir)`.
- **Numbers & formulas:** 30 s, 4 keys (MISSING: which), 0.25 s windows; > 80% → +10 Favor; < 40% → +10 Suspicion.
- **Current state:** 🔲 Not started.
- **Required work:** only if time allows in week 3; otherwise a cutscene.

## 4.6 Cutscene System
- **GDD basis:** 3 Themes (Prologue, Pyre, Epilogue), 1 Gameplay Walkthrough.
- **Architecture:** visual novel cutscenes with a 2D background and text, Space to advance; Timeline where camera work is needed **(derived)**; the pyre uses generated lines with memories (should have; pre-written fallback).
- **Current state:** 🔲 Not started.
- **Required work:** cutscene player, scripts (~150 lines), backgrounds.

---

## 5. Input
> Unity Input System action asset **(derived name: `AshesInput.inputactions`)**. Action maps replace Unreal mapping contexts.

| Input Action | Mapping Context | Bound to |
| :-- | :-- | :-- |
| `Move` | `Exploration` | W A S D |
| `Look` | `Exploration` | Mouse delta |
| `Run` | `Exploration` | Left Shift (hold) |
| `Interact` | `Exploration` | E |
| `Submit` | `Conversation` | Enter |
| `PickReply1` / `PickReply2` / `PickReply3` | `Conversation` | 1 / 2 / 3 |
| `OpenLog` | `Exploration`, `Conversation` | Tab |
| `Advance` | `Cutscene` | Space |
| `Pause` | `Exploration`, `Conversation`, `Cutscene` | Esc |
| `DanceKey1–4` | `Dance` | MISSING: dance keys |
| `Jump` | none | Not in the game |

Text entry uses a TextMeshPro input field; the `Exploration` map is disabled while typing so W A S D do not move Sahra **(derived)**.

---

## 6. AI
- **States:** Guards: Patrol → Notice → Investigate → Alert (GDD 5.1). Conversation: Scene Director states (section 4.2). Emotions: per-character emotion state machine driven by the LLM's emotion tag.
- **Perception:** sight only: 70° cone, 9 m range. MISSING: hearing, Notice and Investigate timings, line-of-sight rule.
- **Classes:** `GuardFSM` + `NavMeshAgent` with patrol waypoints; NavMesh baked with AI Navigation; `SceneDirector`; `PortraitAnimator`.
- **Current state:** 🔲 Not started.
- **Required work:** guard prefab, patrol routes in the garden (2 guards Ch1, 3 guards Ch2), suspicion hook.

---

## 7. UI System
- **Widgets:** uGUI + TextMeshPro prefabs (follow `design.md` tokens): Title, Settings, Exploration HUD, Conversation, Chapter card, Log and memories, Pause, Cutscene, Credits.
- **How UI gets data:** C# events / `UnityEvent`s from `GameManager`, `SceneDirector`, `NPCBrain` and `SuspicionMeter`; never polling in `Update`.
- **Current state:** 🔲 Not started.
- **Required work:** `UIManager` screen stack, conversation screen first (Figure 5), then HUD and menus.

---

## 8. Save & Progress
- **What is saved:** current chapter (checkpoint), relationship values, memories, settings (text speed, volume, AI mode). MISSING: confirm the save content and whether the conversation log is saved.
- **When:** at the start of each chapter (checkpoint, Figure 3 "Next chapter: save").
- **Class:** `SaveSystem` **(derived)**: JSON (Newtonsoft) file in `Application.persistentDataPath`; settings in `PlayerPrefs` **(derived)**.

---

## 9. Audio
- **Sound classes / mixes:** Unity Audio Mixer **(derived)** with groups Music, SFX, UI; one master volume slider in Settings (GDD lists "volume"; MISSING: separate sliders).
- **Who plays which sound:** `PlayerController` footsteps on stone; fire sources on lanterns and braziers; `ConversationUI` UI sounds (MISSING in `design.md`); music manager **(derived)** switches palace loop (exploration), romance loop (Zafir scenes, rooftop), finale (pyre, epilogue).

---

## 10. Performance Rules
- Avoid `Update` for gameplay and UI logic. Use events, coroutines / async, timers and delegates.
- Every network call is async with a 6 s timeout and never blocks the main thread.
- Turn limits keep the number of LLM calls per playthrough fixed; measure tokens per playthrough in week 2.
- MISSING: frame-rate target and minimum PC spec for HDRP.

---

## 11. Testing
- **Test map:** `Test` scene (every new mechanic is tested here first), then `Palace`.
- **How each mechanic is tested:**
  - Validator, JSON parser, clamping, Scene Director transitions: Unity Test Framework Edit Mode tests in `Assets/_Project/Tests/EditMode`.
  - LLM characters: test harness that sends scripted player lines; red-team 50 lines per character (off-topic, jailbreak, spoiler, rude); LLM judge flags spoilers and broken voice; read every flagged reply + 20 random. Targets: 0 spoiler leaks, 95% valid JSON.
  - Offline mode: play every chapter with no key and no internet.
  - Movement, guards, UI: Play Mode in `Test`, then a full run of `Palace`.
  - Playtests: 5 outside players (end of week 3), second playtest in week 4.

---

## 12. Open Decisions
| Question | Options | Decision |
| :-- | :-- | :-- |
| Unreal templates vs Unity project | Adapt docs to Unity / switch engine | Unity 6.3 HDRP (GDD and repo); docs adapted |
| Engine version text in GDD | 6000.0.x (GDD) / 6000.3.27f2 (repo) | 6000.3.27f2 since 03.11.2026; update the GDD |
| Timeout | 6 s (GDD text) / 5 s (Figure 2) | 6 s (text wins) |
| LLM provider and model | Hosted low-cost model (for example Claude Haiku 5.5) | MISSING: final choice |
| Unity editor access for the AI agent | Unity MCP bridge / user does editor work | MISSING |
| Court NPCs (Nura, Idris) as LLM | LLM / pre-written | Pre-written first; LLM if week 2 risk check passes |
| Dance minigame | Minigame / cutscene | Cutscene unless week 3 has time |
| Guard patrols | Full FSM / simple patrol + cone | MISSING |
| Vashir JSON fields | `favor_delta`, `suspicion_delta` | MISSING |

---

## Section Status
| Section | Status |
| :-- | :-- |
| 0. Meta & Conventions | FILLED (MISSING: namespaces, editor bridge) |
| 1. Project Architecture | FILLED |
| 2. Core Types & Data | FILLED (MISSING: Nura/Idris emotions, Vashir JSON fields) |
| 3. Player System | FILLED (MISSING: camera values, interaction radius) |
| 4. Mechanic systems (4–4.6) | FILLED (MISSING items listed per system) |
| 5. Input | FILLED (MISSING: dance keys) |
| 6. AI | FILLED (MISSING: hearing, timings) |
| 7. UI System | FILLED |
| 8. Save & Progress | FILLED (derived; confirm) |
| 9. Audio | FILLED (derived mixer) |
| 10. Performance | FILLED (MISSING: fps target) |
| 11. Testing | FILLED |
| 12. Open Decisions | FILLED |
