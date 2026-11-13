# Ashes for a Goddess · Agent Front Door (CLAUDE.md)

> Claude reads this file automatically when it opens the project. For Codex, copy this file and name the copy `AGENTS.md`.
> Unreal Engine 5.8, C++ and Blueprints (module `Ashes`). Not Unity: the old Unity skeleton lives only in the Git tag `unity-skeleton`.
> Course: Advanced Game Programming. Team of four (12.11.2026): Merve Özal (level design, story and data), Tuana Tuna (art), Tala Alsayed (audio, VFX and other work), Ebrar Baha Özüpek (code from Phase 7 on). The story has 7 chapters; turn limits Zafir 11, Vashir 7, others 5.

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
- Prompt log (Google Sheet linked in `Docs/PromptLog.md`, for the course Game Design Studio-III, shared with its instructor): about once a day, add one short row for that day's big change (a big design decision, a new document or asset, a new way of using AI), not for every phase or message. Skip days with no big change. If you cannot edit the sheet, add the row to the waiting table in `Docs/PromptLog.md` and tell me to copy it.
- Never change things outside the current step.
- When you fill or update a document: fill it COMPLETELY. Never skip or shorten sections to save tokens.
- Never put an API key in the repo, `Config/*.ini` or the build; the game must run in offline mode without one.
- Game code reaches the LLM only through `ULLMProvider`; every reply passes `UResponseValidator`.
- Never write or edit `.uasset` / `.umap` files as text; use MCP + VibeUE in the editor, or give me editor steps.
- Lock binary assets with `git lfs lock` before editing; never commit `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`.
- Keep Tick off unless something must run every frame; use delegates and timers.
- Every AI-assisted C++ file starts with `// AI-assisted: <tool>, <date>, <what>. Reviewed by <name>.` (a Comment node in Blueprints).
- Work on `feature/<name>-<task>` branches; `main` must always open and run.

## Agent delegation
- The main agent plans the step, hands each part to the matching subagent in `.claude/agents/`, checks the result itself (build output, test result, diff) and updates `Docs/PROGRESS.md` and the tracker. One pipeline step at a time.
- `cpp-implementer` (sonnet): C++ in `Source/Ashes` plus build and Automation tests. `editor-operator` (sonnet): editor work through MCP + VibeUE only. `data-tools` (haiku): CSV files, data validation reports, documents. `reviewer` (opus): read-only review before a commit.
- No subagent commits, pushes or switches branches; only the user decides when to commit.

## Editor access (MCP + VibeUE)
- The editor serves MCP at `http://127.0.0.1:8000/mcp` (Unreal MCP plugin, auto-starts with the editor; `.mcp.json` points Claude Code at it). The editor must be open for MCP tools to work.
- VibeUE is a git submodule in `Plugins/VibeUE` (after cloning: `git submodule update --init`). Its agent guide is imported below with a relative path so it works on every team member's PC; do not replace it with `VibeUE.GenerateAgentConfig`, which writes an absolute path.

<!-- BEGIN VibeUE — relative import of the plugin's agent guide -->
@Plugins/VibeUE/Content/samples/AGENTS.md.sample
<!-- END VibeUE -->
