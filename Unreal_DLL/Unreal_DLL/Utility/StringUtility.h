#pragma once
#include "../Common/vector.h"
#include "../Common/CommonInclude.h"
#include <fstream>
#include "../Common/vstring.h"
#include <WinNls.h>
typedef unsigned int uint;
class DLL_CLASS StringUtil
{
private:
	StringUtil() = delete;
	KILL_COPY_CONSTRUCT(StringUtil)
public:
	static void IndicesOf(const vengine::string& str, const vengine::string& sign, vengine::vector<uint>& v);
	static void IndicesOf(const vengine::string& str, char, vengine::vector<uint>& v);
	static void CutToLine(const vengine::string& str, vengine::vector<vengine::string>& lines);
	static void CutToLine(const char* str, int64_t size, vengine::vector<vengine::string>& lines);
	static void ReadLines(std::ifstream& ifs, vengine::vector<vengine::string>& lines);
	static int GetFirstIndexOf(const vengine::string& str, const vengine::string& sign);
	static int GetFirstIndexOf(const vengine::string& str, char sign);
	static void Split(const vengine::string& str, const vengine::string& sign, vengine::vector<vengine::string>& v);
	static void Split(const vengine::string& str, char sign, vengine::vector<vengine::string>& v);
	static void GetDataFromAttribute(const vengine::string& str, vengine::string& result);
	static void GetDataFromBrackets(const vengine::string& str, vengine::string& result);
	static int StringToInteger(const vengine::string& str);
	static void ToLower(vengine::string& str);
	static void ToUpper(vengine::string& str);
	/// <summary>
	/// int转字符串
	/// </summary>
	static void ToString(int32_t value, vengine::string& str);
	/// <summary>
	/// double转字符串
	/// </summary>
	static void ToString(double value, vengine::string& str);
	/// <summary>
	/// float转字符串
	/// </summary>
	static void ToString(float value, vengine::string& str);
	/// <summary>
	/// 截取字符串
	/// 从startIndex截到第N位  result:startIndex - size字符串 
	/// </summary>
	static void SubString(const vengine::string& str, uint64_t startIndex, uint64_t size, vengine::string& result);
	/// <summary>
	/// wchar_t转vstring
	/// </summary>
	static const vengine::string WcharToVString(wchar_t* str);
	/// <summary>
	/// vstring转wchar_t
	/// </summary>
	static wchar_t* VStringToWchar(const vengine::string& str);
	/// <summary>
	/// 从文件读取文本
	/// </summary>
	/// <param name="path"></param>
	/// <param name="data"></param>
	/// <returns></returns>
	static bool ReadStringFromFile(vengine::string const& path, vengine::string& data);
};