#pragma once
#include "framework.h"
inline auto ImageBase = *(uint64_t*)(__readgsqword(0x60) + 0x10);
namespace Addresses {
    inline uint64_t Step = 0xd6e9dc;
    inline uint64_t StepExplicitProperty = 0xfaeb20;
    inline uint64_t GetWorldContext = 0xc9f891;
    inline uint64_t CreateNetDriverWorldContext = 0x1794a5c;
    inline uint64_t InitHost = 0x3b26ed8;
    inline uint64_t PauseBeaconRequests = 0x50a55e0;
    inline uint64_t InitListen = 0x3b27284;
    inline uint64_t SetWorld = 0x11b826c;
    inline uint64_t GetNetMode = 0xd4e718;
    inline uint64_t TickFlush = 0xd122e0;
    inline uint64_t KickPlayer = 0x509bf28;
    inline uint64_t ServerReplicateActors = 0x3fd2b1c;
    inline uint64_t DispatchRequest = 0x1023d3c;
    inline uint64_t ReadyToStartMatch = 0x4a80d0c;
    inline uint64_t GetMaxTickRate = 0xe976cc;
    inline uint64_t SendRequestNow = 0x1023d3c;
    inline uint64_t Realloc = 0x197ebdc;
    inline uint64_t StaticFindObject = 0xd13c3c;
    inline uint64_t GiveAbility = 0x3d53ee0;
    inline uint64_t SpawnDefaultPawnFor = 0x4a8abf8;
    inline uint64_t StaticLoadObject = 0x14ad4a0;
    inline uint64_t GIsClient = 0x9c0af6b;
    inline uint64_t GameSessionPatch = 0x12a0044;
    inline uint64_t EncryptionPatch = 0x66e410e;
    inline uint64_t ApplyCharacterCustomization = 0x526b2e4;
    inline uint64_t AbilitySpecConstructor = 0x3d47310;
    inline uint64_t InternalTryActivateAbility = 0x3d555ec;
    inline uint64_t GiveAbilityAndActivateOnce = 0x3d54000;
    inline uint64_t GetInterfaceAddress = 0xcf9ea0;
    inline uint64_t InitPlayerAbilities = 0x4ec023c;
    inline std::vector<uint64_t> NullFuncs = { 0x4A8F4F8 };
    inline std::vector<uint64_t> RetTrueFuncs = { 0xd4e718, 0x3D27AC8, 0x4B21D38, 0x1229364 };
};