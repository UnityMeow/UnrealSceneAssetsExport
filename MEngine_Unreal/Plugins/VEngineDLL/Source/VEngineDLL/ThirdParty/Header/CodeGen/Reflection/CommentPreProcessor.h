#pragma once
#include "../IPreProcessor.h"
#include "../../Common/CommonInclude.h"
#include "../../Utility/StringUtility.h"
class CommentPreProcessor : public IPreProcessor
{
public:
	void PreProcess(vengine::string& str) override
	{
		vengine::vector<char> newStr;
		newStr.reserve(str.size());
		char const* c = str.data();
		char const* endPtr = c + str.size();
		char const* end = nullptr;
		bool inString = false;
		while (c < endPtr)
		{
			if (inString)
			{
				if (*c == '\"' && c != str.data() && c[-1] != '\\')
				{
					inString = false;
					newStr.push_back(*c);
					c++;
					continue;
				}
				newStr.push_back(*c);
				c++;
			}
			else
			{
				if (!end)
				{
					if (endPtr - c >= 2)
					{
						if (StringUtil::CompareCharArray<2>(c, "/*"))
						{
							end = "*/";
							c += 2;
							continue;
						}
						else if (StringUtil::CompareCharArray<2>(c, "//"))
						{
							end = "\n";
							c += 2;
							continue;
						}
						else if (*c == '\"' && c != str.data() && c[-1] != '\\')
						{
							inString = true;
						}
					}
					newStr.push_back(*c);
					c++;
					continue;
				}
				else
				{
					auto len = strlen(end);
					if (endPtr - c >= len &&
						StringUtil::CompareCharArray(c, end, len))
					{
						end = nullptr;
						c += len;
						newStr.push_back(' ');
					}
					else
						c++;
				}
			}
		}
		str.clear();
		str.resize(newStr.size());
		memcpy(str.data(), newStr.data(), newStr.size());
	}
};