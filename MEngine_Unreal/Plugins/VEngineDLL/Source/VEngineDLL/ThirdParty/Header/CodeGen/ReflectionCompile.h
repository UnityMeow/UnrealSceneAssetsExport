#pragma once
#include "../Common/CommonInclude.h"
class ReflectionCompile
{
	vengine::vector<vengine::string> strLines;
	vengine::vector<vengine::string> includeStrs;
	vengine::vector<vengine::string> includePaths;
public:
	ReflectionCompile(
		vengine::string const& path
	);
	void Generate(
		vengine::vector<vengine::string> const& includePaths
	);
	void Output(
		vengine::string const& path);
};