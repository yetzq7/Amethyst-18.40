// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "framework.h"
#include "Gamemode.h"
#include "Globals.h"
#include "NetMode.h"
#include "Misc.h"
#include "Tick.h"
#include "Abilities.h"
#include "Addresses.h"
#include "PlayerController.h"

void AmethystConsole() 
{
	AllocConsole();
	FILE* f = nullptr;
	freopen_s(&f, "CONOUT$", "w+", stdout);
	SetConsoleTitleA("Amethyst 18.40 | Loading.");
}

void JewTerrain()
{
	UEngine::GetEngine()->GameViewport->World->OwningGameInstance->LocalPlayers.Remove(0);
	//UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open NoApollo_Terrain", nullptr);
	UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"open Apollo_Terrain", nullptr);
}

void Main() {
	AmethystConsole();

	MH_Initialize();
	Sleep(5000);

	Gamemode::Hooks();
	Tick::Hooks();
	Misc::Hooks();
	Net::Hook();
	Abilities::Hook();
	PC::Hook();
	
	MH_EnableHook(MH_ALL_HOOKS);
	*(bool*)(ImageBase + Addresses::GIsClient) = false;

	JewTerrain();
	
}

// properproper 100% jew israel
void ReturnHook()
{
	for (auto Offset : Addresses::NullFuncs)
		Hook(ImageBase + Offset, ReturnHook, nullptr);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	
		std::thread(Main).detach();
		break;
	}
	return TRUE;
}