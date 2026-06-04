#include "GameSDK.h"
#include "gamesdk_offsets.h"
#include "Core/Memory.h"
#include "Core/Log.h"
#include "Engine/HookManager.h"
#include <Windows.h>
#include <cstring>
#include <cmath>

namespace GameSDK
{
    int GetLocalClientNum()
    {
        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        std::uintptr_t cg_ptr_addr = baseAddress - 0x400000 + 0xD4F178;
        
        std::uintptr_t cg_ptr = Memory::Read<std::uintptr_t>(cg_ptr_addr, 0);
        if (cg_ptr != 0)
        {
            return Memory::Read<int>(cg_ptr, 0);
        }
        return 0; // Default to slot 0 if not found
    }

    bool IsEntityFriendly(int clientNum)
    {
        if (clientNum < 0 || clientNum >= 32)
            return false;

        int localClientNum = GetLocalClientNum();
        if (localClientNum < 0 || localClientNum >= 32)
            return false;

        if (clientNum == localClientNum)
            return true;

        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        std::uintptr_t cg_ptr_addr = baseAddress - 0x400000 + 0xD4F178;
        std::uintptr_t cg_ptr = Memory::Read<std::uintptr_t>(cg_ptr_addr, 0);
        if (cg_ptr >= 0x10000 && cg_ptr < 0x7FFE0000)
        {
            // Read team directly using the verified offset from CG_RelativeTeamColor decompilation (0x43E7E0)
            // Index in DWORDs: 372 * clientNum + 97433.
            // Offset in bytes: 1488 * clientNum + 389732.
            int localTeam = Memory::Read<int>(cg_ptr + 1488 * localClientNum + 389732, 0);
            int targetTeam = Memory::Read<int>(cg_ptr + 1488 * clientNum + 389732, 0);

            // CoD team values:
            // 0: TEAM_FREE (unassigned or FFA)
            // 1: TEAM_ALLIES
            // 2: TEAM_AXIS
            // 3: TEAM_SPECTATOR
            if (localTeam == 0 || targetTeam == 0)
                return false;

            if (localTeam == 3 || targetTeam == 3)
                return false;

            return localTeam == targetTeam;
        }

        return false;
    }

    void CleanPlayerName(const char* in, char* out, int maxLen)
    {
        int outIdx = 0;
        for (int i = 0; in[i] != '\0' && outIdx < maxLen - 1; i++)
        {
            if (in[i] == '^' && in[i+1] != '\0')
            {
                i++; // Skip the caret and the color digit/character
                continue;
            }
            out[outIdx++] = in[i];
        }
        out[outIdx] = '\0';
    }

