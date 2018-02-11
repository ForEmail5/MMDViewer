#ifndef PATH_HELPER_H
#define PATH_HELPER_H

/* 导出程序的宏，表示资源路径和可执行程序在同一级目录，发布程序时可以正确检索资源路径 */
#define EXPORT_PROGRAM

#include <string>

class PathHelper
{
public:
	PathHelper();

	void SetAssetRoot(const std::string& path);

	static std::string AssetFullPath(const std::string& path);

	static std::string ModelFullPath(const std::string& path);

	static std::string ShaderFullPath(const std::string& path);

	static std::string FileName(const std::string& path);

	static std::string FilePath(const std::string& path);

private:
	static std::string asset_root;

	std::string asset_dirname;
	std::string model_dirname;
	std::string shader_dirname;
};

#endif