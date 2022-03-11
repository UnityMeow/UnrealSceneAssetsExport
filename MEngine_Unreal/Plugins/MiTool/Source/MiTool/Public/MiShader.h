#pragma once
#include "VEngine_Include.h"
#include "SceneExport/MiJson.h"

enum class ShaderDataType : int32_t
{
	MFloat = 0,
    MFloat2,
    MFloat3,
    MFloat4,
    MInt,
    MInt2,
    MInt3,
    MInt4,
	MUInt,
	MUInt2,
	MUInt3,
	MUInt4,
};

struct ShaderData
{
	vengine::string name;
	ShaderDataType type;
	vengine::string typeName;
};


class MiShader
{
	vengine::vector<ShaderData> DataByte4;
	vengine::vector<ShaderData> DataByte2;
	vengine::vector<ShaderData> DataByte1;
	vengine::vector<vengine::string> Textures;
	int64 size;
	int32_t index;
	vengine::string shaderName;
public:
	MiShader(const vengine::string& name,int32_t mindex);
	~MiShader();
	int64 GetSize();
	int32_t GetIndex();
	vengine::string& GetName();
	void Import(const vengine::string& mname,ShaderDataType mtype);
	void Import(const vengine::string& texName);
	void ExportCode(int64 maxSize,vengine::string& code);
	void ExportJson(const vengine::string& path);
	void GetJson(MiJson& exportJson);
};
