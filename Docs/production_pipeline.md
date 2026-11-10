# Ashes for a Goddess · Production Pipeline (State Machine)

> **Canonical design source:** `gdd.md`. Companions: `technical_design.md` (how the code works) and `level-design.md` (where things are). This file is the **order of operations**: small phases, in order, each with a hard check before the next one starts.
>
> **How to use this file:** treat each Phase as a node in a state machine. A phase may not start until its `Depends on` phases are `DONE`. Every phase ends with a **Verification Gate**: the feature is tested in Unity **Play Mode** and the user confirms before moving on. Never build UI before the gameplay it shows. Never build a level before its mechanics work in the `Test` scene.
>
> **Time box: 4 weeks in total, Monday 09.11.2026 to Sunday 06.12.2026.** Final build on Saturday 05.12.2026; Sunday 06.12 is buffer only. This compresses the GDD's Weeks 9–14 plan (which ended 20.12.2026). Owners follow the GDD roles: **Merve** (code, level, audio) and **Tuana** (art, UI, writing, programming). Two tracks run in parallel; a phase lists who owns it.
>
> **Updated 10.11.2026 for the agreed gameplay:** each chapter is a day and a night in the open palace. Day = gather clues into the **Rumor Journal** while **avoiding patrolling guards** (hiding spots, eavesdropping). Night = the **nightly dance** minigame to keep Sahra's cover. Then the key conversation and a cutscene. Guards and the dance are now must-haves.

**Legend:** `[C#]` code · `[UNITY]` scene / prefab / editor work · `[ART]` assets · `[UI]` interface · `[AUDIO]` sound · `[WRITE]` lore, cards, lines · `⚠️` risk / decision

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

**Stage 1 goal:** Sahra walks and talks in a test scene; one conversation with Zafir (Chapter 1) works end to end, online through the LLM and offline through pre-written lines, with the validator in between.
**Stage 2 goal:** Week 2 builds every gameplay system and finishes Chapter 1 as a complete day and night (clues and Rumor Journal, guards with hiding spots and eavesdropping, nightly dance, key scene, cutscene). Week 3 fills Chapters 2–4, the cutscenes (prologue, pyre, epilogue), HUD and menus, offline mode for every scene. First playtest with outside players.
**Stage 3 goal:** Art and audio pass, tuning from playtests (dance difficulty, patrol timing, clue counts), red-team pass with 0 spoiler leaks, Windows build that starts in offline mode without a key, credits and AI disclosure.

### Scope for 4 weeks
| Priority | Feature | In the 4-week plan |
| :-- | :-- | :-- |
| Must | Open palace (hub + 5 rooms), third-person movement and camera | Yes (grey-box week 2, art week 4) |
| Must | LLM conversations with Zafir and Vashir (prompt builder, validator, clamped values) | Yes (weeks 1–2) |
| Must | Scene Director with nudge and forced close | Yes (week 2) |
| Must (new 09.11) | Clues and Rumor Journal; key scene locked until enough clues | Yes (week 2) |
| Must (raised 10.11) | Guards on timed routes, Suspicion, hiding spots, eavesdropping | Yes (week 2), simplified: no chasing |
| Must (raised 10.11) | Nightly dance minigame, one per chapter, rewards and Suspicion | Yes (week 2 system, week 3 songs) |
| Must | Day / night chapter flow, offline mode for every scene | Yes (weeks 2–3) |
| Must | 4 chapters + prologue, pyre, epilogue cutscenes | Yes (week 3) |
| Must | Conversation UI, exploration HUD, title, pause, chapter cards | Yes (weeks 1–3) |
| Must | Placeholder portraits, basic fire VFX, 1 music loop + 1 dance song per night | Yes |
| Should | Memories + generated pyre confession | Week 3 if the Phase 12 risk check passes |
| Should | Nura and Idris as LLM characters | ⚠️ Pre-written by default (minor NPCs always pre-written) |
| Should | Final portraits, 3 music loops, phoenix-rise VFX | Week 4 if time |
| Nice | Local model, TTS for Zafir, varying epilogue lines, proxy server | ⚠️ Cut (varying epilogue lines only if memories land early) |

