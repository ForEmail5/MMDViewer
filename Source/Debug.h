#ifndef DEBUG_H
#define DEBUG_H

#include <string>

class Debug
{
public:
	Debug();

	static void Log(char* format, ...);

	static void Log(wchar_t* format, ...);

	static void Write(char* format, ...);

	static void LogAndWrite(char* format, ...);

	static void SetOutputPath(const std::string& path);

	static std::string Format(char* format, ...);

	static std::string GetDefaultPath();

private:
	void WriteToFile(const std::string& text);

private:
	std::string path;
	std::string filename;

	static const int buffer_size = 2048;
	static char buffer[buffer_size];
	static wchar_t wbuffer[buffer_size];
};


#define LOG_STRING "%s { %s } { line:%d } \n\t Error: %s"

#define FORMAT(format, ...) Debug::Format(format, ##__VA_ARGS__).c_str()

#define LOG(format, ...) Debug::Log(format, ##__VA_ARGS__)

#define WRITE(format, ...) Debug::Write(format, ##__VA_ARGS__)

#define LOG_DEBUG(format, ...) Debug::Log(LOG_STRING, __FILE__, __FUNCTION__, __LINE__, FORMAT(format, ##__VA_ARGS__))

#define WRITE_DEBUG(format, ...) Debug::Write(LOG_STRING, __FILE__, __FUNCTION__, __LINE__, FORMAT(format, ##__VA_ARGS__))


#endif