// VD_DlgMapManager.cpp : 구현 파일입니다.
//

#pragma warning(disable:4311)
#include "stdafx.h"
#include "VD_DlgXmlListEditor.h"

#include "MLocale.h"
#include "C_ControlMediatorHelperResource.h"

#include <fstream>

// CDlgXmlListEditor 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgXmlListEditor, CDialog)

bool CDlgXmlListEditor::m_isActive = false;
CDlgXmlListEditor::CDlgXmlListEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgXmlListEditor::IDD, pParent)
{
	m_bZoneFieldMulti_ = false;
	CDlgXmlListEditor::m_isActive = true;
}

CDlgXmlListEditor::~CDlgXmlListEditor()
{
	CDlgXmlListEditor::m_isActive = false;
}

BOOL CDlgXmlListEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//init scinCtrl
	m_ScinCtrl_.Init();
	m_ScinCtrl_.SetDisplayLinenumbers(TRUE);
	m_ScinCtrl_.SetDisplayFolding(TRUE);
	m_ScinCtrl_.SetLexer(SCLEX_XML);
	m_ScinCtrl_.SendMessage(SCI_SETKEYWORDS, 0, (long)_T("zone zonelist maiet field fieldlist attribute element property type name default npc"));

	if(m_bZoneFieldMulti_)
	{
		char *tName[2] ={ ("존 리스트 (ZoneList.xml)"), ("필드 리스트 (FieldList.xml)") };
		TCITEM tabItem;
		tabItem.mask = TCIF_TEXT;
		for ( int i = 0; i < 2; ++i )
		{
			tabItem.pszText = tName[i];
			m_ctlListTab_.InsertItem(i,&tabItem);
		}

		if(m_eType_ == ZONE_LIST)
		{
			m_ScinCtrl_.SetText(m_strZoneContent_.c_str());
			m_ctlListTab_.SetCurSel(0);
			SetWindowText("존 리스트");
			GetDlgItem(ID_FIELDLIST_SAVE)->EnableWindow(FALSE);
		}
		else
		{
			m_ScinCtrl_.SetText(m_strFieldContent_.c_str());
			m_ctlListTab_.SetCurSel(1);
			SetWindowText("필드 리스트");
			GetDlgItem(ID_ZONELIST_SAVE)->EnableWindow(FALSE);
		}
	}
	else
	{
		TCITEM tabItem;
		tabItem.mask = TCIF_TEXT;

		switch( m_eType_ )
		{
			case ZONE_LIST :
				tabItem.pszText = "존 리스트 (ZoneList.xml)";
				m_ctlListTab_.InsertItem(0, &tabItem);
				SetWindowText("존 리스트");
			break;
			case FIELD_LIST :
				tabItem.pszText = "필드 리스트 (FieldList.xml)";
				m_ctlListTab_.InsertItem(0, &tabItem);
			break;
			case NPC_LIST :
				tabItem.pszText = "NPC 리스트 (npc.xml)";
				m_ctlListTab_.InsertItem(0, &tabItem);
			break;
			case PROPERTY_LIST :
				tabItem.pszText = "Property 리스트 (property.xml)";
				m_ctlListTab_.InsertItem(0, &tabItem);
			break;

		}

		m_ctlListTab_.SetCurSel(0);
		m_ScinCtrl_.SetText(m_strGeneralContent_.c_str());
		GetDlgItem(ID_ZONELIST_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(ID_FIELDLIST_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDOK)->SetWindowText("저장");
	}

	return TRUE;
}

void CDlgXmlListEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCINCTRL, m_ScinCtrl_);
	DDX_Control(pDX, IDC_TAB_MAP_EDIT, m_ctlListTab_);
}

void CDlgXmlListEditor::LoadXmlListFile( const char* _pZoneFile, const char* _pServerZoneFile, const char* _pFieldFile, LIST_TYPE _eType )
{
	m_bZoneFieldMulti_ = true;
	m_zoneFile_ = _pZoneFile;
	m_serverZoneFile_ = _pServerZoneFile;
	m_fieldFile_ = _pFieldFile;
	m_eType_ = _eType;
	LoadZoneList_();
	LoadFieldList_();
}

