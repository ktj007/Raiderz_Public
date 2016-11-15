#pragma once

#include "ext_lib/CChartCtrl/ChartCtrl.h"
#include "afxpropertygridctrl.h"

// CDlgCutSceneEditor 대화 상자입니다.
class CXCutSceneWrapper;
class CControlMediator;

class AFX_EXT_CLASS CDlgCutSceneEditor : public CDialog
{
	DECLARE_DYNAMIC(CDlgCutSceneEditor)

public:
	CDlgCutSceneEditor(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgCutSceneEditor();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CUTSCENE_EDITOR };

	enum E_BAR_TYPE
	{
		E_TOTAL = 0,
		E_SELECTED_OBJ,

		E_INTERVAL_KEY,
		E_WARP_KEY,
		E_SELECTED_KEY,

		E_INTERVAL,
		E_INTERVAL_END,
		E_INTERVAL_START,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

private:
	CChartCtrl		m_ChartCtrlForObject;
	CChartCtrl		m_ChartCtrlForController;
	friend class	CChartControllerAxis;
	friend class	CCustomObjectMouseListener;
	friend class	CCustomControllerMouseListener;

	CChartMouseListener* m_pListenerForObject;
	CChartMouseListener* m_pListenerForController;

	double			m_dLastObjectMinHeight_;
	double			m_dLastObjectNum_;

	double			m_dLastControllerMinHeight_;
	double			m_dLastControllerNum_;

	double			m_dScrollBarHideProtection_;

	typedef			std::map< E_BAR_TYPE, std::vector<double> >	MAP_BAR_VALUES;

	struct STrackBarInfo
	{
		STrackBarInfo(const std::string& _rStrName, double _dFirstValue)
		{
			if (_rStrName.length() > 10)
				m_strBarName = _rStrName.substr(0, 7) + "...";
			else
				m_strBarName = _rStrName;

			char buf[100] = {0, };
			sprintf_s(buf, "%10s", m_strBarName.c_str());
			m_strBarName = buf;
			m_mapBarValues.insert( MAP_BAR_VALUES::value_type(E_TOTAL, std::vector<double>(1,_dFirstValue) ) );
		}

		std::string		m_strBarName;
		MAP_BAR_VALUES	m_mapBarValues;
	};
	typedef			std::vector< STrackBarInfo > VEC_BARS;
	VEC_BARS		m_vObjectsBar_;
	VEC_BARS		m_vControllerBar_;

	typedef			std::map< E_BAR_TYPE, CChartSerie* > MAP_BAR_TYPES;
	MAP_BAR_TYPES	m_mapBarTypesForObject_;
	MAP_BAR_TYPES	m_mapBarTypesForController_;

	CControlMediator*	m_pControlMediator_;
	CXCutSceneWrapper*	m_pCutSceneWrapper;

	int					m_nCurrentSelected;
	float				m_fCurrentSelectedValue;

	int					m_nLastControllerHPoint;
	int					m_nLastControllerVPoint;

	class CCutSceneMFCPropertyGridCtrl : public CMFCPropertyGridCtrl
	{
	public:

		variant_t				m_varInfo[50];
		variant_t				m_varCmdCombo[50];

		enum LOC_IDX
		{
			POS_X = 0,
			POS_Y,
			POS_Z,
			DIR_X,
			DIR_Y,
			DIR_Z,
			UP_X,
			UP_Y,
			UP_Z,

			LOC_IDX_NUM
		};

		CMFCPropertyGridProperty* m_LocProps[LOC_IDX_NUM];


		int						m_nCurrentInfo;
		int						m_nCurrentCombo;
		CDlgCutSceneEditor*		m_pOwner;

		virtual void OnPropertyChanged( CMFCPropertyGridProperty* pProp ) const
		{
			CString strValue = pProp->GetValue();
			bool bRet = OnChangedProcWithPropValue(strValue, pProp);
			if (false == bRet)
			{
				CString strPropName = pProp->GetName();
				bRet = OnChangedProcWithPropName(strPropName, strValue, pProp);
				CMFCPropertyGridProperty* pParent = pProp->GetParent();
				if (false == bRet && pParent)
				{
					CString strParentName = pParent->GetName();
					bRet = OnChangedProcWithPropParent(strParentName, strPropName, strValue, pProp);

					CMFCPropertyGridProperty* pGrandParent = pParent->GetParent();
					if (false == bRet && pGrandParent)
					{
						CString strGrandParentName = pGrandParent->GetName();
						bRet = OnChangedProcWithPropGrandParent(strGrandParentName, strParentName, strPropName, strValue, pProp);
					}
				}
			}
		}

