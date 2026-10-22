#pragma once
#include "framework.h"
#include "Addresses.h"
//#include "Funcs.h"
namespace Misc 
{
	static inline void (*KickPlayerOG)(AGameSession*, AController*);
	static void KickPlayer(AGameSession*, AController*) {
		return;
	}
	void (*DispatchRequestOG)(__int64 a1, unsigned __int64* a2, int a3);
	void DispatchRequest(__int64 a1, unsigned __int64* a2, int a3)
	{
		return DispatchRequestOG(a1, a2, 3);
	}
	
	static int ReturnTrue()
	{
		return 1;
	}
	static void ReturnHook()
	{
	}

	void Hooks()
	{
		MH_CreateHook((LPVOID)(ImageBase + Addresses::KickPlayer), KickPlayer, (LPVOID*)&KickPlayerOG);
		MH_CreateHook((LPVOID)(ImageBase + Addresses::DispatchRequest), DispatchRequest, (LPVOID*)&DispatchRequestOG);
		std::cout << "Misc hooked111\n";
	}

}

	
	