#include "DebugInfo.h"
#include "imgui.h"
#include "Core/Memory.h"
#include "SDK/gamesdk_offsets.h"
#include <Windows.h>

namespace
{
    constexpr std::uintptr_t Dvar_cg_drawFPS = 0xCC19E8;
    constexpr std::uintptr_t Dvar_cg_drawLagometer = 0xCC1AA8;

    void SetDvarIntDirect(std::uintptr_t dvarPointerOffset, int value)
    {
        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        std::uintptr_t dvarPtr = Memory::Read<std::uintptr_t>(baseAddress - 0x400000 + dvarPointerOffset, 0);
        if (dvarPtr != 0)
        {
            Memory::Write<int>(dvarPtr + 24, value);
        }
    }

    void SetDvarBoolDirect(std::uintptr_t dvarPointerOffset, bool value)
    {
        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        std::uintptr_t dvarPtr = Memory::Read<std::uintptr_t>(baseAddress - 0x400000 + dvarPointerOffset, 0);
        if (dvarPtr != 0)
        {
            Memory::Write<char>(dvarPtr + 24, value ? 1 : 0);
        }
    }

    void SetDvarBoolByName(const char* name, bool value)
    {
        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        typedef void (__cdecl* Dvar_SetBoolByName_t)(const char* name, bool value);
        Dvar_SetBoolByName_t fn = reinterpret_cast<Dvar_SetBoolByName_t>(baseAddress - 0x400000 + BlackOpsSDK::Dvar_SetBoolByName);
        __try
        {
            if (fn != nullptr)
                fn(name, value);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            // Fail silently
        }
    }
}

DebugInfoFeature::DebugInfoFeature()
    : FeatureBase("Upper Right Debug Info", "Diagnostics")
{
}

void DebugInfoFeature::OnMenu()
{
    ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "HUD Debug Controls (sub_430E00)");
    
    if (ImGui::Checkbox("Show FPS Counter", &m_showFPS))
    {
        SetEnabled(m_showFPS || m_showLagometer || m_showViewPos);
    }
    if (ImGui::Checkbox("Show Lagometer", &m_showLagometer))
    {
        SetEnabled(m_showFPS || m_showLagometer || m_showViewPos);
    }
    if (ImGui::Checkbox("Show View Position (debug_show_viewpos)", &m_showViewPos))
    {
        SetEnabled(m_showFPS || m_showLagometer || m_showViewPos);
    }
}

void DebugInfoFeature::OnUpdate()
{
    // Write directly to the game's dvar values
    SetDvarIntDirect(Dvar_cg_drawFPS, m_showFPS ? 1 : 0);
    SetDvarBoolDirect(Dvar_cg_drawLagometer, m_showLagometer);
    SetDvarBoolByName("debug_show_viewpos", m_showViewPos);
}

void DebugInfoFeature::OnDisable()
{
    // Reset everything when the feature is disabled/unloaded
    m_showFPS = false;
    m_showLagometer = false;
    m_showViewPos = false;
    
    SetDvarIntDirect(Dvar_cg_drawFPS, 0);
    SetDvarBoolDirect(Dvar_cg_drawLagometer, false);
    SetDvarBoolByName("debug_show_viewpos", false);
}

DebugInfoFeature g_debugInfoFeature;
