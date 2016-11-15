// VD_WaterEditor.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VD_WaterEditor.h"
#include "V_MFCHelper.h"

#include "MainFrm.h"
#include "C_ControlMediator.h"
#include "M_ToolCmdMgr.h"
#include "C_XMLConfig.h"
#include "C_XMLResourceList.h"
#include "VD_DlgCommonFileOpen.h"

#include "RWaterSceneNode.h"


// CDlgWaterEditor 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgWaterEditor, CDialog)

CDlgWaterEditor::CDlgWaterEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWaterEditor::IDD, pParent)
{
	m_pWaterPlane_ = NULL;
	m_bUseCustomFog_ = FALSE;

	m_bActive_ = false;
}

CDlgWaterEditor::~CDlgWaterEditor()
{
}

BOOL CDlgWaterEditor::OnInitDialog()
{
	CDialog::OnInitDialog();

	CMFCHelper::SetMFCSlider(&m_sliderDepthLimit_, 0, 300, 0); //총 간격갯수 300개
	m_fDepthSlope_ = 10; //기울기 10
	m_fDepthIntercept_ = 0; //초기 y 절편 0

	CMFCHelper::SetMFCSlider(&m_sliderBumpUVScale1_, 0, 400, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpUVScale2_, 0, 400, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpUVScale3_, 0, 400, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpUVScale4_, 0, 400, 0);
	m_fBumpScaleSlope_ = 50;
	m_fBumpScaleIntercept_ = 100;

	CMFCHelper::SetMFCSlider(&m_sliderBumpMove1U_, 0, 100, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpMove1V_, 0, 100, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpMove2U_, 0, 100, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpMove2V_, 0, 100, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpMove3U_, 0, 100, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpMove3V_, 0, 100, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpMove4U_, 0, 100, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpMove4V_, 0, 100, 0);
	m_fBumpMoveIntercept_ = -0.05f;
	m_fBumpMoveSlope_ = 0.001f;

	CMFCHelper::SetMFCSlider(&m_sliderAdjustFactor_, 0, 100, 0);
	m_fBumpAdjustSlope_ = 0.01f;
	m_fBumpAdjustIntercept_ = 0;

	CMFCHelper::SetMFCSlider(&m_sliderBumpWeight1_, 0, 500, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpWeight2_, 0, 500, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpWeight3_, 0, 500, 0);
	CMFCHelper::SetMFCSlider(&m_sliderBumpWeight4_, 0, 500, 0);
	m_fWeightSlope_ = 0.01f;
	m_fWeightIntercept_ = 0.0;

	CMFCHelper::SetMFCSlider(&m_sliderR_dec_, 0, 500, 0);
	CMFCHelper::SetMFCSlider(&m_sliderG_dec_, 0, 500, 0);
	CMFCHelper::SetMFCSlider(&m_sliderB_dec_, 0, 500, 0);
	m_fHalfSlope_ = 20;
	m_fHalfIntercept_ = 20;

	CMFCHelper::SetMFCSlider(&m_sliderFresnel_, 0, 100, 0);
	m_fFresnelSlope_ = 0.01;
	m_fFresnelIntercept_ = 0;

	CMFCHelper::SetMFCSlider(&m_sliderReflectAdjust_, 0, 100, 0);
	m_fReflectAdjustSlope_ = 0.01;
	m_fReflectAdjustIntercept_ = 0;

	//Water Fog - near : far 때문에 보통과 조금 다름
	CMFCHelper::SetMFCSlider(&m_sliderFogNear_, 0, 800, 0);
	CMFCHelper::SetMFCSlider(&m_sliderFogFar_, 0, 800, 0);
	
	//water color
	m_ctlAutoEditFogR_.SetMaxChars(3);
	m_ctlAutoEditFogR_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditFogR_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditFogR_.SetDefaultValue(_T("0"));

	m_ctlAutoEditFogG_.SetMaxChars(3);
	m_ctlAutoEditFogG_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditFogG_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditFogG_.SetDefaultValue(_T("0"));

	m_ctlAutoEditFogB_.SetMaxChars(3);
	m_ctlAutoEditFogB_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditFogB_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditFogB_.SetDefaultValue(_T("0"));

	//diffuse color
	m_ctlAutoEditDiffuseR_.SetMaxChars(3);
	m_ctlAutoEditDiffuseR_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditDiffuseR_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditDiffuseR_.SetDefaultValue(_T("0"));

	m_ctlAutoEditDiffuseG_.SetMaxChars(3);
	m_ctlAutoEditDiffuseG_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditDiffuseG_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditDiffuseG_.SetDefaultValue(_T("0"));

	m_ctlAutoEditDiffuseB_.SetMaxChars(3);
	m_ctlAutoEditDiffuseB_.SetValidChar(_T("0123456789"));
	m_ctlAutoEditDiffuseB_.SetValidationHandler(&m_validatorColorPicker_);
    m_ctlAutoEditDiffuseB_.SetDefaultValue(_T("0"));

	//-------------------------------------------------------------------
	//Underwater

	//fog
	CMFCHelper::SetMFCSlider(&m_sliderUnderWaterFogNear_, -400, 400, 0);
	CMFCHelper::SetMFCSlider(&m_sliderUnderWaterFogFar_, -400, 400, 0);

	//Distortion scale
	m_fUnderWaterBumpScaleSlope_ =1;
	m_fUnderWaterBumpScaleIntercept_ =0;
	CMFCHelper::SetMFCSlider(&m_sliderUnderWaterBumpUVScale_, 0, 500, 0);
	//-------------------------------------------------------------------

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgWaterEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//depth limit
	DDX_Control(pDX, IDC_SLIDER_DEPTH, m_sliderDepthLimit_);
	DDX_Text(pDX, IDC_EDIT_DEPTH, m_fDepthLimit_);

	//bump scale
	DDX_Control(pDX, IDC_SLIDER_UVSCALE_1, m_sliderBumpUVScale1_);
	DDX_Text(pDX, IDC_EDIT_UVSCALE_1, m_fBumpUVScale1_);
	//
	DDX_Control(pDX, IDC_SLIDER_UVSCALE_2, m_sliderBumpUVScale2_);
	DDX_Text(pDX, IDC_EDIT_UVSCALE_2, m_fBumpUVScale2_);
	//
	DDX_Control(pDX, IDC_SLIDER_UVSCALE_3, m_sliderBumpUVScale3_);
	DDX_Text(pDX, IDC_EDIT_UVSCALE_3, m_fBumpUVScale3_);
	//
	DDX_Control(pDX, IDC_SLIDER_UVSCALE_4, m_sliderBumpUVScale4_);
	DDX_Text(pDX, IDC_EDIT_UVSCALE_4, m_fBumpUVScale4_);

	//move
	DDX_Control(pDX, IDC_SLIDER_X_MOVE_1, m_sliderBumpMove1U_);
	DDX_Text(pDX, IDC_EDIT_X_MOVE_1, m_fBumpMove1U_);
	DDX_Control(pDX, IDC_SLIDER_Y_MOVE_1, m_sliderBumpMove1V_);
	DDX_Text(pDX, IDC_EDIT_Y_MOVE_1, m_fBumpMove1V_);

	DDX_Control(pDX, IDC_SLIDER_X_MOVE_2, m_sliderBumpMove2U_);
	DDX_Text(pDX, IDC_EDIT_X_MOVE_2, m_fBumpMove2U_);
	DDX_Control(pDX, IDC_SLIDER_Y_MOVE_2, m_sliderBumpMove2V_);
	DDX_Text(pDX, IDC_EDIT_Y_MOVE_2, m_fBumpMove2V_);

	DDX_Control(pDX, IDC_SLIDER_X_MOVE_3, m_sliderBumpMove3U_);
	DDX_Text(pDX, IDC_EDIT_X_MOVE_3, m_fBumpMove3U_);
	DDX_Control(pDX, IDC_SLIDER_Y_MOVE_3, m_sliderBumpMove3V_);
	DDX_Text(pDX, IDC_EDIT_Y_MOVE_3, m_fBumpMove3V_);

	DDX_Control(pDX, IDC_SLIDER_X_MOVE_4, m_sliderBumpMove4U_);
	DDX_Text(pDX, IDC_EDIT_X_MOVE_4, m_fBumpMove4U_);
	DDX_Control(pDX, IDC_SLIDER_Y_MOVE_4, m_sliderBumpMove4V_);
	DDX_Text(pDX, IDC_EDIT_Y_MOVE_4, m_fBumpMove4V_);

	//adjust factor
	DDX_Control(pDX, IDC_SLIDER_ADJUST, m_sliderAdjustFactor_);
	DDX_Text(pDX, IDC_EDIT_ADJUST, m_fBumpAdjustFactor_);

	//bump weight
	DDX_Control(pDX, IDC_SLIDER_WEIGHT_1, m_sliderBumpWeight1_);
	DDX_Text(pDX, IDC_EDIT_WEIGHT_1, m_fBumpWeight1_);
	//
	DDX_Control(pDX, IDC_SLIDER_WEIGHT_2, m_sliderBumpWeight2_);
	DDX_Text(pDX, IDC_EDIT_WEIGHT_2, m_fBumpWeight2_);
	//
	DDX_Control(pDX, IDC_SLIDER_WEIGHT_3, m_sliderBumpWeight3_);
	DDX_Text(pDX, IDC_EDIT_WEIGHT_3, m_fBumpWeight3_);
	//
	DDX_Control(pDX, IDC_SLIDER_WEIGHT_4, m_sliderBumpWeight4_);
	DDX_Text(pDX, IDC_EDIT_WEIGHT_4, m_fBumpWeight4_);
	
	//반감거리
	DDX_Control(pDX, IDC_SLIDER_R_HALF, m_sliderR_dec_);
	DDX_Text(pDX, IDC_EDIT_R_HALF, m_fRdec_);
	//
	DDX_Control(pDX, IDC_SLIDER_G_HALF, m_sliderG_dec_);
	DDX_Text(pDX, IDC_EDIT_G_HALF, m_fGdec_);
	//
	DDX_Control(pDX, IDC_SLIDER_B_HALF, m_sliderB_dec_);
	DDX_Text(pDX, IDC_EDIT_B_HALF, m_fBdec_);

	//프레넬 상수
	DDX_Control(pDX, IDC_SLIDER_FRESNEL, m_sliderFresnel_);
	DDX_Text(pDX, IDC_EDIT_FRESNEL, m_fFresnelConstant_);

	//Reflect Adj
	DDX_Control(pDX, IDC_SLIDER_REFL_ADJ, m_sliderReflectAdjust_);
	DDX_Text(pDX, IDC_EDIT_REFL_ADJ, m_fReflectAdjust_);

	//워터포그 near - far
	DDX_Control(pDX, IDC_SLIDER_WATER_FOG_NEAR, m_sliderFogNear_);
	DDX_Text(pDX, IDC_EDIT_WATER_FOG_NEAR, m_nFogNear_);
	DDX_Control(pDX, IDC_SLIDER_WATER_FOG_FAR , m_sliderFogFar_);
	DDX_Text(pDX, IDC_EDIT_WATER_FOG_FAR, m_nFogFar_);

	//포그 사용 여부
	DDX_Check(pDX, IDC_CHECK_CUSTOM_FOG, m_bUseCustomFog_);
	//포그 칼라
	DDX_Control(pDX, IDC_EDIT_WATER_COLOR_R, m_ctlAutoEditFogR_);
	DDX_Control(pDX, IDC_EDIT_WATER_COLOR_G, m_ctlAutoEditFogG_);
	DDX_Control(pDX, IDC_EDIT_WATER_COLOR_B, m_ctlAutoEditFogB_);

	//Diffuse color
	DDX_Control(pDX, IDC_EDIT_WATER_DIFFUSE_R, m_ctlAutoEditDiffuseR_);
	DDX_Control(pDX, IDC_EDIT_WATER_DIFFUSE_G, m_ctlAutoEditDiffuseG_);
	DDX_Control(pDX, IDC_EDIT_WATER_DIFFUSE_B, m_ctlAutoEditDiffuseB_);

	//범프맵 텍스쳐
	DDX_Text(pDX, IDC_EDIT_BUMP_TEXTURE1, m_cstrBumpTex1_);
	DDX_Text(pDX, IDC_EDIT_BUMP_TEXTURE2, m_cstrBumpTex2_);

	//-----------------------------------------------------------------------
	//UnderWater
	
	//fog near - far
	DDX_Control(pDX, IDC_SLIDER_WATER_FOG_NEAR2, m_sliderUnderWaterFogNear_);
	DDX_Text(pDX, IDC_EDIT_WATER_FOG_NEAR2, m_nUnderWaterFogNear_);
	DDX_Control(pDX, IDC_SLIDER_WATER_FOG_FAR2 , m_sliderUnderWaterFogFar_);
	DDX_Text(pDX, IDC_EDIT_WATER_FOG_FAR2, m_nUnderWaterFogFar_);

	//distortion scale
	DDX_Control(pDX, IDC_SLIDER_UNDERWATERUVSCALE, m_sliderUnderWaterBumpUVScale_);
	DDX_Text(pDX, IDC_EDIT_UNDERWATERUVSCALE, m_fUnderWaterBumpUVScale_);
	//-----------------------------------------------------------------------
}


