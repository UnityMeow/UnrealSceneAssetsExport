#pragma once
#define VENGINE_CDECL _cdecl
#ifdef DLL_EXPORT
#define EXPORT_DLL _declspec(dllexport)
#define EXTERN_DLLFUNC extern "C" _declspec(dllexport)
#else
#define EXPORT_DLL _declspec(dllimport)
#define EXTERN_DLLFUNC extern "C" _declspec(dllimport)
#endif