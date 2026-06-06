#include "ConstantRadar.h"
#include "imgui.h"
#include "Core/Memory.h"
#include "Core/Log.h"
#include "SDK/gamesdk_offsets.h"
#include <Windows.h>

namespace
{
    // Arrays of 4 DWORDs (one per team: 0 = free, 1 = allies, 2 = axis, 3 = spectator)
    constexpr std::uintptr_t activeSatellites_ptr = 0x3446814; // Advanced UAV
    constexpr std::uintptr_t activeUAVs_ptr = 0x3446824;       // Standard UAV
}

ConstantRadarFeature::ConstantRadarFeature()
    : FeatureBase("Constant Radar", "Visuals")
{
}

void ConstantRadarFeature::OnMenu()
{
    ImGui::Text("Forces enemy red dots on the minimap/compass at all times.");
}

void ConstantRadarFeature::OnUpdate()
{
    static DWORD lastLogTime = 0;
    DWORD curTime = GetTickCount();
    bool shouldLog = (curTime - lastLogTime > 2000); // Log every 2 seconds
    if (shouldLog)
    {
        lastLogTime = curTime;
    }

    std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    std::uintptr_t dvarPtrAddr = baseAddress - 0x400000 + BlackOpsSDK::g_compassShowEnemies_ptr;
    std::uintptr_t satellitesAddr = baseAddress - 0x400000 + activeSatellites_ptr;
    std::uintptr_t uavsAddr = baseAddress - 0x400000 + activeUAVs_ptr;

    // 1. Set the Dvar value (g_compassShowEnemies)
    std::uintptr_t dvarPtr = Memory::Read<std::uintptr_t>(dvarPtrAddr, 0);
    if (dvarPtr != 0)
    {
        Memory::Write<uint8_t>(dvarPtr + 24, 1);
    }

    // 2. Set the team UAV/Satellite status to 1 for all teams
    for (int team = 0; team < 4; team++)
    {
        Memory::Write<int>(satellitesAddr + (team * 4), 1);
        Memory::Write<int>(uavsAddr + (team * 4), 1);
    }

    if (shouldLog)
    {
        Log::Debug("[Radar Info] Updated Dvar and forced UAV/Satellite status to 1 for all teams.");
    }
}

void ConstantRadarFeature::OnDisable()
{
    std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    std::uintptr_t dvarPtrAddr = baseAddress - 0x400000 + BlackOpsSDK::g_compassShowEnemies_ptr;
    std::uintptr_t satellitesAddr = baseAddress - 0x400000 + activeSatellites_ptr;
    std::uintptr_t uavsAddr = baseAddress - 0x400000 + activeUAVs_ptr;

    // 1. Reset Dvar
    std::uintptr_t dvarPtr = Memory::Read<std::uintptr_t>(dvarPtrAddr, 0);
    if (dvarPtr != 0)
    {
        Memory::Write<uint8_t>(dvarPtr + 24, 0);
    }

    // 2. Reset team UAV/Satellite status
    for (int team = 0; team < 4; team++)
    {
        Memory::Write<int>(satellitesAddr + (team * 4), 0);
        Memory::Write<int>(uavsAddr + (team * 4), 0);
    }

    Log::Debug("[Radar Info] Constant Radar Disabled. Reset all states.");
}

ConstantRadarFeature g_constantRadarFeature;