BEGIN_MESSAGE_MAP(CDlgWaterEditor, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_WATER_EXPORT, &CDlgWaterEditor::OnBnClickedButtonWaterExport)
	ON_BN_CLICKED(IDC_BUTTON_WATER_IMPORT, &CDlgWaterEditor::OnBnClickedButtonWaterImport)
	ON_BN_CLICKED(IDC_CHECK_CUSTOM_FOG, &CDlgWaterEditor::OnBnClickedCheckCustomFog)
	ON_STN_CLICKED(IDC_WATER_COLOR_PICK, &CDlgWaterEditor::OnStnClickedWaterColorPick)
	ON_BN_CLICKED(IDC_BUTTON_BUMP_TEXTURE1, &CDlgWaterEditor::OnBnClickedButtonBumpTexture1)
	ON_BN_CLICKED(IDC_BUTTON_BUMP_TEXTURE2, &CDlgWaterEditor::OnBnClickedButtonBumpTexture2)
	ON_STN_CLICKED(IDC_WATER_DIFFUSE_PICK, &CDlgWaterEditor::OnStnClickedWaterDiffusePick)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgWaterEditor 메시지 처리기입니다.
void CDlgWaterEditor::OnCancel()
{
	RollBackProp_();
	ShowWindow(SW_HIDE);
}
void CDlgWaterEditor::OnOK()
{
	CControlMediator* pMediator = ((CMainFrame*)AfxGetMainWnd())->GetControlMediator();
	pMediator->GetToolCmdMgr()->RecordCommand(NULL);
	ShowWindow(SW_HIDE);
}

