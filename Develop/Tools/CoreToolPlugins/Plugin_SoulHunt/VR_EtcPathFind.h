#pragma once
#include "VL_ListenableView.h"
#include "RTypes.h"
#include "C_ControlMediator.h"

#include "afxwin.h"
#include "resource.h"

// CRollupEtcPathFind 대화 상자입니다.
/** @brief Map View Rollup */
class CORETOOL_SOULHUNT_API CRollupEtcPathFind : public CDialog, public CListenableViewImpl_Rollup<CRollupEtcPathFind>
{
	DECLARE_DYNAMIC(CRollupEtcPathFind)

public:
						CRollupEtcPathFind(CWnd* pParent = NULL);	///< 표준 생성자입니다.
	virtual				~CRollupEtcPathFind();						///< 소멸자

	// Listenable View
	virtual void		Listen(CoreMessage& _message);			///< Listen
	virtual const char*	GetViewName(){ return "Path Find"; }		///< View Name

	// Dialog
	virtual BOOL		OnInitDialog();

	void				OnZoneSaved();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ETC_PATH_FIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	enum WB_TYPE
	{
		SEL_START = 0,
		SEL_END,
		SEL_NONE,
		SEL_DONE
	};
	WB_TYPE m_eWBType;

	void SetViewNavigationMesh(BOOL flag);

	afx_msg void OnBnClickedCheckActiveNavi();
	afx_msg void OnBnClickedButtonPathReset();
	afx_msg void OnBnClickedButtonPathFind();
	CButton m_btnCheckNaviActivity;

	void SetStartPosition(float _x, float _y, float _z)
	{
		m_fStartX_ = _x;
		m_fStartY_ = _y;
		m_fStartZ_ = _z;
		UpdateData(FALSE);
	}
	void GetStartPosition(rs3::RVector& _refStartPos)
	{
		_refStartPos.x = m_fStartX_;
		_refStartPos.y = m_fStartY_;
		_refStartPos.z = m_fStartZ_;
	}

	void SetEndPosition(float _x, float _y, float _z)
	{
		m_fEndX_ = _x;
		m_fEndY_ = _y;
		m_fEndZ_ = _z;
		UpdateData(FALSE);
	}
	void GetEndPosition(rs3::RVector& _refEndPos)
	{
		_refEndPos.x = m_fEndX_;
		_refEndPos.y = m_fEndY_;
		_refEndPos.z = m_fEndZ_;
	}

	enum FINDER_MODE
	{
		TEST_FIND = 0,
		SOW_SEED
	};
	FINDER_MODE GetFinderMode()
	{
		if (m_nFindMode == 0)
			return TEST_FIND;
		return SOW_SEED;
	}

	void AddSeed(rs3::RVector& _refSeedPos);
	void GetSeeds( std::vector< MVector3 >& _refVecArray );
	void RenderSeed();

private:
	MRect m_rtPickZArea;
	string m_strSeedsFilename;
	string m_strPickzFilename;

	void SaveSeeds(const char *filename);
	bool LoadSeeds(const char *filename);

	bool LoadPickz(const char *filename);
	void SavePickz(const char *filename);

	float m_fStartX_;
	float m_fStartY_;
	float m_fStartZ_;

	float m_fEndX_;
	float m_fEndY_;
	float m_fEndZ_;

	enum FINDER_UI_MODE
	{
		UI_DISABLE = 0,
		UI_TEST,
		UI_SEED
	};

	CControlMediator::MAP_INFO& GetZoneInfo();

	BOOL m_nFindMode;
	void UpdateFinderModeUI( FINDER_UI_MODE _eFinderMode );

	CListBox m_ctlSeedList;
	void ClearSeedList();
	void CreateNavigationFile(vector<MVector3> &seeds);
public:
	afx_msg void OnBnClickedButtonPathCreate();
	afx_msg void OnBnClickedRadioFindTest();
	afx_msg void OnBnClickedRadioFindSeed();
	afx_msg void OnDestroy();
	afx_msg void OnLbnDblclkListSeedList();
	afx_msg void OnBnClickedButtonDelSelSeed();
	afx_msg void OnBnClickedCheckPickingArea();
	BOOL m_bEnablePickingArea;
	afx_msg void OnBnClickedButtonPathSavePickingz();
	int m_nPickingMinX;
	int m_nPickingMaxX;
	int m_nPickingMinY;
	int m_nPickingMaxY;
};