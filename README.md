# Ashes for a Goddess

> *"Some gods brighten the sky when they are born; others, when they die."*

A third-person narrative RPG / visual novel hybrid by **Burnt Potato Games (BPG)**, built in **Unreal Engine 5.8** (C++ and Blueprints) for the course *Game Design Studio-III*.

An assassin disguised as a dancer sneaks into Sama'zar, the palace of a king who burns a village every year to save a dying sun. You explore the palace, slip past guards, dance every night to keep your cover, and talk to the characters in your own words. They remember what you said: a rival assassin who slowly falls for you, and a king who has been hunting you all along.

| | |
|---|---|
| **Genre** | Third-person narrative RPG + visual novel; dark fantasy romance |
| **Platform** | Windows PC, keyboard and mouse |
| **Engine** | Unreal Engine 5.8 (C++ + Blueprints, Lumen) |
| **Play time** | About 45–60 minutes (*proposed*): 6 palace chapters, each a day and a night, the Chapter 7 spirit walk and a 2D Prologue and Epilogue |
| **Players** | Single player |
| **Rating target** | Teen |

## The game

- **Court of Whispers.** Each chapter is a day and a night in an open palace. By day, follow the chapter's question and two "do" objectives, find dancers, guards, servants and priests where the chapter puts them, and collect clues in the **Rumor Journal** (6 per chapter); any 3 open the chapter's key conversation with Zafir or Vashir.
- **A living palace.** About 10 courtiers, servants and guards give one line each that changes every chapter; the 8 real characters hold full conversations. The palace changes with the month: moon, banners, plum blossoms, more guards.
- **Free conversation.** Instead of picking from a list, type anything (up to 200 characters), or pick one of 3 suggested replies. Characters answer in character via an LLM with structured JSON output, remember what was said, and treat Sahra differently because of it.
- **Guards.** Patrols watch the halls. Running in view or entering restricted rooms raises Suspicion; hide behind curtains and lattice screens and eavesdrop on guard gossip for extra clues. Tail Zafir through the dark palace (Chapter 4) and shake off the king's servant who follows you (Chapter 5). At 100 Suspicion the chapter restarts from its checkpoint.
- **Nightly dance.** Every night Sahra performs to keep her cover: a short rhythm minigame (30 s on the first night, 45 s on the last). A good dance wins Favor and a secret; a clumsy one draws Suspicion.
- **The spirit chapter.** After the trap in Chapter 6, Sahra walks the empty, ghostly palace as a spirit and gathers six embers of her soul, each replaying a line the player really said, while Zafir's confession reaches her from the fire.
- **Fixed story, personal romance.** Main events and the ending are always the same; how the romance develops and what characters reveal depends on the player.
- **Characters stay in the world.** They only know a hand-written ~600-word lore bible, brush off off-topic questions in character, and can't reveal the twist early.
- **Built-in AI, always playable.** The AI model ships inside the game and runs on the player's PC: no account, no API key, no internet. If it fails or is too slow, pre-written offline dialogue trees and 142 fallback lines answer instead, so the whole game is still playable.

### Characters

| Character | Role |
|---|---|
| **Sahra** (player) | Sole survivor of the village Tamra; the desert's best assassin, disguised as a dancer |
| **Zafir** (LLM, 11 turns) | A dancer in the royal troupe, secretly a blade for hire; rival, ally and romance; calls her "Little Ember" |
| **Vashir** (LLM, 7 turns) | Priest-King of the Dunes, who feeds villages to the Sun Goddess |
| **Nura** (LLM, 5 turns) | Head of the royal dancers |
| **Captain Idris** (LLM, 5 turns) | Captain of the palace guard |
| **Omar, Ilyas, Tariq, Aisha** (LLM, 5 turns) | A palace servant, a temple priest, an off-duty guard and a second dancer, each holding clues |
| **The king's servant** (fixed lines) | Takes Sahra's cord and follows her |
| **Ambient NPCs** (one line) | About 10 courtiers, servants, guards, dancers and priests |

### Story structure

Prologue: Ash (2D) → 1. The Garden of Dusk → 2. The Practice Room → 3. The Private Dance → 4. The King's Favor → 5. Rooftop of Smoke → 6. The Trap → 7. The Pyre (spirit walk) → Epilogue: The Long Flame (2D)

The story spans one month, full moon to full moon, before the Festival of the Long Flame. Chapters 1–7 are played in 3D in the same palace; the main story and the ending are fixed, and the romance or comrade route changes the lines.

### Controls

| Input | Action |
|---|---|
| W A S D | Move |
| Shift | Run (raises Suspicion near guards) |
| Mouse | Orbit camera |
| E | Talk / interact / hide / pick up an ember |
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
- **MCP + VibeUE** for AI-agent editor access (git submodule `Plugins/VibeUE`: run `git submodule update --init` after cloning).

### First open
1. `git lfs install`, then `git clone` this repo and run `git submodule update --init`.
2. Right-click `Ashes.uproject` → **Generate Visual Studio project files**, then open `Ashes.sln` and build **Development Editor | Win64** (or just double-click `Ashes.uproject` and let it build).
3. Run `Tools/GetLocalLLM.ps1` once to download the built-in AI (llama-server + Qwen3.5-4B, about 2.7 GB, into the git-ignored `ThirdParty/LLM`). On a PC with 4 GB of VRAM, set the user environment variable `ASHES_LLM_DEVICE=none` to run the AI on the CPU.
4. The editor opens on `L_Test`. Press **Play** (PIE) to test.
5. Before you edit any `.uasset` or `.umap`, lock it: `git lfs lock Content/Ashes/...`. Unlock after your pull request is merged.

