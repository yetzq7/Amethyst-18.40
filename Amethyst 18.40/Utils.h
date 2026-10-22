#pragma once
#include "pch.h"
#include "framework.h"

inline void HookVTable(void* Object, int Index, void* Detour, void** OG = nullptr)
{
    void** VTable = *(void***)Object;

    if (OG)
        *OG = VTable[Index];

    DWORD OldProtect;
    VirtualProtect(&VTable[Index], sizeof(void*), PAGE_EXECUTE_READWRITE, &OldProtect);
    VTable[Index] = Detour;
    VirtualProtect(&VTable[Index], sizeof(void*), OldProtect, &OldProtect);
}
static void Hook(uint64_t Address, void* Detour, void** OG)
{
    MH_CreateHook(reinterpret_cast<LPVOID>(Address), Detour, OG);
    MH_EnableHook(reinterpret_cast<LPVOID>(Address));
}
