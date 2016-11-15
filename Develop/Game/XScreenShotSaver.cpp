#include "stdafx.h"
#include "XScreenShotSaver.h"
#include "XBaseApplication.h"
#include "XPath.h"
#include "XStrings.h"

void XScreenShotSaver::Save(const wchar_t* path)
{
	static unsigned long int st_nLastTime = 0;
	unsigned long int nNowTime = XGetNowTime();
	const uint32 SCREENSHOT_DELAY = 1000;		// 저장 딜레이

	// 딜레이
	if ((nNowTime-st_nLastTime) < SCREENSHOT_DELAY)	return;
	st_nLastTime = nNowTime;

	if (XPath::CreateDirectoryInMyDocuments(PATH_MYDOC_SCREEN_SHOT) == false)
	{
		return;
	}

	wstring strScreenshotFolderPath = XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_SCREEN_SHOT);
	wstring strScreenshotFileName = GetScreenShotFileNameByFormat(strScreenshotFolderPath);

	if (strScreenshotFileName.empty() == false)
	{
		global.device->SaveScreenShot( MLocale::ConvUTF16ToAnsi(strScreenshotFileName).c_str());
		global.ui->OnSystemMsg( XGetStr( L"MSG_SAVEDSCREENSHOT"), FSParam(strScreenshotFileName));
	}

	return;
}

wstring XScreenShotSaver::GetScreenShotFileNameByFormat( const wstring& strScreenshotFolderPath )
{
	int nsscount=0;
	wchar_t screenshotfilename[_MAX_PATH];
	wchar_t screenshotfilenameJPG[_MAX_PATH];
	wchar_t screenshotfilenameBMP[_MAX_PATH];
	wchar_t screenshotfilenamePNG[_MAX_PATH];

	// 제일 마지막 번호로 저장하기 위함
	do {
		swprintf_s(screenshotfilename,	  L"%s%s%03d" , strScreenshotFolderPath.c_str() , GAME_NAME_STR, nsscount);
		swprintf_s(screenshotfilenameJPG, L"%s%s%03d.jpg" , strScreenshotFolderPath.c_str() , GAME_NAME_STR, nsscount);
		swprintf_s(screenshotfilenameBMP, L"%s%s%03d.bmp" , strScreenshotFolderPath.c_str() , GAME_NAME_STR, nsscount);
		swprintf_s(screenshotfilenamePNG, L"%s%s%03d.png" , strScreenshotFolderPath.c_str() , GAME_NAME_STR, nsscount);

		nsscount++;
	}
	while( (IsExist(screenshotfilenameJPG)||(IsExist(screenshotfilenameBMP))||(IsExist(screenshotfilenamePNG))) && nsscount<10000);

	if (nsscount<10000)
	{
		if(XGETCFG_VIDEO_SCREENSHOTFORMAT == "JPEG(*.jpg)")
			return screenshotfilenameJPG;
		else if(XGETCFG_VIDEO_SCREENSHOTFORMAT == "BMP(*.bmp)")
			return screenshotfilenameBMP;
		else if(XGETCFG_VIDEO_SCREENSHOTFORMAT == "PNG(*.png)")
			return screenshotfilenamePNG;
	}

	return wstring();
}