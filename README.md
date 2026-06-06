# BlackOpsInternal

A premium, performance-optimized, and exception-safe internal cheat DLL for **Call of Duty: Black Ops 1 Multiplayer** (Steam version v7.0.189). Built upon a custom overlay framework with ImGui for rendering.

---

## 🚀 Features

*   **Player 3D ESP**
    *   3D Bounding Boxes drawn accurately around player models.
    *   Caret-cleaned Player Name tags.
    *   Teammate filtering toggle.
    *   Distance slider for custom rendering ranges.
*   **Constant Radar**
    *   Forces enemy red dots to draw on the minimap/compass at all times by bypassing native UAV status checks.
    *   Safely updates the `g_compassShowEnemies` Dvar pointer and the internal `activeUAVs` & `activeSatellites` team status arrays.
*   **Overhead Name Customization**
    *   **Force Enemy Names**: Forces the game engine to render nameplates for enemy players.
    *   **Custom Display Mode**: Forces names/icons/ranks display modes.
    *   **Disable**: Complete override to block overhead nameplates from rendering.
*   **HUD Debug Diagnostics**
    *   Allows toggling native engine debug graphics (FPS, Lagometer, View Position) directly via ImGui menu controls by writing to `cg_drawFPS`, `cg_drawLagometer`, and `debug_show_viewpos`.
*   **Active Entity List**
    *   An interactive debug table displaying all active slots, clean names, team affiliations, entity type, coordinates, real-time 3D distance, and raw entity pointer addresses.
*   **Crash-Resistant Design**
    *   Full structured exception handling (`__try`/`__except`) wrapping game engine memory reads and direct function calls.

---

## 🛠️ Architecture & Codebase Structure

The project is logically structured into modular components:

```
Src/
├── Core/
│   ├── Memory.h      - Type-safe wrappers for memory reads/writes.
│   ├── Log.h         - Console/debug logging mechanism.
│   └── PatternScan.h - Dynamic signature scanning for offsets.
├── Engine/
│   ├── HookManager   - MinHook integration, detour hook definitions.
│   └── dllmain.cpp   - Entry point, initialization, overlay loop hooks.
├── Features/
│   ├── Feature.h        - Abstract base class for cheat features.
│   ├── FeatureManager   - Coordinates execution and rendering of features.
│   ├── OverheadNames    - Controls game-rendered overhead tag logic.
│   └── PlayerESP        - Performs 3D coordinates projection and ESP drawing.
└── SDK/
    ├── gamesdk_offsets.h - Verified game address offsets.
    ├── GameSDK.h         - Game function pointers and helper wrappers.
    └── GameSDK.cpp       - Implementations of GetLocalClientNum, IsEntityFriendly, etc.
```

---

## ⚙️ Compilation & Build Instructions

### Prerequisites
*   **Visual Studio 2022** with the **Desktop development with C++** workload installed.
*   The solution links against `UniversalOverlay.lib` and `libMinHook.x86.lib` configured for 32-bit x86.

### Compiling
To build the Release DLL:
1. Open `BlackOpsInternal.sln` in Visual Studio 2022.
2. Select the **Release | x86** configuration.
3. Build the solution (`Ctrl+Shift+B`).

The output DLL will be generated at:
`Build\Release\x86\BlackOpsInternal.dll`

---

## 🔧 Recent Engineering Fixes

### 1. Team-Switching Stability (G_Say Removal)
*   **Issue**: Intercepting chat messages via a `G_Say` detour caused access violations and immediate game crashes when players changed teams due to the engine sending system messages with invalid entity pointers.
*   **Fix**: Completely removed the `G_Say` hook, the `G_Say Test` UI tab, and related chat capture variables to guarantee 100% stability during matches.

### 2. Player Name Stack Corruption
*   **Issue**: Calling `CL_GetClientName` with a mismatched signature caused a `Run-Time Check Failure #0` (esp register corruption), crashing the overlay upon opening the Entity List.
*   **Fix**: Corrected the signature to a 4-parameter `__cdecl` format matching the engine:
    ```cpp
    void (__cdecl*)(int localClientNum, int clientNum, char* buf, int size)
    ```

### 3. Accurate Team Classification
*   **Issue**: Outdated/incorrect struct stride parameters caused teammate checks to fail, marking everyone as an `Enemy`.
*   **Fix**: Used the decompiled logic of `CG_RelativeTeamColor` (`0x43E7E0`):
    ```cpp
    v6 = *((_DWORD *)dword_D4F178 + 372 * a1 + 97433);
    ```
    Converting pointer arithmetic to offsets:
    *   `clientInfo_t` size is `372 * 4 = 1488` bytes.
    *   Team ID offset is `97433 * 4 = 389732` bytes.
    
    The team check now reads directly from `cg_ptr + 1488 * clientNum + 389732`, matching the game's native team classification.

### 4. Constant Radar (UAV Bypass)
*   **Issue**: Modifying only the `g_compassShowEnemies` Dvar (`0x343E6F4`) did not reveal enemies on the radar if no UAV killstreak was active, because the engine skipped calculating and drawing red dots when the internal UAV status flags were inactive.
*   **Fix**: Reversed the drawing check function `sub_538FF0` to map the game's UAV checks. Added a feature writing directly to the team status arrays:
    - **`activeSatellites`** (Advanced UAV / SR-71): `0x3446814`
    - **`activeUAVs`** (Standard UAV): `0x3446824`
    
    Writing `1` to these addresses for the client's team forces the engine to bypass target filtering and render all enemy dots natively.

---

## ⚠️ Disclaimer
This repository is created strictly for educational, reverse engineering, and research purposes. Do not use this tool on public game servers.
