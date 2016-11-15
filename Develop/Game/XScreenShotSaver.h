#pragma once

class XScreenShotSaver
{
private:
	wstring	GetScreenShotFileNameByFormat(const wstring& strScreenshotFolderPath);

public:
	XScreenShotSaver() {}
	void Save(const wchar_t* szPath=NULL);
};