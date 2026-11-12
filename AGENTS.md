# Ashes for a Goddess Codex instructions

## Shared team rules
Read `CLAUDE.md` and `SKILL.md` explicitly at the start of each session. They contain the shared team workflow; do not assume Claude's `@file` imports are expanded by Codex. Also read:
- `Docs/gdd.md`
- `Docs/design.md`
- `Docs/technical_design.md`
- `Docs/level-design.md`
- `Docs/production_pipeline.md`
- `Docs/PROGRESS.md`

Before Unreal work, explicitly read `Plugins/VibeUE/Content/samples/AGENTS.md.sample`. Keep this as a relative path; never generate an agent config containing another developer's absolute paths. User instructions take precedence over repository guidance.

## Team and scope
- Merve: levels, story, reviewed data, progress tracking; original Phase 0-5 code.
- Tuana: art, UI appearance, writing review.
- Tala: audio, VFX, dance content, sequences, playtests.
- Baha: gameplay C++/Blueprint logic, CSV import, fallbacks, Scene Director, offline trees, clues, guards, dance, cutscene player, spirit walk, packaging.
- Reply to Baha in Turkish unless asked otherwise. Keep shared code and documentation in the repository's established English style.
- Work on the user's requested task and preserve teammates' work. For gameplay implementation, follow the next open phase and its dependencies; setup/audit tasks do not authorize implementing later gameplay phases.

## Source and asset workflow
- Use a `feature/<name>-<task>` branch. Inspect `git status` first; never discard unrelated changes. Do not commit, push, merge or close issues unless requested.
- Unreal Engine 5.8, C++ module `Ashes`; the old Unity tag is not the current implementation.
- Build/test mechanics in `L_Test` before placing them in `L_Palace`.
- Never parse or edit `.uasset`/`.umap` as text. Use verified Unreal MCP/VibeUE tools; acquire Git LFS locks before asset edits.
- C++ owns testable rules; Blueprints own presentation/glue. Prefer delegates/timers over Tick.
- All LLM requests go through `ULLMProvider` and responses through `UResponseValidator`. The model suggests; code validates changes and story progression.
- Keep authored content in DataTables/Data Assets. Seven chapters; scene limits Zafir 11, Vashir 7, others 5. Do not treat old defaults or documentation checkboxes as evidence of implementation.
- Never stage generated build/cache folders, secrets, keys or `ThirdParty/LLM`.
- Mark AI-assisted source with tool/date/purpose and an honest pending reviewer; do not claim someone reviewed code without evidence.

## Codex Unreal MCP connection
- Project-scoped server: `ashes_unreal` in `.codex/config.toml`, endpoint `http://127.0.0.1:8001/mcp`.
- Start Ashes with `powershell -NoProfile -ExecutionPolicy Bypass -File Tools/Start-CodexEditor.ps1`. Port 8001 avoids this PC's occupied default port. Claude's `.mcp.json` remains unchanged at port 8000; teammates may continue their existing setup.
- Before editor operations, call `unreal.WorkflowService.get_environment()` through VibeUE and verify `projectFile` identifies this checkout's `Ashes.uproject`; inspect the current map. Never use another project's Unreal MCP server because its tools happen to be available.
- Read-only inspection must pass `auto_save=false` to `execute_python_code`; VibeUE otherwise saves dirty packages before execution.
- If the server is missing, verify editor process, port, project identity and log. Do not kill another editor or silently switch endpoints. A running listener or readiness file alone is not proof of a successful MCP tool call.
- `Tools/Test-UnrealMCP.py` performs a read-only initialize/tools-list/Python identity probe. It requires only Python's standard library.
- New project configuration is loaded when starting a Codex session rooted at this checkout. Do not claim newly configured tools are already callable in an older chat.

## Verification and handoff
- After C++ edits, perform a real build and appropriate `Ashes.` Automation tests; distinguish build, test, live-model latency and manual PIE verification.
- On Baha's PC a parallel XGE build hit Windows error 1455. The successful retry used `-NoXGE -NoUBA -MaxParallelActions=1`; use these command-line switches if memory pressure recurs, without changing system-wide settings.
- Do not mark a gameplay phase done until its verification gate is met; request the planned manual test before moving to the next phase.
- Record completed work, evidence, next steps and unresolved problems in `Docs/PROGRESS.md` and the pipeline tracker. Preserve existing session notes. Record significant AI workflow changes in the waiting table of `Docs/PromptLog.md` when the shared sheet cannot be edited.
