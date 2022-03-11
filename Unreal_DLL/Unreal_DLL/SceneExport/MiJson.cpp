#include "MiJson.h"
#include "../Utility/StringUtility.h"
#include "../Utility/FileUtility.h"
#include "../CodeGen/JsonCompiler.h"

MiJson::MiJson()
{
}

MiJson::MiJson(const vengine::string& path)
{
	//MessageBoxA(NULL, path.c_str(), "获取地址成功", MB_OK);
	ReadJson(path, (&json));
}

MiJson::~MiJson()
{

}

void MiJson::Delete(const vengine::string& key)
{
	(&json)->Delete(key);
}

void MiJson::Add(const vengine::string& key, int32_t* value, uint valueCount)
{
	vengine::string s;
	vengine::string valueStr;
	if (valueCount == 1)
	{
		(&json)->Add(key, value[0]);
	}
	else
	{
		for (int i = 0; i < valueCount; i++)
		{
			StringUtil::ToString(value[i], valueStr);
			s += valueStr;
			if (i < valueCount - 1)
				s += ",";
		}
		(&json)->Add(key, s);
	}
}

void MiJson::Add(const vengine::string& key, float* value, uint valueCount)
{
	std::string c = std::to_string(value[0]);
	vengine::string s;
	vengine::string valueStr;
	if (valueCount == 1)
	{
		(&json)->Add(key, value[0]);
	}
	else
	{
		for (int i = 0; i < valueCount; i++)
		{
			StringUtil::ToString(value[i], valueStr);
			s += valueStr;
			if (i < valueCount - 1)
				s += ",";
		}
	}
	(&json)->Add(key, s);
}

void MiJson::Add(const vengine::string& key, double* value, uint valueCount)
{
	vengine::string s;
	vengine::string valueStr;
	if (valueCount == 1)
	{
		(&json)->Add(key, value[0]);
	}
	else
	{
		for (int i = 0; i < valueCount; i++)
		{
			StringUtil::ToString(value[i], valueStr);
			s += valueStr;
			if (i < valueCount - 1)
				s += ",";
		}
		(&json)->Add(key, s);
	}
}

void MiJson::Add(const vengine::string& key, const vengine::string& value)
{
	(&json)->Add(key, value);
}

void MiJson::Add(const vengine::string& key, const CJsonObject& oJsonObject)
{
	(&json)->Add(key, oJsonObject);
}

void MiJson::Add(const CJsonObject& oJsonObject)
{
	(&json)->Add(oJsonObject);
}

void MiJson::Add(float* value, uint valueCount)
{
	std::string c = std::to_string(value[0]);
	vengine::string s;
	vengine::string valueStr;
	if (valueCount == 1)
	{
		(&json)->Add(value[0]);
	}
	else
	{
		for (int i = 0; i < valueCount; i++)
		{
			StringUtil::ToString(value[i], valueStr);
			s += valueStr;
			if (i < valueCount - 1)
				s += ",";
		}
		(&json)->Add(s);
	}
}

void MiJson::Add(const vengine::string& value)
{
	(&json)->Add(value);
}

void MiJson::Add(ExportTransform* tr)
{
	MiJson* njson = new MiJson;
	njson->Add("position", (float*)&(tr->position), 3);
	njson->Add("rotation", (float*)&(tr->rotation), 4);
	njson->Add("localscale", (float*)&(tr->localscale), 3);
	(&json)->Add("transform", (CJsonObject*)(njson->GetJsonPtr()));
	delete njson;
}

void MiJson::Add(ExportRenderer* renderer)
{
	MiJson* njson = new MiJson;

	njson->Add("meshCount", (int32_t*)&(renderer->meshCount), 1);
	MiJson* disJson = new MiJson;
	MiJson* guidJson = new MiJson;
	for (int32_t i = 0; i < renderer->meshCount; i++)
	{
		disJson->Add((float*)&(renderer->meshDis[i]), 1);
		guidJson->Add(renderer->meshGUID[i]);
	}
	njson->Add("meshDis", (CJsonObject*)(disJson->GetJsonPtr()));
	njson->Add("meshGUID", (CJsonObject*)(guidJson->GetJsonPtr()));
	delete disJson;
	delete guidJson;

	MiJson* matJson = new MiJson;
	for (int32_t i = 0; i < renderer->matCount; i++)
	{
		matJson->Add(renderer->matGUID[i]);
	}
	njson->Add("material", (CJsonObject*)(matJson->GetJsonPtr()));
	delete matJson;

	njson->Add("mask", (int32_t*)&(renderer->mask), 1);
	(&json)->Add("gpu_rend", (CJsonObject*)(njson->GetJsonPtr()));

	delete njson;
}

