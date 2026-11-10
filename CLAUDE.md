# Ashes for a Goddess · Agent Front Door (CLAUDE.md)

> Claude reads this file automatically when it opens the project. For Codex, copy this file and name the copy `AGENTS.md`.
> Unreal Engine 5.8, C++ and Blueprints (module `Ashes`). Not Unity: the old Unity skeleton lives only in the Git tag `unity-skeleton`.

## Read first (every session)
- `SKILL.md` (how to work)
- `Docs/gdd.md` (what the game is)
- `Docs/design.md` (look, UI tokens, names)
- `Docs/technical_design.md` (how the code is built)
- `Docs/level-design.md` (where things are)
- `Docs/production_pipeline.md` (what to build next)
- `Docs/PROGRESS.md` (where we left off)

## Rules
- One pipeline step at a time.
- Follow the names in `Docs/design.md`.
- After a step works: update the tracker and `Docs/PROGRESS.md`, and tell me to commit.
- Never change things outside the current step.
- When you fill or update a document: fill it COMPLETELY. Never skip or shorten sections to save tokens.
- Never put an API key in the repo, `Config/*.ini` or the build; the game must run in offline mode without one.
- Game code reaches the LLM only through `ULLMProvider`; every reply passes `UResponseValidator`.
- Never write or edit `.uasset` / `.umap` files as text; use MCP + VibeUE in the editor, or give me editor steps.
- Lock binary assets with `git lfs lock` before editing; never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`.
- Keep Tick off unless something must run every frame; use delegates and timers.
- Every AI-assisted C++ file starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.` (a Comment node in Blueprints).
- Work on `feature/<name>-<task>` branches; `main` must always open and run.
