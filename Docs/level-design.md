# Ashes for a Goddess · Level Design Document (level-design.md)

> **Canonical design source:** `gdd.md` (Level Design, Themes, Game Flow). This document turns the GDD levels into buildable detail. **Where this document and the GDD disagree, the GDD wins.**
> **Companion documents:** `technical_design.md` and `production_pipeline.md`.

**World overview:** One 3D level, the palace (House of Embers), about 60 × 30 m: a hub (Garden of Ash) with five rooms around it (Dancers' Quarters, Guard Post, Throne Hall, King's Chamber, Rooftop Terrace). Played in order: Prologue: Ash (2D) → Chapter 1: The Garden of Ash → Chapter 2: The Private Dance → Chapter 3: Rooftop of Smoke → Chapter 4: The Trap → Chapter 5: The Pyre (2D) → Epilogue: The Long Flame (2D). In Unity, the palace is the `Palace` scene; the 2D parts play in `Cutscenes`.

---

## 1. Level Design Philosophy
1. **Guidance:** Rooms unlock chapter by chapter, so only the doors that matter are open. The goal line on the HUD names the target ("Find out why the other dancer carries a blade"). Lanterns and firelight (Old gold, Ember orange) pick out details in plum and burgundy shadows; the restricted King's Chamber is marked with Rose red. Characters show an "E: Talk" prompt. MISSING: specific light, sound or shape cues that lead to each character and door.
2. **Teaching structure:** Chapter 1 introduces walking, talking and the garden guard patrol safely (2 guards, three friendly or neutral characters) → Chapter 2 combines exploration with Vashir's Suspicion in conversation and adds a third guard and a restricted room → Chapter 3 is a twist in tone: no threat, only the romance, decided by Affection → Chapter 4 is the final test: only 4 turns with Vashir, then the trap.
3. **Layout style:** Hub and spokes. The Garden of Ash is the hub; every route passes through it, so its guard patrol is the stealth challenge in every chapter.

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
| Room unlock by chapter | Level | 3 Level Design | Doors open at chapter start |
| Checkpoint | Level | 1 Win and Lose | Dancers' Quarters, start of each chapter |
| Dance minigame (should have) | Sahra | 1 Mechanics 6 | Chapter 2, King's Chamber |
| Cutscene (visual novel) | Game | 3 Themes | Prologue, after Chapter 4, Pyre, Epilogue |

---

## 3. LEVEL 1: Chapter 1 · The Garden of Ash

**Theme / mood:** Moonlit garden under a huge pale moon; lanterns, a dry fountain; night plum shadows and old-gold light. Palace music loop (oud, frame drum, drone); footsteps on stone.
**Threats:** 2 guards; one patrols the garden.
**Goal:** Learn why Zafir is here and agree to work together (beats such as `zafir_reveals_target`, `alliance_agreed`). On-screen goal: "Find out why the other dancer carries a blade."

### Layout
Rooms open: Dancers' Quarters, Garden of Ash, Guard Post.
```
                 [Rooftop Terrace]*   [Throne Hall]*   [King's Chamber]* (locked)
                        |  (stairs)         |
[Dancers' Quarters] -- [ Garden of Ash (hub) ] -- [Guard Post] -- (gate to city, locked)
  START / checkpoint      Zafir, dry fountain       Captain Idris
  Nura                    guard patrol loop
* = closed in this chapter
```

### Progression
- **1. Introduce:** Sahra starts in the Dancers' Quarters (safe, no guard) and can talk to Nura (4 turns) to learn talking.
- **2. Combine:** walk into the garden while the guard patrols; walking keeps Suspicion low, running in view adds +12/s.
- **3. Twist:** find Zafir in the garden and talk under the patrol; Zafir dodges questions (10 turns, nudge at 2 left).
- **4. Final test:** reach the alliance beat before the turn limit; Sahra steals Zafir's knife; chapter resolves.

### Secrets & extras
- Captain Idris at the Guard Post (4 turns): he doubts the burnings.
- Nura's gossip about the court; she lost a sister to a burning.
- MISSING: any hidden areas or collectibles (none in the GDD).

### Enemy / hazard notes
- Garden patrol loop around the hub (GDD Figure 4, "guard patrol"); MISSING: exact waypoint route, timing and where the second guard stands (Guard Post is likely).

---

## 4. LEVEL 2: Chapter 2 · The Private Dance

**Theme / mood:** Vashir's court: burgundy curtains, gold, firelight; the court in the Throne Hall (crowd sounds).
**Threats:** 3 guards (1 in the Throne Hall); Vashir's probing questions raise Suspicion (+10 / −5 per turn); the King's Chamber is restricted (+20/s in view) until Favor 60.
**Goal:** Get a private meeting with Vashir without raising Suspicion (invitation at Favor 60). Afterwards, Zafir's jealousy shows.

### Layout
Rooms open: + Throne Hall, King's Chamber (by invitation).
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
Rooms open: + Rooftop Terrace.
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
Rooms open: King's Chamber.
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
| Chapter 1: The Garden of Ash | Walk, talk, garden patrol | MISSING (suggested 1) | 2 guards; 10 turns with Zafir |
| Chapter 2: The Private Dance | Vashir's Suspicion, restricted room, dance | MISSING (suggested 3) | 3 guards; 8 turns |
| Chapter 3: Rooftop of Smoke | Affection decides the confession | MISSING (suggested 2) | No new threat |
| Chapter 4: The Trap | Only 4 turns | MISSING (suggested 2) | Scripted outcome |

---

## 9. Build Notes
1. **Grey-box first** (pipeline Phase 8): block out the hub and all five rooms at 60 × 30 m with primitives in `Palace`; place doors, stairs, the dry fountain, start point and checkpoint; bake the NavMesh.
2. **Mechanics next** (Phases 7–13): room unlocking per chapter, restricted volume for the King's Chamber, character spawn points per chapter, guard patrol routes. Every mechanic is proven in `Test` first.
3. **Art last** (Phase 16): licensed modular palace kit, lantern lights, moonlight, HDRP volumes, fire VFX. Early warning: if the palace is still grey-box at the end of week 2, custom models are cut.
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