void MiJson::Add(ExportSkinnedRenderer* renderer)
{
	MiJson* njson = new MiJson;

	njson->Add("meshCount", (int32_t*)&(renderer->meshCount), 1);
	MiJson* disJson = new MiJson;
	MiJson* guidJson = new MiJson;
	for (int32_t i = 0; i < renderer->meshCount; i++)
	{
		disJson->Add((float*)&(renderer->meshDis[i]), 1);
		guidJson->Add(renderer->meshGUID[i]);
	}
	njson->Add("meshDis", (CJsonObject*)(disJson->GetJsonPtr()));
	njson->Add("meshGUID", (CJsonObject*)(guidJson->GetJsonPtr()));
	delete disJson;
	delete guidJson;

	MiJson* matJson = new MiJson;
	for (int32_t i = 0; i < renderer->matCount; i++)
	{
		matJson->Add(renderer->matGUID[i]);
	}
	njson->Add("material", (CJsonObject*)(matJson->GetJsonPtr()));
	delete matJson;

	njson->Add("mask", (int32_t*)&(renderer->mask), 1);
	(&json)->Add("skinned", (CJsonObject*)(njson->GetJsonPtr()));

	delete njson;
}

void MiJson::Add(ExportAnimClip* clip)
{
	MiJson* njson = new MiJson;

	njson->Add("clipCount", (int32_t*)&(clip->clipCount), 1);
	MiJson* nameJson = new MiJson;
	MiJson* guidJson = new MiJson;
	for (int32_t i = 0; i < clip->clipCount; i++)
	{
		nameJson->Add(clip->clipName[i]);
		guidJson->Add(clip->clipGUID[i]);
	}
	njson->Add("clipName", (CJsonObject*)(nameJson->GetJsonPtr()));
	njson->Add("clipGUID", (CJsonObject*)(guidJson->GetJsonPtr()));
	delete nameJson;
	delete guidJson;
	(&json)->Add("animClip", (CJsonObject*)(njson->GetJsonPtr()));

	delete njson;
}

void MiJson::Add(ExportLight* light)
{
	MiJson* njson = new MiJson;
	njson->Add("lightType", &light->lightType, 1);
	njson->Add("intensity", &light->intensity, 1);
	njson->Add("shadowBias", &light->shadowBias, 1);
	njson->Add("shadowNearPlane", &light->shadowNearPlane, 1);
	njson->Add("range", &light->range, 1);
	njson->Add("color", (float*)&(light->color), 4);
	njson->Add("smallAngle", &light->smallAngle, 1);
	njson->Add("useShadow", &light->useShadow, 1);
	njson->Add("shadowCache", &light->shadowCache, 1);
	(&json)->Add("light", (CJsonObject*)(njson->GetJsonPtr()));
	delete njson;
}


void MiJson::Add(ExportReflectionProbe* rp)
{
	MiJson* njson = new MiJson;
	njson->Add("blendDistance", &rp->blendDistance, 1);
	njson->Add("color", &rp->blendDistance, 1);
	njson->Add("localPosition", (float*)&(rp->localPosition), 3);
	njson->Add("boxProjection", &rp->boxProjection, 1);
	njson->Add("localExtent", (float*)&(rp->localExtent), 3);
	njson->Add("cubemap", rp->cubemapGUID);
	(&json)->Add("rp", (CJsonObject*)(njson->GetJsonPtr()));
	delete njson;
}

void MiJson::Add(ExportLandscape* ls)
{
	MiJson* njson = new MiJson;
	njson->Add("heightMap", ls->heightMap);

	MiJson* guidJson = new MiJson;
	for (int32_t i = 0; i < ls->weightMapCount; i++)
	{
		guidJson->Add(ls->weightMaps[i]);
	}
	njson->Add("weightMaps", (CJsonObject*)(guidJson->GetJsonPtr()));
	(&json)->Add("landscape", (CJsonObject*)(njson->GetJsonPtr()));
	delete guidJson;
	delete njson;
}

uint64 MiJson::GetJsonPtr()
{
	return (uint64)(&json);
}

void MiJson::ExportFile(vengine::string const& path)
{
	vengine::vector<char> data;
	JsonCompiler::Serialize(json, data);

	std::ofstream ofs;
	FileUtility::AutoCreatFolder(path);
	ofs.open(path.c_str(), std::ofstream::binary);
	ofs.write(data.data(), data.size());
	ofs.close();
	/*std::ofstream ofs;
	FileUtility::AutoCreatFolder(path);
	ofs.open(path.c_str(), std::ofstream::out);
	vengine::string str = (&json)->ToString();
	ofs << str;
	ofs.close();*/
}
void MiJson::ExportFileTest(vengine::string const& path)
{
	std::ofstream ofs;
	FileUtility::AutoCreatFolder(path);
	ofs.open(path.c_str(), std::ofstream::out);
	vengine::string str = (&json)->ToString();
	ofs << str;
	ofs.close();
}