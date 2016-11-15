#pragma once

#include "d3d9.h"
#include <string>
#include <vector>

namespace rs3 {
	class RGrassSystem;
};

// CDlgGrassListOpen 대화 상자입니다.
class CDlgGrassListOpen : public CDialog
{
	DECLARE_DYNAMIC(CDlgGrassListOpen)

public:
	enum OPEN_TYPE { GRL_MAKE_OPEN = 0, GRL_SELECT_OPEN };
	//CDlgGrassListOpen(CWnd* pParent = NULL);   // 표준 생성자입니다.

	CDlgGrassListOpen(OPEN_TYPE _type, const char* _pszZonePath,
		rs3::RGrassSystem* _pGrassSystem, rs3::RDeviceD3D* _pDevice);

	///// public member
	//for multi Selection
	std::vector<std::string> m_vecGrassPieceName;
	bool m_bChanged;

	virtual ~CDlgGrassListOpen();
	virtual BOOL OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TERRAIN_GRASS_OPEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg void OnLbnSelchangeListGrlFile();
	afx_msg void OnBnClickedGrlAdd();
	afx_msg void OnBnClickedGrlDelete();
	afx_msg void OnBnClickedGrlChange();

	DECLARE_MESSAGE_MAP()
private:
	CListBox m_ctlGrassList_;
	//LPDIRECT3DDEVICE9 m_pDevice_;
	rs3::RDeviceD3D* m_pDevice_;
	OPEN_TYPE m_eOpenType_;
	typedef std::vector<std::string> GRASS_LIST;
	std::vector<std::string> m_vecGrassList_;
	rs3::RGrassSystem* m_pGrassSystem_;
	CString m_cstrGrlPath_;
	std::string m_strGrassResPath_;
	void UpdateListBox_();
	void EditAndMakeGrl_(const char* _szFrom = NULL, const char* _szTo = NULL);
public:
	afx_msg void OnLbnDblclkListGrlFile();
};