		bool OnChangedProcWithPropValue(const CString& _strValue, CMFCPropertyGridProperty* pProp) const;
		bool OnChangedProcWithPropName(const CString& _strPropName, const CString& _strValue, CMFCPropertyGridProperty* pProp) const;
		bool OnChangedProcWithPropParent(const CString& _strParentName, const CString& _strPropName, const CString& _strValue, CMFCPropertyGridProperty* pProp) const;
		bool OnChangedProcWithPropGrandParent(const CString& _strGrandParentName, const CString& _strParentName, const CString& _strPropName, const CString& _strValue, CMFCPropertyGridProperty* pProp) const;

		virtual void Init()
		{
			CMFCPropertyGridCtrl::Init();

			EnableHeaderCtrl(FALSE);
			MarkModifiedProperties();

			ClearTempVariable();
		}

		void ClearTempVariable()
		{
			m_nCurrentInfo = m_nCurrentCombo = 0;
			ZeroMemory(m_LocProps, sizeof(m_LocProps));
		}

		void SetToEmpty(){ RemoveAll(); RedrawWindow(); }

		// object
		void SetToCameraObject();
		void SetToActorObject();
		void SetToEventObject();

		// controller
		void SetToMoveController();
		void SetToDofController();
		void SetToCaptionController();
		void SetToLetterBoxController();
		void SetToBGMController();
		void SetToEffectController();
		void SetToEnvController();
		void SetToAniController();
		void SetToFadeController();
		void SetToTalentController();

		// internal method
		void UpdateLocation();
		void ChangeMoveLocation(LOC_IDX _idx, float fValue) const;
		void ChangeEffectLocation( LOC_IDX _idx, float fValue ) const;


	private:
		void SetToGeneralController(CMFCPropertyGridProperty* _pControllerProp, variant_t& _var);
	};

	friend class CCutSceneMFCPropertyGridCtrl;
	CCutSceneMFCPropertyGridCtrl m_ctrlObjectCmd;
	CCutSceneMFCPropertyGridCtrl m_ctrlControllerCmd;
	void OnSelectObject(int nIdx, bool bScroll);
	void OnChangeActor(int nNpcId, const char* _pSzActorName, bool bPopDialog);

	// key 정보 copy 관련
	struct SCopiedKeyInfo
	{
		SCopiedKeyInfo()
		{
			Clear();
		}

		void Clear()
		{
			m_moveControllerKey.bCopiedMoveLocKey = false;
		}

		struct MoveControllerKey
		{
			bool bCopiedMoveLocKey;
			float fTime;
			rs3::RVector3 vPos;
			rs3::RVector3 vDir;
			rs3::RVector3 vUp;
			bool bWarp;
		} m_moveControllerKey;

	} m_copiedKeyInfo;

public:
	void Init(CControlMediator* pMediator, CXCutSceneWrapper* pCutSceneWrapper);
	void ShowEditor(bool bShow);

	void ClearObject();
	void AddObject( const std::string& _rName, double _dDuration );
	void RefreshObject();

	void SelectCameraObject(int nIdx, bool bScroll){ OnSelectObject(nIdx, bScroll); m_ctrlObjectCmd.SetToCameraObject(); m_ctrlControllerCmd.SetToEmpty(); }
	void SelectActorObject(int nIdx, bool bScroll){ OnSelectObject(nIdx, bScroll); m_ctrlObjectCmd.SetToActorObject(); m_ctrlControllerCmd.SetToEmpty(); }
	void SelectEventObject(int nIdx, bool bScroll){ OnSelectObject(nIdx, bScroll); m_ctrlObjectCmd.SetToEventObject(); m_ctrlControllerCmd.SetToEmpty(); }

	void ClearController();
	int AddController(const std::string& _rName);
	void RefreshController();
	void ClearSelectedController();
	void AddBarGraphValue(int nIdx, E_BAR_TYPE _eBarType, double _dValue1, double _dValue2);

	void UpdateLocation(){ m_ctrlControllerCmd.UpdateLocation(); }

