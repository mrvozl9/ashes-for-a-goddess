# Ashes for a Goddess
## Game Design Document (gdd.md)

> *"Some gods brighten the sky when they are born; others, when they die."*

**Author:** Burnt Potato Games (BPG): Merve Özal, Tuana Tuna · **Version:** 1.1 (Markdown version of the midterm GDD dated 07.11.2026, schedule updated for the 4-week plan) · **Date:** 09.11.2026
**Genre:** Third-person narrative RPG and visual novel hybrid; dark fantasy romance · **Platform:** Windows PC, keyboard and mouse (Unity standalone build) · **Engine:** Unity 6.3 LTS (6000.3.27f2), C#, 3D, HDRP · **Players:** Single player

| Field | Details |
| :-- | :-- |
| Course | Game Studio: AI in Game Development (midterm GDD in Week 8, final build in this 4-week plan) |
| Target player | Players aged 16+ who enjoy story-driven and romance games; visual novel fans curious about AI characters |
| Play time | 12–15 minutes per playthrough: 4 free-talk chapters of about 3 minutes each, plus short exploration and cutscenes |
| Rating target | Teen: violence and death are shown; the romance is not explicit |

| Member | Role | Owns |
| :-- | :-- | :-- |
| Merve Özal | Lead Programmer, Level Designer, Audio Designer | Player controller, camera, LLM client, prompt builder, validator, level, audio |
| Tuana Tuna | Lead Artist, UI Designer, Narrative Designer, Programmer | Lore bible, character and scene cards, fallback lines, portraits, UI screens, playtests |

> **Engine note.** The course templates assume Unreal Engine 5.8. This project uses **Unity 6.3 LTS with HDRP** (GDD 13.1, and the engine version both members agreed on 03.11.2026; the GDD text still says "Unity 6 LTS 6000.0.x"). Every document in `Docs/` uses Unity terms.

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
An assassin disguised as a dancer sneaks into the palace of a king who burns villages to save a dying sun. The player is Sahra, the sole survivor of a burned village, who explores the palace in third person while guards watch and talks to the characters in her own words. Instead of picking from a list, the player types anything (or picks one of 3 suggested replies), and the characters answer in character, remember what was said and treat Sahra differently because of it. The fun is a romance that feels personal: a rival assassin who slowly falls for you, and a king who has been hunting you all along. The main story and ending are fixed, but how the romance develops and what the characters reveal depends on the player.

### Characters

