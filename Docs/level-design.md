# Ashes for a Goddess · Level Design Document (level-design.md)

> **Canonical design source:** `gdd.md` (section 3 Level Design, Themes, Game Flow). This document turns the GDD levels into buildable detail. **Where this document and the GDD disagree, the GDD wins.**
> **Companion documents:** `technical_design.md` and `production_pipeline.md`. Story and clue texts: the Master Lore document (tabs "Chapter Cards", "Scene Cards", "Clues", "Barks").
> **Course:** Advanced Game Programming. **Owner:** Merve (level design). Version 2, updated 12.11.2026 for the 7-chapter story and the team of four.

**World overview:** One 3D level, the palace **Sama'zar**, about 60 × 30 m: a hub, the **Garden of Dusk** (its fountain still runs, because the palace pretends to be unaffected by the dying sun), with five rooms around it: the **Dancers' Quarters** (with the practice room; stairs up to the Rooftop Terrace), the **Throne Hall**, the **King's Chamber** (restricted; a servants' passage behind a curtain), the **Guard Post** (with the locked **City Gate**) and the **Rooftop Terrace**. Played in order: Prologue: Ash (2D) → Chapter 1: The Garden of Dusk → Chapter 2: The Practice Room → Chapter 3: The Private Dance → Chapter 4: The King's Favor → Chapter 5: Rooftop of Smoke → Chapter 6: The Trap → Chapter 7: The Pyre (3D spirit walk in the same palace) → Epilogue: The Long Flame (2D). In Unreal Engine 5.8, the palace is the level `L_Palace` (One File Per Actor, so several people can edit different actors at once; no World Partition streaming needed at this size). The 2D parts play as Level Sequences (`LS_Prologue`, `LS_Epilogue`) with `WBP_Cutscene` over them; the Chapter 6 trap is an in-engine sequence in `L_Palace` (`LS_Trap`); Chapter 7 reuses `L_Palace` with the ghost look. The title screen is `L_Title` and mechanics are tested in `L_Test`.

**How a chapter plays (10.11, 11.11):** the palace is **open** from Chapter 1 (everything except the restricted King's Chamber and the locked City Gate). Chapters 1–6 are each **a day and a night**: by day the player reads the main quest (the chapter question), finds the real characters where the chapter puts them, gathers clues for the Rumor Journal, completes two "do" objectives and avoids the guards (hiding spots, eavesdropping); at night Sahra dances; then comes the key conversation (needs 3 of the chapter's 6 clues, not in Chapters 6 and 7) and an in-engine ending where the player keeps control. NPC positions, guard routes and the palace's look (moon, banners, blossoms, lanterns) change per chapter and between day and night. Chapter 7 has no guards and no fail state.

---

## 1. Level Design Philosophy
1. **Guidance:** the palace is open, so the main quest, the characters who are present and lit eavesdrop spots guide the player instead of locked doors. The goal line on the HUD names the target ("Why does the other dancer carry a blade?"), with the two "do" objectives under it. Ambient NPC barks point at people worth finding ("Ask the old priest, he never sleeps"). At nightfall a bell rings, the goal line changes to "Perform in the Throne Hall" and the Throne Hall is lit brightest. Lanterns and firelight (Old gold, Ember orange) pick out details in plum and burgundy shadows; the restricted King's Chamber is marked with Rose red. Characters show an "E: Talk" prompt; hiding spots have a soft gold glow. **Proposed cues (12.11, Merve to confirm):** each real character stands under or next to a warm light source (a lantern, a brazier, a lit doorway), so a lit figure reads as "someone to talk to" and ambient NPCs stand in cooler, dimmer light; eavesdrop stops have a lantern on a post and a hiding spot within 2 m; the bell sound plays from the Throne Hall at nightfall so the player can follow it; the fountain's sound is the hub landmark (audible from every doorway); Zafir is placed where the player will pass, never hidden.
2. **Teaching structure:** Chapter 1 introduces walking, talking, clues, hiding and the garden patrol safely (one patrolling guard, friendly side characters, a 30 s dance) → Chapter 2 adds a timed threat (the search) and a second stealth stop → Chapter 3 adds Vashir's Suspicion in conversation, a restricted door to scout and a clue that needs another clue first (Aisha) → Chapter 4 adds following (tail Zafir) → Chapter 5 turns it around (being followed by the king's servant; doubled guards) → Chapter 6 drops the challenge for the story (fixed outcome) → Chapter 7 is a quiet walk with no fail state.
3. **Layout style:** Hub and spokes, fully open. The Garden of Dusk is the hub; every route passes through it, so its guard patrol is the stealth challenge in every chapter. Each room has at least one hiding spot near its door, and each chapter has 1–2 eavesdrop stops (7 eavesdrops in the game). The palace changes with the month (11.11): the moon (full, waning half, dark, waxing half, almost full, full), the festival banners (from Chapter 4), plum blossoms (Chapter 5), more or fewer guards and lanterns, and in Chapter 7 a ghost version of the same level.

**Scale standard:** Palace about 60 × 30 m (6000 × 3000 cm); walk 2.2 m/s, run 4.5 m/s (crossing 60 m takes about 27 s walking or 13 s running); guard sight 9 m in a 70° cone. No jumping. **Proposed (12.11, Merve to confirm):** player capsule 180 cm high (UE Third Person default, radius 42 cm); doors 160 cm wide and 300 cm high (the palace feels tall); corridors 300 cm wide so a guard and Sahra can pass with room to hide; stairs to the Rooftop Terrace 400 cm rise in two flights.

