/*******************************************************************************
 * Project:  neb
 * @file     CJsonObject.hpp
 * @brief    Json
 * @author   bwarliao
 * @date:    2014-7-16
 * @note
 * Modify history:
 ******************************************************************************/

#ifndef CJSONOBJECT_HPP_
#define CJSONOBJECT_HPP_
#include "../Common/vector.h"
#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <string>
#include <fstream>
#include <list>
#include "cJSON.h"
#include "../Common/HashMap.h"
#include "../Common/DLL.h"
#include "../Common/vstring.h"
template <typename T, bool b>
class StackObject;
namespace neb
{
	class DLL_CLASS CJsonObject
	{
	public:     // method of ordinary json object or json array
		DECLARE_VENGINE_OVERRIDE_OPERATOR_NEW
		CJsonObject();
		CJsonObject(const vengine::string& strJson);
		CJsonObject(const CJsonObject* pJsonObject);
		CJsonObject(const CJsonObject& oJsonObject);
		virtual ~CJsonObject();

		CJsonObject& operator=(const CJsonObject& oJsonObject);
		bool operator==(const CJsonObject& oJsonObject) const;
		bool Parse(const vengine::string& strJson);
		void Clear();
		bool IsEmpty() const;
		bool IsArray() const;
		vengine::string ToString() const;
		vengine::string ToFormattedString() const;
		const vengine::string& GetErrMsg() const
		{
			return(m_strErrMsg);
		}

	public:     // method of ordinary json object
		bool AddEmptySubObject(const vengine::string& strKey);
		bool AddEmptySubArray(const vengine::string& strKey);
		bool GetKey(vengine::string& strKey);
		void ResetTraversing();
		CJsonObject& operator[](const vengine::string& strKey);
		vengine::string operator()(const vengine::string& strKey) const;
		bool Get(const vengine::string& strKey, CJsonObject& oJsonObject) const;
		cJSON* Get(const vengine::string& strKey) const;
		bool Get(const vengine::string& strKey, vengine::string& strValue) const;
		bool Get(const vengine::string& strKey, int32& iValue) const;
		bool Get(const vengine::string& strKey, uint32& uiValue) const;
		bool Get(const vengine::string& strKey, int64& llValue) const;
		bool Get(const vengine::string& strKey, uint64& ullValue) const;
		bool Get(const vengine::string& strKey, bool& bValue) const;
		bool Get(const vengine::string& strKey, float& fValue) const;
		bool Get(const vengine::string& strKey, double& dValue) const;
		bool IsNull(const vengine::string& strKey) const;
		bool Add(const vengine::string& strKey, const CJsonObject& oJsonObject);
		bool Add(const vengine::string& strKey, const vengine::string& strValue);
		bool Add(const vengine::string& strKey, int32 iValue);
		bool Add(const vengine::string& strKey, uint32 uiValue);
		bool Add(const vengine::string& strKey, int64 llValue);
		bool Add(const vengine::string& strKey, uint64 ullValue);
		bool Add(const vengine::string& strKey, bool bValue, bool bValueAgain);
		bool Add(const vengine::string& strKey, float fValue);
		bool Add(const vengine::string& strKey, double dValue);
		bool AddNull(const vengine::string& strKey);    // add null like this:   "key":null
		bool Delete(const vengine::string& strKey);
		bool Replace(const vengine::string& strKey, const CJsonObject& oJsonObject);
		bool Replace(const vengine::string& strKey, const vengine::string& strValue);
		bool Replace(const vengine::string& strKey, int32 iValue);
		bool Replace(const vengine::string& strKey, uint32 uiValue);
		bool Replace(const vengine::string& strKey, int64 llValue);
		bool Replace(const vengine::string& strKey, uint64 ullValue);
		bool Replace(const vengine::string& strKey, bool bValue, bool bValueAgain);
		bool Replace(const vengine::string& strKey, float fValue);
		bool Replace(const vengine::string& strKey, double dValue);
		bool ReplaceWithNull(const vengine::string& strKey);    // replace value with null

