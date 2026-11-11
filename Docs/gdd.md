# Ashes for a Goddess
## Game Design Document (gdd.md)

> *"Some gods brighten the sky when they are born; others, when they die."*

**Author:** Burnt Potato Games (BPG): Merve Özal, Tuana Tuna · **Version:** 1.3 (Markdown version of the midterm GDD dated 07.11.2026; 4-week schedule; "Court of Whispers" investigation concept added 09.11.2026; day/night loop, nightly dance and guard stealth added 10.11.2026; engine changed to Unreal Engine 5.8 on 10.11.2026) · **Date:** 10.11.2026
**Genre:** Third-person narrative RPG and visual novel hybrid; dark fantasy romance · **Platform:** Windows PC, keyboard and mouse (Unreal packaged build) · **Engine:** Unreal Engine 5.8, C++ and Blueprints, 3D, Lumen · **Players:** Single player

| Field | Details |
| :-- | :-- |
| Course | Game Studio: AI in Game Development (midterm GDD in Week 8, final build in this 4-week plan) |
| Target player | Players aged 16+ who enjoy story-driven and romance games; visual novel fans curious about AI characters |
| Play time | About 25–30 minutes per playthrough (*proposed*, changed 10.11): 4 chapters of about 6–7 minutes (day investigation, night dance, key conversation) plus cutscenes. The midterm GDD said 12–15 minutes |
| Rating target | Teen: violence and death are shown; the romance is not explicit |

| Member | Role | Owns |
| :-- | :-- | :-- |
| Merve Özal | Lead Programmer, Level Designer, Audio Designer | Player controller, camera, LLM client, prompt builder, validator, level, audio |
| Tuana Tuna | Lead Artist, UI Designer, Narrative Designer, Programmer | Lore bible, character and scene cards, fallback lines, portraits, UI screens, playtests |

> **Concept update (09.11.2026): "Court of Whispers".** The team chose to make the palace an **open palace** that the player explores freely, talking to **several NPCs to gather information (clues)**, with cutscenes as the story advances. On 10.11.2026 the team fixed the three active parts: **gather information (Rumor Journal)**, **avoid patrolling guards** and **dance every night** to keep Sahra's cover, in a **day / night loop** per chapter. The story, characters, AI conversation systems and ending from the midterm GDD stay the same. New parts are marked **(new 09.11)** or **(new 10.11)**; numbers marked *proposed* are starting values for playtests, not from the midterm GDD.

> **Engine note (changed 10.11.2026).** The midterm GDD planned Unity 6 LTS. On 10.11.2026 the teacher approved the switch to **Unreal Engine 5.8** (C++ and Blueprints), which the course templates assume. Every document in `Docs/` now uses Unreal terms. The earlier Unity skeleton is kept in Git as the tag `unity-skeleton`.

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
An assassin disguised as a dancer sneaks into the palace of a king who burns villages to save a dying sun. The player is Sahra, the sole survivor of a burned village, who explores the palace in third person while guards watch and talks to the characters in her own words. Instead of picking from a list, the player types anything (or picks one of 3 suggested replies), and the characters answer in character, remember what was said and treat Sahra differently because of it. The palace is open: each chapter poses a question, and the player roams the House of Embers and draws answers out of dancers, guards, servants and priests in her own words. They gossip, lie, bargain and clam up, and every useful answer becomes a clue in Sahra's Rumor Journal. By day she gathers clues while avoiding patrolling guards, slipping into hiding spots and eavesdropping where the guards gossip. Every night she must dance in the court to keep her cover: a rhythm minigame where a fine performance wins the king's favor and overheard secrets, and a clumsy one draws suspicion. Enough clues open the chapter's key conversation with Zafir or Vashir, and the story moves on in a cutscene. The fun is detective work where your words are the tool, wrapped around a romance that feels personal: a rival assassin who slowly falls for you, and a king who has been hunting you all along. The main story and ending are fixed, but how the romance develops and what the characters reveal depends on the player.

### Characters