---

## 2. Mechanic Vocabulary
> Every level is built ONLY from these verbs. No level may use a mechanic that is not in this table.

| Verb / mechanic | Who uses it | GDD source | Level use |
| :-- | :-- | :-- | :-- |
| Walk / run | Sahra | 2 Controls, 1 Mechanics 5 | Crossing the garden hub between rooms; running in a guard's view raises Suspicion (+12/s) |
| Talk (type or pick reply) | Sahra ↔ Zafir, Vashir, Nura, Idris, Omar, Ilyas, Tariq, Aisha | 1 Mechanics 1 | Key scenes (Zafir 11 turns, Vashir 7) and clue talks (side characters 5 turns at most) |
| Bark (one line) | Sahra ↔ about 10 ambient NPCs | 1 Mechanics 9 | E gives one line that changes per chapter; some point to a real character |
| Interact (E) | Sahra | 2 Controls | Start conversations; enter and leave hiding spots; hide the knife (Chapter 2); check the servants' passage and the City Gate (Chapter 6); pick up soul embers (Chapter 7) |
| Guard sight / Suspicion | Guards, Vashir, the dance | 1 Mechanics 5 | Garden patrol every chapter; extra guards in Chapters 2, 4 and 5; the King's Chamber door from Chapter 3 |
| Restricted room | Level | 3 Level Design | King's Chamber (+20/s in view) except when invited (Chapter 3 private dance, Chapter 6) |
| Gather clues (Rumor Journal) | Sahra | 1 Mechanics 6 | 6 clues per palace chapter (36 in all): talk, eavesdrop, "do" and dance-reward clues; any 3 open the key scene |
| Hide (E) | Sahra | 1 Mechanics 8 | Curtains, lattice screens, alcoves; at least one per room, near its door |
| Eavesdrop | Sahra, guards, nobles | 1 Mechanics 8 | Stops next to a hiding spot; stay unseen about 10 s for the whole exchange |
| Follow (tail) | Sahra → Zafir | 1 Mechanics 5 | Chapter 4: tail Zafir to the City Gate at the midnight bell |
| Be followed / shake off | King's servant → Sahra | 1 Mechanics 5 | Chapter 5: break line of sight, then double back and corner him |
| Day / night phase | Level | 1 Gameplay | Lighting, NPC positions and the goal change at nightfall |
| Checkpoint | Level | 1 Win and Lose | Dancers' Quarters, morning of each chapter |
| Nightly dance | Sahra | 1 Mechanics 7 | Throne Hall (Chapters 1, 2, 4), King's Chamber (Chapters 3 and 6), duet in the Throne Hall (Chapter 5) |
| Gather soul embers | Sahra's spirit | 1 Mechanics 10 | Chapter 7: 6 embers, any order, no guards |
| Cutscene (2D) | Game | 3 Themes | Prologue and Epilogue only; every chapter ends in-engine |

---

## 3. Palace Rooms (all chapters)

```
                  [Rooftop Terrace]  (upper level, view toward the Crimson Zenith)
                         | stairs
[Dancers' Quarters] ---- [ Garden of Dusk (hub) ] ---- [Guard Post] ---- [City Gate] (locked)
 (practice room,           fountain (running),
  start, checkpoint)       lattice screen = hide + eavesdrop
                                   |
                            [Throne Hall] ---- [King's Chamber] (restricted, Rose red)
                                                    | curtain
                                             [servants' passage]
```

