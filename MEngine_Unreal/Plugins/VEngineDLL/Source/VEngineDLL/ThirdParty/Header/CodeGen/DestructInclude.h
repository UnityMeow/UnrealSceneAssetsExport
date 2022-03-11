#pragma once
#include "../Common/CommonInclude.h"
#include "../File/Path.h"
struct DestructInclude
{
private:
	static bool ReadFile(Path const& path, HashMap<Path, bool>& exclusiveMap, bool pragmaOnce, vengine::string& codeDest);
public:
	static bool ReadFile(Path const& originPath, vengine::string& codeDest);
	KILL_COPY_CONSTRUCT(DestructInclude)
};