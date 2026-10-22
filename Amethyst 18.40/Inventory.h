#pragma once
#include "framework.h"
//#include "Funcs.h"
#include "Utils.h"

namespace Inv 
{
	inline bool GuidEq(const FGuid& A, const FGuid& B)
	{
		return A.A == B.A && A.B == B.B && A.C == B.C && A.D == B.D;
	}

	inline FFortItemEntry* FindEntry(AFortInventory* Inv, const FGuid& Guid)
	{
		if (!Inv) return nullptr;

		for (int i = 0; i < Inv->Inventory.ReplicatedEntries.Num(); i++)
		{
			if (GuidEq(Inv->Inventory.ReplicatedEntries[i].ItemGuid, Guid))
				return &Inv->Inventory.ReplicatedEntries[i];
		}

		return nullptr;
	}

	inline AFortInventory* SpawnInventoryActor(AActor* Owner)
	{
		FTransform Transform{};
		Transform.Scale3D = FVector{ 1, 1, 1 };
		Transform.Translation = FVector{ 0, 0, 0 };
		Transform.Rotation = FRotToQuat(FRotator{});

		return (AFortInventory*)UGameplayStatics::FinishSpawningActor(UGameplayStatics::BeginDeferredActorSpawnFromClass(UWorld::GetWorld(), AFortInventory::StaticClass(), Transform, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, Owner), Transform);
	}

	inline UFortWorldItem* AddItem(AFortInventory* Inv, UFortItemDefinition* Def, int32 Count, int32 LoadedAmmo = 0, int32 Level = 0)
	{
		if (!Inv || !Def || !Inv->GetOwner())
			return nullptr;

		auto Item = (UFortWorldItem*)Def->CreateTemporaryItemInstanceBP(Count, Level);
		if (!Item)
			return nullptr;

		Item->ItemEntry.LoadedAmmo = LoadedAmmo;
		Item->SetOwningControllerForTemporaryItem(Cast<AFortPlayerController>(Inv->GetOwner()));

		if (Def->IsA(UFortAmmoItemDefinition::StaticClass()) || Def->IsA(UFortResourceItemDefinition::StaticClass()))
		{
			FFortItemEntryStateValue Value{};
			Value.IntValue = true;
			Value.StateType = EFortItemEntryState::ShouldShowItemToast;
			Item->ItemEntry.StateValues.Add(Value);
		}

		Inv->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		Inv->Inventory.ItemInstances.Add(Item);

		Inv->bRequiresLocalUpdate = true;
		Inv->HandleInventoryLocalUpdate();
		Inv->Inventory.MarkItemDirty(Item->ItemEntry);

		return Item;
	}

	inline void SetupPlayer(AFortGameModeAthena* GameMode, AFortPlayerControllerAthena* PC)
	{
		if (!GameMode || !PC) return;

		if (!PC->WorldInventory)
		{
			PC->WorldInventory = SpawnInventoryActor(PC);

			if (PC->WorldInventory)
			{
				PC->WorldInventory->Owner = PC;
				PC->WorldInventory->OnRep_Owner();
			}
		}

		AFortInventory* Inv = PC->WorldInventory;
		if (!Inv) return;

		if (Inv->Inventory.ReplicatedEntries.Num() > 0)
			return;

		for (int i = 0; i < GameMode->StartingItems.Num(); i++)
		{
			if (GameMode->StartingItems[i].Count > 0)
				AddItem(Inv, GameMode->StartingItems[i].Item, GameMode->StartingItems[i].Count);
		}

		UFortItemDefinition* Pickaxe = nullptr;

		if (PC->CosmeticLoadoutPC.Pickaxe)
			Pickaxe = PC->CosmeticLoadoutPC.Pickaxe->WeaponDefinition;

		if (!Pickaxe)
		{
			Pickaxe = StaticLoadObject<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01");
		}

		if (Pickaxe)
			AddItem(Inv, Pickaxe, 1);
	}
}