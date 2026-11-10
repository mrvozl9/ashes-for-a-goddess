# Prompt Log

Our AI prompt log (GDD 8.2) is a Google Sheet that is shared with the instructor:

**[BurntPotatoGames_PromptLog](https://docs.google.com/spreadsheets/d/1D1uqRMgC_2E4n-PsOUjiOAxivMPjW9H0ComF00bLa40/edit?usp=sharing)**

The sheet is the only real log. This file only links to it and holds rows that still have to be copied into it.

## Rule: log the important prompts
Log only the prompts that mattered, not every message: one row per task or decision that produced code, text, art, assets or a design choice (for example a pipeline phase, a design decision, a document, an illustration). Put the main prompt word for word and add only the follow-ups that changed the result. Skip small talk, quick questions, status checks and setup help. Add the row at the end of the task, before the commit. Merve and Tuana both log their own sessions, including tools outside Claude (ChatGPT, Copilot, image models).

Columns (section "2. Prompt Log Entries"; one section row per course week, e.g. "WEEK 10: ..."):

| Column | What to write |
| :-- | :-- |
| # | Next number |
| Date | The day of the session |
| Week | Course week (Week 8 = 02.11–08.11.2026, Week 9 = 09.11–15.11, Week 10 = 16.11–22.11, Week 11 = 23.11–29.11, Week 12 = 30.11–06.12) |
| Who | Merve / Tuana / Team |
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
