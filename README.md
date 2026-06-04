# BlackOpsInternal

A premium, performance-optimized, and exception-safe internal cheat DLL for **Call of Duty: Black Ops 1 Multiplayer** (Steam version v7.0.189). Built upon a custom overlay framework with ImGui for rendering.

---

## 🚀 Features

*   **Player 3D ESP**
    *   3D Bounding Boxes drawn accurately around player models.
    *   Caret-cleaned Player Name tags.
    *   Teammate filtering toggle.
    *   Distance slider for custom rendering ranges.
*   **Overhead Name Customization**
    *   **Force Enemy Names**: Forces the game engine to render nameplates for enemy players.
    *   **Custom Display Mode**: Forces names/icons/ranks display modes.
    *   **Disable**: Complete override to block overhead nameplates from rendering.
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

Alternatively, compile from PowerShell:
```powershell
powershell -ExecutionPolicy Bypass -File scratch/build.ps1
```
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

---

## ⚠️ Disclaimer
This repository is created strictly for educational, reverse engineering, and research purposes. Do not use this tool on public game servers.
