#include "MiShaderExport.h"
int64 MiShaderExport::maxSize = 0;
int32_t MiShaderExport::ShaderNum = 0;
bool MiShaderExport::b = false;
HashMap<vengine::string,std::unique_ptr<MiShader>> MiShaderExport::datas;
MiShaderExport::MiShaderExport()
{
    maxSize = 0;
}

MiShaderExport::~MiShaderExport()
{
}

void MiShaderExport::Import(const vengine::string& shaderName)
{
    if(datas.Contains(shaderName))
    {
       return; 
    }
    
    std::unique_ptr<MiShader> shader(new MiShader(shaderName,ShaderNum++));
    
    if(shaderName == "BaseShader")
    {
        shader->Import("uvScale",ShaderDataType::MFloat2);
        shader->Import("uvOffset",ShaderDataType::MFloat2);
        shader->Import("albedo",ShaderDataType::MFloat4);
        shader->Import("clipalpha",ShaderDataType::MFloat);
        shader->Import("useclip",ShaderDataType::MFloat);
        shader->Import("metallic",ShaderDataType::MFloat);
        shader->Import("emission",ShaderDataType::MFloat4);
        shader->Import("smoothness",ShaderDataType::MFloat);
        shader->Import("occlusion",ShaderDataType::MFloat);
        shader->Import("albedoTexIndex");
        shader->Import("specularTexIndex");
        shader->Import("normalTexIndex");
        shader->Import("emissionTexIndex");
        shader->Import("clipTexIndex");
    }
    else if(shaderName == "RockShader")
    {
        shader->Import("DetailNormal_uv_scale",ShaderDataType::MFloat);
        shader->Import("Detailnormal_intesity",ShaderDataType::MFloat);
        shader->Import("DetailNormal_mask_intesity",ShaderDataType::MFloat);
        shader->Import("Normal_mask_intesity",ShaderDataType::MFloat);
        shader->Import("LUTcolorpower",ShaderDataType::MFloat);
        shader->Import("LUTselect",ShaderDataType::MFloat);
        shader->Import("dirtlerp",ShaderDataType::MFloat);
        shader->Import("roughness_max",ShaderDataType::MFloat);
        shader->Import("roughness_min",ShaderDataType::MFloat);
        shader->Import("roughness_mask_power_intesity",ShaderDataType::MFloat);
        shader->Import("AOlerp",ShaderDataType::MFloat);
        shader->Import("dirt_mask_power_intesity",ShaderDataType::MFloat);
        shader->Import("AO_mask_intesity",ShaderDataType::MFloat);
        shader->Import("dirtColor",ShaderDataType::MFloat4);
        shader->Import("NormalTexIndex");
        shader->Import("DetailNormalTexIndex");
        shader->Import("LUTColorTexIndex");
        shader->Import("NoiseTexIndex");
        shader->Import("UVOCTexIndex");
        shader->Import("TCVTexIndex");
    }
    datas.Insert(shaderName,std::move(shader));
}

void MiShaderExport::Export(const vengine::string& path)
{
    vengine::string exportCode;
    vengine::string guid = MGuid::GetGUID();
    MiJson exportJson;
    exportCode = "#ifndef M" + guid + "\n#define M" + guid + "\n\n";
    
    datas.IterateAll([&](uint64_t i,const vengine::string& str, std::unique_ptr<MiShader>& shaderPtr)->void
    {
        maxSize = shaderPtr->GetSize() >= maxSize ? shaderPtr->GetSize() : maxSize;
        vengine::string name = shaderPtr->GetName();
        StringUtil::ToUpper(name);
        exportCode += "#define " + name + "_INDEX " + std::to_string(shaderPtr->GetIndex()).c_str() +"\n";
    });
    
    datas.IterateAll([&](uint64_t i,const vengine::string& str, std::unique_ptr<MiShader>& shaderPtr)->void
    {
        shaderPtr->GetJson(exportJson);
        vengine::string code;
        shaderPtr->ExportCode(maxSize,code);
        exportCode += code;
    });

    exportCode += "\n#endif";
    FString a = StringUtil::VStringToWchar(exportCode);
    FileUtility::ExportTextFile(path + "Shader.hlsl",exportCode);
    exportJson.ExportFile(path + "Shader.json");
    exportJson.ExportFileTest(path + "Shader.json" + ".meowtest");
}

