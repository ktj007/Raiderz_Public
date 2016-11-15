// V_DlgResourceInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VL_ResourceInfo.h"

#include "M_ToolLogicObjectManager.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "C_ControlPreviewSceneManager.h"
#include "C_XMLResourceList.h"
#include "C_XMLUserConfig.h"
#include "C_XMLConfig.h"
#include "V_MFCHelper.h"

#include "CoreToolRS3_Doc.h"
#include "MainFrm.h"

// CDlgResourceInfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgResourceInfo, CDialog)
CDlgResourceInfo::CDlgResourceInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgResourceInfo::IDD, pParent)
{
	m_pCurrentResInfo_ = NULL;
	m_bModal_ = false;
}

CDlgResourceInfo::CDlgResourceInfo(CControlMediator* _pControlMediator, MAP_RES_TYPE _type, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgResourceInfo::IDD, pParent)
{
	m_pControlMediator_ = _pControlMediator;
	m_pCurrentResInfo_ = NULL;
	m_bModal_ = true;
	m_eResType_ = _type;
}

CDlgResourceInfo::~CDlgResourceInfo()
{
}

BOOL CDlgResourceInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_bModal_)
		SetResourceType(m_eResType_);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgResourceInfo::OnDestroy()
{
	ResetTreeCtrl_();
	CDialog::OnDestroy();
}

void CDlgResourceInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_MAP_RES, m_ctrlExTreeMapRes_);
}

BEGIN_MESSAGE_MAP(CDlgResourceInfo, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_MAP_RES, &CDlgResourceInfo::OnTvnSelchangedTreeMapRes)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_QUEST, &CDlgResourceInfo::OnBnClickedButtonEditQuest)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_MAP, &CDlgResourceInfo::OnBnClickedButtonOpenMap)
	ON_BN_CLICKED(IDC_BUTTON_RUN_MAP, &CDlgResourceInfo::OnBnClickedButtonRunMap)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_MAP_RES, &CDlgResourceInfo::OnNMDblclkTreeMapRes)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CDlgResourceInfo 메시지 처리기입니다.


void CDlgResourceInfo::OnCancel()
{
	if(m_bModal_)
		CDialog::OnCancel();
	else
		ShowWindow(SW_HIDE);
}

void CDlgResourceInfo::OnOK()
{
	if(m_bModal_)
		CDialog::OnOK();
	else
		ShowWindow(SW_HIDE);
}

void CDlgResourceInfo::Listen(CoreMessage& _message)
{
	if(_message.nID == VIEW_MESSAGE::ACTIVE_VIEW)
	{
		ShowWindow(SW_SHOW);
	}
}

void CDlgResourceInfo::UpdateTreeWithZoneList_()
{
	MXml kXml;
	if ( !kXml.LoadFile(CXMLResourceList::GetReference().GetZoneListPath()) )
		return;

	if(kXml.Doc()->FirstChildElement("maiet") == NULL)
		return;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet")->FirstChildElement("ZONELIST");
	if ( pRootElement == NULL )
		return;

	HTREEITEM hParent, hChild;
	hParent = m_ctrlExTreeMapRes_.InsertItem("Zone List", TVI_ROOT);
	m_ctrlExTreeMapRes_.SetItemColor(hParent, RGB(0, 0, 0));

	std::string screenshotPath(CXMLResourceList::GetReference().GetZoneUtilsPath());
	std::string zonePath(CXMLResourceList::GetReference().GetZoneFolderPath());

	std::string name;
	std::string id;
	std::string file;

	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		if ( _stricmp( pChildElement->Value(), "ZONE") == 0)
		{
			_Attribute(id, pChildElement, "id", &kXml);
			_Attribute(name, pChildElement, "name", &kXml);
			_Attribute(file, pChildElement, "file", &kXml);

			hChild = m_ctrlExTreeMapRes_.InsertItem( (name + " // ID:" + id).c_str() ,hParent,TVI_LAST);
			MAP_RES_INFO* info = new MAP_RES_INFO;
			m_ctrlExTreeMapRes_.SetItemData(hChild, (DWORD_PTR)info);

			//fill resource info
			info->m_eResType = RES_ZONE;
			std::string fullZonePath(zonePath + file + "\\" + file + ".zone.xml");
			if(GetFileAttributes(fullZonePath.c_str()) != INVALID_FILE_ATTRIBUTES)
			{
				info->m_strNameToEdit = fullZonePath;
				std::string fullScreenshotPath(screenshotPath + file + ".bmp");
				if(GetFileAttributes(fullScreenshotPath.c_str()) != INVALID_FILE_ATTRIBUTES)
					info->m_strNameToView = fullScreenshotPath;

				m_ctrlExTreeMapRes_.SetItemColor(hChild, RGB(0, 0, 0));
			}
			else
				m_ctrlExTreeMapRes_.SetItemColor(hChild, RGB(255, 0, 0));
		}
	}
}

