// V_MapNewDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VL_MapNew.h"

#include "CoreToolRS3_Doc.h"

#include "VL_ResourceInfo.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperResource.h"

#include "M_ToolCmdMgr.h"
#include "C_XMLConfig.h"
#include "C_XMLUserConfig.h"
#include "C_XMLResourceList.h"
#include "VD_DlgCommonFileOpen.h"

#include "V_MFCHelper.h"
#include "VD_DlgInputMapName.h"

#include "WS_WorkSpaceManager.h"
#include "WS_Field.h"
#include "VD_DlgCutSceneNewMap.h"

#include "M_EntityObject.h"

// CDlgMapNew 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMapNew, CDialog)
CDlgMapNew::CDlgMapNew(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapNew::IDD, pParent)
{
	m_bActive_ = false;
	m_nZoneID_ = -1;
	m_nFromFieldID_ = -1;
}

CDlgMapNew::~CDlgMapNew()
{
}

BOOL CDlgMapNew::OnInitDialog(){
	CDialog::OnInitDialog();

	// 데이타 폴더
	char szfull[256]= "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	m_strDataPath_ = szfull;

	//리스트 채우기
	OnCbnDropdownComboZoneId();
	OnCbnDropdownComboFieldId();

	return TRUE;
}

void CDlgMapNew::EnableTemplateCtrl_(UINT _bool)
{
	CWnd::GetDlgItem(IDC_RADIO_600)->EnableWindow(_bool);
	CWnd::GetDlgItem(IDC_RADIO_800)->EnableWindow(_bool);
	CWnd::GetDlgItem(IDC_RADIO_CUSTOM)->EnableWindow(_bool);

	CWnd::GetDlgItem(IDC_RADIO_256)->EnableWindow(_bool);
	CWnd::GetDlgItem(IDC_RADIO_512)->EnableWindow(_bool);
	CWnd::GetDlgItem(IDC_RADIO_CUSTOM2)->EnableWindow(_bool);

	CWnd::GetDlgItem(IDC_EDIT_BASE_TILE)->EnableWindow(_bool);
	CWnd::GetDlgItem(IDC_BUTTON_BASE_TILE_FIND)->EnableWindow(_bool);

	//하위에서 Enable여부를 결정하므로 무조건 false로
	CWnd::GetDlgItem(IDC_EDIT_NEW_X)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_Y)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_XQ)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_YQ)->EnableWindow(FALSE);
}
void CDlgMapNew::EnableImportSceneCtrl_(UINT _bool)
{
	CWnd::GetDlgItem(IDC_EDIT_IMPORT_SCENE)->EnableWindow(_bool);
	CWnd::GetDlgItem(IDC_BUTTON_IMPORT_SCENE_FIND)->EnableWindow(_bool);
}

void CDlgMapNew::EnableZone_(UINT _bool)
{
	EnableTemplateCtrl_(_bool);
	EnableImportSceneCtrl_(_bool);
	CWnd::GetDlgItem(IDC_RADIO_TEMPLATE)->EnableWindow(_bool);
	CWnd::GetDlgItem(IDC_RADIO_IMPORT_SCENE)->EnableWindow(_bool);
	CWnd::GetDlgItem(IDC_COMBO_ZONE_ID)->EnableWindow(_bool);
	//CWnd::GetDlgItem(IDC_BUTTON_ZONE_MANAGER)->EnableWindow(_bool);
}
void CDlgMapNew::OnOK(){
	//int a = AfxMessageBox("메세지박스", MB_YESNOCANCEL);
	ShowWindow(SW_HIDE);
	m_bActive_ = false;
}

void CDlgMapNew::OnCancel(){
	ShowWindow(SW_HIDE);
	m_bActive_ = false;
}