### Cut order if a phase slips more than one day
1. Generated pyre confession and memories (pre-written pyre).
2. Chapter 3 clue count from 3 required to 2; Chapter 4 day phase shortened to a walk to the King's Chamber.
3. Dance songs: reuse one song with a faster tempo each night instead of 4 songs.
4. Eavesdrop zones: keep 1 per chapter.
5. Final portraits and custom models (placeholders stay).

---

## PROGRESS TRACKER
> Update this table after every work session. Status: `⬜ NOT STARTED` · `🔄 IN PROGRESS` · `✅ DONE` · `⏸️ BLOCKED`.
> When you resume, start from the first row that is not `✅`.
> **GitHub:** every phase is an issue on the project board: Phase N = issue #(N+2) (Phase 0 = #2 … Phase 22 = #24), labelled by week and owner, with the day-by-day tasks as checkboxes. Close the issue when its gate passes (or put `Closes #N` in the PR). The day-by-day view is `Docs/workplan/index.html`.

| # | Phase | Stage | Owner | Target | Status | Last update | Notes |
| :-- | :-- | :-- | :-- | :-- | :-- | :-- | :-- |
| 0 | Project setup & test scene | 1 | Merve | Mon 09.11 | 🔄 IN PROGRESS | 09.11.2026 | HDRP, packages, folders, Input System done; scenes not created yet |
| 1 | Player movement & camera | 1 | Merve | Tue 10.11 | ⬜ NOT STARTED | | |
| 2 | Interaction system (E: Talk) | 1 | Merve | Wed 11.11 | ⬜ NOT STARTED | | |
| 3 | LLM provider (cloud + offline) | 1 | Merve | Wed 11.11 | ⬜ NOT STARTED | | |
| 4 | Prompt builder & validator + lore and Zafir's card | 1 | Merve + Tuana | Fri 13.11 | ⬜ NOT STARTED | | |
| 5 | Conversation UI + Chapter 1 Zafir scene | 1 | Tuana + Merve | Sat 14.11 | ⬜ NOT STARTED | | |
| 6 | 🏁 Prototype gate | 1 | Both | Sun 15.11 | ⬜ NOT STARTED | | user sign-off |
| 7 | Scene Director & relationship values | 2 | Merve | Tue 17.11 | ⬜ NOT STARTED | | |
| 8 | Open palace grey-box & day / night flow | 2 | Merve | Wed 18.11 | ⬜ NOT STARTED | | |
| 9 | Clues & Rumor Journal | 2 | Merve + Tuana | Thu 19.11 | ⬜ NOT STARTED | | |
| 10 | Guards, hiding spots & eavesdropping | 2 | Merve | Sat 21.11 | ⬜ NOT STARTED | | |
| 11 | Nightly dance minigame | 2 | Tuana | Sat 21.11 | ⬜ NOT STARTED | | |
| 12 | ⚠️ Chapter 1 complete + risk check | 2 | Both | Sun 22.11 | ⬜ NOT STARTED | | decides should-haves |
| 13 | Chapters 2–4 content | 2 | Tuana + Merve | Wed 25.11 | ⬜ NOT STARTED | | |
| 14 | Cutscene system + 2D parts | 2 | Merve + Tuana | Wed 25.11 | ⬜ NOT STARTED | | |
| 15 | HUD, menus, chapter cards | 2 | Tuana | Thu 26.11 | ⬜ NOT STARTED | | |
| 16 | Memories + pyre confession (should) | 2 | Merve | Fri 27.11 | ⬜ NOT STARTED | | |
| 17 | 🏁 Content gate + playtest 1 | 2 | Both | Sun 29.11 | ⬜ NOT STARTED | | user sign-off |
| 18 | Art pass (palace kit, portraits, VFX) | 3 | Tuana | Tue 01.12 | ⬜ NOT STARTED | | |
| 19 | Audio pass | 3 | Merve | Tue 01.12 | ⬜ NOT STARTED | | |
| 20 | Tuning, playtest 2, red-team pass | 3 | Both | Thu 03.12 | ⬜ NOT STARTED | | |
| 21 | Credits, disclosure, licenses, prompt log | 3 | Tuana | Fri 04.12 | ⬜ NOT STARTED | | |
| 22 | 🏁 Packaged Windows build | 3 | Merve | Sat 05.12 | ⬜ NOT STARTED | | final |