void CDlgResourceInfo::UpdateTreeWithFieldList_()
{
	MXml kFieldXml;
	if ( !kFieldXml.LoadFile(CXMLResourceList::GetReference().GetFieldListPath()) ) return;
	if(kFieldXml.Doc()->FirstChildElement("maiet") == NULL) return;

	MXmlElement *pRootElement = kFieldXml.Doc()->FirstChildElement("maiet")->FirstChildElement("FIELDLIST");
	if ( pRootElement == NULL ) return;

	//존 아이디와 미니맵 파일명을 연결
	std::map< int, std::string > mapZoneIdShot;
	int zoneId;
	std::string zoneFile;
	MXml kZoneXml;
	if ( !kZoneXml.LoadFile(CXMLResourceList::GetReference().GetZoneListPath()) ) return;
	if(kZoneXml.Doc()->FirstChildElement("maiet") == NULL) return;

	MXmlElement *pRootZoneElement = kZoneXml.Doc()->FirstChildElement("maiet")->FirstChildElement("ZONELIST");
	if ( pRootZoneElement == NULL ) return;

	std::string screenshotPath(CXMLResourceList::GetReference().GetZoneUtilsPath());
	for ( MXmlElement *pChildElement = pRootZoneElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		if ( _stricmp( pChildElement->Value(), "ZONE") == 0)
		{
			_Attribute(&zoneId, pChildElement, "id");
			_Attribute(zoneFile, pChildElement, "file", &kZoneXml);
			mapZoneIdShot.insert( std::map<int, std::string>::value_type( zoneId, screenshotPath + zoneFile + ".bmp" ) );
		}
	}


	HTREEITEM hParent, hChild;
	hParent = m_ctrlExTreeMapRes_.InsertItem("Field List", TVI_ROOT);
	m_ctrlExTreeMapRes_.SetItemColor(hParent, RGB(0, 0, 0));

	std::string id;
	std::string name;
	std::string file;

	char szfull[256]= "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	CString fullFieldPath( szfull );
	fullFieldPath = fullFieldPath + "field\\";
	CString currentField;
	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement();
		pChildElement; pChildElement = pChildElement->NextSiblingElement())
	{
		if ( _stricmp( pChildElement->Value(), "FIELD") == 0)
		{
			_Attribute(id, pChildElement, "id", &kFieldXml);
			_Attribute(name, pChildElement, "name", &kFieldXml);
			_Attribute(file, pChildElement, "file", &kFieldXml);

			hChild = m_ctrlExTreeMapRes_.InsertItem( (name + " // ID:" + id).c_str() ,hParent,TVI_LAST);
			MAP_RES_INFO* info = new MAP_RES_INFO;
			m_ctrlExTreeMapRes_.SetItemData(hChild, (DWORD_PTR)info);

			int nId = atoi(id.c_str());
			info->m_eResType = RES_FIELD;
			info->m_nID = nId;
			currentField.Format("%d.field.xml", nId);
			currentField = fullFieldPath + currentField;

			MXml kCurrentFieldXml;
			if(kCurrentFieldXml.LoadFile( currentField ) )
			{
				info->m_strNameToEdit = currentField.GetString();
				MXmlHandle fieldXmlHandle( kCurrentFieldXml.Doc() );
				MXmlElement* pFieldInfo = fieldXmlHandle.FirstChildElement("maiet").FirstChildElement("FIELDINFO").ToElement();
				if( pFieldInfo != NULL )
				{
					int currentZoneId;
					_Attribute( &currentZoneId, pFieldInfo, "zone" );
					std::map<int, std::string>::iterator itr = mapZoneIdShot.find(currentZoneId);
					if( itr != mapZoneIdShot.end() )
					{
						const char* toViewFile = itr->second.c_str();
						if(GetFileAttributes(toViewFile) != INVALID_FILE_ATTRIBUTES)
							info->m_strNameToView = itr->second;
					}
				}
				m_ctrlExTreeMapRes_.SetItemColor(hChild, RGB(0, 0, 0));
			}
			else
				m_ctrlExTreeMapRes_.SetItemColor(hChild, RGB(255, 0, 0));
		}
	}
}