void CDlgMapNew::Listen(CoreMessage& _message)
{
	if(_message.nID == VIEW_MESSAGE::ACTIVE_VIEW)
	{
		m_bActive_ = true;
		if(m_enumNewType_ == ZONE)
		{
			NewZoneType_(TRUE);
			ShowWindow(SW_SHOW);

		}
		else if (m_enumNewType_ == FIELD)
		{
			NewZoneType_(FALSE);
			ShowWindow(SW_SHOW);
		}
		else
		{
			CDlgCutSceneNewMap dlg(m_pControlMediator_);
			dlg.m_strFileName =  (CXMLConfig::GetInstance()->strDataFullPath + "CutScene\\TestCutScene.cutscene.xml").c_str();
			dlg.m_strName = "Describe This CutScene";
			dlg.m_nFieldId = 0;
			dlg.m_fDuration = 1.f;

			if (dlg.DoModal() == IDOK && dlg.MakeNewCutSceneFile())
			{
				m_pControlMediator_->GetWorkSpaceMgr()->OpenMapFileWithDoc(dlg.m_strFileName.GetString());
			}

			m_bActive_ = false;
		}
	}
}

void CDlgMapNew::NewZoneType_(BOOL _bool)
{
	//커트롤 위치 초기화
	m_nZoneType_ = 0;
	m_nSizeType_ = 1; //사용자 정의, 800*800
	m_nDetailType_ = 1; //512 X 512

	//field 리스트 채우기
	OnCbnDropdownComboZoneId();
	OnCbnDropdownComboFieldId();
	m_nFieldType_ = 1; //기존 지역으로 생성이 초기
	UpdateListBox_(m_nFieldType_ - 1); //지역에서 하나 작은것이 탭번호
	UpdateData(FALSE);

	int width=::GetSystemMetrics(SM_CXSCREEN);
	int height=::GetSystemMetrics(SM_CYSCREEN);

	CWnd* pZoneOk = GetDlgItem(IDOK_ZONE);
	CWnd* pZoneCancel = GetDlgItem(IDCANCEL_ZONE);

	int show_flag = _bool ? SW_SHOW : SW_HIDE;
	int reverse_bool = _bool ? FALSE : TRUE;

	pZoneOk->EnableWindow(_bool);
	pZoneOk->ShowWindow(show_flag);
	pZoneCancel->EnableWindow(_bool);
	pZoneCancel->ShowWindow(show_flag);
	GetDlgItem(IDOK)->EnableWindow(reverse_bool);

	if(_bool)
	{
		EnableZone_(TRUE);
		EnableImportSceneCtrl_(FALSE);
		SetWindowText("Create new Zone");
		SetWindowPos(NULL, width/2 - 185, height/2 - 210, 370, 420, SWP_NOZORDER);

		// 존 생성
		GetDlgItem(IDC_STATIC_ZONE_GROUP)->SetWindowText("Zone Created");
		m_ctlListBox_.ShowWindow(SW_HIDE);
	}
	else
	{
		EnableZone_(FALSE);
		SetWindowText("Create new Field");
		SetWindowPos(NULL, width/2 - 320, height/2 -210, 640, 420, SWP_NOZORDER);

		// 기본이 기존 존으로부터 생성이다.
		GetDlgItem(IDC_STATIC_ZONE_GROUP)->SetWindowText("Field Created");
		m_ctlListBox_.ShowWindow(SW_SHOW);
	}
}

void CDlgMapNew::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_TEMPLATE, m_nZoneType_);
	DDX_Radio(pDX, IDC_RADIO_600, m_nSizeType_);
	DDX_Radio(pDX, IDC_RADIO_256, m_nDetailType_);
	DDX_Radio(pDX, IDC_NEW_ZONE, m_nFieldType_);

	DDX_Text(pDX, IDC_EDIT_NEW_X, m_strNewX_);
	DDX_Text(pDX, IDC_EDIT_NEW_Y, m_strNewY_);
	DDX_Text(pDX, IDC_EDIT_NEW_XQ, m_strNewXQ_);
	DDX_Text(pDX, IDC_EDIT_NEW_YQ, m_strNewYQ_);
	DDX_Text(pDX, IDC_EDIT_BASE_TILE, m_strNewBaseTile_);
	DDX_Text(pDX, IDC_EDIT_IMPORT_SCENE, m_strImportSceneFile_);

	DDX_Control(pDX, IDC_LIST_MAP_CANDI, m_ctlListBox_);
	DDX_Control(pDX, IDC_COMBO_ZONE_ID, m_zoneComboBox_);
	DDX_Control(pDX, IDC_COMBO_FIELD_ID, m_fieldComboBox_);
}

