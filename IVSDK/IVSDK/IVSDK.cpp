#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "version.lib")
#include <windows.h>
#include <stdint.h>
#include <string>
#include <list>
#include <d3dx9.h>
#include "injector/injector.hpp"

#include "IVSDK.h"
#include "Scripting/Scripting.h"
#include "Hooks.h"

namespace plugin
{
	HMODULE GetCurrentModule()
	{
		HMODULE hModule = NULL;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetCurrentModule, &hModule);
		return hModule;
	}

	uintptr_t DoHook(uintptr_t address, void(*Function)())
	{
		if (address)
			return (uintptr_t)injector::MakeCALL(address, Function);

		return 0;
	}

	// Responsible for initializing the hooks for all the IV-SDK events like "processScriptsEvent" or "ingameStartupEvent".
	void InitHooks()
	{
		DebugLog(L"[IV-SDK] [plugin::InitHooks] Initializing hooks.\r\n");
		processScriptsEvent::returnAddress =	DoHook(AddressSetter::Get(0x21601, 0x95141), processScriptsEvent::MainHook);
		gameLoadEvent::returnAddress =			DoHook(AddressSetter::Get(0x4ADB38, 0x770748), gameLoadEvent::MainHook);
		gameLoadPriorityEvent::returnAddress =	DoHook(AddressSetter::Get(0x4ADA9D, 0x7706AD), gameLoadPriorityEvent::MainHook);
		drawingEvent::returnAddress =			DoHook(AddressSetter::Get(0x46AFA8, 0x60E1C8), drawingEvent::MainHook);
		processAutomobileEvent::callAddress =	DoHook(AddressSetter::Get(0x7FE9C6, 0x652C26), processAutomobileEvent::MainHook);
		processPadEvent::callAddress =			DoHook(AddressSetter::Get(0x3C4002, 0x46A802), processPadEvent::MainHook);
		processCameraEvent::returnAddress =		DoHook(AddressSetter::Get(0x52C4C2, 0x694232), processCameraEvent::MainHook);
		mountDeviceEvent::returnAddress =		DoHook(AddressSetter::Get(0x3B2E27, 0x456C27), mountDeviceEvent::MainHook);
		ingameStartupEvent::returnAddress =		DoHook(AddressSetter::Get(0x20379, 0x93F09), ingameStartupEvent::MainHook);
	}

	DWORD WINAPI Init(HMODULE hModule)
	{
		// Initialize important stuff
		if (!AddressSetter::Init())
		{
			DebugLog(L"[IV-SDK] [plugin::Init] Failed to initialize AddressSetter! Returning.\r\n");
			return 0;
		}

		DebugLog(L"[IV-SDK] [plugin::Init] Starting to initialize hooks now.\r\n");
		InitHooks();

		// Call startup event
		DebugLog(L"[IV-SDK] [plugin::Init] Calling startup event now.\r\n");
		gameStartupEvent();

		return 1;
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		DebugLog(L"[IV-SDK] [DllMain] DLL_PROCESS_ATTACH called. Creating initialize thread now.\r\n");

		// TODO: Check if this causes no problems
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)plugin::Init, plugin::GetCurrentModule(), 0, nullptr);
	}

	return TRUE;
}
