#pragma once
#include "MiTool/Public/MiShader.h"
#include "VEngine_Include.h"

class MiShaderExport
{
	
	static int64 maxSize;
	static int32_t ShaderNum;
	static HashMap<vengine::string,std::unique_ptr<MiShader>> datas;
public:
	static bool b;
	MiShaderExport();
	~MiShaderExport();
	void static Export(const vengine::string& path);
	void static Import(const vengine::string& shaderName);
};
