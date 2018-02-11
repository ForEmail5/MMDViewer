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

	/* 将宽字符（Unicode）转换为原生字符（GB2312） */
	std::string  UTF16ToNative(const std::wstring& ws);

	/* 将多字节字符（系统正在使用的 GB2312）转换为宽字符（Unicode） */
	std::wstring NativeToUTF16(const std::string& s);

	/* 将多字节字符（UTF-8）转换为宽字符（Unicode） */
	std::wstring UTF8ToUTF16(const std::string& s);

	/* 将宽字符（Unicode）转换为多字节字符（UTF-8） */
	std::string  UTF16ToUTF8(const std::wstring& ws);

	/* 将日文的 ShiftJIS 编码的多字节转换为 Unicode */
	std::wstring ShiftJISToUTF16String(const std::string& s);
}