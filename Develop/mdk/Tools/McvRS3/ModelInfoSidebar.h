#pragma once

#include "resource.h"
#include "afxwin.h"


class CModelInfoSidebar : public CDialog
{
	DECLARE_DYNAMIC(CModelInfoSidebar)

public:
	//{{AFX_DATA(CModelInfoSidebar)        
	enum { IDD = IDD_DIALOG_MODELINFO };

	BOOL		m_bNodeMesh;
	BOOL		m_bNodeBone;
	BOOL		m_bNodeDummy;
	CTreeCtrl	m_treeNodes;
	CString		m_strNodeInfo;
	//}}AFX_DATA 


public:
	CModelInfoSidebar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CModelInfoSidebar();

	void SelectNodeByID(int nID);

protected:
	void _resize( int nID, int delta, bool top_anchor);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	//{{AFX_MSG(CControlToolBar)
	afx_msg LRESULT HandleInitDialog(WPARAM, LPARAM);
	afx_msg void OnTvnSelchangedTreeNodes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckNode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_nodeTMInfo;
};
