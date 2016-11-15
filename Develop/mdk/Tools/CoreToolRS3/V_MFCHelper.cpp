#include "StdAfx.h"
#include "V_MFCHelper.h"

#include "C_StatusBarHandler.h"
#include "C_RubyManager.h"
#include "C_XMLConfig.h"
#include "C_XMLUserConfig.h"

#include "VD_DlgOption.h"
#include "VD_DlgShortCut.h"
#include "VD_DlgXmlListEditor.h"
#include "VD_DlgMapBatchScript.h"

#include <d3dx9.h>
#include "il/ilut.h"
#include "RDevice.h"
#include "RTexture.h"
#include "MFileLoader.h"

CMFCHelper* CMFCHelper::m_pMFCHelper_ = NULL;
CMFCHelper::CMFCHelper(CControlMediator* _pControlMediator)
{
	m_pControlMediator_ = _pControlMediator;
	m_hSciDll_ = CScintillaWnd::LoadScintillaDll();
}

CMFCHelper::~CMFCHelper(void)
{
	::FreeLibrary(m_hSciDll_);
}

void CMFCHelper::InitMFCHelper(CControlMediator* _pControlMediator, CCoreToolStatusBar* _pStatusBar)
{
	m_pMFCHelper_ = new CMFCHelper(_pControlMediator);
	CStatusBarHandler::GetInstance()->InitStatusBar(_pStatusBar);
}

void CMFCHelper::DestroyMFCHelper()
{
	delete m_pMFCHelper_;
	m_pMFCHelper_ = NULL;

	CStatusBarHandler::GetInstance()->InvalidateStatusBar();
}

void CMFCHelper::OpenToolOptionDlg()
{
	CDlgOption dlg("Tool Option");
	dlg.InitFromConfig();
	if (IDOK == dlg.DoModal())
	{
		bool bFolderChanged = false;

		if (_stricmp(CXMLConfig::GetInstance()->pcDataPath,
						(const char*)dlg.m_pPage2->m_strDataPath))
		{
			bFolderChanged = true;
		}

		// 조작
		CXMLConfig::GetInstance()->fMaxFarZ = dlg.m_pPage1->m_fMaxFarZ;
		CXMLConfig::GetInstance()->fMinFarZ = dlg.m_pPage1->m_fMinFarZ;
		CXMLConfig::GetInstance()->fWheelZoomFactor = dlg.m_pPage1->m_fWheelZoomFactor;
		CXMLConfig::GetInstance()->fKeyZoomFactor = dlg.m_pPage1->m_fKeyZoomFactor;
		CXMLConfig::GetInstance()->fRotateStep = dlg.m_pPage1->m_fRotateStep;
		CXMLConfig::GetInstance()->fPanStep = dlg.m_pPage1->m_fPanStep;
		strcpy(CXMLConfig::GetInstance()->pcDataPath, dlg.m_pPage2->m_strDataPath);

		//Text Editor
		CXMLUserConfig::GetReference().SetTextEditor(dlg.m_pPage1->m_strTextEditor.GetString());

		//Check Option
		CXMLUserConfig::GetReference().SetRememberWork(dlg.m_pPage1->m_nRememberWork);

		//Server
		CXMLConfig::GetInstance()->strServerFieldPath = dlg.m_pPage2->m_strServerPath.GetString();

		//LoginID, IP
		CXMLUserConfig::GetReference().SetTextLoginID(dlg.m_pPage1->m_strTextLoginID.GetString());
		CXMLUserConfig::GetReference().SetTextLoginIP(dlg.m_pPage1->m_strTextLoginIP.GetString());

		if(dlg.m_pPage1->m_bConfigXmlSave)
			CXMLConfig::GetInstance()->SaveXML(FILENAME_CONFIG);


		// 실제 어플리케이션에 반영
		m_pMFCHelper_->m_pControlMediator_->m_cameraInfo.ROTATE_STEP		= CXMLConfig::GetInstance()->fRotateStep;
		m_pMFCHelper_->m_pControlMediator_->m_cameraInfo.MOVE_STEP			= CXMLConfig::GetInstance()->fPanStep;
		m_pMFCHelper_->m_pControlMediator_->m_cameraInfo.KEY_STEP			= CXMLConfig::GetInstance()->fVelocity;
		m_pMFCHelper_->m_pControlMediator_->m_cameraInfo.WHEEL_ZOOM_STEP	= CXMLConfig::GetInstance()->fWheelZoomFactor;

		if (bFolderChanged)
		{
			AfxMessageBox("Folder changed.");
		}
	}
}

