#include "MiTextureFileExport.h"
#include "../Utility/StringUtility.h"
#include "../Utility/FileUtility.h"

inline uint16_t FloatToHalf(float f)
{
	uint fltInt32 = *((uint*)&f);
	uint16_t fltInt16 = (fltInt32 >> 31) << 5;
	uint16_t tmp = (fltInt32 >> 23) & 0xff;
	tmp = (tmp - 0x70) & ((uint32_t)((int32_t)(0x70 - tmp) >> 4) >> 27);
	fltInt16 = (fltInt16 | tmp) << 10;
	fltInt16 |= (fltInt32 >> 13) & 0x3ff;
	return fltInt16;
}

void MiTextureFileExport::DataInit(uint64 headerSize, uint64 size, byte* dataPtr)
{
	teData.clear();
	teData.reserve(size + headerSize);
	teData.resize(headerSize);
	memcpy(teData.data(), dataPtr, headerSize);
}

void MiTextureFileExport::DataTransfer(uint2 blocks, byte* datas, TextureFileExportDataType type, uint passCount)
{
	uint curData;
	// 字节数
	uint64 typeCount;
	switch (type)
	{
	case TextureFileExportDataType::Bit8:
		typeCount = passCount;
		break;
	case TextureFileExportDataType::Bit16:
	case TextureFileExportDataType::Half:
		typeCount = (uint64_t)passCount * 2;
		break;
	case TextureFileExportDataType::Bit32:
		typeCount = (uint64_t)passCount * 4;
		break;
	}
	curData = Max<int64_t>(0, 256 - (int64_t)blocks.x * typeCount);

	vengine::vector<byte> b;
	for (uint y = 0, j = 0; y < blocks.y; ++y)
	{
		for (uint x = 0; x < blocks.x; ++x)
		{
			void* ptr = nullptr;
			// 指针偏移
			uint64 offset = teData.size();
			switch (type)
			{
			case TextureFileExportDataType::Bit8:
			{
				b.resize(typeCount);
				// 根据字节数开辟相应空间
				byte* tmp;
				float* datasPtrFloat = (float*)datas;
				for (uint i = 0; i < passCount; i++)
				{
					tmp = b.data() + i;
					*tmp = (byte)(datasPtrFloat[j * 4 + i] * 255);
				}
				ptr = b.data();
			}
			break;
			case TextureFileExportDataType::Bit16:
			{
				b.resize(typeCount);
				// 根据字节数开辟相应空间
				USHORT* tmp;
				float* datasPtrFloat = (float*)datas;
				for (uint i = 0; i < passCount; i++)
				{
					tmp = (USHORT*)(b.data() + ((uint64_t)i * 2));
					*tmp = (USHORT)(datasPtrFloat[j * 4 + i] * 65535);
				}
				ptr = b.data();
			}
			break;
			case TextureFileExportDataType::Half:
			{
				b.resize(typeCount);
				// 根据字节数开辟相应空间
				USHORT* tmp;
				float* datasPtrFloat = (float*)datas;
				for (uint i = 0; i < passCount; i++)
				{
					tmp = (USHORT*)(b.data() + ((uint64_t)i * 2));
					*tmp = FloatToHalf(datasPtrFloat[j * 4 + i]);
				}
				ptr = b.data();
			}
			break;
			case TextureFileExportDataType::Bit32:
				ptr = datas + j * typeCount;
				break;
			}

			teData.resize(teData.size() + typeCount);
			memcpy(teData.data() + offset, ptr, typeCount);

			j++;
		}
		teData.resize(teData.size() + curData);
	}
}

void MiTextureFileExport::DataTransfer(uint size, byte* datas, TextureFileExportDataType type)
{
	// 指针偏移
	uint64 offset = teData.size();
	switch (type)
	{
	case TextureFileExportDataType::Bit8:
	{
		uint* ptr = (uint*)datas;
		vengine::vector<byte> tmp(size);
		for (uint i = 0; i < size; i++)
		{
			tmp[i] = (byte)ptr[i];
		}
		teData.resize(teData.size() + size);
		memcpy(teData.data() + offset, tmp.data(), size);
	}
	break;
	case TextureFileExportDataType::Bit16:
	{
		uint* ptr = (uint*)datas;
		vengine::vector<USHORT> tmp(size);
		for (uint i = 0; i < size; i++)
		{
			tmp[i] = (USHORT)ptr[i];
		}
		teData.resize(teData.size() + (uint64)size * 2);
		memcpy(teData.data() + offset, tmp.data(), (uint64)size * 2);
	}
	break;
	case TextureFileExportDataType::Bit32:
		teData.resize(teData.size() + (uint64)size * 4);
		memcpy(teData.data() + offset, datas, (uint64)size * 4);
		break;
	}

}

void MiTextureFileExport::ExportFile(const vengine::string& path)
{
	std::ofstream ofs;
	FileUtility::AutoCreatFolder(path);
	ofs.open(path.c_str(), std::ofstream::binary);
	ofs.write((char*)teData.data(), teData.size());
	ofs.close();
}