//#pragma once

#ifndef SPYMEM
#define SPYMEM

#include <Windows.h>
#include <cstdio>

extern BYTE ret, jmp, nop, call, push;

#define NT_SUCCESS(x) ((x) >= 0)
typedef NTSTATUS(NTAPI *_NtWriteVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, LPCVOID Buffer, ULONG NumberOfBytesToWrite, PULONG NumberOfBytesWritten);
_NtWriteVirtualMemory NtWriteVirtualMemory = (_NtWriteVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWriteVirtualMemory");
typedef NTSTATUS(NTAPI *_NtReadVirtualMemory)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesRead);
_NtReadVirtualMemory NtReadVirtualMemory = (_NtReadVirtualMemory)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtReadVirtualMemory");

extern HANDLE hProcess;

 int rvm(PVOID addr, int num, void* buf)
{
	SIZE_T sz = 0;
	DWORD oldProtect = 0;
	VirtualProtectEx(hProcess, addr, num, PAGE_EXECUTE_READWRITE, &oldProtect);
	if (NTSTATUS status = NtReadVirtualMemory(hProcess, addr, buf, num, &sz) >= 0)
	{
		VirtualProtectEx(hProcess, addr, num, oldProtect, NULL);
#ifdef DEBUG
		//cout << "NtReadVirtualMemory failed with status: " << hex << status << "\n";
		//printf("RVM error %02X at %08x\n", status, addr);
#endif
		return 0;
	}

	VirtualProtectEx(hProcess, addr, num, oldProtect, NULL);
	return sz;
}

 DWORD rpm(DWORD addr) {
	SIZE_T sz = 0;
	DWORD buf;
	int r = ReadProcessMemory(hProcess, PVOID(addr), &buf, sizeof(DWORD), &sz);
	if (r == 0 || sz == 0) {
#ifdef DEBUG
		//printf("RPM error %02X at %08x\n", GetLastError(), addr);
#endif
		return 0;
	}
	return buf;
}

 int wpm(PVOID addr, int num, void* buf)
{
	SIZE_T sz = 0;
	int r = WriteProcessMemory(hProcess, addr, buf, num, &sz);
	if (r == 0 || sz == 0) {
#ifdef DEBUG
		printf("WPM error %02X at %08x \n", GetLastError(), addr);
#endif
		return 0;
	}
	return 1;
}

template <class dataType>
 BOOL wpm(DWORD addr, dataType buf)
{
	SIZE_T sz = 0;
	int r = WriteProcessMemory(hProcess, PVOID(addr), &buf, sizeof(dataType), &sz);
	if (r == 0 || sz == 0) {
#ifdef DEBUG
		printf("WPM error %02X at %08x \n", GetLastError(), addr);
#endif
		return 0;
	}
	return 1;
}

template <class dataType>
 void wvm(LPVOID addr, ULONG num, dataType buf)
{
	DWORD oldProtect; NTSTATUS status;
	VirtualProtectEx(hProcess, addr, num, PAGE_EXECUTE_READWRITE, &oldProtect);
	if (!NT_SUCCESS(status = NtWriteVirtualMemory(hProcess, addr, &buf, num, NULL)))
		printf("WVM error %02X at %08x \n", status, addr);
	VirtualProtectEx(hProcess, addr, num, oldProtect, NULL);
	//printf("Writing virtual memory %08x (%d bytes, addr %08x) to %08x. \n", buf, num, &buf, addr);
}

