#pragma once 
#include "framework.h"
#include "Globals.h"
#include "Addresses.h"
#include "Abilities.h"
#include "Inventory.h"

namespace Gamemode {
	inline bool (*ReadyToStartMatchOG)(AFortGameModeAthena* GameMode);
	inline bool ReadyToStartMatch(AFortGameModeAthena* GameMode)
	{
		static bool bPlaylist = false;
		auto GameState = (AFortGameStateAthena*)GameMode->GameState;
		if (!GameState || !GameState->MapInfo) return false;
		if (!bPlaylist)
		{
			SetConsoleTitleA("Amethyst 18.40 | Loading..");
			static UFortPlaylistAthena* Playlist = nullptr;
			if (Amethyst::bArena)
			{
				Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_ShowdownAlt_Solo.Playlist_ShowdownAlt_Solo");
			}
			else
			{
				Playlist = UObject::FindObject<UFortPlaylistAthena>("FortPlaylistAthena Playlist_DefaultSolo.Playlist_DefaultSolo");
			}

			if (Amethyst::Respawns)
			{
				//#scuffed nations
				Playlist->bRespawnInAir = true;
				Playlist->RespawnHeight.Curve.CurveTable = nullptr;
				Playlist->RespawnHeight.Curve.RowName = FName();
				Playlist->RespawnHeight.Value = 9000;
				Playlist->RespawnTime.Curve.CurveTable = nullptr;
				Playlist->RespawnTime.Curve.RowName = FName();
				Playlist->RespawnTime.Value = 3;
				Playlist->RespawnType = EAthenaRespawnType::InfiniteRespawn;
				Playlist->bForceRespawnLocationInsideOfVolume = true;
			}

			GameState->CurrentPlaylistInfo.BasePlaylist = Playlist;
			GameState->CurrentPlaylistInfo.OverridePlaylist = Playlist;
			GameState->CurrentPlaylistInfo.PlaylistReplicationKey++;
			GameState->CurrentPlaylistInfo.MarkArrayDirty();

			GameState->WarmupCountdownStartTime = Amethyst::WarmupStartTime;

			GameMode->CurrentPlaylistId = Playlist->PlaylistId;
			GameMode->CurrentPlaylistName = Playlist->PlaylistName;

			GameState->CurrentPlaylistId = Playlist->PlaylistId;

			GameState->AirCraftBehavior = Playlist->AirCraftBehavior;

			GameMode->WarmupRequiredPlayerCount = 1;
			bPlaylist = true;
		}

		static bool bDriver = false;
		if (bDriver == false) {
			auto BeaconHost = SpawnActor<AFortOnlineBeaconHost>(FVector{ 0, 0, 0 });
			BeaconHost->ListenPort = Amethyst::bPort;
			InitHost(BeaconHost);
			PauseBeaconRequests(BeaconHost, false);
			UWorld* CurrentWorld = UWorld::GetWorld();
			CurrentWorld->NetDriver = BeaconHost->NetDriver;
			CurrentWorld->NetDriver->World = CurrentWorld;
			CurrentWorld->NetDriver->NetDriverName = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");
			FURL NetURL{};
			NetURL.Port = Amethyst::bPort;
			FString ListenError;
			if (InitListen(CurrentWorld->NetDriver, CurrentWorld, NetURL, true, ListenError) == false) {
				return false;
			}
			SetWorld(CurrentWorld->NetDriver, CurrentWorld);
			for (int LevelIndex = 0; LevelIndex < CurrentWorld->LevelCollections.Num(); LevelIndex++)
				CurrentWorld->LevelCollections[LevelIndex].NetDriver = CurrentWorld->NetDriver;
			GameMode->bWorldIsReady = true;
			GameState->OnRep_CurrentPlaylistInfo();
			GameState->OnRep_CurrentPlaylistId();
			bDriver = true;
		}

		if (bDriver)
		{
			SetConsoleTitleA("Amethyst 18.40 | ready!");
			std::cout << "[AMETHYST] Gs is up!", '\n';
		}

		return GameMode->AlivePlayers.Num() > 0;
	}

	namespace Pwn
	{
		using SpawnDefaultPawnForFn = APawn * (*)(AFortGameModeAthena*, AFortPlayerControllerAthena*, AActor*);
		inline SpawnDefaultPawnForFn SpawnDefaultPawnForOG = nullptr;

		inline APawn* SpawnDefaultPawnFor(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PC, AActor* StartSpot)
		{
			if (!SpawnDefaultPawnForOG)
				return nullptr;

			APawn* Pawn = SpawnDefaultPawnForOG(GameMode, PC, StartSpot);
			if (!Pawn || !PC)
				return Pawn;

			if (auto PlayerPawn = Cast<AFortPlayerPawnAthena>(Pawn))
			{
				PlayerPawn->bIsInAnyStorm = false;
				PlayerPawn->OnRep_IsInAnyStorm();
			}

			Inv::SetupPlayer(GameMode, PC);
			Abilities::GiveAbilitySet((AFortPlayerStateAthena*)PC->PlayerState, StaticLoadObject<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_AthenaPlayer.GAS_AthenaPlayer"));

			return Pawn;
		}
	}

	inline void Hooks()
	{
		MH_CreateHook((LPVOID)(ImageBase + Addresses::ReadyToStartMatch), ReadyToStartMatch, (LPVOID*)&ReadyToStartMatchOG);
		Pwn::SpawnDefaultPawnForOG = (Pwn::SpawnDefaultPawnForFn)(*(void***)AFortGameMode::GetDefaultObj())[0xca];
		HookVTable(AFortGameModeAthena::GetDefaultObj(), 0xca, Pwn::SpawnDefaultPawnFor, nullptr);
		std::cout << "Gamemode Hooked\n";
	}
}