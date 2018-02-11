#ifndef DIALOG_H
#define DIALOG_H

#include <string>
#include <ShlObj.h>
#include <Windows.h>

class Dialog
{
public:
	static std::string DGetOpenFileName(const char* title, const char* filter)
	{
		char buffer[MAX_PATH] = { 0 };

		OPENFILENAMEA ofn = { 0 };
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrTitle = title;
		ofn.lpstrFilter = filter;
		ofn.lpstrInitialDir = "C:\\";
		ofn.lpstrFile = buffer;
		ofn.nMaxFile = sizeof(buffer) / sizeof(buffer[0]);
		ofn.nFilterIndex = 0;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

		GetOpenFileNameA(&ofn);
		return buffer;
	}

	static std::string DGetOpenDirectory(const char* title)
	{
		char buffer[MAX_PATH] = { 0 };

		BROWSEINFOA bif = { 0 };
		bif.lpszTitle = title;
		bif.pszDisplayName = buffer;
		//bif.ulFlags = BIF_BROWSEINCLUDEFILES;
		bif.ulFlags = BIF_RETURNFSANCESTORS;

		if ( LPITEMIDLIST pil = SHBrowseForFolderA(&bif) )
		{
			SHGetPathFromIDListA(pil, buffer);
			return buffer;
		}
		return "";
	}
};

#endif