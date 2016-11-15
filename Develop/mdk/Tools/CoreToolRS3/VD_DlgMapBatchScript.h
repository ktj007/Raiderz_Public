#pragma once

#include <map>
#include <vector>
#include <string>
#include "ext_lib/DIB.h"

// CDlgMapBatchScript 대화 상자입니다.

class CDlgMapBatchScript : public CDialog
{
	DECLARE_DYNAMIC(CDlgMapBatchScript)

public:
	CDlgMapBatchScript(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMapBatchScript();

	std::vector<std::string> m_vecSelectedMap;
	std::string m_strSelectedScript;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MAP_BATCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	BOOL OnInitDialog();

	afx_msg void OnTcnSelchangeTabMapFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomListDraw( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnLvnItemchangedMapFileList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSelectAll();

	DECLARE_MESSAGE_MAP()

private:

	enum E_TYPE {
		ZONE_TAB = 0,
		FIELD_TAB
	};

	CTabCtrl m_ctlMapTab_;
	CListCtrl m_ctlMapFileList_;

	CStatic	m_contentLabel_;
	CFont	m_contentFont_;
	CDib	m_miniMapDib_;

	CComboBox m_ctlScriptCombo_;

	void UpdateMapFileList_(int _nIndex);
	void UpdateScriptComboBox_(int _nIndex);
	void PaintPreviewBkgAndPutRect_(CRect& _rect);

	typedef std::map<int, std::pair<std::string, std::string> > MAP_INDEX_STRING;
	std::map<int, std::pair<std::string, std::string> > m_mapFileNameIfExist_;
public:
	afx_msg void OnBnClickedOk();
};