BEGIN_MESSAGE_MAP(CDlgMapNew, CDialog)
	ON_BN_CLICKED(IDC_RADIO_TEMPLATE, &CDlgMapNew::OnBnClickedRadioTemplate)
	ON_BN_CLICKED(IDC_RADIO_IMPORT_SCENE, &CDlgMapNew::OnBnClickedRadioImportScene)
	ON_BN_CLICKED(IDC_RADIO_600, &CDlgMapNew::OnBnClickedRadio600)
	ON_BN_CLICKED(IDC_RADIO_800, &CDlgMapNew::OnBnClickedRadio800)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM, &CDlgMapNew::OnBnClickedRadioCustom)
	ON_BN_CLICKED(IDC_RADIO_256, &CDlgMapNew::OnBnClickedRadio256)
	ON_BN_CLICKED(IDC_RADIO_512, &CDlgMapNew::OnBnClickedRadio512)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM2, &CDlgMapNew::OnBnClickedRadioCustom2)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT_SCENE_FIND, &CDlgMapNew::OnBnClickedButtonImportSceneFind)
	ON_BN_CLICKED(IDC_BUTTON_BASE_TILE_FIND, &CDlgMapNew::OnBnClickedButtonBaseTileFind)
	ON_BN_CLICKED(IDOK_ZONE, &CDlgMapNew::OnBnClickedZoneOk)
	ON_BN_CLICKED(IDCANCEL_ZONE, &CDlgMapNew::OnBnClickedZoneCancel)
	ON_BN_CLICKED(IDOK, &CDlgMapNew::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgMapNew::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_NEW_ZONE, &CDlgMapNew::OnBnClickedNewZone)
	ON_BN_CLICKED(IDC_FROM_ZONE, &CDlgMapNew::OnBnClickedFromZone)
	ON_BN_CLICKED(IDC_FROM_FIELD, &CDlgMapNew::OnBnClickedFromField)

	//ON_BN_CLICKED(IDC_BUTTON_ZONE_MANAGER, &CDlgMapNew::OnBnClickedButtonZoneManager)
	ON_BN_CLICKED(IDC_BUTTON_FIELD_MANAGER, &CDlgMapNew::OnBnClickedButtonFieldListEditor)

	ON_CBN_DROPDOWN(IDC_COMBO_ZONE_ID, &CDlgMapNew::OnCbnDropdownComboZoneId)
	ON_CBN_DROPDOWN(IDC_COMBO_FIELD_ID, &CDlgMapNew::OnCbnDropdownComboFieldId)
END_MESSAGE_MAP()


// CDlgMapNew 메시지 처리기입니다.

void CDlgMapNew::OnBnClickedRadioTemplate()
{
	EnableTemplateCtrl_(TRUE);
	EnableImportSceneCtrl_(FALSE);
}

void CDlgMapNew::OnBnClickedRadioImportScene()
{
	EnableTemplateCtrl_(FALSE);
	EnableImportSceneCtrl_(TRUE);
}

void CDlgMapNew::OnBnClickedRadio600()
{
	CWnd::GetDlgItem(IDC_EDIT_NEW_X)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_Y)->EnableWindow(FALSE);
	UpdateData(TRUE);
}

void CDlgMapNew::OnBnClickedRadio800()
{
	CWnd::GetDlgItem(IDC_EDIT_NEW_X)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_Y)->EnableWindow(FALSE);
	UpdateData(TRUE);
}

void CDlgMapNew::OnBnClickedRadioCustom()
{
	CWnd::GetDlgItem(IDC_EDIT_NEW_X)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_Y)->EnableWindow(TRUE);
	UpdateData(TRUE);
}

void CDlgMapNew::OnBnClickedRadio256()
{
	CWnd::GetDlgItem(IDC_EDIT_NEW_XQ)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_YQ)->EnableWindow(FALSE);
	UpdateData(TRUE);
}

