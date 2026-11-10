# Ashes for a Goddess · Level Design Document (level-design.md)

> **Canonical design source:** `gdd.md` (Level Design, Themes, Game Flow). This document turns the GDD levels into buildable detail. **Where this document and the GDD disagree, the GDD wins.**
> **Companion documents:** `technical_design.md` and `production_pipeline.md`.

**World overview:** One 3D level, the palace (House of Embers), about 60 × 30 m: a hub (Garden of Ash) with five rooms around it (Dancers' Quarters, Guard Post, Throne Hall, King's Chamber, Rooftop Terrace). Played in order: Prologue: Ash (2D) → Chapter 1: The Garden of Ash → Chapter 2: The Private Dance → Chapter 3: Rooftop of Smoke → Chapter 4: The Trap → Chapter 5: The Pyre (2D) → Epilogue: The Long Flame (2D). In Unreal Engine 5.8, the palace is the level `L_Palace` (One File Per Actor, so both members can edit different actors at once; no World Partition streaming needed at this size). The 2D parts play as Level Sequences (`LS_Prologue`, `LS_Pyre`, `LS_Epilogue`) with `WBP_Cutscene` over them; the title screen is `L_Title` and mechanics are tested in `L_Test`.

**Updated 09.11 and 10.11.2026:** the palace is **open** from Chapter 1 (everything except the restricted King's Chamber and the locked city gate). Each chapter is **a day and a night**: by day the player gathers clues for the Rumor Journal while avoiding guards (hiding spots, eavesdropping); at night Sahra dances in the Throne Hall (Chapter 2: the King's Chamber); then the key conversation and a cutscene. NPCs and guard routes change per chapter and per phase.

---

## 1. Level Design Philosophy
1. **Guidance:** (Changed 09.11) the palace is open, so the chapter question, the NPCs present and lit eavesdrop spots guide the player instead of locked doors; at nightfall the Throne Hall is lit brightest and the court gathers there. The goal line on the HUD names the target ("Find out why the other dancer carries a blade"). Lanterns and firelight (Old gold, Ember orange) pick out details in plum and burgundy shadows; the restricted King's Chamber is marked with Rose red. Characters show an "E: Talk" prompt. MISSING: specific light, sound or shape cues that lead to each character and door.
2. **Teaching structure:** Chapter 1 introduces walking, talking and the garden guard patrol safely (2 guards, three friendly or neutral characters) → Chapter 2 combines exploration with Vashir's Suspicion in conversation and adds a third guard and a restricted room → Chapter 3 is a twist in tone: no threat, only the romance, decided by Affection → Chapter 4 is the final test: only 4 turns with Vashir, then the trap.
3. **Layout style:** Hub and spokes, fully open. The Garden of Ash is the hub; every route passes through it, so its guard patrol is the stealth challenge in every chapter. Each room has at least one hiding spot near its door and each chapter has 1–2 eavesdrop stops on guard routes (proposed).

**Scale standard:** Palace about 60 × 30 m; walk 2.2 m/s, run 4.5 m/s (crossing 60 m takes about 27 s walking or 13 s running); guard sight 9 m in a 70° cone. MISSING: player height, door width and height, corridor width. No jumping.

---

## 2. Mechanic Vocabulary
> Every level is built ONLY from these verbs. No level may use a mechanic that is not in this table.

| Verb / mechanic | Who uses it | GDD source | Level use |
| :-- | :-- | :-- | :-- |
| Walk / run | Sahra | 2 Controls, 2 Mechanics 5 | Crossing the garden hub between rooms; running raises Suspicion in view |
| Talk (type or pick reply) | Sahra ↔ Zafir, Vashir, Nura, Idris | 1 Mechanics 1 | Chapter goals in every room with a character |
| Interact (E) | Sahra | 2 Controls | Start conversations; objects (MISSING: which objects are interactive besides characters) |
| Guard sight / Suspicion | Guards, Vashir | 1 Mechanics 5 | Garden patrol; Throne Hall guard; restricted King's Chamber |
| Restricted room | Level | 3 Level Design | King's Chamber until Favor 60 |
| Gather clues (Rumor Journal) | Sahra | 1 Mechanics 6 | 5–6 clues per chapter from NPCs, eavesdrop stops and the night's dance reward |
| Hide (E) | Sahra | 1 Mechanics 8 | Curtains, lattice screens, alcoves; at least one per room |
| Eavesdrop | Sahra, guards, courtiers | 1 Mechanics 8 | Guard stops and courtier corners next to a hiding spot |
| Day / night phase | Level | 1 Gameplay | Lighting, NPC positions and the goal change at nightfall |
| Checkpoint | Level | 1 Win and Lose | Dancers' Quarters, start of each chapter |
| Nightly dance (must have since 10.11) | Sahra | 1 Mechanics 7 | Every night in the Throne Hall; Chapter 2 in the King's Chamber |
| Cutscene (visual novel) | Game | 3 Themes | Prologue, after Chapter 4, Pyre, Epilogue |