| Room | Look and mood | Hiding spots | Eavesdrop stops | Notes |
| :-- | :-- | :-- | :-- | :-- |
| Garden of Dusk (hub) | Moonlit garden, lanterns, the running fountain; night plum and old gold | Lattice screen (by the guards' stop), 2 alcoves along the colonnade (*proposed*) | Lattice screen stop (Ch 1, 3, 6) | Every route crosses it; one guard patrols it in every palace chapter |
| Dancers' Quarters | Where the dancers live; the practice room with a mirror wall and braziers; stairs up | Curtain by the door; 3 knife hiding places in Chapter 2 (loose tile, costume chest, behind the brazier screen, *proposed*) | — | Start and checkpoint; safe ground, except Chapter 2 (search) and Chapter 5 (doubled guard) |
| Throne Hall | Vashir's court: burgundy curtains, gold, firelight; the dance floor before the throne | Curtain alcove behind the pillars | Nobles' corner (Ch 5) | Nightly dance; Vashir's audience in Chapter 3 |
| King's Chamber | Vashir's private room in Rose red; candles he pinches out; a curtain hiding the servants' passage | — (restricted) | — | Restricted: +20 Suspicion/s in view without an invitation; private dance (Ch 3), the trap (Ch 6) |
| Guard Post | Guards' base: racks, a brazier, a duty board; beside the locked City Gate | Alcove behind the weapon rack | Guards' stop by the Guard Post (Ch 2, 4, 5) | Idris here in Chapters 1–4; the City Gate stays locked (only Idris has the key) |
| Rooftop Terrace | Upper level; smoke and moonlight; the glow of the Crimson Zenith beyond the walls | Chimney alcove | — | Ilyas in Chapters 1–3; the Chapter 5 key scene |

---

## 4. LEVEL 1: Chapter 1 · The Garden of Dusk

**Theme / mood:** Sahra's first day; 30 nights to the festival; full moon over the Garden of Dusk; lanterns, the running fountain; night plum and old gold. Palace music loop (oud, frame drum, drone); footsteps on stone; the fountain.
**Threats:** One guard patrols the garden (plus a second guard who meets him at the lattice screen stop).
**Main quest:** "Why does the other dancer carry a blade?" Find 3 of the 6 clues, dance, then confront Zafir in the garden.
**Do 1:** learn the garden guard's patrol and reach the lattice screen unseen, then listen (gives `ch1_guards_blade`). This teaches hiding.
**Do 2:** steal Zafir's knife during the key scene (it goes into the inventory for the whole game). A timed prompt "[E] Take his knife" shows for a few seconds when he is distracted (he leans close, or after `sahra_confronts_blade`); success → he says "Careful, Little Ember. Blades that small cut their owners first." and lets her keep it; a try at a bad moment → he catches her wrist, Trust down, he turns angry. The game sets beat `knife_taken`, not the LLM (Phase 7).

### Layout (who is where)
```
                 [Rooftop Terrace]          [Throne Hall]               [King's Chamber] (restricted)
                   Ilyas (priest)             Omar (setting out wine)
                        | stairs               NIGHT: dance 1 (30 s), Vashir watches
[Dancers' Quarters] -- [ Garden of Dusk (hub) ] -- [Guard Post] -- [City Gate] (locked)
  START / checkpoint     Tariq off duty by the fountain   Captain Idris
  Nura, Aisha            guard patrol loop                guard 2 (meets guard 1
                         lattice screen = hide + eavesdrop  at the lattice stop)
                         NIGHT: Zafir by the fountain
```

### Progression
- **1. Introduce (day):** Sahra starts in the Dancers' Quarters (safe, no guard) and talks to Nura to learn talking and the Rumor Journal (`ch1_nura_disappears`). Aisha is here too.
- **2. Combine (day):** cross the garden while the guard patrols; learn to hide behind the lattice screen and eavesdrop on the two guards (`ch1_guards_blade`, Do 1); question Omar in the Throne Hall (`ch1_omar_kings_door`) or Tariq by the fountain (`ch1_tariq_soldier_walk`); a second talk with Nura after any other clue gives `ch1_nura_lantern_house`. Zafir passes Sahra once in a corridor and says nothing.
- **3. Twist (night):** a bell; dance 1 in the Throne Hall (30 s) before the court; Vashir watches from the throne but can't be talked to yet. A great dance gives `ch1_nobles_doors`.
- **4. Final test (key scene):** with 3 clues, Zafir waits by the fountain: `Ch1_ZafirKeyScene`, 11 turns, beats `sahra_confronts_blade`, `uneasy_truce`; Sahra steals his knife (Do 2).
- **In-engine ending:** Sahra walks back to the Dancers' Quarters with the knife in her hand; the next chapter card fades in over the moonlit garden.

### Clues
| Clue id | Where and how |
| :-- | :-- |
| `ch1_nura_disappears` | Nura, Dancers' Quarters (talk) |
| `ch1_guards_blade` | Two guards behind the lattice screen (eavesdrop, Do 1) |
| `ch1_omar_kings_door` | Omar, Throne Hall (talk) |
| `ch1_tariq_soldier_walk` | Tariq, Garden of Dusk (talk) |
| `ch1_nura_lantern_house` | Nura, second talk after any other clue |
| `ch1_nobles_doors` | Nobles in the Throne Hall after a good dance (reward) |

### Secrets & extras
- Captain Idris at the Guard Post (5 turns): short and formal; if Sahra says "Tamra" he pauses and tells her not to say that name in the palace. No clue this chapter (small-talk node).
- Ilyas on the Rooftop Terrace: small talk about the fires (no clue this chapter).
- Ambient NPCs: barks about "the new dancer", the rumor of a blade and the cold nights.
- No hidden collectibles (none in the GDD).

### Enemy / hazard notes
- Guard 1: garden patrol loop around the hub, 30 s loop (*proposed*: fountain → Dancers' Quarters door → lattice screen stop (10 s, talks with guard 2) → Throne Hall door → Guard Post door → fountain).
- Guard 2: stands at the Guard Post door; walks to the lattice screen stop to meet guard 1 once per loop (*proposed*).
- At night both guards stand at the Throne Hall doors during the dance, then guard 1 resumes the garden loop (*proposed*).

---

## 5. LEVEL 2: Chapter 2 · The Practice Room

**Theme / mood:** About a week later; 22 nights to the festival; waning half moon; a nervous palace; the Dancers' Quarters turned over by a search at the afternoon bell.
**Threats:** The garden patrol, plus one extra guard outside the Dancers' Quarters until the search ends; Suspicion is higher around the Dancers' Quarters today.
**Main quest:** "Why are the guards searching the dancers?" Keep the stolen knife hidden, find 3 of the 6 clues, then rehearse with Zafir.
**Do 1:** hide the knife before the search reaches the Dancers' Quarters: pick one of 3 hiding places (Aisha's tip helps). Wherever she hides it, the guards check the spot and find it empty: Zafir watched her hide it and lifted it minutes before. If she didn't hide it, Zafir brushes past her and takes it. They search him first and find nothing.
**Do 2:** slip out to the Guard Post unseen and listen at the guards' stop to learn the search hour (`ch2_guards_search_hour`).

### Layout (who is where)
```
                 [Rooftop Terrace]          [Throne Hall]
                   Ilyas                       Omar
                        | stairs               NIGHT: dance 2 (33 s)
[Dancers' Quarters] -- [ Garden of Dusk ] -- [Guard Post] -- [City Gate] (locked)
  Nura, Aisha, Zafir      guard patrol          Captain Idris (giving orders)
  3 knife hiding places                          Tariq (off shift)
  extra guard at the door                        guards' stop = eavesdrop (Do 2)
  NIGHT: practice room = key scene
```

### Progression
- **1. Introduce (morning):** Nura pairs Sahra with Zafir for a festival duet; rehearsals start tonight. Sahra still has Zafir's knife.
- **2. Combine (day):** hide the knife (Do 1); slip past the extra guard and the garden patrol to the Guard Post stop (Do 2); question Idris, Omar, Aisha and Tariq (Tariq wants a small favor first).
- **3. Twist (afternoon bell):** the search turns the Dancers' Quarters over; the hiding place is empty; Zafir is searched first and found clean.
- **4. Final test (night):** dance 2 (33 s) in the Throne Hall; then `Ch2_ZafirRehearsal` in the practice room after the court sleeps, 11 turns, beats `sahra_asks_why`, `one_true_thing` (Trust 30).
- **In-engine ending:** Sahra alone in the practice room with the knife back in her hand; the lanterns go out one by one. Closing line depends on the hiding place: obvious or none: "Hide it better, Little Ember."; the best one: "Not bad. It took me a whole minute."

### Clues
| Clue id | Where and how |
| :-- | :-- |
| `ch2_idris_search_order` | Captain Idris, Guard Post (talk) |
| `ch2_guards_search_hour` | Two guards at their stop by the Guard Post (eavesdrop, Do 2) |
| `ch2_omar_someone_asked` | Omar, Throne Hall (talk) |
| `ch2_aisha_loose_tile` | Aisha, Dancers' Quarters (talk) |
| `ch2_tariq_wagers` | Tariq, Guard Post (talk, after a small favor) |
| `ch2_nobles_kings_table` | Nobles in the Throne Hall after a good dance (reward) |

### Secrets & extras
- The three knife hiding places rank obvious / fair / best (*proposed*: costume chest = obvious, loose tile = fair (Aisha says the guards know it), behind the brazier screen = best).
- Ilyas on the rooftop: small talk only.
- Barks turn nervous: dancers whisper about the search, guards joke about blades under silk.

### Enemy / hazard notes
- Guard 1: garden loop as in Chapter 1.
- Guard 2: Guard Post; stops with a third guard at the guards' stop (eavesdrop, Do 2).
- Guard 3 (extra): stands outside the Dancers' Quarters door until the search ends, then leaves with the search party (*proposed* timing: from morning to the afternoon bell).
- Search party: 2 guards walk a scripted path through the Dancers' Quarters at the afternoon bell (scripted, they do not raise Suspicion if Sahra stays out of the way).

---

## 6. LEVEL 3: Chapter 3 · The Private Dance

**Theme / mood:** About a week later; 15 nights to the festival; dark moon; the garden is darker, with fewer lanterns lit; Vashir's court in burgundy and gold; the rose-red King's Chamber.
**Threats:** The garden patrol; one guard at the King's Chamber door; Vashir's probing questions (+10 / −5 Suspicion per turn); the King's Chamber is restricted (+20/s in view) until the invitation.
**Main quest:** "How does a dancer get a private audience with the king?" Learn what pleases Vashir (3 of 6 clues) before his audience at nightfall, then win his invitation.
**Do 1:** sneak to the King's Chamber door and scout it (restricted area, guard sight). A guard turns her way and Zafir steps in to ask him a question, drawing his eyes off her (scripted save, no Suspicion). Gives `ch3_door_scouted`.
**Do 2:** find Ilyas on the Rooftop Terrace before the dusk bell sends him back to the temple (`ch3_ilyas_ash_at_dusk`).

### Layout (who is where)
```
                 [Rooftop Terrace]          [Throne Hall]                   [King's Chamber] (restricted)
                   Ilyas until the dusk bell  Omar (pouring wine)             door guard; Zafir at the door (scouting)
                        | stairs              NIGHTFALL: Vashir's audience     NIGHT: private dance (36 s)
[Dancers' Quarters] -- [ Garden of Dusk ] -- [Guard Post]
  Nura                  Aisha                   Captain Idris
                        guard patrol, fewer lanterns
                        lattice screen = eavesdrop
                        LATE NIGHT: Zafir by the fountain
```

### Progression
- **1. Introduce (day):** clues about what pleases Vashir; Ilyas leaves at the dusk bell (Do 2 is timed).
- **2. Combine (day):** scout the King's Chamber door (Do 1); Aisha only talks after the guards' eavesdrop (`ch3_guards_sent_away` first: a clue that needs another clue).
- **3. Twist (nightfall):** `Ch3_VashirAudience` in the Throne Hall, 7 turns, beats `faith_shown`, `invitation_given` (Favor 60). The player's clues become suggested replies. Below Favor 60 the invitation still comes, later and cooler, through Nura.
- **4. Final test (night):** the private dance for Vashir in the King's Chamber (36 s; no reward clue; Sahra notices a draft behind one curtain, which seeds the servants' passage). Then `Ch3_ZafirFountain` by the fountain, 11 turns, beats `jealousy_shows` (romance route, Affection 40), `joke_covers_it`.
- **In-engine ending:** the player walks Sahra back through the dark garden, past the fountain where Zafir stood.