template <class dataType>
 void wvm(LPVOID addr, ULONG num, dataType *buf)
{
	DWORD oldProtect; NTSTATUS status;
	VirtualProtectEx(hProcess, addr, num, PAGE_EXECUTE_READWRITE, &oldProtect);
	if (!NT_SUCCESS(status = NtWriteVirtualMemory(hProcess, addr, buf, num, NULL)))
		printf("WVM error %02X at %08x \n", status, addr);
	VirtualProtectEx(hProcess, addr, num, oldProtect, NULL);
}


 LPVOID SpyInject(void * from, void * to)
{
	BYTE ret = 0xC3, push = 0x68;

	size_t funcsize = 0;
	unsigned char *p = (unsigned char *)from;
	for (funcsize = 0; funcsize < 1024; ++funcsize)
			if (p[funcsize] == (int)0xCC && p[funcsize + 1] == (int)0xCC && p[funcsize + 2] == (int)0xCC && p[funcsize + 3] == (int)0xCC)
				break;

	LPVOID Cave = VirtualAllocEx(hProcess, NULL, funcsize + 8, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
#ifdef DEBUG
	cout << "Injecting our function (addr 0x" << hex << from << ", size 0x" << funcsize << ") to cave (addr 0x" << Cave << ")" << endl;
#endif
	WriteProcessMemory(hProcess, Cave, p, funcsize, 0);

	DWORD dBack = (DWORD)to + 0x5;
	WriteProcessMemory(hProcess, LPVOID((DWORD)Cave + funcsize), &push, sizeof(BYTE), NULL);
	WriteProcessMemory(hProcess, LPVOID((DWORD)Cave + funcsize + sizeof(BYTE)), &dBack, sizeof(DWORD), NULL);
	WriteProcessMemory(hProcess, LPVOID((DWORD)Cave + funcsize + sizeof(BYTE) + sizeof(DWORD)), &ret, sizeof(BYTE), NULL);
#ifdef DEBUG
	cout << "push & ret 0x" << hex << dBack << " at 0x" << (DWORD)Cave + funcsize << endl;
#endif
	return Cave;
}


 void SpyJmp(void * to, void * from, int nops) {

	DWORD dCave = 0xFFFFFFFF - ((DWORD)to + 0x4 - (DWORD)from);
	DWORD oldProtect = 0;
	BYTE *jumpbytes = new BYTE[5 + nops];
	jumpbytes[0] = 0xE9;
	memcpy(jumpbytes+1, &dCave, sizeof(DWORD));
	for (int i = 0; i < nops; i++)
		jumpbytes[5 + i] = 0x90;
	wvm(LPVOID(to), 5 + nops, jumpbytes);
	delete[] jumpbytes;

#ifdef DEBUG
	cout << "jump performed from 0x" << hex << to << " to 0x" << from << endl;
#endif
}

 PVOID SpyInjectAndJump(void * from, void * to, int nops)
{
	size_t funcsize = 0;
	unsigned char *p = (unsigned char *)from;
	for (funcsize = 0; funcsize < 512; ++funcsize)
		if (p[funcsize] == (int)0xCC && p[funcsize + 1] == (int)0xCC && p[funcsize + 2] == (int)0xCC && p[funcsize + 3] == (int)0xCC)
				break;

	LPVOID Cave = VirtualAllocEx(hProcess, NULL, funcsize + 8, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

#ifdef DEBUG
	cout << "Injecting our function (addr 0x" << hex << from << ", size 0x" << funcsize << ") to cave (addr 0x" << Cave << ")" << endl;
#endif
	WriteProcessMemory(hProcess, Cave, p, funcsize, 0);

	DWORD dCave = 0xFFFFFFFF - ((DWORD)to + 0x4 - (DWORD)Cave);

	BYTE *jumpbytes = new BYTE[5 + nops];
	jumpbytes[0] = 0xE9;
	memcpy(jumpbytes + 1, &dCave, sizeof(DWORD));
	for (int i = 0; i < nops; i++)
		jumpbytes[5 + i] = 0x90;
	wvm(LPVOID(to), 5 + nops, jumpbytes);
	delete[] jumpbytes;

	DWORD dBack = (DWORD)to + 0x5;

	BYTE *pushbytes = new BYTE[6];
	pushbytes[0] = 0x68;
	pushbytes[5] = 0xC3;
	memcpy(pushbytes + 1, &dBack, sizeof(DWORD));
	wvm(LPVOID((DWORD)Cave+funcsize), 5 + nops, pushbytes);
	delete[] pushbytes;
	
	return Cave;
}

 BOOL ComparePattern(HANDLE pHandle, DWORD address, char *pattern, char *mask) {
	 DWORD patternSize = strlen(mask);
	 auto memBuf = new char[patternSize + 1];
	 memset(memBuf, 0, patternSize + 1);
	 ReadProcessMemory(pHandle, (LPVOID)address, memBuf, patternSize, 0);
	 for (DWORD i = 1; i < patternSize; i++) {
		 if (memBuf[i] != pattern[i] && mask[i] != *"?") {
			 delete memBuf;
			 return false;
		 }
	 }
	 delete memBuf;
	 return true;
 }

 DWORD ExternalAoBScan(HANDLE pHandle, DWORD moduleBase, char *pattern, char *mask) {

	 DWORD patternSize = strlen(mask);
	 DWORD moduleSize = 0x10000000;

	 auto moduleBytes = new char[moduleSize + 1];
	 memset(moduleBytes, 0, moduleSize + 1);
	 ReadProcessMemory(pHandle, (LPVOID)moduleBase, moduleBytes, moduleSize, 0);

	 for (int i = 0; i + patternSize < moduleSize; i++) {
		 if (pattern[0] == moduleBytes[i]) {
			 if (ComparePattern(pHandle, moduleBase + i, pattern, mask)) {
				 delete moduleBytes;
				 return moduleBase + i;
			 }
		 }
	 }
	 delete moduleBytes;
	 return NULL;
 }

 extern char tWindowName[256];
 extern DWORD engine_dll_base, gameui_dll_base, vgui2_dll_base, vguimatsurface_dll_base, d3d9_dll_base, nvd3dum_dll_base;

 HANDLE get_process_handle()
 {
	 HANDLE h = 0;
	 DWORD pid = 0;
	 HWND hWnd = FindWindow(0, _T(tWindowName));
	 if (hWnd == 0) {
#ifdef DEBUG
		 printf("FindWindow failed, %08X\n", GetLastError());
#endif
		 return h;
	 }
	 printf("hWnd = %08X\n", hWnd);
	 GetWindowThreadProcessId(hWnd, &pid);
	 h = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	 if (h == 0) {
#ifdef DEBUG
		 printf("OpenProcess failed, %08X\n", GetLastError());
#endif
		 return h;
	 }
#ifdef DEBUG
	 printf("pid = %d, process handle = %08X\n", pid, h);
#endif
	 HMODULE hMods[1024];
	 int i;
	 if (EnumProcessModules(h, hMods, sizeof(hMods), &pid) == FALSE) {
#ifdef DEBUG
		 printf("enumprocessmodules failed, %08X\n", GetLastError());
#endif
	 }

	 else {
		 for (i = 0; i < (pid / sizeof(HMODULE)); i++)
		 {
			 TCHAR szModName[MAX_PATH];
			 if (GetModuleFileNameEx(h, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
				 if (_tcsstr(szModName, _T("engine.dll")) != NULL) {
#ifdef DEBUG
					 printf("engine.dll base: %08X\n", hMods[i]);
#endif
					 engine_dll_base = (DWORD)hMods[i];
				 }

				 if (_tcsstr(szModName, _T("gameui.dll")) != NULL) {
#ifdef DEBUG
					 printf("gameui.dll base: %08X\n", hMods[i]);
#endif
					 gameui_dll_base = (DWORD)hMods[i];
				 }

				 if (_tcsstr(szModName, _T("vgui2.dll")) != NULL) {
#ifdef DEBUG
					 printf("vgui2.dll base: %08X\n", hMods[i]);
#endif
					 vgui2_dll_base = (DWORD)hMods[i];
				 }

				 if (_tcsstr(szModName, _T("vguimatsurface.dll")) != NULL) {
#ifdef DEBUG
					 printf("vguimatsurface.dll base: %08X\n", hMods[i]);
#endif
					 vguimatsurface_dll_base = (DWORD)hMods[i];
				 }

				 if (_tcsstr(szModName, _T("d3d9.dll")) != NULL) {
#ifdef DEBUG
					 printf("d3d9.dll base: %08X\n", hMods[i]);
#endif
					 d3d9_dll_base = (DWORD)hMods[i];
				 }

				 if (_tcsstr(szModName, _T("nvd3dum.dll")) != NULL) {
#ifdef DEBUG
					 printf("nvd3dum.dll base: %08X\n", hMods[i]);
#endif
					 nvd3dum_dll_base = (DWORD)hMods[i];
				 }
				 
			 }
		 }
	 }
	 return h;
 }

#endif
