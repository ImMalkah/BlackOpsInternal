#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "UniversalOverlay.h"
#include "imgui.h"
#include "Core/Log.h"
#include "Features/FeatureManager.h"
#include "Engine/HookManager.h"

// Global DLL module handle for unloading
HMODULE g_hModule = nullptr;

DWORD WINAPI MainThread(LPVOID lpParam)
{
    // Allocate a debug console for visibility
    AllocConsole();
    FILE* fDummy = nullptr;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    Log::Debug("Console allocated. DLL starting up...");

    // A brief delay to make sure the target game process has completed initialization
    Sleep(1000);

    Log::Debug("Initializing UniversalOverlay (D3D9)...");

    // Initialize the UniversalOverlay framework for DirectX 9
    if (UniversalOverlay::Initialize(UniversalOverlay::GraphicsAPI::D3D9))
    {
        Log::Debug("UniversalOverlay initialized successfully!");

        // Register configs
        FeatureManager::RegisterConfigs();

        // Register Features tab
        UniversalOverlay::RegisterTab("Features", []() {
            FeatureManager::DrawMenu();
        });
        Log::Debug("Features tab registered.");

        // Register background rendering callback for ESP drawing
        UniversalOverlay::RegisterRenderCallback([]() {
            FeatureManager::OnRender();
        });
        Log::Debug("Background rendering callback registered.");


        // Register Entity List tab
        UniversalOverlay::RegisterTab("Entity List", []() {
            FeatureManager::DrawEntityListTab();
        });
        Log::Debug("Entity List tab registered.");

        // Initialize detour hooks
        HookManager::InitializeHooks();

        // Configure default keybinds programmatically (F1 to toggle, F11 to unload)
        WritePrivateProfileStringW(L"Keys", L"MenuToggle", L"112", L".\\overlay_config.ini"); // 112 = VK_F1
        WritePrivateProfileStringW(L"Keys", L"Unload", L"122", L".\\overlay_config.ini");      // 122 = VK_F11
        UniversalOverlay::LoadConfig(L".\\overlay_config.ini");
        Log::Debug("Loaded keybinds: F1 = Toggle Menu, F11 = Unload DLL.");

        // Set the menu to be open by default so it shows up immediately on injection
        UniversalOverlay::SetMenuOpen(true);
        Log::Debug("Menu open state activated.");
        Log::Debug("Press [F1] to toggle the overlay menu.");
        Log::Debug("Press [F11] to unload the DLL.");

        // Run until the unload hotkey (F11) is detected by UniversalOverlay
        while (!UniversalOverlay::ShouldUnload())
        {
            // Polling loop for cheat logic update
            FeatureManager::OnUpdate();
            Sleep(50);
        }

        Log::Debug("Unload requested. Shutting down overlay...");

        // Clean up detour hooks, overlay and features
        HookManager::ShutdownHooks();
        UniversalOverlay::Shutdown();
        FeatureManager::Shutdown();
    }
    else
    {
        Log::Debug("Error: Failed to initialize UniversalOverlay hooks!");
        Sleep(5000);
    }

    Log::Debug("Freeing console and exiting...");

    if (fDummy)
    {
        fclose(fDummy);
    }
    FreeConsole();

    // Exit thread and unload the DLL
    FreeLibraryAndExitThread(g_hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);
        if (HANDLE hThread = CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr))
        {
            CloseHandle(hThread);
        }
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