### Clues
| Clue id | Where and how |
| :-- | :-- |
| `ch3_nura_dawn_dances` | Nura, Dancers' Quarters (talk) |
| `ch3_ilyas_ash_at_dusk` | Ilyas, Rooftop Terrace before the dusk bell (talk, Do 2) |
| `ch3_omar_home_and_family` | Omar, Throne Hall (talk) |
| `ch3_aisha_one_lie` | Aisha, Garden of Dusk (talk, only after `ch3_guards_sent_away`) |
| `ch3_guards_sent_away` | Two guards at their stop by the lattice screen (eavesdrop) |
| `ch3_door_scouted` | Scouting the King's Chamber door (Do 1); Captain Idris says the same if asked |

### Secrets & extras
- The draft behind the curtain in the King's Chamber (a curtain that moves; *proposed*: a cloth physics or vertex-animated curtain and a soft wind sound).
- Barks: who will the king choose? After the audience, people step aside for Sahra in the corridors.

### Enemy / hazard notes
- Guard 1: garden loop (darker garden; the lattice stop with guard 2).
- Guard 2: Guard Post; meets guard 1 at the lattice stop.
- Guard 4: stands at the King's Chamber door; turns toward the corridor on a timer (the Do 1 scripted moment fires when Sahra enters the door zone).
- King's Chamber: restricted volume except during the private dance.