    bool SafeReadClientInfo(int clientNum, char* nameOut)
    {
        if (clientNum < 0 || clientNum >= 32)
            return false;

        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));

        // 1. Try calling the game's CL_GetClientName engine function
        __try
        {
            typedef void (__cdecl* CL_GetClientName_t)(int localClientNum, int clientNum, char* buf, int size);
            CL_GetClientName_t fn = reinterpret_cast<CL_GetClientName_t>(baseAddress - 0x400000 + BlackOpsSDK::CL_GetClientName);
            
            char buf[32] = { 0 };
            fn(0, clientNum, buf, sizeof(buf)); // 0 is localClientNum, buf is output buffer
            
            if (std::strlen(buf) > 0)
            {
                CleanPlayerName(buf, nameOut, 32);
                return true;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            Log::Debug("Crash prevented in CL_GetClientName call.");
        }

        // 2. Fallback: Read clientInfo name directly from cg_t memory
        std::uintptr_t cg_ptr_addr = baseAddress - 0x400000 + 0xD4F178;
        std::uintptr_t cg_ptr = Memory::Read<std::uintptr_t>(cg_ptr_addr, 0);
        if (cg_ptr >= 0x10000 && cg_ptr < 0x7FFE0000)
        {
            std::uintptr_t namePtr = cg_ptr + 1488 * clientNum + 389716;

            char rawName[32] = { 0 };
            for (int i = 0; i < 31; i++)
            {
                rawName[i] = Memory::Read<char>(namePtr + i, '\0');
            }
            rawName[31] = '\0';

            if (std::strlen(rawName) > 0)
            {
                CleanPlayerName(rawName, nameOut, 32);
                return true;
            }
        }

        return false;
    }

    bool SafeReadEntityData(int clientNum, float* posOut)
    {
        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        
        std::uintptr_t entityBaseAddr = baseAddress - 0x400000 + BlackOpsSDK::CG_EntitiesBase;
        std::uintptr_t entityBase = Memory::Read<std::uintptr_t>(entityBaseAddr, 0);
        if (entityBase == 0)
            return false;

        std::uintptr_t entityPtr = entityBase + BlackOpsSDK::CEntitySize * clientNum;

        // Check if entity is valid/alive (offset 804 is 2 for alive players, 0 for dead/inactive)
        uint8_t valid = Memory::Read<uint8_t>(entityPtr + 804, 0);
        if (valid == 0)
            return false;

        // Read origin coordinates safely
        posOut[0] = Memory::Read<float>(entityPtr + BlackOpsSDK::CEntityOriginOffset, 0.0f);
        posOut[1] = Memory::Read<float>(entityPtr + BlackOpsSDK::CEntityOriginOffset + 4, 0.0f);
        posOut[2] = Memory::Read<float>(entityPtr + BlackOpsSDK::CEntityOriginOffset + 8, 0.0f);

        // Sanity check coordinates to avoid rendering zero/uninitialized coords
        if (posOut[0] == 0.0f && posOut[1] == 0.0f)
            return false;

        return true;
    }

    bool CustomWorldPosToScreenPos(int localClientNum, float* worldPos, float* screenPosOut)
    {
        std::uintptr_t baseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
        std::uintptr_t cg_ptr_addr = baseAddress - 0x400000 + 0xD4F178;
        
        std::uintptr_t cg_ptr = Memory::Read<std::uintptr_t>(cg_ptr_addr, 0);
        if (cg_ptr == 0)
            return false;

        float originX = Memory::Read<float>(cg_ptr + 274720, 0.0f);
        float originY = Memory::Read<float>(cg_ptr + 274724, 0.0f);
        float originZ = Memory::Read<float>(cg_ptr + 274728, 0.0f);

        float m00 = Memory::Read<float>(cg_ptr + 274740, 0.0f);
        float m01 = Memory::Read<float>(cg_ptr + 274744, 0.0f);
        float m02 = Memory::Read<float>(cg_ptr + 274748, 0.0f);

        float m10 = Memory::Read<float>(cg_ptr + 274752, 0.0f);
        float m11 = Memory::Read<float>(cg_ptr + 274756, 0.0f);
        float m12 = Memory::Read<float>(cg_ptr + 274760, 0.0f);

        float m20 = Memory::Read<float>(cg_ptr + 274764, 0.0f);
        float m21 = Memory::Read<float>(cg_ptr + 274768, 0.0f);
        float m22 = Memory::Read<float>(cg_ptr + 274772, 0.0f);

        float fovX = Memory::Read<float>(cg_ptr + 274704, 0.0f);
        float fovY = Memory::Read<float>(cg_ptr + 274708, 0.0f);

        float relX = worldPos[0] - originX;
        float relY = worldPos[1] - originY;
        float relZ = worldPos[2] - originZ;

        float v7 = (m02 * relZ) + (m01 * relY) + (m00 * relX);
        float v8 = (m12 * relZ) + (m11 * relY) + (m10 * relX);
        float v9 = (m22 * relZ) + (m21 * relY) + (m20 * relX);

        std::uintptr_t viewportBase = baseAddress - 0x400000 + 0x23C6628 + (120 * localClientNum);
        float viewport_x = Memory::Read<float>(viewportBase, 0.0f);
        float viewport_y = Memory::Read<float>(viewportBase + 4, 0.0f);
        float viewport_width = Memory::Read<float>(viewportBase + 8, 0.0f);
        float viewport_height = Memory::Read<float>(viewportBase + 12, 0.0f);

        if (v7 >= 0.0f)
        {
            if (fovX == 0.0f || fovY == 0.0f || v7 == 0.0f)
                return false;

            screenPosOut[0] = ((1.0f - (v8 / fovX) * (1.0f / v7)) * viewport_width * 0.5f) + viewport_x;
            screenPosOut[1] = ((1.0f - (v9 / fovY) * (1.0f / v7)) * viewport_height * 0.5f) + viewport_y;
            return true;
        }
        else
        {
            float v10 = -v8;
            float v11 = -v9;
            screenPosOut[0] = v10;
            screenPosOut[1] = v11;
            float v12 = std::fabs(v10);
            if (v12 >= 0.001f || std::fabs(v11) >= 0.001f)
            {
                if (v12 < 0.001f)
                    screenPosOut[0] = 0.001f;
                if (std::fabs(v11) < 0.001f)
                    screenPosOut[1] = 0.001f;

                for (float i = viewport_width; i > std::fabs(screenPosOut[0]); i = viewport_width)
                {
                    screenPosOut[0] *= i;
                    screenPosOut[1] *= i;
                }

                float v15 = viewport_height;
                if (v15 > std::fabs(screenPosOut[1]))
                {
                    do
                    {
                        screenPosOut[0] *= v15;
                        float v16 = screenPosOut[1] * v15;
                        screenPosOut[1] = v16;
                        v15 = viewport_height;
                    } while (v15 > std::fabs(screenPosOut[1]));
                }
                return false;
            }
            else
            {
                screenPosOut[1] = viewport_height * 2.0f;
                return false;
            }
        }
    }

}