#### Sahra
- **Role:** Player character. Sole survivor of the village Tamra; the desert's best assassin, disguised as a dancer.
- **Look:** A dancer in silks and a veil with anklets (her disguise); long dark hair and red-and-gold jewelry in the conversation portrait. Linked to the Ember orange color (#E8642C).
- **Abilities:** Walk (2.2 m/s) and run (4.5 m/s) through the palace; talk to characters by typing up to 200 characters or picking 1 of 3 suggested replies; interact with highlighted characters and objects (E); hide in hiding spots (E) and eavesdrop from them; dance every night in the court (rhythm minigame, must have since 10.11), with the private dance for Vashir in Chapter 2. In the epilogue she is revealed to be the phoenix and walks unburned out of the sacred fire.
- **Personality / behaviour:** Voiced by the player's own words. She wants Vashir dead. She hides that she survived the fire at all.

#### Zafir
- **Role:** NPC driven by the LLM. Charming rival assassin and romance interest.
- **Look:** Long dark wavy hair, gold earrings and a gold circlet, dark clothing (conversation portrait in GDD Figure 5). Linked to Old gold (#C9A25B). Carries a small knife that Sahra steals in Chapter 1.
- **Abilities:** Free conversation (10 turns per scene); hidden Trust (start 20) and Affection (start 10), each ±8 per turn; 6 emotion states (Neutral, Teasing, Tender, Jealous, Cold, Angry); at Trust 50 he admits he is also hunting Vashir; at Trust 70 he shares the route to the king; at Affection 60 the rooftop confession plays in full; remembers up to 6 memories.
- **Personality / behaviour:** Calls Sahra "Little Ember". Teases, uses fire metaphors and makes jokes when he is hurt. Wants the king first; then, quietly, Sahra's safety. Hides that Vashir hired him to find and kill her, and that he has sabotaged the contract for months. Asks questions back, refuses, bargains and changes the subject. Shows jealousy after the private dance in Chapter 2.

#### Vashir
- **Role:** NPC driven by the LLM. The Priest-King of the Dunes and the antagonist.
- **Look:** MISSING: physical description of Vashir (the GDD only links him to Burgundy #6B1E2E, his court and its curtains).
- **Abilities:** Free conversation (8 turns per scene; 4 turns in Chapter 4); hidden Favor (start 30, ±6 per turn) and Suspicion (start 10, +10 / −5 per turn); 5 emotion states (Serene, Curious, Pleased, Suspicious, Wrathful); at Favor 60 he invites Sahra to dance in his chamber; at Suspicion 100 guards seize Sahra and the chapter restarts.
- **Personality / behaviour:** Talks slowly, seems warm and kind, religious; asks probing questions that raise Suspicion if Sahra says the wrong thing. Wants the phoenix's power, to live forever. Hides that every fire was a hunt for the phoenix. Teaches that the Sun Goddess is a phoenix who must be fed with fire.

#### Nura
- **Role:** Court NPC (pre-written dialogue by default; LLM character only if the week-2 risk check passes). Head of the royal dancers; she runs the nightly performances and judges Sahra's dance. Found in the Dancers' Quarters in Chapter 1.
- **Look:** MISSING: physical description of Nura.
- **Abilities:** Free conversation (4 turns per scene); 4–6 portrait emotions.
- **Personality / behaviour:** Gossipy, quick, practical. Wants to survive the court. Hides that she lost a sister to a burning.

#### Captain Idris
- **Role:** Court NPC (pre-written dialogue by default; LLM character only if the week-2 risk check passes). Captain of the palace guard; he sets the patrol routes. Found at the Guard Post in Chapter 1.
- **Look:** MISSING: physical description of Captain Idris.
- **Abilities:** Free conversation (4 turns per scene); 4–6 portrait emotions.
- **Personality / behaviour:** Short, formal, suspicious. Wants order in the palace. Hides that he doubts the burnings.

#### Minor Court NPCs (new 09.11)
- **Role:** 3–4 short-lived information sources (proposed: a palace servant, a temple priest, a talkative off-duty guard, a second dancer). Each uses a pre-written dialogue tree (pick 1 / 2 / 3) and holds 1–3 clues per chapter.
- **Look:** Licensed base models, recolored (same rule as the guards). MISSING: individual looks.
- **Abilities:** Free conversation (4 turns); give clues; may ask a small favor or demand a clue in return before talking (proposed).
- **Personality / behaviour:** MISSING: names, voices and secrets; Tuana writes their cards by hand (GDD 12.3: character cards are not AI-generated).

#### Palace Guards
- **Role:** Enemies (**must have since 10.11**). 2–4 guards on fixed, timed patrol routes that the player can learn.
- **Look:** The same licensed base model, recolored.
- **Abilities:** View cone of 70° and 9 m; simple state machine Patrol → Notice (stops and looks while Suspicion rises) → back to Patrol; no chasing (simplified 10.11 for the 4-week scope); cannot see into hiding spots; pairs of guards stop at set points and talk (eavesdrop chances); raise Suspicion when Sahra runs in view, stays in a restricted room or is seen with a weapon.
- **Personality / behaviour:** Predictable patrols; they do not talk.

### Gameplay
Second by second, the player walks Sahra through the palace (walk by default, Shift to run), keeping out of guard view cones. When a highlighted character shows "E: Talk", the player presses E and a conversation opens over the 3D scene. The player types a line (up to 200 characters) and presses Enter, or presses 1 / 2 / 3 to pick a suggested reply. After a typing indicator of at least 0.4 s, the character answers in 1–4 sentences, and the portrait and gesture change within 1 second. A one-line hint (for example "Zafir looks at you a moment too long.") shows how the character's opinion changed. Each chapter is **one day and one night** (new 10.11). By day, the player roams the open palace choosing whom to question, collects clues into the Rumor Journal (a short "New clue" toast appears), uses those clues as new topics with other characters, slips past patrols, hides behind curtains and lattice screens and eavesdrops on guards. At nightfall the player goes to the Throne Hall and dances (30–45 s rhythm minigame) to keep Sahra's cover; a good dance pays out Favor and a reward. Then, with enough clues, the player opens the chapter's key conversation and pushes it toward the hidden beats before the turns run out. A cutscene and chapter card then move the story on, and the NPCs move to new places for the next question.

#### Core Loop
**Chapter loop (new 10.11):** DAY: read the chapter question → explore the open palace while avoiding patrols (hide, eavesdrop, question NPCs) → clues go into the Rumor Journal → use clues as topics with others → NIGHT: dance in the court (good dance = Favor + reward; bad dance = Suspicion) → KEY SCENE with Zafir or Vashir (needs the required clues) → cutscene → next day.

**Investigation loop (new 09.11):** read the chapter question → explore the open palace (walk, sneak, find people) → question an NPC (type or pick a reply) → the NPC reacts and may give a clue (Rumor Journal) → use the clue as a topic with someone else → repeat until enough clues → the key conversation opens. **Key scene loop (GDD Figure 1):** talk freely → the character reacts (mood, hidden values, memory) → is the scene goal met? If no, keep talking (the turn limit applies). If yes, a story beat plays (cutscene, chapter card) → the next chapter question appears → explore again. If Suspicion reaches 100 at any point, the cover is blown and the chapter restarts from its checkpoint (GDD Figure 1).

#### Main Mechanics

##### 1. Free Conversation
The player types up to 200 characters or picks one of 3 suggested replies. The character answers in 1–4 sentences (max 80 words), with an emotion and a gesture. Each scene has a turn limit (10 Zafir, 8 Vashir, 4 court NPCs), so it stays short and API costs stay predictable. It is fun because pre-written dialogue trees can't respond to what the player actually types, so the romance feels personal.

##### 2. Relationship Values
Each main character has two hidden values from 0 to 100 (Zafir: Trust and Affection; Vashir: Favor and Suspicion). Every turn the LLM suggests a change and the game clamps it before applying it. The values unlock dialogue, change the character's tone and decide which epilogue lines play. The UI never shows numbers, only a mood icon and a one-line hint. It is fun because characters react to what was actually said, not to picking "the nice option".

##### 3. Scene Goals and Beats
Each scene has 2–3 hidden beats (key story moments such as `zafir_reveals_target` or `alliance_agreed`). The LLM marks a beat when it happens, and the Scene Director checks the rules (for example enough Trust) before accepting it. When all required beats are done, the scene goal is met. The Scene Director nudges the character 2 turns before the limit or after 3 turns with no progress, and force-closes the scene with a weaker outcome at the limit. It is fun because open conversations still have a clear purpose and never drag.

##### 4. Memory
After each scene, a summary call writes 1–2 short memories for each character (6 kept per character, oldest dropped). Memories come back in later scenes and in the epilogue; in the pyre scene Zafir's confession quotes two moments from the player's own conversations. It is fun because the ending becomes personal (should have).

##### 5. Exploration and Suspicion
Sahra explores the palace in third person. Guards on set patrol routes notice if she runs, stays in restricted rooms or is seen with a weapon, and Suspicion rises; it falls slowly when no guard is watching. Vashir can also raise Suspicion when Sahra answers carelessly. **(Changed 10.11, must have):** guards walk fixed, timed routes the player can learn; hiding spots (curtains, lattice screens, alcoves) make Sahra invisible; the best clues sit near guards (guards gossiping at their stops, guarded rooms), so sneaking is how information is earned, not only an obstacle. It is fun because it adds readable tension between conversations.

##### 6. Clues and the Rumor Journal (new 09.11)
Each chapter has one question and 5–6 clues spread over 3–4 NPCs (*proposed*). A clue is a short fact the team writes in advance (for example "Nura: the dancer with the blade arrived the same night as the new contract riders"). When the conversation reveals it, the LLM marks it in the JSON (`clues`) the same way it marks beats, and the Scene Director accepts it only if that NPC holds that clue in this chapter, so the model cannot invent clues. Accepted clues go into the Rumor Journal (Tab) with the authored text. Clues unlock new topics: a clue becomes an extra suggested reply and is added to the next NPC's prompt so they react to it ("Nura told me about your sister..."). When the chapter's required clues are found (3 of 5–6, *proposed*), the key conversation opens. In offline mode, pre-written trees give the same clues. It is fun because the player is a detective whose tool is conversation, and every NPC is worth talking to.

##### 7. Nightly Dance (must have since 10.11)
Every night Sahra performs in the Throne Hall, because a dancer who skips the performance blows her cover. It is the GDD's timing minigame: 4 keys, 0.25 s hit windows, 30–45 s per dance. Above 80% accuracy: +10 Favor and a reward (an overheard clue from the watching nobles, or access such as an invitation or a key); 40–80%: no change; below 40%: +10 Suspicion ("a real dancer wouldn't miss"). Each night is different: a new song, a faster tempo and a new audience; the Chapter 2 dance is the private dance for Vashir in the King's Chamber. It is fun because it is a short skill break between conversations and it feeds the investigation.

##### 8. Hiding and Eavesdropping (new 10.11)
Hiding spots (curtains, lattice screens, alcoves) are marked by a soft gold glow; E enters and leaves them; guards cannot see into them. Eavesdrop zones sit next to hiding spots where guards or courtiers stop to talk: staying in the zone unseen for the whole exchange (about 10 s) adds an authored clue. Being seen breaks the exchange. It is fun because it turns the guard patrols into an information source.

#### Win and Lose
- **Win:** Finish the final chapter. The ending is always the same, but the epilogue lines change with the relationship values and the memories the characters kept.
- **Lose:** Suspicion reaches 100, either from guards (running, going into restricted rooms), from Vashir (asking careless questions) or from a bad night dance (below 40%). Running out of turns is not a game over: the scene still ends, but with a weaker outcome and lower relationship values.
- **What happens after losing:** The chapter restarts from its checkpoint (the start of each chapter).

#### Gameplay Walkthrough
1. **Title screen:** embers drift over the dying sun. The player picks New Game.
2. **Prologue: Ash (cutscene):** Tamra burns and a girl walks out of the smoke. Years later, Sahra arrives at the palace as a dancer. Space advances the text.
3. **Chapter card:** "Chapter 1 · The Garden of Ash" with one line of verse, for 3 seconds.
4. **Dancers' Quarters (start and checkpoint):** Sahra starts here. The chapter question at the top left reads "Find out why the other dancer carries a blade." Nura is here; talking to her gives gossip and a first clue. A toast shows "New clue" and Tab opens the Rumor Journal.
5. **Open palace:** every room except the King's Chamber (restricted) and the city gate (locked) is open. The player walks into the moonlit garden with its dry fountain and lanterns. A guard patrols the garden; running in view raises Suspicion by 12 per second, and the eye icon fills while a guard sees Sahra. The player chooses whom to question next: Captain Idris at the Guard Post, a servant in the Throne Hall, a priest on the Rooftop Terrace (minor NPCs with pre-written dialogue). Mentioning Nura's clue to Idris opens a new topic. Behind a lattice screen in the garden, Sahra can eavesdrop on two guards at their stop for a clue. Zafir is visible in the garden but avoids Sahra ("Not now, Little Ember.") until she has 3 clues.
6. **Nightfall:** a bell rings; the goal line changes to "Perform in the Throne Hall". Sahra dances for the court (30 s, 4 keys). A good dance adds Favor and a noble's gossip as a clue; a poor one adds Suspicion. After the performance, Zafir waits in the garden.
7. **Meeting Zafir (key scene):** with 3 clues, after the dance the player confronts Zafir in the garden and presses E. Zafir opens with an authored line ("Careful, Little Ember. Blades that small cut their owners first."). The player has 10 turns to learn why he is here and to agree to work together (beats such as `zafir_reveals_target` and `alliance_agreed`). Hints like "Zafir looks at you a moment too long." show the mood. If 2 turns are left, Zafir suggests the alliance himself. Clues the player found are in his prompt, so he reacts to what she knows.
8. **Resolve:** when all beats are met, a closing line plays; Sahra has stolen Zafir's knife. The memory summary writes 1–2 memories per character.
9. **Next chapter:** a short cutscene, then the game saves at the checkpoint and the Chapter 2 card plays with a new question ("How does a dancer get a private audience with the king?"); NPCs move to new places.

### Mindset
The player should feel playful tension in the conversations and a slowly growing tragedy in the story: the talks are teasing and flirtatious, but fire and ash are everywhere and the ending is a death. Stealth sections (guards, Vashir's probing questions) should feel tense, because one careless word or step raises Suspicion. Zafir's warmth growing over the chapters should feel earned and personal, because it comes from the player's own words. The Trap (Chapter 4) should feel like betrayal and loss, and the pyre and epilogue should turn grief into triumph when Sahra rises as the phoenix and the sun returns. On a second playthrough the player should feel the irony of Zafir's dodged answers, now knowing what he was hiding.

### Uniqueness
- **Free conversation inside a fixed story:** the player types anything and characters remember it, unlike choice-list romance games. Compared with **Façade** (Procedural Arts, 2005), which also uses typed conversation inside a fixed story.
- **Dialogue as the main gameplay:** compared with **Disco Elysium** (ZA/UM, 2019), dialogue with hidden checks; here the hidden values are moved by an LLM and clamped by our code.
- **Relationships through many short conversations:** compared with **Hades** (Supergiant Games, 2020).
- **Investigation by conversation (new 09.11):** gather clues by talking to many NPCs in an open palace and use them as new topics. MISSING: the team's own comparison games for this part (detective and social-stealth games).
- **Open AI storytelling with hard limits:** compared with **AI Dungeon** (Latitude, 2019), our characters only know a ~600-word hand-written lore bible, brush off off-topic questions in character and cannot reveal the twist early, because secrets are only added to the prompt in the chapter where they can be revealed.
- **Memory system** based on Park et al. (2023), *Generative Agents*.
- **Always playable, no internet:** the AI model ships inside the game and runs on the player's PC (no account, no key, no internet); if it fails or is too slow, pre-written fallback lines answer instead (decided 10.11.2026).

### Design Pillars
> MISSING: the GDD has no explicit design pillars. The four below are suggested from the GDD's Unique Selling Points and scope rules; confirm or replace them.

#### P1. Your words matter
Every system (values, beats, memories, epilogue lines) must react to what the player actually said, never to "the nice option".

#### P2. Fixed story, personal romance
The main events and ending never change; only the romance, the reveals and the epilogue lines change.

#### P3. Characters stay in the world
Characters only know the lore bible, refuse off-topic questions in character and cannot leak secrets that are not yet in their prompt.

#### P4. Always playable
Every AI feature has a fallback (timeout fallback lines, offline mode), and the must-have features alone make a complete game.

### Out of Scope
- A true open world outside the palace: "open" means the whole palace is free to explore (one 3D level, about 60 × 30 m).
- Clues invented by the LLM: every clue is authored; the model can only reveal clues from the NPC's card.
- Guards that chase or fight: guards only notice and raise Suspicion (simplified 10.11).
- A free day / night clock: night falls on a story trigger (enough clues or the player chooses "Go to perform"), not on a timer (*proposed*).
- 3D levels for the pyre and the festival temple: they are 2D illustrations.
- Any 3D level other than the palace (one hub with five rooms).
- Voice acting (text only); text-to-speech for Zafir is only a nice-to-have and is cut in the 4-week plan.
- A hosted (cloud) LLM API as the main AI: replaced on 10.11.2026 by the built-in local model (free, no key); an external OpenAI-compatible endpoint stays an optional setting.
- A proxy server for an API key: not needed (no key).
- Full pre-written offline dialogue trees for every scene: proposed cut on 10.11.2026 (the built-in model is always there; ~120 fallback lines stay). MISSING: team confirmation.
- Gamepad support (the GDD targets keyboard and mouse only).
- Branching story or multiple endings: the ending is always the same.
- Visible relationship meters or numbers.
- Explicit romance content (Teen rating).
- AI-generated lore bible, character cards, key story lines, twist pacing, Scene Director logic, final portrait look and level design (made by the team, GDD 12.3).
- Combat system: MISSING confirmation, the GDD describes no combat mechanic.

---

## 2. Technical

### Screens

#### A. Main Menu (Title)
- **Elements:** New Game, Continue, Settings, Quit buttons; background with embers drifting over the dying sun.
- **Goes to:** New Game → Prologue cutscene → Chapter 1 card; Continue → last chapter checkpoint; Settings → Settings; Quit → exit to desktop.

#### B. In-Game HUD
- **Elements:** Exploration HUD: goal line at the top left; an eye icon that fills only while a guard sees Sahra; an "E: Talk" prompt over characters. Conversation screen (GDD Figure 5): chapter title and goal at the top left, "PLAYING AS SAHRA" tag, "Turns left: N · Tab: log" at the top right; dialogue panel with the speaker's line next to their small portrait and name; italic hint line under the line instead of a meter; Sahra's portrait below as the point of view; large character portrait on the right that changes with the emotion tag, with emotion icons; 3 numbered suggested-reply buttons (1 / 2 / 3); text field "Reply as Sahra to [name] · 200 characters" with a Send (Enter) button. "New clue" toast when a clue is accepted. Chapter card: chapter number, title, one line of verse and the chapter question, for 3 seconds. Time of day tag ("Day 1 · Morning" / "Night") and a "Perform in the Throne Hall" goal at nightfall (new 10.11). Dance screen (new 10.11): note lane with 4 arrows, hit feedback, accuracy at the end with the reward or Suspicion result. Rumor Journal (Tab, new 09.11; extends the GDD's log screen): the chapter question, found clues (authored text, who said it), clues still missing as "?" slots, the full conversation log and each character's memories in their own words.

#### C. Pause Menu
- **Elements:** Resume, Settings, Restart Chapter, Quit.

#### D. Game Over / Lose Screen
- **Elements:** MISSING: a dedicated lose screen. The GDD says that when Suspicion reaches 100, guards seize Sahra ("Cover blown") and the chapter restarts from its checkpoint; the message and any buttons are not specified.

#### E. Win / Level Complete Screen
- **Elements:** Chapter complete: the closing line, then the next chapter card (no stats screen, the values stay hidden). Game complete: the Epilogue cutscene ("The Long Flame") ending with "Careful, Little Ember.", then the Credits screen with the team, licensed assets and a list of AI-assisted assets and tools. MISSING: buttons on the Credits screen.

#### F. Settings
- **Elements:** Text speed, volume, AI mode (Online / Offline) and a status line: "AI: connected" or "AI: offline, pre-written dialogue". MISSING: video and control settings.

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
- **Behaviour:** The mouse orbits the camera around Sahra. During conversations the 3D scene stays visible behind the conversation UI. MISSING: follow distance, height, orbit limits, collision behaviour and any special camera zones.

### Mechanics

#### 1. Free Conversation
- **Rule:** The player types a line or picks one of 3 suggestions; the prompt builder sends the character card, lore bible, scene card, state and the last 8 turns; the reply must be JSON and must pass the validator before the player sees it.
- **Numbers:** Max input 200 characters; turn limit 10 (Zafir), 8 (Vashir; 4 in Chapter 4), 4 (court NPCs); 3 suggested replies; reply 1–4 sentences, max 80 words; response timeout 6 s; typing indicator at least 0.4 s; portrait and gesture change within 1 s of the reply.
- **Edge cases:** Longer input is cut with a warning. Timeout or network error: "hesitates" animation, then a pre-written fallback line for this beat and emotion; values unchanged. Broken JSON: 1 silent retry, then fallback. Off-topic input: in-character reply ("A dancer who talks of 'phones'? The heat has got to you."). Spoiler leak: regenerate once, then fallback. Explicit or hateful content: the character refuses in character; after 3 times the scene closes politely. Jailbreak: in-character confused reply. No API key or no internet: offline mode with pre-written dialogue trees. (GDD Figure 2 says "5 s timeout"; the GDD text says 6 s, and 6 s is used here.)

#### 2. Relationship Values
- **Rule:** Each value is 0–100 and hidden. The LLM suggests a delta every turn; our C++ code clamps it to the per-turn maximum and applies it.
- **Numbers:** Zafir Trust start 20, ±8 per turn (50: admits he is also hunting Vashir; 70: shares the route to the king). Zafir Affection start 10, ±8 (moves tone from teasing to tender; 60: rooftop confession plays in full). Vashir Favor start 30, ±6 (60: invites Sahra to dance in his chamber; also unlocks the King's Chamber). Vashir Suspicion start 10, +10 / −5 (100: guards seize Sahra, chapter restarts).
- **Edge cases:** An impossible change (for example +50 Trust) is cut to the per-turn maximum. Values are clamped to 0–100 (MISSING: confirmation of the 0 and 100 bounds behaviour). On fallback lines, values do not change. Unknown emotion: portrait falls back to Neutral (Zafir) or Serene (Vashir).

#### 3. Scene Director
- **Rule:** States Intro (authored opener) → Free talk (player leads) → Nudge (character steers) → Resolve (closing line) or Forced close (weaker beat) → Next chapter (save, cutscene); Caught (retry chapter) when Suspicion reaches 100.
- **Numbers:** Nudge at 2 turns before the limit or after 3 turns with no progress; 2–3 hidden beats per scene; forced close at the turn limit.
- **Edge cases:** Beat claimed without enough Trust: rejected by the Director. Scene goal never reached: nudge, then forced close. Out of turns is not a game over.

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
- **Numbers (proposed):** 5–6 clues per investigation chapter, held by 3–4 NPCs, 1–3 clues per NPC; 3 required to open the key scene; minor NPC turn limit 4; "New clue" toast 2 s; Chapter 4 has no clue requirement (the trap is scripted).
- **Edge cases:** The model claims a clue the NPC doesn't hold: rejected, nothing added. A clue already found: ignored. The NPC's turns run out before the clue: the clue is offered in the forced-close line (proposed) so the chapter can never soft-lock. Offline mode: each pre-written tree contains the NPC's clues. Caught (Suspicion 100): clues found in this chapter are kept (proposed, MISSING: confirm).

#### 7. Nightly Dance (must have since 10.11)
- **Rule:** Timing minigame with 4 keys, once per chapter at night in the Throne Hall (Chapter 2: the private dance in the King's Chamber); new song, faster tempo and new audience each night.
- **Numbers:** Hit window 0.25 s; accuracy above 80%: +10 Favor; below 40%: +10 Suspicion.
- **Numbers (added 10.11, proposed):** 30 s on night 1 rising to 45 s; tempo +10% per night; 40–80% accuracy: no change; > 80%: also a reward (overheard clue or access). Dance keys ← ↑ ↓ →.
- **Edge cases:** Below 40% adds +10 Suspicion and can trigger Caught at 100 (restart the chapter at its checkpoint). Missing the reward clue never blocks progress: every required clue can also be found by day. Pause during the dance stops the music and notes.

---

## 3. Level Design

### Level Design Philosophy
The game has one 3D level: the palace, built as a hub (the Garden of Ash) with five rooms around it, about 60 × 30 m. **(Changed 09.11:)** the palace is open from Chapter 1: every room except the restricted King's Chamber and the locked city gate can be explored at any time, and NPCs move to new rooms each chapter. The chapter question and the NPCs who are present guide the player, instead of locked doors. The player always passes through the garden, so its guard patrol is the main stealth challenge in every chapter. Lanterns and firelight pick out details against plum and burgundy shadows; restricted rooms (the King's Chamber, Rose red) raise Suspicion when Sahra is seen in them. The on-screen goal line states the chapter goal. MISSING: specific guidance techniques (light, color, sound or shape cues) for leading the player to characters and doors.

### Themes

#### Theme 1: Prologue: Ash (2D cutscene)
- **Mood:** Burning village at night; fire, smoke and ash.
- **Objects:** 2D background "Burning Tamra".
- **Challenges:** None (cutscene).
- **Goal:** Watch Tamra burn and Sahra arrive at the palace years later.

#### Theme 2: Chapter 1: The Garden of Ash
- **Mood:** Moonlit garden with a huge pale moon, lanterns, a dry fountain; night plum and old gold.
- **Objects:** Whole palace open except the King's Chamber; Dancers' Quarters (start, checkpoint), Garden of Ash hub (dry fountain, lanterns), Guard Post (gate to the city, locked); Zafir's knife.
- **Challenges:** 2 guards, garden patrol; gather 3 clues from Nura, Idris and minor NPCs (4 turns each), then talk to Zafir (10 turns).
- **Goal:** Question: "Why does the other dancer carry a blade?" Learn why Zafir is here and agree to work together.

#### Theme 3: Chapter 2: The Private Dance
- **Mood:** Vashir's court: burgundy curtains, gold, firelight.
- **Objects:** + Throne Hall (audience with Vashir, court NPCs, 1 guard), King's Chamber (by invitation).
- **Challenges:** 3 guards; gather 3 clues about what pleases the king (proposed question: "How does a dancer get a private audience with the king?"); Vashir's probing questions raise Suspicion (8 turns); the night's dance is the private dance in the King's Chamber (rhythm minigame).
- **Goal:** Get a private meeting with Vashir without raising Suspicion. Afterwards, Zafir's jealousy shows.

#### Theme 4: Chapter 3: Rooftop of Smoke
- **Mood:** Rooftop terrace on the upper level, smoke and moonlight; romance.
- **Objects:** + Rooftop Terrace, reached by stairs from the Dancers' Quarters.
- **Challenges:** Gather 3 clues for the strike (proposed question: "How do we reach the king's bed unseen?": guard shifts, the chamber's hidden door, the festival timing), then talk with Zafir (10 turns).
- **Goal:** Plan the strike. If Affection is high (60), they both admit their feelings.

#### Theme 5: Chapter 4: The Trap
- **Mood:** King's Chamber, Rose red; betrayal.
- **Objects:** King's Chamber.
- **Challenges:** Vashir (4 turns), Zafir, guards.
- **Goal:** Vashir reveals that he hired Zafir to kill her. Sahra is cut down and dies in Zafir's arms.

#### Theme 6: Chapter 5: The Pyre (2D cutscene with generated lines)
- **Mood:** Night at the ruins of Tamra, the pyre burning; grief.
- **Objects:** 2D background "The pyre".
- **Challenges:** None (cutscene).
- **Goal:** Zafir burns her body in Tamra and confesses to the flames, using the memories he kept. The dying sun flares.

#### Theme 7: Epilogue: The Long Flame (2D cutscene)
- **Mood:** Festival temple, sacred fire, the phoenix; triumph.
- **Objects:** 2D background "The temple"; Zafir's stolen knife.
- **Challenges:** None (cutscene).
- **Goal:** Sahra rises as the phoenix and walks unburned out of the sacred fire; Vashir falls and the sun returns. Final shot: "Careful, Little Ember."

### Game Flow
1. Title → Prologue: Ash (cutscene, start).
2. Chapter 1: The Garden of Ash (tutorial for walking, talking, gathering clues and the garden patrol; whole palace open except the King's Chamber; 2 guards).
3. Chapter 2: The Private Dance (new question and clues; Vashir's Suspicion; the King's Chamber opens by invitation; 3 guards).
4. Chapter 3: Rooftop of Smoke (new question and clues; Affection decides the confession on the rooftop).
5. Chapter 4: The Trap (King's Chamber; only 4 turns; the twist).
6. Chapter 5: The Pyre (cutscene with generated confession).
7. Epilogue: The Long Flame (cutscene; epilogue lines depend on the values and memories) → Credits.

Difficulty grows through more guards (2 → 3), clues that need other clues first (proposed for Chapters 2–3), Vashir's Suspicion entering in Chapter 2 and fewer turns in Chapter 4.

---

## 4. Development

### Abstract Classes
> Unreal Engine 5.8: C++ base classes for the testable core, Blueprints for glue, UI and animation. Class names come from GDD Figure 6, translated to Unreal (full spec in `technical_design.md`).

| Class | Parent | Purpose |
| :-- | :-- | :-- |
| `ULLMProvider` | UObject (abstract, C++) | The only way game code reaches the LLM; implemented by Cloud, Local and Offline script providers |
| `AAshesGameMode` | AGameModeBase (C++) | Rules: chapters, checkpoints, Caught → restart chapter |
| `UAshesGameInstance` | UGameInstance (C++) | Current chapter, AI mode, relationship values and memories across level loads |
| `AAshesCharacter` | ACharacter (C++) | Sahra's movement (walk / run), hide state, Spring Arm camera |
| `AAshesPlayerController` | APlayerController (C++) | Enhanced Input mapping contexts, UI focus, pause |
| `UInteractionComponent` | UActorComponent (C++) | "E: Talk" starts a scene; E enters a hiding spot |
| `USceneDirectorSubsystem` | UWorldSubsystem (C++ state machine) | Intro, talk, nudge, resolve, forced close; accepts beats and clues |
| `UClueJournalSubsystem` (new 09.11) | UGameInstanceSubsystem (C++) | Found clues per chapter, topic unlocks, key-scene gate |
| `UDayNightSubsystem` (new 10.11) | UWorldSubsystem (C++) | Day → Night → Key scene per chapter; nightfall, lighting, NPC spawns |
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
| `UCloudLLMProvider` | `ULLMProvider` | Cloud LLM API over `FHttpModule`, JSON output, 6 s timeout, key from an environment variable |
| `UOfflineLLMProvider` | `ULLMProvider` | Pre-written dialogue trees and fallback lines (DataTables); offline mode |
| `ULocalLLMProvider` | `ULLMProvider` | Local model through Ollama (nice to have, cut in the 4-week plan) |
| `GM_Ashes`, `GI_Ashes`, `PC_Ashes` | `AAshesGameMode`, `UAshesGameInstance`, `AAshesPlayerController` | Default classes, widget classes and input contexts set in the editor |
| `BP_Sahra` (+ `ABP_Sahra`) | `AAshesCharacter` | Mesh, animation Blueprint (walk, run, hide, dance moves), walk 220 cm/s, run 450 cm/s, Spring Arm camera |
| `BP_Zafir`, `BP_Vashir`, `BP_Nura`, `BP_Idris`, `BP_CourtNPC` | ACharacter + `UNPCBrainComponent` | Character card row, emotion set, values and memories per character |
| `BP_Guard` (+ `BT_Guard`, `BB_Guard`) | ACharacter, controlled by `AGuardAIController` | Patrol route, 70° / 900 cm sight, NavMesh movement |
| `BP_HidingSpot`, `BP_EavesdropZone` | `AHidingSpot`, `AEavesdropZone` | Curtain or lattice mesh; overheard lines and clue id |
| `WBP_Conversation`, `WBP_HUD`, `WBP_RumorJournal`, `WBP_Dance`, `WBP_Title`, `WBP_Settings`, `WBP_Pause`, `WBP_ChapterCard`, `WBP_Cutscene`, `WBP_Credits` | UUserWidget | Screens from section 2 in the `design.md` style |

### Data
> DataTables (`DT_`, rows are C++ structs deriving from `FTableRowBase`, editable as CSV) and Data Assets (`DA_`) in `Content/Ashes/Data/`.

| Data Table | Rows (examples) | Fields |
| :-- | :-- | :-- |
| Character cards (`DT_Characters`) | Zafir, Vashir, Nura, Idris, Sahra | Voice, wants, fears, speech rules, what they know, what they must not say, emotion states, start values, max change per turn, unlock thresholds |
| Scene cards (`DT_Scenes`) | Ch1 Garden of Ash, Ch2 Private Dance, Ch3 Rooftop of Smoke, Ch4 The Trap | Place, time, goal, beats and their rules, turn limit, characters, secrets added in this chapter, authored opener, closing lines |
| Lore bible (`DA_LoreBible`) | One ~600-word text | Kingdom and dying sun, cult of the Sun Goddess, palace rooms and routines, festival, dancers, guard |
| Fallback lines (`DT_FallbackLines`) | ~120 lines by scene, beat and emotion | Scene, beat, emotion, line, AI-assisted flag |
| Offline dialogue trees (`DT_OfflineTrees`) | One tree per scene | Scene, node, 3 replies, character line, value changes, beats, clues |
| Cutscene script (`DT_Cutscenes`) | ~150 lines (Prologue, Ch4–5, Epilogue) | Part, speaker, line, background |
| Clue table (`DT_Clues`, new 09.11) | 5–6 clues per chapter, e.g. `ch1_nura_contract_riders` | Id, chapter, holder NPC, authored journal text, topic text for the suggested reply, prompt hint, required (yes / no) |
| Dance charts (`DT_DanceCharts`, new 10.11) | One per night (4) | Song, BPM, tempo multiplier, length, notes (beat, lane), reward clue |
| Exploration settings (`DA_ExplorationSettings`) | Walk 220 cm/s, run 450 cm/s, view cone 70° / 900 cm, Suspicion rates | Speeds, cone angle and range, Suspicion per second per cause |

### AI
**Guards (must have since 10.11, simplified):** `BP_Guard` with a Behavior Tree on a NavMesh, following fixed, timed waypoint routes: Patrol → Notice (stop and look while Suspicion rises) → Patrol; no chasing; hiding spots block their sight; eavesdrop stops where two guards talk. They see Sahra through AI Perception sight: a 70°, 9 m (900 cm) view cone. They raise Suspicion when she runs in view (+12/s), is in a restricted room in view (+20/s) or is seen with a weapon. The player escapes by leaving their view: Suspicion falls by 5 per second out of view. At 100 the chapter restarts. MISSING: hearing, Notice/Investigate timings and what Alert does before the restart.

**Conversation AI:** the LLM writes what characters say; our C++ decides what changes. See Appendix 8.1.

---

## 5. Graphics

### Style Attributes
- **Art style:** Stylized 3D palace (a licensed modular desert or palace kit, lit with Lumen) and licensed character base models; conversations use painted character portraits inside ornate gold frames; prologue, pyre and temple are painted 2D backgrounds. "A moonlit palace at the end of the world."
- **Color palette:** Night plum `#2A1426` (backgrounds, night sky, UI panels); Burgundy `#6B1E2E` (Vashir's court, curtains, headings); Rose red `#9E1B32` (roses, danger, the King's Chamber); Old gold `#C9A25B` (UI frames, jewelry, Zafir); Ember orange `#E8642C` (fire, the phoenix, Sahra); Moon cream `#F1E3C8` (text on dark UI, moonlight).
- **References:** Mood board from the team's own photos and free or licensed images (no named artist's style copied): desert palaces at night (sandstone arches, lattice screens, a huge pale moon); lanterns and firelight on dark cloth; smoke and red rose petals in the air; dancers' silks and veils in motion; gold jewelry catching the light; burnt villages and ash fields for Tamra; a phoenix of fire for the ending.
- **Lighting:** Night: deep plum and burgundy shadows, moonlight, with gold and firelight (lanterns, fire) picking out the details.

### Graphics Needed

#### Characters
| Asset | Description | Animations |
| :-- | :-- | :-- |
| Sahra | Licensed base model, recolored; dancer disguise (veil, anklets) | Walk, run, idle, talk gestures, hide (enter / leave), 4 dance moves + idle dance loop for the nightly dance |
| Minor court NPCs (3–4, new 09.11) | Same base models, recolored | Idle, talk gestures (shared set) |
| Zafir | Licensed base model, recolored | Idle, walk, 6 talk gestures (shared set), "hesitates" |
| Vashir | Licensed base model, recolored | Idle, talk gestures, "hesitates" |
| Nura | Licensed base model, recolored | Idle, talk gestures |
| Captain Idris | Licensed base model, recolored | Idle, talk gestures |
| Palace guards (2–4) | Same base model, recolored | Walk (patrol), idle, alert (MISSING: exact list) |
| Portraits | Zafir, Vashir, Nura, Idris with 4–6 emotions each (~20) plus Sahra POV portrait; AI-assisted drafts repainted by Tuana | Swap per emotion tag within 1 s |

#### Environment and Props
| Asset | Description | Where used |
| :-- | :-- | :-- |
| Modular palace kit | Licensed desert or palace kit (sandstone arches, lattice screens) | Whole palace, about 60 × 30 m |
| Dry fountain and lanterns | Garden centrepiece and light sources | Garden of Ash (hub) |
| Curtains, throne | Burgundy court dressing | Throne Hall, King's Chamber |
| Stairs and terrace | Upper level reached by stairs | Rooftop Terrace |
| Gate to the city (locked) | Gate prop | Guard Post |
| Zafir's knife | Asset store or modeled by us | Stolen in Chapter 1, final shot |
| Dancer's veil and anklets | Asset store (disguise) | Sahra |
| 2D backgrounds | Burning Tamra, the pyre, the temple; AI-assisted concepts finished by us | Prologue, Chapter 5, Epilogue |

#### Visual Effects (VFX)
| Effect | When it plays |
| :-- | :-- |
| Fire | Lanterns, braziers and burning scenes |
| Embers | Title screen (drifting over the dying sun), around fire |
| Pyre | Chapter 5: The Pyre |
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

---

## 6. Sounds & Music

### Style Attributes
Realistic and atmospheric: stone, fire, cloth and blades, with a Middle-Eastern palace music bed (oud, frame drum, drone), solo strings for romance and choir with drums for the finale. Calm and tense at night in the palace, tender in the romance scenes, epic in the finale. No voice in the must-have scope (text only).

### Sounds Needed
| Sound | When it plays |
| :-- | :-- |
| Footsteps on stone | Sahra and guards walking and running |
| Fire | Lanterns, braziers, the pyre, the sacred fire |
| Cloth | Dancers' silks and veils, movement |
| Blades | Zafir's knife, the Trap |
| Crowd | Court in the Throne Hall, the festival |
| UI sounds | MISSING: UI sound list (hover, confirm, back, typing) |
| New clue | A clue is added to the Rumor Journal (MISSING: sound choice) |

All SFX come from free libraries with CC0 or CC-BY licenses; credits are kept in `Docs/Licenses.md`.

### Music Needed
| Track | Where / when | Mood |
| :-- | :-- | :-- |
| Palace loop (oud, frame drum, drone) | Exploration in the palace | Tense, nocturnal |
| Romance loop (solo strings) | Zafir conversations, rooftop | Tender |
| Finale (choir, drums) | Pyre and epilogue | Epic, triumphant |

Must have: 1 music loop (the palace loop); should have: all 3. Royalty-free library first; AI music only if the tool's terms allow our use.

---

## 7. Schedule
> Updated 09.11.2026: the project now has **4 weeks in total (09.11.2026 – 06.12.2026)**. This replaces the GDD's Week 9–14 schedule (which ended 20.12.2026). The detailed phase plan is in `production_pipeline.md`.

| Milestone | What is done | Date |
| :-- | :-- | :-- |
| Prototype | Movement, camera, grey-box test scene, interaction prompt; prompt builder, validator and one LLM request; Chapter 1 with Zafir playable online and offline. Merve: code. Tuana: lore bible, Zafir's card, Chapter 1 scene card and fallback lines, conversation UI sketch | Sun 15.11.2026 |
| First playable | Scene Director FSM, relationship values, open palace grey-box with all rooms, day / night chapter flow, clues and Rumor Journal, guards with hiding spots and eavesdropping, nightly dance minigame; Chapter 1 complete (day, night, key scene) online and offline; token cost measured; first red-team run. Tuana: Vashir's card, Chapter 2–4 cards, clue lists and NPC trees, dance songs, portraits v1 | Sun 22.11.2026 |
| Content complete | Chapters 2–4 (clues, NPC trees, nights), cutscene system, prologue / pyre / epilogue, HUD and all menus, guards with hiding spots and eavesdropping, nightly dance for all 4 nights, memories if kept; first playtest with outside players | Sun 29.11.2026 |
| Feature freeze and polish | Art and audio pass, tuning, second playtest, red-team pass with 0 leaks | Thu 03.12.2026 |
| Final | Windows build, offline mode verified, credits, AI disclosure, final prompt log | Sat 05.12.2026 (06.12 buffer) |

---

## 8. Appendix: GDD Content Not Covered by the Template

### 8.1 In-Game AI Design
The LLM writes what the characters say, but our own C++ code decides what actually changes. The model only makes suggestions; a state machine and a validator check them.

| Part | Technique | Runs in |
| :-- | :-- | :-- |
| Zafir, Vashir and 2 court NPCs talk freely | LLM character with structured (JSON) output | Built-in local model (llama.cpp + Qwen3.5-4B) on the player's PC |
| Scene flow (nudge, end, fail) | Finite state machine (Scene Director) | Our C++ |
| Faces and gestures | Emotion state machine driven by the LLM's emotion tag | Our C++ + Animation Blueprint |
| Memories between scenes | LLM summary call after each scene | Built-in local model |
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

**Preventing spoilers:** before Chapter 4, Zafir's card only says he is hiding how he got to the palace; the contract with Vashir is not in the prompt at all. Each secret is added only in the chapter where it can be revealed.

**Failure and fallback:**

| Failure | Detection | What the player sees |
| :-- | :-- | :-- |
| Timeout (over 6 s in the game; 30 s in the editor while developing) or network error | Request timeout or error code | "Hesitates" animation, then a pre-written line for this beat and emotion; values unchanged |
| Broken JSON | Parser fails | 1 silent retry, then the fallback line |
| Off-topic input | `off_topic` flag + keyword list | In-character reply |
| Spoiler leak | Secret not in prompt + keyword check | Regenerate once, then fallback |
| Explicit or hateful content | Provider moderation + word list | Refuses in character; after 3 times the scene closes politely |
| Jailbreak | Pattern list; model told to stay in role | In-character confused reply |
| Impossible value change | Limit in C++ | Cut to the per-turn maximum |
| Built-in model missing, failed to start or still loading | `ULocalModelSubsystem` state (`/health`) | Pre-written fallback lines; Settings shows "AI: built-in model unavailable" |
| Scene goal never reached | Turn counter | Nudge, then forced close |

**What makes it believable:** distinct voices; characters have their own goals (ask back, refuse, bargain); they remember; portrait and gesture react within 1 s; hints instead of meters; replay value from Zafir's dodged answers.

### 8.2 AI Tool Plan (workflow)
| Task area | Tool | Use | Check | Cost |
| :-- | :-- | :-- | :-- | :-- |
| Coding (C++ and Blueprints) | Claude Code through MCP + VibeUE (course setup); GitHub Copilot in the IDE; Claude for design questions and errors | Boilerplate, HTTP and JSON code, Blueprint setup in the editor | Other member reviews every merge; Automation tests for validator, parser and limits; AI-assisted header comment | Copilot via GitHub Student Developer Pack; Claude free tier |
| Art and illustration | An image model | Portrait drafts, background concepts, GDD diagrams | Tuana checks palette and style, repaints; no artist names in prompts | Free tier or course credits |
| Audio and music | Royalty-free libraries first; AI music only if terms fit | 3 loops, ambience | Listened in scene, loop seams, license in `Docs/Licenses.md` | Free |
| Writing and dialogue | Claude | Brainstorming, fallback line variants, GDD draft | Tuana edits every line against the lore bible; lore bible and cards hand-written | Free tier |
| Testing | Own test harness (`Ashes.TestLLM`) + LLM judge | Red-team: 50 lines per character (off-topic, jailbreak, spoiler, rude) | Read every flagged reply + 20 random; target 0 spoiler leaks, 95% valid JSON | Free (built-in model) |
| In-game runtime | Built-in model shipped with the game: llama.cpp `llama-server` (MIT) + Qwen3.5-4B Q4_K_M (Apache 2.0); optional external endpoint (Ollama / cloud) as a setting | Replies, memory summaries | C++ validator; playtest logs reviewed weekly | Free: runs on the player's PC; download +2.8 GB; needs a GPU with ~6 GB for fast replies |

**Not made with AI:** the lore bible, character cards and key story lines (final line, nickname); the twist and its pacing; Scene Director logic; final look of the portraits; level design.

**First test (03.11.2026):** 5 player lines sent to a Zafir prompt in a chat session; all 5 were valid JSON, in character and leak-free. Lesson: 4 of 5 replies were teasing, so Zafir's card needs a line saying when he turns cold or angry; retest through the API with harder questions ("Did Vashir hire you?") and measure reply times.

### 8.3 Top Risks and What We Cut
| Risk | Early warning (4-week plan) | What we cut |
| :-- | :-- | :-- |
| LLM integration slower or less reliable than planned (built-in model too slow on target PCs) | End of week 1 / mid week 2: average reply over 4 s on the stronger PC, or under 90% valid JSON | Smaller model or shorter replies; stream the reply word by word; court NPCs and the pyre confession become pre-written |
| Writing takes longer (cards, fallback lines, cutscenes) | End of week 2: Chapter 2 cards not final | Fallback lines cut by half; cutscenes become still images with text |
| 3D art and animation take too long | End of week 2: palace still grey-box | Custom models; final portraits (placeholders stay) |

### 8.4 API Keys and Cost
No API key and no cost (decided 10.11.2026): the AI model ships inside the game and runs on the player's PC, so players download nothing else and nobody pays per token. The model and server files (~2.8 GB) are not in the repository; `Tools/GetLocalLLM.ps1` downloads the pinned versions (SHA-256 checked) and the packaged build ships them in `ThirdParty/LLM`. Licenses: llama.cpp MIT, Qwen3.5-4B Apache 2.0 (`Docs/Licenses.md`). If an optional external endpoint is ever used, its key is never in the repository, `Config/*.ini` or the build (environment variable `ASHES_LLM_API_KEY` or `Secrets/`, git-ignored). Turn limits still keep each scene short.

### 8.5 References
- **Games:** Façade (Procedural Arts, 2005); Disco Elysium (ZA/UM, 2019); Hades (Supergiant Games, 2020); AI Dungeon (Latitude, 2019).
- **Papers:** Mateas, M. and Stern, A. (2003). *Façade: An Experiment in Building a Fully-Realized Interactive Drama.* GDC. Park, J. S. et al. (2023). *Generative Agents: Interactive Simulacra of Human Behavior.* arXiv:2304.03442.
- **Course materials:** state machines, believability, LLM-powered characters, local models.
- **Tools:** Unreal Engine 5.8 documentation (Enhanced Input, UMG, AI Perception, Behavior Trees, Navigation, Sequencer, Quartz, MetaSounds, Niagara, HTTP module, Automation tests); LLM provider docs (structured output, rate limits, usage policy); asset licenses in `Docs/Licenses.md`.

---

## Section Status
| Section | Status |
| :-- | :-- |
| Header | FILLED |
| 1. Pitch | FILLED |
| 1. Characters | FILLED (MISSING: looks of Vashir, Nura, Idris) |
| 1. Gameplay, Core Loop, Main Mechanics | FILLED |
| 1. Win and Lose | FILLED |
| 1. Gameplay Walkthrough | FILLED |
| 1. Mindset | FILLED |
| 1. Uniqueness | FILLED |
| 1. Design Pillars | MISSING (suggested pillars given, need confirmation) |
| 1. Out of Scope | FILLED (MISSING: combat confirmation) |
| 2. Screens | FILLED (MISSING: lose screen, credits buttons, video/control settings) |
| 2. Controls | FILLED (dance keys proposed) |
| 2. Camera | FILLED (MISSING: distances and limits) |
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
| 7. Schedule | FILLED (updated to 4 weeks) |
| 8. Appendix | FILLED |