---

# STAGE 1: PROTOTYPE (Week 1 · 09.11 – 15.11)

## PHASE 0: Project Setup & Test Scene
**Objective:** A project both members open on Unity 6.3 LTS (6000.3.27f2) with `Boot`, `Test` and `Palace` scenes that run.
**Depends on:** none.

**Tasks:**
- ✅ [UNITY] HDRP Wizard "Fix All", HDRP default resources committed.
- ✅ [UNITY] Packages: HDRP, Input System, Cinemachine, AI Navigation, Timeline, uGUI/TMP, Newtonsoft Json, VFX Graph, Test Framework; Active Input Handling = Input System.
- ✅ [UNITY] `Assets/_Project/` folder layout.
- [UNITY] Create `Boot`, `Test`, `Palace` (and later `Title`, `Cutscenes`) in `Assets/_Project/Scenes/` and add them to Build Profiles.
- [UNITY] `Test` scene: floor, a few walls and a ramp, HDRP sky and a night volume.
- [C#] Assembly definitions for `Scripts` and `Tests/EditMode` (derived) so Edit Mode tests compile.
- [UNITY] Both members clone with Git LFS and open the project on the same version.
- [WRITE] Tuana: start the GitHub Projects task board (To Do, Doing, Review, Done) with this tracker's phases.

**Deliverable:** `main` opens and the `Boot` and `Test` scenes play with no errors on both PCs.
**Verification Gate:** Open the project on both PCs, press Play in `Test`, Console shows no errors. ✋ Confirm with the user before the next phase.

---

## PHASE 1: Player Movement & Camera
**Objective:** Sahra walks and runs in third person with an orbit camera.
**Depends on:** Phase 0.

**Tasks:**
- [UNITY] Input action asset: maps `Exploration` (Move, Look, Run, Interact, OpenLog, Pause), `Conversation`, `Cutscene`.
- [C#] `PlayerController`: walk 2.2 m/s, run 4.5 m/s (Shift), `IsRunning`, `SetInputEnabled`.
- [UNITY] `P_Sahra` with a placeholder capsule or licensed base model + Animator (idle, walk, run).
- [UNITY] Cinemachine orbital follow camera (MISSING in GDD: distance and limits; pick and record in `technical_design.md`).

**Deliverable:** `P_Sahra` moving in `Test`.
**Verification Gate:** In Play Mode, W A S D walks at about 2.2 m/s, Shift runs at about 4.5 m/s, mouse orbits the camera, no jump. ✋ Confirm with the user.

---

## PHASE 2: Interaction System (E: Talk)
**Objective:** Pressing E near a highlighted character starts a conversation event.
**Depends on:** Phase 1.

**Tasks:**
- [C#] `InteractionSystem` + `Interactable` component; event `OnInteract`.
- [UI] World-space "E: Talk" prompt (placeholder style).
- [UNITY] Placeholder Zafir capsule in `Test`.

**Deliverable:** E on the placeholder Zafir logs "Start scene: Zafir" and disables movement.
**Verification Gate:** Walk up to Zafir, prompt appears, E starts the event, walking away hides the prompt. ✋ Confirm with the user.

---

## PHASE 3: LLM Provider (Cloud + Offline)
**Objective:** One LLM request is answered in the Console, and the same call works offline.
**Depends on:** Phase 0 (can run in parallel with Phases 1–2).

**Tasks:**
- [C#] `ILLMProvider`, `CloudProvider` (UnityWebRequest, async/await, 6 s timeout), `OfflineScriptProvider` (returns a canned JSON).
- [C#] API key from an environment variable or `Secrets/` (git-ignored); startup check → AI mode Online / Offline.
- ⚠️ Choose the provider and model (GDD: fast low-cost hosted model, for example Claude Haiku 5.5); set a spending cap in the provider console.

**Deliverable:** A debug button sends Zafir's test prompt (GDD 12.4) and prints the JSON; without a key it prints the offline reply.
**Verification Gate:** Run with key → reply in under 6 s; run without key → offline reply; unplug network → timeout path, no freeze. ✋ Confirm with the user.

---

## PHASE 4: Prompt Builder, Validator, Lore Bible and Zafir's Card
**Objective:** Requests are built from the five parts and every reply is validated and clamped.
**Depends on:** Phase 3.

**Tasks:**
- [WRITE] Tuana: lore bible (~600 words, hand-written); Zafir's character card (add the line on when he turns cold or angry, from the 03.11 test); Chapter 1 scene card with beats and turn limit 10.
- [C#] `CharacterCard`, `SceneCard` ScriptableObjects; `PromptBuilder` (card, lore, scene, state words, last 8 turns + new line).
- [C#] `ResponseValidator`: Newtonsoft parse, 1 silent retry, emotion whitelist with fallback, 80-word limit, `off_topic`, spoiler keywords (regenerate once), safety word list, delta clamp.
- [C#] Edit Mode tests: valid JSON, broken JSON, unknown emotion, +50 Trust clamped to +8, over-long reply.

**Deliverable:** Console conversation with Zafir through the real prompt; all Edit Mode tests green.
**Verification Gate:** Test Runner all green; the 5 lines from GDD 12.4 pass again through the API; reply times measured. ✋ Confirm with the user.

---

## PHASE 5: Conversation UI + Chapter 1 Zafir Scene
**Objective:** Talking to Zafir in `Test` uses the real conversation screen, online and offline.
**Depends on:** Phases 2 and 4.

**Tasks:**
- [UI] Tuana: conversation screen per `design.md` (dialogue panel, small portraits, hint line, large portrait, 3 reply buttons, 200-character input, Send, turns left) with placeholder portraits.
- [C#] `ConversationUI`, `PortraitAnimator` (emotion → sprite within 1 s), typing indicator ≥ 0.4 s, input cut at 200 with a warning, 1 / 2 / 3 picks.
- [WRITE] Tuana: Chapter 1 fallback lines and the offline dialogue tree for Zafir.
- [C#] Turn counter and simple end at 10 turns (full Director comes in Phase 7).

**Deliverable:** A playable Zafir conversation from E to the 10th turn.
**Verification Gate:** Play online: type, pick suggestions, portrait changes, fallback on timeout. Play offline: the whole scene is playable with the 3 pre-written replies. ✋ Confirm with the user.

---

## PHASE 6: 🏁 Prototype Gate (Sun 15.11)
**Objective:** Confirm the core loop works before content.
**Depends on:** Phases 0–5.

**Tasks:**
- Both: play the `Test` scene end to end, online and offline.
- ⚠️ Early warning check (GDD 13.5): average reply over 4 s, or under 90% valid JSON? If yes → offline mode becomes the default and court NPCs and the pyre confession become pre-written.
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
- [C#] `SceneDirector` FSM: Intro → FreeTalk → Nudge (2 turns left or 3 stalled) → Resolve / ForcedClose → NextChapter; Caught at Suspicion 100.
- [C#] `NPCBrain` with Trust 20 / Affection 10 (±8), Favor 30 (±6), Suspicion 10 (+10 / −5); thresholds 50, 60, 70; value-to-word bands for the prompt.
- [WRITE] Tuana: hint lines ("Zafir looks at you a moment too long.") per value change; nudge notes per scene.
- [C#] Edit Mode tests for transitions and thresholds.

**Deliverable:** The Zafir scene resolves with full or weaker outcome; hints instead of numbers.
**Verification Gate:** Reach the goal → Resolve; stall → Nudge; hit the limit → Forced close; tests green. ✋ Confirm with the user.

---

## PHASE 8: Open Palace Grey-Box & Day / Night Flow
**Objective:** The whole palace is explorable and a chapter runs Day → Night → Key scene → Next day.
**Depends on:** Phase 7.

**Tasks:**
- [UNITY] Grey-box hub + 5 rooms at about 60 × 30 m in `Palace` (see `level-design.md` section 9); stairs to the Rooftop; start point and checkpoint in the Dancers' Quarters; King's Chamber as a restricted volume; locked city gate; bake NavMesh.
- [UNITY] Mark hiding spots (curtains, lattice screens, alcoves) and eavesdrop stops in the grey-box, even before they work.
- [C#] `GameManager`: chapters, phases of the day (Day, Night, KeyScene), checkpoint save at chapter start, NPC spawn points per chapter and per phase; `SaveSystem` (Continue).
- [C#] Nightfall trigger: when the required clues are found or the player chooses "Go to perform" (*proposed*), the goal line switches to "Perform in the Throne Hall"; lighting switches to night.

**Deliverable:** Walk the whole palace; Chapter 1 phases advance with placeholder triggers.
**Verification Gate:** Every room except the King's Chamber is reachable; Day → Night → Key scene → Chapter 2 card works; quit and Continue returns to the checkpoint. ✋ Confirm with the user.

---

## PHASE 9: Clues & Rumor Journal
**Objective:** Talking gives clues, clues open topics and the key scene.
**Depends on:** Phase 8.

**Tasks:**
- [C#] `Clue` ScriptableObject table and `ClueJournal`: accept a clue only if the NPC holds it this chapter; ignore duplicates; required-clue gate for the key scene; clue → extra suggested reply and prompt hint for later NPCs.
- [C#] Add `clues` to the LLM JSON and to the validator; pre-written NPC trees give clues directly.
- [UI] Tuana: Rumor Journal (Tab): chapter question, found clues with speaker, "?" slots, conversation log, memories; "New clue" toast.
- [WRITE] Tuana: Chapter 1 question and 5–6 clues; pre-written trees for Nura, Idris and the minor NPCs present in Chapter 1.
- [C#] Edit Mode tests: rejected unknown clue, duplicate, gate opens at the required count.

**Deliverable:** Chapter 1 day: gather 3 clues, Zafir opens up.
**Verification Gate:** Zafir refuses with 2 clues and talks with 3; journal shows the right entries online and offline. ✋ Confirm with the user.

---

## PHASE 10: Guards, Hiding Spots & Eavesdropping
**Objective:** Patrols make moving around tense, and sneaking earns information.
**Depends on:** Phase 8 (built first in `Test`, then placed in `Palace`).

**Tasks:**
- [C#] `GuardFSM`: NavMeshAgent on fixed waypoints with stops (loop 20–40 s, stops 3–5 s, *proposed*); Patrol → Notice → Patrol; 70° / 9 m cone with line of sight; no chasing.
- [C#] `SuspicionMeter`: +12/s running in view, +20/s restricted room in view, −5/s out of view; dance and Vashir add to the same meter; Caught at 100 → restart at checkpoint.
- [C#] `HidingSpot` (E to enter / leave; invisible to guards) and `EavesdropZone` (hear the whole ~10 s exchange unseen → authored clue; seen → exchange stops).
- [UI] Eye icon, hiding-spot glow and prompt.
- [UNITY] `P_Guard`; 2 guards in Chapter 1 with one garden route and one eavesdrop stop behind a lattice screen.

**Deliverable:** Chapter 1 day with guards, one hiding route and one eavesdrop clue.
**Verification Gate:** Run past a guard → Suspicion rises → restart at 100; hide → the guard walks past; eavesdrop → clue added. ✋ Confirm with the user.

---

## PHASE 11: Nightly Dance Minigame
**Objective:** The night performance is a short, readable rhythm game that feeds Favor, Suspicion and clues.
**Depends on:** Phase 6 (built in `Test` in parallel with Phases 7–10; Tuana's programming task).

**Tasks:**
- [C#] `DanceMinigame`: note chart from a ScriptableObject (`SO_Dance_Night1`...), 4 lanes (← ↑ ↓ →, *proposed*), 0.25 s windows, accuracy; > 80% → +10 Favor + reward; 40–80% → nothing; < 40% → +10 Suspicion; pause support.
- [UI] Dance screen: lanes, hit feedback, end result with reward or warning.
- [UNITY] Sahra's dance loop and 4 move animations; Throne Hall audience placement.
- [AUDIO] Night 1 song (royalty-free); tempo as a parameter.
- [WRITE] Night 1 reward clue (overheard noble gossip).

**Deliverable:** Night 1 dance in the Throne Hall.
**Verification Gate:** Play well → Favor hint + reward clue; play badly → Suspicion; notes stay in sync with the music. ✋ Confirm with the user.

---

## PHASE 12: ⚠️ Chapter 1 Complete + Risk Check (Sun 22.11)
**Objective:** Chapter 1 is the full loop, and real numbers decide what stays.
**Depends on:** Phases 9, 10 and 11.

**Tasks:**
- Play Chapter 1 end to end: Day (clues, guards, eavesdrop) → Night (dance) → Zafir key scene → cutscene placeholder.
- [C#] Test harness and first red-team run (50 lines per character: off-topic, jailbreak, spoiler, rude); LLM judge flags spoilers and broken voice.
- Measure tokens per playthrough and average reply time.
- Decide: memories + pyre confession in or out; Nura / Idris stay pre-written or become LLM.
- ⚠️ Early warnings (GDD 13.5): average reply over 4 s or under 90% valid JSON → offline mode becomes the default. Chapter 2 cards not final → apply the cut order. Palace still grey-box → custom models cut.

**Deliverable:** Playable Chapter 1, red-team report, decisions in Notes and `PROGRESS.md`.
**Verification Gate:** User plays Chapter 1 and confirms the loop is fun; decisions confirmed. ✋

---

## PHASE 13: Chapters 2–4 Content
**Objective:** Every chapter has its day, night and key scene.
**Depends on:** Phase 12.

**Tasks:**
- [WRITE] Tuana: Vashir's card; scene cards, chapter questions, 5–6 clues, NPC trees, fallback lines and offline trees for Chapters 2–4.
- [UNITY] NPC positions per chapter; 3 guards and new routes in Chapter 2; eavesdrop stops per chapter.
- [C#] Chapter 2: night = private dance for Vashir in the King's Chamber (Favor 60 invitation); Vashir's Suspicion from conversation; Zafir's jealousy beat.
- [C#] Chapter 3: night dance, then the rooftop key scene (Affection 60 confession).
- [C#] Chapter 4: short day, night dance, then the Trap (Vashir 4 turns; Zafir's secret now in his prompt).
- [AUDIO] Dance songs for nights 2–4, or one song at rising tempo (cut order 3).

**Deliverable:** Chapters 1–4 in sequence.
**Verification Gate:** Play Chapters 1–4 online and offline; get caught once on purpose to see the restart. ✋ Confirm with the user.

---

## PHASE 14: Cutscene System + 2D Parts
**Objective:** The story plays from Prologue to Epilogue.
**Depends on:** Phase 12 (in parallel with Phase 13).

**Tasks:**
- [C#] Visual-novel cutscene player (background, speaker, text, Space to advance) in `Cutscenes`.
- [WRITE] Tuana: cutscene script (~150 lines) for Prologue, chapter-end beats, after Chapter 4, Pyre (pre-written version), Epilogue.
- [ART] Tuana: 2D backgrounds (Burning Tamra, the pyre, the temple), AI-assisted concepts finished by hand.

**Deliverable:** Full story path.
**Verification Gate:** Play New Game to Credits offline. ✋ Confirm with the user.

---

## PHASE 15: HUD, Menus, Chapter Cards
**Objective:** Every screen in `design.md` section 7 exists.
**Depends on:** Phases 13 and 14 (UI after the gameplay it shows).

**Tasks:**
- [UI] Title (embers over the dying sun), Settings (text speed, volume, AI mode + status line), Pause (Resume, Settings, Restart Chapter, Quit), Chapter card with the chapter question (3 s), Credits.
- [UI] Exploration HUD: goal line, day / night tag, eye icon, "E: Talk", hide prompt, "New clue" toast.
- [C#] `UIManager` screen stack, event-driven updates.

**Deliverable:** All screens reachable through the flow.
**Verification Gate:** Walk every arrow in the `design.md` flow chart. ✋ Confirm with the user.

---

## PHASE 16: Memories + Pyre Confession (should have)
**Objective:** Characters remember, and Zafir's pyre confession quotes two of the player's moments.
**Depends on:** Phase 12 decision = keep.

**Tasks:**
- [C#] Summary call after each key scene (1–2 memories, max 6 per character); memories in the prompt and the Rumor Journal.
- [C#] Generated pyre confession with 2 memories; pre-written fallback.

**Deliverable:** Memories visible in the journal; pyre lines change between runs.
**Verification Gate:** Two different playthroughs give different pyre lines; offline still works. ✋ Confirm with the user.

---

## PHASE 17: 🏁 Content Gate + Playtest 1 (Sun 29.11)
**Objective:** Content complete and tested by outsiders.
**Depends on:** Phases 13–15 (16 if kept).

**Tasks:**
- Playtest with 5 outside players (Tuana leads); watch for: lost players (which clue next?), patrols that feel unfair, dance difficulty, conversation pacing, broken replies.
- Feature freeze from here: no new features in Stage 3.

**Deliverable:** Playtest notes and a fix list.
**Verification Gate:** Full playthrough online and offline with no blocker. ✋ User sign-off.

---

# STAGE 3: POLISH (Week 4 · 30.11 – 06.12)

## PHASE 18: Art Pass
**Objective:** The palace and portraits look like the GDD.
**Depends on:** Phase 17.

**Tasks:**
- [ART] Licensed modular palace kit assembled and lit in HDRP (moonlight, lanterns); license check for educational and portfolio use.
- [ART] Final portraits if time (4–6 emotions each), otherwise placeholders stay.
- [ART] Fire and ember VFX; phoenix-rise VFX for the epilogue (most important effect).

**Deliverable:** Art-complete build.
**Verification Gate:** Play the palace; frame rate acceptable on both PCs. ✋ Confirm with the user.

## PHASE 19: Audio Pass
**Objective:** Music and SFX in place.
**Depends on:** Phase 17.

**Tasks:**
- [AUDIO] Palace loop (must); romance and finale loops (should); loop seams checked.
- [AUDIO] Footsteps on stone, fire, cloth, blades, crowd (CC0 / CC-BY); licenses in `Docs/Licenses.md`.
- [AUDIO] Volume setting wired to an Audio Mixer.

**Deliverable:** Sound-complete build.
**Verification Gate:** Listen through every scene; volume slider works. ✋ Confirm with the user.

## PHASE 20: Tuning, Playtest 2, Red-Team Pass
**Objective:** The game feels right and leaks nothing.
**Depends on:** Phases 18–19.

**Tasks:**
- Tune values, turn limits, hints, clue counts, patrol timing and dance difficulty from playtest 1.
- Second playtest.
- Red-team pass: 0 spoiler leaks, at least 95% valid JSON.

**Deliverable:** Tuned build and red-team report.
**Verification Gate:** Targets met. ✋ Confirm with the user.

## PHASE 21: Credits, Disclosure, Licenses, Prompt Log
**Objective:** Course paperwork complete.
**Depends on:** Phase 20.

**Tasks:**
- Credits screen: team, licensed assets, AI-assisted assets and tools.
- `Docs/Licenses.md` complete; `Docs/PromptLog.md` final; AI-assisted headers on every script.

**Deliverable:** Paperwork complete.
**Verification Gate:** Reviewer checklist from the pull request template passes. ✋ Confirm with the user.

## PHASE 22: 🏁 Packaged Windows Build (Sat 05.12)
**Objective:** A Windows build that always runs.
**Depends on:** Phase 21.

**Tasks:**
- Build to `Builds/` (git-ignored); no API key in the build; starts offline without a key.
- Test on a PC without Unity and without internet.

**Deliverable:** Final Windows build, tagged release on `main`.
**Verification Gate:** Clean-machine run online and offline from Title to Credits, including all 4 nights. ✋ Final sign-off.

---

# APPENDIX: State-Machine Rules
1. One phase at a time per owner. Finish and verify before starting the next.
2. Commit with Git after every phase that passes its gate (feature branch → pull request → review → merge when `Palace` runs).
3. If a phase is blocked, mark it `⏸️ BLOCKED`, write why in Notes, and ask the user.
4. Never change finished phases without writing the reason in Notes.
5. If a phase misses its target date by more than one day, apply the cut list in the Scope table before starting the next phase.

---

## Section Status
| Section | Status |
| :-- | :-- |
| Macro-stage map and goals | FILLED |
| Scope for 4 weeks | FILLED |
| Progress tracker | FILLED |
| Stage 1 phases 0–6 | FILLED |
| Stage 2 phases 7–17 | FILLED |
| Stage 3 phases 18–22 | FILLED |
| Appendix | FILLED |