void CDlgMapNew::OnBnClickedRadio512()
{
	CWnd::GetDlgItem(IDC_EDIT_NEW_XQ)->EnableWindow(FALSE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_YQ)->EnableWindow(FALSE);
	UpdateData(TRUE);
}

void CDlgMapNew::OnBnClickedRadioCustom2()
{
	CWnd::GetDlgItem(IDC_EDIT_NEW_XQ)->EnableWindow(TRUE);
	CWnd::GetDlgItem(IDC_EDIT_NEW_YQ)->EnableWindow(TRUE);
	UpdateData(TRUE);
}

void CDlgMapNew::OnBnClickedButtonImportSceneFind()
{
	//초본 업데이트
	UpdateData(TRUE);

	// 임포트할 씬 선택 대화창
	CFileDialog Dlg(TRUE);
	Dlg.GetOFN().lpstrTitle = _T(".scene.xml Choose a file");
	Dlg.GetOFN().lpstrFilter = "scene.xml File(*.scene.xml)\0*.scene.xml";
	Dlg.GetOFN().Flags |= OFN_NOCHANGEDIR | OFN_READONLY;

	if ( Dlg.DoModal()==IDOK )
	{
		// 파일을 선택했다면 UI 업데이트
		m_strImportSceneFile_ = Dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CDlgMapNew::OnBnClickedButtonBaseTileFind()
{
	//초본 업데이트
	UpdateData(TRUE);

	CDlgResourceInfo dlg(m_pControlMediator_, CDlgResourceInfo::RES_TILE);
	if (dlg.DoModal()==IDOK )
	{
		if(dlg.m_strSelectedTile.empty())
			m_strNewBaseTile_ = "";
		else
			m_strNewBaseTile_ = dlg.m_strSelectedTile.c_str();

		//파일을 선택했다면 UI에 업데이트
		if(m_strNewBaseTile_.GetLength() != 0)
			UpdateData(FALSE);
	}
}

bool CDlgMapNew::IsValidNewZone_()
{
	// 새로운 지형 템플릿으로 부터 생성일 경우
	if(m_nZoneType_ == 0)
	{
		if(m_nSizeType_ == 0)
		{
			m_mapHeight_ = 60000;
			m_mapWidth_  = 60000;
		}
		else if(m_nSizeType_ == 1)
		{
			m_mapHeight_ = 80000;
			m_mapWidth_  = 80000;
		}
		else
		{
			char s[20];
			memset(s, 0, sizeof(s));
			GetDlgItemText(IDC_EDIT_NEW_X, s, sizeof(s));
			m_mapHeight_ = (float)atof(s) * 100;

			memset(s, 0, sizeof(s));
			GetDlgItemText(IDC_EDIT_NEW_Y, s, sizeof(s));
			m_mapWidth_ = (float)atof(s) * 100;

			if(m_mapWidth_ != m_mapHeight_)
			{
				AfxMessageBox("Zone Size must be a square.");
				return false;
			}
		}

		if(m_nDetailType_ == 0)
		{
			m_colFace_ = 256;
			m_rowFace_ = 256;
		}
		else if(m_nDetailType_ == 1)
		{
			m_colFace_ = 512;
			m_rowFace_ = 512;
		}
		else
		{
			char s[20];
			memset(s, 0, sizeof(s));
			GetDlgItemText(IDC_EDIT_NEW_XQ, s, sizeof(s));
			m_colFace_ = (int)atof(s);

			memset(s, 0, sizeof(s));
			GetDlgItemText(IDC_EDIT_NEW_YQ, s, sizeof(s));
			m_rowFace_ = (int)atof(s);

			if(m_colFace_ != m_rowFace_)
			{
				AfxMessageBox("Zone must be square.");
				return false;
			}

			double fWidth = (double)(m_colFace_);
			double fV1 = _logb(fWidth);
			double fV2 = pow(2.0,fV1);
			if ( fV2 != fWidth )
			{
				AfxMessageBox("존의 디테일은 2의 거듭제곱꼴이여야 합니다.");
				return false;
			}
		}

		//base tile 설정
		CString baseTile;
		GetDlgItemText(IDC_EDIT_BASE_TILE, baseTile);
		baseTile.MakeLower();
		CString validPath(CXMLResourceList::GetReference().GetTileFolderPath());
		validPath.MakeLower();
		int test = baseTile.Find(validPath);
		if(test < 0)
		{
			AfxMessageBox("베이스 타일이 잘못 되어 있습니다. \n베이스 타일 위치가 설정에 있는 타일 위치와 다를수 있습니다.");
			return false;
		}
		m_strMapBaseTile_ = (LPTSTR)(LPCTSTR)baseTile;
	}
	else // 새로운 씬으로부터 임포트하여 생성할 경우
	{
		if( m_strImportSceneFile_.IsEmpty() )
		{
			AfxMessageBox("임포트할 씬을 선택하지 않았습니다.");
			return false;
		}
	}

	//지역정보 설정
	int a = m_zoneComboBox_.GetCurSel();
	if(a < 0)
	{
		AfxMessageBox("Zone ID not set.");
		return false;
	}
	CString str_combo;
	m_zoneComboBox_.GetLBText(a, str_combo);
	if(str_combo.GetLength() == 0)
	{
		AfxMessageBox("Zone ID not set.");
		return false;
	}
	//테스트 지역파일일 경우
	if(str_combo.Compare("Test Zone")  == 0)
	{
		CDlgInputMapName dlg;
		dlg.m_strDlgCaption = "테스트 존파일 설정";
		if (IDOK == dlg.DoModal())
		{
			m_strZonePath_ = (LPTSTR)(LPCTSTR)(dlg.m_cstrTestDirectory + "\\");
			m_strZoneFile_ = (LPTSTR)(LPCTSTR)dlg.m_cstrTestFile;
			if(GetFileAttributes((m_strZonePath_ + m_strZoneFile_).c_str()) != INVALID_FILE_ATTRIBUTES)
			{
				AfxMessageBox("Zone already exists.");
				return false;
			}
			m_nZoneID_ = -1;
			return true;
		}
		else
		{
			AfxMessageBox("테스트 존일 경우 해당 경로와 파일을 설정하셔야 합니다.");
			return false;
		}
	}
	else
	{
		// "//"을 기준으로 토크닝
		int b = str_combo.Find("//");
		int c = str_combo.GetLength();

		std::string strID(str_combo.GetString());
		int start_id = strID.find(":");
		int end_id = strID.find(")");
		m_nZoneID_ = atoi(strID.substr(start_id+1, end_id - start_id-1).c_str());

		CString newZoneId = str_combo.Right(c-b-2);
		CString fullPath(m_strDataPath_.c_str());
		fullPath = fullPath + "zone\\";
		fullPath = fullPath + newZoneId;
		if(GetFileAttributes(fullPath) == INVALID_FILE_ATTRIBUTES)
		{
			AfxMessageBox("해당 존 ID의 폴더가 존재하지 않습니다. 생성후 다시 시도하세요.");
			return false;
		}
		m_strZonePath_ = (LPTSTR)(LPCTSTR)(fullPath + "\\");
		newZoneId += ".zone.xml";
		m_strZoneFile_ = (LPTSTR)(LPCTSTR)(newZoneId);
		if(GetFileAttributes((m_strZonePath_ + m_strZoneFile_).c_str()) != INVALID_FILE_ATTRIBUTES)
		{
			AfxMessageBox("Zone already exists.");
			return false;
		}
		return true;
	}
}

bool CDlgMapNew::IsValidNewMapInfo_()
{
	UpdateData(TRUE);
	
	if( m_enumNewType_ == ZONE) //zone 생성
		return IsValidNewZone_();
	else //field 생성
	{
		//만들 필드 ID
		int field_id = m_fieldComboBox_.GetCurSel();
		if(field_id < 0)
		{
			AfxMessageBox("필드 아이디가 설정되지 않았습니다.");
			return false;
		}

		CString str_combo;
		m_fieldComboBox_.GetLBText(field_id, str_combo);
		std::string strID(str_combo.GetString());
		int start_id = strID.find(":");
		int end_id = strID.find(")");
		int newFieldID = atoi(strID.substr(start_id+1, end_id - start_id-1).c_str());

		//zone_id : field_id 비교하여 field 생성
		//a:0 - 새로운 존으로부터 생성
		//-1,a - 기존 필드로 부터 생성
		// a, -1 기존 존으로 부터 생성
		if(m_nFieldType_ == 0) //create with new zone
		{
			//기본적인 존 생성조건 확인
			if(IsValidNewZone_() ==false)
				return false;
			if(m_nZoneID_ == -1)
			{
				AfxMessageBox("Zone ID not set.");
				return false;
			}
			std::string zone_name(m_strZonePath_ + m_strZoneFile_);
			m_nFromFieldID_ = 0;
		}
		//기존에 것들로 부터 생성 - 존 혹은 필드
		else
		{
			int selected_list_index = m_ctlListBox_.GetCurSel();
			if(selected_list_index < 0 )
			{
				AfxMessageBox("Incorrect list selection.");
				return false;
			}

			//기존의 존으로 부터 생성
			if(m_nFieldType_ == 1)
			{
				CString cstrZone;
				m_ctlListBox_.GetText(selected_list_index, cstrZone);
				std::string strZoneId(cstrZone.GetString());
				int start_id = strZoneId.find(":");
				int end_id = strZoneId.find(")");
				m_nZoneID_ = atoi(strZoneId.substr(start_id+1, end_id - start_id-1).c_str());
				m_nFromFieldID_ = -1;
			}
			//기존의 필드로 부터 생성
			else
			{
				m_nZoneID_ = -1;

				CString cstrField;
				m_ctlListBox_.GetText(selected_list_index, cstrField);
				std::string strFieldId(cstrField.GetString());
				int start_id = strFieldId.find(":");
				int end_id = strFieldId.find(")");
				m_nFromFieldID_ = atoi(strFieldId.substr(start_id+1, end_id - start_id-1).c_str());
			}
		}

		CString fullPath(m_strDataPath_.c_str());
		fullPath = fullPath + "field\\";
		if(GetFileAttributes(fullPath) == INVALID_FILE_ATTRIBUTES)
		{
			AfxMessageBox("해당 필드의 폴더가 존재하지 않습니다. 생성후 다시 시도하세요.");
			return false;
		}
		CString newFieldName;
		newFieldName.Format("%05d", newFieldID);
		newFieldName = fullPath + newFieldName + ".field.xml";
		if(GetFileAttributes(newFieldName) != INVALID_FILE_ATTRIBUTES)
		{
			AfxMessageBox("Field already exists.");
			return false;
		}
		m_strFieldName_ = newFieldName.GetString();
		return true;
	}
}

//PutNewZoneInfoForCreation_()로 부터 존 생성이 필요한 정보 담김.
//NewField는 ID로 부터 생성후 map_info를 채운다.
void CDlgMapNew::PutNewZoneInfoForCreation_()
{
	// 새로운 지형으로 생성
	if( m_nZoneType_ == 0 )
	{
		// 아웃도어 정보
		m_pControlMediator_->m_mapInfo.m_height = m_mapHeight_;
		m_pControlMediator_->m_mapInfo.m_width = m_mapWidth_;
		m_pControlMediator_->m_mapInfo.m_colFace = m_colFace_;
		m_pControlMediator_->m_mapInfo.m_rowFace = m_rowFace_;
		m_pControlMediator_->m_mapInfo.m_strTerrainBaseTile = m_strMapBaseTile_;
		m_pControlMediator_->m_mapInfo.m_strImportSceneFile.clear();
	}
	else // 임포트로 부터 생성 : Indoor
	{
		// 인도어 정보
		m_pControlMediator_->m_mapInfo.m_strImportSceneFile = m_strImportSceneFile_.GetString();
	}

	m_pControlMediator_->m_mapInfo.m_eMapEditType = MAP_EDIT_TYPE_SCENE;
	m_pControlMediator_->m_mapInfo.m_strSceneFile = m_strZoneFile_;
	m_pControlMediator_->m_mapInfo.m_strScenePath = m_strZonePath_;

	// init logic object
	m_pControlMediator_->GetToolLogicObjectMgr()->InitLogicObjectManager<CEntityObject>();
}

bool CDlgMapNew::IsCreateAtLoaded_()
{
	int iResult;
	if (m_pControlMediator_->GetToolCmdMgr()->IsMapChanged() == false )
	{
		iResult = IDNO;
	}
	else
	{
		iResult = MessageBox("Loaded map exists. Would you like to save?",
									"Warning - Map Exists",
									MB_YESNOCANCEL | MB_ICONASTERISK);
	}
	
	switch ( iResult )
	{
	case IDCANCEL:
		return false;
	case IDYES:
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_SAVEALL);

		// reset resource
		m_pControlMediator_->ResetResource();
		m_pControlMediator_->GetHelperResource()->DeleteTempFile();

		// put information
		PutNewZoneInfoForCreation_();
		if(m_enumNewType_ == ZONE)
		{
			m_pControlMediator_->CreateZone();
		}
		else
		{
			CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
			pField->SetFieldCreationInfo(m_nZoneID_, m_nFromFieldID_, m_strFieldName_.c_str());
			m_pControlMediator_->CreateWorkSpaceFile(pField->GetWorkSpaceKeyword(), MAP_EDIT_TYPE_LOGIC);
		}
		SetToolAfterCreationMap_(m_enumNewType_);
		return true;
	}
	case IDNO:

		m_pControlMediator_->ResetResource();
		m_pControlMediator_->GetHelperResource()->DeleteTempFile();

		PutNewZoneInfoForCreation_();
		if(m_enumNewType_ == ZONE)
		{
			m_pControlMediator_->CreateZone();
		}
		else
		{
			CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
			pField->SetFieldCreationInfo(m_nZoneID_, m_nFromFieldID_, m_strFieldName_.c_str());
			m_pControlMediator_->CreateWorkSpaceFile(pField->GetWorkSpaceKeyword(), MAP_EDIT_TYPE_LOGIC);
		}
		SetToolAfterCreationMap_(m_enumNewType_);
		return true;
	}

	AfxMessageBox("로딩된후 맵 정책이 중간에 선택되지 않고 리턴되었습니다.");
	return false;
}

