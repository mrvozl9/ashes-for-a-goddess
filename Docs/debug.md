# Debug Log (debug.md) · Burnt Potato Games (Merve Özal, Tuana Tuna, Tala, Baha)

> One block per bug. Newest on top. Write the member's name in each block.
> Unreal Engine 5.8 project since 10.11.2026. Course: Advanced Game Programming. Anyone on the team who finds a bug adds a block (Baha owns the C++ fixes from Phase 7 on).

## Bug 4: two or three Zafir lines per player line, real reply never arrives
- **Found by:** Merve (play test of the timeout change, 12.11.2026)
- **When I:** talked to Zafir in PIE after the soft / hard limit change.
- **I expected:** one answer per line.
- **But:** after each line Zafir posted an evasive line at 6 s and another at the hard limit; no real reply; turns stayed at 11.
- **Error from the Output Log:** `LLM request failed after 8.0 s: timeout or connection failed` (during the test the hard limit was set to 8 s by Claude's own forced-timeout test, which ran at the same time).
- **My prompt:** (play test screenshot, relayed by the project assistant)
- **The fix:** the soft limit only changes the status text ("Zafir hesitates..."); exactly one Zafir line per player line (real reply, or one evasive line after the hard limit / two invalid replies); a per-line request serial drops late replies. Editor hard limit `EditorHardTimeoutSeconds` = 60 s. Requests send `cache_prompt`; the log shows prompt / cached tokens and the limits used. Unreal's HTTP activity timeout (30 s without bytes) now uses the hard limit, so the cold warm-up (~30 s on the laptop CPU) is no longer cut.
- **Did it work?** Yes: 3 turns, one line each, "Zafir hesitates..." at 6 s, real replies in 22.1 / 25.3 / 29.3 s, prompt ~1,900 tokens with ~1,860–1,910 from the cache (read in 0.9–1.7 s), both beats in order; cold warm-up 30.2 s without error; 13 tests green.

## Bug 3: Zafir's gesture says "her lips"
- **Found by:** Merve (play test, 12.11.2026)
- **When I:** talked to Zafir in PIE.
- **I expected:** gestures about Zafir with he / his.
- **But:** "(tilts head, a smirk playing on her lips)"; the reply also showed markdown asterisks ("*you*").
- **Error from the Output Log:** none (wrong text, not an error).
- **My prompt:** "zafir is a he, but in here it says her" (with a screenshot)
- **The fix:** new `Pronouns` field on `FCharacterCardRow` (Zafir: he/him, in `Data/Chapter1/DT_Characters_Zafir.json` and `DT_Characters`); the prompt says "You are Zafir (he/him)" and asks for gestures about what Zafir himself does ("tilts his head, smiling"), never Sahra; the validator removes `*` and `_` from reply and gesture. Commits `[phase 4]: pronouns in gestures, strip * and _ from replies`.
- **Did it work?** Yes: "tilts his head, eyes narrowing at her boots", "tightens his grip on the hilt of his knife"; no asterisks; 10 Automation tests green.

## Bug 2: first AI request after a cold start fails with HTTP 500 ("No user query found in messages")
- **Found by:** Claude Code while testing `Ashes.TalkZafir` (11.11.2026)
- **When I:** ran `Ashes.TalkZafir <line>` with the built-in model stopped, so the command started it and sent the line when it was ready.
- **I expected:** Zafir's reply.
- **But:** the first request always failed in 0.02 s with HTTP 500 and the fallback line answered; later requests worked.
- **Error from the Output Log:** `LLM request failed after 0.04 s: http 500: ... Jinja Exception: No user query found in messages`; with `log LogAshes Verbose` the request body showed garbage text in every message.
- **My prompt:** (found during testing)
- **The fix:** the "send when the model is ready" lambda removed itself from `OnStateChanged` and then used its own captured request, which `Remove()` had already destroyed (use after free). Now it copies what it needs before `Remove()` (`ConversationDebugCommands.cpp`, same fix in `LLMDebugCommands.cpp`). HTTP errors now include the server's message; the request body is logged at Verbose level.
- **Did it work?** Yes: two cold-start runs in a row both got Zafir's reply (21 s and 24 s on the laptop).

## Bug 1: AI reply still times out after raising the timeout to 30 s
- **Found by:** Merve (11.11.2026)
- **When I:** ran `Ashes.TestLLM` with the 6 s limit (fallback, fine), raised Request Timeout Seconds to 30 and ran it again.
- **I expected:** Zafir's real reply within 30 s.
- **But:** the second run also fell back after 30 s with 0 tokens.
- **Error from the Output Log:** `LogHttp: Warning: HTTP request timed out after 30.00 seconds URL=http://127.0.0.1:8731/v1/chat/completions` / `LLM request failed after 30.01 s: timeout or connection failed`
- **My prompt:** "it still does not show..." (Claude Code, with the log screenshot)
- **The fix:** cause: llama-server runs one request at a time and, for non-streamed requests, keeps writing an answer the game already gave up on, so the next request waited behind it (measured: 38 s instead of 22 s; `/slots` still busy after the 6 s timeout). `UOpenAICompatibleLLMProvider` now requests with `stream: true` and collects the streamed chunks into the same `FLLMReply`; when the game times out and closes the connection, the server stops at once. Commit `[phase 3]: cancel timed-out AI requests`.
- **Did it work?** Yes: two back-to-back runs with the 6 s limit both fell back after ~6 s and the server was idle right after each; a full answer right after took 8.3 s; a 30 s run returned Zafir's reply in 9.0 s (26 tok/s). Side find: closing the editor also killed llama-server (job object works).

## Bug N: <short title> (template: copy this block for the next bug)
- **Found by:**
- **When I:** what you did
- **I expected:** what should happen
- **But:** what really happened
- **Error from the Output Log:** paste the red error line from the Unreal Output Log (or the compiler error from Visual Studio / Rider, or the Blueprint compiler message)
- **My prompt:** the prompt you wrote to fix it (also log it in `PromptLog.md`)
- **The fix:** what changed (file, commit)
- **Did it work?** yes / no, what else you tried
