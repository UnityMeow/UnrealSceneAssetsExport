#pragma once
#include "../Common/DLL.h"
#include "../Common/CommonInclude.h"
#include "../Common/vstring.h"
#include "../CJsonObject/CJsonObject.hpp"
#include "ExportType.h"

using namespace neb;

class DLL_CLASS MiJson
{
    CJsonObject json;
public:
    KILL_COPY_CONSTRUCT(MiJson)
    DECLARE_VENGINE_OVERRIDE_OPERATOR_NEW
    MiJson();
    MiJson(const vengine::string& path);
    ~MiJson();
    void Delete(const vengine::string& key);
    void Add(const vengine::string& key, int32_t* value, uint valueCount);
    void Add(const vengine::string& key, float* value, uint valueCount);
    void Add(const vengine::string& key, double* value, uint valueCount);
    void Add(const vengine::string& key, const vengine::string& value);
    void Add(const vengine::string& key, const CJsonObject& oJsonObject);
    // 添加数组-------------
    void Add(const CJsonObject& oJsonObject);
    void Add(float* value, uint valueCount);
    void Add(const vengine::string& value);
    // --------------------
    void Add(ExportTransform* tr);
    void Add(ExportRenderer* renderer);
    void Add(ExportSkinnedRenderer* renderer);
    void Add(ExportAnimClip* clip);
    void Add(ExportLight* light);
    void Add(ExportReflectionProbe* rp);
    void Add(ExportLandscape* Ls);
    uint64 GetJsonPtr();
    void ExportFile(vengine::string const& path);
    void ExportFileTest(vengine::string const& path);
};