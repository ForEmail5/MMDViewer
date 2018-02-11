#ifndef PATH_HELPER_H
#define PATH_HELPER_H

/* ��������ĺ꣬��ʾ��Դ·���Ϳ�ִ�г�����ͬһ��Ŀ¼����������ʱ������ȷ������Դ·�� */
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