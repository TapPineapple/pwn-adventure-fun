#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

namespace hack
{
	//Internal Pattern Scan
	void* PatternScan(char* base, size_t size, char* pattern, char* mask);

	//External Wrapper
	void* PatternScanEx(HANDLE hPRocess, uintptr_t begin, uintptr_t end, char* pattern, char* mask);

	void* PatternScanExModule(HANDLE hProcess, wchar_t* exeName, wchar_t* module, char* pattern, char* mask);

	DWORD GetProcId(const wchar_t* procName);

	MODULEENTRY32 GetModule(DWORD dwProcID, wchar_t* moduleName);

	uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

	uintptr_t FindDAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
}