void CDlgXmlListEditor::LoadXmlListFile(const char* _pSzXmlList, LIST_TYPE _eType)
{
	m_eType_ = _eType;
	m_strGeneralFile_ = _pSzXmlList;
	MXml xml;
	xml.LoadFile(_pSzXmlList);
	MXmlPrinter printer;
	xml.Doc()->Accept(&printer);
	if (xml.IsUTF8Encoding())
	{
		m_strGeneralContent_ = MLocale::ConvUTF8ToAnsi(printer.CStr());
	}
	else
	{
		m_strGeneralContent_ = printer.CStr();
	}
}


void CDlgXmlListEditor::LoadZoneList_()
{
	MXmlDocument doc(m_zoneFile_.c_str());
	doc.LoadFile();
	MXmlPrinter printer;
	doc.Accept(&printer);
	m_strZoneContent_ = MLocale::ConvUTF8ToAnsi(printer.CStr());
}
void CDlgXmlListEditor::LoadFieldList_()
{
	MXmlDocument doc(m_fieldFile_.c_str());
	doc.LoadFile();
	MXmlPrinter printer;
	doc.Accept(&printer);
	m_strFieldContent_ = MLocale::ConvUTF8ToAnsi(printer.CStr());
}
BEGIN_MESSAGE_MAP(CDlgXmlListEditor, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAP_EDIT, OnTcnSelchangeTabList)
	ON_BN_CLICKED(ID_ZONELIST_SAVE, &CDlgXmlListEditor::OnBnClickedZonelistSave)
	ON_BN_CLICKED(ID_FIELDLIST_SAVE, &CDlgXmlListEditor::OnBnClickedFieldlistSave)
END_MESSAGE_MAP()


// CDlgXmlListEditor 메시지 처리기입니다.

BOOL CDlgXmlListEditor::OnNotify( 
								WPARAM wParam,       //@parm not used
								LPARAM lParam,       //@parm pointer to notification structure
								LRESULT* pResult )   //@parm result
{
	NMHDR *phDR;
	phDR = (NMHDR*)lParam;
	// does notification come from my scintilla window?
	if (phDR != NULL && phDR->hwndFrom == m_ScinCtrl_.m_hWnd)
	{
		SCNotification *pMsg = (SCNotification*)lParam;
		switch (phDR->code)
		{
			case SCN_STYLENEEDED:
			break;
			case SCN_CHARADDED:
			break;
			case SCN_SAVEPOINTREACHED:
			break;
			// called when the document is changed - mark document modified
			case SCN_SAVEPOINTLEFT:
			break;
			case SCN_MODIFYATTEMPTRO:
			break;
			case SCN_KEY:
			break;
			case SCN_DOUBLECLICK:
			break;
			// called when something changes and we want to show new indicator state or brace matching
			case SCN_UPDATEUI:
			{
				m_ScinCtrl_.UpdateUI();
			}
			break;
			case SCN_MODIFIED:
			break;
			case SCN_MACRORECORD:
			break;
			// user clicked margin - try folding action
			case SCN_MARGINCLICK:
			{
				m_ScinCtrl_.DoDefaultFolding(pMsg->margin, pMsg->position);
			}
			break;
			case SCN_NEEDSHOWN:
			break;
			case SCN_PAINTED:
			break;
			case SCN_USERLISTSELECTION:
			break;
			case SCN_URIDROPPED:
			break;
			case SCN_DWELLSTART:
			break;
			case SCN_DWELLEND:
			break;
		}
		return TRUE; // we processed the message
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

void CDlgXmlListEditor::OnTcnSelchangeTabList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_bZoneFieldMulti_)
	{
		int i_currentTab = m_ctlListTab_.GetCurFocus();
		if(i_currentTab == 0)
		{
			m_ScinCtrl_.GetText(m_strFieldContent_);
			m_ScinCtrl_.SetText(m_strZoneContent_.c_str());
			SetWindowText("지역 리스트");
			GetDlgItem(ID_ZONELIST_SAVE)->EnableWindow(TRUE);
			GetDlgItem(ID_FIELDLIST_SAVE)->EnableWindow(FALSE);
		}
		else
		{
			m_ScinCtrl_.GetText(m_strZoneContent_);
			m_ScinCtrl_.SetText(m_strFieldContent_.c_str());
			SetWindowText("필드 리스트");
			GetDlgItem(ID_ZONELIST_SAVE)->EnableWindow(FALSE);
			GetDlgItem(ID_FIELDLIST_SAVE)->EnableWindow(TRUE);
		}
		*pResult = 0;
		UpdateData(FALSE);
	}
}

