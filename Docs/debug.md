# Debug Log (debug.md) · Burnt Potato Games (Merve Özal, Tuana Tuna)

> One block per bug. Newest on top. Write the member's name in each block.
> Unreal Engine 5.8 project since 10.11.2026.

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
