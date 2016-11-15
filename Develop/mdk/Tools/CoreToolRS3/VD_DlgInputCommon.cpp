// NameInput.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VD_DlgInputCommon.h"
#include "M_XMLDataStructure.h"


// CDlgInputCommon 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInputCommon, CDialog)

CDlgInputCommon::CDlgInputCommon(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInputCommon::IDD, pParent)
{
	CDlgInputCommon::m_isActive = true;
}

bool CDlgInputCommon::m_isActive = false;
CDlgInputCommon::CDlgInputCommon(const char* _pSzValue, const char* _pSzWindowName, const char* _pSzExtrType)
	: CDialog(CDlgInputCommon::IDD, NULL), m_cstrWindowName_(_pSzWindowName)
{
	m_cstrCommonValue_ = _pSzValue;
	CDlgInputCommon::m_isActive = true;
	m_cstrComboType_ = _pSzExtrType;
}

CDlgInputCommon::~CDlgInputCommon()
{
	CDlgInputCommon::m_isActive = false;
}

BOOL CDlgInputCommon::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(!m_cstrWindowName_.IsEmpty())
		SetWindowText(m_cstrWindowName_);

	if (m_cstrComboType_.IsEmpty())
	{
		GetDlgItem(IDC_EDIT_COMMON_VALUE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_COMMON_COMBO)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_COMMON_VALUE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_COMMON_COMBO)->ShowWindow(SW_SHOW);

		const std::vector< std::string >* pComboItem = CAttributeTypeExtraManager::GetReference().GetStringTable(m_cstrComboType_.GetString());
		if (pComboItem != NULL)
		{
			int nDefaultIndex = 0;
			int nSize = (int)pComboItem->size();
			for (int i = 0; i < nSize; ++i)
			{
				const char* pSzData = pComboItem->at(i).c_str();
				if (m_cstrCommonValue_ == pSzData)
				{
					nDefaultIndex = i;
				}

				m_ctrlComboBox_.AddString(pSzData);
			}
			m_ctrlComboBox_.SetCurSel(nDefaultIndex);
		}
	}
	return TRUE;
}

void CDlgInputCommon::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_COMMON_VALUE, m_cstrCommonValue_);
	DDX_Control(pDX, IDC_EDIT_COMMON_COMBO, m_ctrlComboBox_);
}

const char* CDlgInputCommon::GetInputData()
{
	if (!m_cstrComboType_.IsEmpty())
		return m_cstrComBoValue_.GetString();

	return m_cstrCommonValue_.GetString();
}

BEGIN_MESSAGE_MAP(CDlgInputCommon, CDialog)
	ON_CBN_SELCHANGE(IDC_EDIT_COMMON_COMBO, &CDlgInputCommon::OnCbnSelchangeEditCommonCombo)
END_MESSAGE_MAP()


// CDlgInputCommon 메시지 처리기입니다.


void CDlgInputCommon::OnCbnSelchangeEditCommonCombo()
{
	if (!m_cstrComboType_.IsEmpty())
	{
		int nCurSel = m_ctrlComboBox_.GetCurSel();
		if (nCurSel >= 0)
			m_ctrlComboBox_.GetLBText(nCurSel, m_cstrComBoValue_);
	}
}
