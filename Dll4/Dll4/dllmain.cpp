#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#define DllExport __declspec(dllexport)

HINSTANCE   hInstance = NULL;
ATOM        wclAtom = NULL;
HWND        hWnd = NULL;
HDC         hDC = NULL;
HGLRC       hRC = NULL;






LRESULT WINAPI DLLWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_DESTROY:
        //PostQuitMessage(0);
        break;

       

    default: 
        return DefWindowProc(hwnd, msg, wParam, lParam);


   
    }
    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        WNDCLASSEXA wcl;
        wcl.cbSize = sizeof(WNDCLASSEXA);
        wcl.style = CS_OWNDC;
        wcl.lpfnWndProc = DLLWindowProc;
        wcl.cbClsExtra = 0;
        wcl.cbWndExtra = 0;
        wcl.hInstance = hInstance;
        wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcl.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcl.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
        wcl.lpszMenuName = NULL;
        wcl.lpszClassName = "Title";
        wcl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

        wclAtom = RegisterClassExA(&wcl);
        break;
    case DLL_THREAD_ATTACH:
        hInstance = hModule;
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        //Shutdown();
        break;
    }
    return TRUE;
}










#pragma once


extern "C"
{

    DllExport int  Initalize() {
            if (hWnd != NULL)
        return 0;

            const char* title = "Title";


    if (!wclAtom)
    {
        //failed to register class
        return 1;
    }

     //creating window

    hWnd = CreateWindowExA(0,
        (LPCSTR)wclAtom,
        (LPCSTR)title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        512, 512,
        NULL, NULL,
        hInstance, NULL);

    if (hWnd == NULL)
    {

        //window creation failed

        return 2;
    }



  

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

  

    return 0;
}
    
    DllExport void Shutdown() {
       
        UnregisterClassA((LPCSTR)wclAtom, hInstance);
    
    }
  

};