void CDlgMapNew::OnBnClickedZoneOk()
{
	if(!IsValidNewMapInfo_())
		return;

	if(m_pControlMediator_->m_mapInfo.m_bLoaded)
	{
		if(!IsCreateAtLoaded_())
			return;
	}
	else
	{
		PutNewZoneInfoForCreation_();
		m_pControlMediator_->CreateZone();
		SetToolAfterCreationMap_(ZONE);
	}
	ShowWindow(SW_HIDE);
	m_bActive_ = false;
}

void CDlgMapNew::OnBnClickedZoneCancel()
{
	ShowWindow(SW_HIDE);
	m_bActive_ = false;
}

//필드쪽
void CDlgMapNew::OnBnClickedOk()
{
	if(!IsValidNewMapInfo_())
		return;

	if(m_pControlMediator_->m_mapInfo.m_bLoaded)
	{
		if(!IsCreateAtLoaded_())
			return;
	}
	else
	{
		PutNewZoneInfoForCreation_();

		CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
		_ASSERT(pField);
		pField->SetFieldCreationInfo(m_nZoneID_, m_nFromFieldID_, m_strFieldName_.c_str());
		m_pControlMediator_->CreateWorkSpaceFile(pField->GetWorkSpaceKeyword(), MAP_EDIT_TYPE_LOGIC);
		SetToolAfterCreationMap_(FIELD);
	}

	//TODO indoor field를 만들때
	m_pControlMediator_->GetWorkSpaceMgr()->ChagneWorkSpaceWithMainFrame("field.xml", NULL);
	ShowWindow(SW_HIDE);
	m_bActive_ = false;
}