void CDlgWaterEditor::ResetWaterPlane(rs3::RWaterPlane* _pWaterPlane)
{
	_ASSERT( _pWaterPlane );
	rs3::RWaterPlane::SWaterPlaneProperty* pWaterProperty = _pWaterPlane->GetWaterPlaneProperty();

	m_fDepthLimitOrg_ = m_fDepthLimit_ = pWaterProperty->m_fDepthLimit;
	SetLinearSliderWithValue_(m_fDepthSlope_, m_fDepthIntercept_, m_fDepthLimit_, &m_sliderDepthLimit_);

	m_fBumpUVScale1Org_ = m_fBumpUVScale1_ = pWaterProperty->m_fBumpUVScale1;
	m_fBumpUVScale2Org_ = m_fBumpUVScale2_ = pWaterProperty->m_fBumpUVScale2;
	m_fBumpUVScale3Org_ = m_fBumpUVScale3_ = pWaterProperty->m_fBumpUVScale3;
	m_fBumpUVScale4Org_ = m_fBumpUVScale4_ = pWaterProperty->m_fBumpUVScale4;

	SetLinearSliderWithValue_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_, m_fBumpUVScale1_, &m_sliderBumpUVScale1_);
	SetLinearSliderWithValue_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_, m_fBumpUVScale2_, &m_sliderBumpUVScale2_);
	SetLinearSliderWithValue_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_, m_fBumpUVScale3_, &m_sliderBumpUVScale3_);
	SetLinearSliderWithValue_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_, m_fBumpUVScale4_, &m_sliderBumpUVScale4_);

	m_fBumpMove1UOrg_ = m_fBumpMove1U_ = pWaterProperty->m_vBumpMove1.x;
	m_fBumpMove1VOrg_ = m_fBumpMove1V_ = pWaterProperty->m_vBumpMove1.y;

	m_fBumpMove2UOrg_ = m_fBumpMove2U_ = pWaterProperty->m_vBumpMove2.x;
	m_fBumpMove2VOrg_ = m_fBumpMove2V_ = pWaterProperty->m_vBumpMove2.y;

	m_fBumpMove3UOrg_ = m_fBumpMove3U_ = pWaterProperty->m_vBumpMove3.x;
	m_fBumpMove3VOrg_ = m_fBumpMove3V_ = pWaterProperty->m_vBumpMove3.y;

	m_fBumpMove4UOrg_ = m_fBumpMove4U_ = pWaterProperty->m_vBumpMove4.x;
	m_fBumpMove4VOrg_ = m_fBumpMove4V_ = pWaterProperty->m_vBumpMove4.y;

	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove1U_, &m_sliderBumpMove1U_);
	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove1V_, &m_sliderBumpMove1V_);

	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove2U_, &m_sliderBumpMove2U_);
	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove2V_, &m_sliderBumpMove2V_);

	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove3U_, &m_sliderBumpMove3U_);
	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove3V_, &m_sliderBumpMove3V_);

	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove4U_, &m_sliderBumpMove4U_);
	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove4V_, &m_sliderBumpMove4V_);

	m_fBumpAdjustFactorOrg_ = m_fBumpAdjustFactor_ = pWaterProperty->m_fBumpAdjustFactor;
	SetLinearSliderWithValue_(m_fBumpAdjustSlope_, m_fBumpAdjustIntercept_, m_fBumpAdjustFactor_, &m_sliderAdjustFactor_);

	m_fBumpWeight1Org_ = m_fBumpWeight1_ = pWaterProperty->m_fBumpWeight1;
	m_fBumpWeight2Org_ = m_fBumpWeight2_ = pWaterProperty->m_fBumpWeight2;
	m_fBumpWeight3Org_ = m_fBumpWeight3_ = pWaterProperty->m_fBumpWeight3;
	m_fBumpWeight4Org_ = m_fBumpWeight4_ = pWaterProperty->m_fBumpWeight4;

	SetLinearSliderWithValue_(m_fWeightSlope_, m_fWeightIntercept_, m_fBumpWeight1_, &m_sliderBumpWeight1_);
	SetLinearSliderWithValue_(m_fWeightSlope_, m_fWeightIntercept_, m_fBumpWeight2_, &m_sliderBumpWeight2_);
	SetLinearSliderWithValue_(m_fWeightSlope_, m_fWeightIntercept_, m_fBumpWeight3_, &m_sliderBumpWeight3_);
	SetLinearSliderWithValue_(m_fWeightSlope_, m_fWeightIntercept_, m_fBumpWeight4_, &m_sliderBumpWeight4_);

	m_fRdecOrg_ = m_fRdec_ = pWaterProperty->m_vWaterRGBHalflife.x;
	m_fGdecOrg_ = m_fGdec_ = pWaterProperty->m_vWaterRGBHalflife.y;
	m_fBdecOrg_ = m_fBdec_ = pWaterProperty->m_vWaterRGBHalflife.z;

	SetLinearSliderWithValue_(m_fHalfSlope_, m_fHalfIntercept_, m_fRdec_, &m_sliderR_dec_);
	SetLinearSliderWithValue_(m_fHalfSlope_, m_fHalfIntercept_, m_fGdec_, &m_sliderG_dec_);
	SetLinearSliderWithValue_(m_fHalfSlope_, m_fHalfIntercept_, m_fBdec_, &m_sliderB_dec_);

	m_fFresnelConstantOrg_ = m_fFresnelConstant_ = pWaterProperty->m_fFresnelConstant;
	SetLinearSliderWithValue_(m_fFresnelSlope_, m_fFresnelIntercept_, m_fFresnelConstant_, &m_sliderFresnel_);

	m_fReflectAdjustOrg_ = m_fReflectAdjust_ = pWaterProperty->m_fReflectAdjust;
	SetLinearSliderWithValue_(m_fReflectAdjustSlope_, m_fReflectAdjustIntercept_, m_fReflectAdjust_, &m_sliderReflectAdjust_);

	//water fog
	m_nFogNearOrg_ = m_nFogNear_ = pWaterProperty->m_fFogNear * 0.01;
	SetLinearSliderWithValue_( 1, 0, m_nFogNear_, &m_sliderFogNear_);
	m_nFogFarOrg_ = m_nFogFar_ = pWaterProperty->m_fFogFar * 0.01;
	SetLinearSliderWithValue_( 1, 0, m_nFogFar_, &m_sliderFogFar_);
	m_bUseCustomFog_ = pWaterProperty->m_bUseCustomFog? TRUE : FALSE;

	//water color
	m_crFogColor_ = RGB( (int)(pWaterProperty->m_vFogColor.x * 255), (int)(pWaterProperty->m_vFogColor.y * 255), (int)(pWaterProperty->m_vFogColor.z * 255) );
	UpdateWaterFogEditBox_();

	//bumpmap texture name
	m_cstrBumpTex1_ = pWaterProperty->m_strBumpTex1.c_str();
	m_cstrBumpTex2_ = pWaterProperty->m_strBumpTex2.c_str();

	m_crDiffuseColor_ =
		RGB( (int)(pWaterProperty->m_vDiffuseColor.x * 255), (int)(pWaterProperty->m_vDiffuseColor.y * 255), (int)(pWaterProperty->m_vDiffuseColor.z * 255) );
	UpdateWaterDiffuseEditBox_();

	//--------------------------------------------------------
	//Undertwarter

	//fog
	m_nUnderWaterFogNearOrg_ = m_nUnderWaterFogNear_ = pWaterProperty->m_fUnderWaterFogNear * 0.01;
	SetLinearSliderWithValue_( 1, 0, m_nUnderWaterFogNear_, &m_sliderUnderWaterFogNear_);
	m_nUnderWaterFogFarOrg_ = m_nUnderWaterFogFar_ = pWaterProperty->m_fUnderWaterFogFar * 0.01;
	SetLinearSliderWithValue_( 1, 0, m_nUnderWaterFogFar_, &m_sliderUnderWaterFogFar_);

	//distortion scale
	m_fUnderWaterBumpUVScaleOrg_ = m_fUnderWaterBumpUVScale_ = pWaterProperty->m_fUnderWaterBumpUVScale;
	SetLinearSliderWithValue_(m_fUnderWaterBumpScaleSlope_, m_fUnderWaterBumpScaleIntercept_, m_fUnderWaterBumpUVScale_, &m_sliderUnderWaterBumpUVScale_);
	//--------------------------------------------------------

	m_pWaterPlane_ = _pWaterPlane;
	UpdateData(FALSE);
}

