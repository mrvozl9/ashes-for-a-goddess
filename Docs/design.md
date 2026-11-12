# Ashes for a Goddess · Design Tokens & UI/UX System (design.md)

> The single source of truth for every UI color, font, spacing and micro-interaction. Code and UI (UMG widgets in Unreal Engine 5.8) must use these token names. Status: **DRAFT**.
> **Aesthetic north stars:** Painted character portraits inside ornate gold frames (GDD 8, Figure 5); desert palaces at night with lanterns and firelight (mood board, GDD 8.1); visual novel conversation screens.
> **Guiding law:** "A moonlit palace at the end of the world": gold line work and firelight on deep plum shadows.

Source: `gdd.md` sections 5 (Graphics), 2 (Screens) and 8 (Appendix). Values marked `MISSING` are not in the GDD and must be decided before the UI phase in `production_pipeline.md` (Phase 15): visual tokens by **Tuana** (UI artist), sound tokens by **Tala** (audio). Updated 12.11.2026 for the 7-chapter story and the team of four (course: Advanced Game Programming).

**Names to use everywhere (12.11.2026):** the palace is **Sama'zar** (keep the apostrophe); the hub is the **Garden of Dusk** (Chapter 1's title is still open: "The Garden of Ash" or "The Garden of Dusk"); rooms: Dancers' Quarters (with the practice room), Throne Hall, King's Chamber (with the servants' passage), Guard Post (with the City Gate), Rooftop Terrace; the festival temple is the **Crimson Zenith**; Sahra's village is **Tamra**; her cover is the **Lantern House**; Zafir calls her **"Little Ember"**. Chapters: 1 The Garden of Ash, 2 The Practice Room, 3 The Private Dance, 4 The King's Favor, 5 Rooftop of Smoke, 6 The Trap, 7 The Pyre; Prologue: Ash; Epilogue: The Long Flame.

---

## 1. Color Tokens

### 1.1 Core Palette
| Token | Hex | Use |
| :-- | :-- | :-- |
| `Core_Primary` | `#C9A25B` | Old gold: UI frames, jewelry, button outlines, Zafir |
| `Core_Secondary` | `#6B1E2E` | Burgundy: Vashir's court, curtains, headings, panel fills |
| `Core_Background` | `#2A1426` | Night plum: backgrounds, night sky, UI panels |
| `Core_Text` | `#F1E3C8` | Moon cream: text on dark UI, moonlight |
| `Core_Accent` | `#E8642C` | Ember orange: fire, the phoenix, Sahra |

### 1.2 Character / Team Accents
| Token | Hex | Character | Use |
| :-- | :-- | :-- | :-- |
| `Char_Sahra` | `#E8642C` | Sahra | Sahra POV tag, her name plate |
| `Char_Zafir` | `#C9A25B` | Zafir | Zafir's name plate and portrait frame highlight |
| `Char_Vashir` | `#6B1E2E` | Vashir | Vashir's name plate and portrait frame fill |
| `Char_Nura` | MISSING: Nura's accent color | Nura | Name plate |
| `Char_Idris` | MISSING: Idris's accent color | Captain Idris | Name plate |
| `Char_Minor` | MISSING: one shared accent or one per character | Omar, Ilyas, Tariq, Aisha | Name plates |
| `Char_Ambient` | `#F1E3C8` at 70% (*proposed*) | Ambient NPCs, the king's servant | Bark bubble text (no name plate) |

### 1.3 Semantic Tokens
| Token | Value | Use |
| :-- | :-- | :-- |
| `Sem_Danger` | `#9E1B32` (Rose red) | Damage, warnings, Suspicion (eye icon fill), restricted King's Chamber |
| `Sem_Success` | MISSING: success color (the GDD has no success color; Old gold `#C9A25B` is the closest palette match) | Win, collected, beat met |
| `Sem_Disabled` | MISSING: disabled color | Locked buttons (for example Continue with no save) |
| `Sem_Hint` | `#F1E3C8`, italic | The one-line mood hint under a reply ("Zafir looks at you a moment too long.") |

## 2. Typography Tokens
| Token | Font / style | Use |
| :-- | :-- | :-- |
| `Type_Title` | MISSING: title font (the GDD document itself uses the serif Spectral; Figure 5 shows a classical serif, italic for goals) | Titles, menu headers, chapter cards |
| `Type_Body` | MISSING: body font (Figure 5 shows a serif) | Dialogue lines, log, settings |
| `Type_Button` | MISSING: button font | Reply buttons, menu buttons |
| Sizes | `T1 MISSING` / `T2 MISSING` / `Body MISSING` / `Caption MISSING` (pt at 1080p) | Max 3 sizes per screen |

## 3. Shape, Spacing & Material Tokens
| Token | Value | Note |
| :-- | :-- | :-- |
| `Space_Unit` | `8px` | All margins are multiples of this (template default) |
| `Panel_Padding` | MISSING: panel padding | Inside panels |
| `Button_MinSize` | MISSING: minimum button size | Reply and menu buttons |
| `Radius` | Ornate gold frames with decorative corners (GDD 8, Figure 5); MISSING: exact sprite / 9-slice spec | Corner style |
| `Shadow` | MISSING: shadow / glow spec | Figure 5 shows a soft dark glow behind panels |

## 4. Motion & Juice Tokens
| Token | Value | Use |
| :-- | :-- | :-- |
| `Motion_FocusBounce` | MISSING: focus animation | Focused button |
| `Motion_PanelIn` | MISSING: panel open animation | Panel opening |
| `Motion_Confirm` | MISSING: confirm animation | Confirm click |
| `Motion_TypingIndicator` | 0.4 s minimum | Shown before every reply so replies don't feel robotic |
| `Motion_TextTypeOut` | Text types out at the Settings "Text speed" (MISSING: default characters per second) | Reply text |
| `Motion_PortraitSwap` | Within 1 s of the reply | Portrait and gesture change to the emotion tag |
| `Motion_ChapterCard` | 3 s on screen | Chapter number, title and verse |
| `Motion_Hesitate` | "Hesitates" animation before a fallback line | Timeout or error |
| Rule | Every interaction = motion + sound | Nothing responds silently |

## 5. UI Sound Tokens
| Token | Sound | Use |
| :-- | :-- | :-- |
| `SND_UI_Hover` | MISSING: hover sound | Focus move |
| `SND_UI_Confirm` | MISSING: confirm sound | Select |
| `SND_UI_Back` | MISSING: back sound | Cancel / back |
| `SND_UI_Locked` | MISSING: locked sound | Disabled press |

## 6. Button & Focus State Spec
| State | Visual | Motion / Sound |
| :-- | :-- | :-- |
| Idle | Burgundy / plum fill, Old gold frame, Moon cream text, number badge on reply buttons (Figure 5) | None |
| Focused | MISSING: focused visual | `Motion_FocusBounce` + `SND_UI_Hover` (both MISSING) |
| Pressed | MISSING: pressed visual | `Motion_Confirm` + `SND_UI_Confirm` (both MISSING) |
| Disabled | `Sem_Disabled` (MISSING) | `SND_UI_Locked` (MISSING) |

## 7. Screen Inventory & Navigation Flow
```
[Boot] -> [Title] -> New Game -> [Prologue cutscene (2D)] -> [Chapter card] -> DAY [Exploration HUD] <-> [Conversation] / [Hiding] / [Bark]
                                                       -> NIGHT [Dance] -> KEY SCENE [Conversation] -> [In-engine ending] -> next [Chapter card]
                  -> Continue -> [Chapter card at last checkpoint]
                  -> [Settings]
                  -> Quit
[Exploration HUD / Conversation] -> Esc -> [Pause] -> Resume / [Settings] / Restart Chapter / Quit
[Exploration HUD / Conversation] -> Tab -> [Rumor Journal]
Suspicion 100 -> (Caught) -> [Chapter card at checkpoint]          (Chapters 1–5 only)
Chapter 6: [Dance (solo, fixed)] -> [Conversation with Vashir, 7 turns] -> [Trap sequence] -> [Chapter 7 card]
Chapter 7: [Spirit HUD: ember counter] -> 6 x ([Ember memory caption] -> [Confession piece]) -> [Epilogue cutscene (2D)] -> [Credits] -> [Title]
```
| # | Screen | Key elements |
| :-- | :-- | :-- |
| S1 | Main Menu (Title) | New Game, Continue, Settings, Quit; embers drifting over the dying sun |
| S2 | In-Game HUD | Exploration: goal line top left with the 2 "do" objectives under it, eye icon, "E: Talk" prompt, bark bubble; Chapter 7: ember counter only. Conversation: chapter + goal, "PLAYING AS SAHRA", "Turns left · Tab: log", dialogue panel with small portraits, hint line, Sahra POV portrait, large emotion portrait with emotion icons, 3 reply buttons, 200-character input with Send (Enter) |
| S3 | Pause | Resume, Settings, Restart Chapter, Quit |
| S4 | Game Over | MISSING: no lose screen in the GDD; being caught restarts the chapter from its checkpoint |
| S5 | Win | Epilogue cutscene, then Credits (team, licensed assets, AI-assisted assets and tools) |
| S6 | Settings | Text speed, volume, AI mode (Online / Offline), status line "AI: connected" / "AI: offline, pre-written dialogue" |
| S7 | Chapter card | Chapter number, title, one line of verse, 3 s |
| S8 | Rumor Journal (new 09.11) | Chapter question, found clues with speaker portrait, "?" slots for missing clues, full conversation log, each character's memories |
| S10 | Dance (new 10.11) | 4 arrow lanes, notes, hit feedback (Great / Good / Miss), accuracy result with reward or Suspicion warning; Throne Hall visible behind |
| S9 | Cutscene (visual novel) | 2D background, speaker, text; Space advances (Prologue and Epilogue only) |
| S11 | Ember memory (new 11.11) | Chapter 7: the player's own line from that chapter over a blurred glimpse of the scene, then Zafir's confession piece in `Char_Zafir` italics; no input except Space to continue |

## 8. HUD Spec
- **Goal line:** top left; italic one-line main quest (the chapter question), for example "Why does the other dancer carry a blade?"; shown under the chapter title in conversations.
- **"Do" objectives (new 11.11):** two smaller lines under the goal line, ticked with `Core_Primary` when done (for example "Learn the garden patrol and listen behind the lattice screen").
- **Eye icon:** fills only while a guard sees Sahra (`Sem_Danger`); empty otherwise; shows a closed eye while Sahra is in a hiding spot (new 10.11). MISSING: exact position.
- **Day / night tag (new 10.11):** "Day" or "Night" with the moon phase icon next to the goal line (*proposed*: full, waning half, dark, waxing half, almost full); at nightfall the goal becomes "Perform in the Throne Hall" (Chapter 3 and 6: "Dance for the king").
- **Bark bubble (new 11.11):** one line over an ambient NPC's head for 3 s (*proposed*), `Type_Body`, no portrait, no name plate.
- **Ember counter (new 11.11):** Chapter 7 only, top centre, six small ember icons that light up in `Core_Accent`; every other HUD element is hidden.
- **Hiding-spot glow and prompt (new 10.11):** soft `Core_Primary` glow on curtains and lattice screens; "E: Hide" / "E: Leave".
- **Eavesdrop indicator (new 10.11):** small ear icon with a fill bar while an exchange is being overheard.
- **"New clue" toast (new 09.11):** gold-framed note at the top right for 2 s.
- **"E: Talk" prompt:** world-space prompt over a highlighted character or object in range.
- **Turns left:** top right in conversations, "Turns left: 6 · Tab: log".
- **Mood hint:** italic `Sem_Hint` line under the latest reply; replaces any relationship meter. Numbers are never shown.
- **Large portrait:** right side, changes with the emotion tag within 1 s; emotion icons beneath it.
- **Reply input:** bottom; placeholder "Reply as Sahra to [name] · 200 characters"; warning when input is cut at 200 characters.
- **Suggested replies:** three framed buttons numbered 1 / 2 / 3 above the input.

## 9. Naming Conventions
> Unreal Engine 5.8 prefixes from the course template (Epic's usual style). The midterm GDD (13.1) listed Unity prefixes (`P_`, `SO_`); since the switch to Unreal on 10.11.2026 the Unreal prefixes below are used.

| Prefix | Asset type | Example |
| :-- | :-- | :-- |
| `BP_` | Blueprint actor | `BP_Sahra`, `BP_Zafir`, `BP_Guard`, `BP_HidingSpot` |
| `WBP_` | Widget Blueprint (UMG screen or element) | `WBP_Conversation`, `WBP_HUD`, `WBP_RumorJournal`, `WBP_Dance` |
| `GM_` / `GI_` / `PC_` | Game mode / game instance / player controller Blueprint | `GM_Ashes`, `GI_Ashes`, `PC_Ashes` |
| `ABP_` | Animation Blueprint | `ABP_Sahra` |
| `BT_` / `BB_` | Behavior Tree / Blackboard | `BT_Guard`, `BB_Guard` |
| `M_` / `MI_` | Material / material instance | `M_Sandstone`, `MI_Sandstone_Night` |
| `SM_` / `SK_` | Static mesh / skeletal mesh | `SM_Arch_Lattice`, `SK_Sahra` |
| `T_` | Texture | `T_Portrait_Zafir_Teasing` |
| `L_` | Level (map) | `L_Test`, `L_Title`, `L_Palace` |
| `LS_` | Level Sequence (cutscene) | `LS_Prologue`, `LS_Trap`, `LS_Epilogue` |
| `DT_` / `DA_` | DataTable / Data Asset | `DT_Characters`, `DT_Clues`, `DA_LoreBible` |
| `IA_` / `IMC_` | Enhanced Input action / mapping context | `IA_Interact`, `IMC_Exploration` |
| `NS_` | Niagara system | `NS_Embers`, `NS_RosePetals` |
| `S_` / `MS_` | Sound Wave / MetaSound | `S_UI_Confirm`, `MS_Dance_Night1` |
| `F_` | Font asset | `F_Title` (font itself MISSING, see section 2) |

Code: C++ classes use Unreal prefixes (`A`, `U`, `F`, `E`, `I`) and the project prefix `Ashes` where a name could clash (`AAshesCharacter`). One class per `.h` / `.cpp` pair, PascalCase.

## 10. Folder Structure
```
Ashes.uproject
Source/Ashes/      Core, Player, Dialogue, Gameplay, AI, UI, Data, Tests (C++)
Content/Ashes/
  Core/        GM_Ashes, GI_Ashes, PC_Ashes
  Player/      BP_Sahra, ABP_Sahra
  Characters/  BP_Zafir, BP_Vashir, BP_Nura, BP_Idris, BP_Omar, BP_Ilyas, BP_Tariq, BP_Aisha,
               BP_KingsServant, BP_AmbientNPC, BP_Guard
  Gameplay/    BP_HidingSpot, BP_EavesdropZone, BP_RestrictedVolume, BP_NPCSpawnPoint, BP_SoulEmber, BP_KnifeHidingPlace
  AI/          BT_Guard, BB_Guard
  UI/          WBP_* screens and elements, F_ fonts, T_UI_ textures
  Data/        DT_ DataTables, DA_ Data Assets
  Input/       IA_ actions, IMC_ mapping contexts
  Maps/        L_Test, L_Title, L_Palace
  Cinematics/  LS_Prologue, LS_Trap, LS_Epilogue
  Art/         Meshes, Materials, Textures, Portraits, VFX
  Audio/       Music, SFX, MetaSounds
Config/            Engine and project settings (never the API key)
Docs/              gdd.md, design.md, technical_design.md, level-design.md, production_pipeline.md, PROGRESS.md, debug.md, Licenses.md, PromptLog.md
```

## 11. Accessibility Rules
| Concern | Rule |
| :-- | :-- |
| Color-blind | MISSING: color-blind rule (note: Suspicion uses Rose red; pair it with the eye icon shape, not color alone) |
| Text scale | Text speed is adjustable in Settings; players who don't like typing can always pick 1 of 3 suggested replies. MISSING: text size option |
| Screen shake | MISSING: screen shake rule |
| Photosensitivity | MISSING: rule for fire, phoenix-rise and flare effects (note: the Chapter 7 red → gold change and the sky lightening must not flash) |
| Chapter 7 ghost look | The desaturated, blurred palace must keep the embers and the path readable (*proposed*: embers stay fully saturated and unblurred) |

## 12. Token Traceability
Every color, margin or motion value used in C++, Widget Blueprints or UMG styles carries a token comment, for example `// Token: Core_Primary` in C++ or a Comment node / variable tooltip `Token: Core_Primary` in a Widget Blueprint. New UI must not invent values that are not in this file. Add the token here first.

---

## Section Status
| Section | Status |
| :-- | :-- |
| Header / north stars / guiding law | FILLED |
| 1.1 Core Palette | FILLED |
| 1.2 Character Accents | FILLED (MISSING: Nura, Idris, minor characters) |
| 1.3 Semantic Tokens | FILLED (MISSING: success, disabled) |
| 2. Typography | MISSING |
| 3. Shape, Spacing | MISSING (except Space_Unit and frame style) |
| 4. Motion | FILLED for GDD timings (MISSING: focus, panel, confirm) |
| 5. UI Sounds | MISSING |
| 6. Button States | MISSING (except Idle) |
| 7. Screen Inventory | FILLED (12.11: Chapters 6–7 flow, ember memory screen; MISSING: lose screen) |
| 8. HUD Spec | FILLED (12.11: "do" objectives, bark bubble, ember counter) |
| 9. Naming | FILLED (Unreal prefixes since 10.11) |
| 10. Folder Structure | FILLED |
| 11. Accessibility | MISSING (except text speed and suggested replies) |
| 12. Traceability | FILLED |