void CDlgMapNew::OnBnClickedCancel()
{
	ShowWindow(SW_HIDE);
	m_bActive_ = false;
}

void CDlgMapNew::OnBnClickedNewZone()
{
	EnableZone_(TRUE);
	GetDlgItem(IDC_STATIC_ZONE_GROUP)->SetWindowText("Zone created");
	EnableImportSceneCtrl_(FALSE);
	m_ctlListBox_.ShowWindow(SW_HIDE);
	UpdateData(TRUE);
}

void CDlgMapNew::OnBnClickedFromZone()
{
	EnableZone_(FALSE);
	GetDlgItem(IDC_STATIC_ZONE_GROUP)->SetWindowText("Zone created from Field");
	m_ctlListBox_.ShowWindow(SW_SHOW);
	UpdateListBox_(0);
	UpdateData(TRUE);
}

void CDlgMapNew::OnBnClickedFromField()
{
	EnableZone_(FALSE);
	GetDlgItem(IDC_STATIC_ZONE_GROUP)->SetWindowText("Field to be copied");
	m_ctlListBox_.ShowWindow(SW_SHOW);
	UpdateListBox_(1);
	UpdateData(TRUE);
}

void CDlgMapNew::UpdateListBox_(int nIndex)
{
	m_ctlListBox_.ResetContent();
	for ( int i = 0; i<(int)m_listBoxArray_[nIndex].size(); ++i )
	{
		m_ctlListBox_.InsertString(i, m_listBoxArray_[nIndex][i].c_str());
	}
}

