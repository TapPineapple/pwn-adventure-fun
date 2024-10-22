#include <Windows.h>
#include <iostream>

#include "hooklib.hpp"

#define PI 3.14159265359

Utility::HookManager* pHookManager = 0;

// Define a structure for a 3D vector
struct vec3
{
    float x;
    float y;
    float z;
};


vec3* pPlayerPosition = 0;
vec3* pPlayerVelocity = 0;
float* pPlayerYaw = 0;

int getPlayerPosition(Utility::x86Registers* pRegs)
{
    uintptr_t playerPosition = pRegs->ecx + 0x90;
    pPlayerPosition = (vec3*)playerPosition;
    uintptr_t playerYaw = pRegs->ecx + 0xFC;
    pPlayerYaw = (float*)playerYaw;

	// Return the original function
	return Utility::HookManager::EXECUTE_TARGET_FUNCTION;
}

int getPlayerVelocity(Utility::x86Registers* pRegs)
{
    uintptr_t playerVelocity = pRegs->ecx + 0x7C;
	pPlayerVelocity = (vec3*)playerVelocity;

	// Return the original function
	return Utility::HookManager::EXECUTE_TARGET_FUNCTION;
}

int PwnThread(HMODULE hModule)
{
    // Allocate a console for debugging
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	std::cout << "Hello from inside the game's process!" << std::endl;
	std::cout << "TID: " << GetCurrentThreadId() << std::endl;
	
    // Begin hacking -.-

    // Define a boolean to store the state of the fly hack
    bool bFlyHack = false;

    uintptr_t gameLogicBA = (uintptr_t)GetModuleHandle(L"GameLogic.dll");
    uintptr_t pwnAdventure3BA = (uintptr_t)GetModuleHandle(L"PwnAdventure3-Win32-Shipping.exe");

    std::cout << "GameLogic.dll base address: 0x" << std::hex << gameLogicBA << std::endl;
    std::cout << "PwnAdventure3-Win32-Shipping.exe base address: 0x" << std::hex << pwnAdventure3BA << std::endl;
    


    pHookManager = new Utility::HookManager();

    uintptr_t hPlayerPosition = pwnAdventure3BA + 0xC0087; //the functions we will be hooking
    uintptr_t hPlayerVelocity = pwnAdventure3BA + 0x8926A4;
    pHookManager->HookFunctionExt(hPlayerPosition, (uintptr_t)getPlayerPosition, 7, false); // hook player position
    pHookManager->HookFunctionExt(hPlayerVelocity, (uintptr_t)getPlayerVelocity, 6, false); // hook player velocity


    // Wait for the user to press the Escape key
    while (!GetAsyncKeyState(VK_ESCAPE))
    {


        //toggles when you press the 'F' key
        if (GetAsyncKeyState(0x46) & 1)
		{
			bFlyHack = !bFlyHack;
		}
        if (bFlyHack && pPlayerPosition && pPlayerVelocity && pPlayerYaw)
        {
            
            pPlayerVelocity->x = 0.0f;
            pPlayerVelocity->y = 0.0f;
            pPlayerVelocity->z = 0.0f;
            
            float speed = 175.0f;
            float radYaw = (*pPlayerYaw * (PI / 180 ));
            if (GetAsyncKeyState(0x57)) //w
            {
                pPlayerPosition->x = pPlayerPosition->x + cos(radYaw) * speed;
                pPlayerPosition->y = pPlayerPosition->y + sin(radYaw) * speed;
            }
            if (GetAsyncKeyState(0x53)) //s
            {

                pPlayerPosition->x = pPlayerPosition->x - cos(radYaw) * speed;
                pPlayerPosition->y = pPlayerPosition->y - sin(radYaw) * speed;
            }
            if (GetAsyncKeyState(0x51)) //q
            {
                pPlayerPosition->z -= speed;
            }
            if (GetAsyncKeyState(0x45)) //e
            {
                pPlayerPosition->z += speed;
            }
        
        }

        Sleep(25);
	}
    pHookManager->UnhookFunction(hPlayerPosition);
    pHookManager->UnhookFunction(hPlayerVelocity);
    delete pHookManager;
	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

// Entry point for the DLL application
BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)PwnThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}