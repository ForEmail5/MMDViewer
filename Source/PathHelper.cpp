#include "PathHelper.h"
#include <Windows.h>

std::string PathHelper::asset_root = "";

/* ����һ����̬����ȷ�� PathHelper ��������ܹ����κ�ʱ��ʹ�� */
static PathHelper path_helper;


PathHelper::PathHelper()
{
	/* ����Ŀ¼���� */
	asset_dirname = "Assets";
	model_dirname = "Model";
	shader_dirname = "Shader";

#ifdef EXPORT_PROGRAM
	std::string buffer;
	buffer.resize(1024);

	/* ��ȡִ�г���ȫ·�� */
	GetModuleFileNameA(0, &buffer[0], buffer.size());

	/* ��ȡִ���ļ�����Ŀ¼ */
	std::string exe_path = buffer.substr(0, buffer.find_last_of('\\'));

	/* ������Դ·�� */
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