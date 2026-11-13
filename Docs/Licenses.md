# Asset Licenses and Credits

Every third-party asset goes here before it is merged (GDD 8.2). AI-assisted assets are marked. Tala keeps this file (from 12.11.2026); whoever adds an asset adds its row.

| Asset | Path in project | Source / author | License | AI-assisted? | Added by |
|-------|-----------------|-----------------|---------|--------------|----------|
| llama.cpp `llama-server` b12138 (Windows x64, Vulkan) — built-in AI server, shipped in the packaged game | `ThirdParty/LLM/llama-server/` (git-ignored; `Tools/GetLocalLLM.ps1`) | ggml-org/llama.cpp, https://github.com/ggml-org/llama.cpp | MIT (the llama.cpp LICENSE text must ship with the build, Phase 22; includes LLVM OpenMP `libomp.dll`: Apache 2.0 with LLVM exception, `LICENSE-LLVM-OpenMP`) | No | Merve (Claude Code), 10.11.2026 |
| Qwen3.5-4B Q4_K_M GGUF — built-in AI model, shipped in the packaged game | `ThirdParty/LLM/models/Qwen3.5-4B-Q4_K_M.gguf` (git-ignored) | Qwen team (Alibaba); GGUF by unsloth, https://huggingface.co/unsloth/Qwen3.5-4B-GGUF (commit e87f176) | Apache 2.0 | It is an AI model (its output is AI-generated dialogue, disclosed in the credits) | Merve (Claude Code), 10.11.2026 |
| VibeUE editor plugin (development only, not in the packaged game) | `Plugins/VibeUE/` (git submodule) | Kevin Buckley, https://github.com/kevinpbuckley/VibeUE | MIT (`Plugins/VibeUE/LICENSE`) | No | Merve (Claude Code), 10.11.2026 |
| Egypt Ruins Environment (environment pack: meshes, materials, textures, demo map) | `Content/Egypt_Ruins_Environment/` | Fab; listing name and author MISSING (copy from the Fab listing) | Fab Standard License | MISSING: check the Fab listing | Merve, 13.11.2026 |
| Landscape Collection: Dune Desert (landscape pack: materials, foliage, meshes, demo map) | `Content/LandscapeCollection/` | Fab; listing name and author MISSING (copy from the Fab listing) | Fab Standard License | MISSING: check the Fab listing | Merve, 13.11.2026 |
| Fantasy Chimney (3D model, imported from FBX at 100× scale) | `Content/Ashes/Art/Environment/Chimney/` (`SM_Chimney`) | CGTrader; author and link MISSING | CGTrader Royalty Free License | No | Merve, 13.11.2026 |
| Monopteros (3D model, imported from FBX at 100× scale) | `Content/Ashes/Art/Environment/Monopteros/` (`SM_Monopteros`) | CGTrader; author and link MISSING | CGTrader Royalty Free License | No | Merve, 13.11.2026 |
| Oval Temple (3D model, imported from FBX at 100× scale, Nanite on) | `Content/Ashes/Art/Environment/OvalTemple/` (`SM_OvalTemple`) | CGTrader; author and link MISSING | CGTrader Royalty Free License | No | Merve, 13.11.2026 |
