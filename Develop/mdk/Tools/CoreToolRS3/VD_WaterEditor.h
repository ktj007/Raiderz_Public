#pragma once

#include "ext_lib/CAutoEdit/AutoEdit.h"
#include "ext_lib/CAutoEdit/AutoEditValidator.h"
#include "afxcmn.h"

// CDlgWaterEditor 대화 상자입니다.
namespace rs3 {
	class RWaterPlane;
}

class CDlgWaterEditor : public CDialog
{
	DECLARE_DYNAMIC(CDlgWaterEditor)

public:
	CDlgWaterEditor(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgWaterEditor();
	virtual BOOL OnInitDialog();
	virtual void OnCancel(); ///< 취소시에 다이어로그 감춤
	virtual void OnOK(); ///< 보여주기를 완료하고 다이어로그 감춤
	void ResetWaterPlane(rs3::RWaterPlane* _pWaterPlane);

	const bool IsActive() const
	{
		return m_bActive_;
	}

// 대화 상자 데이터입니다.
	enum { IDD = IDD_OBJECT_EDIT_WATER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheckCustomFog();
	afx_msg void OnStnClickedWaterColorPick();

	DECLARE_MESSAGE_MAP()

private:
	bool m_bActive_;

	rs3::RWaterPlane* m_pWaterPlane_;

	float m_fDepthLimit_, m_fDepthLimitOrg_;
	CSliderCtrl m_sliderDepthLimit_;
	//for slider
	float m_fDepthSlope_;
	float m_fDepthIntercept_;

	float m_fBumpUVScale1_, m_fBumpUVScale1Org_;
	float m_fBumpUVScale2_, m_fBumpUVScale2Org_;
	float m_fBumpUVScale3_, m_fBumpUVScale3Org_;
	float m_fBumpUVScale4_, m_fBumpUVScale4Org_;
	CSliderCtrl m_sliderBumpUVScale1_;
	CSliderCtrl m_sliderBumpUVScale2_;
	CSliderCtrl m_sliderBumpUVScale3_;
	CSliderCtrl m_sliderBumpUVScale4_;
	//for slider
	float m_fBumpScaleSlope_;
	float m_fBumpScaleIntercept_;

	float m_fBumpMove1U_, m_fBumpMove1UOrg_;
	float m_fBumpMove1V_, m_fBumpMove1VOrg_;
	float m_fBumpMove2U_, m_fBumpMove2UOrg_;
	float m_fBumpMove2V_, m_fBumpMove2VOrg_;
	float m_fBumpMove3U_, m_fBumpMove3UOrg_;
	float m_fBumpMove3V_, m_fBumpMove3VOrg_;
	float m_fBumpMove4U_, m_fBumpMove4UOrg_;
	float m_fBumpMove4V_, m_fBumpMove4VOrg_;
	CSliderCtrl m_sliderBumpMove1U_;
	CSliderCtrl m_sliderBumpMove1V_;
	CSliderCtrl m_sliderBumpMove2U_;
	CSliderCtrl m_sliderBumpMove2V_;
	CSliderCtrl m_sliderBumpMove3U_;
	CSliderCtrl m_sliderBumpMove3V_;
	CSliderCtrl m_sliderBumpMove4U_;
	CSliderCtrl m_sliderBumpMove4V_;
	//for slider
	float m_fBumpMoveSlope_;
	float m_fBumpMoveIntercept_;

	float m_fBumpAdjustFactor_, m_fBumpAdjustFactorOrg_;
	CSliderCtrl m_sliderAdjustFactor_;
	//for slider
	float m_fBumpAdjustSlope_;
	float m_fBumpAdjustIntercept_;

	float m_fBumpWeight1_, m_fBumpWeight1Org_;
	float m_fBumpWeight2_, m_fBumpWeight2Org_;
	float m_fBumpWeight3_, m_fBumpWeight3Org_;
	float m_fBumpWeight4_, m_fBumpWeight4Org_;
	CSliderCtrl m_sliderBumpWeight1_;
	CSliderCtrl m_sliderBumpWeight2_;
	CSliderCtrl m_sliderBumpWeight3_;
	CSliderCtrl m_sliderBumpWeight4_;
	//for slider
	float m_fWeightSlope_;
	float m_fWeightIntercept_;

	//반감거리
	float m_fRdec_, m_fRdecOrg_;
	float m_fGdec_, m_fGdecOrg_;
	float m_fBdec_, m_fBdecOrg_;
	CSliderCtrl m_sliderR_dec_;
	CSliderCtrl m_sliderG_dec_;
	CSliderCtrl m_sliderB_dec_;
	//for slider
	float m_fHalfSlope_;
	float m_fHalfIntercept_;

	float m_fFresnelConstant_, m_fFresnelConstantOrg_;
	CSliderCtrl m_sliderFresnel_;
	//for slider
	float m_fFresnelSlope_;
	float m_fFresnelIntercept_;

	float m_fReflectAdjust_, m_fReflectAdjustOrg_;
	CSliderCtrl m_sliderReflectAdjust_;
	//for slider
	float m_fReflectAdjustSlope_;
	float m_fReflectAdjustIntercept_;

	//bumpmap texture name
	CString m_cstrBumpTex1_;
	CString m_cstrBumpTex2_;

	// water fog ---
	//near
	int m_nFogNear_, m_nFogNearOrg_;
	CSliderCtrl m_sliderFogNear_;
	//far
	int m_nFogFar_, m_nFogFarOrg_;
	CSliderCtrl m_sliderFogFar_;
	
	BOOL m_bUseCustomFog_;

	// water fog color
	CAutoEdit m_ctlAutoEditFogR_;
	CAutoEdit m_ctlAutoEditFogG_;
	CAutoEdit m_ctlAutoEditFogB_;
	COLORREF m_crFogColor_;

	// diffuse color
	CAutoEdit m_ctlAutoEditDiffuseR_;
	CAutoEdit m_ctlAutoEditDiffuseG_;
	CAutoEdit m_ctlAutoEditDiffuseB_;
	COLORREF m_crDiffuseColor_;

	//---------------------------------------------------------
	//UnderWater

	//fog
	//near
	int m_nUnderWaterFogNear_, m_nUnderWaterFogNearOrg_;
	CSliderCtrl m_sliderUnderWaterFogNear_;
	//far
	int m_nUnderWaterFogFar_, m_nUnderWaterFogFarOrg_;
	CSliderCtrl m_sliderUnderWaterFogFar_;

	//distortion scale
	float m_fUnderWaterBumpScaleSlope_;
	float m_fUnderWaterBumpScaleIntercept_;
	float m_fUnderWaterBumpUVScale_, m_fUnderWaterBumpUVScaleOrg_;
	CSliderCtrl m_sliderUnderWaterBumpUVScale_;
	//---------------------------------------------------------


	CColorPickerValidate m_validatorColorPicker_;

	inline void SetLinearSliderOnHScroll_(
		float _fSlope, float _fIntercept, float& _refValue, float& _refWater, CSliderCtrl* _pSlider)
	{
		_refValue = _fSlope * _pSlider->GetPos() + _fIntercept;
		_refWater = _refValue;
	}

	inline void SetLinearSliderWithValue_(
		float _fSlope, float _fIntercept, float _fValue, CSliderCtrl* _pSlider)
	{
		_pSlider->SetPos( static_cast<int>( (_fValue - _fIntercept)/_fSlope ) );
	}

	void RollBackProp_();
	void UpdateWaterFogEditBox_();
	void DrawAndSetWaterFog_();

	void UpdateWaterDiffuseEditBox_();
	void DrawAndSetWaterDiffuse_();

public:
	afx_msg void OnBnClickedButtonWaterExport();
	afx_msg void OnBnClickedButtonWaterImport();
	afx_msg void OnBnClickedButtonBumpTexture1();
	afx_msg void OnBnClickedButtonBumpTexture2();
	afx_msg void OnStnClickedWaterDiffusePick();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
