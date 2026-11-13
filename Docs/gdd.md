# Ashes for a Goddess
## Game Design Document (gdd.md)

> *"Some gods brighten the sky when they are born; others, when they die."*

**Author:** Burnt Potato Games (BPG): Merve Özal, Tuana Tuna, Tala Alsayed, Ebrar Baha Özüpek · **Version:** 1.4 (Markdown version of the midterm GDD dated 07.11.2026; 4-week schedule; "Court of Whispers" investigation concept added 09.11.2026; day/night loop, nightly dance and guard stealth added 10.11.2026; engine changed to Unreal Engine 5.8 and the AI model built into the game on 10.11.2026; 7-chapter story, living palace and spirit chapter added 11.11.2026; character, scene and clue data, turn limits, offline trees and the team of four added 12.11.2026) · **Date:** 12.11.2026
**Genre:** Third-person narrative RPG and visual novel hybrid; dark fantasy romance · **Platform:** Windows PC, keyboard and mouse (Unreal packaged build) · **Engine:** Unreal Engine 5.8, C++ and Blueprints, 3D, Lumen · **Players:** Single player

| Field | Details |
| :-- | :-- |
| Course | Advanced Game Programming (midterm GDD in Week 8, final build in this 4-week plan) |
| Target player | Players aged 16+ who enjoy story-driven and romance games; visual novel fans curious about AI characters |
| Play time | About 45–60 minutes per playthrough (*proposed*, changed 11.11): 6 palace chapters of about 6–8 minutes (day investigation, night dance, key conversation), the Chapter 7 spirit walk (about 4 minutes) and the 2D Prologue and Epilogue. The midterm GDD said 12–15 minutes; the 10.11 plan said 25–30 minutes for 4 chapters |
| Rating target | Teen: violence and death are shown; the romance is not explicit |

| Member | Role | Owns |
| :-- | :-- | :-- |
| Merve Özal | Team Lead, Level Designer, Narrative Lead | Level (palace grey-box, rooms, spawn points, guard routes, hiding and eavesdrop spots, lighting), story and lines with Claude, data entry, tracker and board; wrote the code for Phases 0–5 (movement, interaction, built-in AI model, prompt builder, validator, conversation screen) |
| Tuana Tuna | Lead Artist, UI Artist, Narrative Designer	 | Portraits, character looks, palace kit, UI style, 2D backgrounds; reviews the writing |
| Tala Alsayed | Audio and VFX Designer | Music, dance songs, dance charts and animations, sound effects and mix, Niagara effects, cutscene sequences, credits, licenses, playtests |
| Ebrar Baha Özüpek | Lead Programmer and Gameplay Systems | Gameplay code from Phase 7: Scene Director, offline trees, clues, guards, dance, cutscene player, the spirit chapter, packaging |

> **Team update (12.11.2026).** Tala and Baha joined Merve and Tuana. The work is split by role (table above); `production_pipeline.md` and `Docs/workplan/index.html` list who owns each phase and each day's task.

> **Concept update (09.11.2026): "Court of Whispers".** The team chose to make the palace an **open palace** that the player explores freely, talking to **several NPCs to gather information (clues)**, with cutscenes as the story advances. On 10.11.2026 the team fixed the three active parts: **gather information (Rumor Journal)**, **avoid patrolling guards** and **dance every night** to keep Sahra's cover, in a **day / night loop** per chapter. New parts are marked **(new 09.11)**, **(new 10.11)**, **(new 11.11)** or **(new 12.11)**; numbers marked *proposed* are starting values for playtests, not from the midterm GDD.

> **Story update (11.11.2026): 7 chapters and a living palace.** The story now spans one month, full moon to full moon, in **7 chapters, all played in 3D**: six palace chapters (each a day and a night ending in a free-talk key scene) and **Chapter 7, played as Sahra's spirit**. Only the Prologue and Epilogue are 2D. The palace is a **living palace**: about 10 ambient NPCs give one line each (barks), while the 8 real characters (Zafir, Vashir, Nura, Idris and 4 minor characters) must be found and hold full LLM conversations. Each chapter has a main quest and two "do" objectives, and ends in the engine, not in a cutscene. The full story, cards and lines are in the team's **Master Lore** document (Claude Doc), which is the single source for all game text.

> **Engine note (changed 10.11.2026).** The midterm GDD planned Unity 6 LTS. On 10.11.2026 the teacher approved the switch to **Unreal Engine 5.8** (C++ and Blueprints). Every document in `Docs/` now uses Unreal terms. The earlier Unity skeleton is kept in Git as the tag `unity-skeleton`.

---

## Index
1. Game Design: Pitch, Characters, Gameplay, Mindset, Uniqueness, Design Pillars, Out of Scope
2. Technical: Screens, Controls, Camera, Mechanics
3. Level Design: Philosophy, Themes, Game Flow
4. Development: Abstract Classes, Derived Classes, Data, AI
5. Graphics: Style Attributes, Graphics Needed
6. Sounds & Music: Style Attributes, Sounds Needed, Music Needed
7. Schedule
8. Appendix: In-Game AI Design, AI Tool Plan, References (GDD content that has no heading in this template)

---

## 1. Game Design

### Pitch
An assassin disguised as a dancer sneaks into Sama'zar, the palace of a king who burns a village every year to save a dying sun. The player is Sahra, the sole survivor of the burned village of Tamra, who explores the palace in third person while guards watch and talks to the characters in her own words. Instead of picking from a list, the player types anything (or picks one of 3 suggested replies), and the characters answer in character, remember what was said and treat Sahra differently because of it. The palace is open and alive: each chapter poses a question, and the player roams the Garden of Dusk and the rooms around it, drawing answers out of dancers, guards, servants and priests. They gossip, lie, bargain and clam up, and every useful answer becomes a clue in Sahra's Rumor Journal. By day she gathers clues while avoiding patrolling guards, slipping into hiding spots and eavesdropping where the guards gossip. Every night she must dance in the court to keep her cover: a rhythm minigame where a fine performance wins the king's favor and overheard secrets, and a clumsy one draws suspicion. Enough clues open the chapter's key conversation with Zafir or Vashir. Over a month to the Festival of the Long Flame, a rival assassin becomes her ally and, if she lets him, more; the king she came to kill has been hunting her all along. She dies in the trap in Chapter 6, walks the ghostly palace as a spirit in Chapter 7 while Zafir confesses at her pyre, and rises from the sacred fire as the phoenix. The main story and ending are fixed, but how the romance develops and what the characters reveal depends on the player.

### Characters
> Full game-format cards (voice, wants, fears, speech rules, knows, must not say, chapter secrets, emotions, values, spoiler keywords) are in the Master Lore document, tab "Character Cards", ready to enter into `DT_Characters`. Looks come from Tuana's character notes (same tab, "Looks and reference").

#### Sahra
- **Role:** Player character. Sole survivor of the village Tamra, burned ten years ago at the Festival of the Long Flame; trained by the Lantern House, a capital dance house that secretly trains assassins; the desert's best assassin, disguised as a dancer. She does not know she is the phoenix.
- **Look:** Dancer's silks, a veil and anklets; long dark hair, red-and-gold jewelry. Linked to the Ember orange color (#E8642C). Wears a cord of Tamra weave on her ankle (taken by the king in Chapter 4, returned mended by Zafir in Chapter 5).
- **Abilities:** Walk (2.2 m/s) and run (4.5 m/s) through the palace; talk to characters by typing up to 200 characters or picking 1 of 3 suggested replies; interact with highlighted characters and objects (E); hide in hiding spots (E) and eavesdrop from them; follow and shake off characters (Chapters 4 and 5); dance every night (rhythm minigame); in Chapter 7 she walks the palace as a spirit and gathers 6 soul embers. In the epilogue she walks unburned out of the sacred fire as the phoenix.
- **Personality / behaviour:** Voiced by the player's own words. She wants Vashir dead. She hides that she survived the fire at all, and that the Lantern House trains assassins.