> **Status (12.11.2026):** Phases 0–5 are built (movement, interaction, built-in AI, prompt builder, validator, conversation screen); Phase 6 (prototype gate) is next. See `Docs/PROGRESS.md`. The earlier Unity 6.3 skeleton is kept in the Git tag `unity-skeleton`.

### AI model and keys
The game needs **no API key**: the AI model is built in (decided 10.11.2026) and starts hidden on `127.0.0.1` with the game. An optional external OpenAI-compatible endpoint can be set in Project Settings > Game > Ashes AI; if it needs a key, the key is **never** committed, never put in `Config/*.ini` and never packaged (environment variable `ASHES_LLM_API_KEY` or `Secrets/llm_key.txt`, git-ignored). Without the model, the game runs in offline mode.

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
Tools/             GetLocalLLM.ps1 (downloads the built-in AI)
Plugins/VibeUE/    editor access for the AI agent (git submodule)
```

### Conventions
- C++: `A` actors, `U` objects / components / subsystems, `F` structs, `E` enums, `I` interfaces; one class per `.h` / `.cpp` pair.
- Assets: `BP_` Blueprints, `WBP_` widgets, `M_` / `MI_` materials, `SM_` / `SK_` meshes, `T_` textures, `L_` levels, `LS_` Level Sequences, `DT_` / `DA_` data, `IA_` / `IMC_` input, `NS_` Niagara (full list in `Docs/design.md` section 9).
- Every AI-assisted C++ file starts with: `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.` (a Comment node in Blueprints).
- Tick stays off unless something must run every frame.

### Branching
- `main` always opens and runs.
- Work on `feature/<name>-<task>` branches, open a pull request, another team member reviews, and merge only when `L_Palace` plays.
- `.uasset`, `.umap`, art, audio, video and fonts are stored with **Git LFS** (see `.gitattributes`) and are lockable.
- `L_Palace` uses One File Per Actor, so several people can change different actors in the same level.

## Planning
- Work plan (day by day, 09.11 – 06.12.2026): `Docs/workplan/index.html`
- Phases and tracker: `Docs/production_pipeline.md` (Phase N = GitHub issue #N+2)
- Task board: GitHub Projects (kept by Merve)
- Story, cards, clues, barks and lines: the team's Master Lore document (single source for all game text)

## Team

| Member | Role |
|---|---|
| Merve Özal | Team Lead, Level Designer, Narrative Lead (wrote the Phase 0–5 code) |
| Tuana Tuna | Lead Artist, UI Artist, Narrative Designer (reviews the writing) |
| Tala Alsayed | Audio and VFX Designer (music, dance songs and charts, SFX, Niagara, cutscene sequences, credits, playtests) |
| Ebrar Baha Özüpek | Lead Programmer and Gameplay Systems (gameplay code from Phase 7 on, packaging) |

Tala and Baha joined on 12.11.2026.

## AI disclosure
This project uses AI tools during development (see the GDD's AI Tool Plan and `Docs/PromptLog.md`). The in-game characters are powered by a built-in LLM (Qwen3.5-4B via llama.cpp). The story, twist, lore bible and key lines are the team's own; game-data lines (cards, clues, barks, fallback lines, offline trees) were drafted with Claude from the team's story and are reviewed and rewritten by the team. This repository's setup, planning documents and parts of the C++ code were drafted with Claude and reviewed by the team (see the AI-assisted headers).

## Working with Codex

Claude and Codex use the same gameplay rules and branch/LFS workflow. Codex reads `AGENTS.md`, which explicitly points to `CLAUDE.md`, `SKILL.md`, the project design documents and VibeUE's relative agent guide. Keep shared decisions in those documents rather than in private chat history.

Open this checkout as the Codex project/workspace; project settings in `.codex/config.toml` are scoped to this repository and require a trusted project. On Baha's PC the checkout is `C:\Ashes`; its trust entry was added during setup. Do not copy another developer's global Codex configuration.

Start the editor from the repository root:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File Tools/Start-CodexEditor.ps1
```

The launcher uses UE 5.8 at the standard Epic Games installation path; pass `-EngineRoot` if installed elsewhere. It opens this checkout with MCP on **127.0.0.1:8001** and never closes an existing editor. If Ashes is already open on another port, save your work and close that Ashes editor normally before relaunching. Other projects and Claude's existing `.mcp.json` (port 8000) are unchanged.

After the map loads, check the endpoint with Python 3:

```powershell
python Tools/Test-UnrealMCP.py
codex mcp get ashes_unreal --json
```

The connection probe performs MCP initialization, lists tools, and invokes VibeUE Python with `auto_save=false`. It verifies the live project path matches this checkout's `Ashes.uproject`, reports the map, and checks WorkflowService. It does not edit or save assets. A readiness JSON or open TCP port is not enough to prove tool execution works.

Start a new Codex chat/session rooted at this repository after adding the configuration. This does not dynamically add tools to an already running chat rooted in another folder. In the CLI, `codex -C C:\Ashes` selects Baha's checkout. In the desktop app, open/select `C:\Ashes` as the project and start the chat there. Verify project identity before any editor mutation; do not use another project's MCP integration.

Setup verified on 12 November 2026: `ashes_unreal` enabled at `http://127.0.0.1:8001/mcp`, 11 MCP tools advertised, live project `C:/Ashes/Ashes.uproject`, map `/Game/Ashes/Maps/L_Test.L_Test`, VibeUE WorkflowService available. This connection check does not replace manual PIE or live-model performance testing.