---

## 3. LEVEL 1: Chapter 1 · The Garden of Ash

**Theme / mood:** Moonlit garden under a huge pale moon; lanterns, a dry fountain; night plum shadows and old-gold light. Palace music loop (oud, frame drum, drone); footsteps on stone.
**Threats:** 2 guards; one patrols the garden.
**Goal:** Learn why Zafir is here and agree to work together (beats such as `zafir_reveals_target`, `alliance_agreed`). On-screen question: "Find out why the other dancer carries a blade." Day: 3 of 5–6 clues. Night: dance 1 in the Throne Hall (30 s).

### Layout
Rooms open: Dancers' Quarters, Garden of Ash, Guard Post.
```
                 [Rooftop Terrace]       [Throne Hall]          [King's Chamber] (restricted)
                   priest (minor NPC)      servant (minor NPC)
                        |  (stairs)            |   NIGHT: dance 1
[Dancers' Quarters] -- [ Garden of Ash (hub) ] -- [Guard Post] -- (gate to city, locked)
  START / checkpoint      Zafir, dry fountain       Captain Idris
  Nura                    guard patrol loop,        guard 2 stop
                          lattice screen = hide + eavesdrop
```

### Progression
- **1. Introduce (day):** Sahra starts in the Dancers' Quarters (safe, no guard) and talks to Nura to learn talking and the Rumor Journal (first clue).
- **2. Combine (day):** cross the garden while the guard patrols; learn to hide behind the lattice screen and eavesdrop on the two guards (second clue); question Idris, the servant or the priest for the third.
- **3. Twist (night):** nightfall; dance 1 in the Throne Hall in front of the court; a great dance gives an extra clue from a noble.
- **4. Final test (key scene):** Zafir waits in the garden; reach the alliance beat before the turn limit; Sahra steals Zafir's knife; chapter resolves.

### Secrets & extras
- Captain Idris at the Guard Post (4 turns): he doubts the burnings.
- Nura's gossip about the court; she lost a sister to a burning.
- MISSING: any hidden areas or collectibles (none in the GDD).

### Enemy / hazard notes
- Garden patrol loop around the hub (GDD Figure 4, "guard patrol"); loop 20–40 s with a stop at the lattice screen where the two guards talk (eavesdrop) (proposed). MISSING: exact waypoints and timing; second guard's stop (Guard Post is likely). At night the guards stand at the Throne Hall doors instead (proposed).

---

## 4. LEVEL 2: Chapter 2 · The Private Dance

**Theme / mood:** Vashir's court: burgundy curtains, gold, firelight; the court in the Throne Hall (crowd sounds).
**Threats:** 3 guards (1 in the Throne Hall); Vashir's probing questions raise Suspicion (+10 / −5 per turn); the King's Chamber is restricted (+20/s in view) until Favor 60.
**Goal:** Get a private meeting with Vashir without raising Suspicion (invitation at Favor 60). Afterwards, Zafir's jealousy shows.

### Layout
Whole palace open; the King's Chamber stays restricted until the invitation (Favor 60). Night: the private dance (dance 2) in the King's Chamber.
```
[Dancers' Quarters] -- [Garden of Ash] -- [Guard Post]
                              |
                        [Throne Hall] ---- [King's Chamber] (restricted until invited)
                         Vashir, court,      dance
                         1 guard
```

### Progression
- **1. Introduce:** cross the garden past the patrol to the Throne Hall.
- **2. Combine:** audience with Vashir (8 turns): talk well (Favor ±6) while not raising Suspicion.
- **3. Twist:** at Favor 60 Vashir invites Sahra to dance in his chamber; the restricted room opens.
- **4. Final test:** the dance (minigame if built: > 80% +10 Favor, < 40% +10 Suspicion; otherwise a cutscene); then Zafir's jealousy beat.

### Secrets & extras
- Court NPCs in the Throne Hall. MISSING: what extra the court offers.

### Enemy / hazard notes
- 3 guards: garden patrol, Throne Hall guard, MISSING: third guard position. King's Chamber entry before invitation = +20 Suspicion per second in view.

---

## 5. LEVEL 3: Chapter 3 · Rooftop of Smoke

**Theme / mood:** Rooftop terrace on the upper level; smoke and moonlight; romance loop (solo strings).
**Threats:** None stated in the GDD. MISSING: whether guards still patrol the garden on the way.
**Goal:** Plan the strike with Zafir. If Affection is high (60), they both admit their feelings (full rooftop confession).

### Layout
Whole palace open. Day: clues for the strike plan. Night: dance 3 in the Throne Hall (faster tempo), then the rooftop key scene.
```
[Rooftop Terrace]  (upper level)
        | stairs
[Dancers' Quarters] -- [Garden of Ash] -- ...
```

### Progression
- **1. Introduce:** climb the stairs from the Dancers' Quarters.
- **2. Combine:** talk with Zafir (10 turns); Trust 70 → he shares the route to the king.
- **3. Twist:** Affection decides how much of the confession plays.
- **4. Final test:** agree the strike plan before the limit.

