#pragma once

#include <string>
#include <iostream>

namespace mmd
{
	class FileReader
	{
	public:
		FileReader(const std::string& filename);

		~FileReader();

		void Skip(int size);

		int GetRemainedLength();

		template<typename Type>
		Type Read()
		{
			if ( file_cursor + sizeof(Type) >= file_size ) return Type();
			Type* head = reinterpret_cast< Type* >(&buffer[file_cursor]);
			file_cursor += sizeof(Type);

			return *head;
		}

		int ReadIndex(int byte_size);

		std::string ReadStringA(size_t n);

		std::wstring ReadStringW(bool utf8 = false);

		void Reset() { file_cursor = 0; }

		bool HasData() { return buffer != nullptr; }

	private:
		char* buffer;
		int file_size;
		int file_cursor;
	};

	/* �����ַ���Unicode��ת��Ϊԭ���ַ���GB2312�� */
	std::string  UTF16ToNative(const std::wstring& ws);

	/* �����ֽ��ַ���ϵͳ����ʹ�õ� GB2312��ת��Ϊ���ַ���Unicode�� */
	std::wstring NativeToUTF16(const std::string& s);

	/* �����ֽ��ַ���UTF-8��ת��Ϊ���ַ���Unicode�� */
	std::wstring UTF8ToUTF16(const std::string& s);

	/* �����ַ���Unicode��ת��Ϊ���ֽ��ַ���UTF-8�� */
	std::string  UTF16ToUTF8(const std::wstring& ws);

	/* �����ĵ� ShiftJIS ����Ķ��ֽ�ת��Ϊ Unicode */
	std::wstring ShiftJISToUTF16String(const std::string& s);
}