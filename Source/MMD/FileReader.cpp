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
		/* ֻ����ʽ�򿪶������ļ� */
		FILE* file = fopen(filename.c_str(), "rb");

		if ( file == nullptr ) return;

		/* ��ȡ�ļ���С */
		file_size = fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		if ( file_size == 0 ) return;

		/* �ļ�ָ�뷵���ļ�ͷ */
		fseek(file, 0, SEEK_SET);

		/* Ϊ�����������ڴ� */
		buffer = ( char* ) malloc(sizeof( char ) * file_size);
		if ( buffer == nullptr )
		{
			fclose(file);
			return;
		}

		/* ��ȡ�ļ����ݵ������� */
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

	/* ��ȡ�ַ��� */
	std::string FileReader::ReadStringA(size_t n)
	{
		static char tmp_buffer[256];
		memset(tmp_buffer, 0, 256);
		memcpy(tmp_buffer, &buffer[file_cursor], n);
		file_cursor += n;

		return std::string(tmp_buffer);
	}

	/* ��ȡ Unicode ������ַ��� */
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
	** �����ֽ��ַ���utf-8��GB2312 ......��ת��Ϊ���ַ���Unicode����ת��ǰ��Ҫ
	** ʹ�� setlocal ����������ȷ���ַ����뻷�� 
	*/
	static std::wstring s2ws(const std::string& s)
	{
		static wchar_t tmp_bufferw[256];
		memset(tmp_bufferw, 0, sizeof(tmp_bufferw));

		mbstowcs(&tmp_bufferw[0], s.c_str(), 256);
		return tmp_bufferw;
	}

	/*
	** �����ַ���Unicode��ת��Ϊ���ֽ��ַ���utf-8��GB2312 ......����ת��ǰ��Ҫ
	** ʹ�� setlocal ����������ȷ���ַ����뻷��
	*/
	static std::string ws2s(const std::wstring& ws)
	{
		size_t l = ws.size() * 4 + 1;
		std::string ts(l, 0);
		wcstombs(&ts[0], ws.c_str(), l);
		return std::string(ts.c_str());
	}

	/* �����ַ���Unicode��ת��Ϊԭ���ַ���GB2312�� */
	std::string UTF16ToNative(const std::wstring& ws)
	{
		std::string c_locale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "");
		std::string s = ws2s(ws);
		setlocale(LC_ALL, c_locale.c_str());
		return s;
	}

	/* �����ֽ��ַ���ϵͳ����ʹ�õ� GB2312��ת��Ϊ���ַ���Unicode�� */
	std::wstring NativeToUTF16(const std::string& s)
	{
		std::string c_locale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "");
		std::wstring ws = s2ws(s);
		setlocale(LC_ALL, c_locale.c_str());
		return ws;
	}

	/* �����ֽ��ַ���UTF-8��ת��Ϊ���ַ���Unicode�� */
	std::wstring UTF8ToUTF16(const std::string& s)
	{
		std::string c_locale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "C");
		std::wstring ws = s2ws(s);
		setlocale(LC_ALL, c_locale.c_str());
		return ws;
	}

	/* �����ַ���Unicode��ת��Ϊ���ֽ��ַ���UTF-8��.ע�⣺�ַ����ȹ�����ת������ */
	std::string UTF16ToUTF8(const std::wstring& ws)
	{
		static char utf8_buf[512];
		memset(utf8_buf, 0, 512);

		::WideCharToMultiByte(CP_UTF8, 0, &ws[0], ws.size(), utf8_buf, 512, NULL, NULL);

		return utf8_buf;
	}

	/* �����ĵ� ShiftJIS ����Ķ��ֽ�ת��Ϊ Unicode */
	std::wstring ShiftJISToUTF16String(const std::string& s)
	{
		std::string c_locale = setlocale(LC_ALL, NULL);
		/* ������ַ����벻������Ϊ��ϵͳ��һ������Ϊ���ĵ� GB2312 �����ĵ� SHIFT-JIS ����
		   ��һ�������Խ��ַ���������Ϊ "Japanese_Japan.932" */
		setlocale(LC_ALL, "Japanese_Japan.932");
		std::wstring ws = s2ws(s);
		setlocale(LC_ALL, c_locale.c_str());
		return ws;
	}
}