void CMFCHelper::OpenShortCutList()
{
	CDlgShortCut dlg;
	dlg.DoModal();
}

void CMFCHelper::OpenZoneListEditorWithField()
{
	CDlgXmlListEditor dlg;
	char szfull[256];
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string zonelist(szfull);
	std::string fieldlist(szfull);
	_fullpath( szfull, CXMLConfig::GetInstance()->strServerPath.c_str(), 256 );
	std::string serverzonelist(szfull);
	zonelist += "system\\zonelist.xml";
	serverzonelist += "system\\zonelist.xml";
	fieldlist += "system\\fieldlist.xml";
	dlg.LoadXmlListFile(zonelist.c_str(), serverzonelist.c_str(), fieldlist.c_str(), CDlgXmlListEditor::ZONE_LIST);
	dlg.DoModal();
}

void CMFCHelper::OpenFieldListEditorWithZone()
{
	CDlgXmlListEditor dlg;
	char szfull[256];
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string zonelist(szfull);
	std::string fieldlist(szfull);
	_fullpath( szfull, CXMLConfig::GetInstance()->strServerPath.c_str(), 256 );
	std::string serverzonelist(szfull);
	zonelist += "system\\zonelist.xml";
	serverzonelist += "system\\zonelist.xml";
	fieldlist += "system\\fieldlist.xml";
	dlg.LoadXmlListFile(zonelist.c_str(), serverzonelist.c_str(), fieldlist.c_str(), CDlgXmlListEditor::FIELD_LIST);
	dlg.DoModal();
}

void CMFCHelper::OpenMapBatchScrip()
{
	CDlgMapBatchScript dlg;
	if(dlg.DoModal() == IDOK)
	{
		CRubyManager::GetReference().DoMapBatchScript(dlg.m_strSelectedScript.c_str(), dlg.m_vecSelectedMap);
	}
}

void CMFCHelper::OpenFieldListEditor()
{
	CDlgXmlListEditor dlg;

	char szfull[256];
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string fieldlist(szfull);
	fieldlist += "system\\fieldlist.xml";

	dlg.LoadXmlListFile(fieldlist.c_str(), CDlgXmlListEditor::FIELD_LIST);
	dlg.DoModal();
}

void CMFCHelper::OpenZoneListEditor()
{
	CDlgXmlListEditor dlg;

	char szfull[256];
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string zoneList(szfull);
	zoneList += "system\\zonelist.xml";

	dlg.LoadXmlListFile(zoneList.c_str(), CDlgXmlListEditor::ZONE_LIST);
	dlg.DoModal();
}

void CMFCHelper::OpenNpcListEditor()
{
	CDlgXmlListEditor dlg;

	char szfull[256];
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string npcList(szfull);
	npcList += "system\\npc.xml";

	dlg.LoadXmlListFile(npcList.c_str(), CDlgXmlListEditor::NPC_LIST);
	dlg.DoModal();
}

void CMFCHelper::OpenPropertyListEditor()
{
	CDlgXmlListEditor dlg;
	dlg.LoadXmlListFile("property.xml", CDlgXmlListEditor::PROPERTY_LIST);
	dlg.DoModal();
}