void CDlgMapNew::OnBnClickedButtonFieldListEditor()
{
	int nListIndex = m_nFieldType_ -1;
	if (nListIndex == 1) //필드로부터 생성
		CMFCHelper::OpenFieldListEditorWithZone();
	else
		CMFCHelper::OpenZoneListEditorWithField();

	OnCbnDropdownComboZoneId();
	OnCbnDropdownComboFieldId();

	if (nListIndex >= 0 )
		UpdateListBox_(nListIndex);
}

void CDlgMapNew::OnCbnDropdownComboZoneId()
{
	CXMLResourceList::GetReference().LoadZoneList();
	CXMLResourceList::GetReference().PutComboBox(m_zoneComboBox_, CXMLResourceList::ZONE_LIST);
	PutListStringFromComboBox_(m_listBoxArray_[0], m_zoneComboBox_);
}

void CDlgMapNew::OnCbnDropdownComboFieldId()
{
	CXMLResourceList::GetReference().LoadFieldList();
	CXMLResourceList::GetReference().PutComboBox(m_fieldComboBox_, CXMLResourceList::FIELD_LIST);
	PutListStringFromComboBox_(m_listBoxArray_[1], m_fieldComboBox_);
}

void CDlgMapNew::PutListStringFromComboBox_(std::vector<string>& _refVec, CComboBox& _refCombo)
{
	_refVec.clear();

	CString comboContent;
	int combo_num = _refCombo.GetCount() - 1; //콤보박스에는 테스트 영역까지 들어가 있다.
	for(int a = 0; a < combo_num; ++a)
	{
		_refCombo.GetLBText(a, comboContent);
		_refVec.push_back((LPTSTR)(LPCTSTR)comboContent);
	}
}

void CDlgMapNew::SetToolAfterCreationMap_(MAP_NEW_TYPE _type)
{
	m_pControlMediator_->GetHelperResource()->ResetRefMesh(CXMLUserConfig::GetReference().GetRefMeshName());
	if(_type == ZONE)
	{
		m_pControlMediator_->GetWorkSpaceMgr()->ChagneWorkSpaceWithMainFrame
		(
			"zone.xml",
			(m_pControlMediator_->m_mapInfo.m_strScenePath + m_pControlMediator_->m_mapInfo.m_strSceneFile).c_str()
		);
	}
	else
	{
		CWorkSpaceField* pField = m_pControlMediator_->GetWorkSpacePtr<CWorkSpaceField>();
		std::string fullPath(pField->m_fieldInfo.m_strFieldPath + pField->m_fieldInfo.m_strFieldFile);
		m_pControlMediator_->GetWorkSpaceMgr()->ChagneWorkSpaceWithMainFrame
		(
			"field.xml",
			fullPath.c_str()
		);
	}
}
