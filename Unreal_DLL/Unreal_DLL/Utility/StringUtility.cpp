#ifdef DLL_EXPORT
#include "StringUtility.h"
#include <algorithm>

void StringUtil::IndicesOf(const vengine::string& str, const vengine::string& sign, vengine::vector<uint>& v)
{
	v.clear();
	if (str.empty()) return;
	int count = str.length() - sign.length() + 1;
	v.reserve(10);
	for (int i = 0; i < count; ++i)
	{
		bool success = true;
		for (int j = 0; j < sign.length(); ++j)
		{
			if (sign[j] != str[i + j])
			{
				success = false;
				break;
			}
		}
		if (success)
			v.push_back(i);
	}
}

bool StringUtil::ReadStringFromFile(vengine::string const& path, vengine::string& data)
{
	std::ifstream ifs(path.c_str());
	if (!ifs) return false;
	ifs.seekg(0, std::ios::end);
	auto sz = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	data.clear();
	data.resize(sz);
	memset(data.data(), 0, data.size());
	ifs.read(data.data(), data.size());
	for (int64_t sz = data.size() - 1; sz >= 0; --sz)
	{
		if (data[sz] != '\0')
		{
			data.resize(sz + 1);
			return true;
		}
	}
	return true;
}

void StringUtil::IndicesOf(const vengine::string& str, char sign, vengine::vector<uint>& v)
{
	v.clear();
	int count = str.length();
	v.reserve(10);
	for (int i = 0; i < count; ++i)
	{
		if (sign == str[i])
		{
			v.push_back(i);
		}
	}
}

void StringUtil::CutToLine(const char* str, int64_t size, vengine::vector<vengine::string>& lines)
{
	lines.clear();
	lines.reserve(32);
	vengine::string buffer;
	buffer.reserve(32);
	for (size_t i = 0; i < size; ++i)
	{
		auto&& value = str[i];
		if (value == '\0') break;
		if (!(value == '\n' || value == '\r'))
		{

			buffer.push_back(value);
		}
		else
		{
			if (value == '\n' || (value == '\r' && i < size - 1 && str[i + 1] == '\n'))
			{
				if (!buffer.empty())
					lines.push_back(buffer);
				buffer.clear();
			}
		}
	}
	if (!buffer.empty())
		lines.push_back(buffer);
}

void StringUtil::CutToLine(const vengine::string& str, vengine::vector<vengine::string>& lines)
{
	lines.clear();
	lines.reserve(32);
	vengine::string buffer;
	buffer.reserve(32);
	for (size_t i = 0; i < str.length(); ++i)
	{
		if (!(str[i] == '\n' || str[i] == '\r'))
		{
			buffer.push_back(str[i]);
		}
		else
		{
			if (str[i] == '\n' || (str[i] == '\r' && i < str.length() - 1 && str[i + 1] == '\n'))
			{
				if (!buffer.empty())
					lines.push_back(buffer);
				buffer.clear();
			}
		}
	}
	if (!buffer.empty())
		lines.push_back(buffer);
}

void StringUtil::ReadLines(std::ifstream& ifs, vengine::vector<vengine::string>& lines)
{
	ifs.seekg(0, std::ios::end);
	int64_t size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	vengine::vector<char> buffer(size);
	memset(buffer.data(), 0, size);
	ifs.read(buffer.data(), size);
	CutToLine(buffer.data(), size, lines);
}

int StringUtil::GetFirstIndexOf(const vengine::string& str, char sign)
{
	int count = str.length();
	for (int i = 0; i < count; ++i)
	{
		if (sign == str[i])
		{
			return i;
		}
	}
	return -1;
}

int StringUtil::GetFirstIndexOf(const vengine::string& str, const vengine::string& sign)
{
	int count = str.length() - sign.length() + 1;
	for (int i = 0; i < count; ++i)
	{
		bool success = true;
		for (int j = 0; j < sign.length(); ++j)
		{
			if (sign[j] != str[i + j])
			{
				success = false;
				break;
			}
		}
		if (success)
			return i;
	}
	return -1;
}