rs3::RTexture* CMFCHelper::CreateTextureFromResourceBitmap(rs3::RDevice *pDevice, UINT uiResID)
{
	using namespace rs3;
	CBitmap bitmap;
	if ( bitmap.LoadBitmap(uiResID) == FALSE )
		return NULL;

	BITMAP tBitmap;
	memset(&tBitmap, 0, sizeof(tBitmap));
	bitmap.GetObject(sizeof(tBitmap),&tBitmap);

	DWORD nSize = tBitmap.bmWidthBytes * tBitmap.bmHeight;

	DWORD *pdwBuffer = (DWORD*)_alloca(nSize);
	DWORD dwSize = bitmap.GetBitmapBits(nSize, pdwBuffer);
	_ASSERT(nSize==dwSize);

	_ASSERT(tBitmap.bmBitsPixel == 32 || tBitmap.bmBitsPixel == 24);
	RFORMAT fmt = (tBitmap.bmBitsPixel == 32 ? RFMT_ARGB32 : RFMT_XRGB32);
	return pDevice->CreateTexture(tBitmap.bmWidth, tBitmap.bmHeight, fmt, RTF_LINEAR, 0, pdwBuffer);
}

bool CMFCHelper::D3DTextureToHDC(HDC hDC, LPDIRECT3DTEXTURE9 lpTexture, RECT *pTargetRect, bool bDrawAlpha)
{
	HRESULT hr;

	// D3DXSaveTextureToFileInMemory를 이용하여, Memory상에 BMP로 저장하고 그것을 이용한다.
	LPD3DXBUFFER pBuffer = NULL;
	hr = D3DXSaveTextureToFileInMemory(&pBuffer, D3DXIFF_BMP, lpTexture, NULL);
	if ( FAILED(hr))
		return false;

	// 헤더를 읽고
	BITMAPFILEHEADER *pHeader = (BITMAPFILEHEADER*)pBuffer->GetBufferPointer();
	BITMAPINFOHEADER *pInfo = (BITMAPINFOHEADER*)((BYTE*)pBuffer->GetBufferPointer() + sizeof(BITMAPFILEHEADER));
	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(bmpInfo));
	bmpInfo.bmiHeader = *pInfo;
	void *pData = (void*)((BYTE*)pBuffer->GetBufferPointer() + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

	// 동일한 DIBSECTION을 만든다.
	HDC hMemDC = CreateCompatibleDC(hDC);
	if ( hMemDC == NULL )
	{
		SAFE_RELEASE(pBuffer);
		return false;
	}

	ILuint nID;
	ilGenImages(1,&nID);
	if ( nID == 0 )
	{
		SAFE_RELEASE(pBuffer);
		return false;
	}

	int nX = pTargetRect->left;
	int nY = pTargetRect->top;
	int nWidth = pTargetRect->right-pTargetRect->left;
	int nHeight = pTargetRect->bottom - pTargetRect->top;

	BYTE *pPixel = NULL;

	UINT dibFormat = DIB_RGB_COLORS;
	BITMAPINFO bmpInfoMem = bmpInfo;
	bmpInfoMem.bmiHeader.biWidth = nWidth;
	bmpInfoMem.bmiHeader.biHeight = nHeight;

	// 텍스쳐와 같은 크기의 메모리 hMemDCBitmap을 만든다.
	HBITMAP hMemDCBitmap = CreateDIBSection(hMemDC,&bmpInfoMem, dibFormat, (void**)&pPixel, NULL, 0);
	SelectObject(hMemDC, hMemDCBitmap);

	// BITMAP의 내용을 DevIL로 적용시킨다.
	ilBindImage(nID);
	ilTexImage(pInfo->biWidth, pInfo->biHeight, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE,(void*)pData);
	iluImageParameter(ILU_FILTER, ILU_BILINEAR);
	iluScale(nWidth, nHeight, 1);

	if ( !bDrawAlpha )
	{
		// DevIL의 색상 값을 (RGB)를 hMemDC 로 복사한다.
		ILubyte *pILData = ilGetData();
		if ( pILData )
		{
			int nSize = nWidth * nHeight * 4;	// width * height * bpp
			memcpy(pPixel, pILData, nSize);
		}
	}
	else
	{
		// DevIL의 Alpha 값을 Gray 컬러로 hMemDC에 복사한다.
		ILubyte *pILData = ilGetAlpha(IL_UNSIGNED_BYTE);
		if ( pILData )
		{	
			for ( int h = 0; h<nHeight; ++h)
				for ( int w = 0; w<nWidth; ++w )
				{
					int nIndex = (w + h*nWidth);
					unsigned char *pTargetPixel = &pPixel[nIndex*4];
					unsigned char alphaPixel = pILData[nIndex];

					*pTargetPixel = alphaPixel;	++pTargetPixel;
					*pTargetPixel = alphaPixel;	++pTargetPixel;
					*pTargetPixel = alphaPixel;	++pTargetPixel;
					*pTargetPixel = 0xff;
				}
		}
	}


	BitBlt(hDC, nX, nY, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);

	ilDeleteImages(1,&nID);
	// DIBSection 제거
	pBuffer->Release();
	DeleteDC(hMemDC);
	DeleteObject(hMemDCBitmap);

	return true;
}

