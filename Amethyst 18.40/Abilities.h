#pragma once
#include "framework.h"
#include "Addresses.h"
#include "Utils.h"

namespace Abilities 
{
	namespace Native {
		inline void (*GiveAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec) = nullptr;
		inline void (*SpecCtor)(FGameplayAbilitySpec*, UGameplayAbility*, int, int, UObject*) = nullptr;
		inline bool (*InternalTryActivateAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, FPredictionKey, UGameplayAbility**, void*, const FGameplayEventData*) = nullptr;
	}

	inline bool bReady = false;

	inline bool Init()
	{
		if (!Addresses::GiveAbility || !Addresses::AbilitySpecConstructor || !Addresses::InternalTryActivateAbility)
			return false;

		Native::GiveAbility = decltype(Native::GiveAbility)(ImageBase + Addresses::GiveAbility);
		Native::SpecCtor = decltype(Native::SpecCtor)(ImageBase + Addresses::AbilitySpecConstructor);
		Native::InternalTryActivateAbility = decltype(Native::InternalTryActivateAbility)(ImageBase + Addresses::InternalTryActivateAbility);

		bReady = true;
		return true;
	}

	inline FGameplayAbilitySpec* FindSpecFromHandle(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle)
	{
		for (int i = 0; i < ASC->ActivatableAbilities.Items.Num(); i++)
		{
			if (ASC->ActivatableAbilities.Items[i].Handle.Handle == Handle.Handle)
				return &ASC->ActivatableAbilities.Items[i];
		}
		return nullptr;
	}

	inline void GiveAbility(AFortPlayerStateAthena* PS, UClass* AbilityClass)
	{
		if (!bReady || !PS || !PS->AbilitySystemComponent || !AbilityClass || !AbilityClass->DefaultObject)
			return;

		UAbilitySystemComponent* ASC = PS->AbilitySystemComponent;

		for (int i = 0; i < ASC->ActivatableAbilities.Items.Num(); i++)
		{
			auto& Existing = ASC->ActivatableAbilities.Items[i];
			if (Existing.Ability && Existing.Ability->Class == AbilityClass)
				return;
		}

		FGameplayAbilitySpec Spec{};
		Native::SpecCtor(&Spec, (UGameplayAbility*)AbilityClass->DefaultObject, 1, -1, nullptr);
		Native::GiveAbility(ASC, &Spec.Handle, Spec);
	}

	inline void GiveAbilitySet(AFortPlayerStateAthena* PS, UFortAbilitySet* AbilitySet)
	{
		if (!PS || !AbilitySet || !PS->AbilitySystemComponent)
			return;

		for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
			GiveAbility(PS, AbilitySet->GameplayAbilities[i].Get());

		for (int i = 0; i < AbilitySet->GrantedGameplayEffects.Num(); i++)
		{
			UClass* Effect = AbilitySet->GrantedGameplayEffects[i].GameplayEffect.Get();
			if (!Effect) continue;

			PS->AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(Effect, AbilitySet->GrantedGameplayEffects[i].Level, FGameplayEffectContextHandle());
		}
	}

	inline void InternalServerTryActivateAbility(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle Handle, bool InputPressed, const FPredictionKey& PredictionKey, const FGameplayEventData* TriggerEventData)
	{
		FGameplayAbilitySpec* Spec = FindSpecFromHandle(ASC, Handle);
		if (!Spec)
		{
			ASC->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
			return;
		}

		UGameplayAbility* InstancedAbility = nullptr;
		Spec->InputPressed = true;

		if (!Native::InternalTryActivateAbility(ASC, Handle, PredictionKey, &InstancedAbility, nullptr, TriggerEventData))
		{
			ASC->ClientActivateAbilityFailed(Handle, PredictionKey.Current);
			Spec->InputPressed = false;
			ASC->ActivatableAbilities.MarkItemDirty(*Spec);
		}
	}

	inline void Hook()
	{
		if (!Init())
			return;

		HookVTable(UAbilitySystemComponent::GetDefaultObj(), 0xFE, InternalServerTryActivateAbility, nullptr);
		HookVTable(UFortAbilitySystemComponent::GetDefaultObj(), 0xFE, InternalServerTryActivateAbility, nullptr);
		HookVTable(UFortAbilitySystemComponentAthena::GetDefaultObj(), 0xFE, InternalServerTryActivateAbility, nullptr);
		std::cout << "Abilities hooked\n";
	}
}