void StringUtil::Split(const vengine::string& str, char sign, vengine::vector<vengine::string>& v)
{
	vengine::vector<uint> indices;
	IndicesOf(str, sign, indices);
	v.clear();
	v.reserve(10);
	vengine::string s;
	s.reserve(str.size());
	int startPos = 0;
	for (auto index = indices.begin(); index != indices.end(); ++index)
	{
		s.clear();
		for (int i = startPos; i < *index; ++i)
		{
			s.push_back(str[i]);
		}
		startPos = *index + 1;
		if (!s.empty())
			v.push_back(s);
	}
	s.clear();
	for (int i = startPos; i < str.length(); ++i)
	{
		s.push_back(str[i]);
	}
	if (!s.empty())
		v.push_back(s);
}

void StringUtil::Split(const vengine::string& str, const vengine::string& sign, vengine::vector<vengine::string>& v)
{
	vengine::vector<uint> indices;
	IndicesOf(str, sign, indices);
	v.clear();
	v.reserve(10);
	vengine::string s;
	s.reserve(str.size());
	int startPos = 0;
	for (auto index = indices.begin(); index != indices.end(); ++index)
	{
		s.clear();
		for (int i = startPos; i < *index; ++i)
		{
			s.push_back(str[i]);
		}
		startPos = *index + sign.size();
		if (!s.empty())
			v.push_back(s);
	}
	s.clear();
	for (int i = startPos; i < str.length(); ++i)
	{
		s.push_back(str[i]);
	}
	if (!s.empty())
		v.push_back(s);
}
void StringUtil::GetDataFromAttribute(const vengine::string& str, vengine::string& result)
{
	int firstIndex = GetFirstIndexOf(str, '[');
	result.clear();
	if (firstIndex < 0) return;
	result.reserve(5);
	for (int i = firstIndex + 1; str[i] != ']' && i < str.length(); ++i)
	{
		result.push_back(str[i]);
	}
}

void StringUtil::GetDataFromBrackets(const vengine::string& str, vengine::string& result)
{
	int firstIndex = GetFirstIndexOf(str, '<');
	result.clear();
	if (firstIndex < 0) return;
	result.reserve(5);
	for (int i = firstIndex + 1; str[i] != '>' && i < str.length(); ++i)
	{
		result.push_back(str[i]);
	}
}

int StringUtil::StringToInteger(const vengine::string& str)
{
	if (str.empty()) return 0;
	if (str[0] == '-')
	{
		int value = 0;
		for (int i = 1; i < str.length(); ++i)
		{
			value *= 10;
			value += (int)str[i] - 48;
		}
		return value * -1;
	}
	else
	{
		int value = 0;
		for (int i = 0; i < str.length(); ++i)
		{
			value *= 10;
			value += (int)str[i] - 48;
		}
		return value;
	}
}
inline constexpr void mtolower(char& c)
{
	if ((c >= 'A') && (c <= 'Z'))
		c = c + ('a' - 'A');
}
inline constexpr void mtoupper(char& c)
{
	if ((c >= 'a') && (c <= 'z'))
		c = c + ('A' - 'a');
}

void StringUtil::ToLower(vengine::string& str)
{
	char* c = str.data();
	const uint size = str.length();
	for (uint i = 0; i < size; ++i)
	{
		mtolower(c[i]);
	}
}
void StringUtil::ToUpper(vengine::string& str)
{
	char* c = str.data();
	const uint size = str.length();
	for (uint i = 0; i < size; ++i)
	{
		mtoupper(c[i]);
	}
}