void CMFCHelper::SetMFCSlider(CSliderCtrl *pCtrl, int iMin, int iMax, int iPos)
{
	pCtrl->ClearTics();
	pCtrl->ClearSel();

	pCtrl->SetPos(iPos);
	pCtrl->SetRange(iMin, iMax, TRUE);
}

bool CMFCHelper::GetFileFromPathWithFilter(std::vector<std::string> &vecFilenames, const char *pPathname, const char *szFilter, bool bGetOnlyFileName)
{
	using namespace std;

	HANDLE hFind;
	WIN32_FIND_DATA fd;

	BOOL bRet  = TRUE;
	int  index = 0;

	string strPath = pPathname;
	strPath += "*";

	hFind = FindFirstFile( strPath.c_str(),&fd);
	if ( hFind == NULL )
		return false;

	while ( hFind != INVALID_HANDLE_VALUE && bRet )
	{
		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) &&
				(strcmp(fd.cFileName, ".") != 0 ) &&
				(strcmp(fd.cFileName, "..") != 0 ) )
			{
				string strChildPath = string(pPathname) + string(fd.cFileName) + "/";
				GetFileFromPathWithFilter(vecFilenames, strChildPath.c_str(), szFilter, bGetOnlyFileName );
			}
		}
		else
		{
			string fileName = fd.cFileName;
			string::value_type findPos = fileName.rfind(szFilter);

			if ( findPos != string::npos )
			{
				if (bGetOnlyFileName)
				{
					vecFilenames.push_back(fileName);
				}
				else
				{
					vecFilenames.push_back(pPathname + fileName);
				}
			}
		}

		bRet = FindNextFile( hFind, &fd );
	}
	bRet = FindNextFile( hFind, &fd );
	FindClose( hFind );

	return true;
}

bool CMFCHelper::FileExist(const char *szFilename)
{
	FILE *fp = fopen(szFilename,"rb");
	if ( fp == NULL )
		return false;

	fclose(fp);
	return true;
}

bool CMFCHelper::EqualFloat(const float &fA, const float &fB)
{
	if ( abs(fA - fB) < FLT_EPSILON )
		return true;
	return false;
}

void CMFCHelper::FillWnd(CWnd *pWnd, DWORD dwR, DWORD dwG, DWORD dwB)
{
	CClientDC dc(pWnd);
	CRect rect;
	pWnd->GetClientRect(&rect);
	CBrush brush, *pOldBrush;

	brush.CreateSolidBrush(RGB(dwR, dwG, dwB));
	pOldBrush = (CBrush*)dc.SelectObject(&brush);
	dc.Rectangle(&rect);
	dc.SelectObject(pOldBrush);
}

void CMFCHelper::FillWnd(CWnd *pWnd, COLORREF _color)
{
	CClientDC dc(pWnd);
	CRect rect;
	pWnd->GetClientRect(&rect);
	CBrush brush, *pOldBrush;

	brush.CreateSolidBrush(_color);
	pOldBrush = (CBrush*)dc.SelectObject(&brush);
	dc.Rectangle(&rect);
	dc.SelectObject(pOldBrush);
}