void CDlgResourceInfo::UpdateTreeWithNpcList_()
{
	MXml kXml;
	if ( !kXml.LoadFile(CXMLResourceList::GetReference().GetNpcListPath()) )
		return;

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("maiet");
	if ( pRootElement == NULL )
		return;

	HTREEITEM hParent, hChild;
	hParent = m_ctrlExTreeMapRes_.InsertItem("NPC List", TVI_ROOT);
	m_ctrlExTreeMapRes_.SetItemColor(hParent, RGB(0, 0, 0));

	std::string name;
	std::string id;
	//std::string luaFile;
	CString luaFile;

	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement("NPC");
		pChildElement; pChildElement = pChildElement->NextSiblingElement("NPC"))
	{
		_Attribute(id, pChildElement, "id", &kXml);
		_Attribute(name, pChildElement, "Name", &kXml);
		//_Attribute(luaFile, pChildElement, "NameInLua", &kXml);

		hChild = m_ctrlExTreeMapRes_.InsertItem( (name + " // ID:" + id).c_str() ,hParent,TVI_LAST);
		MAP_RES_INFO* info = new MAP_RES_INFO;
		m_ctrlExTreeMapRes_.SetItemData(hChild, (DWORD_PTR)info);

		//fill resource info
		info->m_eResType = RES_NPC;
		info->m_nID = atoi(id.c_str());
		luaFile.Format("NPC_%05d.lua", info->m_nID);

		// 파일 존재여부 확인 및 설정
		bool bNotFound = true;
		SNpcInfo* pFieldInfo = m_pControlMediator_->GetToolLogicObjectMgr()->GetNpcInfoFromId( info->m_nID );
		if (pFieldInfo)
		{
			if(GetFileAttributes( pFieldInfo->m_strNpcMeshName.c_str()) != INVALID_FILE_ATTRIBUTES)
			{
				info->m_strNameToView = pFieldInfo->m_strNpcMeshName;
				//info->m_strNameToEdit = CXMLConfig::GetInstance()->strServerNPCScriptFullPath + luaFile + ".lua";
				info->m_strNameToEdit = CXMLConfig::GetInstance()->strServerNPCScriptFullPath + luaFile.GetString();
				m_ctrlExTreeMapRes_.SetItemColor(hChild, RGB(0, 0, 0));

				bNotFound = false;
			}
		}

		if (bNotFound)
			m_ctrlExTreeMapRes_.SetItemColor(hChild, RGB(255, 0, 0));
	}
}

void CDlgResourceInfo::UpdateTreeWithTileList_()
{
	HTREEITEM hParent, hChild;
	hParent = m_ctrlExTreeMapRes_.InsertItem("Tile List", TVI_ROOT);
	m_ctrlExTreeMapRes_.SetItemColor(hParent, RGB(0, 0, 0));

	std::string tileFolder(CXMLResourceList::GetReference().GetTileFolderPath());
	std::string fileFilter( tileFolder + "*.*");
	CFileFind finder;
	BOOL bWorking = finder.FindFile(fileFilter.c_str());
	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		CString cstrExtName(finder.GetFileName().Right(4));
		if( cstrExtName.CompareNoCase(".bmp") == 0 
			|| cstrExtName.CompareNoCase(".tga") == 0
			|| cstrExtName.CompareNoCase(".dds") == 0
			)
		{
			hChild = m_ctrlExTreeMapRes_.InsertItem(finder.GetFileName(), hParent, TVI_LAST);
			MAP_RES_INFO* info = new MAP_RES_INFO;
			m_ctrlExTreeMapRes_.SetItemData(hChild, (DWORD_PTR)info);
			m_ctrlExTreeMapRes_.SetItemColor(hChild, RGB(0, 0, 0));

			//fill resource info
			info->m_eResType = RES_TILE;
			info->m_strNameToView = tileFolder + (LPCTSTR) finder.GetFileName();
		}
	}
}

