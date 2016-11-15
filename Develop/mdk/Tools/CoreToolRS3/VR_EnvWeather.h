#pragma once
#include "VL_ListenableView.h"

#include "V_PropertyGridCtrl.h"
#include "ext_lib/MultiSlider.h"

#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"

#include "RSceneSky.h"
#include "REnvironmentSet.h"

#include "ReflectiveUI.h"
#include "ReflectivePropertyGridCtrl.h"

namespace rs3 {
	class RToolSceneManager;
}

class CEnvSetRef;

// CRollupEnvWeather 대화 상자입니다.
/** @brief Environemt - Weather Rollup */
class CORETOOL_API CRollupEnvWeather : public CDialog, public CListenableViewImpl_Rollup<CRollupEnvWeather>
{
	DECLARE_DYNAMIC(CRollupEnvWeather)

public:
	// 대화 상자 데이터입니다.
	enum { IDD = IDD_VR_ENV_WEATHER };

						CRollupEnvWeather(CWnd* pParent = NULL);
	virtual				~CRollupEnvWeather();

	// Listenable View
	virtual void		Listen(CoreMessage& _message);
	virtual const char*	GetViewName(){ return "날씨"; }

	void				GetWeatherInfo(int& _viewBOOL, int& _time, int& _weather, std::string& _strEnvSet);
	void				SetWeatherInfo(int _viewBOOL, int _time, int _weather, const std::string& _strEnvSet, float fFadingTime = 1.f );
	void				GetEnvSetName(std::vector<std::string>& _refStringVec);
	void				SetEnvSetName(const std::string& _strEnvSetName);

	void				UpdateEnvironment();	///< 환경선택이 바뀌었을때

	void				ApplyDefaultEnvironment();
	void				ApplyEnvironment();

	void				ToggleFog();

	afx_msg void		OnDestroy();
	afx_msg void		OnBnClickedCheckViewFog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	virtual void OnCancel(){}
	virtual void OnOK(){}

	DECLARE_MESSAGE_MAP()

	afx_msg void OnLbnSelchangeListEnv();
	afx_msg void OnStnClickedSkyPick();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	afx_msg void OnBnClickedAddSky();
	afx_msg void OnBnClickedDelSky();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();

	afx_msg void OnBnClickedCheckViewEnv();
	afx_msg void OnCbnSelchangeComboTimes();
	afx_msg void OnCbnSelchangeComboWeathers();
	afx_msg void OnCbnSelchangeComboEnvset();

	afx_msg void OnBnClickedButtonDelenvset();
	afx_msg void OnBnClickedButtonAddedenvset();

	//config export / import
	afx_msg void OnBnClickedButtonEnvImportDefault();
	afx_msg void OnBnClickedButtonEnvImport();
	afx_msg void OnBnClickedButtonEnvExport();

	// tree property control
	afx_msg LRESULT OnPropertyViewNotify(WPARAM wParam, LPARAM lParam);

private:
	void UpdateComboEnvSet_();
	void SetUIActivity_(bool _bActive);

	void UpdateSkyLayerList();

	void UpdateCurrentEnvSet();
	rs3::REnvironment::SEnvSet* GetCurrentEnvSet();
	void ResetCurrentEnvironmetObjectPtr_(rs3::RToolSceneManager* pSceneManager, rs3::REnvironment::SEnvSet *pSet);

	void UpdateScrollBars();

	void OnPropertyChanged();

private:
	float m_fEnvUpdateFadingTime_;

	// 환경 셋
	CComboBox	m_ctlEnvSet;
	CComboBox	m_ctlTimes;
	CComboBox	m_ctlWeathers;
	CButton		m_ctlEnvViewButton;

	// 환경
	CEnvSetRef*						m_pEnvSetRef;
	CReflectiveUI					m_UI;
	CReflectivePropertyGridCtrl*	m_pPropCtrl;

	// 스카이
	rs3::RSkyManager* m_pCurrentSkyManager;
	CListBox m_ctlListBoxSkies;

	UINT m_nSkyColorR;
	UINT m_nSkyColorG;
	UINT m_nSkyColorB;

	CSliderCtrl m_ctlSlideSkyAlpha;

	CSliderCtrl m_ctlSliderSkyFog;
	int			m_nSkyFogPercent;

	CSliderCtrl m_ctlSliderSkyUVAniSpeed;

public:
	// 카메라 포그
	CButton		m_fogCheckBox;
};