void CDlgXmlListEditor::OnOK()
{
	int closed_window = IDYES;
	if(m_bZoneFieldMulti_)
	{
		int iResult = MessageBox("모든 맵 리스트를 저장하시겠습니까?",
										"경고 - 맵 리스트 편집.",
										MB_YESNO | MB_ICONASTERISK);
		switch ( iResult ){
		case IDNO:
			return;
		case IDYES:
			break;
		}

		m_bSaveGood_ = false;

		OnBnClickedZonelistSave();
		if(!m_bSaveGood_)
			return;

		OnBnClickedFieldlistSave();
		if(!m_bSaveGood_)
			return;
	}
	else
	{
		MXmlDocument doc;
		m_ScinCtrl_.GetText(m_strGeneralContent_);
		if (m_strGeneralContent_.find("encoding=\"UTF-8\"") != std::string::npos)
		{
			m_strGeneralContent_ = MLocale::ConvAnsiToUTF8(m_strGeneralContent_.c_str());
		}
		else
		{
			m_strGeneralContent_ = m_strGeneralContent_.c_str();
		}
		doc.Parse(m_strGeneralContent_.c_str());
		if(doc.Error())
			closed_window = AfxMessageBox("편집한 파일 리스트에 문제가 있습니다, 창을 닫으시겠습니까?", MB_ICONERROR | MB_YESNO);
		else
		{
			doc.SaveFile(m_strGeneralFile_);
			closed_window = AfxMessageBox("파일 리스트가 저장되었습니다, 창을 닫으시겠습니까?", MB_ICONINFORMATION | MB_YESNO);
			if (m_strGeneralFile_.find("list.xml") != std::string::npos)
			{
				CControlMediatorHelperResource::CopyToServerSystemFolder(m_strGeneralFile_.c_str());
			}
		}
	}

	if (IDYES == closed_window)
	{
		CDialog::OnOK();
	}
}

void CDlgXmlListEditor::OnBnClickedZonelistSave()
{
	int i_currentTab = m_ctlListTab_.GetCurFocus();
	if(i_currentTab == 0)
		m_ScinCtrl_.GetText(m_strZoneContent_);

	MXmlDocument doc;
	m_strZoneContent_ = MLocale::ConvAnsiToUTF8(m_strZoneContent_.c_str());
	doc.Parse(m_strZoneContent_.c_str());
	if(doc.Error())
		AfxMessageBox("편집한 존 파일 리스트에 문제가 있습니다.", MB_ICONERROR);
	else{
		doc.SaveFile(m_zoneFile_);
		doc.SaveFile(m_serverZoneFile_);
		AfxMessageBox("존 파일 리스트가 저장되었습니다.", MB_ICONINFORMATION);
		m_bSaveGood_ = true;
	}
}

void CDlgXmlListEditor::OnBnClickedFieldlistSave()
{
	int i_currentTab = m_ctlListTab_.GetCurFocus();
	if(i_currentTab == 1)
		m_ScinCtrl_.GetText(m_strFieldContent_);

	MXmlDocument doc;
	m_strFieldContent_ = MLocale::ConvAnsiToUTF8(m_strFieldContent_.c_str());
	doc.Parse(m_strFieldContent_.c_str());

	if(doc.Error())
	{
		AfxMessageBox("편집한 필드 파일 리스트에 문제가 있습니다.", MB_ICONERROR);
	}
	else
	{
		doc.SaveFile(m_fieldFile_);
		CControlMediatorHelperResource::CopyToServerSystemFolder(m_fieldFile_.c_str());
		AfxMessageBox("필드 파일 리스트가 저장되었습니다.", MB_ICONINFORMATION);
		m_bSaveGood_ = true;
	}
}