void CDlgWaterEditor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	rs3::RWaterPlane::SWaterPlaneProperty* pWaterPlaneProperty = m_pWaterPlane_->GetWaterPlaneProperty();

	SetLinearSliderOnHScroll_(m_fDepthSlope_, m_fDepthIntercept_,
		m_fDepthLimit_, pWaterPlaneProperty->m_fDepthLimit, &m_sliderDepthLimit_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_,
		m_fBumpUVScale1_, pWaterPlaneProperty->m_fBumpUVScale1, &m_sliderBumpUVScale1_);

	SetLinearSliderOnHScroll_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_,
		m_fBumpUVScale2_, pWaterPlaneProperty->m_fBumpUVScale2, &m_sliderBumpUVScale2_);

	SetLinearSliderOnHScroll_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_,
		m_fBumpUVScale3_, pWaterPlaneProperty->m_fBumpUVScale3, &m_sliderBumpUVScale3_);

	SetLinearSliderOnHScroll_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_,
		m_fBumpUVScale4_, pWaterPlaneProperty->m_fBumpUVScale4, &m_sliderBumpUVScale4_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_,
		m_fBumpMove1U_, pWaterPlaneProperty->m_vBumpMove1.x, &m_sliderBumpMove1U_);

	SetLinearSliderOnHScroll_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_,
		m_fBumpMove1V_, pWaterPlaneProperty->m_vBumpMove1.y, &m_sliderBumpMove1V_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_,
		m_fBumpMove2U_, pWaterPlaneProperty->m_vBumpMove2.x, &m_sliderBumpMove2U_);

	SetLinearSliderOnHScroll_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_,
		m_fBumpMove2V_, pWaterPlaneProperty->m_vBumpMove2.y, &m_sliderBumpMove2V_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_,
		m_fBumpMove3U_, pWaterPlaneProperty->m_vBumpMove3.x, &m_sliderBumpMove3U_);

	SetLinearSliderOnHScroll_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_,
		m_fBumpMove3V_, pWaterPlaneProperty->m_vBumpMove3.y, &m_sliderBumpMove3V_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_,
		m_fBumpMove4U_, pWaterPlaneProperty->m_vBumpMove4.x, &m_sliderBumpMove4U_);

	SetLinearSliderOnHScroll_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_,
		m_fBumpMove4V_, pWaterPlaneProperty->m_vBumpMove4.y, &m_sliderBumpMove4V_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fBumpAdjustSlope_, m_fBumpAdjustIntercept_,
		m_fBumpAdjustFactor_, pWaterPlaneProperty->m_fBumpAdjustFactor, &m_sliderAdjustFactor_);


	SetLinearSliderOnHScroll_(m_fWeightSlope_, m_fWeightIntercept_,
		m_fBumpWeight1_, pWaterPlaneProperty->m_fBumpWeight1, &m_sliderBumpWeight1_);

	SetLinearSliderOnHScroll_(m_fWeightSlope_, m_fWeightIntercept_,
		m_fBumpWeight2_, pWaterPlaneProperty->m_fBumpWeight2, &m_sliderBumpWeight2_);

	SetLinearSliderOnHScroll_(m_fWeightSlope_, m_fWeightIntercept_,
		m_fBumpWeight3_, pWaterPlaneProperty->m_fBumpWeight3, &m_sliderBumpWeight3_);

	SetLinearSliderOnHScroll_(m_fWeightSlope_, m_fWeightIntercept_,
		m_fBumpWeight4_, pWaterPlaneProperty->m_fBumpWeight4, &m_sliderBumpWeight4_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fHalfSlope_, m_fHalfIntercept_,
		m_fRdec_, pWaterPlaneProperty->m_vWaterRGBHalflife.x, &m_sliderR_dec_);

	SetLinearSliderOnHScroll_(m_fHalfSlope_, m_fHalfIntercept_,
		m_fGdec_, pWaterPlaneProperty->m_vWaterRGBHalflife.y, &m_sliderG_dec_);

	SetLinearSliderOnHScroll_(m_fHalfSlope_, m_fHalfIntercept_,
		m_fBdec_, pWaterPlaneProperty->m_vWaterRGBHalflife.z, &m_sliderB_dec_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fFresnelSlope_, m_fFresnelIntercept_,
		m_fFresnelConstant_, pWaterPlaneProperty->m_fFresnelConstant, &m_sliderFresnel_);

	/////////////////////////////////////////////////////////////////////////////////////

	SetLinearSliderOnHScroll_(m_fReflectAdjustSlope_, m_fReflectAdjustIntercept_,
		m_fReflectAdjust_, pWaterPlaneProperty->m_fReflectAdjust, &m_sliderReflectAdjust_);

	/////////////////////////////////////////////////////////////////////////////////////

	//water fog
	m_nFogNear_ = m_sliderFogNear_.GetPos();
	m_nFogFar_ = m_sliderFogFar_.GetPos();
	if( m_nFogFar_ < m_nFogNear_ )
	{
		m_nFogNear_ = m_nFogFar_;
		m_sliderFogNear_.SetPos( m_nFogNear_ );
	}
	pWaterPlaneProperty->m_fFogNear = m_nFogNear_ * 100;
	pWaterPlaneProperty->m_fFogFar = m_nFogFar_ * 100;

	//-------------------------------------------------------------------
	//UnderWater
	
	//fog
	m_nUnderWaterFogNear_ = m_sliderUnderWaterFogNear_.GetPos();
	m_nUnderWaterFogFar_ = m_sliderUnderWaterFogFar_.GetPos();
	if( m_nUnderWaterFogFar_ < m_nUnderWaterFogNear_ )
	{
		m_nUnderWaterFogNear_ = m_nUnderWaterFogFar_;
		m_sliderUnderWaterFogNear_.SetPos( m_nUnderWaterFogNear_ );
	}
	pWaterPlaneProperty->m_fUnderWaterFogNear = m_nUnderWaterFogNear_ * 100;
	pWaterPlaneProperty->m_fUnderWaterFogFar = m_nUnderWaterFogFar_ * 100;

	//Distortion scale
	SetLinearSliderOnHScroll_(m_fUnderWaterBumpScaleSlope_, m_fUnderWaterBumpScaleIntercept_,
		m_fUnderWaterBumpUVScale_, pWaterPlaneProperty->m_fUnderWaterBumpUVScale, &m_sliderUnderWaterBumpUVScale_);
	//-------------------------------------------------------------------


	UpdateData(FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgWaterEditor::RollBackProp_()
{
	if(m_pWaterPlane_ == NULL)
		return;

	rs3::RWaterPlane::SWaterPlaneProperty* pWaterPlaneProperty = m_pWaterPlane_->GetWaterPlaneProperty();

	m_fDepthLimit_ = pWaterPlaneProperty->m_fDepthLimit = m_fDepthLimitOrg_;
	SetLinearSliderWithValue_(m_fDepthSlope_, m_fDepthIntercept_, m_fDepthLimit_, &m_sliderDepthLimit_);

	m_fBumpUVScale1_ = pWaterPlaneProperty->m_fBumpUVScale1 = m_fBumpUVScale1Org_;
	m_fBumpUVScale2_ = pWaterPlaneProperty->m_fBumpUVScale2 = m_fBumpUVScale2Org_;
	m_fBumpUVScale3_ = pWaterPlaneProperty->m_fBumpUVScale3 = m_fBumpUVScale3Org_;
	m_fBumpUVScale4_ = pWaterPlaneProperty->m_fBumpUVScale4 = m_fBumpUVScale4Org_;

	SetLinearSliderWithValue_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_, m_fBumpUVScale1_, &m_sliderBumpUVScale1_);
	SetLinearSliderWithValue_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_, m_fBumpUVScale2_, &m_sliderBumpUVScale2_);
	SetLinearSliderWithValue_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_, m_fBumpUVScale3_, &m_sliderBumpUVScale3_);
	SetLinearSliderWithValue_(m_fBumpScaleSlope_, m_fBumpScaleIntercept_, m_fBumpUVScale4_, &m_sliderBumpUVScale4_);

	m_fBumpMove1U_ = pWaterPlaneProperty->m_vBumpMove1.x = m_fBumpMove1UOrg_;
	m_fBumpMove1V_ = pWaterPlaneProperty->m_vBumpMove1.y = m_fBumpMove1VOrg_;

	m_fBumpMove2U_ = pWaterPlaneProperty->m_vBumpMove2.x = m_fBumpMove2UOrg_;
	m_fBumpMove2V_ = pWaterPlaneProperty->m_vBumpMove2.y = m_fBumpMove2VOrg_;

	m_fBumpMove3U_ = pWaterPlaneProperty->m_vBumpMove3.x = m_fBumpMove3UOrg_;
	m_fBumpMove3V_ = pWaterPlaneProperty->m_vBumpMove3.y = m_fBumpMove3VOrg_;

	m_fBumpMove4U_ = pWaterPlaneProperty->m_vBumpMove4.x = m_fBumpMove4UOrg_;
	m_fBumpMove4V_ = pWaterPlaneProperty->m_vBumpMove4.y = m_fBumpMove4VOrg_;

	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove1U_, &m_sliderBumpMove1U_);
	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove1V_, &m_sliderBumpMove1V_);

	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove2U_, &m_sliderBumpMove2U_);
	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove2V_, &m_sliderBumpMove2V_);

	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove3U_, &m_sliderBumpMove3U_);
	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove3V_, &m_sliderBumpMove3V_);

	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove4U_, &m_sliderBumpMove4U_);
	SetLinearSliderWithValue_(m_fBumpMoveSlope_, m_fBumpMoveIntercept_, m_fBumpMove4V_, &m_sliderBumpMove4V_);

	m_fBumpAdjustFactor_ = pWaterPlaneProperty->m_fBumpAdjustFactor = m_fBumpAdjustFactorOrg_;
	SetLinearSliderWithValue_(m_fBumpAdjustSlope_, m_fBumpAdjustIntercept_, m_fBumpAdjustFactor_, &m_sliderAdjustFactor_);

	m_fBumpWeight1_ = pWaterPlaneProperty->m_fBumpWeight1 = m_fBumpWeight1Org_;
	m_fBumpWeight2_ = pWaterPlaneProperty->m_fBumpWeight2 = m_fBumpWeight2Org_;
	m_fBumpWeight3_ = pWaterPlaneProperty->m_fBumpWeight3 = m_fBumpWeight3Org_;
	m_fBumpWeight4_ = pWaterPlaneProperty->m_fBumpWeight4 = m_fBumpWeight4Org_;

	SetLinearSliderWithValue_(m_fWeightSlope_, m_fWeightIntercept_, m_fBumpWeight1_, &m_sliderBumpWeight1_);
	SetLinearSliderWithValue_(m_fWeightSlope_, m_fWeightIntercept_, m_fBumpWeight2_, &m_sliderBumpWeight2_);
	SetLinearSliderWithValue_(m_fWeightSlope_, m_fWeightIntercept_, m_fBumpWeight3_, &m_sliderBumpWeight3_);
	SetLinearSliderWithValue_(m_fWeightSlope_, m_fWeightIntercept_, m_fBumpWeight4_, &m_sliderBumpWeight4_);

	m_fRdec_ = pWaterPlaneProperty->m_vWaterRGBHalflife.x = m_fRdecOrg_;
	m_fGdec_ = pWaterPlaneProperty->m_vWaterRGBHalflife.y = m_fGdecOrg_;
	m_fBdec_ = pWaterPlaneProperty->m_vWaterRGBHalflife.z = m_fBdecOrg_;

	SetLinearSliderWithValue_(m_fHalfSlope_, m_fHalfIntercept_, m_fRdec_, &m_sliderR_dec_);
	SetLinearSliderWithValue_(m_fHalfSlope_, m_fHalfIntercept_, m_fGdec_, &m_sliderG_dec_);
	SetLinearSliderWithValue_(m_fHalfSlope_, m_fHalfIntercept_, m_fBdec_, &m_sliderB_dec_);

	m_fFresnelConstant_ = pWaterPlaneProperty->m_fFresnelConstant = m_fFresnelConstantOrg_;
	SetLinearSliderWithValue_(m_fFresnelSlope_, m_fFresnelIntercept_, m_fFresnelConstant_, &m_sliderFresnel_);

	m_fReflectAdjust_ = pWaterPlaneProperty->m_fReflectAdjust = m_fReflectAdjustOrg_;
	SetLinearSliderWithValue_(m_fReflectAdjustSlope_, m_fReflectAdjustIntercept_, m_fReflectAdjust_, &m_sliderAdjustFactor_);

	//water fog
	m_nFogNear_ = m_nFogNearOrg_;
	pWaterPlaneProperty->m_fFogNear = m_nFogNearOrg_ * 100;
	m_nFogFar_ = m_nFogFarOrg_;
	pWaterPlaneProperty->m_fFogFar = m_nFogFarOrg_ * 100;
	SetLinearSliderWithValue_( 1, 0, m_nFogNear_, &m_sliderFogNear_);
	SetLinearSliderWithValue_( 1, 0, m_nFogFar_, &m_sliderFogFar_);

	//-------------------------------------------------------------------
	//UnderWater
	
	//fog
	m_nUnderWaterFogNear_ = m_nUnderWaterFogNearOrg_;
	pWaterPlaneProperty->m_fUnderWaterFogNear = m_nUnderWaterFogNearOrg_ * 100;
	m_nUnderWaterFogFar_ = m_nUnderWaterFogFarOrg_;
	pWaterPlaneProperty->m_fUnderWaterFogFar = m_nUnderWaterFogFarOrg_ * 100;
	SetLinearSliderWithValue_( 1, 0, m_nUnderWaterFogNear_, &m_sliderUnderWaterFogNear_);
	SetLinearSliderWithValue_( 1, 0, m_nUnderWaterFogFar_, &m_sliderUnderWaterFogFar_);

	//Distortion scale
	m_fUnderWaterBumpUVScale_ = pWaterPlaneProperty->m_fUnderWaterBumpUVScale = m_fUnderWaterBumpUVScaleOrg_;
	SetLinearSliderWithValue_(m_fUnderWaterBumpScaleSlope_, m_fUnderWaterBumpScaleIntercept_, m_fUnderWaterBumpUVScale_, &m_sliderUnderWaterBumpUVScale_);
	//-------------------------------------------------------------------

	UpdateData(FALSE);
}

