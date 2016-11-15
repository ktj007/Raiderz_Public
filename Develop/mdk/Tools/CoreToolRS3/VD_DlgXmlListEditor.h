#pragma once

#include "ext_lib/ScintillaWnd.h"
//LoadLibrary를 쓰는걸로 변경
//#pragma comment(lib, "ext_lib/Scintilla/SciLexer.lib")

#include <string>

// CDlgXmlListEditor 대화 상자입니다.

class CDlgXmlListEditor : public CDialog
{
	DECLARE_DYNAMIC(CDlgXmlListEditor)

public:
	enum LIST_TYPE {
		ZONE_LIST = 0,
		FIELD_LIST,
		NPC_LIST,
		PROPERTY_LIST
	};

	CDlgXmlListEditor(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgXmlListEditor();

	virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void LoadXmlListFile(const char* _pZoneFile, const char* _pServerZoneFile, const char* _pFieldFile, LIST_TYPE _eType);
	void LoadXmlListFile(const char* _pSzXmlList, LIST_TYPE _eType);
	void SetContent(const char* _pContent);

	static bool m_isActive;

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_XML_LIST_EDITOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnTcnSelchangeTabList(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
private:
	void LoadZoneList_();
	void LoadFieldList_();

	CScintillaWnd m_ScinCtrl_;

	std::string m_strZoneContent_;
	std::string m_strFieldContent_;
	std::string m_strGeneralContent_;

	std::string m_zoneFile_;
	std::string m_serverZoneFile_;
	std::string m_fieldFile_;
	std::string m_strGeneralFile_;

	CTabCtrl m_ctlListTab_;
	LIST_TYPE m_eType_;
	bool m_bSaveGood_;
	bool m_bZoneFieldMulti_;
	afx_msg void OnBnClickedZonelistSave();
	afx_msg void OnBnClickedFieldlistSave();
};
