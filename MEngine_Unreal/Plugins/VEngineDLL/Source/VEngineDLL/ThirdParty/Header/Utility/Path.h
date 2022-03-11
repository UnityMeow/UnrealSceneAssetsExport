#pragma once
#include "../Common/CommonInclude.h"
class DLL_CLASS Path
{
private:
	vengine::string pathData;
	static void SeparatePath(vengine::string const& path, vengine::vector<vengine::string>& blocks);
public:
	Path(vengine::string const& path);
	Path() {}
	static Path GetProgramPath();
	Path GetParentLevelPath() const;
	Path GetSubLevelPath(vengine::string const& subName) const;
	bool IsFile() const;
	bool IsDirectory() const;
	bool Exists() const;
	vengine::string const& GetPathStr() const
	{
		return pathData;
	}
	vengine::string GetExtension();
	Path& operator=(Path const& v);
	Path& operator=(vengine::string const& path);
	bool operator==(Path const& a) const;
	bool operator!=(Path const& a) const;
};
namespace vengine
{
	template <>
	struct hash<Path>
	{
		size_t operator()(Path const& p) const noexcept
		{
			static hash<vengine::string> vc;
			return vc(p.GetPathStr());
		}
	};
}