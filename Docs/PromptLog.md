# Prompt Log

Our AI prompt log (GDD 8.2) is a Google Sheet that is shared with the instructor:

**[BurntPotatoGames_PromptLog](https://docs.google.com/spreadsheets/d/1D1uqRMgC_2E4n-PsOUjiOAxivMPjW9H0ComF00bLa40/edit?usp=sharing)**

The sheet is the only real log. This file only links to it and holds rows that still have to be copied into it.

## Rule: log the important prompts
Keep the log short: about once a day, at the end of the day's work, add one row for that day's big change (a big design decision, a new document or asset, or a new way of using AI), with its main prompt as sent. Skip days with no big change. Do not add a row for every phase or message; repeated work of the same kind (for example building the next phase) shares one row. Keep each cell to one or two sentences. Add the row at the end of the task, before the commit. Everyone on the team (Merve, Tuana, Tala, Baha) logs their own sessions, including tools outside Claude (ChatGPT, Copilot, image, music or voice tools); Merve keeps the sheet tidy.

Columns (section "2. Prompt Log Entries"; one section row per course week, e.g. "WEEK 10: ..."):

| Column | What to write |
| :-- | :-- |
| # | Next number |
| Date | The day of the session |
| Week | Advanced Game Programming course week (Week 8 = 02.11–08.11.2026, Week 9 = 09.11–15.11, Week 10 = 16.11–22.11, Week 11 = 23.11–29.11, Week 12 = 30.11–06.12) |
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