void CDlgWaterEditor::OnBnClickedButtonWaterExport()
{
	if(m_pWaterPlane_ == NULL)
		return;

	CFileDialog dlg(FALSE,".wpi", (m_pWaterPlane_->GetNodeName() + ".wpi").c_str());
	dlg.m_ofn.lpstrFilter = "Water Plane Information (*.wpi)\0*.wpi";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR;
	dlg.m_ofn.lpstrInitialDir = CXMLResourceList::GetReference().GetZoneUtilsPath();
	if (dlg.DoModal() == IDOK)
	{
		MXml kXml;
		MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "UTF-8", "");
		kXml.Doc()->LinkEndChild(pDecl);

		MXmlElement *pRootElement = new MXmlElement("WATER_CONFIG");
		kXml.Doc()->LinkEndChild(pRootElement);

		m_pWaterPlane_->SaveToXML(*pRootElement);

		CString fileName(dlg.GetPathName());
		kXml.SaveFile(fileName);
		//AfxMessageBox( fileName + "으로 내보내기 성공" );
	}
}

void CDlgWaterEditor::OnBnClickedButtonWaterImport()
{
	if(m_pWaterPlane_ == NULL)
		return;

	std::string fullPath(CXMLResourceList::GetReference().GetZoneUtilsPath());
	CListViewOpenDlg dlg(fullPath.c_str());
	dlg.SetTitle("Water Plane Config File");
	dlg.SetFilter("Water Plane Config\0*.wpi");

	if ( dlg.DoModal() == IDOK ) 
	{
		std::string strFilename(dlg.GetFileName().GetString());
		if(strFilename.empty())
			return;
		
		fullPath += strFilename;
		MXml kXml;
		if ( !kXml.LoadFile( fullPath.c_str() ) )
			return;

		MXmlElement *pRoot = kXml.Doc()->FirstChildElement("WATER_CONFIG");
		if(pRoot == NULL)
			return;

		m_pWaterPlane_->SetWaterPlaneInfoFromXML(*pRoot);
		ResetWaterPlane(m_pWaterPlane_);
	}
}