### Secrets & extras
- Full confession at Affection 60.

### Enemy / hazard notes
- MISSING: guard placement for Chapter 3.

---

## 6. LEVEL 4: Chapter 4 · The Trap

**Theme / mood:** The King's Chamber, Rose red; betrayal.
**Threats:** Vashir (only 4 turns), Zafir, guards.
**Goal:** The scripted twist: Vashir reveals that he hired Zafir to kill her; Sahra is cut down and dies in Zafir's arms. The chapter ends in a cutscene.

### Layout
Whole palace open. Short day; night: dance 4 (45 s) in the Throne Hall; then Vashir summons Sahra to the King's Chamber for the trap (proposed).
```
[Garden of Ash] -- [Throne Hall] -- [King's Chamber]  Vashir, Zafir, guards
```

### Progression
- **1. Introduce:** reach the King's Chamber.
- **2. Combine:** 4 turns with Vashir; Zafir's secret is now in his prompt.
- **3. Twist:** the reveal.
- **4. Final test:** cutscene: Sahra dies in Zafir's arms → Pyre.

### Secrets & extras
- None.

### Enemy / hazard notes
- MISSING: guard positions in the King's Chamber.

---

## 7. 2D Parts: Prologue, The Pyre, Epilogue
- **Prologue: Ash:** background "Burning Tamra"; Tamra burns, a girl walks out of the smoke; years later Sahra arrives as a dancer.
- **Chapter 5: The Pyre:** background "The pyre"; Zafir burns her body in Tamra and confesses, quoting 2 memories (generated; pre-written fallback). The dying sun flares.
- **Epilogue: The Long Flame:** background "The temple"; Sahra rises as the phoenix, Vashir falls, the sun returns; final shot with Zafir's stolen knife: "Careful, Little Ember." Then Credits.

---

## 8. Difficulty Curve
| Level | New mechanic | Difficulty (1 to 5) | Notes |
| :-- | :-- | :-- | :-- |
| Chapter 1: The Garden of Ash | Walk, talk, clues, hide, eavesdrop, dance 1 | MISSING (suggested 1) | 2 guards; 3 clues; 30 s dance; 10 turns with Zafir |
| Chapter 2: The Private Dance | Vashir's Suspicion, restricted room, private dance | MISSING (suggested 3) | 3 guards; 3 clues; dance 2 (+10% tempo); 8 turns |
| Chapter 3: Rooftop of Smoke | Affection decides the confession; clues that need other clues first | MISSING (suggested 3) | 3 guards (proposed); dance 3; 10 turns |
| Chapter 4: The Trap | Only 4 turns | MISSING (suggested 2) | Short day; dance 4 (45 s); scripted outcome |

---

## 9. Build Notes
1. **Grey-box first** (pipeline Phase 8): block out the hub and all five rooms at 60 × 30 m with BSP / Geometry Script or modeling-mode primitives in `L_Palace` (units in cm: about 6000 × 3000 cm); place doors, stairs, the dry fountain, Player Start and checkpoint; add a Nav Mesh Bounds Volume and check the green NavMesh with `P` in the viewport.
2. **Mechanics next** (Phases 7–13): room unlocking per chapter, restricted volume for the King's Chamber, character spawn points per chapter, guard patrol routes. Every mechanic is proven in `L_Test` first. Restricted rooms are `BP_RestrictedVolume` triggers; spawn points are `BP_NPCSpawnPoint` actors tagged by chapter and phase; patrol routes are Target Point actors referenced by each `BP_Guard`.
3. **Art last** (Phase 16): licensed modular palace kit (Fab), lantern lights, moonlight (Directional Light + Sky Atmosphere), Lumen global illumination, Post Process Volume per day and night, Niagara fire VFX. Early warning: if the palace is still grey-box at the end of week 2, custom models are cut.
4. Chapter content table:

| Chapter | Rooms open | Characters there |
| :-- | :-- | :-- |
| 1 | Dancers' Quarters, Garden of Ash, Guard Post | Zafir, Nura, Idris, 2 guards |
| 2 | + Throne Hall, King's Chamber (by invitation) | Vashir, the court, 3 guards |
| 3 | + Rooftop Terrace | Zafir |
| 4 | King's Chamber | Vashir, Zafir, guards |

---

## Section Status
| Section | Status |
| :-- | :-- |
| World overview | FILLED |
| 1. Philosophy | FILLED (MISSING: guidance cues, scale sizes) |
| 2. Mechanic Vocabulary | FILLED (MISSING: interactive objects) |
| 3. Level 1 | FILLED (MISSING: patrol route, extras) |
| 4. Level 2 | FILLED (MISSING: third guard, court extras) |
| 5. Level 3 | FILLED (MISSING: guard placement) |
| 6. Level 4 | FILLED (MISSING: guard positions) |
| 7. 2D Parts | FILLED |
| 8. Difficulty Curve | MISSING (suggested values) |
| 9. Build Notes | FILLED |
