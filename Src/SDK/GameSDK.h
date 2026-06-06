#pragma once
#include <cstdint>

struct Vector3
{
    float x;
    float y;
    float z;
};

namespace GameSDK
{
    class clientinfo_t
    {
    public:
        uint32_t isValid; //0x0000
        uint32_t isActive; //0x0004
        uint32_t clientNum; //0x0008
        char name[32]; //0x000C
        uint32_t team; //0x002C
        char pad_0030[52]; //0x0030
        uint32_t ping; //0x0064
        uint32_t isDead; //0x0068
        char pad_006C[1380]; //0x006C
    }; //Size: 0x05D0 (1488)
    static_assert(sizeof(clientinfo_t) == 0x5D0, "clientinfo_t size mismatch");

    class centity_t
    {
    public:
        char pad_0000[48]; //0x0000
        Vector3 Position; //0x0030
        float Yaw; //0x003C
        float Pitch; //0x0040
        char pad_0044[420]; //0x0044
        int32_t clientNum; //0x01E8
        char pad_01EC[184]; //0x01EC
        uint8_t N0001C982; //0x02A4
        uint8_t N0001C9A4; //0x02A5
        uint8_t type; //0x02A6
        char pad_02A7[125]; //0x02A7
        uint32_t interpolationFlags; //0x0324
    }; //Size: 0x0328 (808)
    static_assert(sizeof(centity_t) == 0x328, "centity_t size mismatch");

    class playerState_t
    {
    public:
        char pad_0000[36]; //0x0000
        Vector3 position; //0x0024
        Vector3 velocity; //0x0030
        char pad_003C[264]; //0x003C
        uint16_t currentEquippedWeaponIndex; //0x0144
        char pad_0146[58]; //0x0146
        Vector3 viewAngles; //0x0180
        char pad_018C[56]; //0x018C
        uint32_t health; //0x01C4
        char pad_01C8[500]; //0x01C8
    }; //Size: 0x03BC
    static_assert(sizeof(playerState_t) == 0x3BC, "playerState_t size mismatch");

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
