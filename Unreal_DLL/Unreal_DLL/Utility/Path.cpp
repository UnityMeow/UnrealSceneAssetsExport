#ifdef DLL_EXPORT
#include "Path.h"
#include "FileUtility.h"
#include <io.h>
Path::Path(vengine::string const& path)
{
	operator=(path);
}

bool Path::operator==(Path const& a) const
{
	return pathData == a.pathData;
}
bool Path::operator!=(Path const& a) const
{
	return !operator==(a);
}

bool Path::Exists() const
{
	return _access(pathData.c_str(), 0) != -1;
}

Path& Path::operator=(vengine::string const& path)
{
	if (path.empty())
	{
		pathData = GetProgramPath().GetPathStr();
		return *this;
	}
	char* start = path.data();
	char* end = start + path.size();
	//Not Absolute 
	if (*start < 'A' || *start > 'Z' || start[1] != ':')
	{
		static Path selfPath = GetProgramPath();
		Path curPath = selfPath;
		vengine::vector<vengine::string> blocks;
		SeparatePath(path, blocks);
		for (auto ite = blocks.begin(); ite != blocks.end(); ++ite)
		{
			if (*ite == ".." || *ite == ".")
			{
				curPath = curPath.GetParentLevelPath();
			}
			else
			{
				curPath = curPath.GetSubLevelPath(*ite);
			}
		}
		pathData = curPath.pathData;
	}
	//Absolute Pos
	else
	{
		pathData = path;
	}
	start = pathData.data();
	end = start + pathData.size();
	for (char* i = start; i != end; ++i)
	{
		if (*i == '\\')
			*i = '/';
	}
	return *this;
}
Path& Path::operator=(Path const& v)
{
	auto&& path = v.GetPathStr();
	return operator=(path);
}
void Path::SeparatePath(vengine::string const& path, vengine::vector<vengine::string>& blocks)
{
	blocks.clear();
	char* start = path.data();
	char* end = start + path.size();
	char* i;
	for (i = start; i < end; ++i)
	{
		if (*i == '\\' || *i == '/')
		{
			blocks.emplace_back(start, i);
			start = i + 1;
		}
	}
	if (start < end)
	{
		blocks.emplace_back(start, end);
	}
}
Path Path::GetProgramPath()
{
	return Path(FileUtility::GetProgramPath());
}
Path Path::GetParentLevelPath() const
{
	char* start = pathData.data();
	char* end = start + pathData.size();
	for (char* i = end; i >= start; i--)
	{
		if (*i == '/' || *i == '\\')
		{
			return Path(vengine::string(start, i));
		}
	}
	return *this;
}
Path Path::GetSubLevelPath(vengine::string const& subName) const
{
	return Path(pathData + '/' + subName);
}
bool Path::IsFile() const
{
	char* start = pathData.data();
	char* end = start + pathData.size();
	for (char* i = end; i >= start; i--)
	{
		if (*i == '.')
			return true;
	}
	return false;

}
bool Path::IsDirectory() const
{
	return !IsFile();
}

vengine::string Path::GetExtension()
{
	char* start = pathData.data();
	char* end = start + pathData.size();
	for (char* i = end; i >= start; i--)
	{
		if (*i == '.')
			return vengine::string(i + 1, end);
	}
	return vengine::string();
}
#endif