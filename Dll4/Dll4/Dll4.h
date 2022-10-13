// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the DLL4_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// DLL4_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DLL4_EXPORTS
#define DLL4_API __declspec(dllexport)
#else
#define DLL4_API __declspec(dllimport)
#endif

// This class is exported from the dll
class DLL4_API CDll4 {
public:
	CDll4(void);
	// TODO: add your methods here.
};

extern DLL4_API int nDll4;

DLL4_API int fnDll4(void);
