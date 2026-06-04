#pragma once
#include <cstdint>

namespace GameSDK
{
    // Resolve the local player's client slot from cg_t
    int GetLocalClientNum();

    // Check if the given client is a friendly player/teammate
    bool IsEntityFriendly(int clientNum);

    // Safely retrieve player name (cleaned of carets) via CL_GetClientName or fallback memory reading
    bool SafeReadClientInfo(int clientNum, char* nameOut);

    // Safely retrieve origin coordinates for the player entity slot, validating alive state
    bool SafeReadEntityData(int clientNum, float* posOut);

    // Perform coordinate projection of 3D game coordinates to 2D screen coordinates
    bool CustomWorldPosToScreenPos(int localClientNum, float* worldPos, float* screenPosOut);


    // Strip Caret-based Call of Duty color codes from strings (e.g., "^1Name" to "Name")
    void CleanPlayerName(const char* in, char* out, int maxLen);
}
