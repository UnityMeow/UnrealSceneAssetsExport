#pragma once
#include <stdint.h>
enum class TexFormat : uint32_t
{
	BC6H,
	BC7
};
struct TextureCompressCommand
{
	TexFormat format;
	uint32_t width;
	uint32_t height;
	uint32_t mipCount;
	float alphaWeight;
	char const* outputPath;
	void* dataPtr;
};

struct GenerateMipCommand
{
	uint32_t width;
	uint32_t height;
	uint32_t mipCount;
	void* dataPtr;

};