// App.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DLL_App.h"

#include "MainFrm.h"
#include "CoreToolRS3_Doc.h"
#include "CoreToolRS3_View.h"

#include "C_KeyEventHandler.h"
#include "Test_UnitTestRunner.h"

#include "MFileSystem.h"
#include "PropertySystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include <hash_map>
#include <algorithm>
typedef stdext::hash_map<std::string,std::string> FILE2PATH;

class CCheckDuplicateFiles
{
	static void FindAllFile(std::string FilePath,FILE2PATH& FileNameTable,std::string& DuplicateFiles)
	{

		WIN32_FIND_DATA FindFileData;
		char Buffer[255];
		GetCurrentDirectory(255,Buffer);
		std::string SearchFilePath = FilePath + "*.*";

		HANDLE hFind = FindFirstFile(SearchFilePath.c_str(), &FindFileData);

		std::string Path;
		std::string strTmp;
		std::string cur_Tmp;

		if (hFind == INVALID_HANDLE_VALUE)   // 에러 처리
		{
			return;
		} 

		// 해당 폴더의 모든파일에 대해 검사
		while( true )
		{
			if(! (	!(strcmp(FindFileData.cFileName, "."))		|| 
				!(strcmp(FindFileData.cFileName,".."))		||
				!(strcmp(FindFileData.cFileName,".svn")) 
				) )
			{
				//--현재 파일의 전체 경로를 얻음--
				Path = FilePath + FindFileData.cFileName;

				//------------------------------------
				// 해당 파일이 디렉토리이면,
				if( (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
				{
					Path += "/";
					// 재귀함수 호출(해당 폴더 안으로 들어가서 검색)
					FindAllFile(Path, FileNameTable,DuplicateFiles);
				}
				else// 디렉토리가 아니고 파일이면,
				{
					std::string lowercase = FindFileData.cFileName;
					transform( lowercase.begin(), lowercase.end(), lowercase.begin(), tolower);
					FILE2PATH::iterator iter = FileNameTable.find( lowercase );
					if( FileNameTable.end() != iter )
					{//파일이름이 이미 있다면
						DuplicateFiles.append(iter->second +" __ "+Path+"\r\n");
					}
					else
					{
						FileNameTable.insert(pair<std::string,std::string>(lowercase,Path));
					}

				}
			}

			if( FindNextFile( hFind, &FindFileData ) == FALSE )	// 다음 파일을 검색
			{
				DWORD dwErr = GetLastError();
				if( dwErr != ERROR_NO_MORE_FILES )
				{
					// 더이상 파일이 없는 이유가 아닌 다른 이유로 FindNextFile이 실패한 경우,
					// TO DO : Implement as you want_
				}
				break;
			}
		}
	}
public:
	static void FindDuplicateFiles(std::string FilePath,std::string& DuplicateFiles)
	{
		FILE2PATH	FileNameTable;
		FindAllFile(FilePath,FileNameTable,DuplicateFiles);
	}

};

/////////////////////////////////////////////////////////////////////////////
// App

App* pObj;

App*	App_GetApp()
{
	return pObj;
}

//---------------------------------------------------------------------------
// instantiation of the interface class object
BOOL App_GetClassObject(int nClsid, int nId, void** ppvObj)
{
    ASSERT(nClsid == CLSID_APP);
    ASSERT(nId == IID_IDocument);

	pObj = new App;;
	MyIUnknown* pUnk = &pObj->m_xDocument; 

	if(pUnk->QueryInterface( nId, ppvObj ))
	{
		pObj->ExternalRelease();		// to balance m_dwRef, ref counter
		return TRUE;
	}
    return FALSE;
}

//---------------------------------------------------------------------------
// ctor
App::App()
{
    TRACE("App::ctor\n");
}

//---------------------------------------------------------------------------
// dtor
App::~App()
{
    TRACE("App::dtor\n");

	CPropertySystem::Release();
}

//---------------------------------------------------------------------------
// ExternalQueryInterface
BOOL App::ExternalQueryInterface(int nId, void** ppvObj) 
{
	TRACE("Entering App::ExternalQueryInterface--nIid = %d\n" ,nId);
    switch (nId) {
    case IID_IDocument:
       *ppvObj = &m_xDocument;
      break;
    default:
      *ppvObj = NULL;
      return FALSE;
    }
    ExternalAddRef();
    return TRUE;
}                               

//---------------------------------------------------------------------------
// OnFinalRelease
void App::OnFinalRelease()
{
    TRACE("AnRcp::OnFinalRelease\n");
	CCmdTarget::OnFinalRelease();
}

//---------------------------------------------------------------------------
// XDocument implementation
DWORD App::XDocument::AddRef()
{
    TRACE("App::XDocument::AddRef\n");
    MY_METHOD_PROLOGUE(App, Document)
    return pThis->ExternalAddRef();
}

DWORD App::XDocument::Release()
{
    TRACE("App::XDocument::Release\n");
    MY_METHOD_PROLOGUE(App, Document)
    return pThis->ExternalRelease();
}

BOOL App::XDocument::QueryInterface( int nId, void** ppvObj )
{
    MY_METHOD_PROLOGUE(App, Document)
    return pThis->ExternalQueryInterface(nId, ppvObj);
}

//---------------------------------------------------------------------------
// Create document template
//		nId:	document type
CDocTemplate* App::XDocument::CreateDocTempl( CWinApp* pWinApp, int nId )
{
    TRACE("App::XDocument::OpenDocTempl\n");
    MY_METHOD_PROLOGUE(App, Document)

	pThis->m_pWinApp = pWinApp;

	pThis->SetRsc();	// setup DLL resource handler

	CDocTemplate* ret;

	switch (nId)
	{
	case C_DOC_1:
		CSingleDocTemplate* pDocTemplate;
		pDocTemplate = new CSingleDocTemplate(
			IDR_MAINFRAME,
			RUNTIME_CLASS(CCoreToolRS3Doc),
			RUNTIME_CLASS(CMainFrame),       // 주 SDI 프레임 창입니다.
			RUNTIME_CLASS(CCoreToolRS3View));
		if (!pDocTemplate)
			return FALSE;
		pThis->m_pWinApp->AddDocTemplate(pDocTemplate);

// 	pThis->m_pDoc1Templ = new CMultiDocTemplate(
// 			IDR_EXETYPE,
// 			RUNTIME_CLASS(CExeDoc),
// 			RUNTIME_CLASS(CChildFrame),
// 			RUNTIME_CLASS(CExeView));
// 
// 	pThis->m_pWinApp->AddDocTemplate( pThis->m_pDoc1Templ );
	ret = pThis->m_pDoc1Templ;
	break;
/*
	case C_DOC_2:
	pThis->m_pDoc2Templ = new CMultiDocTemplate(
			IDR_VIEW2,
			RUNTIME_CLASS(CExeDoc),
			RUNTIME_CLASS(CChildFrame2),
			RUNTIME_CLASS(CExeView));

	pThis->m_pWinApp->AddDocTemplate( pThis->m_pDoc2Templ );
	ret = pThis->m_pDoc2Templ;
	break;
*/
	default:
		TRACE1("App::XDocument::CreateDocTempl; ID inknown %d\n", nId);
		ret = NULL;
	}

	pThis->RestoreRsc();	// restore client's resource handler

	return ret;
}

//---------------------------------------------------------------------------
// create main MDI Frame window
void App::XDocument::CreateFrame()
{
    MY_METHOD_PROLOGUE(App, Document)

	pThis->SetRsc();

// 	CMainFrame* pMainFrame = new CMainFrame;
// 	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
// 		ASSERT( 0 );
// 	pThis->m_pWinApp->m_pMainWnd = pMainFrame;

//	pThis->RestoreRsc();
}

//---------------------------------------------------------------------------
// Handle resources
void App::SetRsc()
{
	m_hDLL = ::GetModuleHandle(CORETOOL_DLL_NAME);
	if( !m_hDLL )
	{
		DWORD dwLastError = GetLastError();
		mlog3("can't locate %s, error code = %d\n",CORETOOL_DLL_NAME, dwLastError);
		_ASSERT(FALSE);
	}
	m_hEXE = AfxGetResourceHandle();
	AfxSetResourceHandle((HINSTANCE) m_hDLL);
}

void App::RestoreRsc()
{
	AfxSetResourceHandle(m_hEXE);
}

void App::XDocument::OnInitInstance()
{
// 	_CrtSetBreakAlloc(766);

	// Current Directory를 맞춘다.
 	char szModuleFileName[_MAX_DIR] = {0,};
 	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
 	PathRemoveFileSpec(szModuleFileName);
 	SetCurrentDirectory(szModuleFileName);

	// Initialize file system
	if( rs3::REngine::IsDevelopVersion() )
	{
		MCreateFileSystem( szModuleFileName, MFILEACCESS_GENERIC, "../../EngineResDev;../../Data/");
	}
	else
	{
		MCreateFileSystem( szModuleFileName, MFILEACCESS_GENERIC, "../../EngineRes;../../Data/");
	}

 	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
 	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
 	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
 	INITCOMMONCONTROLSEX InitCtrls;
 	InitCtrls.dwSize = sizeof(InitCtrls);
 	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
 	// 이 항목을 설정하십시오.
 	InitCtrls.dwICC = ICC_WIN95_CLASSES;
 	InitCommonControlsEx(&InitCtrls);
 
 	// OLE 라이브러리를 초기화합니다.
 	if (!AfxOleInit())
 	{
 		AfxMessageBox(IDP_OLE_INIT_FAILED);
 		return ;
 	}
 	AfxEnableControlContainer();

 	CMFCVisualManagerOffice2007::SetStyle( CMFCVisualManagerOffice2007::Office2007_ObsidianBlack );
 	CMFCVisualManager::SetDefaultManager( RUNTIME_CLASS( CMFCVisualManagerOffice2007 ));
// 	CMFCVisualManager::SetDefaultManager( RUNTIME_CLASS( CMFCVisualManagerVS2005 ));

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

void App::XDocument::OnExitInstance()
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	afxGlobalData.CleanUp();

	CMFCToolBar::CleanUpImages();
	CMenuImages::CleanUp();

	if (GetCmdMgr() != NULL)
	{
		GetCmdMgr()->CleanUp();
	}

	CKeyboardManager::CleanUp();

	CMFCVisualManager::DestroyInstance(TRUE /* bAutoDestroyOnly */);
	CMFCVisualManagerOffice2007::CleanStyle();
}

void App::XDocument::OnPostCreate()
{
	m_pMainFrame_ = (CMainFrame*)AfxGetMainWnd();
	CKeyEventHandler::GetReference().InitHandler( m_pMainFrame_->GetControlMediator() );

	m_pMainFrame_->OnDocViewCreated_();
	m_pDoc_ = static_cast<CCoreToolRS3Doc*>(m_pMainFrame_->GetActiveDocument());
	m_pDoc_->InitDocument(m_pMainFrame_->GetControlMediator(), m_pMainFrame_);
	m_pView_ = static_cast<CCoreToolRS3View*>(m_pMainFrame_->GetActiveView());
	m_pDoc_->ExitCode = &m_nExitCode_;

	// 파일중복 검사
	std::string DuplicateFiles;
	CCheckDuplicateFiles::FindDuplicateFiles("../../Data/",DuplicateFiles);
	const char* DUPLICATE_FILENAME = _T("DuplicateFiles.txt");
	if( 0 != DuplicateFiles.size() )
	{
		// 코어툴을 이용한 스크립트 실행이나 디버깅시 편의성을 위해
		// 커맨드라인으로 문서를 지정하여 시작하면 중복검사 메시지박스를 열지 않는다.
		int nReturn = IDNO;
		if ( m_pDoc_->EnterFileName.IsEmpty() )
		{
			nReturn = MessageBox( m_pMainFrame_->m_hWnd,
						"Data폴더안에 중복된 파일이 있습니다. DuplicateFiles.txt 를 열어보시겠습니까?",
						"파일 시스템 에러", MB_YESNO );
		}

		HANDLE hFile = CreateFile( DUPLICATE_FILENAME,	GENERIC_WRITE, FILE_SHARE_READ,	NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		CFile file(hFile);
		file.Write(DuplicateFiles.c_str(),DuplicateFiles.size());
		file.Close();

		if( nReturn == IDYES )
			ShellExecute( m_pMainFrame_->m_hWnd, _T("open"), DUPLICATE_FILENAME, NULL, NULL, SW_SHOW );
		
	}
	else
	{
		DeleteFile(_T("DuplicateFiles.txt"));
	}

}

BOOL App::XDocument::OnIdle(LONG lCount)
{
	BOOL bResult = FALSE;
	if ( m_pView_ && m_pMainFrame_ )
	{
		m_pView_->OnUserTimer();
		Sleep(1);
		bResult = TRUE;
	}

	if( m_pMainFrame_->IsActivated() == false )
		Sleep(10);

	return bResult;
}

void App::XDocument::OnShellOpenFile()
{
	if (m_pDoc_->EnterFileName.IsEmpty())
		return;

	m_pDoc_->OpenMapFile(m_pDoc_->EnterFileName);
}

bool App::XDocument::PreTranslateMessage(MSG* pMsg)
{
	UINT message = pMsg->message;
	WPARAM wParam = pMsg->wParam;

	bool bKeyIntercepted = false;
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		CKeyEventHandler::GetReference().CallBackAppKeyEvent((UINT)wParam, true, bKeyIntercepted);
		//키 가로채기
		//if(wParam == VK
		break;
	case WM_KEYUP:
		CKeyEventHandler::GetReference().CallBackAppKeyEvent((UINT)wParam, false, bKeyIntercepted);
		break;
	case WM_SYSKEYDOWN:
		CKeyEventHandler::GetReference().SysKeyEvent((UINT)wParam, true, bKeyIntercepted);
		break;
	case WM_SYSKEYUP:
		CKeyEventHandler::GetReference().SysKeyEvent((UINT)wParam, false, bKeyIntercepted);
		break;
	};

	return bKeyIntercepted;
}