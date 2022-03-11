#pragma once
#include "../IPreProcessor.h"
#include "ReflectionCompileData.h"
class MonitorEvent final : public IPreProcessor
{
public:
	void PreProcess(vengine::string& str) override
	{
		compiler->data->inStringState.Resize(str.size());
		bool isInString = false;
		int ranges[4] = { 0,0,0,0 };
		HashMap<vengine::string, vengine::vector<int>>::Iterator allItes[4];
		ReflectionCompileData* data = compiler->data;

		auto func = [data, &str](char const* ptr, HashMap<vengine::string, vengine::vector<int>>::Iterator& ite)->void
		{
			ite = data->ranges.Find(ptr);
			if (!ite) ite = data->ranges.Insert(ptr, 0);
			if (ite.Value().empty())
			{
				ite.Value().resize(str.size());
			}
		};
		func("{}", allItes[0]);
		func("<>", allItes[1]);
		func("()", allItes[2]);
		func("[]", allItes[3]);

		for (size_t i = 0; i < str.size(); ++i)
		{
			auto ptr = &str[i];
			if (*ptr == '\\')
			{
				i++;
				continue;
			}
			if (*ptr == '\"')
				isInString = !isInString;
			compiler->data->inStringState[i] = isInString;
			if (!isInString)
			{
				switch (*ptr)
				{
				case '{':
				{
					
					ranges[0]++;
				}
				break;
				case '}':
				{
				
					ranges[0]--;
				}
				break;
				case '<':
				{
					
					ranges[1]++;
				}
				break;
				case '>':
				{
					
					ranges[1]--;
				}
				break;
				case '(':
				{
					
					ranges[2]++;
				}
				break;
				case ')':
				{
					
					ranges[2]--;
				}
				break;
				case '[':
				{
					
					ranges[3]++;
				}
				break;
				case ']':
				{
					ranges[3]--;
				}
				break;
				}
			}
			for (size_t j = 0; j < 4; ++j)
			{
				allItes[j].Value()[i] = ranges[j];
			}
		}
	}
};