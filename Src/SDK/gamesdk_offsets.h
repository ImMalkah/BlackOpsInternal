// Generated Call of Duty: Black Ops resolved offsets
#pragma once

#include <cstdint>

namespace BlackOpsSDK
{
    constexpr std::uintptr_t CG_DrawOverheadNames = 0x68C600;
    constexpr std::uintptr_t CG_DrawUpperRightDebugInfo = 0x430E00;
    constexpr std::uintptr_t CG_KickAngles = 0x7F4B60;
    constexpr std::uintptr_t CG_PredictPlayerState = 0x477BD0;
    constexpr std::uintptr_t CG_RelativeTeamColor = 0x43E7E0;
    constexpr std::uintptr_t CG_LocationalTrace = 0x56AB80;
    constexpr std::uintptr_t CG_Flashbanged = 0x583F80;
    constexpr std::uintptr_t CG_Flared = 0x5AC900;
    constexpr std::uintptr_t CG_BulletEndPos = 0x7FC040;
    constexpr std::uintptr_t BG_WeaponAmmo = 0x459680;
    constexpr std::uintptr_t BG_GetSpreadForWeapon = 0x47DE70;
    constexpr std::uintptr_t BG_GetWeaponDef = 0x529FF0;
    constexpr std::uintptr_t BG_EvaluateTrajectory = 0x594C60;
    constexpr std::uintptr_t BG_GetViewmodelWeaponIndex = 0x6B94F0;
    constexpr std::uintptr_t BG_GetSurfacePenetrationDepth = 0x534980;
    constexpr std::uintptr_t BG_AdvanceTrace = 0x5A1E40;
    constexpr std::uintptr_t BG_IsAds = 0x67DC00;
    constexpr std::uintptr_t R_RegisterFont_FastFile = 0x6F2460;
    constexpr std::uintptr_t R_AddCmdDrawText = 0x6F44A0;
    constexpr std::uintptr_t R_AddCmdDrawStretchPic = 0x6F3F70;
    constexpr std::uintptr_t Material_RegisterHandle = 0x6D7AC0;
    constexpr std::uintptr_t R_AddDObjToScene = 0x6E0AF0;
    constexpr std::uintptr_t R_TextWidth = 0x6F24C0;
    constexpr std::uintptr_t CL_DrawScreen = 0x4A6520;
    constexpr std::uintptr_t CL_DrawSpinnerPhysical = 0x6AD5F0;
    constexpr std::uintptr_t CL_GetClientName = 0x561930;
    constexpr std::uintptr_t CL_AddReliableCommand = 0x639830;
    constexpr std::uintptr_t CL_ReadyToSendPacket = 0x805E70;
    constexpr std::uintptr_t CL_SetViewAngles = 0x4FA7D0;
    constexpr std::uintptr_t G_Say = 0x51BBD0;
    constexpr std::uintptr_t BulletTrace = 0x7FBE60;
    constexpr std::uintptr_t CachedTag_NoCache_GetTagPos = 0x7D5BD0;
    constexpr std::uintptr_t Cmd_ExecuteSingleCommand = 0x50B470;
    constexpr std::uintptr_t RandomBulletDir = 0x826E10;
    constexpr std::uintptr_t Trace_GetEntityHitId = 0x4CD4D0;
    constexpr std::uintptr_t WorldPosToScreenPos = 0x45CBB0;
    constexpr std::uintptr_t SL_FindLowercaseString = 0x52E450;
    constexpr std::uintptr_t Dvar_SetBoolByName = 0x47DFE0;
    constexpr std::uintptr_t Dvar_SetFloatByName = 0x5A1BF0;
    constexpr std::uintptr_t Dvar_GetBool = 0x5FD100;
    constexpr std::uintptr_t Dvar_GetFloat = 0x65FA20;
    constexpr std::uintptr_t va = 0x502CA0;

    constexpr std::uintptr_t CG_IsEntityFriendly = 0x5E2CB0;
    constexpr std::uintptr_t ClientInfoBase = 0xCBA4C0; // baseAddress + 0x8BA4C0
    constexpr std::uintptr_t ClientSize = 104;          // sizeof(clientInfo_t) (0x68)
    constexpr std::uintptr_t ClientNameOffset = 0;      // Name starts at offset 0 (0x0)

    // Entity array and layout settings
    constexpr std::uintptr_t CG_EntitiesBase = 0xD52374; // Pointer to array of entities (centity_t) -> BlackOpsMP.exe + 0x952374 (assuming 0x400000 base)
    constexpr std::uintptr_t CEntitySize = 808;          // sizeof(centity_t) (0x328)
    constexpr std::uintptr_t CEntityOriginOffset = 48;   // offsetof(centity_t, pos) (0x30)
}
