# wii-stack-pvp

C++ starter project for a local Wii PvP game (original Wii) built as a boot.dol for SD/Homebrew Channel.

Features (starter):
- Main menu to choose 2–4 players
- MotionPlus-aware sword swings mapped to variable damage (approx 3%–15%) based on swing speed
- Health bars labeled with player IDs (P1, P2, ...)
- Rumble when health falls below 15%
- Simple placeholders for sword/hit SFX (replace with real WAVs in assets/)
- MIT license

Notes
- This is a starter project. It focuses on input, menu, damage/health and rumble logic. The in-game rendering uses simple console-driven HUD placeholders so you can extend to full 3D split-screen rendering later.
- Builds with devkitPro (devkitPPC) and libogc + wiiuse.

Repository contents
- src/: C++ source
- Makefile: builds boot.dol
- .github/workflows/build.yml: GitHub Actions build (attempts to install devkitPro and build)
- assets/: placeholder assets (replace with your SFX / models)

Building locally
1. Install devkitPro and devkitPPC. See: https://devkitpro.org/wiki/Getting_Started
   You need libogc and wiiuse (dkp-pacman packages `libogc` and `wiiuse`).
2. From repo root:
   make
   The produced `boot.dol` can be copied to the root of an SD card in a directory like `apps/wii-stack-pvp/` or as `boot.dol` inside a folder and launched with the Homebrew Channel.

GitHub Actions
- The workflow attempts to set up devkitPro and build on ubuntu-latest. See `.github/workflows/build.yml`.

Extending the project
- Replace the console HUD with split-screen views using libogc/GX for 3D rendering.
- Replace placeholder SFX in assets/ with actual WAV files and load with libogc audio routines.

