#define _CRT_SECURE_NO_WARNINGS

#include "FileReader.h"
#include "MmdCommon.h"

#include <Windows.h>

namespace mmd
{
	FileReader::FileReader(const std::string& filename)
		: buffer(nullptr)
		, file_size(0)
		, file_cursor(0)
	{
		/* 只读方式打开二进制文件 */
		FILE* file = fopen(filename.c_str(), "rb");

		if ( file == nullptr ) return;

		/* 获取文件大小 */
		file_size = fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		if ( file_size == 0 ) return;

		/* 文件指针返回文件头 */
		fseek(file, 0, SEEK_SET);

		/* 为缓冲区分配内存 */
		buffer = ( char* ) malloc(sizeof( char ) * file_size);
		if ( buffer == nullptr )
		{
			fclose(file);
			return;
		}

		/* 读取文件内容到缓冲区 */
		fread(buffer, 1, file_size, file);
		fclose(file);
	}

	FileReader::~FileReader()
	{
		if ( buffer != nullptr )
		{
			free(buffer);
		}
	}

	void FileReader::Skip(int size)
	{
		if ( file_cursor + size < file_size )
		{
			file_cursor += size;
		}
	}

	int FileReader::GetRemainedLength()
	{
		return file_size - file_cursor;
	}

	int FileReader::ReadIndex(int byte_size)
	{
		if ( file_cursor + byte_size > file_size ) return -1;

		int result = -1;
		switch ( byte_size )
		{
		case 1: result = this->Read<uint8>(); break;
		case 2: result = this->Read<uint16>(); break;
		case 4: result = this->Read<uint32>(); break;
		default: return -1;
		}
		return result;
	}

	/* 读取字符串 */
	std::string FileReader::ReadStringA(size_t n)
	{
		static char tmp_buffer[256];
		memset(tmp_buffer, 0, 256);
		memcpy(tmp_buffer, &buffer[file_cursor], n);
		file_cursor += n;

		return std::string(tmp_buffer);
	}

	/* 读取 Unicode 编码的字符串 */
	std::wstring FileReader::ReadStringW(bool utf8)
	{
		int length = this->Read<int>();
		file_cursor += length;

		if ( utf8 )
		{
			return UTF8ToUTF16(std::string(( char* ) &buffer[file_cursor - length], length));
		}
		else
		{
			return std::wstring(( wchar_t* ) &buffer[file_cursor - length], length / sizeof(wchar_t));
		}
	}

	/* 
	** 将多字节字符（utf-8、GB2312 ......）转换为宽字符（Unicode），转换前需要
	** 使用 setlocal 函数设置正确的字符编码环境 
	*/
	static std::wstring s2ws(const std::string& s)
	{
		static wchar_t tmp_bufferw[256];
		memset(tmp_bufferw, 0, sizeof(tmp_bufferw));

		mbstowcs(&tmp_bufferw[0], s.c_str(), 256);
		return tmp_bufferw;
	}

	/*
	** 将宽字符（Unicode）转换为多字节字符（utf-8、GB2312 ......），转换前需要
	** 使用 setlocal 函数设置正确的字符编码环境
	*/
	static std::string ws2s(const std::wstring& ws)
	{
		size_t l = ws.size() * 4 + 1;
		std::string ts(l, 0);
		wcstombs(&ts[0], ws.c_str(), l);
		return std::string(ts.c_str());
	}

	/* 将宽字符（Unicode）转换为原生字符（GB2312） */
	std::string UTF16ToNative(const std::wstring& ws)
	{
		std::string c_locale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "");
		std::string s = ws2s(ws);
		setlocale(LC_ALL, c_locale.c_str());
		return s;
	}

	/* 将多字节字符（系统正在使用的 GB2312）转换为宽字符（Unicode） */
	std::wstring NativeToUTF16(const std::string& s)
	{
		std::string c_locale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "");
		std::wstring ws = s2ws(s);
		setlocale(LC_ALL, c_locale.c_str());
		return ws;
	}

	/* 将多字节字符（UTF-8）转换为宽字符（Unicode） */
	std::wstring UTF8ToUTF16(const std::string& s)
	{
		std::string c_locale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "C");
		std::wstring ws = s2ws(s);
		setlocale(LC_ALL, c_locale.c_str());
		return ws;
	}

	/* 将宽字符（Unicode）转换为多字节字符（UTF-8）.注意：字符长度过长会转换错误 */
	std::string UTF16ToUTF8(const std::wstring& ws)
	{
		static char utf8_buf[512];
		memset(utf8_buf, 0, 512);

		::WideCharToMultiByte(CP_UTF8, 0, &ws[0], ws.size(), utf8_buf, 512, NULL, NULL);

		return utf8_buf;
	}

	/* 将日文的 ShiftJIS 编码的多字节转换为 Unicode */
	std::wstring ShiftJISToUTF16String(const std::string& s)
	{
		std::string c_locale = setlocale(LC_ALL, NULL);
		/* 这里的字符编码不能设置为和系统的一样，因为中文的 GB2312 和日文的 SHIFT-JIS 编码
		   不一样。所以将字符编码设置为 "Japanese_Japan.932" */
		setlocale(LC_ALL, "Japanese_Japan.932");
		std::wstring ws = s2ws(s);
		setlocale(LC_ALL, c_locale.c_str());
		return ws;
	}
}