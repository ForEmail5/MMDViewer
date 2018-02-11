#define _CRT_SECURE_NO_WARNINGS
#include "Debug.h"

#include <cstdarg>
#include <fstream>
#include <Windows.h>

static Debug debug;

char Debug::buffer[Debug::buffer_size];			/* ×Ö·û»º³åÇø */
wchar_t Debug::wbuffer[Debug::buffer_size];		/* ¿í×Ö·û»º³åÇø */

Debug::Debug()
{
	filename = "debug.log";
	path = GetDefaultPath() + "\\" + filename;
}

void Debug::Log(char* format, ...)
{
	va_list ap;
	va_start(ap, format);

	vsprintf(buffer, format, ap);
	va_end(ap);
	sprintf(buffer, "%s\n", buffer);

	OutputDebugStringA(buffer);
}

void Debug::Log(wchar_t* format, ...)
{
	va_list ap;
	va_start(ap, format);

	wvsprintf(wbuffer, format, ap);
	va_end(ap);
	wsprintf(wbuffer, L"%s\n", wbuffer);

	OutputDebugStringW(wbuffer);
}

void Debug::Write(char* format, ...)
{
	va_list ap;
	va_start(ap, format);

	vsprintf(buffer, format, ap);
	va_end(ap);

	debug.WriteToFile(buffer);
}

void Debug::LogAndWrite(char* format, ...)
{
	va_list ap;
	va_start(ap, format);

	vsprintf(buffer, format, ap);
	va_end(ap);
	debug.WriteToFile(buffer);

	sprintf(buffer, "%s\n", buffer);
	OutputDebugStringA(buffer);
}

std::string Debug::Format(char* format, ...)
{
	va_list ap;
	va_start(ap, format);

	vsprintf(buffer, format, ap);
	va_end(ap);

	return buffer;
}

std::string Debug::GetDefaultPath()
{
	std::string directory;
	directory.resize(2048);

	GetModuleFileNameA(GetModuleHandle(0), &directory[0], directory.size());

	return directory.substr(0, directory.find_last_of('\\'));
}

void Debug::SetOutputPath(const std::string& path)
{
	debug.path = path + "\\" + debug.filename;
}

void Debug::WriteToFile(const std::string& text)
{
	static bool first = true;
	std::fstream out;

	try
	{
		if ( first )
		{
			first = false;
			out.open(debug.path.c_str(), std::ios::out);
		}
		else
		{
			out.open(debug.path.c_str(), std::ios::out | std::ios::app);
		}
		out << text << std::endl;
	}
	catch ( std::exception e )
	{
		Log("Faild to open file:  %s.", e.what());
	}
	out.close();
}