void CDlgResourceInfo::OnTvnSelchangedTreeMapRes(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItem = m_ctrlExTreeMapRes_.GetSelectedItem();
	m_pCurrentResInfo_ = (MAP_RES_INFO*)m_ctrlExTreeMapRes_.GetItemData(hItem);
	if(m_pCurrentResInfo_ == NULL)
	{
		*pResult = 0;
		GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_OPEN_MAP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RUN_MAP)->ShowWindow(SW_HIDE);
		return;
	}

	if(m_pCurrentResInfo_->m_eResType == RES_ZONE || m_pCurrentResInfo_->m_eResType == RES_FIELD)
	{
		//init Preview
		CRect rect;
		PaintPreviewBkgAndPutRect_(rect);

		//check invalid resource
		if(m_pCurrentResInfo_->m_strNameToEdit.empty()) //작업중인 파일이 없음
		{
			GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_OPEN_MAP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_RUN_MAP)->ShowWindow(SW_HIDE);
			DrawTextMessage_("작업중인 맵이 없습니다.", true);
			return;
		}

		if (!m_bModal_)
		{
			//init button
			GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->SetWindowText("Quest Edit");
			GetDlgItem(IDC_BUTTON_OPEN_MAP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_OPEN_MAP)->SetWindowText("Open");
			GetDlgItem(IDC_BUTTON_RUN_MAP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_RUN_MAP)->SetWindowText("Map to run");
		}

		// 필드 정보 채우기
		if (m_pCurrentResInfo_->m_eResType == RES_FIELD)
		{
			m_nSelectedFieldID = m_pCurrentResInfo_->m_nID;
		}

		if(m_pCurrentResInfo_->m_strNameToView.empty()) //미니맵 없음
			DrawTextMessage_("Mini-map does not exist.", false);
		else
			DrawMiniMap_(rect);
	}
	else if(m_pCurrentResInfo_->m_eResType == RES_NPC)
	{
		//Init Preview Window
		CRect rect;
		PaintPreviewBkgAndPutRect_(rect);

		if(m_pCurrentResInfo_->m_strNameToView.empty())
		{
			GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_OPEN_MAP)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_BUTTON_RUN_MAP)->ShowWindow(SW_HIDE);
			DrawTextMessage_("NPC Text Window is EMPTY", true);
			return;
		}

		if (!m_bModal_)
		{
			GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->SetWindowText("No Model");
			GetDlgItem(IDC_BUTTON_OPEN_MAP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_OPEN_MAP)->SetWindowText("No Model");
			GetDlgItem(IDC_BUTTON_RUN_MAP)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_BUTTON_RUN_MAP)->SetWindowText("NPC List");
		}

		using namespace rs3;
		RSceneNode *pNode = CControlPreviewSceneManager::GetInstance()->LoadSceneNode(m_pCurrentResInfo_->m_strNameToView.c_str(), RSID_ACTOR);
		if ( pNode == NULL )
			return;
		
		CControlPreviewSceneManager::GetInstance()->UpdateRender(pNode);
		SAFE_DELETE(pNode);

		m_nSelectedNpcID = m_pCurrentResInfo_->m_nID;
		DrawNpcResource_(rect);
	}
	else if(m_pCurrentResInfo_->m_eResType == RES_TILE)
	{

		if(m_pCurrentResInfo_->m_strNameToView.empty())
			return;

		//init button
		GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_OPEN_MAP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RUN_MAP)->ShowWindow(SW_HIDE);

		if(DrawTileResource_())
			m_strSelectedTile = m_pCurrentResInfo_->m_strNameToView;
	}
	*pResult = 0;
}