---

## 7. LEVEL 4: Chapter 4 · The King's Favor

**Theme / mood:** About a week later; 8 nights to the festival; waxing half moon; the first festival banners; envy around the king's new favorite.
**Threats:** The garden patrol; one extra guard at the City Gate at night; the tail (stay out of Zafir's sight and the guards' view).
**Main quest:** "Who is Zafir writing to?" Find 3 of the 6 clues, then follow him at the midnight bell.
**Morning (in-engine):** the king's servant brings a jeweled anklet "to replace that old cord" and takes the cord away "to be blessed at the temple". The same morning Sahra sees Zafir pass a sealed letter to Omar and catches the royal wax. The anklet replaces the cord on Sahra's model.
**Do 1:** tail Zafir at the midnight bell to the City Gate without being seen (follow, keep distance). She watches him hand a sealed letter to the king's rider. Required: the key scene starts when the tail ends.
**Do 2:** find Omar, who carries Zafir's letters, and get him to talk (`ch4_omar_letters`).

### Layout (who is where)
```
                 [Rooftop Terrace]          [Throne Hall]
                                              NIGHT: dance 4 (39 s), anklet on
                        | stairs              Omar somewhere between here and the City Gate
[Dancers' Quarters] -- [ Garden of Dusk ] -- [Guard Post] -- [City Gate]
  Nura, Aisha (cool)     Tariq                  Captain Idris    NIGHT: extra guard; the king's rider
  practice room =        guard patrol           guards' stop = eavesdrop
  key scene              first banners
  MIDNIGHT: Zafir leaves → tail route → City Gate
```

### Progression
- **1. Introduce (morning):** the anklet and the cord; Zafir's letter to Omar.
- **2. Combine (day):** find Omar (he moves between the Throne Hall and the City Gate, *proposed*: three stops on a slow loop); question Nura, Idris and Tariq (for a coin); eavesdrop at the guards' stop. Zafir keeps his distance all day.
- **3. Twist (night):** dance 4 (39 s) in the Throne Hall wearing the anklet; at the midnight bell, tail Zafir to the City Gate (Do 1): stay within 15 m but out of his view; he looks back at set points (*proposed*: 3 look-back points with hiding spots nearby); losing him or being seen restarts the tail from its start.
- **4. Final test (key scene):** `Ch4_ZafirKnife` in the practice room, 11 turns, with Sahra's knife at his throat; beats `zafir_reveals_target` (Trust 50), `alliance_agreed`.
- **In-engine ending:** the knife lowered, Zafir leaves the practice room first; Sahra stands alone, looking at the anklet the king gave her.

### Clues
| Clue id | Where and how |
| :-- | :-- |
| `ch4_omar_letters` | Omar, once found (talk, Do 2) |
| `ch4_nura_recommendation` | Nura, Dancers' Quarters (talk) |
| `ch4_idris_rider` | Captain Idris, Guard Post (talk) |
| `ch4_guards_dance_houses` | Two guards at their stop by the Guard Post (eavesdrop) |
| `ch4_tariq_sold_gossip` | Tariq, Garden of Dusk (talk, for a coin) |
| `ch4_nobles_cord` | Nobles in the Throne Hall after a good dance (reward) |

### Secrets & extras
- Ilyas: small talk only this chapter (*proposed*: on the Rooftop Terrace).
- Barks: envious and uneasy about the king's new favorite; the first banners go up.

