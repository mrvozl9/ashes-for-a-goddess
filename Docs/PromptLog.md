# Prompt Log

Our AI prompt log (GDD 8.2) is for the course **Game Design Studio-III** (the rest of the project docs belong to Advanced Game Programming). It is a Google Sheet that is shared with the Game Design Studio-III instructor:

**[BurntPotatoGames_PromptLog](https://docs.google.com/spreadsheets/d/1D1uqRMgC_2E4n-PsOUjiOAxivMPjW9H0ComF00bLa40/edit?usp=sharing)**

The sheet is the only real log. This file only links to it and holds rows that still have to be copied into it.

## Rule: log the important prompts
Keep the log short: about once a day, at the end of the day's work, add one row for that day's big change (a big design decision, a new document or asset, or a new way of using AI), with its main prompt as sent. Skip days with no big change. Do not add a row for every phase or message; repeated work of the same kind (for example building the next phase) shares one row. Keep each cell to one or two sentences. Add the row at the end of the task, before the commit. Everyone on the team (Merve, Tuana, Tala, Baha) logs their own sessions, including tools outside Claude (ChatGPT, Copilot, image, music or voice tools); Merve keeps the sheet tidy.

Columns (section "2. Prompt Log Entries"; one section row per course week, e.g. "WEEK 10: ..."):

| Column | What to write |
| :-- | :-- |
| # | Next number |
| Date | The day of the session |
| Week | Game Design Studio-III course week (Week 8 = 02.11–08.11.2026, Week 9 = 09.11–15.11, Week 10 = 16.11–22.11, Week 11 = 23.11–29.11, Week 12 = 30.11–06.12) |
| Who | Merve / Tuana / Tala / Baha / Team |
| Tool | e.g. `Claude Opus 5.5`, `Claude Opus 5.5 (Claude Code + Unreal MCP + VibeUE)`, `ChatGPT` |
| Purpose | Short goal, e.g. "Phase 3: Rumor Journal" |
| Prompt (as sent, spelling corrected) | The prompt word for word, only spelling and grammar fixed; follow-ups as `[Follow-up] ...`; attachments as `[Attached: ...]` |
| Result (what the AI gave) | Files, code, assets or answers it produced |
| What we changed or checked | What we kept, changed or threw away; who tested or reviewed it |
| Notes | PR link, issue, anything else |

Never paste API keys, passwords or private data into the log.

## Rows waiting to be copied into the sheet
An AI session that cannot edit the sheet writes its rows here, in the same columns. The next person who opens the sheet copies them over and deletes them here.

| # | Date | Week | Who | Tool | Purpose | Prompt (as sent, spelling corrected) | Result (what the AI gave) | What we changed or checked | Notes |
| :-- | :-- | :-- | :-- | :-- | :-- | :-- | :-- | :-- | :-- |
| pending | 13.11.2026 | Week 9 | Merve | Claude Opus 5.5 (Claude Code) | Reply speed rule for the prototype gate | ok but 4 secons is a bit too low even for another strong pc right? am i wrong [Follow-up] okay we can change gdd and md files with that | Estimate of reply time per PC (laptop CPU ~20 s, laptop GPU ~4–6 s, gaming GPU 2–3 s); proposal: show the reply word by word and measure the time to the first words. | Rule changed in gdd.md, technical_design.md and production_pipeline.md to "first words within 4 s"; real measurement in the game window still to do. | Copy to the shared prompt log sheet. |
| pending | 12.11.2026 | Week 9 | Baha | Codex | Shared-team Codex and Unreal MCP setup | O zaman projeyi Codex ile çalışmaya uygun hale getir. AGENTS.md dosyasını hazırla ve Unreal MCP bağlantısını ayarlayıp kontrol et. | AGENTS.md, project-scoped MCP config, editor launcher and read-only connection probe; README and progress notes. | Codex CLI reads ashes_unreal; MCP tool execution confirmed Ashes.uproject, L_Test and VibeUE on port 8001. Gameplay source/assets unchanged. | Copy to the shared prompt log sheet; human review pending. |
| pending | 12.11.2026 | Week 9 | Baha | Claude Fable 5.2 (Claude Code) | Pre-Flight, subagents and CSV import for the DataTables | Hiçbir şeyi değiştirmeden önce Pre-Flight yap: dosyaları oku, git status ve son commit'leri kontrol et, Visual Studio/UE 5.8 kurulu mu bak, derleme komutunu dene. Sonra bana Türkçe kısa bir rapor ver. [Follow-up] Evet, başla (alt ajanları kur, CSV'den DataTable içe aktarma). | Pre-Flight report; four subagents in `.claude/agents/`; row structs for six new tables; `Ashes.Data.Import / Validate / ExportTemplates`; data report; CSV templates; 7 Automation tests. | Build succeeded and 15 of 15 Automation tests passed; import, reimport and duplicate-id refusal tried headless with test rows (removed afterwards). No real game data entered; Baha's code review pending. | Copy to the shared prompt log sheet. |
