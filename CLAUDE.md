# Ashes for a Goddess · Agent Front Door (CLAUDE.md)

> Claude reads this file automatically when it opens the project. For Codex, copy this file and name the copy `AGENTS.md`.
> Unity 6.3 LTS (6000.3.27f2) + HDRP, C#. Not Unreal.

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
- Never put an API key in the repo or the build; the game must run in offline mode without one.
- Game code reaches the LLM only through `ILLMProvider`; every reply passes `ResponseValidator`.
- Don't hand-edit `.unity`, `.prefab`, `.asset` or `.meta` files; give me editor steps instead.
- Every AI-assisted script starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.`
- Work on `feature/<name>-<task>` branches; `main` must always open and run.