### Enemy / hazard notes
- Guard 1: garden loop.
- Guard 2: Guard Post and the guards' stop.
- Guard 5 (extra, night): stands at the City Gate; the tail route passes behind him through the Guard Post alcove (*proposed*).
- Zafir during the tail: a scripted NavMesh path with 3 look-backs (sight cone 60°, 8 m, *proposed*); being seen by Zafir or a guard fails the tail (no Suspicion from Zafir).

---

## 8. LEVEL 5: Chapter 5 · Rooftop of Smoke

**Theme / mood:** 3 nights to the festival; the moon almost full; plum blossoms in the garden; smoke and moonlight on the rooftop; romance.
**Threats:** The garden patrol; doubled guards around the Dancers' Quarters; the king's servant follows Sahra all day.
**Main quest:** "Who is watching me?" Shake off the king's servant, find 3 of the 6 clues, then corner him.
**Do 1:** shake off the king's servant (break line of sight, use the hiding spots and patrol gaps), then double back and corner him (`ch5_servant_cord_stolen`).
**Do 2:** overhear two nobles in the Throne Hall without being seen (`ch5_nobles_desert_weave`).

### Layout (who is where)
```
                 [Rooftop Terrace]            [Throne Hall]
                   NIGHT: key scene            Omar; two nobles (eavesdrop, Do 2)
                   view to the Crimson Zenith  NIGHT: duet with Zafir (42 s)
                        | stairs
[Dancers' Quarters] -- [ Garden of Dusk ] -- [Guard Post]
  Nura                   Ilyas (back from the temple)
  2 extra guards         guard patrol, plum blossoms
  Captain Idris walking  the king's servant follows Sahra everywhere
  the doubled guard
```

### Progression
- **1. Introduce (morning):** the cord never came back; the servant turns up wherever Sahra goes.
- **2. Combine (day):** shake him off (he loses her after she breaks line of sight for about 3 s, *proposed*); question Nura, Idris and Ilyas; eavesdrop on the guards and on the nobles (Do 2). Aisha avoids Sahra. Zafir is not seen until the duet.
- **3. Twist (day):** double back and corner the servant: a fixed 3-line exchange (no LLM) gives `ch5_servant_cord_stolen`.
- **4. Final test (night):** the public duet with Zafir in the Throne Hall (42 s); a good dance earns Vashir's invitation for the night before the festival (a bad one still brings it, cooler, through Nura). Then `Ch5_ZafirRooftop` on the Rooftop Terrace, 11 turns, beats `cord_returned`, `come_back_out`; the confession at Affection 60 (romance route).
- **In-engine ending:** the rooftop at night, the mended cord on her ankle; the player can look out toward the glow of the Crimson Zenith before the chapter ends. The mended cord replaces the anklet on Sahra's model.

### Clues
| Clue id | Where and how |
| :-- | :-- |
| `ch5_nura_kings_eyes` | Nura, Dancers' Quarters (talk) |
| `ch5_nobles_desert_weave` | Two nobles in the Throne Hall (eavesdrop, Do 2) |
| `ch5_guards_servant_beaten` | Two guards at their stop by the Guard Post (eavesdrop) |
| `ch5_idris_doubled_guard` | Captain Idris, near the Dancers' Quarters (talk) |
| `ch5_ilyas_thread_read` | Ilyas, Garden of Dusk (talk) |
| `ch5_servant_cord_stolen` | The king's servant, once cornered (Do 1) |

### Secrets & extras
- The view from the rooftop toward the Crimson Zenith (a distant glow card; *proposed*: a lit matte painting behind the walls).
- Barks: quiet and watchful around Sahra.

### Enemy / hazard notes
- Guard 1: garden loop.
- Guard 2: Guard Post and the guards' stop.
- Guards 6 and 7 (doubled guard): two short loops around the Dancers' Quarters door and the stairs to the rooftop (*proposed*).
- Captain Idris walks with the doubled guard (talkable; does not raise Suspicion himself).
- The king's servant: follows at 6–10 m on the NavMesh; not a guard (does not raise Suspicion), but while he sees Sahra the "do" objective cannot progress.

---

## 9. LEVEL 6: Chapter 6 · The Trap

**Theme / mood:** The night before the festival; the moon almost full; banners everywhere, loud and busy, then strangely empty around the King's Chamber; the rose-red chamber; betrayal.
**Threats:** None that can fail: no clue gate, no dance Suspicion, fixed outcome.
**Main quest:** "Is everything ready?" Check each part of the plan yourself. Every check comes back right, and that is the problem.
**Do 1:** check the plan: the servants' passage (unlocked, swept clean), the City Gate (the guards' stop: it opens before dawn for the temple carts), the guard change. Each check gives one clue and each is a little too easy.
**Do 2:** the solo dance for Vashir alone in the King's Chamber (45 s), the last and longest.

### Layout (who is where)
```
                 [Rooftop Terrace]          [Throne Hall]               [King's Chamber] (open tonight)
                   Aisha                      Omar                         Vashir alone; NIGHT: solo dance (45 s)
                        | stairs                                            curtain → [servants' passage]
[Dancers' Quarters] -- [ Garden of Dusk ] -- [Guard Post] -- [City Gate]     (Zafir waits; guards hidden)
  Nura                   Ilyas (to the temple)   Tariq; Idris absent
                         banners everywhere      guards' stop = eavesdrop
                         Zafir seen once, from a distance
```

