// Dll4.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "Dll4.h"


// This is an example of an exported variable
DLL4_API int nDll4=0;

// This is an example of an exported function.
DLL4_API int fnDll4(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CDll4::CDll4()
{
    return;
}