#### Zafir
- **Role:** NPC driven by the LLM. A dancer in the royal troupe (he joined some months before Sahra), secretly an ash-walker (a blade for hire); rival, then ally, and the romance interest.
- **Look:** Long dark wavy hair, gold earrings and a gold circlet, dark clothing; only a thin line of candle ash. Linked to Old gold (#C9A25B). Carries a small knife that Sahra steals in Chapter 1.
- **Abilities:** Free conversation, **11 turns** per key scene (decided 12.11); hidden Trust (start 20) and Affection (start 10), each ±8 per turn; 6 emotion states (neutral, teasing, tender, jealous, cold, angry). Unlocks: Trust 30: tells one true thing ("I lost someone to a fire too", Chapter 2); Trust 50: admits he hunts Vashir and that his letters are false leads (Chapter 4); Trust 60 on the romance route: his village burned; Affection 40: jealousy shows (Chapter 3); Affection 60: the rooftop confession (Chapter 5). Remembers up to 6 memories; his generated confession in Chapter 7 quotes them.
- **Personality / behaviour:** Calls Sahra "Little Ember". Teases, speaks in fire metaphors, answers a question with a question, and jokes harder when he is hurt; once in a while he says one sentence with no joke in it, and those are the true ones. Turns jealous, then cold, when Sahra praises Vashir; angry only when she risks her life or turns his own knife on him. Hides that Vashir hired him a year ago to find and kill "the girl from Tamra"; he took the contract only to get close to Vashir, whose fires burned his village, and fed the king false leads. Hums an old harvest song from his village while he warms up.

#### Vashir
- **Role:** NPC driven by the LLM. The Priest-King of the Dunes, king and high priest at once; the antagonist.
- **Look:** Past fifty, tall and lean, with a close-cut silver beard; burgundy robes edged in gold, a sun disk on his chest, the heaviest candle ash at court. Kind dark eyes that never look away first. Linked to Burgundy (#6B1E2E).
- **Abilities:** Free conversation, **7 turns** per scene (decided 12.11: he has less patience; the Chapter 3 audience and the Chapter 6 trap); hidden Favor (start 30, ±6 per turn) and Suspicion (start 10, +10 / −5 per turn); 5 emotion states (serene, curious, pleased, suspicious, wrathful); at Favor 60 he invites Sahra to dance for him alone; at Suspicion 100 guards seize Sahra and the chapter restarts.
- **Personality / behaviour:** Slow, warm and soft, like a priest giving a blessing; calls Sahra "child". His questions sound like care and land like traps: home, family, faith, fire. Devotion (candle ash, the old dawn dances) pleases him more than skill; he hates a lie. Wants the phoenix's power, to live forever; every fire was a hunt for the phoenix. Pinches out candle flames with his bare fingers.

#### Nura
- **Role:** LLM character (decided 11.11: every real character is an LLM character). Head of the royal dancers; runs the nightly performances and judges Sahra's dance. Always in the Dancers' Quarters.
- **Look:** Late twenties, sharp-featured, black hair braided with small brass bells, kohl-lined eyes, plum silks; her candle ash is always perfect.
- **Abilities:** Free conversation, 5 turns at most; emotions neutral, amused, sharp, wary, sad; holds clues in Chapters 1, 3, 4, 5 and 6; brings the king's invitations.
- **Personality / behaviour:** Quick, mocking and nosy; trades a secret for a secret and turns every compliment into a jab. Wants to survive the court and keep her place. Hides that she lost a sister to a burning (may say it once, from Chapter 5).

#### Captain Idris
- **Role:** LLM character. Captain of the palace guard; sets the patrol routes and holds the only key to the City Gate. Found at the Guard Post (Chapters 1–4), walking the doubled guard (Chapter 5), absent in Chapter 6.
- **Look:** Short dark hair, brown skin, a little older; a rugged face with sharp features, calloused hands; cold like a soldier.
- **Abilities:** Free conversation, 5 turns at most; emotions neutral, stern, wary, troubled; holds clues in Chapters 2, 3, 4 and 5.
- **Personality / behaviour:** Short, formal, gives orders, not explanations; approves of a proper greeting, hand on heart. Hides that he is the soldier who saw a girl walk out of Tamra's fire; if Sahra says "Tamra", he pauses and tells her not to say that name in the palace. In Chapter 6 he is called to name her. At the festival fire he watches the crowd, never the flames.

#### Minor Characters (new 09.11; named 12.11)
- **Role:** 4 LLM characters with full conversations (5 turns at most) and 1–3 clues each per chapter: **Omar**, a palace servant, the quiet helper who carries Zafir's letters to the City Gate; **Ilyas**, a temple priest who fears the fires are not working; **Tariq**, an off-duty guard with gambling debts who sells gossip (and sold the Lantern House names to the king's rider); **Aisha**, a second dancer, the favorite the king invited this year and sent away within a week.
- **Look:** Licensed base models, recolored. MISSING: individual looks (Tuana).
- **Abilities:** Free conversation (5 turns); give clues, some for a small favor or a coin; offline small-talk nodes in chapters where they hold no clue.
- **Personality / behaviour:** Omar is polite and low and never finishes a sentence near nobles; Ilyas is gentle and devout and quotes the doctrine; Tariq is bored and blunt and complains about every shift; Aisha is honey-sweet in public and cutting in private.

#### The King's Servant (new 12.11)
- **Role:** Not an LLM character. Takes Sahra's cord in Chapter 4 (in the engine) and tails her in Chapter 5; when she corners him, a fixed 3-line exchange plays and gives a clue. Missing in Chapter 6.

#### Ambient NPCs (new 11.11)
- **Role:** About 10 courtiers, servants, guards, dancers and priests. E gives one line (a bark), then they turn away; barks change every chapter and react to events (60 lines in all).

#### Palace Guards
- **Role:** Enemies (**must have since 10.11**). 2–4 guards on fixed, timed patrol routes that the player can learn.
- **Look:** The same licensed base model, recolored.
- **Abilities:** View cone of 70° and 9 m; simple state machine Patrol → Notice (stops and looks while Suspicion rises) → back to Patrol; no chasing (simplified 10.11 for the 4-week scope); cannot see into hiding spots; pairs of guards stop at set points and talk (eavesdrop chances); raise Suspicion when Sahra runs in view, stays in a restricted room or is seen with a weapon. None in Chapter 7.
- **Personality / behaviour:** Predictable patrols; their talk at the stops is fixed lines (7 eavesdrops).

### Gameplay
Second by second, the player walks Sahra through the palace (walk by default, Shift to run), keeping out of guard view cones. When a highlighted character shows "E: Talk", the player presses E and a conversation opens over the 3D scene. The player types a line (up to 200 characters) and presses Enter, or presses 1 / 2 / 3 to pick a suggested reply. After a typing indicator of at least 0.4 s, the character answers in 1–4 sentences, and the portrait and gesture change within 1 second. A one-line hint (for example "Zafir looks at you a moment too long.") shows how the character's opinion changed. Each palace chapter is **one day and one night** (new 10.11). By day, the player reads the main quest, roams the open palace choosing whom to question, collects clues into the Rumor Journal (a short "New clue" toast appears), uses those clues as new topics with other characters, completes the chapter's two "do" objectives (for example: learn a patrol and listen behind a lattice screen, hide the stolen knife before a search, tail Zafir to the City Gate, shake off the king's servant), slips past patrols and eavesdrops on guards. At nightfall the player dances (30–45 s rhythm minigame) to keep Sahra's cover; a good dance pays out Favor and a reward clue. Then, with 3 clues, the player opens the chapter's key conversation and pushes it toward the hidden beats before the turns run out. The chapter ends in the engine (for example, Sahra walking back with the knife in her hand), then the chapter card moves the story on and the characters move to new places.

#### Core Loop
**Chapter loop (new 10.11):** DAY: read the chapter question → explore the open palace while avoiding patrols (hide, eavesdrop, question characters, do objectives) → clues go into the Rumor Journal → use clues as topics with others → NIGHT: dance (good dance = Favor + reward; bad dance = Suspicion) → KEY SCENE with Zafir or Vashir (needs 3 clues; not in Chapters 6 and 7) → in-engine ending → next chapter.

**Investigation loop (new 09.11):** read the chapter question → explore the open palace (walk, sneak, find people) → question a character (type or pick a reply) → the character reacts and may give a clue (Rumor Journal) → use the clue as a topic with someone else → repeat until enough clues → the key conversation opens. **Key scene loop (GDD Figure 1):** talk freely → the character reacts (mood, hidden values, memory) → is the scene goal met? If no, keep talking (the turn limit applies). If yes, a story beat plays and the chapter ends in the engine → the next chapter question appears → explore again. If Suspicion reaches 100 at any point, the cover is blown and the chapter restarts from its checkpoint (GDD Figure 1).

**Spirit loop (Chapter 7, new 11.11):** walk the empty, ghostly palace → find a soul ember → it replays a line the player really said in that chapter → one piece of Zafir's generated confession plays from the fire → find the next ember (any order, 6 in all) → the fire turns from red to gold → Epilogue.

#### Main Mechanics

##### 1. Free Conversation
The player types up to 200 characters or picks one of 3 suggested replies. The character answers in 1–4 sentences (max 80 words), with an emotion and a gesture. Each scene has a fixed turn limit (decided 12.11): **Zafir 11, Vashir 7, Nura, Idris and the minor characters 5 at most, ambient NPCs 1 line**, so it stays short and the built-in model stays fast. It is fun because pre-written dialogue trees can't respond to what the player actually types, so the romance feels personal.

##### 2. Relationship Values
Zafir and Vashir each have two hidden values from 0 to 100 (Zafir: Trust and Affection; Vashir: Favor and Suspicion). Every turn the LLM suggests a change and the game clamps it before applying it. The values unlock dialogue and secrets, change the character's tone, decide the romance or comrade route, and decide which epilogue lines play. The UI never shows numbers, only a mood icon and a one-line hint. The other characters have no values; they give clues. It is fun because characters react to what was actually said, not to picking "the nice option".

##### 3. Scene Goals and Beats
Each key scene has 1–3 hidden beats (key story moments such as `zafir_reveals_target` or `alliance_agreed`; the full list is in the Master Lore document, tab "Scene Cards"). The LLM marks a beat when it happens, and the Scene Director checks the rules (for example Trust 50) before accepting it. When all required beats are done, the scene goal is met. The Scene Director nudges the character 2 turns before the limit or after 3 turns with no progress, and force-closes the scene with the weaker closing line at the limit (the required beats are then set by the Director). It is fun because open conversations still have a clear purpose and never drag.

##### 4. Memory
After each scene, a summary call writes 1–2 short memories for each character (6 kept per character, oldest dropped). Memories come back in later scenes; in Chapter 7 each soul ember replays a line the player really said, and Zafir's confession quotes the player's own moments. In the Chapter 6 trap, whatever Sahra says to Vashir is saved as Zafir's memory. It is fun because the ending becomes personal (should have).

##### 5. Exploration and Suspicion
Sahra explores the palace in third person. Guards on set patrol routes notice if she runs, stays in restricted rooms or is seen with a weapon, and Suspicion rises; it falls slowly when no guard is watching. Vashir can also raise Suspicion when Sahra answers carelessly. **(Changed 10.11, must have):** guards walk fixed, timed routes the player can learn; hiding spots (curtains, lattice screens, alcoves) make Sahra invisible; the best clues sit near guards, so sneaking is how information is earned. **(New 11.11):** Chapter 4 adds following (tail Zafir to the City Gate at a distance) and Chapter 5 turns it around (the king's servant follows Sahra; she must break his line of sight, then corner him). It is fun because it adds readable tension between conversations.

##### 6. Clues and the Rumor Journal (new 09.11)
Each palace chapter has one question and **6 clues** (36 in all, written 12.11): talk clues held by specific characters, eavesdrop clues, a dance-reward clue and "do" objective clues. A clue is a short fact the team writes in advance (for example "Nura: Zafir never misses a dance, and he is never there after it."). When the conversation reveals it, the LLM marks it in the JSON (`clues`) the same way it marks beats, and the Scene Director accepts it only if that character holds that clue in this chapter, so the model cannot invent clues. Accepted clues go into the Rumor Journal (Tab) with the authored text. Clues unlock new topics: a clue becomes an extra suggested reply and is added to the next character's prompt so they react to it. **Any 3 of a chapter's 6 clues** open the key conversation (none needed in Chapters 6 and 7). In offline mode, the pre-written trees give the same clues. It is fun because the player is a detective whose tool is conversation, and every character is worth talking to.

##### 7. Nightly Dance (must have since 10.11)
Every night Sahra performs, because a dancer who skips the performance blows her cover. It is the GDD's timing minigame: 4 keys, 0.25 s hit windows; 30 s on night 1, then +3 s and +10% tempo each night up to 45 s on night 6. Above 80% accuracy: +10 Favor and a reward clue (overheard noble gossip); 40–80%: no change; below 40%: +10 Suspicion ("a real dancer wouldn't miss"). Each night is different: the court dances in Chapters 1, 2 and 4; the private dance for Vashir in the King's Chamber in Chapter 3; the public duet with Zafir in Chapter 5; the last solo dance for Vashir alone in Chapter 6 (fixed outcome). It is fun because it is a short skill break between conversations and it feeds the investigation.

##### 8. Hiding and Eavesdropping (new 10.11)
Hiding spots (curtains, lattice screens, alcoves) are marked by a soft gold glow; E enters and leaves them; guards cannot see into them. Eavesdrop zones sit next to hiding spots where guards or courtiers stop to talk: staying in the zone unseen for the whole exchange (about 10 s) adds an authored clue (7 eavesdrops in all). Being seen breaks the exchange. It is fun because it turns the guard patrols into an information source.

##### 9. Living Palace and Barks (new 11.11)
About 10 ambient NPCs fill the palace. E on one of them gives one line, and they turn away. Barks change each chapter and react to the story (the blade rumor, the search, the king's new favorite, festival eve). The real characters are not marked on a map: the player finds them where the chapter puts them. It is fun because the palace feels alive and reacts to what Sahra has done.

##### 10. The Spirit Chapter (new 11.11)
Chapter 7 is played as Sahra's spirit in the empty palace on festival night (desaturated, ash drifting upward, the Crimson Zenith's fire glowing over the walls). There are no guards and no fail state. Six soul embers wait in the six places of her story (the fountain, the practice room, the Throne Hall, the City Gate, the Rooftop Terrace, the King's Chamber); each replays one of her own lines. After each ember, one piece of Zafir's generated confession plays (1–3 sentences); a pre-written 6-piece confession plays if the AI fails. It is fun because the ending is built from the player's own words.

#### Win and Lose
- **Win:** Finish Chapter 7. The ending is always the same, but Zafir's confession and the epilogue lines change with the relationship values (romance or comrade route) and the memories the characters kept.
- **Lose:** Suspicion reaches 100, either from guards (running, going into restricted rooms), from Vashir (careless answers) or from a bad night dance (below 40%). Running out of turns is not a game over: the scene still ends, with the weaker closing line and lower values. Chapters 6 and 7 cannot be lost (fixed outcome; no guards).
- **What happens after losing:** The chapter restarts from its checkpoint (the Dancers' Quarters in the morning of that chapter).

#### Gameplay Walkthrough
1. **Title screen:** embers drift over the dying sun. The player picks New Game.
2. **Prologue: Ash (2D cutscene):** Tamra burns at the Festival of the Long Flame and a girl walks out of the smoke. Ten years later, Sahra arrives at Sama'zar as a dancer from the Lantern House, a month before the next festival. Space advances the text.
3. **Chapter card:** "Chapter 1 · The Garden of Dusk" with one line of verse, for 3 seconds.
4. **Dancers' Quarters (start and checkpoint):** Sahra starts here. The main quest at the top left reads "Why does the other dancer carry a blade?" Nura and Aisha are here; Nura's gossip gives a first clue ("Zafir never misses a dance, and he is never there after it."). A toast shows "New clue" and Tab opens the Rumor Journal.
5. **Open palace:** every room except the King's Chamber (restricted) and the City Gate (locked) is open. In the Garden of Dusk the fountain still runs under lanterns and a full moon. A guard patrols the garden; running in view raises Suspicion by 12 per second, and the eye icon fills while a guard sees Sahra. The player chooses whom to question next: Tariq off duty by the fountain, Omar setting out the wine in the Throne Hall, Captain Idris at the Guard Post, Ilyas on the Rooftop Terrace. Ambient NPCs give one line each. "Do 1": learn the garden guard's patrol and reach the lattice screen unseen to hear two guards talk about a blade under Zafir's silks. Zafir passes Sahra once in a corridor and says nothing.
6. **Nightfall:** a bell rings; the goal line changes to "Perform in the Throne Hall". Sahra dances for the court (30 s, 4 keys) while Vashir watches. A good dance adds Favor and a noble's gossip as a clue; a poor one adds Suspicion. After the performance, Zafir waits by the fountain.
7. **Meeting Zafir (key scene):** with 3 clues, the player confronts Zafir in the garden and presses E. Zafir opens with an authored line ("Following me after the dance, Little Ember? Most dancers just go to bed."). The player has 11 turns to confront him about the blade and reach an uneasy truce (beats `sahra_confronts_blade`, `uneasy_truce`). "Do 2": steal his knife during the talk: when he is distracted (he leans close, or after the blade beat), a prompt "[E] Take his knife" shows for a few seconds. Success puts the knife in the inventory and Zafir answers with the authored line "Careful, Little Ember. Blades that small cut their owners first." (decided 12.11.2026: Sahra has no blade before this, so the line belongs to the steal); his prompt then holds "Sahra just took your knife; you noticed and let her keep it." A try at a bad moment: he catches her wrist and Trust drops, then he lets go: "Sloppy, Little Ember. Keep it. You clearly need the practice." The knife still goes into the inventory: Sahra always ends Chapter 1 with it (decided 12.11.2026). *Open:* what happens if she never tries. The game decides the steal, never the LLM (beat `knife_taken`, set by the Scene Director, Phase 7). Hints like "Zafir looks at you a moment too long." show the mood. Clues the player found are in his prompt, so he reacts to what she knows.
8. **In-engine ending:** "You keep my secret, I keep yours." Sahra walks back to the Dancers' Quarters with his knife in her hand. The memory summary writes 1–2 memories per character.
9. **Next chapter:** the game saves at the checkpoint and the Chapter 2 card plays with a new question ("Why are the guards searching the dancers?"); the characters move to new places.

### Mindset
The player should feel playful tension in the conversations and a slowly growing tragedy in the story: the talks are teasing and flirtatious, but fire and ash are everywhere and the festival comes closer with every chapter (30, 22, 15, 8 and 3 nights, then the eve). Stealth sections (guards, the tail, being followed, Vashir's probing questions) should feel tense, because one careless word or step raises Suspicion. Zafir's warmth growing over the chapters should feel earned and personal, because it comes from the player's own words. The Trap (Chapter 6) should feel like betrayal and loss, played, not watched. The spirit walk (Chapter 7) should feel quiet and grieving, built from the player's own lines, and the epilogue should turn grief into triumph when Sahra rises as the phoenix and the sun returns. On a second playthrough the player should feel the irony of Zafir's dodged answers, now knowing what he was hiding.

### Uniqueness
- **Free conversation inside a fixed story:** the player types anything and characters remember it, unlike choice-list romance games. Compared with **Façade** (Procedural Arts, 2005), which also uses typed conversation inside a fixed story.
- **Dialogue as the main gameplay:** compared with **Disco Elysium** (ZA/UM, 2019), dialogue with hidden checks; here the hidden values are moved by an LLM and clamped by our code.
- **Relationships through many short conversations:** compared with **Hades** (Supergiant Games, 2020).
- **Investigation by conversation (new 09.11):** gather clues by talking to many characters in an open palace and use them as new topics. MISSING: the team's own comparison games for this part (detective and social-stealth games).
- **Open AI storytelling with hard limits:** compared with **AI Dungeon** (Latitude, 2019), our characters only know a ~600-word lore bible, brush off off-topic questions in character and cannot reveal the twist early, because secrets are only added to the prompt in the chapter where they can be revealed.
- **An ending built from the player's words (new 11.11):** the spirit chapter replays the player's own lines and Zafir's confession quotes them.
- **Memory system** based on Park et al. (2023), *Generative Agents*.
- **Always playable, no internet:** the AI model ships inside the game and runs on the player's PC (no account, no key, no internet); if it fails or is too slow, pre-written fallback lines and offline dialogue trees answer instead (decided 10.11 and 12.11.2026).

### Design Pillars
> MISSING: the GDD has no explicit design pillars. The four below are suggested from the GDD's Unique Selling Points and scope rules; confirm or replace them.

#### P1. Your words matter
Every system (values, beats, memories, the Chapter 7 embers, the confession, epilogue lines) must react to what the player actually said, never to "the nice option".

#### P2. Fixed story, personal romance
The main events and ending never change; only the romance (romance or comrade route), the reveals and the closing lines change.

#### P3. Characters stay in the world
Characters only know the lore bible and their own card, refuse off-topic questions in character and cannot leak secrets that are not yet in their prompt.

#### P4. Always playable
Every AI feature has a fallback (fallback lines, offline dialogue trees for every key scene and every side character, the pre-written confession), and the must-have features alone make a complete game.

### Out of Scope
- A true open world outside the palace: "open" means the whole palace is free to explore (one 3D level, about 60 × 30 m). The capital, Tamra and the Crimson Zenith are seen only as glow, 2D images or text.
- Clues invented by the LLM: every clue is authored; the model can only reveal clues from the character's chapter hints.
- Guards that chase or fight: guards only notice and raise Suspicion (simplified 10.11).
- A free day / night clock: night falls on a story trigger (enough clues or the player chooses "Go to perform"), not on a timer (*proposed*).
- 3D levels for the pyre and the festival temple: the Crimson Zenith is a glow over the walls in Chapter 7 and a 2D image in the Epilogue.
- Any 3D level other than the palace (one hub with five rooms); Chapter 7 reuses the palace with a ghost look.
- Voice acting (text only); text-to-speech for Zafir is only a nice-to-have and is cut in the 4-week plan.
- A hosted (cloud) LLM API as the main AI: replaced on 10.11.2026 by the built-in local model (free, no key); an external OpenAI-compatible endpoint stays an optional setting.
- A proxy server for an API key: not needed (no key).
- Gamepad support (the GDD targets keyboard and mouse only).
- Branching story or multiple endings: the ending is always the same (two routes only change lines).
- Visible relationship meters or numbers.
- Explicit romance content (Teen rating).
- AI-generated story, twist pacing, lore bible, key story lines, Scene Director logic, final portrait look and level design (made by the team, GDD 12.3). Game-data lines (cards, clues, barks, fallback lines, offline trees) are drafted with Claude from the team's story and reviewed and rewritten by the team (12.11).
- Combat system: none. The fight at the end of the trap is part of the fixed outcome, not a mechanic.

---

## 2. Technical

### Screens

#### A. Main Menu (Title)
- **Elements:** New Game, Continue, Settings, Quit buttons; background with embers drifting over the dying sun.
- **Goes to:** New Game → Prologue cutscene → Chapter 1 card; Continue → last chapter checkpoint; Settings → Settings; Quit → exit to desktop.

#### B. In-Game HUD
- **Elements:** Exploration HUD: goal line at the top left; an eye icon that fills only while a guard sees Sahra; an "E: Talk" prompt over characters. Conversation screen (GDD Figure 5): chapter title and goal at the top left, "PLAYING AS SAHRA" tag, "Turns left: N · Tab: log" at the top right; dialogue panel with the speaker's line next to their small portrait and name; italic hint line under the line instead of a meter; Sahra's portrait below as the point of view; large character portrait on the right that changes with the emotion tag, with emotion icons; 3 numbered suggested-reply buttons (1 / 2 / 3); text field "Reply as Sahra to [name] · 200 characters" with a Send (Enter) button. "New clue" toast when a clue is accepted. Chapter card: chapter number, title, one line of verse and the chapter question, for 3 seconds. Time of day tag ("Day 1 · Morning" / "Night") and a "Perform in the Throne Hall" goal at nightfall (new 10.11). Two "do" objectives under the main quest (new 11.11). Chapter 7: no HUD except an ember counter (new 11.11). Dance screen (new 10.11): note lane with 4 arrows, hit feedback, accuracy at the end with the reward or Suspicion result. Rumor Journal (Tab, new 09.11; extends the GDD's log screen): the chapter question, found clues (authored text, who said it), clues still missing as "?" slots, the full conversation log and each character's memories in their own words.

#### C. Pause Menu
- **Elements:** Resume, Settings, Restart Chapter, Quit.

#### D. Game Over / Lose Screen
- **Elements:** MISSING: a dedicated lose screen. The GDD says that when Suspicion reaches 100, guards seize Sahra ("Cover blown") and the chapter restarts from its checkpoint; the message and any buttons are not specified.

#### E. Win / Level Complete Screen
- **Elements:** Chapter complete: the closing line, then the next chapter card (no stats screen, the values stay hidden). Game complete: Chapter 7 ends when the fire turns gold, then the Epilogue cutscene ("The Long Flame") ending with "Careful, Little Ember.", then the Credits screen with the team, licensed assets and a list of AI-assisted assets and tools. MISSING: buttons on the Credits screen.

#### F. Settings
- **Elements:** Text speed, volume, AI mode (Online / Offline) and a status line: "AI: ready" (built-in model running), "AI: starting" or "AI: offline, pre-written dialogue". MISSING: video and control settings.

### Controls
> Enhanced Input (Input Actions `IA_` and Mapping Contexts `IMC_`). Keyboard and mouse only.

| Action | Keyboard / Mouse | Gamepad | Input Type | Notes |
| :-- | :-- | :-- | :-- | :-- |
| Move | W A S D | Not supported (keyboard and mouse only) | Value (Vector2) | Walk by default, 2.2 m/s (220 cm/s) |
| Look / Camera | Mouse | Not supported | Value (Vector2) | Orbits the third-person camera |
| Jump | None | Not supported | None | The GDD has no jump |
| Interact | E | Not supported | Button | Talk to or interact with a highlighted character or object |
| Run | Shift (hold) | Not supported | Button | 4.5 m/s (450 cm/s); raises Suspicion near guards |
| Say anything | Type + Enter | Not supported | Text input + Button | Max 200 characters; longer text is cut with a warning |
| Pick suggested reply | 1 / 2 / 3 | Not supported | Button | Picks one of the 3 suggested replies instead of typing |
| Rumor Journal | Tab | Not supported | Button | Opens the Rumor Journal: chapter question, clues, conversation log and memories |
| Advance cutscene | Space | Not supported | Button | Advances cutscene text |
| Pause | Esc | Not supported | Button | Opens the pause menu |
| Dance keys | ← ↑ ↓ → (*proposed*; MISSING in the midterm GDD) | Not supported | Button | Nightly dance minigame |
| Hide / leave hiding spot | E | Not supported | Button | Same key as Interact, near a hiding spot |

### Camera
- **View:** Third person (Spring Arm + Camera from the Third Person template).
- **Behaviour:** The mouse orbits the camera around Sahra. During conversations the 3D scene stays visible behind the conversation UI. Spring Arm 350 cm, pitch −60° to +25°, camera collision on (Phase 1, `technical_design.md` section 3). MISSING: height offset and special camera zones (the Chapter 6 fall keeps the camera low with Sahra).

### Mechanics

#### 1. Free Conversation
- **Rule:** The player types a line or picks one of 3 suggestions; the prompt builder sends the character card, lore bible, scene card, state and the last 8 turns; the reply must be JSON and must pass the validator before the player sees it.
- **Numbers:** Max input 200 characters; turn limit (decided 12.11) 11 (Zafir), 7 (Vashir), 5 at most (Nura, Idris, the 4 minor characters), 1 line (ambient NPCs); 3 suggested replies; reply 1–4 sentences, max 80 words; the reply is shown word by word as the model writes it, and the first words appear within 4 s (decided 13.11.2026); response timeout 6 s for the whole reply; typing indicator at least 0.4 s; portrait and gesture change within 1 s of the reply.
- **Edge cases:** Longer input is cut with a warning. Timeout or model error: "hesitates" animation, then a pre-written fallback line (pick order: scene + beat + emotion → the clue the character holds → scene → any scene; never the same line twice in a row; 142 lines); values unchanged. Broken JSON: 1 silent retry, then fallback. Off-topic input: in-character reply ("A dancer who talks of 'phones'? The heat has got to you."). Spoiler leak: regenerate once, then fallback. Explicit or hateful content: the character refuses in character; after 3 times the scene closes politely. Jailbreak: in-character confused reply. Built-in model missing or too slow: offline mode with pre-written dialogue trees for every key scene and every side character (`DT_OfflineTrees`, written 12.11; every path reaches the scene's required beats). (GDD Figure 2 says "5 s timeout"; the GDD text says 6 s, and 6 s is used here.)

#### 2. Relationship Values
- **Rule:** Each value is 0–100 and hidden. The LLM suggests a delta every turn; our C++ code clamps it to the per-turn maximum and applies it.
- **Numbers:** Zafir Trust start 20, ±8 per turn (30: one true thing, Chapter 2; 50: admits he hunts Vashir and the letters are false, Chapter 4; 60 on the romance route: his village burned). Zafir Affection start 10, ±8 (moves tone from teasing to tender; 40: jealousy shows, Chapter 3; 60: rooftop confession, Chapter 5; decides the romance or comrade route). Vashir Favor start 30, ±6 (60: invites Sahra to dance for him alone, Chapter 3; below 60 the invitation still comes later through Nura). Vashir Suspicion start 10, +10 / −5 (100: guards seize Sahra, chapter restarts).
- **Edge cases:** An impossible change (for example +50 Trust) is cut to the per-turn maximum. Values are clamped to 0–100 (MISSING: confirmation of the 0 and 100 bounds behaviour). On fallback lines, values do not change. Unknown emotion: portrait falls back to Neutral (Zafir) or Serene (Vashir).

#### 3. Scene Director
- **Rule:** States Intro (authored opener) → Free talk (player leads) → Nudge (character steers) → Resolve (closing line) or Forced close (weaker beat) → Next chapter (save, cutscene); Caught (retry chapter) when Suspicion reaches 100.
- **Numbers:** Nudge at 2 turns before the limit or after 3 turns with no progress; 1–3 hidden beats per scene (8 scene cards); forced close at the turn limit with the weaker closing line; the Chapter 6 beats are forced (turn 1 and by turn 3).
- **Edge cases:** Beat claimed without enough Trust: rejected by the Director. Scene goal never reached: nudge, then forced close, and the Director sets the required beats so the story goes on. Out of turns is not a game over. Offline: the tree places the required beats; optional beats check their rule and use a stand-in line when it fails.

#### 4. Memory
- **Rule:** After each scene, one LLM summary call writes 1–2 short memories per character, written in the character's voice.
- **Numbers:** 6 memories kept per character (oldest dropped); 1–2 written per scene; last 8 turns kept in full in the prompt.
- **Edge cases:** MISSING: what happens if the summary call fails or times out (no memory is written is the likely default; confirm).

#### 5. Exploration and Suspicion
- **Rule:** Guards walk fixed, timed routes; they see Sahra inside their view cone unless she is in a hiding spot; running, staying in a restricted room or carrying a visible weapon raises Suspicion; it falls when no guard sees her. Eavesdrop zones give an authored clue after the whole exchange is heard unseen (new 10.11).
- **Numbers:** Walk 2.2 m/s; run 4.5 m/s; guard view cone 70°, 9 m; running in view +12 per second; restricted room in view +20 per second; out of view −5 per second; checkpoint at the start of each chapter. Patrol loop 20–40 s with 3–5 s stops (*proposed*); eavesdrop exchange about 10 s (*proposed*).
- **Edge cases:** Suspicion reaches 100: chapter restarts from its checkpoint. The King's Chamber is restricted until Vashir's Favor reaches 60. MISSING: Suspicion value for "seen with a weapon".

#### 6. Clues and the Rumor Journal (new 09.11)
- **Rule:** Each chapter has a question and authored clues held by specific NPCs. The LLM flags a revealed clue id in `clues`; the Scene Director accepts it only if that NPC holds it in this chapter. Accepted clues are added to the Rumor Journal, become a topic (extra suggested reply) and are added to later prompts. The key conversation of the chapter is locked until the required clues are found.
- **Numbers (written 12.11):** 6 clues per palace chapter (36 in all: 24 talk, 7 eavesdrop, 3 dance reward, 2 "do"), held by 3–5 characters, 1–2 per character per chapter; any 3 open the key scene; side-character turn limit 5; "New clue" toast 2 s (*proposed*); Chapters 6 and 7 have no clue requirement.
- **Edge cases:** The model claims a clue the NPC doesn't hold: rejected, nothing added. A clue already found: ignored. The NPC's turns run out before the clue: the clue is offered in the forced-close line (proposed) so the chapter can never soft-lock. Offline mode: each side character's node contains their clue, and fallback lines can give it too. Caught (Suspicion 100): clues found in this chapter are kept (proposed, MISSING: confirm).

#### 7. Nightly Dance (must have since 10.11)
- **Rule:** Timing minigame with 4 keys, once per palace chapter at night: the court in the Throne Hall (Chapters 1, 2, 4), the private dance in the King's Chamber (Chapter 3), the duet with Zafir (Chapter 5), the solo dance for Vashir (Chapter 6, fixed outcome); new song, faster tempo and new audience each night.
- **Numbers:** Hit window 0.25 s; accuracy above 80%: +10 Favor; below 40%: +10 Suspicion.
- **Numbers (added 10.11, proposed; lengths 12.11):** 30, 33, 36, 39, 42 and 45 s on nights 1–6; tempo +10% per night; 40–80% accuracy: no change; > 80%: also a reward (overheard clue or access). Dance keys ← ↑ ↓ →.
- **Edge cases:** Below 40% adds +10 Suspicion and can trigger Caught at 100 (restart the chapter at its checkpoint). Missing the reward clue never blocks progress: every required clue can also be found by day. Pause during the dance stops the music and notes.

---

## 3. Level Design

### Level Design Philosophy
The game has one 3D level: Sama'zar, the palace, built as a hub (the **Garden of Dusk**, with a fountain that still runs because the palace pretends to be unaffected by the dying sun) and five rooms around it, about 60 × 30 m: the Dancers' Quarters (with the practice room), the Throne Hall, the King's Chamber (restricted; a servants' passage behind a curtain), the Guard Post (with the locked City Gate) and the Rooftop Terrace (stairs from the Dancers' Quarters). **(Changed 09.11:)** the palace is open from Chapter 1: every room except the restricted King's Chamber and the locked City Gate can be explored at any time, and the characters move to new rooms each chapter. The main quest and the characters who are present guide the player, instead of locked doors. The player always passes through the garden, so its guard patrol is the main stealth challenge in every chapter. **(New 11.11):** the palace changes with the month: the moon (full, waning half, dark, waxing half, almost full), the festival banners (from Chapter 4), plum blossoms (Chapter 5), more or fewer guards, and in Chapter 7 a ghost version of the same level. Lanterns and firelight pick out details against plum and burgundy shadows; restricted rooms (the King's Chamber, Rose red) raise Suspicion when Sahra is seen in them. The on-screen goal line states the main quest. Full placement per chapter is in `level-design.md`. MISSING: specific guidance techniques (light, color, sound or shape cues) for leading the player to characters and doors.

### Themes

#### Theme 1: Prologue: Ash (2D cutscene)
- **Mood:** Burning village at night; fire, smoke and ash. Then the palace gate at the full moon.
- **Objects:** 2D backgrounds "Burning Tamra" and "The palace gate".
- **Challenges:** None (cutscene).
- **Goal:** Watch Tamra burn ten years ago and Sahra arrive at Sama'zar as a dancer from the Lantern House, 30 nights before the festival.

#### Theme 2: Chapter 1: The Garden of Dusk
- **Mood:** Full moon over the Garden of Dusk; lanterns, a running fountain; night plum and old gold.
- **Objects:** Whole palace open except the King's Chamber; Dancers' Quarters (start, checkpoint); a lattice screen in the garden; Zafir's knife.
- **Challenges:** 1 guard patrols the garden; 6 clues (find 3) from Nura, Omar, Tariq, an eavesdrop and the dance; Do 1: learn the garden patrol and listen behind the lattice screen; Do 2: steal Zafir's knife in the key scene; night dance 30 s; Zafir, 11 turns.
- **Goal:** Question: "Why does the other dancer carry a blade?" Confront Zafir and end in an uneasy truce ("You keep my secret, I keep yours.").

#### Theme 3: Chapter 2: The Practice Room
- **Mood:** Waning half moon; nervous palace, the Dancers' Quarters turned over by a search.
- **Objects:** 3 hiding places for the knife in the Dancers' Quarters; the practice room; an extra guard outside the Dancers' Quarters until the search ends.
- **Challenges:** Do 1: hide the knife before the search (Zafir lifts it minutes before the guards check the spot, so they find it empty); Do 2: listen at the guards' stop by the Guard Post for the search hour; 6 clues; night dance 33 s; the first rehearsal with Zafir, 11 turns.
- **Goal:** Question: "Why are the guards searching the dancers?" Ask Zafir why he helped; he gives the knife back.

#### Theme 4: Chapter 3: The Private Dance
- **Mood:** Dark moon; fewer lanterns in the garden; Vashir's court in burgundy and gold; the rose-red King's Chamber.
- **Objects:** The King's Chamber door (restricted, one guard); the Throne Hall audience; the private dance in the King's Chamber; the fountain.
- **Challenges:** Do 1: scout the King's Chamber door (Zafir draws the guard's eyes away); Do 2: find Ilyas on the rooftop before the dusk bell; 6 clues; Vashir's audience (7 turns; Favor 60 for the invitation); the private dance 36 s; Zafir at the fountain, 11 turns.
- **Goal:** Question: "How does a dancer get a private audience with the king?" Win Vashir's invitation without raising his Suspicion; Zafir's jealousy shows.

#### Theme 5: Chapter 4: The King's Favor
- **Mood:** Waxing half moon; the first festival banners; envy around the king's new favorite.
- **Objects:** The jeweled anklet (replaces the cord on Sahra's model); Omar between the Throne Hall and the City Gate; an extra guard at the City Gate at night.
- **Challenges:** Do 1: tail Zafir at the midnight bell to the City Gate without being seen; Do 2: find Omar and get him to talk about the letters; 6 clues; night dance 39 s; Zafir with her knife at his throat, 11 turns.
- **Goal:** Question: "Who is Zafir writing to?" He admits he hunts Vashir too; they agree to strike before the festival.

#### Theme 6: Chapter 5: Rooftop of Smoke
- **Mood:** The moon almost full; plum blossoms; smoke and moonlight on the rooftop; romance.
- **Objects:** The king's servant follows Sahra; doubled guards around the Dancers' Quarters; the Rooftop Terrace with a view toward the glow of the Crimson Zenith.
- **Challenges:** Do 1: shake off the king's servant, then corner him; Do 2: overhear two nobles in the Throne Hall; 6 clues; the public duet with Zafir 42 s; Zafir on the rooftop, 11 turns.
- **Goal:** Question: "Who is watching me?" Zafir returns her mended cord; the plan is set; if Affection is 60, they confess their feelings.

#### Theme 7: Chapter 6: The Trap
- **Mood:** Festival eve: banners everywhere, loud and busy, then strangely empty around the King's Chamber.
- **Objects:** The servants' passage (unlocked, swept clean); the King's Chamber; Zafir's knife and the mended cord.
- **Challenges:** Do 1: check the plan (passage, City Gate, guard change), each check a little too easy; Do 2: the last solo dance, 45 s, fixed outcome; Vashir, 7 turns, fixed outcome.
- **Goal:** Question: "Is everything ready?" Vashir names her as the girl from Tamra and reveals he hired Zafir; she is cut down and dies in Zafir's arms. Played, not watched.

#### Theme 8: Chapter 7: The Pyre (spirit walk)
- **Mood:** The same palace, empty and ghostly on festival night: desaturated, blurred, ash drifting upward, the Crimson Zenith's fire glowing red over the walls.
- **Objects:** 6 soul embers at the fountain, the practice room, the Throne Hall, the City Gate, the Rooftop Terrace and the King's Chamber.
- **Challenges:** None: no guards, no fail state, any order.
- **Goal:** Gather the six embers of her soul while Zafir's confession (6 generated pieces) reaches her; with the last ember the fire turns from red to gold.

#### Theme 9: Epilogue: The Long Flame (2D cutscene)
- **Mood:** The sacred fire at the Crimson Zenith before the whole court; the phoenix; triumph.
- **Objects:** 2D background "The Crimson Zenith"; Zafir's stolen knife.
- **Challenges:** None (cutscene).
- **Goal:** Sahra rises from the sacred fire unburned as the phoenix; Vashir burns in the fire he fed for fifteen years and the sun returns. Final shot: "Careful, Little Ember." Lines warmer on the romance route.

### Game Flow
1. Title → Prologue: Ash (2D cutscene, start).
2. Chapter 1: The Garden of Dusk (tutorial for walking, talking, gathering clues, hiding and the garden patrol; whole palace open except the King's Chamber; 1 guard in the garden).
3. Chapter 2: The Practice Room (hide the knife before the search; extra guard; first rehearsal).
4. Chapter 3: The Private Dance (Vashir's Favor and Suspicion enter; the King's Chamber opens by invitation; Zafir's jealousy).
5. Chapter 4: The King's Favor (following stealth; the alliance).
6. Chapter 5: Rooftop of Smoke (being followed; the cord returned; the confession on the romance route).
7. Chapter 6: The Trap (no clue gate; fixed outcome; Sahra dies).
8. Chapter 7: The Pyre (spirit walk; generated confession).
9. Epilogue: The Long Flame (2D cutscene; lines depend on the route and memories) → Credits.

Difficulty grows through more guards (1 in the garden → extra guards in Chapters 2, 4 and 5), new stealth (following in Chapter 4, being followed in Chapter 5), clues that need other clues first (Nura in Chapter 1, Aisha in Chapter 3), Vashir's Suspicion from Chapter 3 and longer, faster dances (30 → 45 s). Chapters 6 and 7 drop the challenge for the story.

---

## 4. Development

### Abstract Classes
> Unreal Engine 5.8: C++ base classes for the testable core, Blueprints for glue, UI and animation. Class names come from GDD Figure 6, translated to Unreal (full spec in `technical_design.md`).

| Class | Parent | Purpose |
| :-- | :-- | :-- |
| `ULLMProvider` | UObject (abstract, C++) | The only way game code reaches the LLM; implemented by the built-in-model provider and the offline provider |
| `ULocalModelSubsystem` | UGameInstanceSubsystem (C++) | Starts and stops the bundled llama-server (hidden, free port, closes with the game) |
| `AAshesGameMode` | AGameModeBase (C++) | Rules: chapters, checkpoints, Caught → restart chapter |
| `UAshesGameInstance` | UGameInstance (C++) | Current chapter, AI mode, relationship values and memories across level loads |
| `AAshesCharacter` | ACharacter (C++) | Sahra's movement (walk / run), hide state, Spring Arm camera |
| `AAshesPlayerController` | APlayerController (C++) | Enhanced Input mapping contexts, UI focus, pause |
| `UInteractionComponent` | UActorComponent (C++) | "E: Talk" starts a scene; E enters a hiding spot |
| `USceneDirectorSubsystem` | UWorldSubsystem (C++ state machine) | Intro, talk, nudge, resolve, forced close; accepts beats and clues |
| `UClueJournalSubsystem` (new 09.11) | UGameInstanceSubsystem (C++) | Found clues per chapter, topic unlocks, key-scene gate |
| `UDayNightSubsystem` (new 10.11) | UWorldSubsystem (C++) | Day → Night → Key scene per chapter; nightfall, lighting, NPC spawns, the palace's look per chapter (moon, banners, Chapter 7 ghost look) |
| `USpiritWalkSubsystem` (new 12.11, *proposed name*) | UWorldSubsystem (C++) | Chapter 7: soul embers, memory replay, one confession piece per ember |
| `UDanceMinigameComponent` (new 10.11) | UActorComponent (C++) | Nightly dance timing (Quartz clock) and grading |
| `UNPCBrainComponent` | UActorComponent (C++, one per character) | Character card, relationship values, memories |
| `UPromptBuilder` | UBlueprintFunctionLibrary (C++) | Builds the prompt from card, lore, scene, state and turns |
| `UResponseValidator` | UBlueprintFunctionLibrary (C++) | JSON parse, lore, spoiler, safety checks and clamping |
| `USuspicionComponent` | UActorComponent (C++) | One Suspicion value fed by guards, Vashir and the dance; Caught at 100 |
| `AGuardAIController` | AAIController (C++) | Patrol and notice through AI Perception (sight) and a Behavior Tree |
| `AHidingSpot`, `AEavesdropZone` | AActor (C++) | Spots hidden from guards; timed overheard exchanges that give clues |
| UI widgets | UUserWidget (UMG) | Every screen; conversation, HUD, Rumor Journal, dance, portraits by emotion |

### Derived Classes
> Blueprints and implementations made from the base classes.

| Blueprint | Derives from | What it adds |
| :-- | :-- | :-- |
| `UOpenAICompatibleLLMProvider` | `ULLMProvider` | Talks to the built-in llama-server (or an optional external endpoint) over `FHttpModule`; streamed JSON; 6 s timeout in the game, 30 s in the editor |
| `UOfflineLLMProvider` | `ULLMProvider` | Offline dialogue trees and fallback lines (DataTables), with the fallback pick order |
| `GM_Ashes`, `GI_Ashes`, `PC_Ashes` | `AAshesGameMode`, `UAshesGameInstance`, `AAshesPlayerController` | Default classes, widget classes and input contexts set in the editor |
| `BP_Sahra` (+ `ABP_Sahra`) | `AAshesCharacter` | Mesh, animation Blueprint (walk, run, hide, dance moves), walk 220 cm/s, run 450 cm/s, Spring Arm camera |
| `BP_Zafir`, `BP_Vashir`, `BP_Nura`, `BP_Idris`, `BP_Omar`, `BP_Ilyas`, `BP_Tariq`, `BP_Aisha` | `AAshesNPC` + `UNPCBrainComponent` | Character card row, emotion set, values and memories per character |
| `BP_KingsServant` (new 12.11) | `AAshesNPC` | Fixed lines, no LLM; follows Sahra in Chapter 5 |
| `BP_AmbientNPC` (new 11.11) | `AAshesNPC` | One bark per chapter from `DT_Barks`; no conversation |
| `BP_SoulEmber` (new 11.11) | AActor | Chapter 7 pickup; replays one memory |
| `BP_Guard` (+ `BT_Guard`, `BB_Guard`) | ACharacter, controlled by `AGuardAIController` | Patrol route, 70° / 900 cm sight, NavMesh movement |
| `BP_HidingSpot`, `BP_EavesdropZone` | `AHidingSpot`, `AEavesdropZone` | Curtain or lattice mesh; overheard lines and clue id |
| `WBP_Conversation`, `WBP_HUD`, `WBP_RumorJournal`, `WBP_Dance`, `WBP_Title`, `WBP_Settings`, `WBP_Pause`, `WBP_ChapterCard`, `WBP_Cutscene`, `WBP_Credits` | UUserWidget | Screens from section 2 in the `design.md` style |

### Data
> DataTables (`DT_`, rows are C++ structs deriving from `FTableRowBase`, editable as CSV) and Data Assets (`DA_`) in `Content/Ashes/Data/`.

| Data Table | Rows (examples) | Fields |
| :-- | :-- | :-- |
| Character cards (`DT_Characters`) | Zafir, Vashir, Nura, Idris, Omar, Ilyas, Tariq, Aisha (8 LLM characters; the king's servant as a fixed-line row) | DisplayName, Voice, Wants, Fears, SpeechRules, Knows, MustNotSay, Secrets per chapter, Emotions, FallbackEmotion, TurnLimit, Values (start, max up, max down), SpoilerKeywords, bUsesLLM |
| Scene cards (`DT_Scenes`) | 8 rows: Ch1_ZafirKeyScene, Ch2_ZafirRehearsal, Ch3_VashirAudience, Ch3_ZafirFountain, Ch4_ZafirKnife, Ch5_ZafirRooftop, Ch6_VashirTrap, Ch7_ZafirConfession; plus a short general scene per chapter for the side characters (*proposed*) | Chapter, place, time, goal text, beats and their rules, turn limit, characters, opener, closing lines (full, weaker; a romance closing is proposed as a new field) |
| Lore bible (`DA_LoreBible`) | One ~600-word text | Kingdom and dying sun, cult of the Sun Goddess, palace rooms and routines, festival, dancers, guard |
| Fallback lines (`DT_FallbackLines`) | 142 lines (Zafir 53, Vashir 22, side characters 67) | Character, scene, beat, emotion, clue, line, AI-assisted flag |
| Offline dialogue trees (`DT_OfflineTrees`) | 7 key-scene trees + 34 side-character nodes (24 clue, 10 small talk) + 12 topic and repeat-visit lines | Scene, node, 3 replies, character line, emotion, value changes, beats, clues, next node, stand-in line when a rule fails |
| Barks (`DT_Barks`, new 11.11) | 60 barks (10 ambient NPCs × Chapters 1–6) + 7 eavesdrops + 3 dance-reward gossips | NPC, chapter, line (eavesdrops: speakers, lines, clue id) |
| Cutscene script (`DT_Cutscenes`) | Prologue (8 lines), Epilogue (romance and comrade variants), 6 ember captions, the 6-piece fallback confession | Part, speaker, line, background, route |
| Clue table (`DT_Clues`, new 09.11) | 36 clues, 6 per palace chapter, e.g. `ch1_nura_disappears` | Id, chapter, holder, source (talk, eavesdrop, dance, do), journal text, topic text for the suggested reply, prompt hint |
| Dance charts (`DT_DanceCharts`, new 10.11) | One per night (6) | Song, BPM, tempo multiplier, length (30–45 s), notes (beat, lane), reward clue |
| Exploration settings (`DA_ExplorationSettings`) | Walk 220 cm/s, run 450 cm/s, view cone 70° / 900 cm, Suspicion rates | Speeds, cone angle and range, Suspicion per second per cause |

### AI
**Guards (must have since 10.11, simplified):** `BP_Guard` with a Behavior Tree on a NavMesh, following fixed, timed waypoint routes: Patrol → Notice (stop and look while Suspicion rises) → Patrol; no chasing; hiding spots block their sight; eavesdrop stops where two guards talk. They see Sahra through AI Perception sight: a 70°, 9 m (900 cm) view cone. They raise Suspicion when she runs in view (+12/s), is in a restricted room in view (+20/s) or is seen with a weapon. The player escapes by leaving their view: Suspicion falls by 5 per second out of view. At 100 the chapter restarts. No guards in Chapter 7. MISSING: hearing, Notice/Investigate timings and what Alert does before the restart.

**Conversation AI:** the LLM writes what characters say; our C++ decides what changes. See Appendix 8.1.

**Ambient NPCs and the king's servant (new 11.11–12.11):** no LLM. Ambient NPCs stand or walk short loops and give one bark; the king's servant follows Sahra in Chapter 5 on a NavMesh path and loses her when she breaks line of sight for a few seconds (*proposed*).

---

## 5. Graphics

### Style Attributes
- **Art style:** Stylized 3D palace (a licensed modular desert or palace kit, lit with Lumen) and licensed character base models; conversations use painted character portraits inside ornate gold frames; the prologue (burning Tamra, the palace gate) and the epilogue (the Crimson Zenith) are painted 2D backgrounds; Chapter 7 reuses the 3D palace with a ghost look. "A moonlit palace at the end of the world."
- **Color palette:** Night plum `#2A1426` (backgrounds, night sky, UI panels); Burgundy `#6B1E2E` (Vashir's court, curtains, headings); Rose red `#9E1B32` (roses, danger, the King's Chamber); Old gold `#C9A25B` (UI frames, jewelry, Zafir); Ember orange `#E8642C` (fire, the phoenix, Sahra); Moon cream `#F1E3C8` (text on dark UI, moonlight).
- **References:** Mood board from the team's own photos and free or licensed images (no named artist's style copied): desert palaces at night (sandstone arches, lattice screens, a huge pale moon); lanterns and firelight on dark cloth; smoke and red rose petals in the air; dancers' silks and veils in motion; gold jewelry catching the light; burnt villages and ash fields for Tamra; a phoenix of fire for the ending.
- **Lighting:** Night: deep plum and burgundy shadows, moonlight, with gold and firelight (lanterns, fire) picking out the details.

### Graphics Needed

#### Characters
| Asset | Description | Animations |
| :-- | :-- | :-- |
| Sahra | Licensed base model, recolored; dancer disguise (veil, anklets) | Walk, run, idle, talk gestures, hide (enter / leave), 4 dance moves + idle dance loop for the nightly dance |
| Minor characters Omar, Ilyas, Tariq, Aisha (new 09.11, named 12.11) | Same base models, recolored (MISSING: looks) | Idle, talk gestures (shared set) |
| The king's servant (new 12.11) | Same base model, recolored | Walk (follow), idle, cower |
| Ambient NPCs (~10, new 11.11) | Same base models, recolored | Idle, short walk loops |
| Zafir | Licensed base model, recolored; long dark wavy hair, gold circlet | Idle, walk, 6 talk gestures (shared set), "hesitates", the duet (Chapter 5), catching Sahra (Chapter 6) |
| Vashir | Licensed base model, recolored; silver beard, burgundy and gold robes, sun disk | Idle, talk gestures, "hesitates", pinching out a candle |
| Nura | Licensed base model, recolored | Idle, talk gestures |
| Captain Idris | Licensed base model, recolored | Idle, talk gestures |
| Palace guards (2–4) | Same base model, recolored | Walk (patrol), idle, alert (MISSING: exact list) |
| Portraits | Zafir (6), Vashir (5), Nura (5), Idris (4), Omar, Ilyas, Tariq, Aisha (4 each) plus Sahra POV portrait (~37); AI-assisted drafts repainted by Tuana | Swap per emotion tag within 1 s |

#### Environment and Props
| Asset | Description | Where used |
| :-- | :-- | :-- |
| Modular palace kit | Licensed desert or palace kit (sandstone arches, lattice screens) | Whole palace, about 60 × 30 m |
| Running fountain and lanterns | Garden centrepiece and light sources (the fountain still runs) | Garden of Dusk (hub) |
| Lattice screens, curtains, alcoves | Hiding spots with a soft gold glow | Every room |
| Servants' passage | A narrow passage behind a curtain | King's Chamber (Chapters 3 and 6) |
| Festival banners, plum blossoms | Change with the month | Chapters 4–6, Chapter 5 |
| Curtains, throne | Burgundy court dressing | Throne Hall, King's Chamber |
| Stairs and terrace | Upper level reached by stairs | Rooftop Terrace |
| City Gate (locked) | Gate prop | Guard Post (the tail in Chapter 4; the plan check in Chapter 6) |
| Zafir's knife | Asset store or modeled by us | Stolen in Chapter 1, final shot |
| Dancer's veil and anklets | Asset store (disguise) | Sahra |
| Tamra cord and jeweled anklet | Asset store or modeled by us | Sahra's ankle (swapped in Chapters 4 and 5) |
| Sealed letters with the royal wax | Small prop | Zafir and Omar (Chapter 4) |
| 2D backgrounds | Burning Tamra, the palace gate, the Crimson Zenith; AI-assisted concepts finished by us | Prologue, Epilogue |

#### Visual Effects (VFX)
| Effect | When it plays |
| :-- | :-- |
| Fire | Lanterns, braziers and burning scenes |
| Embers | Title screen (drifting over the dying sun), around fire |
| Ghost palace | Chapter 7: desaturated post process, blur, ash drifting upward, red glow over the walls |
| Soul embers | Chapter 7: 6 pickups that replay memories |
| Fire turning gold | End of Chapter 7 |
| Phoenix rise | Epilogue: Sahra walks out of the sacred fire (the most important effect) |
| Smoke and rose petals | MISSING: whether these are in-game VFX (they are in the mood board) |

#### UI Art
| Element | Description |
| :-- | :-- |
| Ornate frames | Gold line work on plum, around portraits and panels (made by us in an image editor) |
| Mood / emotion icons | Small icons under the large portrait |
| Eye icon | Fills while a guard sees Sahra |
| Reply buttons | Numbered 1 / 2 / 3 framed buttons |
| Chapter card | Chapter number, title, one line of verse and the chapter question |
| Rumor Journal (new 09.11) | Parchment-style page in the ornate gold frame: question, clue entries with speaker portraits, "?" slots |
| "New clue" toast | Small gold-framed note at the top right |
| Ember counter (new 11.11) | Chapter 7 only: 6 small embers that light up |

---

## 6. Sounds & Music

### Style Attributes
Realistic and atmospheric: stone, fire, cloth and blades, with a Middle-Eastern palace music bed (oud, frame drum, drone), solo strings for romance and choir with drums for the finale. Calm and tense at night in the palace, tender in the romance scenes, epic in the finale. No voice in the must-have scope (text only).

### Sounds Needed
| Sound | When it plays |
| :-- | :-- |
| Footsteps on stone | Sahra and guards walking and running |
| Fire | Lanterns, braziers, the red-to-gold glow of the Crimson Zenith (Chapter 7), the sacred fire (Epilogue) |
| Cloth | Dancers' silks and veils, movement |
| Blades | Zafir's knife, the Trap (Chapter 6) |
| Crowd | Court in the Throne Hall, festival eve; the distant festival in Chapter 7 |
| Ember pickup and confession bed (new 11.11) | Chapter 7 |
| UI sounds | MISSING: UI sound list (hover, confirm, back, typing) |
| New clue | A clue is added to the Rumor Journal (MISSING: sound choice) |

All SFX come from free libraries with CC0 or CC-BY licenses; credits are kept in `Docs/Licenses.md`.

### Music Needed
| Track | Where / when | Mood |
| :-- | :-- | :-- |
| Palace loop (oud, frame drum, drone) | Exploration in the palace | Tense, nocturnal |
| Romance loop (solo strings) | Zafir conversations, rooftop | Tender |
| Tension (new 12.11) | Chapter 6, festival eve and the trap | Tense |
| Finale (choir, drums) | Chapter 7 and epilogue | Epic, triumphant |
| Dance songs | 6 nights (or one song at rising tempo, cut order 3) | Rhythmic, faster each night |

Must have: 1 music loop (the palace loop); should have: all 3. Royalty-free library first; AI music only if the tool's terms allow our use.

---

## 7. Schedule
> Updated 09.11.2026: the project has **4 weeks in total (09.11.2026 – 06.12.2026)**; the teacher's official deadline is 6 weeks (to 20.12.2026), kept as buffer. Updated 12.11.2026 for the team of four and the 7-chapter story. The detailed phase plan is in `production_pipeline.md`; the day-by-day plan is in `Docs/workplan/index.html`.

| Milestone | What is done | Date |
| :-- | :-- | :-- |
| Prototype | Movement, camera, grey-box test scene, interaction prompt; built-in AI model, prompt builder, validator and conversation screen; Chapter 1 with Zafir playable online and offline. Merve: code (done) and the story lines. Tuana: conversation screen style, Zafir's portraits. Tala and Baha: PC setup; Baha: table import and fallback pick order; Tala: UI sounds | Sun 15.11.2026 |
| First playable | Scene Director FSM, relationship values and offline trees (Baha); open palace grey-box with all rooms, Chapter 1 placement, guard routes and lighting (Merve); clues and Rumor Journal, guards with hiding spots and eavesdropping, nightly dance (Baha, with Tala's song, chart and animations and Tuana's UI art); Chapter 1 complete online and offline; reply time measured; first red-team run | Sun 22.11.2026 |
| Content complete | Chapters 2–6 (levels, clues, key scenes, nights), the Chapter 7 spirit walk, cutscene system, Prologue and Epilogue, HUD and all menus, dance for all 6 nights, memories if kept; first playtest with outside players (Tala) | Sun 29.11.2026 |
| Feature freeze and polish | Art pass (Tuana), lighting (Merve), audio and VFX pass (Tala), tuning, second playtest, red-team pass with 0 leaks | Thu 03.12.2026 |
| Final | Windows build with the built-in model, offline mode verified (Baha); credits, licenses and AI disclosure (Tala); final prompt log (Merve) | Sat 05.12.2026 (06.12 buffer) |

---

## 8. Appendix: GDD Content Not Covered by the Template

### 8.1 In-Game AI Design
The LLM writes what the characters say, but our own C++ code decides what actually changes. The model only makes suggestions; a state machine and a validator check them.

| Part | Technique | Runs in |
| :-- | :-- | :-- |
| Zafir, Vashir, Nura, Idris and the 4 minor characters talk freely | LLM character with structured (JSON) output | Built-in local model (llama.cpp + Qwen3.5-4B) on the player's PC |
| Scene flow (nudge, end, fail) | Finite state machine (Scene Director) | Our C++ |
| Faces and gestures | Emotion state machine driven by the LLM's emotion tag | Our C++ + Animation Blueprint |
| Memories between scenes | LLM summary call after each scene | Built-in local model |
| Clues revealed in conversation | The LLM marks clue ids; the Scene Director accepts only authored clues | Built-in local model + our C++ |
| Zafir's confession (Chapter 7) | 6 generated pieces of 1–3 sentences, one after each ember, quoting memories | Built-in local model; pre-written 6-piece fallback |
| Palace guards (must have since 10.11) | Simplified Behavior Tree + NavMesh: Patrol → Notice → Patrol | Our C++ / Blueprint + Unreal AI Perception and Navigation |

**Prompt order:** (1) character card, (2) lore bible (~600 words; nothing outside it exists), (3) scene card, (4) state (values as words, for example "Trust: wary", and up to 6 memories), (5) conversation (last 8 turns and the player's new line).

**Response JSON:**
```json
{
  "reply": "Careful, Little Ember. Blades that small cut their owners first.",
  "emotion": "teasing",
  "gesture": "lean_in",
  "trust_delta": 3,
  "affection_delta": 5,
  "beats": ["zafir_reveals_target"],
  "suggestions": ["Then why are you smiling?", "Stay out of my way.", "Who sent you?"],
  "off_topic": false
}
```

**Preventing spoilers:** each secret is added to a character's prompt only from the chapter where it can be revealed (`FChapterSecret`); for example, Zafir's contract with Vashir is in his prompt only in Chapter 7, and Vashir knows Sahra is the girl from Tamra only from Chapter 6. Spoiler keywords per character are blocked by the validator until their chapter (for example "contract" until 7, "kill Vashir" until 4, "you are the phoenix" never).

**Failure and fallback:**

| Failure | Detection | What the player sees |
| :-- | :-- | :-- |
| Slow reply or model error (online) | Soft limit 6 s, hard limit 20 s in the game (60 s in the editor while developing), error code | After 6 s Zafir visibly hesitates (an evasive line or gesture) while the game keeps waiting; the real answer is shown if it comes before the hard limit. Otherwise, or after two broken / spoiling replies: an evasive line that fits any question (`Hesitate` type, per character, e.g. "Ask me something easier, Little Ember." or a [gesture]); the turn does not count, values unchanged (changed 12.11.2026: unrelated pre-written answers felt broken online) |
| Model not running (offline mode) | `ULocalModelSubsystem` state | A pre-written `Content` line for the scene (beat + emotion; never twice in a row); values unchanged |
| Broken JSON | Parser fails | 1 silent retry, then the fallback line |
| Off-topic input | `off_topic` flag + keyword list | In-character reply |
| Spoiler leak | Secret not in prompt + keyword check | Regenerate once, then fallback |
| Explicit or hateful content | Safety word list; model told to refuse in character | Refuses in character; after 3 times the scene closes politely with a safety line |
| Jailbreak | Pattern list; model told to stay in role | In-character confused reply |
| Impossible value change | Limit in C++ | Cut to the per-turn maximum |
| Built-in model missing, failed to start, still loading or too slow on this PC | `ULocalModelSubsystem` state (`/health`) and timeouts | Offline mode: offline dialogue trees for every key scene and side character, plus fallback lines; Settings shows "AI: offline, pre-written dialogue" |
| The model claims a clue the character doesn't hold | Scene Director checks `DT_Clues` | Rejected; nothing added to the Rumor Journal |
| Scene goal never reached | Turn counter | Nudge, then forced close |

**What makes it believable:** distinct voices; characters have their own goals (ask back, refuse, bargain); they remember; portrait and gesture react within 1 s; hints instead of meters; replay value from Zafir's dodged answers.

### 8.2 AI Tool Plan (workflow)
| Task area | Tool | Use | Check | Cost |
| :-- | :-- | :-- | :-- | :-- |
| Coding (C++ and Blueprints) | Claude Code through MCP + VibeUE; GitHub Copilot in the IDE; Claude for design questions and errors | Boilerplate, HTTP and JSON code, Blueprint setup in the editor | Another team member reviews every merge (Baha for code); Automation tests for validator, parser and limits; AI-assisted header comment | Copilot via GitHub Student Developer Pack; Claude free tier |
| Art and illustration | An image model | Portrait drafts, background concepts, GDD diagrams | Tuana checks palette and style, repaints; no artist names in prompts | Free tier or course credits |
| Audio and music | Royalty-free libraries first; AI music only if terms fit | Music loops, 6 dance songs, ambience | Tala listens in the scene, checks loop seams, license in `Docs/Licenses.md` | Free |
| Writing and dialogue | Claude | Brainstorming; drafts of character cards, scene cards, clues, barks, fallback lines and offline trees from the team's own story (12.11); GDD draft | Merve and Tuana edit every line against the lore bible and the voices; the story, lore and key lines are the team's | Free tier |
| Testing | Own test harness (`Ashes.TestLLM`) + LLM judge | Red-team: 50 lines per character (off-topic, jailbreak, spoiler, rude) | Read every flagged reply + 20 random; target 0 spoiler leaks, 95% valid JSON | Free (built-in model) |
| In-game runtime | Built-in model shipped with the game: llama.cpp `llama-server` (MIT) + Qwen3.5-4B Q4_K_M (Apache 2.0); optional external endpoint (Ollama / cloud) as a setting | Replies, memory summaries | C++ validator; playtest logs reviewed weekly | Free: runs on the player's PC; download +2.8 GB; needs a GPU with ~6 GB for fast replies |

**Not made with AI:** the story, the lore bible and the key story lines (final line, nickname); the twist and its pacing; Scene Director logic; final look of the portraits; level design. Game-data lines are AI-assisted drafts that the team reviews and rewrites (marked in the tables).

**First test (03.11.2026):** 5 player lines sent to a Zafir prompt in a chat session; all 5 were valid JSON, in character and leak-free. Lesson: 4 of 5 replies were teasing, so Zafir's card needs a line saying when he turns cold or angry; retest through the API with harder questions ("Did Vashir hire you?") and measure reply times.

### 8.3 Top Risks and What We Cut
| Risk | Early warning (4-week plan) | What we cut |
| :-- | :-- | :-- |
| LLM integration slower or less reliable than planned (built-in model too slow on target PCs) | End of week 1 / mid week 2: the character's first words take over 4 s on average to appear on screen (measured in the game window, not the editor; changed 13.11.2026 from "whole reply over 4 s", because the reply is shown word by word as it is written), or under 90% valid JSON | Smaller model or shorter replies; court NPCs and the Chapter 7 confession become pre-written |
| Writing takes longer (review of cards, lines, cutscenes) | End of week 2: Chapter 2 lines not reviewed | Use the drafted lines as they are; cutscenes become still images with text |
| Six palace chapters plus a spirit chapter (11.11) | End of week 2: Chapter 1 not complete | Chapters 2 and 4 get a shorter day (fewer clues, no new guard routes); the spirit walk keeps 6 embers but no new art |
| 3D art and animation take too long | End of week 2: palace still grey-box | Custom models; final portraits (placeholders stay) |

### 8.4 API Keys and Cost
No API key and no cost (decided 10.11.2026): the AI model ships inside the game and runs on the player's PC, so players download nothing else and nobody pays per token. The model and server files (~2.8 GB) are not in the repository; `Tools/GetLocalLLM.ps1` downloads the pinned versions (SHA-256 checked) and the packaged build ships them in `ThirdParty/LLM`. Licenses: llama.cpp MIT, Qwen3.5-4B Apache 2.0 (`Docs/Licenses.md`). If an optional external endpoint is ever used, its key is never in the repository, `Config/*.ini` or the build (environment variable `ASHES_LLM_API_KEY` or `Secrets/`, git-ignored). Turn limits (Zafir 11, Vashir 7, others 5) still keep each scene short.

### 8.5 References
- **Games:** Façade (Procedural Arts, 2005); Disco Elysium (ZA/UM, 2019); Hades (Supergiant Games, 2020); AI Dungeon (Latitude, 2019).
- **Papers:** Mateas, M. and Stern, A. (2003). *Façade: An Experiment in Building a Fully-Realized Interactive Drama.* GDC. Park, J. S. et al. (2023). *Generative Agents: Interactive Simulacra of Human Behavior.* arXiv:2304.03442.
- **Course materials (Advanced Game Programming):** state machines, believability, LLM-powered characters, local models.
- **Tools:** Unreal Engine 5.8 documentation (Enhanced Input, UMG, AI Perception, Behavior Trees, Navigation, Sequencer, Quartz, MetaSounds, Niagara, HTTP module, Automation tests); LLM provider docs (structured output, rate limits, usage policy); asset licenses in `Docs/Licenses.md`.

---

## Section Status
| Section | Status |
| :-- | :-- |
| Header | FILLED |
| 1. Pitch | FILLED |
| 1. Characters | FILLED (MISSING: looks of the 4 minor characters) |
| 1. Gameplay, Core Loop, Main Mechanics | FILLED |
| 1. Win and Lose | FILLED |
| 1. Gameplay Walkthrough | FILLED |
| 1. Mindset | FILLED |
| 1. Uniqueness | FILLED |
| 1. Design Pillars | MISSING (suggested pillars given, need confirmation) |
| 1. Out of Scope | FILLED |
| 2. Screens | FILLED (MISSING: lose screen, credits buttons, video/control settings) |
| 2. Controls | FILLED (dance keys proposed) |
| 2. Camera | FILLED (MISSING: height offset, camera zones) |
| 2. Mechanics | FILLED (MISSING: memory failure, weapon Suspicion, dance mid-range) |
| 3. Level Design Philosophy | FILLED (MISSING: guidance cues) |
| 3. Themes | FILLED |
| 3. Game Flow | FILLED |
| 4. Abstract / Derived Classes | FILLED (Unreal 5.8 since 10.11) |
| 4. Data | FILLED |
| 4. AI | FILLED (MISSING: hearing, state timings) |
| 5. Style Attributes | FILLED |
| 5. Graphics Needed | FILLED (MISSING: guard animation list, smoke/petal VFX) |
| 6. Style Attributes | FILLED |
| 6. Sounds Needed | FILLED (MISSING: UI sounds) |
| 6. Music Needed | FILLED |
| 7. Schedule | FILLED (4 weeks; team of four since 12.11) |
| 8. Appendix | FILLED |
