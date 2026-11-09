# Ashes for a Goddess · Production Pipeline (State Machine)

> **Canonical design source:** `gdd.md`. Companions: `technical_design.md` (how the code works) and `level-design.md` (where things are). This file is the **order of operations**: small phases, in order, each with a hard check before the next one starts.
>
> **How to use this file:** treat each Phase as a node in a state machine. A phase may not start until its `Depends on` phases are `DONE`. Every phase ends with a **Verification Gate**: the feature is tested in Unity **Play Mode** and the user confirms before moving on. Never build UI before the gameplay it shows. Never build a level before its mechanics work in the `Test` scene.
>
> **Time box: 4 weeks in total, Monday 09.11.2026 to Sunday 06.12.2026.** Final build on Saturday 05.12.2026; Sunday 06.12 is buffer only. This compresses the GDD's Weeks 9–14 plan (which ended 20.12.2026). Owners follow the GDD roles: **Merve** (code, level, audio) and **Tuana** (art, UI, writing). Two tracks run in parallel; a phase lists who owns it.

**Legend:** `[C#]` code · `[UNITY]` scene / prefab / editor work · `[ART]` assets · `[UI]` interface · `[AUDIO]` sound · `[WRITE]` lore, cards, lines · `⚠️` risk / decision

---

## MACRO-STAGE MAP
```
STAGE 1: PROTOTYPE           STAGE 2: FULL GAME              STAGE 3: POLISH
Week 1: 09.11 – 15.11        Weeks 2–3: 16.11 – 29.11        Week 4: 30.11 – 06.12
Phases 0 to 6                Phases 7 to 15                  Phases 16 to 20
      |                             |                               |
 [PROTOTYPE GATE 15.11] ---> [CONTENT GATE 29.11] ---------> [FINAL BUILD 05.12]
```

**Stage 1 goal:** Sahra walks and talks in a test scene; one conversation with Zafir (Chapter 1) works end to end, online through the LLM and offline through pre-written lines, with the validator in between.
**Stage 2 goal:** The whole game from Title to Credits: palace grey-box with all rooms, 4 free-talk chapters, Scene Director, relationship values, cutscenes (prologue, pyre, epilogue), HUD and menus, offline mode for every scene; should-haves that fit (memories, guards). First playtest with outside players.
**Stage 3 goal:** Art and audio pass, tuning from playtests, red-team pass with 0 spoiler leaks, Windows build that starts in offline mode without a key, credits and AI disclosure.

### Scope for 4 weeks
| Priority (GDD 10) | Feature | In the 4-week plan |
| :-- | :-- | :-- |
| Must | Palace (hub + 5 rooms), third-person movement and camera | Yes (grey-box week 2, art week 4) |
| Must | LLM conversations with Zafir and Vashir (prompt builder, validator, clamped values) | Yes (weeks 1–2) |
| Must | Scene Director with nudge and forced close | Yes (week 2) |
| Must | Offline mode for every scene | Yes (built alongside each chapter) |
| Must | 4 free-talk chapters + prologue, pyre, epilogue cutscenes | Yes (weeks 2–3) |
| Must | Conversation UI, exploration HUD, title, pause, chapter cards | Yes (weeks 1–3) |
| Must | Placeholder portraits, basic fire VFX, 1 music loop | Yes |
| Should | Memories + generated pyre confession | Week 3 if Phase 10 risk check passes |
| Should | Guard patrol FSM + exploration Suspicion | Week 3, simple version; cut first if late |
| Should | Nura and Idris as LLM characters | Pre-written by default; LLM only if week 2 is on time |
| Should | Dance minigame | ⚠️ Cut: the dance is a cutscene (GDD-approved fallback) |
| Should | Final portraits, 3 music loops, phoenix-rise VFX | Week 4 if time |
| Nice | Local model, TTS for Zafir, varying epilogue lines, proxy server | ⚠️ Cut (varying epilogue lines only if memories land early) |

---