### Progression
- **1. Introduce (day):** festival preparations; the palace is loud.
- **2. Combine (day):** the plan checks (Do 1); Nura, Omar, Tariq and Ilyas each give one more "too easy" clue. The king's servant is nowhere to be found; Captain Idris is not at the Guard Post.
- **3. Twist (night):** no court dance; the solo dance for Vashir in the King's Chamber (Do 2), with Zafir's knife under her silks and the mended cord on her ankle. As it ends, Vashir's eyes go to the cord; the curtain opens and guards come out of the passage with Zafir held between them; Idris stands at the door.
- **4. Final test (key scene, fixed outcome):** `Ch6_VashirTrap`, 7 turns; beats `vashir_names_her`, `contract_revealed` (both forced by the Scene Director). Whatever Sahra says is saved as Zafir's memory.
- **In-engine ending (played, not watched, `LS_Trap`):** Vashir orders her taken alive; she fights with Zafir's knife; a guard cuts her down; Zafir breaks free and catches her; she dies in his arms. "Take her to the temple." The camera stays low with Sahra; fade to the Chapter 7 card.

### Clues
| Clue id | Where and how |
| :-- | :-- |
| `ch6_passage_unlocked` | The servants' passage behind the curtain (Do 1) |
| `ch6_guards_gate_carts` | Two guards at their stop by the lattice screen (eavesdrop, Do 1) |
| `ch6_nura_guards_dismissed` | Nura, Dancers' Quarters (talk) |
| `ch6_omar_servant_gone` | Omar, Throne Hall (talk) |
| `ch6_tariq_idris_called` | Tariq, Guard Post (talk) |
| `ch6_ilyas_no_village` | Ilyas, Garden of Dusk (talk) |

> Chapter 6 clues do not gate anything (no key-scene gate); they build the dread.

### Secrets & extras
- Barks: festival eve; busy and loud, then strangely empty around the King's Chamber.

### Enemy / hazard notes
- Guard 1: garden loop (cannot fail the chapter; Suspicion is off in Chapter 6, *proposed*).
- Guard 2: Guard Post and the guards' stop.
- The corridors around the King's Chamber are emptied of guards.
- Trap guards: 3 guards hidden in the servants' passage, holding Zafir (scripted, part of `LS_Trap`; *proposed* count).

---

## 10. LEVEL 7: Chapter 7 · The Pyre (spirit walk)

**Theme / mood:** Festival night; full moon; the same palace, empty and ghostly: desaturated, blurred, ash drifting upward; the court has gone to the Crimson Zenith and its fire glows red over the walls.
**Threats:** None: no guards, no fail state, any order.
**Main quest:** Gather the six embers of her soul.

### Layout (the six embers)
```
                 [Rooftop Terrace] ember 5: the cord
                        | stairs
[Dancers' Quarters] -- [ Garden of Dusk ] ---- [Guard Post] -- [City Gate] ember 4: the letters
 practice room           fountain: ember 1: the knife
 ember 2: the rehearsal          |
                          [Throne Hall] ember 3: the dances ---- [King's Chamber] ember 6: the fall
```

### Progression
- **1. Introduce:** the Chapter 7 card; Sahra's spirit stands in the Dancers' Quarters; the HUD shows only an ember counter.
- **2. Combine:** each ember (`BP_SoulEmber`) replays one memory: a line the player actually said in that chapter, over a blurred glimpse of the scene; then one piece of Zafir's generated confession plays from the fire (6 pieces; pre-written if the AI fails).
- **3. Twist:** the City Gate and the King's Chamber are open in the ghost palace (no restricted volume, no lock).
- **4. Final test:** with the last ember, the fire over the walls turns from red to gold and the sky lightens for one heartbeat. Cut to the Epilogue.

### Secrets & extras
- Each ember glows Ember orange with rising ash particles (Niagara, Tala); the route between embers is lit by faint footprints of light (*proposed*).

### Enemy / hazard notes
- None. All `BP_Guard`, ambient NPCs and real characters are hidden in Chapter 7.

---

## 11. 2D Parts: Prologue and Epilogue
- **Prologue: Ash:** backgrounds "Burning Tamra" and "The palace gate"; Tamra burns at the Festival of the Long Flame ten years ago, a girl walks out of the smoke; Sahra arrives at Sama'zar as a dancer from the Lantern House, 30 nights before the festival. Ends on the first chapter card over the Garden of Dusk.
- **Epilogue: The Long Flame:** background "The Crimson Zenith"; Sahra rises from the sacred fire unburned as the phoenix, turns its fire on Vashir, and the sun returns; final shot with Zafir's stolen knife: "Careful, Little Ember." Lines warmer on the romance route. Then Credits.

---

