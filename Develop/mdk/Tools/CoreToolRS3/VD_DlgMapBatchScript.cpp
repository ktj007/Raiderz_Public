// VD_DlgMapBatchScript.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "CoreToolRS3.h"
#include "VD_DlgMapBatchScript.h"

#include "C_XMLResourceList.h"
#include "C_XMLConfig.h"

// CDlgMapBatchScript 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMapBatchScript, CDialog)

CDlgMapBatchScript::CDlgMapBatchScript(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapBatchScript::IDD, pParent)
{

}

CDlgMapBatchScript::~CDlgMapBatchScript()
{
}

BOOL CDlgMapBatchScript::OnInitDialog(){
	CDialog::OnInitDialog();

	// Tab 정보 Init
	const int NUM_OF_TAB = 2;
	char *tName[NUM_OF_TAB] ={ ("존 목록"), ("필드 목록") };
	TCITEM tabItem;
	tabItem.mask = TCIF_TEXT;
	for ( int i = 0; i < NUM_OF_TAB; ++i )
	{
		tabItem.pszText = tName[i];
		m_ctlMapTab_.InsertItem(i,&tabItem);
	}

	//List controller
	ListView_SetExtendedListViewStyleEx ( m_ctlMapFileList_.GetSafeHwnd(),
									LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );
	m_ctlMapFileList_.InsertColumn ( 0, _T("List"), LVCFMT_LEFT, 180);

	//인포메이션
	m_contentLabel_.Create("맵을 선택하세요.", WS_CHILD | WS_VISIBLE | SS_CENTER,
		CRect(300, 150, 520, 200), this, 0x188);
	m_contentFont_.CreatePointFont(120, "굴림");
	m_contentLabel_.SetFont(&m_contentFont_);

	//update list
	UpdateMapFileList_( m_ctlMapTab_.GetCurFocus());

	return TRUE;
}

void CDlgMapBatchScript::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAP_TYPE, m_ctlMapTab_);
	DDX_Control(pDX, IDC_LIST_MAP_FILES, m_ctlMapFileList_);
	DDX_Control(pDX, IDC_COMBO_MAP_SCRPIT, m_ctlScriptCombo_);
}


BEGIN_MESSAGE_MAP(CDlgMapBatchScript, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAP_TYPE, OnTcnSelchangeTabMapFileList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAP_FILES, OnLvnItemchangedMapFileList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_MAP_FILES, OnCustomListDraw)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, &CDlgMapBatchScript::OnBnClickedButtonSelectAll)
	ON_BN_CLICKED(IDOK, &CDlgMapBatchScript::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgMapBatchScript 메시지 처리기입니다.

void CDlgMapBatchScript::OnTcnSelchangeTabMapFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateMapFileList_( m_ctlMapTab_.GetCurFocus());
	*pResult = 0;
}

void CDlgMapBatchScript::UpdateMapFileList_(int _nIndex)
{
	CRect rect;
	PaintPreviewBkgAndPutRect_(rect);
	m_contentLabel_.SetWindowText("맵을 선택하세요");

	char szfull[256]= "";
	_fullpath( szfull, CXMLConfig::GetInstance()->pcDataPath, 256 );
	std::string filePath(szfull);
	if(_nIndex == ZONE_TAB)
	{
		CXMLResourceList::GetReference().LoadZoneList();
		CXMLResourceList::GetReference().PutListCtrl(m_ctlMapFileList_, m_mapFileNameIfExist_, CXMLResourceList::ZONE_LIST);
	}
	else
	{
		CXMLResourceList::GetReference().LoadFieldList();
		CXMLResourceList::GetReference().PutListCtrl(m_ctlMapFileList_, m_mapFileNameIfExist_, CXMLResourceList::FIELD_LIST);
	}
	UpdateScriptComboBox_(_nIndex);
}

void CDlgMapBatchScript::OnCustomListDraw( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

	// Take the default processing unless we set this to something else below.
	*pResult = CDRF_DODEFAULT;

	// First thing - check the draw stage. If it's the control's prepaint
	// stage, then tell Windows we want messages for every item.

	if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
		// This is the prepaint stage for an item. Here's where we set the
		// item's text color. Our return value will tell Windows to draw the
		// item itself, but it will use the new color we set here.

		int nItem = pLVCD->nmcd.dwItemSpec;
		MAP_INDEX_STRING::iterator it;
		it = m_mapFileNameIfExist_.find(nItem);

		//파일이 존재하지 않으면 그 리스트의 색깔은 다른색으로 칠해준다.
		if (it == m_mapFileNameIfExist_.end())
		{
			//COLORREF crText = RGB(255,255,255);
			//COLORREF crTextBk = RGB(128,0,0);
			//pLVCD->clrTextBk = crTextBk;
			//pLVCD->clrText = crText;
			pLVCD->clrText = RGB(255,0,0);
		}

		// Tell Windows to paint the control itself.
		*pResult = CDRF_DODEFAULT;
	}
}

