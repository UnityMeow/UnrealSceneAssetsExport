#pragma once
#include <stdint.h>
#include "Common/DLL_compute.h"
#include "TextureCompressCommand.h"
using THandle = uint64_t;
class IRendererBase
{
public:
	virtual void VENGINE_CDECL AddTextureCompressTask(
		TexFormat format,
		float alphaWeight,
		uint32_t width,
		uint32_t height,
		uint32_t mipCount,
		char const* outputPath,
		void* dataPtr
	) = 0;
	virtual void VENGINE_CDECL AddTextureMipTask(
		uint32_t width,
		uint32_t height,
		uint32_t mipCount,
		void* dataPtr
	) = 0;
	virtual void VENGINE_CDECL ExecuteAllTask() = 0;
	virtual wchar_t const* VENGINE_CDECL GetLogInfo() = 0;
	virtual bool VENGINE_CDECL WaitAllTask() = 0;
	//virtual void 
};