void StringUtil::ToString(double value, vengine::string& str)
{
	str.clear();
	uint32_t start = 0;
	if (value < 0)
	{
		str += '-';
		value = -value;
		start += 1;
	}
	uint64_t valueInt = (uint64_t)value;
	uint64_t valueOriginInt = valueInt;
	while (valueInt != 0)
	{
		uint32_t v = valueInt % 10;
		str += (char)(v + 48);
		valueInt /= 10;
	}
	for (int32_t i = start, j = str.size() - 1; i < j;)
	{
		auto v = str[i];
		str[i] = str[j];
		str[j] = v;
		i++;
		j--;
	}
	str += '.';
	double leftedPoints = value - valueOriginInt;
	for (uint32_t i = 0; i < 6; ++i)
	{
		leftedPoints *= 10;
		uint32_t a = leftedPoints;
		a = std::min<uint32_t>({ a, 9 });
		leftedPoints -= a;
		str += (char)(a + 48);
	}
}

void StringUtil::ToString(float value, vengine::string& str)
{
	str.clear();
	uint32_t start = 0;
	if (value < 0)
	{
		str += '-';
		value = -value;
		start += 1;
	}
	uint64_t valueInt = (uint64_t)value;
	uint64_t valueOriginInt = valueInt;
	if (valueInt == 0)
	{
		str += '0';
	}
	else
		while (valueInt != 0)
		{
			uint32_t v = valueInt % 10;
			str += (char)(v + 48);
			valueInt /= 10;
		}
	for (int32_t i = start, j = str.size() - 1; i < j;)
	{
		auto v = str[i];
		str[i] = str[j];
		str[j] = v;
		i++;
		j--;
	}
	str += '.';
	float leftedPoints = value - valueOriginInt;
	for (uint32_t i = 0; i < 6; ++i)
	{
		leftedPoints *= 10;
		uint32_t a = leftedPoints;
		a = std::min<uint32_t>({ a, 9 });
		leftedPoints -= a;
		str += (char)(a + 48);
	}
}

void StringUtil::SubString(const vengine::string& str, uint64_t startIndex, uint64_t size, vengine::string& result)
{
	//Debug Mode
#if defined(DEBUG) || defined(_DEBUG)
	if (startIndex + size > str.size())
		throw "Out of Range!";
#endif
	result.clear();
	result.resize(size);
	memcpy(result.data(), str.data() + startIndex * sizeof(char), size * sizeof(char));
}

const vengine::string StringUtil::WcharToVString(wchar_t* str)
{
	vengine::string vstr;
	uint len = WideCharToMultiByte(CP_ACP, 0, str, wcslen(str), NULL, 0, NULL, NULL);
	std::unique_ptr<char> m_char(new char[(uint64_t)len + 1]);
	WideCharToMultiByte(CP_ACP, 0, str, wcslen(str), m_char.get(), len, NULL, NULL);
	m_char.get()[len] = '\0';
	vstr = m_char.get();
	return vstr;
}

wchar_t* StringUtil::VStringToWchar(const vengine::string& str)
{
	const char* pszMultiByte = str.c_str();  //strlen(pwsUnicode)=5
	int iSize;
	wchar_t* wstr;

	//返回接受字符串所需缓冲区的大小，已经包含字符结尾符'\0'
	iSize = MultiByteToWideChar(CP_ACP, 0, pszMultiByte, -1, NULL, 0); //iSize =wcslen(pwsUnicode)+1=6
	wstr = (wchar_t*)malloc(iSize * sizeof(wchar_t)); //不需要 pwszUnicode = (wchar_t *)malloc((iSize+1)*sizeof(wchar_t))
	MultiByteToWideChar(CP_ACP, 0, pszMultiByte, -1, wstr, iSize);
	return wstr;
}

void StringUtil::ToString(int32_t value, vengine::string& str)
{
	str.clear();
	uint32_t start = 0;
	if (value < 0)
	{
		str += '-';
		value = -value;
		start += 1;
	}
	if (value == 0)
	{
		str += '0';
	}
	else
		while (value != 0)
		{
			uint32_t v = value % 10;
			str += (char)(v + 48);
			value /= 10;
		}
	for (int32_t i = start, j = str.size() - 1; i < j;)
	{
		auto v = str[i];
		str[i] = str[j];
		str[j] = v;
		i++;
		j--;
	}
}

#endif