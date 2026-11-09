# Ashes for a Goddess

> *"Some gods brighten the sky when they are born; others, when they die."*

A third-person narrative RPG / visual novel hybrid by **Burnt Potato Games (BPG)**, built in **Unity 6.3 LTS with HDRP** for *Game Studio: AI in Game Development*.

An assassin disguised as a dancer sneaks into the palace of a king who burns villages to save a dying sun. You talk to the characters in your own words, and they remember what you said: a rival assassin who slowly falls for you, and a king who has been hunting you all along.

| | |
|---|---|
| **Genre** | Third-person narrative RPG + visual novel; dark fantasy romance |
| **Platform** | Windows PC, keyboard and mouse |
| **Play time** | 12–15 minutes (4 free-talk chapters + exploration and cutscenes) |
| **Players** | Single player |
| **Rating target** | Teen |

## The game

- **Free conversation.** Instead of picking from a list, type anything (up to 200 characters) to Zafir, Vashir and the court, or pick one of 3 suggested replies. Characters answer in character via an LLM with structured JSON output, remember what was said, and treat Sahra differently because of it.
- **Fixed story, personal romance.** Main events and the ending are always the same; how the romance develops and what characters reveal depends on the player.
- **Characters stay in the world.** They only know a hand-written ~600-word lore bible, brush off off-topic questions in character, and can't reveal the twist early (secrets are only added to the prompt in the chapter where they can be revealed).
- **Stealth.** Explore the House of Embers palace while guards watch. If Suspicion reaches 100, the chapter restarts from its checkpoint.
- **Offline mode.** With no API key or no internet, suggested replies become pre-written dialogue trees, so the whole game is still playable.

### Characters

| Character | Role |
|---|---|
| **Sahra** (player) | Sole survivor of the village Tamra; the desert's best assassin, disguised as a dancer |
| **Zafir** (LLM) | Charming rival assassin who calls her "Little Ember" |
| **Vashir** (LLM) | Priest-King of the Dunes, who feeds villages to the Sun Goddess |
| **Nura** (LLM, court) | Head of the royal dancers |
| **Captain Idris** (LLM, court) | Captain of the palace guard |

### Story structure

Prologue: Ash → 1. The Garden of Ash → 2. The Private Dance → 3. Rooftop of Smoke → 4. The Trap → 5. The Pyre → Epilogue: The Long Flame

### Controls

| Input | Action |
|---|---|
| W A S D | Move |
| Shift | Run (raises Suspicion near guards) |
| Mouse | Orbit camera |
| E | Talk / interact |
| Type + Enter | Say anything |
| 1 / 2 / 3 | Pick a suggested reply |
| Tab | Conversation log and memories |
| Space | Advance cutscene text |
| Esc | Pause |

## Getting started

### Requirements
- **Unity 6.3 LTS (6000.3.27f2)**, the same version for every team member (set in `ProjectSettings/ProjectVersion.txt`). If you both move to another version, open the project with it and commit the updated file.
- **Git LFS**: install it once with `git lfs install` *before* cloning.

### First open
1. `git lfs install` then `git clone` this repo.
2. In Unity Hub: **Add → Add project from disk** and pick the repo folder. The first import takes a while (HDRP shaders).
3. Open **Window → Rendering → HDRP Wizard** and click **Fix All**. This creates the HDRP asset and default settings under `Assets/`. Commit those files.
4. Check **Edit → Project Settings → Player → Active Input Handling** is set to *Input System Package (New)*.
5. Create the `Boot` and `Palace` scenes in `Assets/_Project/Scenes/` and add them to **File → Build Profiles** (Week 8 milestone).

### Packages (Packages/manifest.json)
HDRP, Input System, Cinemachine, AI Navigation (NavMesh), Timeline, uGUI (includes TextMeshPro in Unity 6), Newtonsoft Json, Visual Effect Graph, Unity Test Framework. Unity may adjust versions to match your editor patch on first open; commit the updated `manifest.json` and `packages-lock.json`.

### LLM API key
The key is **never** committed. Read it from an environment variable or from a file outside `Assets/` (e.g. `Secrets/` or `.env`, both ignored by `.gitignore`). Without a key, the build starts in offline mode.

## Project structure

```
Assets/_Project/
  Art/      Models, Materials, Portraits, VFX
  Audio/    Music, SFX
  Data/     Characters, Scenes, Lore (ScriptableObjects and text)
  Prefabs/
  Scenes/   Boot, Title, Palace, Cutscenes
  Scripts/  Core, Player, Dialogue, AI, UI, Guards
  Tests/    EditMode
Docs/       gdd.md, design.md, technical_design.md, level-design.md,
            production_pipeline.md, PROGRESS.md, debug.md, Licenses.md, PromptLog.md
```

### Conventions
- PascalCase for classes and files, one class per file.
- Prefixes: `P_` prefabs, `M_` materials, `T_` textures, `SO_` ScriptableObjects.
- Every AI-assisted script starts with: `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.`
- Asset Serialization is **Force Text** and meta files are **visible**, so scenes and prefabs diff and merge.

### Branching
- `main` always opens and runs.
- Work on `feature/<name>-<task>` branches, open a pull request, the other member reviews, and merge only when the Palace scene runs.
- Art, audio, models, fonts and video are stored with **Git LFS** (see `.gitattributes`).

## Team

| Member | Role |
|---|---|
| Merve Özal | Lead Programmer, Level Designer, Audio Designer |
| Tuana Tuna | Lead Artist, UI Designer, Narrative Designer, Programmer |

## AI disclosure
This project uses AI tools during development (see the GDD's AI Tool Plan and `Docs/PromptLog.md`). The in-game characters are powered by an LLM; all lore, character cards, scene cards and story are written by the team. This repository's initial setup was scaffolded with Claude.
