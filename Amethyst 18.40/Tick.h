#pragma once
#include "framework.h"
#include "Globals.h"

namespace Tick 
{	
	inline void (*TickFlush_OG)(UNetDriver* Driver, float DeltaSeconds);
	inline void TickFlush(UNetDriver* Driver, float DeltaSeconds)
	{
		if (Driver->ReplicationDriver)
		{
			ServerReplicateActors(Driver->ReplicationDriver, DeltaSeconds);
		}

		return TickFlush_OG(Driver, DeltaSeconds);
	}

	inline float GetMaxTickRate()
	{
		return Amethyst::Tickrate;
	}

	void Hooks() 
	{
		MH_CreateHook((LPVOID)(ImageBase + Addresses::TickFlush), TickFlush, (LPVOID*)&TickFlush_OG);
		MH_CreateHook((LPVOID)(ImageBase + Addresses::GetMaxTickRate), GetMaxTickRate, nullptr);
		std::cout << "Tick hooked\n";
	}
}