## PROGRESS TRACKER
> Update this table after every work session. Status: `⬜ NOT STARTED` · `🔄 IN PROGRESS` · `✅ DONE` · `⏸️ BLOCKED`.
> When you resume, start from the first row that is not `✅`.

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
| 8 | Palace grey-box & chapter flow | 2 | Merve | Thu 19.11 | ⬜ NOT STARTED | | |
| 9 | Chapters 2–3 (Vashir, rooftop) | 2 | Tuana + Merve | Sat 21.11 | ⬜ NOT STARTED | | |
| 10 | ⚠️ Risk check: red-team run & token cost | 2 | Both | Sun 22.11 | ⬜ NOT STARTED | | decides should-haves |
| 11 | Chapter 4 + cutscene system + 2D parts | 2 | Merve + Tuana | Wed 25.11 | ⬜ NOT STARTED | | |
| 12 | HUD, menus, chapter cards, log | 2 | Tuana | Thu 26.11 | ⬜ NOT STARTED | | |
| 13 | Memories + pyre confession (should) | 2 | Merve | Fri 27.11 | ⬜ NOT STARTED | | |
| 14 | Guards & exploration Suspicion (should) | 2 | Merve | Sat 28.11 | ⬜ NOT STARTED | | |
| 15 | 🏁 Content gate + playtest 1 | 2 | Both | Sun 29.11 | ⬜ NOT STARTED | | user sign-off |
| 16 | Art pass (palace kit, portraits, VFX) | 3 | Tuana | Tue 01.12 | ⬜ NOT STARTED | | |
| 17 | Audio pass | 3 | Merve | Tue 01.12 | ⬜ NOT STARTED | | |
| 18 | Tuning, playtest 2, red-team pass | 3 | Both | Thu 03.12 | ⬜ NOT STARTED | | |
| 19 | Credits, disclosure, licenses, prompt log | 3 | Tuana | Fri 04.12 | ⬜ NOT STARTED | | |
| 20 | 🏁 Packaged Windows build | 3 | Merve | Sat 05.12 | ⬜ NOT STARTED | | final |

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

## PHASE 7: Scene Director & Relationship Values
**Objective:** Every scene ends by goal or by turn limit, and values change within their limits.
**Depends on:** Phase 6.