void CDlgWaterEditor::OnBnClickedCheckCustomFog()
{
	UpdateData(TRUE);
	_ASSERT( m_pWaterPlane_ );
	m_pWaterPlane_->GetWaterPlaneProperty()->m_bUseCustomFog = m_bUseCustomFog_? true : false;
}

LRESULT CDlgWaterEditor::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = CDialog::WindowProc(message, wParam, lParam);

	switch(message) {
		case WM_PAINT:
		{
			CMFCHelper::FillWnd( GetDlgItem(IDC_WATER_COLOR_PICK), m_crFogColor_);
			CMFCHelper::FillWnd( GetDlgItem(IDC_WATER_DIFFUSE_PICK), m_crDiffuseColor_);
		}
		break;
		case WM_USER_AUTOEDIT_VALID:
		{
			CString cstrColor;
			COLORREF crColor;

			m_ctlAutoEditFogR_.GetWindowText(cstrColor);
			int nR = atoi(cstrColor);

			m_ctlAutoEditFogG_.GetWindowText(cstrColor);
			int nG = atoi(cstrColor);

			m_ctlAutoEditFogB_.GetWindowText(cstrColor);
			int nB = atoi(cstrColor);

			crColor = RGB( nR, nG, nB );
			if( crColor != m_crFogColor_)
			{
				m_crFogColor_ = crColor;
				DrawAndSetWaterFog_();
			}

			m_ctlAutoEditDiffuseR_.GetWindowText(cstrColor);
			nR = atoi(cstrColor);

			m_ctlAutoEditDiffuseG_.GetWindowText(cstrColor);
			nG = atoi(cstrColor);

			m_ctlAutoEditDiffuseB_.GetWindowText(cstrColor);
			nB = atoi(cstrColor);

			crColor = RGB( nR, nG, nB );
			if( crColor != m_crDiffuseColor_ )
			{
				m_crDiffuseColor_ = crColor;
				DrawAndSetWaterDiffuse_();
			}
		}
		break;
	}
	return result;
}