	public:     // method of json array
		int GetArraySize();
		CJsonObject& operator[](uint32_t uiWhich);
		vengine::string operator()(uint32_t uiWhich) const;
		bool Get(int iWhich, CJsonObject& oJsonObject) const;
		cJSON* Get(int iWhich) const;
		bool Get(int iWhich, vengine::string& strValue) const;
		bool Get(int iWhich, int32& iValue) const;
		bool Get(int iWhich, uint32& uiValue) const;
		bool Get(int iWhich, int64& llValue) const;
		bool Get(int iWhich, uint64& ullValue) const;
		bool Get(int iWhich, bool& bValue) const;
		bool Get(int iWhich, float& fValue) const;
		bool Get(int iWhich, double& dValue) const;
		bool IsNull(int iWhich) const;
		bool Add(const CJsonObject& oJsonObject);
		bool Add(const vengine::string& strValue);
		bool Add(int32 iValue);
		bool Add(uint32 uiValue);
		bool Add(int64 llValue);
		bool Add(uint64 ullValue);
		bool Add(int iAnywhere, bool bValue);
		bool Add(float fValue);
		bool Add(double dValue);
		bool AddNull();   // add a null value
		bool AddAsFirst(const CJsonObject& oJsonObject);
		bool AddAsFirst(const vengine::string& strValue);
		bool AddAsFirst(int32 iValue);
		bool AddAsFirst(uint32 uiValue);
		bool AddAsFirst(int64 llValue);
		bool AddAsFirst(uint64 ullValue);
		bool AddAsFirst(int iAnywhere, bool bValue);
		bool AddAsFirst(float fValue);
		bool AddAsFirst(double dValue);
		bool AddNullAsFirst();     // add a null value
		bool Delete(int iWhich);
		bool Replace(int iWhich, const CJsonObject& oJsonObject);
		bool Replace(int iWhich, const vengine::string& strValue);
		bool Replace(int iWhich, int32 iValue);
		bool Replace(int iWhich, uint32 uiValue);
		bool Replace(int iWhich, int64 llValue);
		bool Replace(int iWhich, uint64 ullValue);
		bool Replace(int iWhich, bool bValue, bool bValueAgain);
		bool Replace(int iWhich, float fValue);
		bool Replace(int iWhich, double dValue);
		bool ReplaceWithNull(int iWhich);      // replace with a null value

	private:
		CJsonObject(cJSON* pJsonData);

	private:
		cJSON* m_pJsonData;
		cJSON* m_pExternJsonDataRef;
		cJSON* m_pKeyTravers;
		vengine::string m_strErrMsg;
		HashMap<uint32_t, CJsonObject*> m_mapJsonArrayRef;
		HashMap<vengine::string, CJsonObject*> m_mapJsonObjectRef;
		vengine::vector<uint32_t> deleteKeys;
	};

}

template <typename T>
struct JsonKeyValuePair
{
	const vengine::string& key;
	PureType_t<T>* value;
	JsonKeyValuePair(const vengine::string& str, PureType_t<T>* value) : value(value), key(str) {}
	JsonKeyValuePair(const vengine::string& str, PureType_t<T>& value) : value(&value), key(str) {}
};

template <typename ... Args>
constexpr void GetValuesFromJson(neb::CJsonObject* cjson, Args&& ... args)
{
	char c[] = { (cjson->Get(args.key, *args.value), 0)... };
}

DLL_CLASS bool ReadJson(const vengine::string& filePath, neb::CJsonObject* cjsonObj);
DLL_CLASS neb::CJsonObject* ReadJson(const vengine::string& filePath);
DLL_CLASS double GetDoubleFromChar(char* c, size_t t);
DLL_CLASS float GetFloatFromChar(char* c, size_t t);
DLL_CLASS int GetIntFromChar(char* c, size_t t);
//void  ReadJson(const vengine::string& filePath, StackObject<neb::CJsonObject, true>& placementPtr);
template <typename T>
void ReadStringToVector(char* cPtr, size_t t, T* vec)
{
	const size_t floatNum = sizeof(T) / sizeof(float);
	float* vecPointer = (float*)vec;
	size_t count = 0;
	size_t floatOffset = 0;
	char* start = cPtr;
	for (size_t i = 0; i < t; ++i)
	{
		char c = cPtr[i];
		if (c == ',')
		{
			if (floatOffset >= floatNum) return;
			vecPointer[floatOffset] = GetFloatFromChar(start, count);
			start = cPtr + i + 1;
			count = 0;
			floatOffset++;
		}
		else
		{
			count++;
		}
	}
	if (floatOffset >= floatNum) return;
	vecPointer[floatOffset] = GetFloatFromChar(start, count);
}

template <typename T>
void ReadStringToDoubleVector(char* cPtr, size_t t, T* vec)
{
	const size_t floatNum = sizeof(T) / sizeof(double);
	double* vecPointer = (double*)vec;
	size_t count = 0;
	size_t floatOffset = 0;
	char* start = cPtr;
	for (size_t i = 0; i < t; ++i)
	{
		char c = cPtr[i];
		if (c == ',')
		{
			if (floatOffset >= floatNum) return;
			vecPointer[floatOffset] = GetDoubleFromChar(start, count);
			start = cPtr + i + 1;
			count = 0;
			floatOffset++;
		}
		else
		{
			count++;
		}
	}
	if (floatOffset >= floatNum) return;
	vecPointer[floatOffset] = GetDoubleFromChar(start, count);
}

template <typename T>
void ReadStringToIntVector(char* cPtr, size_t t, T* vec)
{
	const size_t intNum = sizeof(T) / sizeof(int);
	int* vecPointer = (int*)vec;
	size_t count = 0;
	size_t floatOffset = 0;
	char* start = cPtr;
	for (size_t i = 0; i < t; ++i)
	{
		char c = cPtr[i];
		if (c == ',')
		{
			if (floatOffset >= intNum) return;
			vecPointer[floatOffset] = GetIntFromChar(start, count);
			start = cPtr + i + 1;
			count = 0;
			floatOffset++;
		}
		else
		{
			count++;
		}
	}
	if (floatOffset >= intNum) return;
	vecPointer[floatOffset] = GetIntFromChar(start, count);
}
#endif
