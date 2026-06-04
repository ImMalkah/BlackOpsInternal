#pragma once

namespace HookManager
{
    typedef bool (__cdecl* CG_IsEntityFriendly_t)(int localClientNum, int clientNum);
    extern CG_IsEntityFriendly_t oCG_IsEntityFriendly;

    // Registers and enables the detour hooks
    void InitializeHooks();

    // Disables and removes detour hooks
    void ShutdownHooks();
}