#### Sahra
- **Role:** Player character. Sole survivor of the village Tamra; the desert's best assassin, disguised as a dancer.
- **Look:** A dancer in silks and a veil with anklets (her disguise); long dark hair and red-and-gold jewelry in the conversation portrait. Linked to the Ember orange color (#E8642C).
- **Abilities:** Walk (2.2 m/s) and run (4.5 m/s) through the palace; talk to characters by typing up to 200 characters or picking 1 of 3 suggested replies; interact with highlighted characters and objects (E); dance for Vashir in Chapter 2 (timing minigame, should have). In the epilogue she is revealed to be the phoenix and walks unburned out of the sacred fire.
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
- **Role:** Court NPC (LLM character as a should-have; pre-written dialogue otherwise). Head of the royal dancers. Found in the Dancers' Quarters in Chapter 1.
- **Look:** MISSING: physical description of Nura.
- **Abilities:** Free conversation (4 turns per scene); 4–6 portrait emotions.
- **Personality / behaviour:** Gossipy, quick, practical. Wants to survive the court. Hides that she lost a sister to a burning.

#### Captain Idris
- **Role:** Court NPC (LLM character as a should-have; pre-written dialogue otherwise). Captain of the palace guard. Found at the Guard Post in Chapter 1.
- **Look:** MISSING: physical description of Captain Idris.
- **Abilities:** Free conversation (4 turns per scene); 4–6 portrait emotions.
- **Personality / behaviour:** Short, formal, suspicious. Wants order in the palace. Hides that he doubts the burnings.

#### Palace Guards
- **Role:** Enemies (should have). 2–4 guards on set patrol routes.
- **Look:** The same licensed base model, recolored.
- **Abilities:** View cone of 70° and 9 m; finite state machine Patrol → Notice → Investigate → Alert on a NavMesh; raise Suspicion when Sahra runs in view, stays in a restricted room or is seen with a weapon.
- **Personality / behaviour:** Predictable patrols; they do not talk.

### Gameplay
Second by second, the player walks Sahra through the palace (walk by default, Shift to run), keeping out of guard view cones. When a highlighted character shows "E: Talk", the player presses E and a conversation opens over the 3D scene. The player types a line (up to 200 characters) and presses Enter, or presses 1 / 2 / 3 to pick a suggested reply. After a typing indicator of at least 0.4 s, the character answers in 1–4 sentences, and the portrait and gesture change within 1 second. A one-line hint (for example "Zafir looks at you a moment too long.") shows how the character's opinion changed. Minute by minute, the player finds the right character for the chapter goal, pushes the conversation toward the hidden beats before the turns run out, and then moves to the next room as the next chapter opens.

#### Core Loop
Explore the palace (walk, sneak, find people) → talk freely (type or pick a reply) → the character reacts (mood, hidden values, memory) → is the scene goal met? If no, keep talking (the turn limit applies). If yes, a story beat plays (cutscene, chapter card) → the next chapter opens (new area, new goal) → explore again. If Suspicion reaches 100 at any point, the cover is blown and the chapter restarts from its checkpoint (GDD Figure 1).

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
Sahra explores the palace in third person. Guards on set patrol routes notice if she runs, stays in restricted rooms or is seen with a weapon, and Suspicion rises; it falls slowly when no guard is watching. Vashir can also raise Suspicion when Sahra answers carelessly. It is fun because it adds tension between conversations (guard patrols are a should have).

##### 6. Dance Minigame (should have)
In Chapter 2 Sahra dances for Vashir in a 30-second timing minigame with 4 keys and 0.25 s hit windows. Above 80% accuracy adds +10 Favor; below 40% adds +10 Suspicion. If it is cut, the dance becomes a cutscene and Favor comes from conversation only.

#### Win and Lose
- **Win:** Finish the final chapter. The ending is always the same, but the epilogue lines change with the relationship values and the memories the characters kept.
- **Lose:** Suspicion reaches 100, either from guards (running, going into restricted rooms) or from Vashir (asking careless questions). Running out of turns is not a game over: the scene still ends, but with a weaker outcome and lower relationship values.
- **What happens after losing:** The chapter restarts from its checkpoint (the start of each chapter).

#### Gameplay Walkthrough
1. **Title screen:** embers drift over the dying sun. The player picks New Game.
2. **Prologue: Ash (cutscene):** Tamra burns and a girl walks out of the smoke. Years later, Sahra arrives at the palace as a dancer. Space advances the text.
3. **Chapter card:** "Chapter 1 · The Garden of Ash" with one line of verse, for 3 seconds.
4. **Dancers' Quarters (start and checkpoint):** Sahra starts here. The goal line at the top left reads "Find out why the other dancer carries a blade." Nura is here; the player can talk to her (4 turns) for gossip about the court.
5. **Garden of Ash (hub):** the player walks into the moonlit garden with its dry fountain and lanterns. A guard patrols the garden; running in view raises Suspicion by 12 per second, and the eye icon fills while a guard sees Sahra. The Guard Post with Captain Idris (4 turns) is off the garden.
6. **Meeting Zafir:** the player finds Zafir in the garden and presses E. Zafir opens with an authored line ("Careful, Little Ember. Blades that small cut their owners first."). The player has 10 turns to learn why he is here and to agree to work together (beats such as `zafir_reveals_target` and `alliance_agreed`). Hints like "Zafir looks at you a moment too long." show the mood. If 2 turns are left, Zafir suggests the alliance himself.
7. **Resolve:** when all beats are met, a closing line plays; Sahra has stolen Zafir's knife. The memory summary writes 1–2 memories per character.
8. **Next chapter:** the game saves at the checkpoint and the Chapter 2 card plays; the Throne Hall and (by invitation) the King's Chamber open.

### Mindset
The player should feel playful tension in the conversations and a slowly growing tragedy in the story: the talks are teasing and flirtatious, but fire and ash are everywhere and the ending is a death. Stealth sections (guards, Vashir's probing questions) should feel tense, because one careless word or step raises Suspicion. Zafir's warmth growing over the chapters should feel earned and personal, because it comes from the player's own words. The Trap (Chapter 4) should feel like betrayal and loss, and the pyre and epilogue should turn grief into triumph when Sahra rises as the phoenix and the sun returns. On a second playthrough the player should feel the irony of Zafir's dodged answers, now knowing what he was hiding.

### Uniqueness
- **Free conversation inside a fixed story:** the player types anything and characters remember it, unlike choice-list romance games. Compared with **Façade** (Procedural Arts, 2005), which also uses typed conversation inside a fixed story.
- **Dialogue as the main gameplay:** compared with **Disco Elysium** (ZA/UM, 2019), dialogue with hidden checks; here the hidden values are moved by an LLM and clamped by our code.
- **Relationships through many short conversations:** compared with **Hades** (Supergiant Games, 2020).
- **Open AI storytelling with hard limits:** compared with **AI Dungeon** (Latitude, 2019), our characters only know a ~600-word hand-written lore bible, brush off off-topic questions in character and cannot reveal the twist early, because secrets are only added to the prompt in the chapter where they can be revealed.
- **Memory system** based on Park et al. (2023), *Generative Agents*.
- **Always playable:** an offline mode replaces the LLM with pre-written dialogue trees.

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
- 3D levels for the pyre and the festival temple: they are 2D illustrations.
- Any 3D level other than the palace (one hub with five rooms).
- Voice acting (text only); text-to-speech for Zafir is only a nice-to-have and is cut in the 4-week plan.
- Local model backend (offline AI through Ollama): nice-to-have, cut in the 4-week plan; offline mode uses pre-written dialogue.
- A proxy server for the API key: nice-to-have, cut in the 4-week plan.
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
- **Elements:** Exploration HUD: goal line at the top left; an eye icon that fills only while a guard sees Sahra; an "E: Talk" prompt over characters. Conversation screen (GDD Figure 5): chapter title and goal at the top left, "PLAYING AS SAHRA" tag, "Turns left: N · Tab: log" at the top right; dialogue panel with the speaker's line next to their small portrait and name; italic hint line under the line instead of a meter; Sahra's portrait below as the point of view; large character portrait on the right that changes with the emotion tag, with emotion icons; 3 numbered suggested-reply buttons (1 / 2 / 3); text field "Reply as Sahra to [name] · 200 characters" with a Send (Enter) button. Chapter card: chapter number, title and one line of verse, for 3 seconds. Log and memories (Tab): the full conversation log and each character's memories in their own words.

#### C. Pause Menu
- **Elements:** Resume, Settings, Restart Chapter, Quit.

#### D. Game Over / Lose Screen
- **Elements:** MISSING: a dedicated lose screen. The GDD says that when Suspicion reaches 100, guards seize Sahra ("Cover blown") and the chapter restarts from its checkpoint; the message and any buttons are not specified.

#### E. Win / Level Complete Screen
- **Elements:** Chapter complete: the closing line, then the next chapter card (no stats screen, the values stay hidden). Game complete: the Epilogue cutscene ("The Long Flame") ending with "Careful, Little Ember.", then the Credits screen with the team, licensed assets and a list of AI-assisted assets and tools. MISSING: buttons on the Credits screen.

#### F. Settings
- **Elements:** Text speed, volume, AI mode (Online / Offline) and a status line: "AI: connected" or "AI: offline, pre-written dialogue". MISSING: video and control settings.

### Controls
> Unity Input System (the Unreal template says Enhanced Input). Keyboard and mouse only.

| Action | Keyboard / Mouse | Gamepad | Input Type | Notes |
| :-- | :-- | :-- | :-- | :-- |
| Move | W A S D | Not supported (keyboard and mouse only) | Value (Vector2) | Walk by default, 2.2 m/s |
| Look / Camera | Mouse | Not supported | Value (Vector2) | Orbits the third-person camera |
| Jump | None | Not supported | None | The GDD has no jump |
| Interact | E | Not supported | Button | Talk to or interact with a highlighted character or object |
| Run | Shift (hold) | Not supported | Button | 4.5 m/s; raises Suspicion near guards |
| Say anything | Type + Enter | Not supported | Text input + Button | Max 200 characters; longer text is cut with a warning |
| Pick suggested reply | 1 / 2 / 3 | Not supported | Button | Picks one of the 3 suggested replies instead of typing |
| Conversation log | Tab | Not supported | Button | Opens the conversation log and memories |
| Advance cutscene | Space | Not supported | Button | Advances cutscene text |
| Pause | Esc | Not supported | Button | Opens the pause menu |
| Dance keys | MISSING: which 4 keys | Not supported | Button | Dance minigame, Chapter 2 (should have) |

### Camera
- **View:** Third person (Cinemachine).
- **Behaviour:** The mouse orbits the camera around Sahra. During conversations the 3D scene stays visible behind the conversation UI. MISSING: follow distance, height, orbit limits, collision behaviour and any special camera zones.

### Mechanics

#### 1. Free Conversation
- **Rule:** The player types a line or picks one of 3 suggestions; the prompt builder sends the character card, lore bible, scene card, state and the last 8 turns; the reply must be JSON and must pass the validator before the player sees it.
- **Numbers:** Max input 200 characters; turn limit 10 (Zafir), 8 (Vashir; 4 in Chapter 4), 4 (court NPCs); 3 suggested replies; reply 1–4 sentences, max 80 words; response timeout 6 s; typing indicator at least 0.4 s; portrait and gesture change within 1 s of the reply.
- **Edge cases:** Longer input is cut with a warning. Timeout or network error: "hesitates" animation, then a pre-written fallback line for this beat and emotion; values unchanged. Broken JSON: 1 silent retry, then fallback. Off-topic input: in-character reply ("A dancer who talks of 'phones'? The heat has got to you."). Spoiler leak: regenerate once, then fallback. Explicit or hateful content: the character refuses in character; after 3 times the scene closes politely. Jailbreak: in-character confused reply. No API key or no internet: offline mode with pre-written dialogue trees. (GDD Figure 2 says "5 s timeout"; the GDD text says 6 s, and 6 s is used here.)

#### 2. Relationship Values
- **Rule:** Each value is 0–100 and hidden. The LLM suggests a delta every turn; C# clamps it to the per-turn maximum and applies it.
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
- **Rule:** Guards see Sahra inside their view cone; running, staying in a restricted room or carrying a visible weapon raises Suspicion; it falls when no guard sees her.
- **Numbers:** Walk 2.2 m/s; run 4.5 m/s; guard view cone 70°, 9 m; running in view +12 per second; restricted room in view +20 per second; out of view −5 per second; checkpoint at the start of each chapter.
- **Edge cases:** Suspicion reaches 100: chapter restarts from its checkpoint. The King's Chamber is restricted until Vashir's Favor reaches 60. MISSING: Suspicion value for "seen with a weapon".

#### 6. Dance Minigame (should have)
- **Rule:** 30-second timing minigame with 4 keys in Chapter 2.
- **Numbers:** Hit window 0.25 s; accuracy above 80%: +10 Favor; below 40%: +10 Suspicion.
- **Edge cases:** If cut, the dance becomes a cutscene and Favor comes from conversation only. MISSING: result between 40% and 80% (no change is implied).

---

## 3. Level Design

### Level Design Philosophy
The game has one 3D level: the palace, built as a hub (the Garden of Ash) with five rooms around it, about 60 × 30 m. Rooms unlock chapter by chapter, so the open doors guide the player to the next goal. The player always passes through the garden, so its guard patrol is the main stealth challenge in every chapter. Lanterns and firelight pick out details against plum and burgundy shadows; restricted rooms (the King's Chamber, Rose red) raise Suspicion when Sahra is seen in them. The on-screen goal line states the chapter goal. MISSING: specific guidance techniques (light, color, sound or shape cues) for leading the player to characters and doors.

### Themes

#### Theme 1: Prologue: Ash (2D cutscene)
- **Mood:** Burning village at night; fire, smoke and ash.
- **Objects:** 2D background "Burning Tamra".
- **Challenges:** None (cutscene).
- **Goal:** Watch Tamra burn and Sahra arrive at the palace years later.

#### Theme 2: Chapter 1: The Garden of Ash
- **Mood:** Moonlit garden with a huge pale moon, lanterns, a dry fountain; night plum and old gold.
- **Objects:** Dancers' Quarters (start, checkpoint), Garden of Ash hub (dry fountain, lanterns), Guard Post (gate to the city, locked); Zafir's knife.
- **Challenges:** 2 guards, garden patrol; talking to Zafir (10 turns), Nura and Idris (4 turns each).
- **Goal:** Learn why Zafir is here and agree to work together.

#### Theme 3: Chapter 2: The Private Dance
- **Mood:** Vashir's court: burgundy curtains, gold, firelight.
- **Objects:** + Throne Hall (audience with Vashir, court NPCs, 1 guard), King's Chamber (by invitation).
- **Challenges:** 3 guards; Vashir's probing questions raise Suspicion (8 turns); dance minigame (should have).
- **Goal:** Get a private meeting with Vashir without raising Suspicion. Afterwards, Zafir's jealousy shows.

#### Theme 4: Chapter 3: Rooftop of Smoke
- **Mood:** Rooftop terrace on the upper level, smoke and moonlight; romance.
- **Objects:** + Rooftop Terrace, reached by stairs from the Dancers' Quarters.
- **Challenges:** Conversation with Zafir (10 turns).
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
2. Chapter 1: The Garden of Ash (tutorial for walking, talking and the garden patrol; Dancers' Quarters, Garden, Guard Post; 2 guards).
3. Chapter 2: The Private Dance (adds the Throne Hall and King's Chamber; Vashir's Suspicion; 3 guards).
4. Chapter 3: Rooftop of Smoke (adds the Rooftop Terrace; Affection decides the confession).
5. Chapter 4: The Trap (King's Chamber; only 4 turns; the twist).
6. Chapter 5: The Pyre (cutscene with generated confession).
7. Epilogue: The Long Flame (cutscene; epilogue lines depend on the values and memories) → Credits.

Difficulty grows through more guards (2 → 3), restricted rooms opening, Vashir's Suspicion entering in Chapter 2 and fewer turns in Chapter 4.

---

## 4. Development

### Abstract Classes
> Unity C# (the Unreal template asks for C++ / Blueprint base classes). Class names come from GDD Figure 6; parents are the usual Unity bases.

| Class | Parent | Purpose |
| :-- | :-- | :-- |
| `ILLMProvider` | C# interface | The only way game code reaches the LLM; implemented by Cloud, Local and Offline script providers |
| `GameManager` | MonoBehaviour | Chapters and checkpoints |
| `PlayerController` | MonoBehaviour | Movement and camera |
| `InteractionSystem` | MonoBehaviour | "E: Talk" starts a scene |
| `SceneDirector` | MonoBehaviour (FSM) | Intro, talk, nudge, resolve, forced close |
| `NPCBrain` | MonoBehaviour (one per character) | Profile, relationship values, memories |
| `PromptBuilder` | Plain C# class | Builds the prompt from card, lore, scene, state and turns |
| `ResponseValidator` | Plain C# class | JSON parse, lore, spoiler, safety checks and clamping |
| `GuardFSM` | MonoBehaviour | Patrol, notice, investigate, alert |
| `UIManager` | MonoBehaviour | Owns every screen |
| `ConversationUI` | MonoBehaviour | Text, input, 3 replies |
| `HUD` | MonoBehaviour | Goal, eye icon, prompts |
| `PortraitAnimator` | MonoBehaviour | Emotion to portrait |

### Derived Classes
> Unity prefabs and implementations made from the base classes.

| Blueprint | Derives from | What it adds |
| :-- | :-- | :-- |
| `CloudProvider` | `ILLMProvider` | Cloud LLM API over UnityWebRequest with async/await, JSON output, 6 s timeout |
| `OfflineScriptProvider` | `ILLMProvider` | Pre-written dialogue trees and fallback lines; offline mode |
| `LocalProvider` | `ILLMProvider` | Local model through Ollama (nice to have, cut in the 4-week plan) |
| `P_Zafir`, `P_Vashir`, `P_Nura`, `P_Idris` | `NPCBrain` on a character prefab | Character card, emotion set, values and memories per character |
| `P_Guard` | `GuardFSM` on the guard prefab | Patrol route, 70° / 9 m view cone, NavMesh agent |
| `P_Sahra` | `PlayerController` on the player prefab | Walk 2.2 m/s, run 4.5 m/s, Cinemachine orbit |

### Data
> ScriptableObjects and text in `Assets/_Project/Data/` (Characters, Scenes, Lore).

| Data Table | Rows (examples) | Fields |
| :-- | :-- | :-- |
| Character cards (`SO_` per character) | Zafir, Vashir, Nura, Idris, Sahra | Voice, wants, fears, speech rules, what they know, what they must not say, emotion states, start values, max change per turn, unlock thresholds |
| Scene cards (`SO_` per free-talk chapter) | Ch1 Garden of Ash, Ch2 Private Dance, Ch3 Rooftop of Smoke, Ch4 The Trap | Place, time, goal, beats and their rules, turn limit, characters, secrets added in this chapter, authored opener, closing lines |
| Lore bible (text) | One ~600-word text | Kingdom and dying sun, cult of the Sun Goddess, palace rooms and routines, festival, dancers, guard |
| Fallback lines | ~120 lines by scene, beat and emotion | Scene, beat, emotion, line, AI-assisted flag |
| Cutscene script | ~150 lines (Prologue, Ch4–5, Epilogue) | Part, speaker, line, background |
| Exploration settings | Walk 2.2, run 4.5, view cone 70° / 9 m, Suspicion rates | Speeds, cone angle and range, Suspicion per second per cause |

### AI
**Guards (should have):** finite state machine on a NavMesh: Patrol (set routes) → Notice → Investigate → Alert. They see Sahra inside a 70°, 9 m view cone. They raise Suspicion when she runs in view (+12/s), is in a restricted room in view (+20/s) or is seen with a weapon. The player escapes by leaving their view: Suspicion falls by 5 per second out of view. At 100 the chapter restarts. MISSING: hearing, Notice/Investigate timings and what Alert does before the restart.

**Conversation AI:** the LLM writes what characters say; our C# decides what changes. See Appendix 8.1.

---

## 5. Graphics

### Style Attributes
- **Art style:** Stylized 3D palace (a licensed modular desert or palace kit, lit in HDRP) and licensed character base models; conversations use painted character portraits inside ornate gold frames; prologue, pyre and temple are painted 2D backgrounds. "A moonlit palace at the end of the world."
- **Color palette:** Night plum `#2A1426` (backgrounds, night sky, UI panels); Burgundy `#6B1E2E` (Vashir's court, curtains, headings); Rose red `#9E1B32` (roses, danger, the King's Chamber); Old gold `#C9A25B` (UI frames, jewelry, Zafir); Ember orange `#E8642C` (fire, the phoenix, Sahra); Moon cream `#F1E3C8` (text on dark UI, moonlight).
- **References:** Mood board from the team's own photos and free or licensed images (no named artist's style copied): desert palaces at night (sandstone arches, lattice screens, a huge pale moon); lanterns and firelight on dark cloth; smoke and red rose petals in the air; dancers' silks and veils in motion; gold jewelry catching the light; burnt villages and ash fields for Tamra; a phoenix of fire for the ending.
- **Lighting:** Night: deep plum and burgundy shadows, moonlight, with gold and firelight (lanterns, fire) picking out the details.

### Graphics Needed

#### Characters
| Asset | Description | Animations |
| :-- | :-- | :-- |
| Sahra | Licensed base model, recolored; dancer disguise (veil, anklets) | Walk, run, idle, talk gestures, dance loop |
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
| Chapter card | Chapter number, title and one line of verse |

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
| First playable | Scene Director FSM, relationship values, palace grey-box with all rooms, Chapters 1–3 playable online and offline; token cost measured; first red-team run. Tuana: Vashir's card, Chapters 2–4 cards and fallback lines, portraits v1 | Sun 22.11.2026 |
| Content complete | Chapter 4, cutscene system, prologue / pyre / epilogue, HUD and all menus, memories; should-haves that fit (guards); first playtest with outside players | Sun 29.11.2026 |
| Feature freeze and polish | Art and audio pass, tuning, second playtest, red-team pass with 0 leaks | Thu 03.12.2026 |
| Final | Windows build, offline mode verified, credits, AI disclosure, final prompt log | Sat 05.12.2026 (06.12 buffer) |

---

## 8. Appendix: GDD Content Not Covered by the Template

### 8.1 In-Game AI Design
The LLM writes what the characters say, but our own C# code decides what actually changes. The model only makes suggestions; a state machine and a validator check them.

| Part | Technique | Runs in |
| :-- | :-- | :-- |
| Zafir, Vashir and 2 court NPCs talk freely | LLM character with structured (JSON) output | Cloud LLM API; local model as backup |
| Scene flow (nudge, end, fail) | Finite state machine (Scene Director) | Our C# |
| Faces and gestures | Emotion state machine driven by the LLM's emotion tag | Our C# + Animator |
| Memories between scenes | LLM summary call after each scene | Cloud LLM API |
| Palace guards (should have) | FSM + NavMesh: Patrol → Notice → Investigate → Alert | Our C# + Unity AI Navigation |

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
| Timeout (over 6 s) or network error | Request timeout or error code | "Hesitates" animation, then a pre-written line for this beat and emotion; values unchanged |
| Broken JSON | Parser fails | 1 silent retry, then the fallback line |
| Off-topic input | `off_topic` flag + keyword list | In-character reply |
| Spoiler leak | Secret not in prompt + keyword check | Regenerate once, then fallback |
| Explicit or hateful content | Provider moderation + word list | Refuses in character; after 3 times the scene closes politely |
| Jailbreak | Pattern list; model told to stay in role | In-character confused reply |
| Impossible value change | Limit in C# | Cut to the per-turn maximum |
| No API key or no internet | Check at startup | Offline mode with pre-written dialogue trees |
| Scene goal never reached | Turn counter | Nudge, then forced close |

**What makes it believable:** distinct voices; characters have their own goals (ask back, refuse, bargain); they remember; portrait and gesture react within 1 s; hints instead of meters; replay value from Zafir's dodged answers.

### 8.2 AI Tool Plan (workflow)
| Task area | Tool | Use | Check | Cost |
| :-- | :-- | :-- | :-- | :-- |
| Coding (C#) | GitHub Copilot; Claude for design questions and errors | Boilerplate, UnityWebRequest and JSON code, editor tools | Other member reviews every merge; Edit Mode tests for validator, parser and limits; AI-assisted header comment | Copilot via GitHub Student Developer Pack; Claude free tier |
| Art and illustration | An image model or Unity's built-in generators | Portrait drafts, background concepts, GDD diagrams | Tuana checks palette and style, repaints; no artist names in prompts | Free tier or course credits |
| Audio and music | Royalty-free libraries first; AI music only if terms fit | 3 loops, ambience | Listened in scene, loop seams, license in `Docs/Licenses.md` | Free |
| Writing and dialogue | Claude | Brainstorming, fallback line variants, GDD draft | Tuana edits every line against the lore bible; lore bible and cards hand-written | Free tier |
| Testing | Own test harness + LLM judge | Red-team: 50 lines per character (off-topic, jailbreak, spoiler, rude) | Read every flagged reply + 20 random; target 0 spoiler leaks, 95% valid JSON | Same API key, small cost |
| In-game runtime | Fast low-cost hosted model (for example Claude Haiku 5.5); Ollama local model nice-to-have | Replies, memory summaries | C# validator; playtest logs reviewed weekly | Pay per token; spending cap; tokens per playthrough measured in week 2 |

**Not made with AI:** the lore bible, character cards and key story lines (final line, nickname); the twist and its pacing; Scene Director logic; final look of the portraits; level design.

**First test (03.11.2026):** 5 player lines sent to a Zafir prompt in a chat session; all 5 were valid JSON, in character and leak-free. Lesson: 4 of 5 replies were teasing, so Zafir's card needs a line saying when he turns cold or angry; retest through the API with harder questions ("Did Vashir hire you?") and measure reply times.

### 8.3 Top Risks and What We Cut
| Risk | Early warning (4-week plan) | What we cut |
| :-- | :-- | :-- |
| LLM integration slower or less reliable than planned | End of week 1 / mid week 2: average reply over 4 s, or under 90% valid JSON | Court NPCs and the pyre confession become pre-written; offline mode becomes the default |
| Writing takes longer (cards, fallback lines, cutscenes) | End of week 2: Chapter 2 cards not final | Fallback lines cut by half; cutscenes become still images with text |
| 3D art and animation take too long | End of week 2: palace still grey-box | Custom models; final portraits (placeholders stay) |

### 8.4 API Keys and Cost
The key is never in the repository; it is read from an environment variable or a file outside `Assets/` that `.gitignore` excludes. A spending cap is set in the provider console. Turn limits keep each playthrough to a fixed number of calls. The submitted build starts in offline mode unless a key is present, so it always runs.

### 8.5 References
- **Games:** Façade (Procedural Arts, 2005); Disco Elysium (ZA/UM, 2019); Hades (Supergiant Games, 2020); AI Dungeon (Latitude, 2019).
- **Papers:** Mateas, M. and Stern, A. (2003). *Façade: An Experiment in Building a Fully-Realized Interactive Drama.* GDC. Park, J. S. et al. (2023). *Generative Agents: Interactive Simulacra of Human Behavior.* arXiv:2304.03442.
- **Course materials:** state machines, believability, LLM-powered characters, local models.
- **Tools:** Unity 6 manual (Input System, Cinemachine, AI Navigation, Timeline, TextMeshPro, HDRP); LLM provider docs (structured output, rate limits, usage policy); asset licenses in `Docs/Licenses.md`.

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
| 2. Controls | FILLED (MISSING: dance keys) |
| 2. Camera | FILLED (MISSING: distances and limits) |
| 2. Mechanics | FILLED (MISSING: memory failure, weapon Suspicion, dance mid-range) |
| 3. Level Design Philosophy | FILLED (MISSING: guidance cues) |
| 3. Themes | FILLED |
| 3. Game Flow | FILLED |
| 4. Abstract / Derived Classes | FILLED |
| 4. Data | FILLED |
| 4. AI | FILLED (MISSING: hearing, state timings) |
| 5. Style Attributes | FILLED |
| 5. Graphics Needed | FILLED (MISSING: guard animation list, smoke/petal VFX) |
| 6. Style Attributes | FILLED |
| 6. Sounds Needed | FILLED (MISSING: UI sounds) |
| 6. Music Needed | FILLED |
| 7. Schedule | FILLED (updated to 4 weeks) |
| 8. Appendix | FILLED |