	void RememberLastControllerScrollPos(bool bClearToNegative = false)
	{
		if (bClearToNegative)
		{
			m_nLastControllerHPoint = -1;
			m_nLastControllerVPoint = -1;
		}
		else
		{
			CChartScrollBar* pScollBar = m_ChartCtrlForController.GetBottomAxis()->GetScrollBar();
			if (pScollBar)
				m_nLastControllerHPoint = pScollBar->GetScrollPos();

			pScollBar = m_ChartCtrlForController.GetLeftAxis()->GetScrollBar();
			if (pScollBar)
				m_nLastControllerVPoint = pScollBar->GetScrollPos();
		}
	}

	void SetLastControllerScrollPos()
	{
		CChartScrollBar* pScollBar = m_ChartCtrlForController.GetBottomAxis()->GetScrollBar();
		if (pScollBar)
			m_ChartCtrlForController.UpdateWithHScrollPos(SB_THUMBPOSITION, m_nLastControllerHPoint, pScollBar);

		pScollBar = m_ChartCtrlForController.GetLeftAxis()->GetScrollBar();
		if (pScollBar)
			m_ChartCtrlForController.UpdateWithVScrollPos(SB_THUMBPOSITION, m_nLastControllerVPoint, pScollBar);
	}

	afx_msg void OnCutsceneCamobjAddMoveController();
	afx_msg void OnCtrlGeneralDelete();
	afx_msg void OnCutsceneMovectrlDelkey();
	afx_msg void OnCutsceneMovectrlAddkey();
	afx_msg void OnInterpolationtypeLinear();
	afx_msg void OnInterpolationtypeCatmull();
	afx_msg void OnCutsceneMovectrlEditkey();
	afx_msg void OnCutsceneCamobjAddDOF();
	afx_msg void OnCutsceneDofctrlEditkey();
	afx_msg void OnCutsceneDofctrlDelkey();
	afx_msg void OnCutsceneDofctrlAddkey();
	afx_msg void OnCutsceneMovectrlCollision();
	afx_msg void OnCutsceneActorAddmove();
	afx_msg void OnCutsceneActorAddani();
	afx_msg void OnCutsceneActorAddtalent();
	afx_msg void OnCutsceneActorType();
	afx_msg void OnCutsceneAnictrlEditkey();
	afx_msg void OnCutsceneAnictrlDelkey();
	afx_msg void OnCutsceneAnictrlAddkey();
	afx_msg void OnCutsceneAnictrlDelete();
	afx_msg void OnCutsceneEventobjAddbgm();
	afx_msg void OnCutsceneEventobjAddcaption();
	afx_msg void OnCutsceneEventobjAddletterbox();
	afx_msg void OnCutsceneBgmctrlEditkey();
	afx_msg void OnCutsceneBgmctrlDelkey();
	afx_msg void OnCutsceneBgmctrlAddkey();
	afx_msg void OnCutsceneBgmctrlDelete();
	afx_msg void OnCutsceneCaptionctrlEditkey();
	afx_msg void OnCutsceneCaptionctrlDelkey();
	afx_msg void OnCutsceneCaptionctrlAddkey();
	afx_msg void OnCutsceneCaptionctrlDelete();
	afx_msg void OnCutsceneCaptionctrlEdit();
	afx_msg void OnCutsceneMovectrlCamHere();
	afx_msg void OnCutsceneLetterBoxctrlDelete();
	afx_msg void OnCutsceneEventobjAddenv();
	afx_msg void OnCutsceneEnvctrlEditkey();
	afx_msg void OnCutsceneEnvctrlDelkey();
	afx_msg void OnCutsceneEnvctrlAddkey();
	afx_msg void OnCutsceneEnvctrlDelete();
	afx_msg void OnCutsceneEventobjAddEffect();
	afx_msg void OnCutsceneEffectctrlEditkey();
	afx_msg void OnCutsceneEffectctrlDelkey();
	afx_msg void OnCutsceneEffectctrlAddkey();
	afx_msg void OnCutsceneEffectctrlDelete();
	afx_msg void OnCutsceneMovectrlAverageVel();
	afx_msg void OnMovecontrollerCopylocationkey();
	afx_msg void OnMovecontrollerPastelocationkey();

	afx_msg void OnCutsceneEventobjAddFade();
	afx_msg void OnCutsceneFadectrlEditkey();
	afx_msg void OnCutsceneFadectrlAddkey();
	afx_msg void OnCutsceneFadectrlDelkey();	
	afx_msg void OnCutsceneFadectrlDelete();
	afx_msg void OnCutsceneTalentctrlEditkey();
	afx_msg void OnCutsceneTalentctrlDelkey();
	afx_msg void OnCutsceneTalentctrlAddkey();
	afx_msg void OnCutsceneTalentctrlDelete();
};
