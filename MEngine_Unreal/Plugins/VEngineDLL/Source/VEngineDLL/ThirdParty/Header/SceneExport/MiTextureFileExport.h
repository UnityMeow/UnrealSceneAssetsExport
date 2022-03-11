#pragma once
#include "../Common/CommonInclude.h"

enum TextureFileExportDataType
{
	Bit8 = 0,
	Bit16 = 1,
	Bit32 = 2,
	Half = 3,
};

class DLL_CLASS MiTextureFileExport
{
	vengine::vector<byte> teData;
public:
	void DataInit(uint64 headerSize, uint64 size, byte* dataPtr);
	void DataTransfer(uint2 blocks, byte* datas, TextureFileExportDataType type, uint passCount);
	void DataTransfer(uint size, byte* datas, TextureFileExportDataType type);
	void ExportFile(const vengine::string& path);
};