#include "StdAfx.h"
#include "XCrossHair.h"
#include "XPath.h"
#include "MLocale.h"

#define DEFAULT_CROSS_HAIR		"Default"
#define CRISS_HAIR_NAME_PURE	"crosshair"
#define CROSS_HAIR_IMAGE_EXT	L".png"
#define CROSS_HAIR_PICK			"_pick"
#define CROSS_HAIR_AWAYS		"_aways"
#define CROSS_HAIR_BACKSLASH	L"\\"

void XCrossHair::GetCustomCrossHairList( vector<CROSSHAIR_LIST>& vecList )
{

	if (XPath::CreateDirectoryInMyDocuments(PATH_MYDOC_CUSTOM_CROSSHAIR) == false)
	{
		return;
	}

	wstring strFolderPath = XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_CUSTOM_CROSSHAIR);

	GetCustomCrossHairListData(strFolderPath, &vecList, NULL);
}

void XCrossHair::GetCustomCrossHairListData( wstring strPath, vector<CROSSHAIR_LIST>* pvecList, vector<wstring>* pvecListData )
{
	HANDLE hDir;
	WIN32_FIND_DATA DirData = {0};
	BOOL bRet = TRUE;

	wstring strFolderList = strPath + L"*.*";
	hDir = FindFirstFile(strFolderList.c_str(), &DirData);
	while( (hDir != INVALID_HANDLE_VALUE) && bRet )
	{
		if((DirData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == false &&
			(DirData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) == false)
		{
			if (DirData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			{
				if ((strcmp(MLocale::ConvUTF16ToAnsi(DirData.cFileName).c_str(), ".") != 0) &&
					(strcmp(MLocale::ConvUTF16ToAnsi(DirData.cFileName).c_str(), "..") != 0))
				{
					if(pvecList)
					{
						CROSSHAIR_LIST crossHair;
						crossHair.m_strCrossHairListName = DirData.cFileName;
						GetCustomCrossHairListData(strPath + DirData.cFileName + L"/", NULL, &(crossHair.m_vecCrossHairList));
						pvecList->push_back(crossHair);

					}
				}
			}
			else if(pvecListData)
			{
				wstring strFileName		= DirData.cFileName;
				wstring strSubName		= strFileName.substr(0, strFileName.find(L"."));
				pvecListData->push_back(strSubName);
			}
		}

		bRet = FindNextFile(hDir, &DirData);
	}

	FindClose(hDir);
}

void XCrossHair::Init()
{
	m_bAwaysCrossHair = true;
	m_vecCrossHairList.clear();

	// 첫번째는 디폴트이다.
	CROSSHAIR_LIST defaultCrossHair;
	defaultCrossHair.m_strCrossHairListName = MLocale::ConvAnsiToUTF16(DEFAULT_CROSS_HAIR);
	m_vecCrossHairList.push_back(defaultCrossHair);

	// 커스텀 크로스헤어 가져오기
	GetCustomCrossHairList(m_vecCrossHairList);
}

int XCrossHair::GetCrossHairCount()
{
	return m_vecCrossHairList.size();
}

std::wstring XCrossHair::GetCrossHairName( int nIndex )
{
	if(nIndex >= (int)m_vecCrossHairList.size())
		return L"";

	return m_vecCrossHairList[nIndex].m_strCrossHairListName;
}

bool XCrossHair::StartCrossHair( vec2 vPos )
{
	if(XGETCFG_GAME_CROSSHAIR == DEFAULT_CROSS_HAIR)
	{
		EndCrossHair();
		return false;
	}

	global.script->GetGlueGameEvent().OnGameEvent( "CROSSHAIR", "START", vPos.x, vPos.y);
	m_bAwaysCrossHair = false;

	return true;
}

bool XCrossHair::StartCrossHair_Pick( vec2 vPos )
{
	if(XGETCFG_GAME_CROSSHAIR == DEFAULT_CROSS_HAIR)
	{
		EndCrossHair();
		return false;
	}

	global.script->GetGlueGameEvent().OnGameEvent( "CROSSHAIR", "PICK", vPos.x, vPos.y);
	m_bAwaysCrossHair = false;
	
	return true;
}

bool XCrossHair::CreateCrossHairImage( wstring& strName, wstring& strFileName )
{
	if(global.ui == NULL)
		return false;

	MBitmap* pIcon = MBitmapManager::Get( MLocale::ConvUTF16ToAnsi(strName.c_str()).c_str());
	if (pIcon == NULL)
	{
		pIcon = global.ui->GetMint()->NewBitmap(MLocale::ConvUTF16ToAnsi(strName.c_str()).c_str(), MLocale::ConvUTF16ToAnsi(strFileName.c_str()).c_str());
		if (pIcon == NULL) 
			return false;

		MBitmapManager::Add(pIcon);
	}

	return true;
}

bool XCrossHair::SetCrossHairImage( wstring& strCrossHairName )
{
	if (XPath::CreateDirectoryInMyDocuments(PATH_MYDOC_CUSTOM_CROSSHAIR) == false)
	{
		return false;
	}

	wstring strCrossHairFullName = XPath::GetMyDocumentsRaiderzSubPath(PATH_MYDOC_CUSTOM_CROSSHAIR) + strCrossHairName + CROSS_HAIR_IMAGE_EXT;

	return CreateCrossHairImage(strCrossHairName, strCrossHairFullName);
}

void XCrossHair::EndCrossHair()
{
	global.script->GetGlueGameEvent().OnGameEvent( "CROSSHAIR", "END");
	m_bAwaysCrossHair = true;
}

std::wstring XCrossHair::GetCurrentCrossHairName()
{
	wstring strCrossHairListName = MLocale::ConvAnsiToUTF16((XGETCFG_GAME_CROSSHAIR).c_str());
	wstring strCrossHairFileName = MLocale::ConvAnsiToUTF16((string(CRISS_HAIR_NAME_PURE)).c_str());

	if(CheckExistCrossHairFile(strCrossHairListName, strCrossHairFileName) == false)
	{
		strCrossHairFileName = MLocale::ConvAnsiToUTF16((string(CRISS_HAIR_NAME_PURE) + CROSS_HAIR_PICK).c_str());
		if(CheckExistCrossHairFile(strCrossHairListName, strCrossHairFileName) == false)
		{
			strCrossHairFileName = MLocale::ConvAnsiToUTF16((string(CRISS_HAIR_NAME_PURE) + CROSS_HAIR_AWAYS).c_str());
			if(CheckExistCrossHairFile(strCrossHairListName, strCrossHairFileName) == false)
			{
				return L"";
			}
		}
	}

	wstring strCrossHair = strCrossHairListName + CROSS_HAIR_BACKSLASH + strCrossHairFileName;

	if(SetCrossHairImage(strCrossHair) == false)
		return L"";

	return strCrossHair;
}

std::wstring XCrossHair::GetCurrentCrossHairPickName()
{
	wstring strCrossHairListName = MLocale::ConvAnsiToUTF16((XGETCFG_GAME_CROSSHAIR).c_str());
	wstring strCrossHairFileName = MLocale::ConvAnsiToUTF16((string(CRISS_HAIR_NAME_PURE) + CROSS_HAIR_PICK).c_str());

	if(CheckExistCrossHairFile(strCrossHairListName, strCrossHairFileName) == false)
	{
		strCrossHairFileName = MLocale::ConvAnsiToUTF16((string(CRISS_HAIR_NAME_PURE)).c_str());
		if(CheckExistCrossHairFile(strCrossHairListName, strCrossHairFileName) == false)
		{
			strCrossHairFileName = MLocale::ConvAnsiToUTF16((string(CRISS_HAIR_NAME_PURE) + CROSS_HAIR_AWAYS).c_str());
			if(CheckExistCrossHairFile(strCrossHairListName, strCrossHairFileName) == false)
			{
				return L"";
			}
		}
	}

	wstring strCrossHair = strCrossHairListName + CROSS_HAIR_BACKSLASH + strCrossHairFileName;

	if(SetCrossHairImage(strCrossHair) == false)
		return L"";


	return strCrossHair;
}

bool XCrossHair::StartCrossHair_Aways( vec2 vPos )
{
	if(XGETCFG_GAME_CROSSHAIR == DEFAULT_CROSS_HAIR)
	{
		EndCrossHair();
		return false;
	}

	if(m_bAwaysCrossHair == false)
		return false;

	global.script->GetGlueGameEvent().OnGameEvent( "CROSSHAIR", "AWAYS", vPos.x, vPos.y);

	return true;
}

std::wstring XCrossHair::GetCurrentCrossHairAwaysName()
{
	wstring strCrossHairListName = MLocale::ConvAnsiToUTF16((XGETCFG_GAME_CROSSHAIR).c_str());
	wstring strCrossHairFileName = MLocale::ConvAnsiToUTF16((string(CRISS_HAIR_NAME_PURE) + CROSS_HAIR_AWAYS).c_str());

	if(CheckExistCrossHairFile(strCrossHairListName, strCrossHairFileName))
	{
		wstring strCrossHair = strCrossHairListName + CROSS_HAIR_BACKSLASH + strCrossHairFileName;

		if(SetCrossHairImage(strCrossHair) == false)
			return L"";

		return strCrossHair;
	}

	return L"";
}

bool XCrossHair::CheckExistCrossHairFile( wstring strListName, wstring strFileName )
{
	for(vector<CROSSHAIR_LIST>::iterator it = m_vecCrossHairList.begin(); it != m_vecCrossHairList.end(); ++it)
	{
		if(it->m_strCrossHairListName == strListName)
		{
			for(vector<wstring>::iterator itFileName = it->m_vecCrossHairList.begin(); itFileName != it->m_vecCrossHairList.end(); ++itFileName)
			{
				if(*itFileName == strFileName)
					return true;
			}
		}
	}

	return false;
}
