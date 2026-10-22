#pragma once
#include "framework.h"
#include "Addresses.h"
#include "Inventory.h"

namespace PC {

	// :]
	//inline constexpr int ServerAcknowledgePossessionIndex = 0x114;
	//inline constexpr int ServerExecuteInventoryItemIndex = 0x21d;

	inline bool SafeApplyCharacterCustomization(AFortPlayerStateAthena* PS, APawn* Pawn)
	{
		__try
		{
			((void* (*)(AFortPlayerStateAthena*, APawn*))(ImageBase + Addresses::ApplyCharacterCustomization))(PS, Pawn);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	inline bool SafeInitPlayerAbilities(void* AbilityInterface)
	{
		__try
		{
			((void (*)(void*))(ImageBase + Addresses::InitPlayerAbilities))(AbilityInterface);
			return true;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}
	}

	namespace Cosmetics {

		inline void ApplyManual(AFortPlayerControllerAthena* PC, AFortPlayerStateAthena* PS)
		{
			FFortAthenaLoadout& L = PC->CosmeticLoadoutPC;

			auto SetPart = [&](UCustomCharacterPart* Part) {
				if (Part) PS->CharacterData.Parts[(uintptr_t)Part->CharacterPartType] = Part;
				};

			if (L.Character && L.Character->HeroDefinition)
			{
				auto& Specs = L.Character->HeroDefinition->Specializations;
				for (int i = 0; i < Specs.Num(); i++)
				{
					auto Spec = StaticLoadObject<UFortHeroSpecialization>(
						UKismetStringLibrary::Conv_NameToString(Specs[i].ObjectID.AssetPathName).ToString());
					if (!Spec) continue;

					for (int j = 0; j < Spec->CharacterParts.Num(); j++)
					{
						SetPart(StaticLoadObject<UCustomCharacterPart>(
							UKismetStringLibrary::Conv_NameToString(Spec->CharacterParts[j].ObjectID.AssetPathName).ToString()));
					}
				}
			}

			if (L.Backpack)
				for (int i = 0; i < L.Backpack->CharacterParts.Num(); i++)
					SetPart(L.Backpack->CharacterParts[i]);

			PS->OnRep_CharacterData();
		}

		inline void Apply(AFortPlayerControllerAthena* PC, AFortPlayerStateAthena* PS, APawn* Pawn)
		{
			if (Addresses::ApplyCharacterCustomization && Pawn)
				SafeApplyCharacterCustomization(PS, Pawn);

			ApplyManual(PC, PS);
		}
	}

	inline void ServerAcknowledgePossession(AFortPlayerControllerAthena* PC, APawn* Pawn)
	{
		if (!PC) return;

		PC->AcknowledgedPawn = Pawn;

		auto PS = (AFortPlayerStateAthena*)PC->PlayerState;
		if (!PS) return;

		if (PC->CosmeticLoadoutPC.Character)
			PS->HeroType = PC->CosmeticLoadoutPC.Character->HeroDefinition;

		Cosmetics::Apply(PC, PS, Pawn);
		UFortKismetLibrary::UpdatePlayerCustomCharacterPartsVisualization(PS);

		if (Addresses::InitPlayerAbilities && Addresses::GetInterfaceAddress)
		{
			auto GetInterface = (void* (*)(UObject*, UClass*))(ImageBase + Addresses::GetInterfaceAddress);
			void* AbilityInterface = GetInterface(PS, IAbilitySystemInterface::StaticClass());

			if (AbilityInterface)
				SafeInitPlayerAbilities(AbilityInterface);
		}
	}

	inline void ServerExecuteInventoryItem(AFortPlayerController* Controller, FGuid ItemGuid)
	{
		if (!Controller || !Controller->MyFortPawn || !Controller->WorldInventory)
			return;

		FFortItemEntry* Entry = Inv::FindEntry(Controller->WorldInventory, ItemGuid);
		if (!Entry || !Entry->ItemDefinition)
			return;

		UFortItemDefinition* Def = Entry->ItemDefinition;

		if (Def->IsA(UFortDecoItemDefinition::StaticClass()))
			return;

		if (auto Gadget = Cast<UFortGadgetItemDefinition>(Def))
			Def = Gadget->GetWeaponItemDefinition();

		if (!Def)
			return;

		Controller->MyFortPawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)Def, Entry->ItemGuid, Entry->TrackerGuid, false);
	}

	inline void Hook()
	{
		void* Athena = AFortPlayerControllerAthena::GetDefaultObj();
		HookVTable(Athena, 0x114, ServerAcknowledgePossession, nullptr);
		HookVTable(Athena, 0x21d, ServerExecuteInventoryItem, nullptr);
		std::cout << "PlayerController Hooked\n";
	}
}