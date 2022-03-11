#ifdef DLL_EXPORT
#include "FileUtility.h"
#include <io.h>
#include <stdio.h>
#include <direct.h>
#include "StringUtility.h"
void FileUtility::GetFiles(vengine::string const& path, vengine::vector<vengine::string>& files, HashMap<vengine::string, bool> const& ignoreFolders)
{
	//文件句柄
	uint64 hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;

	if ((hFile = _findfirst((path + "\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (ignoreFolders.Contains(fileinfo.name)) continue;
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetFiles(path + ("\\") + (fileinfo.name), files, ignoreFolders);
			}
			else
			{
				files.push_back(path + ("\\") + (fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void FileUtility::GetTrivialFiles(vengine::string const& path, vengine::vector<vengine::string>& files)
{
	//文件句柄
	uint64 hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;

	if ((hFile = _findfirst((path + "\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				files.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

vengine::string FileUtility::GetProgramPath()
{
	vengine::string str;
	str.resize(4096);
	_getcwd(str.data(), str.size());
	str.resize(strlen(str.data()));
	return str;
}

void FileUtility::GetFolders(vengine::vector<vengine::string>& files)
{
	auto str = GetProgramPath();
	str += "\\*";
	uint64 hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;

	if ((hFile = _findfirst(str.c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (fileinfo.name[0] != '.')
					files.push_back(fileinfo.name);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}

}

void FileUtility::AutoCreatFolder(const vengine::string& path)
{
	vengine::vector<uint> index;
	vengine::string fPath;
	StringUtil::IndicesOf(path, '/', index);
	for (uint i = 0; i < index.size(); ++i)
	{
		StringUtil::SubString(path, 0, (int64_t)index[i] + 1, fPath);
		// 判断是否有这个文件夹 没有就新建一个
		if (_access(fPath.c_str(), 6) == -1)
		{
			_mkdir(fPath.c_str());
		}
	}
}

void FileUtility::ExportTextFile(const vengine::string& path, const vengine::string& content)
{
	AutoCreatFolder(path);
	std::ofstream out(path.c_str(), std::ios::out);
	out << content;
	out.close();
}
#endif