void CDlgWaterEditor::OnStnClickedWaterColorPick()
{
	CColorDialog dlg( m_crFogColor_ , CC_FULLOPEN);
	if ( dlg.DoModal() == IDOK )
	{
		m_crFogColor_ = dlg.GetColor();
		UpdateWaterFogEditBox_();
		DrawAndSetWaterFog_();
	}
}

void CDlgWaterEditor::OnStnClickedWaterDiffusePick()
{
	CColorDialog dlg( m_crDiffuseColor_ , CC_FULLOPEN);
	if(dlg.DoModal() == IDOK)
	{
		m_crDiffuseColor_ = dlg.GetColor();
		UpdateWaterDiffuseEditBox_();
		DrawAndSetWaterDiffuse_();
	}
}

void CDlgWaterEditor::UpdateWaterFogEditBox_()
{
	CString cstrColor;
	cstrColor.Format("%d", GetRValue( m_crFogColor_ ) );
    m_ctlAutoEditFogR_.SetText(cstrColor);

	cstrColor.Format("%d", GetGValue( m_crFogColor_ ) );
    m_ctlAutoEditFogG_.SetText(cstrColor);

	cstrColor.Format("%d", GetBValue( m_crFogColor_ ) );
    m_ctlAutoEditFogB_.SetText(cstrColor);
}

