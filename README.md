# Ashes for a Goddess

> *"Some gods brighten the sky when they are born; others, when they die."*

A third-person narrative RPG / visual novel hybrid by **Burnt Potato Games (BPG)**, built in **Unreal Engine 5.8** (C++ and Blueprints) for *Game Studio: AI in Game Development*.

An assassin disguised as a dancer sneaks into the palace of a king who burns villages to save a dying sun. You talk to the characters in your own words, and they remember what you said: a rival assassin who slowly falls for you, and a king who has been hunting you all along.

| | |
|---|---|
| **Genre** | Third-person narrative RPG + visual novel; dark fantasy romance |
| **Platform** | Windows PC, keyboard and mouse |
| **Engine** | Unreal Engine 5.8 (C++ + Blueprints, Lumen) |
| **Play time** | About 25–30 minutes (4 chapters, each a day and a night, plus cutscenes) |
| **Players** | Single player |
| **Rating target** | Teen |

## The game

- **Court of Whispers.** Each chapter is a day and a night in an open palace. By day, question dancers, guards, servants and priests and collect clues in the **Rumor Journal**; enough clues open the chapter's key conversation with Zafir or Vashir.
- **Free conversation.** Instead of picking from a list, type anything (up to 200 characters), or pick one of 3 suggested replies. Characters answer in character via an LLM with structured JSON output, remember what was said, and treat Sahra differently because of it.
- **Guards.** Patrols watch the halls. Running in view or entering restricted rooms raises Suspicion; hide behind curtains and lattice screens and eavesdrop on guard gossip for extra clues. At 100 Suspicion the chapter restarts from its checkpoint.
- **Nightly dance.** Every night Sahra performs in the court to keep her cover: a short rhythm minigame. A good dance wins Favor and a secret; a clumsy one draws Suspicion.
- **Fixed story, personal romance.** Main events and the ending are always the same; how the romance develops and what characters reveal depends on the player.
- **Characters stay in the world.** They only know a hand-written ~600-word lore bible, brush off off-topic questions in character, and can't reveal the twist early.
- **Offline mode.** With no API key or no internet, suggested replies become pre-written dialogue trees, so the whole game is still playable.

### Characters

| Character | Role |
|---|---|
| **Sahra** (player) | Sole survivor of the village Tamra; the desert's best assassin, disguised as a dancer |
| **Zafir** (LLM) | Charming rival assassin who calls her "Little Ember" |
| **Vashir** (LLM) | Priest-King of the Dunes, who feeds villages to the Sun Goddess |
| **Nura** (court, pre-written by default) | Head of the royal dancers |
| **Captain Idris** (court, pre-written by default) | Captain of the palace guard |

### Story structure

Prologue: Ash → 1. The Garden of Ash → 2. The Private Dance → 3. Rooftop of Smoke → 4. The Trap → 5. The Pyre → Epilogue: The Long Flame

### Controls

| Input | Action |
|---|---|
| W A S D | Move |
| Shift | Run (raises Suspicion near guards) |
| Mouse | Orbit camera |
| E | Talk / interact / hide |
| Type + Enter | Say anything |
| 1 / 2 / 3 | Pick a suggested reply |
| Tab | Rumor Journal (clues, conversation log, memories) |
| ← ↑ ↓ → | Dance keys (proposed) |
| Space | Advance cutscene text |
| Esc | Pause |

## Getting started

### Requirements
- **Unreal Engine 5.8** (Epic Games Launcher), the same version for every team member.
- **Visual Studio 2022** with the "Game development with C++" workload (or JetBrains Rider) to build the C++ module.
- **Git LFS**: install it once with `git lfs install` *before* cloning.
- **MCP + VibeUE** for AI-agent editor access (course setup).

### First open
1. `git lfs install`, then `git clone` this repo.
2. Right-click `Ashes.uproject` → **Generate Visual Studio project files**, then open `Ashes.sln` and build **Development Editor | Win64** (or just double-click `Ashes.uproject` and let it build).
3. The editor opens on `L_Test`. Press **Play** (PIE) to test.
4. Before you edit any `.uasset` or `.umap`, lock it: `git lfs lock Content/Ashes/...`. Unlock after your pull request is merged.

> **Status (10.11.2026):** the Unreal project is created in Phase 0 of `Docs/production_pipeline.md`. The earlier Unity 6.3 skeleton is kept in the Git tag `unity-skeleton`.

### LLM API key
The key is **never** committed, never put in `Config/*.ini` and never packaged. Set the environment variable `ASHES_LLM_API_KEY`, or put the key in `Secrets/llm_key.txt` (the `Secrets/` folder is ignored by `.gitignore`). Without a key, the game starts in offline mode.

## Project structure

```
Ashes.uproject
Source/Ashes/      C++: Core, Player, Dialogue, Gameplay, AI, Data, Tests
Content/Ashes/     Core, Player, Characters, Gameplay, AI, UI, Data, Input,
                   Maps (L_Test, L_Title, L_Palace), Cinematics, Art, Audio
Config/            Engine and project settings (never the API key)
Docs/              gdd.md, design.md, technical_design.md, level-design.md,
                   production_pipeline.md, PROGRESS.md, debug.md, Licenses.md,
                   PromptLog.md, workplan/
```

### Conventions
- C++: `A` actors, `U` objects / components / subsystems, `F` structs, `E` enums, `I` interfaces; one class per `.h` / `.cpp` pair.
- Assets: `BP_` Blueprints, `WBP_` widgets, `M_` / `MI_` materials, `SM_` / `SK_` meshes, `T_` textures, `L_` levels, `DT_` / `DA_` data, `IA_` / `IMC_` input, `NS_` Niagara (full list in `Docs/design.md` section 9).
- Every AI-assisted C++ file starts with: `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.` (a Comment node in Blueprints).
- Tick stays off unless something must run every frame.

### Branching
- `main` always opens and runs.
- Work on `feature/<name>-<task>` branches, open a pull request, the other member reviews, and merge only when `L_Palace` plays.
- `.uasset`, `.umap`, art, audio, video and fonts are stored with **Git LFS** (see `.gitattributes`) and are lockable.
- `L_Palace` uses One File Per Actor, so both members can change different actors in the same level.

## Planning
- Work plan (day by day, 09.11 – 06.12.2026): `Docs/workplan/index.html`
- Phases and tracker: `Docs/production_pipeline.md` (Phase N = GitHub issue #N+2)

## Team

| Member | Role |
|---|---|
| Merve Özal | Lead Programmer, Level Designer, Audio Designer |
| Tuana Tuna | Lead Artist, UI Designer, Narrative Designer, Programmer |

## AI disclosure
This project uses AI tools during development (see the GDD's AI Tool Plan and `Docs/PromptLog.md`). The in-game characters are powered by an LLM; all lore, character cards, scene cards and story are written by the team. This repository's initial setup and planning documents were drafted with Claude.
