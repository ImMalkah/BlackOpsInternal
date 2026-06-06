#include "FeatureManager.h"
#include "Feature.h"
#include "imgui.h"
#include "Core/Log.h"
#include "SDK/gamesdk_offsets.h"
#include "SDK/GameSDK.h"
#include "Core/Memory.h"
#include "OverheadNames.h"
#include "PlayerESP.h"
#include <cstring>
#include <cmath>
#include <Windows.h>

namespace
{
    std::vector<Feature*> g_features;

    bool HasCategory(const std::vector<const char*>& categories, const char* category)
    {
        for (const char* existing : categories)
        {
            if (std::strcmp(existing, category) == 0)
                return true;
        }
        return false;
    }

    void DrawFeatureRow(Feature* feature)
    {
        bool enabled = feature->IsEnabled();
        if (ImGui::Checkbox(feature->GetName(), &enabled))
        {
            bool wasEnabled = feature->IsEnabled();
            feature->SetEnabled(enabled);

            if (!wasEnabled && enabled)
                feature->OnEnable();
            else if (wasEnabled && !enabled)
                feature->OnDisable();
        }

        if (feature->IsEnabled())
        {
            ImGui::Indent();
            feature->OnMenu();
            ImGui::Unindent();
        }
    }


}

namespace FeatureManager
{
    void RegisterFeatures()
    {
        if (g_features.empty())
        {
            g_features.push_back(&g_overheadNamesFeature);
            g_features.push_back(&g_playerESPFeature);
        }
    }

    void Shutdown()
    {
        for (Feature* feature : g_features)
        {
            if (feature->IsEnabled())
            {
                feature->OnDisable();
            }
        }
    }

    void RegisterConfigs()
    {
        RegisterFeatures();
        for (Feature* feature : g_features)
        {
            feature->RegisterConfig();
        }
    }

    void OnUpdate()
    {
        for (Feature* feature : g_features)
        {
            if (feature->IsEnabled())
            {
                feature->OnUpdate();
            }
        }
    }

    void OnRender()
    {
        for (Feature* feature : g_features)
        {
            if (feature->IsEnabled())
            {
                feature->OnRender();
            }
        }
    }

    void DrawMenu()
    {
        RegisterFeatures();
        if (g_features.empty())
        {
            ImGui::Text("No features registered.");
            return;
        }

        std::vector<const char*> categories;
        for (Feature* feature : g_features)
        {
            if (!HasCategory(categories, feature->GetCategory()))
            {
                categories.push_back(feature->GetCategory());
            }
        }

        for (const char* category : categories)
        {
            if (!ImGui::CollapsingHeader(category, ImGuiTreeNodeFlags_DefaultOpen))
                continue;

            for (Feature* feature : g_features)
            {
                if (std::strcmp(feature->GetCategory(), category) == 0)
                {
                    DrawFeatureRow(feature);
                }
            }
        }
    }

    void DrawCategoryMenu(const char* category)
    {
        RegisterFeatures();
        bool hasFeatures = false;
        for (Feature* feature : g_features)
        {
            if (std::strcmp(feature->GetCategory(), category) == 0)
            {
                hasFeatures = true;
                feature->OnMenu();
            }
        }

        if (!hasFeatures)
        {
            ImGui::Text("No features in this category.");
        }
    }

    void DrawMenuExcept(const std::vector<const char*>& excludedCategories)
    {
        RegisterFeatures();
        std::vector<const char*> categories;
        for (Feature* feature : g_features)
        {
            bool excluded = false;
            for (const char* excl : excludedCategories)
            {
                if (std::strcmp(feature->GetCategory(), excl) == 0)
                {
                    excluded = true;
                    break;
                }
            }

            if (!excluded && !HasCategory(categories, feature->GetCategory()))
            {
                categories.push_back(feature->GetCategory());
            }
        }

        for (const char* category : categories)
        {
            if (!ImGui::CollapsingHeader(category, ImGuiTreeNodeFlags_DefaultOpen))
                continue;

            for (Feature* feature : g_features)
            {
                if (std::strcmp(feature->GetCategory(), category) == 0)
                {
                    DrawFeatureRow(feature);
                }
            }
        }
    }

    const std::vector<Feature*>& GetFeatures()
    {
        RegisterFeatures();
        return g_features;
    }

