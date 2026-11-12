# Game data sources (text)

Text copies of the DataTable / Data Asset contents, so they can be reviewed in a pull request and re-imported. The `.uasset` files in `Content/Ashes/Data/` are the real game data; these JSON files are imported into them in the editor (DataTable → Reimport / VibeUE).

Source: the team's Master Lore doc (12.11.2026). The lore bible is Tuana's hand-written text. The character card, scene card, clues and fallback lines are Claude's drafts for the team to revise (marked AI-assisted until rewritten).

| File | Goes into |
|---|---|
| `Chapter1/DA_LoreBible.json` | `DA_LoreBible` → Text |
| `Chapter1/DT_Characters_Zafir.json` | `DT_Characters` row `Zafir` |
| `Chapter1/DT_Scenes_Ch1.json` | `DT_Scenes` row `Ch1_ZafirKeyScene` |
| `Chapter1/DT_Clues_Ch1.json` | `DT_Clues` (6 rows) |
| `Chapter1/DT_FallbackLines_Ch1.json` | `DT_FallbackLines` (7 rows) |