void CDlgResourceInfo::PaintPreviewBkgAndPutRect_(CRect& _rect)
{
	CWnd* cwnd = GetDlgItem(IDC_STATIC_MINI_MAP_VIEW);
	CClientDC dc(cwnd);
	CRect rect;
	cwnd->GetClientRect(&rect);
	CBrush brush, *pOldBrush;
	brush.CreateSysColorBrush(COLOR_BTNFACE);
	pOldBrush = (CBrush*)dc.SelectObject(&brush);
	dc.Rectangle(&rect);
	dc.SelectObject(pOldBrush);

	//메시지
	DrawTextMessage_("Loading...", false);

	//put rect
	_rect = rect;
}

void CDlgResourceInfo::OnBnClickedButtonOpenMap()
{
	if(m_pCurrentResInfo_ ==  NULL)
		return;

	if(m_pCurrentResInfo_->m_eResType == RES_ZONE || m_pCurrentResInfo_->m_eResType == RES_FIELD)
	{
		//모래시계 시작
		HCURSOR hOld = SetCursor(LoadCursor(NULL, IDC_WAIT));

		((CCoreToolRS3Doc*)((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument())->OpenMapFile(m_pCurrentResInfo_->m_strNameToEdit.c_str());
		OnOK();

		//모래시계 끝
		SetCursor(hOld);
	}
	else if(m_pCurrentResInfo_->m_eResType == RES_NPC)
	{
		ShellExecute(NULL, "open", CXMLUserConfig::GetReference().GetTextEditor().c_str(), m_pCurrentResInfo_->m_strNameToEdit.c_str(), NULL, SW_SHOWNORMAL);
	}
}

void CDlgResourceInfo::OnBnClickedButtonRunMap()
{
	if(m_pCurrentResInfo_ ==  NULL)
		return;

	if(m_pCurrentResInfo_->m_eResType == RES_ZONE || m_pCurrentResInfo_->m_eResType == RES_FIELD)
	{
		//TODO
		//((CCoreToolRS3Doc*)((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument())->RunGameInMap(m_pCurrentResInfo_->m_strNameToEdit.c_str());
	}
	else if(m_pCurrentResInfo_->m_eResType == RES_NPC)
	{
		ShellExecute(NULL, "open", CXMLUserConfig::GetReference().GetTextEditor().c_str(), CXMLResourceList::GetReference().GetNpcListPath(), NULL, SW_SHOWNORMAL);
	}
}

void CDlgResourceInfo::OnBnClickedButtonEditQuest()
{
	char szParams[512] = {0,};
	sprintf_s(szParams, 512, "%d", this->m_nSelectedNpcID);

	ShellExecute(NULL, _T("open"), _T("../QuestTool/QuestTool.exe"), _T( szParams ), NULL, SW_SHOWNORMAL);

	// 다이얼로그 실행
	/*SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = "open";
	ShExecInfo.lpFile = "QuestTool.exe";		
	ShExecInfo.lpParameters = szParams;
	ShExecInfo.lpDirectory = "../QuestTool/";
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);*/
}


void CDlgResourceInfo::OnNMDblclkTreeMapRes(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hItem = m_ctrlExTreeMapRes_.GetSelectedItem();
	HTREEITEM hChild = m_ctrlExTreeMapRes_.GetNextItem(hItem, TVGN_CHILD);
	if(hChild == NULL) //리프일 경우
	{
		*pResult = 0;
		OnOK();
	}
}

void CDlgResourceInfo::SetResourceType(MAP_RES_TYPE _type)
{
	ResetTreeCtrl_();
	GetDlgItem(IDC_BUTTON_OPEN_DIALOG)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_OPEN_MAP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_BUTTON_RUN_MAP)->ShowWindow(SW_HIDE);

	switch( _type )
	{
		case RES_ALL:
		{
			//update tree
			UpdateTreeWithZoneList_();
			UpdateTreeWithFieldList_();
			UpdateTreeWithNpcList_();
			UpdateTreeWithTileList_();
			SetWindowText("맵 리소스 관리자");
		}
		break;
		case RES_ZONE:
		{
			//update tree
			UpdateTreeWithZoneList_();
			UpdateTreeWithFieldList_();
			UpdateTreeWithNpcList_();
			UpdateTreeWithTileList_();
			SetWindowText("맵 리소스 관리자");

			// 루트가 존
			m_ctrlExTreeMapRes_.Expand(m_ctrlExTreeMapRes_.GetRootItem(), TVE_EXPAND);
		}
		break;
		case RES_FIELD:
		{
			//update tree
			if (m_bModal_)
			{
				UpdateTreeWithFieldList_();
				SetWindowText("필드선택");

				m_ctrlExTreeMapRes_.Expand(m_ctrlExTreeMapRes_.GetRootItem(), TVE_EXPAND);
			}
			else
			{
				UpdateTreeWithZoneList_();
				UpdateTreeWithFieldList_();
				UpdateTreeWithNpcList_();
				UpdateTreeWithTileList_();
				SetWindowText("맵 리소스 관리자");

				HTREEITEM filedItem = m_ctrlExTreeMapRes_.GetNextItem( m_ctrlExTreeMapRes_.GetRootItem(), TVGN_NEXT );
				m_ctrlExTreeMapRes_.Expand(filedItem, TVE_EXPAND);
			}
		}
		break;
		case RES_TILE:
		{
			//update tree
			UpdateTreeWithTileList_();
			m_ctrlExTreeMapRes_.Expand(m_ctrlExTreeMapRes_.GetRootItem(), TVE_EXPAND);
			SetWindowText("타일맵 고르기");
		}
		break;
		case RES_NPC:
		{
			//update tree
			UpdateTreeWithNpcList_();
			m_ctrlExTreeMapRes_.Expand(m_ctrlExTreeMapRes_.GetRootItem(), TVE_EXPAND);
			SetWindowText("NPC 고르기");
		}
		break;
	}
}

void CDlgResourceInfo::ResetTreeCtrl_()
{
	HTREEITEM hParent = m_ctrlExTreeMapRes_.GetRootItem();
	while (hParent != NULL)
	{
		HTREEITEM hChild = m_ctrlExTreeMapRes_.GetNextItem(hParent, TVGN_CHILD);
		while (hChild != NULL)
		{
			MAP_RES_INFO* pInfo = (MAP_RES_INFO*)m_ctrlExTreeMapRes_.GetItemData(hChild);
			SAFE_DELETE(pInfo);
			hChild = m_ctrlExTreeMapRes_.GetNextSiblingItem(hChild);
		}
		hParent = m_ctrlExTreeMapRes_.GetNextSiblingItem(hParent);
	}
	m_ctrlExTreeMapRes_.DeleteAllItems();
	m_pCurrentResInfo_ = NULL;
}

bool CDlgResourceInfo::DrawTileResource_()
{
	rs3::RDeviceD3D *pRDevice = m_pControlMediator_->GetCurrentDevice();
	if(pRDevice == NULL)
		return false;

	//LPDIRECT3DDEVICE9 pDevice = pRDevice->GetD3DDevice();
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	//HRESULT hr = D3DXCreateTextureFromFile(pDevice, m_pCurrentResInfo_->m_strNameToView.c_str(), &pTexture);
	HRESULT hr = pRDevice->D3DXCreateTextureFromFile(m_pCurrentResInfo_->m_strNameToView.c_str(), &pTexture);
	if(SUCCEEDED(hr))
	{
		CWnd* pWnd = GetDlgItem(IDC_STATIC_MINI_MAP_VIEW);
		ASSERT(pWnd);
		CDC *pDC = pWnd->GetDC();
		ASSERT(pDC);
		HDC hDC = pDC->GetSafeHdc();
		ASSERT(hDC);

		RECT rt;
		pWnd->GetClientRect(&rt);
		CMFCHelper::D3DTextureToHDC(hDC, pTexture,&rt);
		pWnd->ReleaseDC(pDC);
		pTexture->Release();
		return true;
	}
	return false;
}

LRESULT CDlgResourceInfo::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CDialog::WindowProc(message, wParam, lParam);

	switch(message) 
	{
		case WM_ERASEBKGND:
			return FALSE;

		case WM_PAINT:
		{
			if(m_pControlMediator_ != NULL && m_pCurrentResInfo_ != NULL)
			{
				if(m_pCurrentResInfo_->m_eResType == RES_TILE)
					DrawTileResource_();
				else if(m_pCurrentResInfo_->m_eResType == RES_ZONE)
				{
					CWnd* cwnd = GetDlgItem(IDC_STATIC_MINI_MAP_VIEW);
					CRect rect;
					cwnd->GetClientRect(&rect);
					DrawMiniMap_(rect);
				}
				else if(m_pCurrentResInfo_->m_eResType == RES_NPC)
				{
					CWnd* cwnd = GetDlgItem(IDC_STATIC_MINI_MAP_VIEW);
					CRect rect;
					cwnd->GetClientRect(&rect);
					DrawNpcResource_(rect);
				}
			}
		}
		return FALSE;
	}
	return lResult;
}

