#include "MiShader.h"

MiShader::MiShader(const vengine::string& name,int32_t mindex)
{
    shaderName = name;
    index = mindex;
    size = 0;
}

MiShader::~MiShader()
{
}

int64 MiShader::GetSize()
{
    return size;
}

int32_t MiShader::GetIndex()
{
    return index;
}

vengine::string& MiShader::GetName()
{
    return shaderName;
}

void MiShader::Import(const vengine::string& mname,ShaderDataType mtype)
{
    ShaderData data;
    data.name = mname;
    data.type = mtype;
    switch (mtype)
    {
    case ShaderDataType::MUInt:
        data.typeName = "uint";
        break;
    case ShaderDataType::MInt:
        data.typeName = "int";
        break;
    case ShaderDataType::MFloat:
        data.typeName = "float";
        break;
    case ShaderDataType::MUInt2:
        data.typeName = "uint2";
        break;
    case ShaderDataType::MInt2:
        data.typeName = "int2";
        break;
    case ShaderDataType::MFloat2:
        data.typeName = "float2";
        break;
    case ShaderDataType::MUInt4:
        data.typeName = "uint4";
        break;
    case ShaderDataType::MInt4:
        data.typeName = "int4";
        break;
    case ShaderDataType::MFloat4:
        data.typeName = "float4";
        break;
    }
    
    switch (mtype)
    {
    case ShaderDataType::MUInt:
    case ShaderDataType::MInt:
    case ShaderDataType::MFloat:
        DataByte1.push_back(data);
        size += 1;
        break;
    case ShaderDataType::MUInt2:
    case ShaderDataType::MInt2:
    case ShaderDataType::MFloat2:
        DataByte2.push_back(data);
        size += 2;
        break;
    case ShaderDataType::MUInt4:
    case ShaderDataType::MInt4:
    case ShaderDataType::MFloat4:
        DataByte4.push_back(data);
        size += 4;
        break;
    }
}
void MiShader::Import(const vengine::string& texName)
{
    Textures.push_back(texName);
}

void MiShader::ExportCode(int64 maxSize,vengine::string& code)
{
    int64 placeholderSize = maxSize - size;
    code = "struct " + shaderName + "\n{\n";

    for(int i = 0; i < DataByte4.size(); ++i)
    {
        code += "\t" + DataByte4[i].typeName + " " + DataByte4[i].name + ";\n";
    }
    for(int i = 0; i < DataByte2.size(); ++i)
    {
        code += "\t" + DataByte2[i].typeName + " " + DataByte2[i].name + ";\n";
    }
    for(int i = 0; i < DataByte1.size(); ++i)
    {
        code += "\t" + DataByte1[i].typeName + " " + DataByte1[i].name + ";\n";
    }

    code += "\tint texture_index;\n";
    
    if(placeholderSize > 0)
    {
        code += ("\tint placeholder["+ std::to_string(placeholderSize) + "];\n").c_str();
    }
    code += "};\n";
}

void MiShader::ExportJson(const vengine::string& path)
{
    // TODO:遍历数据表导出JSON
    MiJson members;
    MiJson textures;
    MiJson shaderJson;
    MiJson exportJson;

    for(int i = 0; i < DataByte4.size(); ++i)
    {
        members.Add(DataByte4[i].name,(int32_t*)&DataByte4[i].type,1);
    }
    for(int i = 0; i < DataByte2.size(); ++i)
    {
        members.Add(DataByte2[i].name,(int32_t*)&DataByte2[i].type,1);
    }
    for(int i = 0; i < DataByte1.size(); ++i)
    {
        members.Add(DataByte1[i].name,(int32_t*)&DataByte1[i].type,1);
    }
    ShaderDataType TIType = ShaderDataType::MInt;
    members.Add("texture_index",(int32_t*)&TIType,1);
    for(int i = 0; i < Textures.size(); ++i)
    {
        textures.Add(Textures[i]);
    }
    
    shaderJson.Add("Members",(CJsonObject*)members.GetJsonPtr());
    shaderJson.Add("Textures",(CJsonObject*)textures.GetJsonPtr());
    shaderJson.Add("Index",&index,1);
    exportJson.Add(shaderName,(CJsonObject*)shaderJson.GetJsonPtr());
    exportJson.ExportFile(path + shaderName + ".json");
}

void MiShader::GetJson(MiJson& exportJson)
{
    MiJson members;
    MiJson textures;
    MiJson shaderJson;
    

    for(int i = 0; i < DataByte4.size(); ++i)
    {
        members.Add(DataByte4[i].name,(int32_t*)&DataByte4[i].type,1);
    }
    for(int i = 0; i < DataByte2.size(); ++i)
    {
        members.Add(DataByte2[i].name,(int32_t*)&DataByte2[i].type,1);
    }
    for(int i = 0; i < DataByte1.size(); ++i)
    {
        members.Add(DataByte1[i].name,(int32_t*)&DataByte1[i].type,1);
    }
    ShaderDataType TIType = ShaderDataType::MInt;
    members.Add("texture_index",(int32_t*)&TIType,1);
    for(int i = 0; i < Textures.size(); ++i)
    {
        textures.Add(Textures[i]);
    }
    
    shaderJson.Add("Members",(CJsonObject*)members.GetJsonPtr());
    shaderJson.Add("Textures",(CJsonObject*)textures.GetJsonPtr());
    shaderJson.Add("Index",&index,1);
    exportJson.Add(shaderName,(CJsonObject*)shaderJson.GetJsonPtr());
}