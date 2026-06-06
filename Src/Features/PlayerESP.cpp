#include "PlayerESP.h"
#include "SDK/GameSDK.h"
#include "SDK/gamesdk_offsets.h"
#include "Core/Memory.h"
#include "Core/Log.h"
#include "imgui.h"
#include <Windows.h>
#include <cmath>
#include <cstdio>

PlayerESPFeature::PlayerESPFeature() 
    : FeatureBase("Player 3D ESP", "Visuals") 
{
}

void PlayerESPFeature::OnMenu()
{
    ImGui::Checkbox("Show Enemy Names Through Walls", &m_showNames);
    ImGui::Checkbox("Show 3D Boxes", &m_showBoxes);
    ImGui::Checkbox("Show Teammates", &m_showTeammates);
    ImGui::SliderFloat("Max Distance (m)", &m_maxDistance, 10.0f, 500.0f, "%.0f m");
}

void PlayerESPFeature::OnRender()
{
    std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));

    // Setup debug logging throttling (outputs every 2.5 seconds)
    static DWORD lastLogTime = 0;
    DWORD curTime = GetTickCount();
    bool shouldLog = (curTime - lastLogTime > 2500);
    if (shouldLog)
    {
        lastLogTime = curTime;
        Log::Debug("--- ESP Frame Diagnostic ---");
        std::uintptr_t dword_D4F178 = baseAddress - 0x400000 + 0xD4F178;
        std::uintptr_t cg_ptr = Memory::Read<std::uintptr_t>(dword_D4F178);
        Log::Debug("CG base address (0xD4F178) = 0x%X, cg_ptr = 0x%X", dword_D4F178, cg_ptr);
        if (cg_ptr != 0)
        {
            float originX = Memory::Read<float>(cg_ptr + 274720);
            float originY = Memory::Read<float>(cg_ptr + 274724);
            float originZ = Memory::Read<float>(cg_ptr + 274728);
            float fovX = Memory::Read<float>(cg_ptr + 274704);
            float fovY = Memory::Read<float>(cg_ptr + 274708);
            Log::Debug("Camera: Origin = {%.2f, %.2f, %.2f}, fovX = %.2f, fovY = %.2f", originX, originY, originZ, fovX, fovY);
        }

        // Check raw entities
        std::uintptr_t entityBaseAddr = baseAddress - 0x400000 + BlackOpsSDK::CG_EntitiesBase;
        std::uintptr_t derefedEntityBase = Memory::Read<std::uintptr_t>(entityBaseAddr);
        Log::Debug("Entities: BaseAddr = 0x%X, DerefedBase = 0x%X", entityBaseAddr, derefedEntityBase);

        if (derefedEntityBase >= 0x10000 && derefedEntityBase < 0x7FFE0000)
        {
            for (int i = 0; i < 3; i++)
            {
                std::uintptr_t derefPtr = derefedEntityBase + sizeof(GameSDK::centity_t) * i;
                int16_t type = Memory::Read<int16_t>(derefPtr + offsetof(GameSDK::centity_t, type), -1);
                Vector3 position = Memory::Read<Vector3>(derefPtr + offsetof(GameSDK::centity_t, Position));
                Log::Debug("  Entity Slot %d: ptr = 0x%X, type = %d, pos = {%.2f, %.2f, %.2f}", 
                           i, derefPtr, type, position.x, position.y, position.z);
            }
        }
    }

    int activeEntitiesCount = 0;
    int projectedCount = 0;

    // Loop through all potential players (up to 32 in multiplayer matches)
    for (int i = 0; i < 32; i++)
    {
        float enemyPos[3] = { 0 };
        if (GameSDK::SafeReadEntityData(i, enemyPos))
        {
            int clientNum = i;

            // Skip rendering ESP for local player
            int localClientNum = GameSDK::GetLocalClientNum();
            if (clientNum == localClientNum)
            {
                continue;
            }

            // Check if teammate ESP is disabled, and skip if entity is friendly
            if (!m_showTeammates)
            {
                if (GameSDK::IsEntityFriendly(clientNum))
                {
                    continue; // Skip rendering ESP for teammates!
                }
            }

            activeEntitiesCount++;

            // Project 3D coordinate to 2D Screen Space
            float screenCoords[2] = { 0 };
            bool projected = GameSDK::CustomWorldPosToScreenPos(0, enemyPos, screenCoords);

            if (shouldLog)
            {
                Log::Debug("Entity %d: Pos = {%.2f, %.2f, %.2f}, Projected = %s, Screen = {%.2f, %.2f}",
                           i, enemyPos[0], enemyPos[1], enemyPos[2], projected ? "YES" : "NO", screenCoords[0], screenCoords[1]);
            }

            if (projected)
            {
                projectedCount++;

                // Safely retrieve player name from clientInfo structure using clientNum
                char nameClean[32] = { 0 };
                bool hasName = GameSDK::SafeReadClientInfo(clientNum, nameClean);
                if (hasName)
                {
                    nameClean[31] = '\0';
                }
                
                const char* displayName = (hasName && std::strlen(nameClean) > 0) ? nameClean : "Enemy";

                ImDrawList* drawList = ImGui::GetBackgroundDrawList();
                ImVec2 textPos(screenCoords[0], screenCoords[1]);
                
                // Render red name tag text on screen
                if (m_showNames)
                {
                    char espLabel[64];
                    sprintf_s(espLabel, "[%s]", displayName);
                    drawList->AddText(ImVec2(textPos.x - 20, textPos.y - 15), ImColor(255, 60, 60, 255), espLabel);
                }

                // Optionally render simple 3D ESP Bounding Boxes around enemies
                if (m_showBoxes)
                {
                    float top3D[3] = { enemyPos[0], enemyPos[1], enemyPos[2] + 65.0f }; // head position offset
                    float topScreen[2] = { 0 };
                    if (GameSDK::CustomWorldPosToScreenPos(0, top3D, topScreen))
                    {
                        float h = screenCoords[1] - topScreen[1];
                        float w = h / 2.0f;
                        drawList->AddRect(ImVec2(screenCoords[0] - w/2.0f, topScreen[1]), ImVec2(screenCoords[0] + w/2.0f, screenCoords[1]), ImColor(255, 60, 60, 255), 0.0f, 0, 1.5f);
                    }
                }
            }
        }
    }

    if (shouldLog)
    {
        Log::Debug("Frame Stats: Active Entities = %d, Projected on Screen = %d", activeEntitiesCount, projectedCount);
    }
}

// Define the global instance
PlayerESPFeature g_playerESPFeature;