bool CDlgResourceInfo::DrawMiniMap_(CRect& _refRect)
{
	CFile ImgFile;
	if(ImgFile.Open(m_pCurrentResInfo_->m_strNameToView.c_str(), CFile::modeRead))
	{
		m_miniMapDib_.Read(ImgFile);
		ImgFile.Close();

		int img_width = (int)m_miniMapDib_.m_pBMI->bmiHeader.biWidth;
		int img_height = (int)m_miniMapDib_.m_pBMI->bmiHeader.biHeight;

		float view_ratio = ((float)_refRect.Width())/((float)_refRect.Height());
		float img_ration = ((float)img_width)/((float)img_height);

		RECT DesRect, SrcRect;
		SrcRect.left = 0;
		SrcRect.top = 0;
		SrcRect.right = m_miniMapDib_.m_pBMI->bmiHeader.biWidth;
		SrcRect.bottom = m_miniMapDib_.m_pBMI->bmiHeader.biHeight;
		if(view_ratio < img_ration) //너비를 기준으로
		{
			float new_height = ((float)img_height) * ((float)_refRect.Width())/((float)img_width);
			float gap = ((float)(_refRect.Height() - new_height))/2;
			DesRect.top = _refRect.top + (int)gap;
			DesRect.right = _refRect.right;
			DesRect.left = _refRect.left;
			DesRect.bottom = DesRect.top + (int)(new_height);

		}
		else
		{
			float new_width = ((float)img_width) * ((float)_refRect.Height())/((float)img_height);
			float gap = ((float)(_refRect.Width() - new_width))/2;
			DesRect.top = _refRect.top;
			DesRect.left = _refRect.left + (int)gap;
			DesRect.right = _refRect.left + (int)(new_width);
			DesRect.bottom = _refRect.bottom;
		}
		CDC* pDC = GetDlgItem(IDC_STATIC_MINI_MAP_VIEW)->GetDC();
		m_miniMapDib_.Paint(pDC->GetSafeHdc(), &DesRect, &SrcRect);
		return true;
	}
	else
		return false;
}

