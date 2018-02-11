#include "PathHelper.h"
#include <Windows.h>

std::string PathHelper::asset_root = "";

/* 创建一个静态对象，确保 PathHelper 这个对象能够在任何时间使用 */
static PathHelper path_helper;


PathHelper::PathHelper()
{
	/* 设置目录名称 */
	asset_dirname = "Assets";
	model_dirname = "Model";
	shader_dirname = "Shader";

#ifdef EXPORT_PROGRAM
	std::string buffer;
	buffer.resize(1024);

	/* 获取执行程序全路径 */
	GetModuleFileNameA(0, &buffer[0], buffer.size());

	/* 获取执行文件所在目录 */
	std::string exe_path = buffer.substr(0, buffer.find_last_of('\\'));

	/* 设置资源路径 */
	this->SetAssetRoot(exe_path + "\\" + asset_dirname);
#else
	this->SetAssetRoot("G:\\MyProject\\C++\\MMDViewer\\Assets");
#endif
}

void PathHelper::SetAssetRoot(const std::string& path)
{
	asset_root = path;
}

std::string PathHelper::AssetFullPath(const std::string& path)
{
	return path_helper.asset_root + "\\" + path;
}

std::string PathHelper::ModelFullPath(const std::string& path)
{
	return path_helper.asset_root + "\\" + path_helper.model_dirname + "\\" + path;
}

std::string PathHelper::ShaderFullPath(const std::string& path)
{
	return path_helper.asset_root + "\\" + path_helper.shader_dirname + "\\" + path;
}

std::string PathHelper::FileName(const std::string& path)
{
	return path.substr(path.find_last_of('\\') + 1);
}

std::string PathHelper::FilePath(const std::string& path)
{
	return path.substr(0, path.find_last_of('\\'));
}