void CMFCHelper::RegisterBrushWorkArea(CSliderCtrl* _pCtrl, float* _pEditSize, CString* _pViewSize)
{
	SetMFCSlider(_pCtrl, 1, 200, 10);
	*_pEditSize = _pCtrl->GetPos()*0.25f;
	m_pMFCHelper_->m_vecRelatedBrushWorkArea_.push_back(
		std::pair<CSliderCtrl*, std::pair<float*, CString*> >(
		_pCtrl, pair<float*, CString*>( _pEditSize, _pViewSize )
		)
	);
}
void CMFCHelper::SetBrushWorkAreaSize(int _size)
{
	float map_size = 1.0f;
	rs3::RToolTerrain* pTerrain = NULL;

	CControlMediator* pMediator = m_pMFCHelper_->m_pControlMediator_;
	if (pMediator && pMediator->GetToolSceneManager() && (pTerrain = (rs3::RToolTerrain*) pMediator->GetToolSceneManager()->GetCurrentTerrain())
		)
	{
		if (pTerrain != NULL)
			map_size = pTerrain->GetXScale() / 50.0f; //지름 ( Grid 단위 )
	}

	if(m_pMFCHelper_->m_vecRelatedBrushWorkArea_.empty()) return;

	BRUSH_WORK_AREA::iterator it = m_pMFCHelper_->m_vecRelatedBrushWorkArea_.begin();
	it->first->SetPos(_size);

	float editSize = it->first->GetPos() * 0.25;
	float viewSize = editSize * map_size;
	//처음것은 edit_size / 두번째 것은 view_size
	*(it->second.first) = editSize;

	CString cstr_viewSize;
	if(map_size == 1)
		cstr_viewSize.Format("%.1f", viewSize);
	else
		cstr_viewSize.Format("%.1fM", viewSize);

	*(it->second.second) = cstr_viewSize;
	static_cast<CWnd*>(it->first)->GetParent()->UpdateData(FALSE);
	it++;
	while( it != m_pMFCHelper_->m_vecRelatedBrushWorkArea_.end())
	{
		it->first->SetPos(_size);
		*(it->second.first) = editSize;
		*(it->second.second) = cstr_viewSize;
		static_cast<CWnd*>(it->first)->GetParent()->UpdateData(FALSE);
		it++;
	}
}

int CMFCHelper::GetBrushWorkAreaSize()
{
	if (m_pMFCHelper_->m_vecRelatedBrushWorkArea_.empty())
		return 1;

	return m_pMFCHelper_->m_vecRelatedBrushWorkArea_.front().first->GetPos();
}

BOOL CMFCHelper::Exec(LPCSTR lpExeFile, LPCSTR lpParam, BOOL bShow, DWORD dwWaitTime)
{
	SHELLEXECUTEINFO ShExecInfo;
	memset(&ShExecInfo, 0, sizeof(ShExecInfo));

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = "open";
	ShExecInfo.lpFile = lpExeFile;
	ShExecInfo.lpParameters = lpParam;
	ShExecInfo.nShow = ( bShow ? SW_SHOW : SW_HIDE);
	ShExecInfo.hInstApp = NULL;
	if ( ShellExecuteEx(&ShExecInfo) == FALSE )
		return FALSE;

	WaitForSingleObject(ShExecInfo.hProcess, dwWaitTime);
	return TRUE;
}

bool CMFCHelper::ConvertImageToDXT(const char *pcSrcFilename, const char *pcDestFilename)
{
	int width = 0;
	int height = 0;
	int channel = 0;

	BYTE *pFileData = MFileLoad::SOIL_CreateRawFromFile(pcSrcFilename,&width,&height,&channel);
	if ( pFileData == NULL )
		return false;

	bool bResult = MFileLoad::SOIL_SaveRawToDXT((pcDestFilename ? pcDestFilename : pcSrcFilename), pFileData, width, height, channel);
	_ASSERT(bResult);

	delete [] pFileData;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CTimeCheck ( 시간을 측정해서 Log를 남긴다 )

CTimeCheck::CTimeCheck(const char *pName)
{
	m_dwConstructTime = GetTickCount();
	mlog("-- %s\n", pName);
}

CTimeCheck::~CTimeCheck()
{
	mlog("elapsed time is %d \n", GetTickCount() - m_dwConstructTime);
}