bool CDlgResourceInfo::DrawNpcResource_(CRect& _refRect)
{
	// Preview 윈도우에 렌더링 한다.
	CWnd *pWnd = GetDlgItem(IDC_STATIC_MINI_MAP_VIEW);
	CDC *pDC = pWnd->GetDC();
	if ( pDC == NULL )
		return false;

	rs3::RDeviceD3D *pd3ddevice = m_pControlMediator_->GetCurrentDevice();
	ASSERT(pd3ddevice);

	HDC hDC = pDC->GetSafeHdc();
	if ( hDC == NULL )
		return false;

	rs3::RTexture* hTexture = CControlPreviewSceneManager::GetInstance()->GetRenderTexture();
	LPDIRECT3DBASETEXTURE9 lpTexture = pd3ddevice->GetD3DTexture(hTexture);
	_ASSERT(lpTexture->GetType()==D3DRTYPE_TEXTURE);

	CMFCHelper::D3DTextureToHDC(hDC,(LPDIRECT3DTEXTURE9)lpTexture,&_refRect);
	pWnd->ReleaseDC(pDC);
	return true;
}

void CDlgResourceInfo::DrawTextMessage_(const char* _pSzMessage, bool _bClearResource)
{
	CString cstrMessage(_pSzMessage);
	CWnd* pWnd = GetDlgItem(IDC_STATIC_MINI_MAP_VIEW);
	ASSERT(pWnd);
	CDC *pDC = pWnd->GetDC();
	pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	pDC->SetTextAlign(TA_CENTER | TA_TOP);
	pDC->TextOut(260, 150, cstrMessage);
	pWnd->ReleaseDC(pDC);
	if(_bClearResource)
		m_pCurrentResInfo_ = NULL;
}