## 12. Difficulty Curve
| Level | New mechanic | Difficulty (1 to 5, *proposed*) | Notes |
| :-- | :-- | :-- | :-- |
| Chapter 1: The Garden of Dusk | Walk, talk, clues, hide, eavesdrop, dance 1 | 1 | 1 patrolling guard (+1 at the stop); 3 of 6 clues; 30 s dance; Zafir 11 turns |
| Chapter 2: The Practice Room | Timed search, hiding an item, second eavesdrop stop | 2 | Extra guard at the Dancers' Quarters; 33 s dance; Zafir 11 turns |
| Chapter 3: The Private Dance | Vashir's Suspicion, restricted door, a clue that needs a clue, timed Do 2 | 3 | Door guard; Vashir 7 turns (Favor 60); private dance 36 s; Zafir 11 turns |
| Chapter 4: The King's Favor | Following (tail) | 3 | Extra City Gate guard at night; 39 s dance; Zafir 11 turns (Trust 50) |
| Chapter 5: Rooftop of Smoke | Being followed, shaking off, cornering | 4 | Doubled guards; duet 42 s; Zafir 11 turns (Affection 60 for the confession) |
| Chapter 6: The Trap | Plan checks; fixed outcome | 1 | No fail state for the story; solo dance 45 s; Vashir 7 turns |
| Chapter 7: The Pyre | Gather embers | 1 | No guards, no fail state |

---

## 13. Build Notes
1. **Grey-box first** (pipeline Phase 8, Merve): block out the hub and all five rooms plus the practice room, the servants' passage and the City Gate at 60 × 30 m with BSP, Geometry Script or modeling-mode primitives in `L_Palace` (units in cm: about 6000 × 3000 cm); place doors, stairs, the running fountain, Player Start and the checkpoint; add a Nav Mesh Bounds Volume and check the green NavMesh with `P` in the viewport.
2. **Mechanics next** (Phases 7–13, Baha codes, Merve places): restricted volume for the King's Chamber (`BP_RestrictedVolume`), character spawn points per chapter and phase (`BP_NPCSpawnPoint`, tagged `Ch<N>.Day` / `Ch<N>.Night`), guard patrol routes (Target Point actors referenced by each `BP_Guard`), hiding spots (`BP_HidingSpot`), eavesdrop zones (`BP_EavesdropZone`, one per stop, with the clue id), ambient NPC spots (`BP_AmbientNPC`), the king's servant (`BP_KingsServant`), the knife hiding places (Chapter 2), the tail route (Chapter 4), the soul embers (`BP_SoulEmber`, Chapter 7). Every mechanic is proven in `L_Test` first.
3. **Look per chapter** (`UDayNightSubsystem`): moon phase, lantern count, banners (from Chapter 4), plum blossoms (Chapter 5), day and night Post Process Volumes, the Chapter 7 ghost post process. Actors that change per chapter carry a chapter tag so the subsystem can show or hide them.
4. **Art last** (Phase 18, Tuana; VFX Tala; lighting Merve): licensed modular palace kit (Fab), lantern lights, moonlight (Directional Light + Sky Atmosphere), Lumen global illumination, Niagara fire, embers and ghost ash. Early warning: if the palace is still grey-box at the end of week 2, custom models are cut.
5. **Chapter content table:**

| Chapter | Moon / look | Real characters by day | Guards | Key scene place |
| :-- | :-- | :-- | :-- | :-- |
| 1 | Full; no banners | Nura, Aisha (Dancers' Quarters); Idris (Guard Post); Omar (Throne Hall); Ilyas (Rooftop); Tariq (fountain); Zafir passes once | Garden patrol + Guard Post | Garden of Dusk, fountain |
| 2 | Waning half; search | Nura, Aisha, Zafir (Dancers' Quarters); Idris, Tariq (Guard Post); Omar (Throne Hall); Ilyas (Rooftop) | + extra guard at the Dancers' Quarters; search party | Practice room |
| 3 | Dark; fewer lanterns | Nura (Dancers' Quarters); Aisha (garden); Omar (Throne Hall); Ilyas (Rooftop, until dusk); Idris (Guard Post); Zafir at the King's Chamber door | + King's Chamber door guard | Throne Hall (Vashir), then the fountain (Zafir) |
| 4 | Waxing half; first banners | Nura, Aisha (Dancers' Quarters); Omar (Throne Hall ↔ City Gate); Idris (Guard Post); Tariq (garden) | + City Gate guard at night | Practice room |
| 5 | Almost full; plum blossoms | Nura (Dancers' Quarters); Idris (with the doubled guard); Ilyas (garden); Omar (Throne Hall); the king's servant follows | + 2 guards around the Dancers' Quarters | Rooftop Terrace |
| 6 | Almost full; banners everywhere | Nura (Dancers' Quarters); Omar (Throne Hall); Ilyas (garden); Aisha (Rooftop); Tariq (Guard Post); Idris absent | Garden + Guard Post; trap guards in the passage | King's Chamber (Vashir) |
| 7 | Full; ghost palace | None (Zafir's voice from the fire) | None | — (6 embers) |

---

## Section Status
| Section | Status |
| :-- | :-- |
| World overview | FILLED (12.11: 7 chapters, Sama'zar, Garden of Dusk) |
| 1. Philosophy | FILLED (guidance cues and scale sizes *proposed*, Merve to confirm) |
| 2. Mechanic Vocabulary | FILLED |
| 3. Palace Rooms | FILLED (hiding spots partly *proposed*) |
| 4.–9. Chapters 1–6 | FILLED (patrol waypoints and timings *proposed*) |
| 10. Chapter 7 | FILLED |
| 11. 2D Parts | FILLED |
| 12. Difficulty Curve | FILLED (*proposed* values) |
| 13. Build Notes | FILLED |
