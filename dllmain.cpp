// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"

#define SIGNATURE 0xE85600000100EC81

void DllInitializer(HMODULE hDllModule)
{
    // Проверка разположения нашей библиотеки 
    if (((UINT64)hDllModule) < 0x7E000000 && ((UINT64)(&hGame)) < 0x7F000000)
    {
        // Провека сигнатуры                   
        if (*((UINT64*)0x401000) == SIGNATURE) 
        {
            // Открываем просесс игры
            if ((hGame = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, GetCurrentProcessId())))
            {
                WriteMemoryWORD(0x006B5B17, 0x026A);//Изменить функцию __fptrap чтобы она делала return и не кидала ошибку.
				BYTE bytes[] = {0xC7,0x05,0x40,0x5E,0x5C,0x00,0x01,0x00,0x00,0x00};
				WriteMemoryBYTES(0x004385C6, bytes, 10);
                CloseHandle(hGame);
                hGame = NULL;
            }
            else
            {
                Msg(_T("Ошибка при загрузке"), _T("[%s]\n не был выдан доступ к игре"), __WFILE__);
                ExitProcess(1);
            }
        }
        else
        {
            Msg(_T("Ошибка при загрузке"), _T("[%s]\n Неправильная сигнатура"), __WFILE__);
            ExitProcess(1);
        }
    }
    else
    {
        Msg(_T("Ошибка при загрузке"), _T("[%s]\n Плохое размещение DLL в памяти"), __WFILE__);
        ExitProcess(1);
    }
}  

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DllInitializer(hModule);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

