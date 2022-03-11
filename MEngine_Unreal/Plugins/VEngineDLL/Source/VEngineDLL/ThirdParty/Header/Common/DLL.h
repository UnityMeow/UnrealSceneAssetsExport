#pragma once
#ifdef DLL_EXPORT
#if defined(DEBUG) || defined(_DEBUG)
#define DLL_CLASS 
#else
#define DLL_CLASS __declspec(dllexport)
#endif
#define DLL_FUNC extern"C" __declspec(dllexport) 
#else
#define DLL_CLASS __declspec(dllimport)
#define DLL_FUNC
#endif