void CDlgWaterEditor::UpdateWaterDiffuseEditBox_()
{
	CString cstrColor;
	cstrColor.Format("%d", GetRValue( m_crDiffuseColor_ ) );
    m_ctlAutoEditDiffuseR_.SetText(cstrColor);

	cstrColor.Format("%d", GetGValue( m_crDiffuseColor_ ) );
    m_ctlAutoEditDiffuseG_.SetText(cstrColor);

	cstrColor.Format("%d", GetBValue( m_crDiffuseColor_ ) );
    m_ctlAutoEditDiffuseB_.SetText(cstrColor);
}

void CDlgWaterEditor::DrawAndSetWaterFog_()
{
	CMFCHelper::FillWnd( GetDlgItem(IDC_WATER_COLOR_PICK), m_crFogColor_);
	m_pWaterPlane_->GetWaterPlaneProperty()->m_vFogColor.x = float( GetRValue( m_crFogColor_ ) ) / 255;
	m_pWaterPlane_->GetWaterPlaneProperty()->m_vFogColor.y = float( GetGValue( m_crFogColor_ ) ) / 255;
	m_pWaterPlane_->GetWaterPlaneProperty()->m_vFogColor.z = float( GetBValue( m_crFogColor_ ) ) / 255;
}

void CDlgWaterEditor::DrawAndSetWaterDiffuse_()
{
	CMFCHelper::FillWnd( GetDlgItem(IDC_WATER_DIFFUSE_PICK), m_crDiffuseColor_);
	m_pWaterPlane_->GetWaterPlaneProperty()->m_vDiffuseColor.x = float( GetRValue( m_crDiffuseColor_ ) ) / 255;
	m_pWaterPlane_->GetWaterPlaneProperty()->m_vDiffuseColor.y = float( GetGValue( m_crDiffuseColor_ ) ) / 255;
	m_pWaterPlane_->GetWaterPlaneProperty()->m_vDiffuseColor.z = float( GetBValue( m_crDiffuseColor_ ) ) / 255;
}

void CDlgWaterEditor::OnBnClickedButtonBumpTexture1()
{
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter = "Water Plane BumpMap File (*.*)\0*.*";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR;
	//dlg.m_ofn.lpstrInitialDir = CXMLResourceList::GetReference().GetZoneUtilsPath();
	if (dlg.DoModal() == IDOK)
	{
		m_cstrBumpTex1_ = dlg.GetFileName();
		m_pWaterPlane_->GetWaterPlaneProperty()->m_strBumpTex1 = m_cstrBumpTex1_.GetString();
		m_pWaterPlane_->DestroyBumpMap();
		m_pWaterPlane_->LoadBumpMap();
		UpdateData(FALSE);
	}
}

void CDlgWaterEditor::OnBnClickedButtonBumpTexture2()
{
	CFileDialog dlg(TRUE);
	dlg.m_ofn.lpstrFilter = "Water Plane BumpMap File (*.*)\0*.*";
	dlg.m_ofn.Flags |= OFN_NOCHANGEDIR;
	//dlg.m_ofn.lpstrInitialDir = CXMLResourceList::GetReference().GetZoneUtilsPath();
	if (dlg.DoModal() == IDOK)
	{
		m_cstrBumpTex2_ = dlg.GetFileName();

		m_pWaterPlane_->GetWaterPlaneProperty()->m_strBumpTex2 = m_cstrBumpTex2_.GetString();
		m_pWaterPlane_->DestroyBumpMap();
		m_pWaterPlane_->LoadBumpMap();
		UpdateData(FALSE);
	}
}

void CDlgWaterEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if ( bShow)
	{
		m_bActive_ = true;
	}
	else
	{
		m_bActive_ = false;
	}
}
