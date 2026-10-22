#pragma once
#include "framework.h"

namespace Net 
{
	int (*GetNetMode_OG)(void*);
	int GetNetMode(void* a1)
	{
		return GetNetMode_OG(a1);
	}

	inline void Hook()
	{
		MH_CreateHook((LPVOID)(ImageBase + Addresses::GetNetMode), GetNetMode, (LPVOID*)&GetNetMode_OG);
		std::cout << "Net hooked\n";
	}
}