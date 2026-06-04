#include "HookManager.h"
#include "SDK/gamesdk_offsets.h"
#include "SDK/GameSDK.h"
#include "Core/Log.h"
#include "Core/Memory.h"
#include "Features/OverheadNames.h"
#include <Windows.h>
#include <MinHook.h>

namespace HookManager
{
    // Detour typedefs and original pointers
    typedef void (__cdecl* CG_DrawOverheadNames_t)(int localClientNum, int centity_ptr, float alpha, int displayMode);
    CG_DrawOverheadNames_t oCG_DrawOverheadNames = nullptr;

    typedef bool (__cdecl* CG_IsEntityFriendly_t)(int localClientNum, int clientNum);
    CG_IsEntityFriendly_t oCG_IsEntityFriendly = nullptr;

    // 1. Hooked CG_IsEntityFriendly (checks if teammates/enemies should show overhead names)
    bool __cdecl Hooked_CG_IsEntityFriendly(int localClientNum, int clientNum)
    {
        __try
        {
            // If enemy names are forced, tell the game's overhead names renderer
            // that this client is friendly/should-be-rendered by returning true (1).
            if (g_overheadNamesFeature.ShouldForceEnemyNames())
            {
                return true;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            Log::Debug("Crash prevented in Hooked_CG_IsEntityFriendly check.");
        }
        return oCG_IsEntityFriendly(localClientNum, clientNum);
    }

    // 2. Hooked CG_DrawOverheadNames (manipulates name drawing based on UI config)
    void __cdecl Hooked_CG_DrawOverheadNames(int localClientNum, int centity_ptr, float alpha, int displayMode)
    {
        // Check if we should disable drawing overhead names completely
        if (g_overheadNamesFeature.ShouldDisable())
        {
            return; // Block rendering entirely!
        }

        // Force overhead display mode if configured
        int forcedMode = g_overheadNamesFeature.GetForcedDisplayMode();
        if (forcedMode != -1)
        {
            displayMode = forcedMode;
        }

        // Call original
        oCG_DrawOverheadNames(localClientNum, centity_ptr, alpha, displayMode);
    }

    void InitializeHooks()
    {
        // Initialize MinHook
        MH_Initialize();

        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));

        // 1. Hook CG_DrawOverheadNames
        std::uintptr_t targetAddress = baseAddress - 0x400000 + BlackOpsSDK::CG_DrawOverheadNames;
        if (MH_CreateHook(reinterpret_cast<LPVOID>(targetAddress), 
                          reinterpret_cast<LPVOID>(&Hooked_CG_DrawOverheadNames), 
                          reinterpret_cast<LPVOID*>(&oCG_DrawOverheadNames)) == MH_OK)
        {
            if (MH_EnableHook(reinterpret_cast<LPVOID>(targetAddress)) == MH_OK)
            {
                Log::Debug("Detoured CG_DrawOverheadNames successfully at 0x%X", targetAddress);
            }
            else
            {
                Log::Debug("Failed to enable detour hook for CG_DrawOverheadNames");
            }
        }
        else
        {
            Log::Debug("Failed to create detour hook for CG_DrawOverheadNames");
        }

        // 2. Hook sub_5E2CB0 (CG_IsEntityFriendly team check)
        std::uintptr_t teamCheckAddress = baseAddress - 0x400000 + BlackOpsSDK::CG_IsEntityFriendly;
        if (MH_CreateHook(reinterpret_cast<LPVOID>(teamCheckAddress), 
                          reinterpret_cast<LPVOID>(&Hooked_CG_IsEntityFriendly), 
                          reinterpret_cast<LPVOID*>(&oCG_IsEntityFriendly)) == MH_OK)
        {
            if (MH_EnableHook(reinterpret_cast<LPVOID>(teamCheckAddress)) == MH_OK)
            {
                Log::Debug("Detoured sub_5E2CB0 successfully at 0x%X", teamCheckAddress);
            }
            else
            {
                Log::Debug("Failed to enable detour hook for sub_5E2CB0");
            }
        }
        else
        {
            Log::Debug("Failed to create detour hook for sub_5E2CB0");
        }
    }

    void ShutdownHooks()
    {
        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));

        // Disable CG_DrawOverheadNames hook
        std::uintptr_t targetAddress = baseAddress - 0x400000 + BlackOpsSDK::CG_DrawOverheadNames;
        MH_DisableHook(reinterpret_cast<LPVOID>(targetAddress));
        Log::Debug("Disabled detour hook for CG_DrawOverheadNames");

        // Disable sub_5E2CB0 hook
        std::uintptr_t teamCheckAddress = baseAddress - 0x400000 + BlackOpsSDK::CG_IsEntityFriendly;
        MH_DisableHook(reinterpret_cast<LPVOID>(teamCheckAddress));
        Log::Debug("Disabled detour hook for sub_5E2CB0");
    }
}