**Tasks:**
- [C#] `SceneDirector` FSM: Intro → FreeTalk → Nudge (2 turns left or 3 stalled) → Resolve / ForcedClose → NextChapter; Caught at Suspicion 100.
- [C#] `NPCBrain` with Trust 20 / Affection 10 (±8), Favor 30 (±6), Suspicion 10 (+10 / −5); thresholds 50, 60, 70; value-to-word bands for the prompt.
- [WRITE] Tuana: hint lines ("Zafir looks at you a moment too long.") per value change; nudge notes per scene.
- [C#] Edit Mode tests for transitions and thresholds.

**Deliverable:** Chapter 1 resolves with full or weaker outcome; hints instead of numbers.
**Verification Gate:** Reach the goal → Resolve; stall → Nudge; hit the limit → Forced close; tests green. ✋ Confirm with the user.

---

## PHASE 8: Palace Grey-Box & Chapter Flow
**Objective:** The full palace layout exists and chapters open rooms in order.
**Depends on:** Phase 7.

**Tasks:**
- [UNITY] Grey-box hub + 5 rooms at about 60 × 30 m in `Palace` (see `level-design.md` section 9); stairs to the Rooftop; start point and checkpoint in the Dancers' Quarters; bake NavMesh.
- [C#] `GameManager`: chapters, checkpoints (save at chapter start), room unlocking per chapter, character spawns per chapter; `SaveSystem` (Continue).
- [UNITY] Move Chapter 1 from `Test` into `Palace` (Zafir in the garden, Nura in the Dancers' Quarters, Idris at the Guard Post with pre-written dialogue).

**Deliverable:** Chapter 1 playable in `Palace`, Chapter 2 rooms open after it.
**Verification Gate:** Play Chapter 1 in the palace, resolve it, Chapter 2 doors open; quit and Continue returns to the checkpoint. ✋ Confirm with the user.

---

## PHASE 9: Chapters 2–3 (Vashir, Rooftop)
**Objective:** Chapters 2 and 3 are playable online and offline.
**Depends on:** Phase 8.

**Tasks:**
- [WRITE] Tuana: Vashir's card; Chapter 2 and 3 scene cards, fallback lines and offline trees.
- [C#] Vashir's Suspicion from conversation → caught at 100 → restart chapter; Favor 60 → invitation and King's Chamber unlock.
- [UNITY] Throne Hall audience, King's Chamber dance as a cutscene (dance minigame cut), Zafir's jealousy beat; Rooftop Terrace with Affection 60 confession.
- [UNITY] Portraits v1 for Zafir and Vashir (Tuana).

**Deliverable:** Chapters 1–3 in sequence.
**Verification Gate:** Play Chapters 1–3 online and offline; force Suspicion to 100 once to see the restart. ✋ Confirm with the user.

---

## PHASE 10: ⚠️ Risk Check: Red-Team Run & Token Cost (Sun 22.11)
**Objective:** Decide which should-haves stay, using real numbers.
**Depends on:** Phase 9.

**Tasks:**
- [C#] Test harness sending scripted lines; first red-team run (50 lines per character: off-topic, jailbreak, spoiler, rude); LLM judge flags spoilers and broken voice.
- Measure tokens per playthrough and average reply time.
- Decide: Nura / Idris LLM or pre-written; memories + pyre confession in or out; guards in or out.
- ⚠️ Early warnings (GDD 13.5): Chapter 2 cards not final → cut fallback lines by half, cutscenes as still images with text. Palace still grey-box → cut custom models, keep placeholder portraits.

**Deliverable:** Red-team report and decisions in Notes and `PROGRESS.md`.
**Verification Gate:** Every flagged reply plus 20 random ones read; decisions confirmed with the user. ✋

---

## PHASE 11: Chapter 4 + Cutscene System + 2D Parts
**Objective:** The story plays from Prologue to Epilogue.
**Depends on:** Phase 10.

**Tasks:**
- [C#] Visual-novel cutscene player (background, speaker, text, Space to advance) in `Cutscenes`.
- [WRITE] Tuana: cutscene script (~150 lines) for Prologue, after Chapter 4, Pyre (pre-written version), Epilogue; Chapter 4 scene card (Vashir 4 turns; Zafir's secret now in his prompt).
- [ART] Tuana: 2D backgrounds (Burning Tamra, the pyre, the temple), AI-assisted concepts finished by hand.
- [UNITY] Chapter 4 in the King's Chamber → death cutscene → Pyre → Epilogue → Credits.

**Deliverable:** Full story path.
**Verification Gate:** Play New Game to Credits offline. ✋ Confirm with the user.

---

## PHASE 12: HUD, Menus, Chapter Cards, Log
**Objective:** Every screen in `design.md` section 7 exists.
**Depends on:** Phase 11 (UI after the gameplay it shows).

**Tasks:**
- [UI] Title (embers over the dying sun), Settings (text speed, volume, AI mode + status line), Pause (Resume, Settings, Restart Chapter, Quit), Chapter card (3 s), Log and memories (Tab), Credits.
- [UI] Exploration HUD: goal line, eye icon, "E: Talk".
- [C#] `UIManager` screen stack, event-driven updates.

**Deliverable:** All screens reachable through the flow.
**Verification Gate:** Walk every arrow in the `design.md` flow chart. ✋ Confirm with the user.

---

## PHASE 13: Memories + Pyre Confession (should have)
**Objective:** Characters remember, and Zafir's pyre confession quotes two of the player's moments.
**Depends on:** Phase 10 decision = keep.

**Tasks:**
- [C#] Summary call after each scene (1–2 memories, max 6 per character); memories in the prompt state and the Log screen.
- [C#] Generated pyre confession with 2 memories; pre-written fallback.

**Deliverable:** Memories visible in the Log; pyre lines change between runs.
**Verification Gate:** Two different playthroughs give different pyre lines; offline still works. ✋ Confirm with the user.

---

## PHASE 14: Guards & Exploration Suspicion (should have)
**Objective:** Garden patrols make sneaking matter.
**Depends on:** Phase 10 decision = keep; Phase 8.

**Tasks:**
- [C#] `GuardFSM` (Patrol → Notice → Investigate → Alert) with NavMeshAgent; 70° / 9 m cone.
- [C#] Suspicion +12/s running in view, +20/s restricted room in view, −5/s out of view; eye icon events.
- [UNITY] 2 guards in Chapter 1, 3 in Chapter 2 (`P_Guard`, recolored base model).

**Deliverable:** Guards patrol and can catch Sahra.
**Verification Gate:** Run past a guard → Suspicion rises → restart at 100; walk → safe. ✋ Confirm with the user.

---

## PHASE 15: 🏁 Content Gate + Playtest 1 (Sun 29.11)
**Objective:** Content complete and tested by outsiders.
**Depends on:** Phases 11–12 (13–14 if kept).

**Tasks:**
- Playtest with 5 outside players (Tuana leads); notes on confusion, pacing, broken replies.
- Feature freeze from here: no new features in Stage 3.

**Deliverable:** Playtest notes and a fix list.
**Verification Gate:** Full playthrough online and offline with no blocker. ✋ User sign-off.

---

# STAGE 3: POLISH (Week 4 · 30.11 – 06.12)

## PHASE 16: Art Pass
**Objective:** The palace and portraits look like the GDD.
**Depends on:** Phase 15.

**Tasks:**
- [ART] Licensed modular palace kit assembled and lit in HDRP (moonlight, lanterns); license check for educational and portfolio use.
- [ART] Final portraits if time (4–6 emotions each), otherwise placeholders stay.
- [ART] Fire and ember VFX; phoenix-rise VFX for the epilogue (most important effect).

**Deliverable:** Art-complete build.
**Verification Gate:** Play the palace; frame rate acceptable on both PCs. ✋ Confirm with the user.

## PHASE 17: Audio Pass
**Objective:** Music and SFX in place.
**Depends on:** Phase 15.

**Tasks:**
- [AUDIO] Palace loop (must); romance and finale loops (should); loop seams checked.
- [AUDIO] Footsteps on stone, fire, cloth, blades, crowd (CC0 / CC-BY); licenses in `Docs/Licenses.md`.
- [AUDIO] Volume setting wired to an Audio Mixer.

**Deliverable:** Sound-complete build.
**Verification Gate:** Listen through every scene; volume slider works. ✋ Confirm with the user.

## PHASE 18: Tuning, Playtest 2, Red-Team Pass
**Objective:** The game feels right and leaks nothing.
**Depends on:** Phases 16–17.

**Tasks:**
- Tune values, turn limits and hints from playtest 1.
- Second playtest.
- Red-team pass: 0 spoiler leaks, at least 95% valid JSON.

**Deliverable:** Tuned build and red-team report.
**Verification Gate:** Targets met. ✋ Confirm with the user.

## PHASE 19: Credits, Disclosure, Licenses, Prompt Log
**Objective:** Course paperwork complete.
**Depends on:** Phase 18.

**Tasks:**
- Credits screen: team, licensed assets, AI-assisted assets and tools.
- `Docs/Licenses.md` complete; `Docs/PromptLog.md` final; AI-assisted headers on every script.

**Deliverable:** Paperwork complete.
**Verification Gate:** Reviewer checklist from the pull request template passes. ✋ Confirm with the user.

## PHASE 20: 🏁 Packaged Windows Build (Sat 05.12)
**Objective:** A Windows build that always runs.
**Depends on:** Phase 19.

**Tasks:**
- Build to `Builds/` (git-ignored); no API key in the build; starts offline without a key.
- Test on a PC without Unity and without internet.

**Deliverable:** Final Windows build, tagged release on `main`.
**Verification Gate:** Clean-machine run online and offline from Title to Credits. ✋ Final sign-off.

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
| Stage 2 phases 7–15 | FILLED |
| Stage 3 phases 16–20 | FILLED |
| Appendix | FILLED |