    void DrawEntityListTab()
    {
        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        
        // 1. Read Local Player State Info
        std::uintptr_t localPlayerPtr = Memory::Read<std::uintptr_t>(baseAddress + 0x2EB1D80, 0);

        float localPos[3] = { 0.0f, 0.0f, 0.0f };
        int localHealth = 0;
        bool hasLocalPlayer = false;

        if (localPlayerPtr >= 0x10000 && localPlayerPtr < 0x7FFE0000)
        {
            localPos[0] = Memory::Read<float>(localPlayerPtr + 0x24, 0.0f);
            localPos[1] = Memory::Read<float>(localPlayerPtr + 0x28, 0.0f);
            localPos[2] = Memory::Read<float>(localPlayerPtr + 0x2C, 0.0f);
            localHealth = Memory::Read<int>(localPlayerPtr + 0x1C4, 0);
            hasLocalPlayer = true;
        }

        // Display Local Player Card
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Local Player Info (playerState_t)");
        if (hasLocalPlayer)
        {
            ImGui::BulletText("Pointer Address: 0x%X", localPlayerPtr);
            ImGui::BulletText("Health: %d HP", localHealth);
            ImGui::BulletText("Position (X, Y, Z): %.2f, %.2f, %.2f", localPos[0], localPos[1], localPos[2]);
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Local player state pointer not found (Must be in a match)");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // 2. Display Entity List Tables (Split into Teammates and Enemies)
        std::uintptr_t cg_ptr_addr = baseAddress - 0x400000 + 0xD4F178;
        std::uintptr_t cg_ptr = Memory::Read<std::uintptr_t>(cg_ptr_addr, 0);

        auto RenderEntityTable = [&](const char* titleLabel, ImVec4 titleColor, bool teammatesOnly)
        {
            ImGui::TextColored(titleColor, "%s", titleLabel);
            
            if (ImGui::BeginTable(titleLabel, 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("Slot", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Team", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                ImGui::TableSetupColumn("Coordinates (X, Y, Z)", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Distance", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Entity Ptr", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                ImGui::TableHeadersRow();

                for (int i = 0; i < 32; i++)
                {
                    if (cg_ptr == 0)
                        continue;

                    std::uintptr_t clientInfoBase = cg_ptr + 389688; // 0x5F238
                    std::uintptr_t clientPtr = clientInfoBase + sizeof(GameSDK::clientinfo_t) * i;
                    uint32_t isActive = Memory::Read<uint32_t>(clientPtr + offsetof(GameSDK::clientinfo_t, isActive), 0);

                    // List the entities based on isActive (shows connected clients even if they are dead)
                    if (isActive == 0)
                        continue;

                    bool isFriendly = GameSDK::IsEntityFriendly(i);
                    if (teammatesOnly && !isFriendly)
                        continue;
                    if (!teammatesOnly && isFriendly)
                        continue;

                    std::uintptr_t entityBaseAddr = baseAddress - 0x400000 + BlackOpsSDK::CG_EntitiesBase;
                    std::uintptr_t entityBase = Memory::Read<std::uintptr_t>(entityBaseAddr, 0);
                    if (entityBase == 0)
                        continue;

                    std::uintptr_t entityPtr = entityBase + sizeof(GameSDK::centity_t) * i;
                    int16_t type = Memory::Read<int16_t>(entityPtr + offsetof(GameSDK::centity_t, type), -1);
                    uint32_t interpolationFlags = Memory::Read<uint32_t>(entityPtr + offsetof(GameSDK::centity_t, interpolationFlags), 0);
                    uint8_t valid = interpolationFlags & 0xFF;

                    Vector3 position = Memory::Read<Vector3>(entityPtr + offsetof(GameSDK::centity_t, Position));
                    float enemyPos[3] = { position.x, position.y, position.z };

                    char nameClean[32] = { 0 };
                    bool hasName = GameSDK::SafeReadClientInfo(i, nameClean);
                    if (hasName)
                    {
                        nameClean[31] = '\0';
                    }
                    const char* displayName = (hasName && std::strlen(nameClean) > 0) ? nameClean : "Bot / Unknown";

                    // Calculate 3D distance safely
                    float distance = 0.0f;
                    if (hasLocalPlayer)
                    {
                        float dx = enemyPos[0] - localPos[0];
                        float dy = enemyPos[1] - localPos[1];
                        float dz = enemyPos[2] - localPos[2];
                        distance = std::sqrt(dx * dx + dy * dy + dz * dz) * 0.0254f; // Convert inches to meters
                    }

                    ImGui::TableNextRow();
                    
                    // Slot
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", i);

                    // Name
                    ImGui::TableNextColumn();
                    int localClientNum = GameSDK::GetLocalClientNum();
                    bool isLocal = (i == localClientNum);
                    if (hasLocalPlayer && isLocal)
                    {
                        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "%s (Local)", displayName);
                    }
                    else
                    {
                        ImGui::Text("%s", displayName);
                    }

                    // Team
                    ImGui::TableNextColumn();
                    if (isLocal)
                    {
                        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Local");
                    }
                    else
                    {
                        if (isFriendly)
                            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Teammate");
                        else
                            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Enemy");
                    }

                    // Type
                    ImGui::TableNextColumn();
                    ImGui::Text("T:%d V:%d", type, valid);

                    // Coordinates
                    ImGui::TableNextColumn();
                    ImGui::Text("X: %.1f, Y: %.1f, Z: %.1f", enemyPos[0], enemyPos[1], enemyPos[2]);

                    // Distance
                    ImGui::TableNextColumn();
                    if (hasLocalPlayer)
                        ImGui::Text("%.1f m", distance);
                    else
                        ImGui::Text("N/A");

                    // Entity Ptr
                    ImGui::TableNextColumn();
                    ImGui::Text("0x%X", entityPtr);
                }

                ImGui::EndTable();
            }
        };

        RenderEntityTable("Teammates (Allies)", ImVec4(0.2f, 0.8f, 1.0f, 1.0f), true);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        RenderEntityTable("Enemies (Axis)", ImVec4(1.0f, 0.4f, 0.4f, 1.0f), false);
    }
}
