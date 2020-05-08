// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"

#define SIGNATURE 0xE85600000100EC81

#include <detours.h>
#include <Windows.h>

#define DRAWTEXTURE 0x466000//46FBD0
int (__cdecl* originalDraw)(int,int,int,float);

#define UIPOSEALIENHEALTH 0x466A90
int (__cdecl* originalUiPoseAlienHealth)(int,float,float,int);

#define UIPOSETEXTURE 0x453530
int (__cdecl* originalUiPoseTexture)(float,float,float,float,float,float,float,float,float,int);

#define UIPOSES 0x4533D0
int (__cdecl* originalUiPoseText)(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9);

typedef int func(void);
typedef int funcFloat(void);
typedef int funcBool(void);
func* getVidFiltering = (func*)0x452C20;
func* getVidFullscreen = (func*)0x452C10;
funcFloat* getVidTextureQuality = (funcFloat*)0x452C30;
funcBool* isDepth32bit = (funcBool*)0x452C40;


//This function is called when starting the game and loading a level to display bioware logo, interplay logo and loading texture
int hookedDraw(int,int,int,float)
{
	return 0;
}
//This function is called when giving an alienhealth texture a position on the screen
int hookedUiPoseAlienHealth(int,float,float,int)
{
	return 0;
}
//This function is called when applying a texture to a loading screen ,bioware logo or interplay logo
int hookedUiPoseTexture(float,float,float,float,float,float,float,float,float,int)
{
	return 0;
}

int hookedUiPoseText(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9)
{
	return 0;
}

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
		//DllInitializer(hModule);
		//originalDraw = (int(__cdecl*)(int,int,int,float))DetourFunction((PBYTE)DRAWTEXTURE, (PBYTE)hookedDraw); //Magic
		originalUiPoseAlienHealth = (int(__cdecl*)(int,float,float,int))DetourFunction((PBYTE)UIPOSEALIENHEALTH, (PBYTE)hookedUiPoseAlienHealth); //Magic
		originalUiPoseText = (int(__cdecl*)(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8, float a9))DetourFunction((PBYTE)UIPOSES, (PBYTE)hookedUiPoseText); //Magic
	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

