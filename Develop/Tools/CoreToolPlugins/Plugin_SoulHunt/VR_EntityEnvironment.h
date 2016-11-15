#pragma once
#include "VL_ListenableView.h"
#include "V_CCoreToolPropertyGridCtrl.h"

#include "afxwin.h"

class CDlgLogicObjectEditor;
class CLogicObject;

// CRollupEntityEnvironment 대화 상자입니다.
/** @brief Entity Environment Rollup */
class CRollupEntityEnvironment : public CDialog, public CListenableViewImpl_Rollup<CRollupEntityEnvironment>
{
	DECLARE_DYNAMIC(CRollupEntityEnvironment)

public:
						CRollupEntityEnvironment(CWnd* pParent = NULL);		///< 표준 생성자입니다.
	virtual				~CRollupEntityEnvironment();						///< 소멸자

	// Listenable View
	virtual void		Listen(CoreMessage& _message);					///< Listen
	virtual const char*	GetViewName(){ return "Entity Environment"; }		///< View Name

	// CDialog
	virtual BOOL		OnInitDialog();
	virtual void		OnCancel();
	virtual void		OnOK();

	//Envrionment Set
	void				ResetEntityEnvironment(std::vector<std::string>& _refVecString);
	void				SetEnvironmentSet(const char* _pSzEnvSetName, bool _bUpdateEnvSet);
	void				GetCurrentEnvironmentSet(std::string& _refString);
	
// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ENTITY_ENVIRONMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

private:

	int			m_nWeather_[WEATHER_NUM];
	int			m_nWeatherRadio_; //for default weather
	void		SetWeatherRadio_(int _nIndex);
	void		ValidUpdateWeather_(int _sel, CWnd* _pWnd);
	void		UpdateEnvSet_();
	CComboBox	m_ctlEnvSet_;

	class CPropertyView : public CCoreToolPropertyGridCtrl
	{
	public:
		CPropertyView();
		virtual ~CPropertyView();

		void Init(CControlMediator* m_pControlMediator_);
		void FreshProperty();

		//void SetPropertyData(rs3::REnvironment::SEnvSet *pSet);

		// event operation
		virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;

		//value table

		variant_t		m_bTown;
		variant_t		m_bReturnable;
		variant_t		m_bTutorial;

		variant_t		m_nMaxGatherMine;
		variant_t		m_nMaxGatherHerb;
		variant_t		m_nMaxGatherRelic;
		variant_t		m_nMaxGatherLumber;

		variant_t		m_nQPVPNPC1;
		variant_t		m_nQPVPNPC2;

		variant_t		m_nQPVPRebirth1;
		variant_t		m_nQPVPRebirth2;

		variant_t		m_strBGM[BGM_MAX];

		CControlMediator* m_pControlMediator_;

	} m_ctlFieldInfoProp;

protected:
	afx_msg void OnBnClickedRadioSun();
	afx_msg void OnBnClickedRadioCld();
	afx_msg void OnBnClickedRadioRny();
	afx_msg void OnBnClickedRadioHrn();
	afx_msg void OnBnClickedRadioSnw();
	afx_msg void OnBnClickedRadioHsw();
	afx_msg void OnBnClickedRadioClr();
	afx_msg void OnEnChangeEditSun();
	afx_msg void OnEnChangeEditCld();
	afx_msg void OnEnChangeEditRny();
	afx_msg void OnEnChangeEditHrn();
	afx_msg void OnEnChangeEditSnw();
	afx_msg void OnEnChangeEditHsw();
	afx_msg void OnEnChangeEditClr();
	afx_msg void OnCbnSelchangeComboEntityEnvSet();

public:
	afx_msg void OnBnClickedDefaultBgmButton();
};