void CDlgMapBatchScript::OnLvnItemchangedMapFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int i = pNMLV->iItem;
	//item changed
	if((pNMLV->uChanged & LVIF_STATE) && (pNMLV->uNewState & (LVIS_SELECTED|LVIS_FOCUSED)) && i > -1)
	{
		MAP_INDEX_STRING::iterator it;
		it = m_mapFileNameIfExist_.find(i);

		//배경색으로 칠해주기
		CRect rect;
		PaintPreviewBkgAndPutRect_(rect);
		if (it != m_mapFileNameIfExist_.end() && !it->second.first.empty())
		{
			//ToDo, Multi Selection
			//m_nSelectedMapListIndex_ = i;
			CFile ImgFile;
			if(ImgFile.Open(it->second.second.c_str(), CFile::modeRead))
			{
				m_miniMapDib_.Read(ImgFile);
				ImgFile.Close();

				int img_width = (int)m_miniMapDib_.m_pBMI->bmiHeader.biWidth;
				int img_height = (int)m_miniMapDib_.m_pBMI->bmiHeader.biHeight;

				float view_ratio = ((float)rect.Width())/((float)rect.Height());
				float img_ration = ((float)img_width)/((float)img_height);

				RECT DesRect, SrcRect;
				SrcRect.left = 0;
				SrcRect.top = 0;
				SrcRect.right = m_miniMapDib_.m_pBMI->bmiHeader.biWidth;
				SrcRect.bottom = m_miniMapDib_.m_pBMI->bmiHeader.biHeight;
				if(view_ratio < img_ration) //너비를 기준으로
				{
					float new_height = ((float)img_height) * ((float)rect.Width())/((float)img_width);
					float gap = ((float)(rect.Height() - new_height))/2;
					DesRect.top = rect.top + (int)gap;
					DesRect.right = rect.right;
					DesRect.left = rect.left;
					DesRect.bottom = DesRect.top + (int)(new_height);

				}
				else
				{
					float new_width = ((float)img_width) * ((float)rect.Height())/((float)img_height);
					float gap = ((float)(rect.Width() - new_width))/2;
					DesRect.top = rect.top;
					DesRect.left = rect.left + (int)gap;
					DesRect.right = rect.left + (int)(new_width);
					DesRect.bottom = rect.bottom;
				}
				CDC* pDC = GetDlgItem(IDC_STATIC_MAP_PREVIEW)->GetDC();
				m_miniMapDib_.Paint(pDC->GetSafeHdc(), &DesRect, &SrcRect);
			}
			else
				m_contentLabel_.SetWindowText("미니맵이 존재하지 않습니다.");
		}
		else
		{
			m_contentLabel_.SetWindowText("선택된 맵의 작업이 없습니다.");
		}
	}
	*pResult = 0;
}

void CDlgMapBatchScript::PaintPreviewBkgAndPutRect_(CRect& _rect)
{
	CWnd* cwnd = GetDlgItem(IDC_STATIC_MAP_PREVIEW);
	CClientDC dc(cwnd);
	CRect rect;
	cwnd->GetClientRect(&rect);
	CBrush brush, *pOldBrush;
	brush.CreateSysColorBrush(COLOR_BTNFACE);
	pOldBrush = (CBrush*)dc.SelectObject(&brush);
	dc.Rectangle(&rect);
	dc.SelectObject(pOldBrush);

	//put rect
	_rect = rect;
}

void CDlgMapBatchScript::OnBnClickedButtonSelectAll()
{
	m_ctlMapFileList_.SetFocus();
	int nItemNum = m_ctlMapFileList_.GetItemCount();
	for(int i = 0; i < nItemNum; ++i)
	{
		m_ctlMapFileList_.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
	m_contentLabel_.SetWindowText("전체선택");
}

void CDlgMapBatchScript::UpdateScriptComboBox_(int _nIndex)
{
	m_ctlScriptCombo_.ResetContent();

	CFileFind finder;
	if(_nIndex == ZONE_TAB)
	{
		BOOL bWorking = finder.FindFile("ruby/map/zone/*.rb");
		while(bWorking)
		{
			bWorking = finder.FindNextFile();
			m_ctlScriptCombo_.AddString(finder.GetFileName());
		}
	}
	else if(_nIndex == FIELD_TAB)
	{
		BOOL bWorking = finder.FindFile("ruby/map/field/*.rb");
		while(bWorking)
		{
			bWorking = finder.FindNextFile();
			m_ctlScriptCombo_.AddString(finder.GetFileName());
		}
	}
	else
		return;

	m_ctlScriptCombo_.SetCurSel(0);
}

void CDlgMapBatchScript::OnBnClickedOk()
{
	m_vecSelectedMap.clear();

	//선택한것을 selected map에 넣기
	int nItemNum = m_ctlMapFileList_.GetItemCount();
	for(int i = 0; i < nItemNum; ++i)
	{
		BOOL bState = m_ctlMapFileList_.GetItemState(i, LVIS_SELECTED);
		if(bState)
		{
			MAP_INDEX_STRING::iterator itr = m_mapFileNameIfExist_.find(i);
			if(itr != m_mapFileNameIfExist_.end())
			{
				//페어의 처음이 파일이름, 두번째가 미니맵 이름
				m_vecSelectedMap.push_back(itr->second.first);
			}
		}
	}

	//선택한 스크립트 파일
	int nSelectedCombo = m_ctlScriptCombo_.GetCurSel();
	if(nSelectedCombo > -1)
	{
		CString cstrScriptName;
		m_ctlScriptCombo_.GetLBText(nSelectedCombo, cstrScriptName);
		int nCurrentTab = m_ctlMapTab_.GetCurSel();
		if(nCurrentTab == ZONE_TAB)
		{
			cstrScriptName = "ruby/map/zone/" + cstrScriptName;
			m_strSelectedScript = cstrScriptName.GetString();
		}
		else if(nCurrentTab == FIELD_TAB)
		{
			cstrScriptName = "ruby/map/field/" + cstrScriptName;
			m_strSelectedScript = cstrScriptName.GetString();
		}
	}
	OnOK();
}
