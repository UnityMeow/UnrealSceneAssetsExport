#pragma once
#include "../Common/CommonInclude.h"
class DLL_CLASS FileUtility
{
private:
	FileUtility() = delete;
	~FileUtility() = delete;
public:
	static void GetFiles(vengine::string const& path, vengine::vector<vengine::string>& files, HashMap<vengine::string, bool> const& ignoreFolders);
	static void GetFolders(vengine::vector<vengine::string>& files);
	static void GetTrivialFiles(vengine::string const& path, vengine::vector<vengine::string>& files);
	static vengine::string GetProgramPath();
	/// <summary>
	/// 根据路径自动创建文件夹
	/// </summary>
	static void AutoCreatFolder(const vengine::string& path);
	/// <summary>
	/// 导出文本文件
	/// </summary>
	static void ExportTextFile(const vengine::string& path, const vengine::string& content);
};