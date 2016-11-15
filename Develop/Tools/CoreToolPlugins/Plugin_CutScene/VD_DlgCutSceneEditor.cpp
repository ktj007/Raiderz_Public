// VD_DlgCutSceneEditor.cpp : 구현 파일입니다.
//


#include "stdafx.h"
//#include "CoreToolRS3.h"

#include "C_ControlMediator.h"
#include "C_ControlMediatorHelperAction.h"
#include "VD_DlgCutSceneEditor.h"

#include "WS_CutSceneXCutSceneWrapper.h"
#include "VD_DlgCutSceneMoveKey.h"
#include "VD_DlgCutSceneDOFKey.h"
#include "VD_DlgCutSceneActorType.h"
#include "VD_DlgCutSceneAniKey.h"
#include "VD_DlgCutSceneBGMKey.h"
#include "VD_DlgCutSceneCaptionKey.h"
#include "VD_DlgCutSceneLetterBoxKey.h"
#include "VD_DlgCutSceneEnvKey.h"
#include "VD_DlgCutSceneEffectKey.h"
#include "VD_DlgCutSceneFadeKey.h"
#include "VD_DlgCutSceneTalentKey.h"
#include "VD_DlgCutSceneMoveVelAverage.h"

#include "M_ToolCmdMgr.h"

#include "V_MFCHelper.h"
#include "VR_CutSceneObjectList.h"

#include "REnvironmentSet.h"
#include "VR_EntityObjectList.h"

#pragma warning( push )
#pragma warning(disable:893)
#pragma warning(disable:4800)
#pragma warning( disable : 4996 )


//////////////////////////////////////////////////////////////////////////

// custom axis controller
class CChartControllerAxis : public CChartStandardAxis
{
public:
	CChartControllerAxis(CDlgCutSceneEditor* pOwner, bool bObjectController)
	{
		m_pOwner = pOwner;
		m_bObjectController = bObjectController;
	}

	virtual ~CChartControllerAxis(){}

protected:
	virtual TChartString GetTickLabel(double TickValue) const
	{
		double dLastObject = m_bObjectController? m_pOwner->m_dLastObjectNum_ : m_pOwner->m_dLastControllerNum_;
		if (TickValue == 0 || TickValue <= dLastObject)
			return "";

		int dObjSize = m_bObjectController? (int)m_pOwner->m_vObjectsBar_.size() : (int)m_pOwner->m_vControllerBar_.size();
		int i = ((int)(-TickValue +1)) - 1;
		if (i >= dObjSize || i < 0)
			return "";

		if (m_bObjectController)
		{
			return m_pOwner->m_vObjectsBar_[i].m_strBarName;
		}
		else
		{
			return m_pOwner->m_vControllerBar_[i].m_strBarName;
		}
	}

	virtual double GetFirstTickValue() const
	{
		double dRetVal = m_dFirstTickValue - 0.5;
		if (m_bDiscrete)
		{
			dRetVal = m_dFirstTickValue - m_dTickIncrement - 0.5;
		}
		return dRetVal; 
	}

private:
	CDlgCutSceneEditor* m_pOwner;
	bool				m_bObjectController;
};

//////////////////////////////////////////////////////////////////////////

class CCustomObjectMouseListener : public CChartMouseListener
{
public:
	CCustomObjectMouseListener(CDlgCutSceneEditor* pOwner)
	{
		m_pOwner = pOwner;
	}

	virtual void OnMouseEventSeries(MouseEvent mouseEvent, CPoint point,CChartSerie* pSerie, unsigned uPointIndex)
	{
		if (mouseEvent == CChartMouseListener::LButtonDown &&
			uPointIndex != INVALID_POINT)
		{
			double dY = pSerie->GetYPointValue(uPointIndex);
			int nCtrlIdx = (int)(-dY);
			if (pSerie == m_pOwner->m_mapBarTypesForObject_[CDlgCutSceneEditor::E_TOTAL])
			{
				--nCtrlIdx;
			}
			m_pOwner->m_nCurrentSelected = -1;
			m_pOwner->m_fCurrentSelectedValue = -1.f;
			m_pOwner->m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->ChangeObjectSelection(nCtrlIdx, false, nCtrlIdx);
			return;
		}

		if (mouseEvent == CChartMouseListener::RButtonDown &&
			uPointIndex != INVALID_POINT)
		{
			int nCurrentObjectType = m_pOwner->m_pCutSceneWrapper->GetContextMenuIdForObject();
			if (nCurrentObjectType < 0)
				return;

			CMenu menu;
			{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				menu.LoadMenu(IDR_MENU_CUTSCENE);
			}
			CMenu* pPopup = menu.GetSubMenu(nCurrentObjectType);
			m_pOwner->m_ChartCtrlForObject.ClientToScreen(&point);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN , point.x, point.y, m_pOwner);

			return;
		}
	}

private:
	CDlgCutSceneEditor* m_pOwner;
};

class CCustomControllerMouseListener : public CChartMouseListener
{
public:
	CCustomControllerMouseListener(CDlgCutSceneEditor* pOwner)
	{
		m_pOwner = pOwner;
	}

	float GetCurrentSelectedValue(CChartSerie* pSerie, CPoint& point, bool& bIsKeyFrame)
	{
		float fResult = 0.f;

		bIsKeyFrame = false;
		for (int i = (int)CDlgCutSceneEditor::E_INTERVAL_KEY; i <= (int)CDlgCutSceneEditor::E_WARP_KEY; ++i )
		{
			CChartSerie* pKeySerie = m_pOwner->m_mapBarTypesForController_[ (CDlgCutSceneEditor::E_BAR_TYPE)i ];
			size_t tPointNum = pKeySerie->GetPointsCount();
			for (size_t i = 0; i < tPointNum; ++i)
			{
				CPoint ValuePoint;
				double dXPointValue = pKeySerie->GetXPointValue(i);
				pKeySerie->ValueToScreen( dXPointValue,  pKeySerie->GetYPointValue(i), ValuePoint);

				int xDist = abs(point.x - ValuePoint.x);
				int yDist = abs(point.y - ValuePoint.y);
				if (xDist<=5 && yDist<=10)
				{
					bIsKeyFrame = true;
					fResult = (float)dXPointValue;
					break;
				}
			}

			if(bIsKeyFrame)
				break;
		}

		if (!bIsKeyFrame)
		{
			fResult = (float)pSerie->XScreenToValue(point.x);
		}

		return fResult;
	}

	virtual void OnMouseEventSeries(MouseEvent mouseEvent, CPoint point,CChartSerie* pSerie, unsigned uPointIndex)
	{
		#define SET_CURRENT_SELECTION \
			m_pOwner->m_nCurrentSelected = (int)(-pSerie->GetYPointValue(uPointIndex)); \
			int nId = m_pOwner->m_pCutSceneWrapper->GetContextMenuIdForController(m_pOwner->m_nCurrentSelected); \
			if (nId < 0) return; \
			bool bIsKeyFrame = false; float fBeforeSelectedValue = m_pOwner->m_fCurrentSelectedValue; float fCurrentSelectedValue; \
			fCurrentSelectedValue = m_pOwner->m_fCurrentSelectedValue = GetCurrentSelectedValue(pSerie, point, bIsKeyFrame);

		if (mouseEvent == CChartMouseListener::RButtonDown &&
			uPointIndex != INVALID_POINT)
		{

			SET_CURRENT_SELECTION
			CMenu menu;
			{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				menu.LoadMenu(IDR_MENU_CUTSCENE);
			}
			CMenu* pPopup = menu.GetSubMenu(nId);
			m_pOwner->m_pCutSceneWrapper->SetContextMenuForController(nId, pPopup, bIsKeyFrame, m_pOwner->m_nCurrentSelected);
			m_pOwner->m_ChartCtrlForController.ClientToScreen(&point);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN , point.x, point.y, m_pOwner);
		}
		else if (mouseEvent == CChartMouseListener::LButtonDown &&
			uPointIndex != INVALID_POINT)
		{
			SET_CURRENT_SELECTION
			if (false == bIsKeyFrame)
			{
				// by pok. 이렇게 코드를 짰습니다. 용서해 주세요....-_-
				// backup (for side effect)
				int nOrgSelected = m_pOwner->m_nCurrentSelected;
				m_pOwner->RememberLastControllerScrollPos();
				int nOrgScrollerH = m_pOwner->m_nLastControllerHPoint;
				int nOrgScrollerV = m_pOwner->m_nLastControllerVPoint;

				m_pOwner->m_pControlMediator_->GetHelperAction()->RemoveAllSelection(true);

				// set (for "RemoveAllSelection" side effect)
				m_pOwner->m_nCurrentSelected = nOrgSelected;
				m_pOwner->m_fCurrentSelectedValue = fBeforeSelectedValue;
				m_pOwner->ClearSelectedController();

				// set (for "ClearSelectedController" side effect)
				m_pOwner->m_nLastControllerHPoint = nOrgScrollerH;
				m_pOwner->m_nLastControllerVPoint = nOrgScrollerV;
				m_pOwner->SetLastControllerScrollPos();
				m_pOwner->m_nCurrentSelected = nOrgSelected;
				m_pOwner->m_fCurrentSelectedValue = fCurrentSelectedValue;
			}

			switch (nId)
			{
			case coretoolcutscene::E_MOVE_CTRL:
				{
					m_pOwner->m_pCutSceneWrapper->MoveCameraToMoveKeyAndSelectIt(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, m_pOwner);
					if (false == bIsKeyFrame)
						m_pOwner->m_ctrlControllerCmd.SetToMoveController();
				}
				break;

			case coretoolcutscene::E_DOF_CTRL:
				{
					if (bIsKeyFrame)
					{
						float fTime, fFocusDist, fFocusRange, fCocSize;
						if (m_pOwner->m_pCutSceneWrapper->GetDOFControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fFocusDist, fFocusRange, fCocSize))
						{
							m_pOwner->m_pCutSceneWrapper->ChangeDOFControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fFocusDist, fFocusRange, fCocSize, m_pOwner);
						}
					}
					m_pOwner->m_ctrlControllerCmd.SetToDofController();
				}
				break;

			case coretoolcutscene::E_CAPTION_CTRL:
				{
					if (bIsKeyFrame)
					{
						float fTime, fDuration;
						std::string strScript;
						if (m_pOwner->m_pCutSceneWrapper->GetCaptionControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDuration, strScript))
						{
							m_pOwner->m_pCutSceneWrapper->ChangeCaptionControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDuration, strScript, m_pOwner);
						}
					}
					m_pOwner->m_ctrlControllerCmd.SetToCaptionController();
				}
				break;

			case coretoolcutscene::E_LETTERBOX_CTRL:
				{
					m_pOwner->m_ctrlControllerCmd.SetToLetterBoxController();
				}
				break;

			case coretoolcutscene::E_BGM_CTRL:
				{
					if (bIsKeyFrame)
					{
						float fTime, fVolume;
						std::string strFileName;
						bool bLoop;
						if (m_pOwner->m_pCutSceneWrapper->GetBGMControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strFileName, fVolume, bLoop))
						{
							m_pOwner->m_pCutSceneWrapper->ChangeBGMControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strFileName, fVolume, bLoop, m_pOwner);
						}
					}
					m_pOwner->m_ctrlControllerCmd.SetToBGMController();
				}
				break;

			case coretoolcutscene::E_ANI_CTRL:
				{
					if (bIsKeyFrame)
					{
						float fTime;
						std::string strAniName;
						if (m_pOwner->m_pCutSceneWrapper->GetAniControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strAniName))
						{
							m_pOwner->m_pCutSceneWrapper->ChangeAniControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strAniName, m_pOwner);
						}
					}
					m_pOwner->m_ctrlControllerCmd.SetToAniController();
				}
				break;

			case coretoolcutscene::E_TALENT_CTRL:
				{
					if (bIsKeyFrame)
					{
						float fTime;
						int nID;
						if (m_pOwner->m_pCutSceneWrapper->GetTalentControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nID))
						{
							m_pOwner->m_pCutSceneWrapper->ChangeTalentControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nID, m_pOwner);
						}
					}
					m_pOwner->m_ctrlControllerCmd.SetToTalentController();
				}
				break;

			case coretoolcutscene::E_ENV_CTRL:
				{
					if (bIsKeyFrame)
					{
						float fTime;
						int nEnvTime, nEnvWeather;
						std::string strSetName;
						if (m_pOwner->m_pCutSceneWrapper->GetEnvControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nEnvTime, nEnvWeather, strSetName))
						{
							m_pOwner->m_pCutSceneWrapper->ChangeEnvControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nEnvTime, nEnvWeather, strSetName, m_pOwner);
						}
					}
					m_pOwner->m_ctrlControllerCmd.SetToEnvController();
				}
				break;

			case coretoolcutscene::E_EFFECT_CTRL:
				{
					if (bIsKeyFrame)
					{
						float fTime;
						float fDurationTime;
						vec3 vPos;
						vec3 vDir;
						vec3 vUp;
						std::string strEffectName;
						std::string strEffectInfo;
						if(m_pOwner->m_pCutSceneWrapper->GetEffectControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo))
						{
							m_pOwner->m_pCutSceneWrapper->ChangeEffectControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo, m_pOwner);
						}
					}
					m_pOwner->m_ctrlControllerCmd.SetToEffectController();
				}
				break;

			case coretoolcutscene::E_FADE_CTRL:
				{
					if (bIsKeyFrame)
					{
						float fTime, fAlpha;
						if(m_pOwner->m_pCutSceneWrapper->GetFadeControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fAlpha))
						{
							m_pOwner->m_pCutSceneWrapper->ChangeFadeControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fAlpha, m_pOwner);
						}
					}
					m_pOwner->m_ctrlControllerCmd.SetToFadeController();
				}
				break;
			}
		}
	}

private:
	CDlgCutSceneEditor* m_pOwner;
};

//////////////////////////////////////////////////////////////////////////

// CDlgCutSceneEditor 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneEditor, CDialog)

CDlgCutSceneEditor::CDlgCutSceneEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneEditor::IDD, pParent)
	, m_pControlMediator_(NULL)
{
	m_dLastObjectNum_ = 0;
	m_dLastObjectMinHeight_ = -5;

	m_dLastControllerNum_ = 0;
	m_dLastControllerMinHeight_ = -12;

	//m_dScrollBarHideProtection_ = -0.022;
	m_dScrollBarHideProtection_ = -0.028;

	m_nCurrentSelected = -1;

	m_nLastControllerHPoint = 0;
	m_nLastControllerVPoint = 0;
}

CDlgCutSceneEditor::~CDlgCutSceneEditor()
{
	delete m_pListenerForObject;
	delete m_pListenerForController;
}

void CDlgCutSceneEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUTSCNE_TRACKBAR, m_ChartCtrlForObject);
	DDX_Control(pDX, IDC_CUTSCNE_CTRL_TRACKBAR, m_ChartCtrlForController);
}

BEGIN_MESSAGE_MAP(CDlgCutSceneEditor, CDialog)
	ON_COMMAND(ID_CUTSCENE_CAMOBJ_ADDMOVE, &CDlgCutSceneEditor::OnCutsceneCamobjAddMoveController)
	ON_COMMAND(ID_CUTSCENE_MOVECTRL_DELETE, &CDlgCutSceneEditor::OnCtrlGeneralDelete)
	ON_COMMAND(ID_CUTSCENE_MOVECTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneMovectrlDelkey)
	ON_COMMAND(ID_CUTSCENE_MOVECTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneMovectrlAddkey)
	ON_COMMAND(ID_INTERPOLATIONTYPE_LINEAR, &CDlgCutSceneEditor::OnInterpolationtypeLinear)
	ON_COMMAND(ID_INTERPOLATIONTYPE_CATMULL, &CDlgCutSceneEditor::OnInterpolationtypeCatmull)
	ON_COMMAND(ID_CUTSCENE_MOVECTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneMovectrlEditkey)
	ON_COMMAND(ID_CUTSCENE_CAMOBJ_ADDDOF, &CDlgCutSceneEditor::OnCutsceneCamobjAddDOF)
	ON_COMMAND(ID_CUTSCENE_DOFCTRL_DELETE, &CDlgCutSceneEditor::OnCtrlGeneralDelete)
	ON_COMMAND(ID_CUTSCENE_DOFCTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneDofctrlEditkey)
	ON_COMMAND(ID_CUTSCENE_DOFCTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneDofctrlDelkey)
	ON_COMMAND(ID_CUTSCENE_DOFCTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneDofctrlAddkey)
	ON_COMMAND(ID_CUTSCENE_MOVECTRL_COLLISION, &CDlgCutSceneEditor::OnCutsceneMovectrlCollision)
	ON_COMMAND(ID_CUTSCENE_ACTOR_ADDMOVE, &CDlgCutSceneEditor::OnCutsceneActorAddmove)
	ON_COMMAND(ID_CUTSCENE_ACTOR_ADDANI, &CDlgCutSceneEditor::OnCutsceneActorAddani)
	ON_COMMAND(ID_CUTSCENE_ACTOR_ADDTALENT, &CDlgCutSceneEditor::OnCutsceneActorAddtalent)
	ON_COMMAND(ID_CUTSCENE_ACTOR_TYPE, &CDlgCutSceneEditor::OnCutsceneActorType)
	ON_COMMAND(ID_CUTSCENE_ANICTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneAnictrlEditkey)
	ON_COMMAND(ID_CUTSCENE_ANICTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneAnictrlDelkey)
	ON_COMMAND(ID_CUTSCENE_ANICTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneAnictrlAddkey)
	ON_COMMAND(ID_CUTSCENE_ANICTRL_DELETE, &CDlgCutSceneEditor::OnCutsceneAnictrlDelete)
	ON_COMMAND(ID_CUTSCENE_EVENTOBJ_ADDBGM, &CDlgCutSceneEditor::OnCutsceneEventobjAddbgm)
	ON_COMMAND(ID_CUTSCENE_EVENTOBJ_ADDCAPTION, &CDlgCutSceneEditor::OnCutsceneEventobjAddcaption)
	ON_COMMAND(ID_CUTSCENE_EVENTOBJ_ADDLETTERBOX, &CDlgCutSceneEditor::OnCutsceneEventobjAddletterbox)
	ON_COMMAND(ID_CUTSCENE_BGMCTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneBgmctrlEditkey)
	ON_COMMAND(ID_CUTSCENE_BGMCTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneBgmctrlDelkey)
	ON_COMMAND(ID_CUTSCENE_BGMCTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneBgmctrlAddkey)
	ON_COMMAND(ID_CUTSCENE_BGMCTRL_DELETE, &CDlgCutSceneEditor::OnCutsceneBgmctrlDelete)
	ON_COMMAND(ID_CUTSCENE_CAPTIONCTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneCaptionctrlEditkey)
	ON_COMMAND(ID_CUTSCENE_CAPTIONCTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneCaptionctrlDelkey)
	ON_COMMAND(ID_CUTSCENE_CAPTIONCTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneCaptionctrlAddkey)
	ON_COMMAND(ID_CUTSCENE_CAPTIONCTRL_DELETE, &CDlgCutSceneEditor::OnCutsceneCaptionctrlDelete)
	ON_COMMAND(ID_CUTSCENE_CAPTIONCTRL_EDIT, &CDlgCutSceneEditor::OnCutsceneCaptionctrlEdit)
	ON_COMMAND(ID_CUTSCENE_MOVECTRL_CAMHERE, &CDlgCutSceneEditor::OnCutsceneMovectrlCamHere)
	ON_COMMAND(ID_CUTSCENE_LETTERBOXCTRL_DELETE, &CDlgCutSceneEditor::OnCutsceneLetterBoxctrlDelete)
	ON_COMMAND(ID_CUTSCENE_EVENTOBJ_ADDENV, &CDlgCutSceneEditor::OnCutsceneEventobjAddenv)
	ON_COMMAND(ID_CUTSCENE_ENVCTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneEnvctrlEditkey)
	ON_COMMAND(ID_CUTSCENE_ENVCTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneEnvctrlDelkey)
	ON_COMMAND(ID_CUTSCENE_ENVCTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneEnvctrlAddkey)
	ON_COMMAND(ID_CUTSCENE_ENVCTRL_DELETE, &CDlgCutSceneEditor::OnCutsceneEnvctrlDelete)
	ON_COMMAND(ID_CUTSCENE_EVENTOBJ_ADDEFFECT, &CDlgCutSceneEditor::OnCutsceneEventobjAddEffect)
	ON_COMMAND(ID_CUTSCENE_EFFECTCTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneEffectctrlEditkey)
	ON_COMMAND(ID_CUTSCENE_EFFECTCTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneEffectctrlDelkey)
	ON_COMMAND(ID_CUTSCENE_EFFECTCTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneEffectctrlAddkey)
	ON_COMMAND(ID_CUTSCENE_EFFECTCTRL_DELETE, &CDlgCutSceneEditor::OnCutsceneEffectctrlDelete)
	ON_COMMAND(ID_CUTSCENE_EVENTOBJ_ADDFADE, &CDlgCutSceneEditor::OnCutsceneEventobjAddFade)
	ON_COMMAND(ID_CUTSCENE_FADECTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneFadectrlEditkey)
	ON_COMMAND(ID_CUTSCENE_FADECTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneFadectrlDelkey)
	ON_COMMAND(ID_CUTSCENE_FADECTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneFadectrlAddkey)
	ON_COMMAND(ID_CUTSCENE_FADECTRL_DELETE, &CDlgCutSceneEditor::OnCutsceneFadectrlDelete)
	ON_COMMAND(ID_CUTSCENE_MOVECTRL_AVERAGEVEL, &CDlgCutSceneEditor::OnCutsceneMovectrlAverageVel)
	ON_COMMAND(ID_MOVECONTROLLER_COPYLOCATIONKEY, &CDlgCutSceneEditor::OnMovecontrollerCopylocationkey)
	ON_COMMAND(ID_MOVECONTROLLER_PASTELOCATIONKEY, &CDlgCutSceneEditor::OnMovecontrollerPastelocationkey)
	ON_COMMAND(ID_CUTSCENE_TALENTCTRL_EDITKEY, &CDlgCutSceneEditor::OnCutsceneTalentctrlEditkey)
	ON_COMMAND(ID_CUTSCENE_TALENTCTRL_DELKEY, &CDlgCutSceneEditor::OnCutsceneTalentctrlDelkey)
	ON_COMMAND(ID_CUTSCENE_TALENTCTRL_ADDKEY, &CDlgCutSceneEditor::OnCutsceneTalentctrlAddkey)
	ON_COMMAND(ID_CUTSCENE_TALENTCTRL_DELETE, &CDlgCutSceneEditor::OnCutsceneTalentctrlDelete)
END_MESSAGE_MAP()


// CDlgCutSceneEditor 메시지 처리기입니다.

BOOL CDlgCutSceneEditor::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlObjectCmd.m_pOwner = this;
	m_ctrlControllerCmd.m_pOwner = this;

	CRect rect( 0, 0, 100, 100 );
	m_ctrlObjectCmd.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | WS_MAXIMIZE, rect, GetDlgItem(IDC_CUTSCENE_OBJECT_CMD), (UINT)-1);
	m_ctrlControllerCmd.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | WS_MAXIMIZE, rect, GetDlgItem(IDC_CUTSCENE_CONTROLLER_CMD), (UINT)-1);

	m_pListenerForObject = new CCustomObjectMouseListener(this);
	m_pListenerForController = new CCustomControllerMouseListener(this);

	// For Object
	m_ChartCtrlForObject.SetEdgeType(EDGE_ETCHED);
	m_ChartCtrlForObject.SetZoomEnabled(false);
	m_ChartCtrlForObject.SetPanEnabled(false);
	m_ChartCtrlForObject.RegisterMouseListener(m_pListenerForObject);

	CChartStandardAxis* pBottomAxis = m_ChartCtrlForObject.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxis->SetAutomatic(false);
	pBottomAxis->EnableScrollBar(true);
	pBottomAxis->SetAutoHideScrollBar(false);

	CChartControllerAxis* pLeftAxis = new CChartControllerAxis(this, true);
	m_ChartCtrlForObject.AttachCustomAxis(pLeftAxis, CChartCtrl::LeftAxis);
	pLeftAxis->SetAutomatic(false);
	pLeftAxis->EnableScrollBar(true);
	pLeftAxis->SetAutoHideScrollBar(false);
	pLeftAxis->SetMinMax(m_dLastObjectMinHeight_, 0);
	pLeftAxis->SetTickIncrement(false, 1);

	CChartBarSerie* pBarSeries = m_ChartCtrlForObject.CreateBarSerie();
	pBarSeries->SetHorizontal(true);
	pBarSeries->SetColor(RGB(196,196,196));
	pBarSeries->SetBarWidth(20);
	pBarSeries->SetInterSpace(1);
	pBarSeries->ShowGradient(false);
	pBarSeries->SetGroupId( unsigned int (E_TOTAL) );
	m_mapBarTypesForObject_[E_TOTAL] = pBarSeries;

	CChartBarSerie* pBarSeriesSelected = m_ChartCtrlForObject.CreateBarSerie();
	pBarSeriesSelected->SetHorizontal(true);
	pBarSeriesSelected->SetColor(RGB(0,255,0));
	pBarSeriesSelected->SetBarWidth(20);
	pBarSeriesSelected->SetInterSpace(1);
	pBarSeriesSelected->ShowGradient(false);
	pBarSeriesSelected->SetGroupId( unsigned int (E_SELECTED_OBJ) );
	m_mapBarTypesForObject_[E_SELECTED_OBJ] = pBarSeriesSelected;

	// For Controller
	m_ChartCtrlForController.SetEdgeType(EDGE_ETCHED);
	m_ChartCtrlForController.SetZoomEnabled(false);
	m_ChartCtrlForController.SetPanEnabled(false);
	m_ChartCtrlForController.RegisterMouseListener(m_pListenerForController);

	CChartStandardAxis* pBottomAxisController = m_ChartCtrlForController.CreateStandardAxis(CChartCtrl::BottomAxis);
	pBottomAxisController->SetAutomatic(false);
	pBottomAxisController->EnableScrollBar(true);
	pBottomAxisController->SetAutoHideScrollBar(false);

	CChartControllerAxis* pLeftAxisController = new CChartControllerAxis(this, false);
	m_ChartCtrlForController.AttachCustomAxis(pLeftAxisController, CChartCtrl::LeftAxis);
	pLeftAxisController->SetAutomatic(false);
	pLeftAxisController->EnableScrollBar(true);
	pLeftAxisController->SetAutoHideScrollBar(false);
	pLeftAxisController->SetMinMax(m_dLastControllerMinHeight_, 0);
	pLeftAxisController->SetTickIncrement(false, 1);

	CChartBarSerie* pBarSeriesController = m_ChartCtrlForController.CreateBarSerie();
	pBarSeriesController->SetHorizontal(true);
	pBarSeriesController->SetColor(RGB(0,255,0));
	pBarSeriesController->SetBarWidth(20);
	pBarSeriesController->SetInterSpace(1);
	pBarSeriesController->ShowGradient(false);
	pBarSeriesController->SetGroupId( unsigned int (E_TOTAL) );
	m_mapBarTypesForController_[E_TOTAL] = pBarSeriesController;

	CChartBarSerie* pBarSeriesControllerEnd = m_ChartCtrlForController.CreateBarSerie();
	pBarSeriesControllerEnd->SetHorizontal(true);
	pBarSeriesControllerEnd->SetColor(RGB(200,200,255));
	pBarSeriesControllerEnd->SetBarWidth(20);
	pBarSeriesControllerEnd->SetInterSpace(1);
	pBarSeriesControllerEnd->ShowGradient(false);
	pBarSeriesControllerEnd->SetGroupId( unsigned int (E_INTERVAL_END) );
	pBarSeriesControllerEnd->SetShadowDepth(0);
	m_mapBarTypesForController_[E_INTERVAL_END] = pBarSeriesControllerEnd;

	CChartBarSerie* pBarSeriesControllerStart = m_ChartCtrlForController.CreateBarSerie();
	pBarSeriesControllerStart->SetHorizontal(true);
	pBarSeriesControllerStart->SetColor(RGB(0,255,0));
	pBarSeriesControllerStart->SetBarWidth(20);
	pBarSeriesControllerStart->SetInterSpace(1);
	pBarSeriesControllerStart->ShowGradient(false);
	pBarSeriesControllerStart->SetGroupId( unsigned int (E_INTERVAL_START) );
	pBarSeriesControllerStart->SetShadowDepth(0);
	m_mapBarTypesForController_[E_INTERVAL_START] = pBarSeriesControllerStart;

	CChartPointsSerie* pBarSeriesControllerKey = m_ChartCtrlForController.CreatePointsSerie();
	pBarSeriesControllerKey->SetPointType(CChartPointsSerie::ptRectangle);
	pBarSeriesControllerKey->SetColor(RGB(0,0,255));
	pBarSeriesControllerKey->SetPointSize(10, 20);
	pBarSeriesControllerKey->SetShadowDepth(0);
	m_mapBarTypesForController_[E_INTERVAL_KEY] = pBarSeriesControllerKey;

	CChartPointsSerie* pBarSeriesControllerWarpKey = m_ChartCtrlForController.CreatePointsSerie();
	pBarSeriesControllerWarpKey->SetPointType(CChartPointsSerie::ptRectangle);
	pBarSeriesControllerWarpKey->SetColor(RGB(255,0,0));
	pBarSeriesControllerWarpKey->SetPointSize(10, 20);
	pBarSeriesControllerWarpKey->SetShadowDepth(0);
	m_mapBarTypesForController_[E_WARP_KEY] = pBarSeriesControllerWarpKey;

	CChartPointsSerie* pBarSeriesControllerSelectedKey = m_ChartCtrlForController.CreatePointsSerie();
	pBarSeriesControllerSelectedKey->SetPointType(CChartPointsSerie::ptRectangle);
	pBarSeriesControllerSelectedKey->SetColor(RGB(255,255,255));
	pBarSeriesControllerSelectedKey->SetPointSize(10, 20);
	pBarSeriesControllerSelectedKey->SetShadowDepth(0);
	m_mapBarTypesForController_[E_SELECTED_KEY] = pBarSeriesControllerSelectedKey;

	m_ChartCtrlForObject.RefreshCtrl();
	m_ChartCtrlForController.RefreshCtrl();

	return TRUE;
}

void CDlgCutSceneEditor::OnCancel()
{
	ShowEditor(false);
}

void CDlgCutSceneEditor::Init( CControlMediator* pMediator, CXCutSceneWrapper* pCutSceneWrapper )
{
	m_pControlMediator_ = pMediator;
	m_pCutSceneWrapper = pCutSceneWrapper;
}

void CDlgCutSceneEditor::ShowEditor( bool bShow )
{
	ShowWindow( bShow? SW_SHOW : SW_HIDE );
}

void CDlgCutSceneEditor::OnOK()
{
}

void CDlgCutSceneEditor::ClearObject()
{
	m_dLastObjectNum_ = 0;
	for (MAP_BAR_TYPES::iterator itr = m_mapBarTypesForObject_.begin(); itr != m_mapBarTypesForObject_.end(); ++itr)
	{
		CChartSerie* pBar = itr->second;
		pBar->ClearSerie();
	}
	m_vObjectsBar_.clear();
	m_ctrlObjectCmd.SetToEmpty();
	m_copiedKeyInfo.Clear();

	ClearController();
}

void CDlgCutSceneEditor::AddObject( const std::string& _rName, double _dDuration )
{
	m_vObjectsBar_.push_back( STrackBarInfo(_rName, _dDuration) );
	m_dLastObjectNum_ -= 1;
}

void CDlgCutSceneEditor::RefreshObject()
{
	double i = 0;
	double dMax = 0;
	for (VEC_BARS::iterator itr = m_vObjectsBar_.begin(); itr != m_vObjectsBar_.end(); ++itr)
	{
		for (MAP_BAR_VALUES::iterator itr_bar = itr->m_mapBarValues.begin(); itr_bar != itr->m_mapBarValues.end(); ++itr_bar)
		{
			MAP_BAR_TYPES::iterator itr_bar_serie = m_mapBarTypesForObject_.find(itr_bar->first);
			if (itr_bar_serie == m_mapBarTypesForObject_.end())
				continue;

			if (dMax == 0)
			{
				dMax = itr->m_mapBarValues[E_TOTAL].at(0);
			}

			CChartSerie* pBarSeries = itr_bar_serie->second;
			for(std::vector<double>::iterator itr_bar_value = itr_bar->second.begin(); itr_bar_value != itr_bar->second.end(); ++itr_bar_value)
			{
				pBarSeries->AddPoint(*itr_bar_value, (-i) -1);
			}
		}

		i += 1;
	}

	if (dMax > 10)
	{
		dMax = 10;
	}

	double dMinWidthValue = m_dScrollBarHideProtection_*dMax;
	m_ChartCtrlForObject.GetAxis(CChartCtrl::BottomAxis)->SetMinMax(dMinWidthValue, dMax);

	m_mapBarTypesForObject_[E_TOTAL]->AddPoint(dMinWidthValue, 0);
	if (m_dLastObjectNum_ <= m_dLastObjectMinHeight_)
	{
		m_mapBarTypesForObject_[E_TOTAL]->AddPoint(0, m_dLastObjectNum_ - 1.5);
	}
	else
	{
		m_ChartCtrlForObject.GetAxis(CChartCtrl::LeftAxis)->SetMinMax(m_dLastObjectMinHeight_, 0);
	}

	m_ChartCtrlForObject.RefreshCtrl();
}

void CDlgCutSceneEditor::OnSelectObject(int nIdx, bool bScroll)
{
	CChartSerie* pSeriesForSelected = m_mapBarTypesForObject_[E_SELECTED_OBJ];
	pSeriesForSelected->ClearSerie();

	CChartSerie* pSeriesTotal = m_mapBarTypesForObject_[E_TOTAL];
	unsigned nTempForReverseCalc = pSeriesTotal->GetPointsCount() - 1;
	double dSelected = pSeriesTotal->GetYPointValue( nTempForReverseCalc - unsigned(nIdx) );
	double dWidth,dTemp;
	pSeriesTotal->GetSerieXMinMax(dTemp, dWidth);

	pSeriesForSelected->AddPoint(dWidth, dSelected - 0.5);

	if (bScroll)
	{
		CChartScrollBar* pVScrollBar = m_ChartCtrlForObject.GetLeftAxis()->GetScrollBar();
		int nScroll = (int)(-dSelected*2);
		if (pVScrollBar && nTempForReverseCalc > 4)
		{
			m_ChartCtrlForObject.UpdateWithVScrollPos(SB_THUMBPOSITION, 0, pVScrollBar);
			m_ChartCtrlForObject.UpdateWithVScrollPos(SB_THUMBPOSITION, (int)nScroll, pVScrollBar);
		}
	}
}

void CDlgCutSceneEditor::ClearController()
{
	m_dLastControllerNum_ = 0;
	RememberLastControllerScrollPos();

	for (MAP_BAR_TYPES::iterator itr = m_mapBarTypesForController_.begin(); itr != m_mapBarTypesForController_.end(); ++itr)
	{
		CChartSerie* pBar = itr->second;
		pBar->ClearSerie();
	}

	m_vControllerBar_.clear();
	SetLastControllerScrollPos();
	m_ctrlControllerCmd.SetToEmpty();
}

int CDlgCutSceneEditor::AddController(const std::string& _rName)
{
	_ASSERT(m_vObjectsBar_.empty() == false);
	_ASSERT(m_vObjectsBar_[0].m_mapBarValues.find(E_TOTAL) != m_vObjectsBar_[0].m_mapBarValues.end());
	m_vControllerBar_.push_back( STrackBarInfo(_rName, m_vObjectsBar_[0].m_mapBarValues[E_TOTAL].at(0)) );
	m_dLastControllerNum_ -= 1;

	return int(m_vControllerBar_.size() - 1);
}

void CDlgCutSceneEditor::RefreshController()
{
	bool bScroll = false;
	double dSelected = 0.0;

	double i = 0;
	double dWidthMax = 0;
	for (VEC_BARS::iterator itr = m_vControllerBar_.begin(); itr != m_vControllerBar_.end(); ++itr)
	{
		double dKeyMin(0), dKeyMax(0);
		for (MAP_BAR_VALUES::iterator itr_bar = itr->m_mapBarValues.begin(); itr_bar != itr->m_mapBarValues.end(); ++itr_bar)
		{
			MAP_BAR_TYPES::iterator itr_bar_serie = m_mapBarTypesForController_.find(itr_bar->first);
			if (itr_bar_serie == m_mapBarTypesForController_.end())
				continue;

			if (dWidthMax == 0)
			{
				dWidthMax = itr->m_mapBarValues[E_TOTAL].at(0);
			}

			bool bCheckMinMax = false;
			double dIndexOffset = -0.65;

			switch(itr_bar->first)
			{
			case E_TOTAL:
				{
					dIndexOffset = -0.2;
					break;
				}
			case E_INTERVAL_KEY:
				{
					bCheckMinMax = true;
					dKeyMin = dWidthMax;
					break;
				}
			case E_INTERVAL_END:
				{
					_ASSERT(itr_bar->second.size() == 1);
					itr_bar->second[0] = dKeyMax;
					break;
				}
			case E_INTERVAL_START:
				{
					_ASSERT(itr_bar->second.size() == 1);
					itr_bar->second[0] = dKeyMin;
					break;
				}
			}

			CChartSerie* pBarSeries = itr_bar_serie->second;
			for(std::vector<double>::iterator itr_bar_value = itr_bar->second.begin(); itr_bar_value != itr_bar->second.end(); ++itr_bar_value)
			{
				double dValue = *itr_bar_value;
				pBarSeries->AddPoint(dValue, (-i) + dIndexOffset);

				if (E_SELECTED_KEY == itr_bar->first)
				{
					bScroll = true;
					dSelected = dValue * 0.6;
				}

				if (bCheckMinMax)
				{
					
					if (dValue < dKeyMin)
					{
						dKeyMin = dValue;
					}

					if (dValue > dKeyMax)
					{
						dKeyMax = dValue;
					}
				}
			}
		}

		i += 1;
	}

	if (dWidthMax > 10)
	{
		dWidthMax = 10;
	}
	double dwWidthMinValue = m_dScrollBarHideProtection_*dWidthMax;
	m_ChartCtrlForController.GetAxis(CChartCtrl::BottomAxis)->SetMinMax(dwWidthMinValue, dWidthMax);

	if (m_dLastControllerNum_ <= m_dLastControllerMinHeight_)
	{
		m_ChartCtrlForController.GetAxis(CChartCtrl::LeftAxis)->SetMinMax(m_dLastControllerMinHeight_-1, -1);
		m_mapBarTypesForController_[E_TOTAL]->AddPoint(dwWidthMinValue, m_dLastControllerNum_ - 3.5);
	}
	else if(dWidthMax > 0)
	{
		m_ChartCtrlForController.GetAxis(CChartCtrl::LeftAxis)->SetMinMax(m_dLastControllerMinHeight_, 0);
		m_mapBarTypesForController_[E_TOTAL]->AddPoint(dwWidthMinValue, m_dLastControllerNum_);
	}

	if (bScroll)
	{
		if (m_nLastControllerHPoint < 0)
		{
			CChartScrollBar* pHScrollBar = m_ChartCtrlForController.GetBottomAxis()->GetScrollBar();
			if (pHScrollBar)
			{
				m_ChartCtrlForController.UpdateWithHScrollPos(SB_THUMBPOSITION, 0, pHScrollBar);
				m_ChartCtrlForController.UpdateWithHScrollPos(SB_THUMBPOSITION, (int)dSelected, pHScrollBar);
			}
			RememberLastControllerScrollPos();
		}
		else
		{
			SetLastControllerScrollPos();
		}
	}
	m_ChartCtrlForController.RefreshCtrl();
	RememberLastControllerScrollPos();

	// Move 컨트롤러만이 워크벤치와 오브젝트 리스트, 컷씬 에디터 모두에서 선택가능하다
	// 다른 컨트롤러는 컷씬 에디터에서만 선택가능하므로 그곳에서 처리하고 Move만 여기서 처리
	if (m_pCutSceneWrapper->GetContextMenuIdForController(m_nCurrentSelected) == coretoolcutscene::E_MOVE_CTRL)
	{
		m_ctrlControllerCmd.SetToMoveController();
	}
}

void CDlgCutSceneEditor::AddBarGraphValue( int nIdx, E_BAR_TYPE _eBarType, double _dValue1, double _dValue2)
{
	STrackBarInfo& info = m_vControllerBar_[nIdx];
	if (_eBarType == E_INTERVAL)
	{
		_ASSERT(info.m_mapBarValues[E_INTERVAL_START].empty());
		_ASSERT(info.m_mapBarValues[E_INTERVAL_END].empty());

		info.m_mapBarValues[E_INTERVAL_START].push_back(_dValue1);
		info.m_mapBarValues[E_INTERVAL_END].push_back(_dValue2);
	}
	else if (_eBarType == E_INTERVAL_KEY || _eBarType == E_WARP_KEY || _eBarType == E_SELECTED_KEY)
	{
		info.m_mapBarValues[_eBarType].push_back(_dValue1);
		if (_eBarType == E_SELECTED_KEY)
		{
			m_nCurrentSelected = nIdx;
			m_fCurrentSelectedValue = (float)_dValue1;
		}
	}
}

void CDlgCutSceneEditor::OnCutsceneCamobjAddMoveController()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddMoveController(this);
}

void CDlgCutSceneEditor::OnCtrlGeneralDelete()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->RemoveCutSceneController(m_nCurrentSelected, this);
	m_nCurrentSelected = -1;
	m_ctrlControllerCmd.SetToEmpty();
}

void CDlgCutSceneEditor::OnCutsceneMovectrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->RemoveMoveControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneMovectrlAddkey()
{
	using namespace rs3;

	char bufPos[256] = {0, };
	RVector3 vPos = m_pControlMediator_->GetToolCamera()->GetPosition();
	_stprintf(bufPos, "%.3f, %.3f, %.3f", vPos.x, vPos.y, vPos.z);

	char bufDir[256] = {0, };
	RVector3 vDir = m_pControlMediator_->GetToolCamera()->GetDirection();
	_stprintf(bufDir, "%.3f, %.3f, %.3f", vDir.x, vDir.y, vDir.z);

	//TODO
	RVector3 vUp(RVector3::AXISZ);

	CDlgCutSceneMoveKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneMoveKey>(m_fCurrentSelectedValue, bufPos, bufDir);
	if (pDlg)
	{
		_stscanf(pDlg->m_strPos,"%f, %f, %f",&vPos.x,&vPos.y,&vPos.z);
		_stscanf(pDlg->m_strDir,"%f, %f, %f",&vDir.x,&vDir.y,&vDir.z);
		m_pCutSceneWrapper->AddMoveControllerKey(m_nCurrentSelected, pDlg->m_fTime, vPos, vDir, vUp, (pDlg->m_bWarpLocation == TRUE), this);

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnInterpolationtypeLinear()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->ChangeMoveControllerToCatmullRom(m_nCurrentSelected, false);
}

void CDlgCutSceneEditor::OnInterpolationtypeCatmull()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->ChangeMoveControllerToCatmullRom(m_nCurrentSelected, true);
}

void CDlgCutSceneEditor::OnCutsceneMovectrlEditkey()
{
	using namespace rs3;

	float fTime;
	bool bWarp;
	RVector3 vPos, vDir, vUp;
	m_pCutSceneWrapper->GetMoveControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp);

	char bufPos[256] = {0, };
	sprintf_s(bufPos, "%.3f, %.3f, %.3f", vPos.x, vPos.y, vPos.z);

	char bufDir[256] = {0, };
	sprintf_s(bufDir, "%.3f, %.3f, %.3f", vDir.x, vDir.y, vDir.z);

	//TODO, Up편집

	CDlgCutSceneMoveKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneMoveKey>(fTime, bufPos, bufDir, bWarp);
	if (pDlg)
	{
		_stscanf(pDlg->m_strPos,"%f, %f, %f",&vPos.x,&vPos.y,&vPos.z);
		_stscanf(pDlg->m_strDir,"%f, %f, %f",&vDir.x,&vDir.y,&vDir.z);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeMoveControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, pDlg->m_fTime, vPos, vDir, vUp, (pDlg->m_bWarpLocation == TRUE), this, true);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneCamobjAddDOF()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddDOFController(this);
}

void CDlgCutSceneEditor::OnCutsceneDofctrlEditkey()
{
	using namespace rs3;

	float fTime, fFocusDist, fFocusRange, fCocSize;
	m_pCutSceneWrapper->GetDOFControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, fFocusDist, fFocusRange, fCocSize);

	CDlgCutSceneDOFKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneDOFKey>(fTime, fFocusDist, fFocusRange, fCocSize);
	if (pDlg)
	{
		RSceneManager* pMng = m_pControlMediator_->GetToolSceneManager();

		pMng->SetDOFEnable(true);
		pMng->SetDOFFocalDist(pDlg->m_fFocusDist);
		pMng->SetDOFFocalRange(pDlg->m_fFocusRange);
		pMng->SetDOFCOCSize(pDlg->m_fCocSize);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeDOFControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, pDlg->m_fTime, pDlg->m_fFocusDist, pDlg->m_fFocusRange, pDlg->m_fCocSize, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneDofctrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->RemoveDOFControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneDofctrlAddkey()
{
	using namespace rs3;

	RSceneManager* pMng = m_pControlMediator_->GetToolSceneManager();

	float fFocusDist = 1000.f;
	float fFocusRange = 1000.f;
	float fCocSize = 20.f;
	if (pMng && pMng->IsEnableDOF())
	{
		fFocusDist = pMng->GetDOFFocalDist();
		fFocusRange = pMng->GetDOFFocalRange();
		fCocSize = pMng->GetDOFCOCSize();
	}

	CDlgCutSceneDOFKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneDOFKey>(m_fCurrentSelectedValue, fFocusDist, fFocusRange, fCocSize);
	if (pDlg)
	{
		pMng->SetDOFEnable(true);
		pMng->SetDOFFocalDist(pDlg->m_fFocusDist);
		pMng->SetDOFFocalRange(pDlg->m_fFocusRange);
		pMng->SetDOFCOCSize(pDlg->m_fCocSize);
		m_pCutSceneWrapper->AddDOFControllerKey(m_nCurrentSelected, pDlg->m_fTime, pDlg->m_fFocusDist, pDlg->m_fFocusRange, pDlg->m_fCocSize, this);

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneMovectrlCollision()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->ToggleMoveControllerCollision(m_nCurrentSelected);
}

void CDlgCutSceneEditor::OnCutsceneActorAddmove()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddMoveController(this);
}

void CDlgCutSceneEditor::OnCutsceneActorAddani()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddAnimationController(this);
}

void CDlgCutSceneEditor::OnCutsceneActorAddtalent()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddTalentController(this);
}

void CDlgCutSceneEditor::OnCutsceneActorType()
{
	OnChangeActor(0, NULL, true);
}

void CDlgCutSceneEditor::OnCutsceneAnictrlEditkey()
{
	float fTime;
	std::string strAniName;
	m_pCutSceneWrapper->GetAniControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, strAniName);
	CDlgCutSceneAniKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneAniKey>(fTime, strAniName.c_str());
	if (pDlg && (pDlg->m_strAnimation.IsEmpty() == false) )
	{
		strAniName = (LPCTSTR)(pDlg->m_strAnimation);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeAniControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, pDlg->m_fTime, strAniName, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}

	delete pDlg;
}

void CDlgCutSceneEditor::OnCutsceneAnictrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->RemoveAniControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneAnictrlAddkey()
{
	CDlgCutSceneAniKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneAniKey>(m_fCurrentSelectedValue);
	if (pDlg && (pDlg->m_strAnimation.IsEmpty() == false) )
	{
		m_pCutSceneWrapper->AddAniControllerKey(m_nCurrentSelected, pDlg->m_fTime, (LPCTSTR)(pDlg->m_strAnimation), this);
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}

	delete pDlg;
}

void CDlgCutSceneEditor::OnCutsceneAnictrlDelete()
{
	OnCtrlGeneralDelete();
}

void CDlgCutSceneEditor::OnCutsceneTalentctrlEditkey()
{
	float fTime;
	int nID;
	m_pCutSceneWrapper->GetTalentControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, nID);
	CDlgCutSceneTalentKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneTalentKey>(fTime, nID);
	if (pDlg)
	{
		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeTalentControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, pDlg->m_fTime, pDlg->m_nID, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}

	delete pDlg;
}

void CDlgCutSceneEditor::OnCutsceneTalentctrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->RemoveTalentControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneTalentctrlAddkey()
{
	CDlgCutSceneTalentKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneTalentKey>(m_fCurrentSelectedValue);

	if (pDlg)
	{
		m_pCutSceneWrapper->AddTalentControllerKey(m_nCurrentSelected, pDlg->m_fTime, pDlg->m_nID, this);
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}

	delete pDlg;
}

void CDlgCutSceneEditor::OnCutsceneTalentctrlDelete()
{
	OnCtrlGeneralDelete();
}

void CDlgCutSceneEditor::OnCutsceneEventobjAddbgm()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddBGMController(this);
}

void CDlgCutSceneEditor::OnCutsceneEventobjAddcaption()
{
	CDlgCutSceneCaptionController* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneCaptionController>("굴림체", 12, FALSE, FALSE);
	if(pDlg)
	{
		m_pCutSceneWrapper->AddCaptionController(this, (LPCTSTR)(pDlg->m_strFont), pDlg->m_nSize, (bool)pDlg->m_bBold, (bool)pDlg->m_bItalic);
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneBgmctrlEditkey()
{
	float fTime;
	std::string strFileName;
	float fVolume;
	bool bIsLoop;
	m_pCutSceneWrapper->GetBGMControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, strFileName, fVolume, bIsLoop);

	CDlgCutSceneBGMKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneBGMKey>(fTime, strFileName.c_str(), fVolume, bIsLoop);
	if(pDlg && (pDlg->m_strFileName.IsEmpty() == false) )
	{
		strFileName = (LPCTSTR)(pDlg->m_strFileName);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeBGMControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, pDlg->m_fTime, strFileName, pDlg->m_fVolume, (pDlg->m_bLoop == TRUE), this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}

	delete pDlg;
}

void CDlgCutSceneEditor::OnCutsceneBgmctrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->RemoveBGMControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneBgmctrlAddkey()
{
	CDlgCutSceneBGMKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneBGMKey>(m_fCurrentSelectedValue, (const char*)NULL, 0.7f, true);
	if (pDlg && (pDlg->m_strFileName.IsEmpty() == false) )
	{
		m_pCutSceneWrapper->AddBGMControllerKey(m_nCurrentSelected, pDlg->m_fTime, (LPCTSTR)(pDlg->m_strFileName), pDlg->m_fVolume, (pDlg->m_bLoop == TRUE), this);
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}
	delete pDlg;
}

void CDlgCutSceneEditor::OnCutsceneBgmctrlDelete()
{
	OnCtrlGeneralDelete();
}

void CDlgCutSceneEditor::OnCutsceneCaptionctrlEditkey()
{
	CString cstrScript;
	std::string strScript;
	float fStartTime, fDurationTime;
	m_pCutSceneWrapper->GetCaptionControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fStartTime, fDurationTime, strScript);
	cstrScript = strScript.c_str();

	CDlgCutSceneCaptionKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneCaptionKey>(fStartTime, fDurationTime, cstrScript);
	if (pDlg && (pDlg->m_strScrip.IsEmpty() == false) )
	{
		strScript = (LPCTSTR)(pDlg->m_strScrip);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeCaptionControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, pDlg->m_fStartTime, pDlg->m_fDurationTime, strScript, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}
	delete pDlg;
}

void CDlgCutSceneEditor::OnCutsceneCaptionctrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->RemoveCaptionControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneCaptionctrlAddkey()
{
	CDlgCutSceneCaptionKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneCaptionKey>(m_fCurrentSelectedValue, 1.0f, CString(""));
	if (pDlg && (pDlg->m_strScrip.IsEmpty() == false) )
	{
		m_pCutSceneWrapper->AddCaptionControllerKey(m_nCurrentSelected, pDlg->m_fStartTime, pDlg->m_fDurationTime, (LPCTSTR)(pDlg->m_strScrip), this);
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	}
	delete pDlg;
}

void CDlgCutSceneEditor::OnCutsceneCaptionctrlDelete()
{
	OnCtrlGeneralDelete();
}

void CDlgCutSceneEditor::OnCutsceneMovectrlCamHere()
{
	float fTime;
	rs3::RVector3 vPos, vDir, vUp;
	bool bWarp;
	m_pCutSceneWrapper->GetMoveControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp);

	rs3::RCameraSceneNode* pCam = m_pControlMediator_->GetToolCamera();
	pCam->Set(vPos, vDir, vUp);
	pCam->UpdateTransform();
}

void CDlgCutSceneEditor::OnCutsceneLetterBoxctrlDelete()
{
	OnCtrlGeneralDelete();
}

void CDlgCutSceneEditor::OnCutsceneEventobjAddletterbox()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddLetterBoxController(this);
}

void CDlgCutSceneEditor::OnCutsceneCaptionctrlEdit()
{
	string strFont;
	int nSize;
	bool bBold;
	bool bItalic;
	m_pCutSceneWrapper->GetCaptionController(m_nCurrentSelected, strFont, nSize, bBold, bItalic);

	CDlgCutSceneCaptionController* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneCaptionController>(strFont.c_str(), nSize, bBold?TRUE:FALSE, bItalic?TRUE:FALSE);
	if (pDlg)
	{
		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeCaptionController(m_nCurrentSelected, (LPCTSTR)(pDlg->m_strFont), pDlg->m_nSize, pDlg->m_bBold?true:false, pDlg->m_bItalic?true:false, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);

		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneEventobjAddenv()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddEnvController(this);
}

void CDlgCutSceneEditor::OnCutsceneEnvctrlEditkey()
{
	float fTime;
	int nEnvTime, nEnvWeather;
	std::string strSetName;
	m_pCutSceneWrapper->GetEnvControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, nEnvTime, nEnvWeather, strSetName);

	CDlgCutSceneEnvKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneEnvKey>(m_pControlMediator_, fTime, nEnvTime, nEnvWeather, strSetName.c_str());
	if (pDlg)
	{
		pDlg->GetEnvKeyInfo(fTime, nEnvTime, nEnvWeather, strSetName);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeEnvControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, nEnvTime, nEnvWeather, strSetName, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneEnvctrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->RemoveEnvControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneEnvctrlAddkey()
{
	CDlgCutSceneEnvKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneEnvKey>(m_pControlMediator_, m_fCurrentSelectedValue, 0, 0, "default");
	if (pDlg)
	{
		float fTime;
		int nEnvTime, nEnvWeather;
		std::string strSetName;
		pDlg->GetEnvKeyInfo(fTime, nEnvTime, nEnvWeather, strSetName);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->AddEnvControllerKey(m_nCurrentSelected, fTime, nEnvTime, nEnvWeather, strSetName, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneEnvctrlDelete()
{
	OnCtrlGeneralDelete();
}

void CDlgCutSceneEditor::OnCutsceneEventobjAddEffect()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddEffectController(this);
}

void CDlgCutSceneEditor::OnCutsceneEffectctrlEditkey()
{
	float fTime;
	float fDurationTime;
	vec3 vPos;
	vec3 vDir;
	vec3 vUp;
	std::string strEffectName;
	std::string strEffectInfo;
	m_pCutSceneWrapper->GetEffectControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo);

	CDlgCutSceneEffectKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneEffectKey>(m_pControlMediator_, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo);
	if (pDlg)
	{
		pDlg->GetEffectData(fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeEffectControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneEffectctrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pCutSceneWrapper->RemoveEffectControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneEffectctrlAddkey()
{
	RVector3 vCameraPos = m_pControlMediator_->GetToolCamera()->GetPosition();
	RVector3 vCameraDir = m_pControlMediator_->GetToolCamera()->GetDirection();

	CDlgCutSceneEffectKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneEffectKey>(m_pControlMediator_, m_fCurrentSelectedValue, -1.0f, vCameraPos, vCameraDir, vec3::AXISZ, string(""), string(""));
	if (pDlg)
	{
		float fTime;
		float fDurationTime;
		vec3 vPos;
		vec3 vDir;
		vec3 vUp;
		std::string strEffectName;
		std::string strEffectInfo;
		pDlg->GetEffectData(fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->AddEffectControllerKey(m_nCurrentSelected, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneEffectctrlDelete()
{
	OnCtrlGeneralDelete();
}

void CDlgCutSceneEditor::OnCutsceneMovectrlAverageVel()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CDlgCutSceneMoveVelAverage* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneMoveVelAverage>(m_pCutSceneWrapper, m_nCurrentSelected);
	if (pDlg)
	{
		if (pDlg->m_nIntervalType == 0)
		{
			// 전체
			float fAverVel = pDlg->m_fAverVel;

			std::vector< rs3::RVector3 > vLocations, dirs, ups;
			std::vector< float > vTimes;

			std::deque<bool> warps;

			m_pCutSceneWrapper->GetCurrentCutSceneObjectMoveKeyInfo(m_nCurrentSelected, vLocations, vTimes, &dirs, &ups, &warps);

			int nSize = (int)vTimes.size();

			float fBeforeTime = vTimes[0];
			for (int i = 1; i < nSize; ++i)
			{
				// 거리
				float fLength = (vLocations[i] - vLocations[i-1]).Length() * 0.01f;

				// 시간
				float fTime = fBeforeTime + fLength / fAverVel;
				fBeforeTime = fTime;

				RememberLastControllerScrollPos();
				m_pCutSceneWrapper->ChangeMoveControllerKey(m_nCurrentSelected, vTimes[i], fTime, vLocations[i], dirs[i], ups[i], warps[i], this, false);
				SetLastControllerScrollPos();
			}
		}
		else
		{
			// 부분
		}
		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnChangeActor( int nNpcId, const char* _pSzActorName, bool bPopDialog )
{
	std::string strActorType, strActorName;
	m_pCutSceneWrapper->GetActorTypeAndName(strActorType, strActorName);
	CDlgCutSceneActorType* pDlg = NULL;

	if (bPopDialog)
	{
		pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneActorType>(m_pControlMediator_, strActorType.c_str(), strActorName.c_str());
		if (pDlg)
		{
			if (pDlg->m_nActorType == 0)
			{
				strActorType = "player";
				strActorName = "my_player";
			}
			else
			{
				CString strFormat;
				strFormat.Format("npc_%d", pDlg->m_nNPCID);

				strActorType = LPCTSTR(strFormat);
				strActorName = LPCTSTR(pDlg->m_strNpcName);
			}

			m_pCutSceneWrapper->SetActorTypeAndName( strActorType.c_str(), strActorName.c_str() );
			m_pCutSceneWrapper->ResetObjectInfo( this );
			m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->ChangeCurrentObjectListName(strActorName.c_str());

			m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		}
	}
	else
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		pDlg = new CDlgCutSceneActorType(m_pControlMediator_, strActorType.c_str(), strActorName.c_str());

		if (_pSzActorName == NULL)
		{
			CString strFormat;
			strFormat.Format("npc_%d", nNpcId);

			strActorType = LPCTSTR(strFormat);
			strActorName = LPCTSTR(pDlg->m_strNpcName);
		}
		else if (nNpcId == -1)
		{
			CString strFormat;
			strFormat.Format("npc_%d", pDlg->m_nNPCID);

			strActorType = LPCTSTR(strFormat);
			strActorName = _pSzActorName;
		}
	}

	m_pCutSceneWrapper->SetActorTypeAndName( strActorType.c_str(), strActorName.c_str() );
	m_pCutSceneWrapper->ResetObjectInfo( this );
	m_pControlMediator_->GetViewPtr<CRollupCutSceneObjectList>()->ChangeCurrentObjectListName(strActorName.c_str());

	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	delete pDlg;
}

void CDlgCutSceneEditor::ClearSelectedController()
{
	int nLastHPoint = m_nLastControllerHPoint;
	int nLastVPoint = m_nLastControllerVPoint;

	int nTotalControlBars = (int)m_vControllerBar_.size();
	for (int i = 0; i < nTotalControlBars; ++i)
	{
		STrackBarInfo& info = m_vControllerBar_[i];
		info.m_mapBarValues[E_SELECTED_KEY].clear();
	}

	m_nCurrentSelected = -1;
	m_mapBarTypesForController_[E_SELECTED_KEY]->ClearSerie();
	RefreshController();
	m_ctrlControllerCmd.SetToEmpty();

	m_nLastControllerHPoint = nLastHPoint;
	m_nLastControllerVPoint = nLastVPoint;
	SetLastControllerScrollPos();
}

#pragma warning( pop )

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToCameraObject()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pCamObjectProp = new CMFCPropertyGridProperty( _T("카메라 오브젝트") );

		// 2. 컨트롤러 추가 프로퍼티
		CMFCPropertyGridProperty* pAddControllerProp = new CMFCPropertyGridProperty( _T("컨트롤러 추가") );
		pCamObjectProp->AddSubItem( pAddControllerProp );

			// 3. 컨트롤러 타입
			m_varCmdCombo[m_nCurrentCombo] = _T("Add Controller ...");
			CMFCPropertyGridProperty* pControllerTypeProp = new CMFCPropertyGridProperty( _T("타입"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
			pControllerTypeProp->AllowEdit(FALSE);
			pControllerTypeProp->AddOption( _T("Move Controller")  );
			pControllerTypeProp->AddOption( _T("DOF Controller")  );
			pAddControllerProp->AddSubItem( pControllerTypeProp );
			++m_nCurrentCombo;

	AddProperty(pCamObjectProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToActorObject()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pActorObjectProp = new CMFCPropertyGridProperty( _T("액터 오브젝트") );

		// 2. 액터정보
		CMFCPropertyGridProperty* pActorInfoProp = new CMFCPropertyGridProperty( _T("액터 정보") );
		std::string strActorType, strActorName;
		m_pOwner->m_pCutSceneWrapper->GetActorTypeAndName(strActorType, strActorName);
		if (strActorType == "player")
		{
			m_varInfo[m_nCurrentInfo] = _T("my_player");
			CMFCPropertyGridProperty* pActorNameProp = new CMFCPropertyGridProperty( _T("액터 이름"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pActorNameProp->Enable(FALSE);
			pActorInfoProp->AddSubItem(pActorNameProp);
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = _T("Not Avaliable");
			CMFCPropertyGridProperty* pActorIdProp = new CMFCPropertyGridProperty( _T("NPC ID"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pActorIdProp->Enable(FALSE);
			pActorInfoProp->AddSubItem(pActorIdProp);
			++m_nCurrentInfo;
		}
		else
		{
			m_varInfo[m_nCurrentInfo] = strActorName.c_str();
			CMFCPropertyGridProperty* pActorNameProp = new CMFCPropertyGridProperty( _T("액터 이름"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pActorInfoProp->AddSubItem(pActorNameProp);
			++m_nCurrentInfo;

			//npc_ : 4
			m_varInfo[m_nCurrentInfo] = atoi( strActorType.substr(4).c_str() );
			CMFCPropertyGridProperty* pActorIdProp = new CMFCPropertyGridProperty( _T("NPC ID"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pActorInfoProp->AddSubItem(pActorIdProp);
			++m_nCurrentInfo;
		}
		pActorObjectProp->AddSubItem( pActorInfoProp );

	AddProperty(pActorObjectProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToEventObject()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pEventObjectProp = new CMFCPropertyGridProperty( _T("이벤트 오브젝트") );

		// 2. 컨트롤러 추가 프로퍼티
		CMFCPropertyGridProperty* pAddControllerProp = new CMFCPropertyGridProperty( _T("컨트롤러 추가") );
		pEventObjectProp->AddSubItem( pAddControllerProp );

			// 3. 컨트롤러 타입
			m_varCmdCombo[m_nCurrentCombo] = _T("Add Controller ...");
			CMFCPropertyGridProperty* pControllerTypeProp = new CMFCPropertyGridProperty( _T("타입"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
			pControllerTypeProp->AllowEdit(FALSE);
			pControllerTypeProp->AddOption( _T("BGM Controller")  );
			pControllerTypeProp->AddOption( _T("Caption Controller")  );
			pControllerTypeProp->AddOption( _T("LetterBox Controller")  );
			pControllerTypeProp->AddOption( _T("Env Controller")  );
			pControllerTypeProp->AddOption( _T("Effect Controller")  );
			pControllerTypeProp->AddOption( _T("Fade Controller")  );
			pAddControllerProp->AddSubItem( pControllerTypeProp );
			++m_nCurrentCombo;

	AddProperty(pEventObjectProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToMoveController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pMoveControllerProp = new CMFCPropertyGridProperty( _T("무브 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pMoveControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;

		// 2-2 Using Collision
		m_pOwner->m_pCutSceneWrapper->ToggleMoveControllerCollision(m_pOwner->m_nCurrentSelected);
		bool bCurrentCollision = (m_pOwner->m_pCutSceneWrapper->ToggleMoveControllerCollision(m_pOwner->m_nCurrentSelected) == 1? true : false);
		m_varCmdCombo[m_nCurrentCombo] = variant_t(bCurrentCollision);
		CMFCPropertyGridProperty* pMoveUsingCollision = new CMFCPropertyGridProperty( _T("Using Collision"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pMoveControllerProp->AddSubItem(pMoveUsingCollision);
		++m_nCurrentCombo;

		// 2-3 Interpolation type
		bool bPrevCatmull = false;
		m_pOwner->m_pCutSceneWrapper->ChangeMoveControllerToCatmullRom(m_pOwner->m_nCurrentSelected, false, &bPrevCatmull);
		CString strCatmulOption;
		if (bPrevCatmull)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeMoveControllerToCatmullRom(m_pOwner->m_nCurrentSelected, true);
			m_varCmdCombo[m_nCurrentCombo] = _T("Catmull");
			strCatmulOption = _T("Linear");
		}
		else
		{
			m_varCmdCombo[m_nCurrentCombo] = _T("Linear");
			strCatmulOption = _T("Catmull");
		}
		CMFCPropertyGridProperty* pMoveInterpolationType = new CMFCPropertyGridProperty( _T("Interpolation"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pMoveControllerProp->AddSubItem(pMoveInterpolationType);
		pMoveInterpolationType->AddOption( strCatmulOption.GetString() );
		pMoveInterpolationType->AllowEdit(FALSE);
		++m_nCurrentCombo;

		// 2-4. Move Controller 로케이션 관련
		CMFCPropertyGridProperty* pMoveLocationKey = new CMFCPropertyGridProperty( _T("무브 컨트롤러 로케이션 키") );
		pMoveControllerProp->AddSubItem(pMoveLocationKey);
		m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
		CMFCPropertyGridProperty* pMoveCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pMoveCmdSpecific->AllowEdit(FALSE);
		pMoveLocationKey->AddSubItem(pMoveCmdSpecific);
		++m_nCurrentCombo;

		float fTime;
		rs3::RVector3 vPos, vDir, vUp;
		bool bWarp;
		if (m_pOwner->m_pCutSceneWrapper->GetMoveControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp))
		{
			pMoveCmdSpecific->AddOption( _T("Delete Location Key")  );

			// 3-1
			m_varInfo[m_nCurrentInfo] = fTime;
			CMFCPropertyGridProperty* pMoveTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMoveLocationKey->AddSubItem(pMoveTime);
			++m_nCurrentInfo;

			// 3-2
			m_varCmdCombo[m_nCurrentCombo] = variant_t(bWarp);
			CMFCPropertyGridProperty* pMoveWarp = new CMFCPropertyGridProperty( _T("Warp"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
			pMoveLocationKey->AddSubItem(pMoveWarp);
			++m_nCurrentCombo;

			// 3-3-1
			CMFCPropertyGridProperty* pMovePos = new CMFCPropertyGridProperty( _T("Position") );
			pMoveLocationKey->AddSubItem(pMovePos);

			m_varInfo[m_nCurrentInfo] = vPos.x;
			CMFCPropertyGridProperty* pMovePosX = new CMFCPropertyGridProperty( _T("X"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMovePos->AddSubItem(pMovePosX);
			m_LocProps[POS_X] = pMovePosX;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vPos.y;
			CMFCPropertyGridProperty* pMovePosY = new CMFCPropertyGridProperty( _T("Y"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMovePos->AddSubItem(pMovePosY);
			m_LocProps[POS_Y] = pMovePosY;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vPos.z;
			CMFCPropertyGridProperty* pMovePosZ = new CMFCPropertyGridProperty( _T("Z"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMovePos->AddSubItem(pMovePosZ);
			m_LocProps[POS_Z] = pMovePosZ;
			++m_nCurrentInfo;

			// 3-3-2
			CMFCPropertyGridProperty* pMoveDir = new CMFCPropertyGridProperty( _T("Direction") );
			pMoveLocationKey->AddSubItem(pMoveDir);

			m_varInfo[m_nCurrentInfo] = vDir.x;
			CMFCPropertyGridProperty* pMoveDirX = new CMFCPropertyGridProperty( _T("X"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMoveDir->AddSubItem(pMoveDirX);
			m_LocProps[DIR_X] = pMoveDirX;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vDir.y;
			CMFCPropertyGridProperty* pMoveDirY = new CMFCPropertyGridProperty( _T("Y"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMoveDir->AddSubItem(pMoveDirY);
			m_LocProps[DIR_Y] = pMoveDirY;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vDir.z;
			CMFCPropertyGridProperty* pMoveDirZ = new CMFCPropertyGridProperty( _T("Z"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMoveDir->AddSubItem(pMoveDirZ);
			m_LocProps[DIR_Z] = pMoveDirZ;
			++m_nCurrentInfo;

			// 3-3-3
			CMFCPropertyGridProperty* pMoveUp = new CMFCPropertyGridProperty( _T("Up") );
			pMoveLocationKey->AddSubItem(pMoveUp);

			m_varInfo[m_nCurrentInfo] = vUp.x;
			CMFCPropertyGridProperty* pMoveUpX = new CMFCPropertyGridProperty( _T("X"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMoveUp->AddSubItem(pMoveUpX);
			m_LocProps[UP_X] = pMoveUpX;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vUp.y;
			CMFCPropertyGridProperty* pMoveUpY = new CMFCPropertyGridProperty( _T("Y"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMoveUp->AddSubItem(pMoveUpY);
			m_LocProps[UP_Y] = pMoveUpY;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vUp.z;
			CMFCPropertyGridProperty* pMoveUpZ = new CMFCPropertyGridProperty( _T("Z"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pMoveUp->AddSubItem(pMoveUpZ);
			m_LocProps[UP_Z] = pMoveUpZ;
			++m_nCurrentInfo;
		}
		else
		{
			pMoveCmdSpecific->AddOption( _T("Add Location Key")  );
		}

	AddProperty(pMoveControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToGeneralController(CMFCPropertyGridProperty* _pControllerProp, variant_t& _var)
{
	CMFCPropertyGridProperty* pGeneral = new CMFCPropertyGridProperty( _T("일반 커맨드"));

	_var = _T("명령선택 ...");
	CMFCPropertyGridProperty* pCmd = new CMFCPropertyGridProperty( _T("커맨드"), _var, _T(""), (DWORD_PTR)&_var );
	pCmd->AllowEdit(FALSE);
	pCmd->AddOption( _T("Delete Controller")  );

	pGeneral->AddSubItem(pCmd);
	_pControllerProp->AddSubItem(pGeneral);
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::UpdateLocation()
{
	LOGIC_OBJECTLIST* pList = m_pOwner->m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();
	if (1 != pList->size())
		return;

	RVector3 vPos, vDir, vUp;
	CLogicObject* pObject = pList->front();
	pObject->GetPosition(vPos);
	pObject->GetDirection(vDir);
	pObject->GetUp(vUp);

	if (m_LocProps[POS_X])
	{
		m_LocProps[POS_X]->SetValue( variant_t(vPos.x) );
		m_LocProps[POS_X]->Redraw();
	}
	if (m_LocProps[POS_Y])
	{
		m_LocProps[POS_Y]->SetValue( variant_t(vPos.y) );
		m_LocProps[POS_Y]->Redraw();
	}
	if (m_LocProps[POS_Z])
	{
		m_LocProps[POS_Z]->SetValue( variant_t(vPos.z) );
		m_LocProps[POS_Z]->Redraw();
	}

	if (m_LocProps[DIR_X])
	{
		m_LocProps[DIR_X]->SetValue( variant_t(vDir.x) );
		m_LocProps[DIR_X]->Redraw();
	}
	if (m_LocProps[DIR_Y])
	{
		m_LocProps[DIR_Y]->SetValue( variant_t(vDir.y) );
		m_LocProps[DIR_Y]->Redraw();
	}
	if (m_LocProps[DIR_Z])
	{
		m_LocProps[DIR_Z]->SetValue( variant_t(vDir.z) );
		m_LocProps[DIR_Z]->Redraw();
	}

	if (m_LocProps[UP_X])
	{
		m_LocProps[UP_X]->SetValue( variant_t(vUp.x) );
		m_LocProps[UP_X]->Redraw();
	}
	if (m_LocProps[UP_Y])
	{
		m_LocProps[UP_Y]->SetValue( variant_t(vUp.y) );
		m_LocProps[UP_Y]->Redraw();
	}
	if (m_LocProps[UP_Z])
	{
		m_LocProps[UP_Z]->SetValue( variant_t(vUp.z) );
		m_LocProps[UP_Z]->Redraw();
	}
}

bool CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::OnChangedProcWithPropValue( const CString& _strValue, CMFCPropertyGridProperty* pProp ) const
{
	bool bStopProc = false;

	// 콤보선택으로 명령을 내리는 Controller 관련 명령은 Value만으로 특정 가능
	if (_strValue == "Move Controller")
	{
		m_pOwner->OnCutsceneCamobjAddMoveController();
		pProp->SetValue(_T("Add Controller ..."));
		bStopProc = true;
	}
	else if (_strValue == "DOF Controller")
	{
		m_pOwner->OnCutsceneCamobjAddDOF();
		pProp->SetValue(_T("Add Controller ..."));
		bStopProc = true;
	}
	else if (_strValue == "BGM Controller")
	{
		m_pOwner->OnCutsceneEventobjAddbgm();
		pProp->SetValue(_T("Add Controller ..."));
		bStopProc = true;
	}
	else if (_strValue == "Caption Controller")
	{
		m_pOwner->OnCutsceneEventobjAddcaption();
		pProp->SetValue(_T("Add Controller ..."));
		bStopProc = true;
	}
	else if (_strValue == "LetterBox Controller")
	{
		m_pOwner->OnCutsceneEventobjAddletterbox();
		pProp->SetValue(_T("Add Controller ..."));
		bStopProc = true;
	}
	else if (_strValue == "Env Controller")
	{
		m_pOwner->OnCutsceneEventobjAddenv();
		pProp->SetValue(_T("Add Controller ..."));
		bStopProc = true;
	}
	else if (_strValue == "Effect Controller")
	{
		m_pOwner->OnCutsceneEventobjAddEffect();
		pProp->SetValue(_T("Add Controller ..."));
		bStopProc = true;
	}
	else if (_strValue == "Delete Controller")
	{
		m_pOwner->OnCtrlGeneralDelete();
		bStopProc = true;
	}
	else if (_strValue == "Add Location Key")
	{
		m_pOwner->OnCutsceneMovectrlAddkey();
		bStopProc = true;
	}
	else if (_strValue == "Delete Location Key")
	{
		m_pOwner->OnCutsceneMovectrlDelkey();
		bStopProc = true;
	}
	else if (_strValue == "Add DOF Key")
	{
		m_pOwner->OnCutsceneDofctrlAddkey();
		bStopProc = true;
	}
	else if (_strValue == "Delete DOF Key")
	{
		m_pOwner->OnCutsceneDofctrlDelkey();
		bStopProc = true;
	}
	else if (_strValue == "Add Caption Key")
	{
		m_pOwner->OnCutsceneCaptionctrlAddkey();
		bStopProc = true;
	}
	else if (_strValue == "Delete Caption Key")
	{
		m_pOwner->OnCutsceneCaptionctrlDelkey();
		bStopProc = true;
	}
	else if (_strValue == "Add BGM Key")
	{
		m_pOwner->OnCutsceneBgmctrlAddkey();
		bStopProc = true;
	}
	else if (_strValue == "Delete BGM Key")
	{
		m_pOwner->OnCutsceneBgmctrlDelkey();
		bStopProc = true;
	}
	else if (_strValue == "Add Env Key")
	{
		m_pOwner->OnCutsceneEnvctrlAddkey();
		bStopProc = true;
	}
	else if (_strValue == "Delete Env Key")
	{
		m_pOwner->OnCutsceneEnvctrlDelkey();
		bStopProc = true;
	}
	else if (_strValue == "Add Ani Key")
	{
		m_pOwner->OnCutsceneAnictrlAddkey();
		bStopProc = true;
	}
	else if (_strValue == "Delete Ani Key")
	{
		m_pOwner->OnCutsceneAnictrlDelkey();
		bStopProc = true;
	}
	else if (_strValue == "Add Effect Key")
	{
		m_pOwner->OnCutsceneEffectctrlAddkey();
		bStopProc = true;
	}
	else if (_strValue == "Delete Effect Key")
	{
		m_pOwner->OnCutsceneEffectctrlDelkey();
		bStopProc = true;
	}
	else if (_strValue == "Fade Controller")
	{
		m_pOwner->OnCutsceneEventobjAddFade();
		pProp->SetValue(_T("Add Controller ..."));
		bStopProc = true;
	}
	else if (_strValue == "Add Fade Key")
	{
		m_pOwner->OnCutsceneFadectrlAddkey();
		bStopProc = true;
	}
	else if (_strValue == "Delete Fade Key")
	{
		m_pOwner->OnCutsceneFadectrlDelkey();
		bStopProc = true;
	}

	return bStopProc;
}

bool CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::OnChangedProcWithPropName( const CString& _strPropName, const CString& _strValue, CMFCPropertyGridProperty* pProp ) const
{
	bool bStopProc = false;

	if (_strPropName == "액터 이름")
	{
		m_pOwner->OnChangeActor(-1, _strValue.GetString(), false);
		bStopProc = true;
	}
	else if (_strPropName == "NPC ID")
	{
		int nNpcId = atoi(_strValue.GetString());
		m_pOwner->OnChangeActor(nNpcId, NULL, false);
		bStopProc = true;
	}

	return bStopProc;
}

bool CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::OnChangedProcWithPropParent( const CString& _strParentName, const CString& _strPropName, const CString& _strValue, CMFCPropertyGridProperty* pProp ) const
{
	bool bStopProc = false;

	if (_strParentName == "무브 컨트롤러")
	{
		if (_strPropName == "Using Collision")
		{
			m_pOwner->m_pCutSceneWrapper->ToggleMoveControllerCollision(m_pOwner->m_nCurrentSelected);
			bStopProc = true;
		}
		else if (_strPropName == "Interpolation")
		{
			if (_strValue == "Catmull")
			{
				m_pOwner->m_pCutSceneWrapper->ChangeMoveControllerToCatmullRom(m_pOwner->m_nCurrentSelected, true);
				pProp->RemoveAllOptions();
				pProp->AddOption(_T("Linear"));
				pProp->SetValue(_T("Catmull"));
				m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			}
			else
			{
				m_pOwner->m_pCutSceneWrapper->ChangeMoveControllerToCatmullRom(m_pOwner->m_nCurrentSelected, false);
				pProp->RemoveAllOptions();
				pProp->AddOption(_T("Catmull"));
				pProp->SetValue(_T("Linear"));
				m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			}
			bStopProc = true;
		}
	}
	else if (_strParentName == "무브 컨트롤러 로케이션 키")
	{
		if (_strPropName == "시각")
		{
			float fTime;
			bool bWarp;
			RVector3 vPos, vDir, vUp;
			m_pOwner->m_pCutSceneWrapper->GetMoveControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp);

			fTime = (float)atof(_strValue.GetString());

			m_pOwner->RememberLastControllerScrollPos();
			m_pOwner->m_pCutSceneWrapper->ChangeMoveControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp, m_pOwner, true);
			m_pOwner->SetLastControllerScrollPos();
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);

			bStopProc = true;
		}
		else if (_strPropName)
		{
			float fTime;
			bool bWarp;
			RVector3 vPos, vDir, vUp;
			m_pOwner->m_pCutSceneWrapper->GetMoveControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp);

			bWarp = (atoi(_strValue.GetString()) == 0)? false : true;
			m_pOwner->RememberLastControllerScrollPos();
			m_pOwner->m_pCutSceneWrapper->ChangeMoveControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp, m_pOwner, true);
			m_pOwner->SetLastControllerScrollPos();
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);

			bStopProc = true;
		}
	}
	else if (_strParentName == "DOF 컨트롤러 키 세팅")
	{
		float fTime, fFocusDist, fFocusRange, fCocSize;
		bool bRet = m_pOwner->m_pCutSceneWrapper->GetDOFControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fFocusDist, fFocusRange, fCocSize);

		float fValue = (float)atof(_strValue.GetString());
		if (_strPropName == "시각")
		{
			fTime = fValue;
		}
		else if (_strPropName == "초점거리")
		{
			fFocusDist = fValue;
		}
		else if (_strPropName == "초점범위")
		{
			fFocusRange = fValue;
		}
		else if (_strPropName == "착란원크기")
		{
			fCocSize = fValue;
		}

		if (bRet)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeDOFControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fFocusDist, fFocusRange, fCocSize, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			m_pOwner->m_ctrlControllerCmd.SetToDofController();
			bStopProc = true;
		}
	}
	else if (_strParentName == "캡션 컨트롤러")
	{
		CMFCPropertyGridFontProperty* pFont = dynamic_cast<CMFCPropertyGridFontProperty*>(pProp);
		if (pFont)
		{
			LPLOGFONT font = pFont->GetLogFont();
			int nFontSize =(int) Roundf( float(abs((font->lfHeight / 96.0) * 72.0)) );
			CString strFontName = _strValue;
			int nPos = strFontName.Find('(');
			strFontName.Delete(nPos, strFontName.GetLength()-nPos );
			m_pOwner->m_pCutSceneWrapper->ChangeCaptionController(m_pOwner->m_nCurrentSelected, strFontName.GetString(), nFontSize, (font->lfWeight == FW_NORMAL)?false:true, font->lfItalic?true:false, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);

			bool bStopProc = true;
		}
	}
	else if (_strParentName == "캡션 컨트롤러 키 세팅")
	{
		float fTime, fDuration;
		std::string strScript;
		bool bRet = m_pOwner->m_pCutSceneWrapper->GetCaptionControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDuration, strScript);

		if (_strPropName == "시각")
		{
			fTime = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "지속시간")
		{
			fDuration = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "내용")
		{
			strScript = _strValue.GetString();
		}

		if (bRet)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeCaptionControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDuration, strScript, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			m_pOwner->m_ctrlControllerCmd.SetToCaptionController();
			bStopProc = true;
		}
	}
	else if (_strParentName == "BGM 컨트롤러 키 세팅")
	{
		float fTime, fVolume;
		std::string strFileName;
		bool bLoop;
		bool bRet = m_pOwner->m_pCutSceneWrapper->GetBGMControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strFileName, fVolume, bLoop);

		if (_strPropName == "시각")
		{
			fTime = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "파일이름")
		{
			strFileName = _strValue.GetString();
		}
		else if (_strPropName == "볼륨")
		{
			fVolume = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "Loop")
		{
			bLoop = (_strValue == "0")? false : true;
		}

		if (bRet)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeBGMControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strFileName, fVolume, bLoop, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			m_pOwner->m_ctrlControllerCmd.SetToBGMController();
			bStopProc = true;
		}
	}
	else if (_strParentName == "Env 컨트롤러 키 세팅")
	{
		float fTime;
		int nEnvTime, nEnvWeather;
		std::string strSetName;
		bool bRet = m_pOwner->m_pCutSceneWrapper->GetEnvControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nEnvTime, nEnvWeather, strSetName);

		if (_strPropName == "시각")
		{
			fTime = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "EnvSet")
		{
			strSetName = _strValue.GetString();
		}
		else if (_strPropName == "EnvTime")
		{
			nEnvTime = (int)REnvironment::StringToETime(_strValue.GetString());
		}
		else if (_strPropName == "EnvWeather")
		{
			nEnvWeather = (int)REnvironment::StringToEWeather(_strValue.GetString());
		}

		if (bRet)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeEnvControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nEnvTime, nEnvWeather, strSetName, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			m_pOwner->m_ctrlControllerCmd.SetToEnvController();
			bStopProc = true;
		}
	}
	else if (_strParentName == "Ani 컨트롤러 키 세팅")
	{
		float fTime;
		std::string strAniName;
		bool bRet = m_pOwner->m_pCutSceneWrapper->GetAniControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strAniName);

		if (_strPropName == "시각")
		{
			fTime = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "이름")
		{
			strAniName = _strValue.GetString();
		}

		if (bRet)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeAniControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strAniName, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			m_pOwner->m_ctrlControllerCmd.SetToAniController();
			bStopProc = true;
		}
	}
	else if (_strParentName == "Talent 컨트롤러 키 세팅")
	{
		float fTime;
		int nID;
		bool bRet = m_pOwner->m_pCutSceneWrapper->GetTalentControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nID);

		if (_strPropName == "시각")
		{
			fTime = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "I D")
		{
			nID = (int)atoi(_strValue.GetString());
		}

		if (bRet)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeTalentControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nID, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			m_pOwner->m_ctrlControllerCmd.SetToTalentController();
			bStopProc = true;
		}
	}
	else if (_strParentName == "Effect 컨트롤러 키 세팅")
	{
		float fTime;
		float fDurationTime;
		vec3 vPos;
		vec3 vDir;
		vec3 vUp;
		std::string strEffectName;
		std::string strEffectInfo;
		bool bRet = m_pOwner->m_pCutSceneWrapper->GetEffectControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo);

		if (_strPropName == "시각")
		{
			fTime = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "지속 시간")
		{
			fDurationTime = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "이펙트 이름")
		{
			strEffectName = _strValue.GetString();
		}

		if (bRet)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeEffectControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			m_pOwner->m_ctrlControllerCmd.SetToEffectController();
			bStopProc = true;
		}
	}
	else if (_strParentName == "페이드 컨트롤러 키 세팅")
	{
		float fTime, fAlpha;
		bool bRet = m_pOwner->m_pCutSceneWrapper->GetFadeControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fAlpha);

		if (_strPropName == "시각")
		{
			fTime = (float)atof(_strValue.GetString());
		}
		else if (_strPropName == "알파")
		{
			fAlpha = (float)atof(_strValue.GetString());
		}

		if (bRet)
		{
			m_pOwner->m_pCutSceneWrapper->ChangeFadeControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fAlpha, m_pOwner);
			m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
			m_pOwner->m_ctrlControllerCmd.SetToFadeController();
			bStopProc = true;
		}
	}

	return bStopProc;
}

bool CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::OnChangedProcWithPropGrandParent( const CString& _strGrandParentName, const CString& _strParentName, const CString& _strPropName, const CString& _strValue, CMFCPropertyGridProperty* pProp ) const
{
	bool bStopProc = false;

	if (_strParentName == "Position")
	{
		// location index
		LOC_IDX loc_idx = LOC_IDX_NUM;
		if (_strPropName == "X")
		{
			loc_idx = POS_X;
		}
		else if (_strPropName == "Y")
		{
			loc_idx = POS_Y;
		}
		else if (_strPropName == "Z")
		{
			loc_idx = POS_Z;
		}

		// value
		float fValue = (float)atof(_strValue.GetString());

		// move controller
		if (_strGrandParentName == "무브 컨트롤러 로케이션 키")
		{
			ChangeMoveLocation(loc_idx, fValue);
			bStopProc = true;
		}
		else if (_strGrandParentName == "Effect 컨트롤러 키 세팅")
		{
			ChangeEffectLocation(loc_idx, fValue);
			bStopProc = true;
		}
	}
	else if (_strParentName == "Direction")
	{
		// location index
		LOC_IDX loc_idx = LOC_IDX_NUM;
		if (_strPropName == "X")
		{
			loc_idx = DIR_X;
		}
		else if (_strPropName == "Y")
		{
			loc_idx = DIR_Y;
		}
		else if (_strPropName == "Z")
		{
			loc_idx = DIR_Z;
		}

		// value
		float fValue = (float)atof(_strValue.GetString());

		// move controller
		if (_strGrandParentName == "무브 컨트롤러 로케이션 키")
		{
			ChangeMoveLocation(loc_idx, fValue);
			bStopProc = true;
		}
		else if (_strGrandParentName == "Effect 컨트롤러 키 세팅")
		{
			ChangeEffectLocation(loc_idx, fValue);
			bStopProc = true;
		}
	}
	else if (_strParentName == "Up")
	{
		// location index
		LOC_IDX loc_idx = LOC_IDX_NUM;
		if (_strPropName == "X")
		{
			loc_idx = UP_X;
		}
		else if (_strPropName == "Y")
		{
			loc_idx = UP_Y;
		}
		else if (_strPropName == "Z")
		{
			loc_idx = UP_Z;
		}

		// value
		float fValue = (float)atof(_strValue.GetString());

		// move controller
		if (_strGrandParentName == "무브 컨트롤러 로케이션 키")
		{
			ChangeMoveLocation(loc_idx, fValue);
			bStopProc = true;
		}
		else if (_strGrandParentName == "Effect 컨트롤러 키 세팅")
		{
			ChangeEffectLocation(loc_idx, fValue);
			bStopProc = true;
		}
	}

	return bStopProc;
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToDofController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pDofControllerProp = new CMFCPropertyGridProperty( _T("DOF 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pDofControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;

		// 2-2. DOF Controller Key Setting 관련
		CMFCPropertyGridProperty* pDofKeySetting = new CMFCPropertyGridProperty( _T("DOF 컨트롤러 키 세팅") );
		pDofControllerProp->AddSubItem(pDofKeySetting);

		m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
		CMFCPropertyGridProperty* pDofCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pDofCmdSpecific->AllowEdit(FALSE);
		pDofKeySetting->AddSubItem(pDofCmdSpecific);
		++m_nCurrentCombo;

		float fTime, fFocusDist, fFocusRange, fCocSize;
		if (m_pOwner->m_pCutSceneWrapper->GetDOFControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fFocusDist, fFocusRange, fCocSize))
		{
			pDofCmdSpecific->AddOption( _T("Delete DOF Key")  );

			// 3-1
			m_varInfo[m_nCurrentInfo] = fTime;
			CMFCPropertyGridProperty* pDofSettingTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pDofKeySetting->AddSubItem(pDofSettingTime);
			++m_nCurrentInfo;

			// 3-2
			m_varInfo[m_nCurrentInfo] = fFocusDist;
			CMFCPropertyGridProperty* pDofFocusDist = new CMFCPropertyGridProperty( _T("초점거리"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pDofKeySetting->AddSubItem(pDofFocusDist);
			++m_nCurrentInfo;

			// 3-3
			m_varInfo[m_nCurrentInfo] = fFocusRange;
			CMFCPropertyGridProperty* pDofFocusRange = new CMFCPropertyGridProperty( _T("초점범위"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pDofKeySetting->AddSubItem(pDofFocusRange);
			++m_nCurrentInfo;

			// 3-4
			m_varInfo[m_nCurrentInfo] = fCocSize;
			CMFCPropertyGridProperty* pDofCocSize = new CMFCPropertyGridProperty( _T("착란원크기"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pDofKeySetting->AddSubItem(pDofCocSize);
			++m_nCurrentInfo;
		}
		else
		{
			pDofCmdSpecific->AddOption( _T("Add DOF Key")  );
		}

	AddProperty(pDofControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToCaptionController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pCaptionControllerProp = new CMFCPropertyGridProperty( _T("캡션 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pCaptionControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;

		// 2-2 폰트관련
		std::string strFontName;
		int nFontSize;
		bool bBold, bItalic;
		m_pOwner->m_pCutSceneWrapper->GetCaptionController(m_pOwner->m_nCurrentSelected, strFontName, nFontSize, bBold, bItalic);

		LOGFONT logfont;
		//logfont.lfHeight   = nFontSize;
		logfont.lfHeight   = -(LONG)((float(nFontSize) * 96.f) / 72.f);
		logfont.lfWidth    = 0;
		logfont.lfEscapement  = 0;
		logfont.lfOrientation  = 0;
		logfont.lfWeight   = bBold? FW_BOLD : FW_NORMAL; // FW_BOLD
		logfont.lfItalic   = bItalic? TRUE : FALSE;
		logfont.lfUnderline   = FALSE;
		logfont.lfStrikeOut   = 0;
		logfont.lfCharSet   = HANGEUL_CHARSET;  // DEFAULT_CHARSET
		logfont.lfOutPrecision  = OUT_DEFAULT_PRECIS;
		logfont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
		logfont.lfQuality   = DEFAULT_QUALITY;
		logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		strcpy_s(logfont.lfFaceName, strFontName.c_str());

		CMFCPropertyGridFontProperty* pFont = new CMFCPropertyGridFontProperty( _T("캡션 폰트"), logfont);
		pCaptionControllerProp->AddSubItem(pFont);
		//++m_nCurrentCombo;

		// 2-3. 캡션 Controller Key Setting 관련
		CMFCPropertyGridProperty* pCaptionKeySetting = new CMFCPropertyGridProperty( _T("캡션 컨트롤러 키 세팅") );
		pCaptionControllerProp->AddSubItem(pCaptionKeySetting);

		m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
		CMFCPropertyGridProperty* pCaptionCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pCaptionCmdSpecific->AllowEdit(FALSE);
		pCaptionKeySetting->AddSubItem(pCaptionCmdSpecific);
		++m_nCurrentCombo;

		float fTime, fDuration;
		std::string strScript;
		if (m_pOwner->m_pCutSceneWrapper->GetCaptionControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDuration, strScript))
		{
			pCaptionCmdSpecific->AddOption( _T("Delete Caption Key")  );

			// 3-1
			m_varInfo[m_nCurrentInfo] = fTime;
			CMFCPropertyGridProperty* pCaptionSettingTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pCaptionKeySetting->AddSubItem(pCaptionSettingTime);
			++m_nCurrentInfo;

			// 3-2
			m_varInfo[m_nCurrentInfo] = fDuration;
			CMFCPropertyGridProperty* pCaptionDuration = new CMFCPropertyGridProperty( _T("지속시간"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pCaptionKeySetting->AddSubItem(pCaptionDuration);
			++m_nCurrentInfo;

			// 3-3
			m_varInfo[m_nCurrentInfo] = strScript.c_str();
			CMFCPropertyGridProperty* pScript = new CMFCPropertyGridProperty( _T("내용"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pCaptionKeySetting->AddSubItem(pScript);
			++m_nCurrentInfo;
		}
		else
		{
			pCaptionCmdSpecific->AddOption( _T("Add Caption Key")  );
		}

	AddProperty(pCaptionControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToLetterBoxController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pLetterBoxControllerProp = new CMFCPropertyGridProperty( _T("레터박스 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pLetterBoxControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;


	AddProperty(pLetterBoxControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToBGMController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pBgmControllerProp = new CMFCPropertyGridProperty( _T("BGM 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pBgmControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;

		// 2-2. BGM Controller Key Setting 관련
		CMFCPropertyGridProperty* pBgmKeySetting = new CMFCPropertyGridProperty( _T("BGM 컨트롤러 키 세팅") );
		pBgmControllerProp->AddSubItem(pBgmKeySetting);

		m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
		CMFCPropertyGridProperty* pBgmCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pBgmCmdSpecific->AllowEdit(FALSE);
		pBgmKeySetting->AddSubItem(pBgmCmdSpecific);
		++m_nCurrentCombo;

		float fTime, fVolume;
		std::string strFileName;
		bool bLoop;
		if (m_pOwner->m_pCutSceneWrapper->GetBGMControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strFileName, fVolume, bLoop))
		{
			pBgmCmdSpecific->AddOption( _T("Delete BGM Key")  );

			// 3-1
			m_varInfo[m_nCurrentInfo] = fTime;
			CMFCPropertyGridProperty* pBgmSettingTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pBgmKeySetting->AddSubItem(pBgmSettingTime);
			++m_nCurrentInfo;

			// 3-2
			m_varInfo[m_nCurrentInfo] = strFileName.c_str();
			CMFCPropertyGridProperty* pBgmSettingFileName = new CMFCPropertyGridProperty( _T("파일이름"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pBgmKeySetting->AddSubItem(pBgmSettingFileName);
			++m_nCurrentInfo;

			// 3-3
			m_varInfo[m_nCurrentInfo] = fVolume;
			CMFCPropertyGridProperty* pBgmSettingVolume = new CMFCPropertyGridProperty( _T("볼륨"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pBgmKeySetting->AddSubItem(pBgmSettingVolume);
			++m_nCurrentInfo;

			// 3-4
			m_varCmdCombo[m_nCurrentCombo] = variant_t(bLoop);
			CMFCPropertyGridProperty* pBgmSettingLoop = new CMFCPropertyGridProperty( _T("Loop"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
			pBgmKeySetting->AddSubItem(pBgmSettingLoop);
			++m_nCurrentCombo;
		}
		else
		{
			pBgmCmdSpecific->AddOption( _T("Add BGM Key")  );
		}

	AddProperty(pBgmControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToEffectController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pEffectControllerProp = new CMFCPropertyGridProperty( _T("이팩트 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pEffectControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;

		// 2-2. Effect Controller Key Setting 관련
		CMFCPropertyGridProperty* pEffectKeySetting = new CMFCPropertyGridProperty( _T("Effect 컨트롤러 키 세팅") );
		pEffectControllerProp->AddSubItem(pEffectKeySetting);

		m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
		CMFCPropertyGridProperty* pEffectCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pEffectCmdSpecific->AllowEdit(FALSE);
		pEffectKeySetting->AddSubItem(pEffectCmdSpecific);
		++m_nCurrentCombo;

		float fTime;
		float fDurationTime;
		vec3 vPos;
		vec3 vDir;
		vec3 vUp;
		std::string strEffectName;
		std::string strEffectInfo;
		if(m_pOwner->m_pCutSceneWrapper->GetEffectControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo))
		{
			pEffectCmdSpecific->AddOption( _T("Delete Effect Key")  );

			// 3-1
			m_varInfo[m_nCurrentInfo] = fTime;
			CMFCPropertyGridProperty* pEffectSettingTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pEffectKeySetting->AddSubItem(pEffectSettingTime);
			++m_nCurrentInfo;

			// 3-2
			m_varInfo[m_nCurrentInfo] = fDurationTime;
			CMFCPropertyGridProperty* pEffectSettingDurTime = new CMFCPropertyGridProperty( _T("지속 시간"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pEffectKeySetting->AddSubItem(pEffectSettingDurTime);
			++m_nCurrentInfo;

			// 3-3
			m_varInfo[m_nCurrentInfo] = strEffectName.c_str();
			CMFCPropertyGridProperty* pEffectSettingEffectName = new CMFCPropertyGridProperty( _T("이펙트 이름"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			//CRollupEntityObjectList* pEntityObjecList = m_pOwner->m_pControlMediator_->GetViewPtr<CRollupEntityObjectList>();
			//if(pEntityObjecList)
			//{
			//	XEffectInfoMgr* pEffectInfoMgr = pEntityObjecList->GetEffectInfoMgr();
			//	if(pEffectInfoMgr)
			//	{
			//		for(std::vector<XEffectInfo *>::iterator itEffect = pEffectInfoMgr->m_vecEffectInfo.begin();
			//			itEffect != pEffectInfoMgr->m_vecEffectInfo.end(); ++itEffect)
			//		{
			//			pEffectSettingEffectName->AddOption((*itEffect)->m_strEffectName.c_str());
			//		}
			//	}
			//}
			//pEffectKeySetting->AddSubItem(pEffectSettingEffectName);
			++m_nCurrentInfo;

			// 3-4
			//m_varInfo[m_nCurrentInfo] = 100.f;
			//CMFCPropertyGridProperty* pEffectSettingEffectScale = new CMFCPropertyGridProperty( _T("이펙트 스케일"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			//pEffectKeySetting->AddSubItem(pEffectSettingEffectScale);
			//++m_nCurrentInfo;

			// 3-5
			CMFCPropertyGridProperty* pPos = new CMFCPropertyGridProperty( _T("위치") );
			pEffectKeySetting->AddSubItem(pPos);

			m_varInfo[m_nCurrentInfo] = vPos.x;
			CMFCPropertyGridProperty* pPosX = new CMFCPropertyGridProperty( _T("X"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pPos->AddSubItem(pPosX);
			m_LocProps[POS_X] = pPosX;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vPos.y;
			CMFCPropertyGridProperty* pPosY = new CMFCPropertyGridProperty( _T("Y"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pPos->AddSubItem(pPosY);
			m_LocProps[POS_Y] = pPosY;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vPos.z;
			CMFCPropertyGridProperty* pPosZ = new CMFCPropertyGridProperty( _T("Z"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pPos->AddSubItem(pPosZ);
			m_LocProps[POS_Z] = pPosZ;
			++m_nCurrentInfo;

			// 3-6
			CMFCPropertyGridProperty* pDir = new CMFCPropertyGridProperty( _T("방향") );
			pEffectKeySetting->AddSubItem(pDir);

			m_varInfo[m_nCurrentInfo] = vDir.x;
			CMFCPropertyGridProperty* pDirX = new CMFCPropertyGridProperty( _T("X"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pDir->AddSubItem(pDirX);
			m_LocProps[POS_X] = pDirX;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vDir.y;
			CMFCPropertyGridProperty* pDirY = new CMFCPropertyGridProperty( _T("Y"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pDir->AddSubItem(pDirY);
			m_LocProps[POS_Y] = pDirY;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vDir.z;
			CMFCPropertyGridProperty* pDirZ = new CMFCPropertyGridProperty( _T("Z"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pDir->AddSubItem(pDirZ);
			m_LocProps[POS_Z] = pDirZ;
			++m_nCurrentInfo;

			// 3-7
			CMFCPropertyGridProperty* pUp = new CMFCPropertyGridProperty( _T("업") );
			pEffectKeySetting->AddSubItem(pUp);

			m_varInfo[m_nCurrentInfo] = vUp.x;
			CMFCPropertyGridProperty* pUpX = new CMFCPropertyGridProperty( _T("X"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pUp->AddSubItem(pUpX);
			m_LocProps[POS_X] = pUpX;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vUp.y;
			CMFCPropertyGridProperty* pUpY = new CMFCPropertyGridProperty( _T("Y"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pUp->AddSubItem(pUpY);
			m_LocProps[POS_Y] = pUpY;
			++m_nCurrentInfo;

			m_varInfo[m_nCurrentInfo] = vUp.z;
			CMFCPropertyGridProperty* pUpZ = new CMFCPropertyGridProperty( _T("Z"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pUp->AddSubItem(pUpZ);
			m_LocProps[POS_Z] = pUpZ;
			++m_nCurrentInfo;
		}
		else
		{
			pEffectCmdSpecific->AddOption( _T("Add Effect Key")  );
		}

	AddProperty(pEffectControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToEnvController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pEnvControllerProp = new CMFCPropertyGridProperty( _T("환경 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pEnvControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;

		// 2-2. Env Controller Key Setting 관련
		CMFCPropertyGridProperty* pEnvKeySetting = new CMFCPropertyGridProperty( _T("Env 컨트롤러 키 세팅") );
		pEnvControllerProp->AddSubItem(pEnvKeySetting);

		m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
		CMFCPropertyGridProperty* pEnvCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pEnvCmdSpecific->AllowEdit(FALSE);
		pEnvKeySetting->AddSubItem(pEnvCmdSpecific);
		++m_nCurrentCombo;

		float fTime;
		int nEnvTime, nEnvWeather;
		std::string strSetName;
		if(m_pOwner->m_pCutSceneWrapper->GetEnvControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nEnvTime, nEnvWeather, strSetName))
		{
			pEnvCmdSpecific->AddOption( _T("Delete Env Key")  );

			// 3-1
			m_varInfo[m_nCurrentInfo] = fTime;
			CMFCPropertyGridProperty* pEnvSettingTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pEnvKeySetting->AddSubItem(pEnvSettingTime);
			++m_nCurrentInfo;

			// 3-2
			m_varInfo[m_nCurrentInfo] = strSetName.c_str();
			CMFCPropertyGridProperty* pEnvSettingSetName = new CMFCPropertyGridProperty( _T("EnvSet"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pEnvKeySetting->AddSubItem(pEnvSettingSetName);
			++m_nCurrentInfo;

			// 3-3
			m_varCmdCombo[m_nCurrentCombo] = REnvironment::ETimeToString((REnvironment::ETime)nEnvTime);
			CMFCPropertyGridProperty* pEnvSettingEnvTime = new CMFCPropertyGridProperty( _T("EnvTime"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
			pEnvSettingEnvTime->AllowEdit(FALSE);
			pEnvSettingEnvTime->AddOption(REnvironment::ETimeToString(REnvironment::DAWN));
			pEnvSettingEnvTime->AddOption(REnvironment::ETimeToString(REnvironment::DAYTIME));
			pEnvSettingEnvTime->AddOption(REnvironment::ETimeToString(REnvironment::SUNSET));
			pEnvSettingEnvTime->AddOption(REnvironment::ETimeToString(REnvironment::NIGHT));
			pEnvKeySetting->AddSubItem(pEnvSettingEnvTime);
			++m_nCurrentCombo;

			// 3-4
			m_varCmdCombo[m_nCurrentCombo] = REnvironment::EWeatherToString((REnvironment::EWeather)nEnvWeather);
			CMFCPropertyGridProperty* pEnvSettingEnvWeather = new CMFCPropertyGridProperty( _T("EnvWeather"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
			pEnvSettingEnvWeather->AllowEdit(FALSE);
			pEnvSettingEnvWeather->AddOption(REnvironment::EWeatherToString(REnvironment::SUNNY));
			pEnvSettingEnvWeather->AddOption(REnvironment::EWeatherToString(REnvironment::CLOUD));
			pEnvSettingEnvWeather->AddOption(REnvironment::EWeatherToString(REnvironment::RAINY));
			pEnvSettingEnvWeather->AddOption(REnvironment::EWeatherToString(REnvironment::HEAVY_RANIY));
			pEnvSettingEnvWeather->AddOption(REnvironment::EWeatherToString(REnvironment::SNOW));
			pEnvSettingEnvWeather->AddOption(REnvironment::EWeatherToString(REnvironment::HEAVY_SNOW));
			pEnvKeySetting->AddSubItem(pEnvSettingEnvWeather);
			++m_nCurrentCombo;
		}
		else
		{
			pEnvCmdSpecific->AddOption( _T("Add Env Key")  );
		}

	AddProperty(pEnvControllerProp);
	RedrawWindow();
}



void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToFadeController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pFadeControllerProp = new CMFCPropertyGridProperty( _T("페이드 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pFadeControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;

		// 2-2. Fade Controller Key Setting 관련
		CMFCPropertyGridProperty* pFadeKeySetting = new CMFCPropertyGridProperty( _T("페이드 컨트롤러 키 세팅") );
		pFadeControllerProp->AddSubItem(pFadeKeySetting);

		m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
		CMFCPropertyGridProperty* pFadeCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pFadeCmdSpecific->AllowEdit(FALSE);
		pFadeKeySetting->AddSubItem(pFadeCmdSpecific);
		++m_nCurrentCombo;

		float fTime, fAlpha;
		if(m_pOwner->m_pCutSceneWrapper->GetFadeControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fAlpha))
		{
			pFadeCmdSpecific->AddOption( _T("Delete Fade Key")  );

			// 3-1
			m_varInfo[m_nCurrentInfo] = fTime;
			CMFCPropertyGridProperty* pFadeSettingTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pFadeKeySetting->AddSubItem(pFadeSettingTime);
			++m_nCurrentInfo;

			// 3-2
			m_varInfo[m_nCurrentInfo] = fAlpha;
			CMFCPropertyGridProperty* pFadeSettingStartAlpha = new CMFCPropertyGridProperty( _T("알파"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pFadeKeySetting->AddSubItem(pFadeSettingStartAlpha);
			++m_nCurrentInfo;
		}
		else
		{
			pFadeCmdSpecific->AddOption( _T("Add Fade Key")  );
		}

	AddProperty(pFadeControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::ChangeMoveLocation( LOC_IDX _idx, float fValue ) const
{
	float fTime;
	bool bWarp;
	RVector3 vPos, vDir, vUp;
	m_pOwner->m_pCutSceneWrapper->GetMoveControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp);

	switch (_idx)
	{
	case POS_X:
		vPos.x = fValue;
		break;

	case POS_Y:
		vPos.y = fValue;
		break;

	case POS_Z:
		vPos.z = fValue;
		break;

	case DIR_X:
		vDir.x = fValue;
		break;

	case DIR_Y:
		vDir.y = fValue;
		break;

	case DIR_Z:
		vDir.z = fValue;
		break;

	case UP_X:
		vUp.x = fValue;
		break;

	case UP_Y:
		vUp.y = fValue;
		break;

	case UP_Z:
		vUp.z = fValue;
		break;
	}

	vDir.Normalize();
	vUp.Normalize();

	m_pOwner->RememberLastControllerScrollPos();
	m_pOwner->m_pCutSceneWrapper->ChangeMoveControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, vPos, vDir, vUp, bWarp, m_pOwner, true);
	m_pOwner->SetLastControllerScrollPos();
	m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToAniController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pAniControllerProp = new CMFCPropertyGridProperty( _T("Ani 컨트롤러") );

		// 2-1. 컨트롤러 일반
		SetToGeneralController(pAniControllerProp, m_varCmdCombo[m_nCurrentCombo]);
		++m_nCurrentCombo;

		// 2-2. Ani Controller Key Setting 관련
		CMFCPropertyGridProperty* pAniKeySetting = new CMFCPropertyGridProperty( _T("Ani 컨트롤러 키 세팅") );
		pAniControllerProp->AddSubItem(pAniKeySetting);

		m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
		CMFCPropertyGridProperty* pAniCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
		pAniCmdSpecific->AllowEdit(FALSE);
		pAniKeySetting->AddSubItem(pAniCmdSpecific);
		++m_nCurrentCombo;

		float fTime;
		std::string strAniName;
		if(m_pOwner->m_pCutSceneWrapper->GetAniControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, strAniName))
		{
			pAniCmdSpecific->AddOption( _T("Delete Ani Key")  );

			// 3-1
			m_varInfo[m_nCurrentInfo] = fTime;
			CMFCPropertyGridProperty* pAniSettingTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pAniKeySetting->AddSubItem(pAniSettingTime);
			++m_nCurrentInfo;

			// 3-2
			m_varInfo[m_nCurrentInfo] = strAniName.c_str();
			CMFCPropertyGridProperty* pAniSettingName = new CMFCPropertyGridProperty( _T("이름"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
			pAniKeySetting->AddSubItem(pAniSettingName);
			++m_nCurrentInfo;
		}
		else
		{
			pAniCmdSpecific->AddOption( _T("Add Ani Key")  );
		}

	AddProperty(pAniControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::SetToTalentController()
{
	ClearTempVariable();
	RemoveAll();

	// 1. 최상위
	CMFCPropertyGridProperty* pTalentControllerProp = new CMFCPropertyGridProperty( _T("Talent 컨트롤러") );

	// 2-1. 컨트롤러 일반
	SetToGeneralController(pTalentControllerProp, m_varCmdCombo[m_nCurrentCombo]);
	++m_nCurrentCombo;

	// 2-2. Talent Controller Key Setting 관련
	CMFCPropertyGridProperty* pTalentKeySetting = new CMFCPropertyGridProperty( _T("Talent 컨트롤러 키 세팅") );
	pTalentControllerProp->AddSubItem(pTalentKeySetting);

	m_varCmdCombo[m_nCurrentCombo] = _T("명령선택 ...");
	CMFCPropertyGridProperty* pTalentCmdSpecific = new CMFCPropertyGridProperty( _T("커맨드"), m_varCmdCombo[m_nCurrentCombo], _T(""), (DWORD_PTR)&m_varCmdCombo[m_nCurrentCombo] );
	pTalentCmdSpecific->AllowEdit(FALSE);
	pTalentKeySetting->AddSubItem(pTalentCmdSpecific);
	++m_nCurrentCombo;

	float fTime;
	int nID;
	if(m_pOwner->m_pCutSceneWrapper->GetTalentControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, nID))
	{
		pTalentCmdSpecific->AddOption( _T("Delete Talent Key")  );

		// 3-1
		m_varInfo[m_nCurrentInfo] = fTime;
		CMFCPropertyGridProperty* pTalentSettingTime = new CMFCPropertyGridProperty( _T("시각"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
		pTalentKeySetting->AddSubItem(pTalentSettingTime);
		++m_nCurrentInfo;

		// 3-2
		m_varInfo[m_nCurrentInfo] = nID;
		CMFCPropertyGridProperty* pTalentSettingID = new CMFCPropertyGridProperty( _T("I D"), m_varInfo[m_nCurrentInfo], _T(""), (DWORD_PTR)&m_varInfo[m_nCurrentInfo] );
		pTalentKeySetting->AddSubItem(pTalentSettingID);
		++m_nCurrentInfo;
	}
	else
	{
		pTalentCmdSpecific->AddOption( _T("Add Talent Key")  );
	}

	AddProperty(pTalentControllerProp);
	RedrawWindow();
}

void CDlgCutSceneEditor::CCutSceneMFCPropertyGridCtrl::ChangeEffectLocation( LOC_IDX _idx, float fValue ) const
{
	float fTime;
	float fDurationTime;
	vec3 vPos;
	vec3 vDir;
	vec3 vUp;
	std::string strEffectName;
	std::string strEffectInfo;
	m_pOwner->m_pCutSceneWrapper->GetEffectControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo);

	switch (_idx)
	{
	case POS_X:
		vPos.x = fValue;
		break;

	case POS_Y:
		vPos.y = fValue;
		break;

	case POS_Z:
		vPos.z = fValue;
		break;

	case DIR_X:
		vDir.x = fValue;
		break;

	case DIR_Y:
		vDir.y = fValue;
		break;

	case DIR_Z:
		vDir.z = fValue;
		break;

	case UP_X:
		vUp.x = fValue;
		break;

	case UP_Y:
		vUp.y = fValue;
		break;

	case UP_Z:
		vUp.z = fValue;
		break;
	}

	vDir.Normalize();
	vUp.Normalize();

	m_pOwner->RememberLastControllerScrollPos();
	m_pOwner->m_pCutSceneWrapper->ChangeEffectControllerKey(m_pOwner->m_nCurrentSelected, m_pOwner->m_fCurrentSelectedValue, fTime, fDurationTime, vPos, vDir, vUp, strEffectName, strEffectInfo, m_pOwner);
	m_pOwner->SetLastControllerScrollPos();
	m_pOwner->m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
}
void CDlgCutSceneEditor::OnMovecontrollerCopylocationkey()
{
	m_copiedKeyInfo.m_moveControllerKey.bCopiedMoveLocKey =
		m_pCutSceneWrapper->GetMoveControllerKey(
		m_nCurrentSelected, m_fCurrentSelectedValue,
		m_copiedKeyInfo.m_moveControllerKey.fTime,
		m_copiedKeyInfo.m_moveControllerKey.vPos,
		m_copiedKeyInfo.m_moveControllerKey.vDir,
		m_copiedKeyInfo.m_moveControllerKey.vUp,
		m_copiedKeyInfo.m_moveControllerKey.bWarp );
}

void CDlgCutSceneEditor::OnMovecontrollerPastelocationkey()
{
	if (m_copiedKeyInfo.m_moveControllerKey.bCopiedMoveLocKey)
	{
		int nSelectedCtrlId = m_nCurrentSelected;
		float fSelectedKeyValue = m_fCurrentSelectedValue;
		bool bRet = m_pCutSceneWrapper->AddMoveControllerKey(
			nSelectedCtrlId, fSelectedKeyValue,
			m_copiedKeyInfo.m_moveControllerKey.vPos,
			m_copiedKeyInfo.m_moveControllerKey.vDir,
			m_copiedKeyInfo.m_moveControllerKey.vUp,
			m_copiedKeyInfo.m_moveControllerKey.bWarp, this );

		m_nCurrentSelected = nSelectedCtrlId;
		m_fCurrentSelectedValue = fSelectedKeyValue;

		if (bRet)
		{
			m_pCutSceneWrapper->MoveCameraToMoveKeyAndSelectIt(m_nCurrentSelected, m_fCurrentSelectedValue, this);
			m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		}
	}
}

void CDlgCutSceneEditor::OnCutsceneEventobjAddFade()
{
	m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
	m_pCutSceneWrapper->AddFadeController(this);
}

void CDlgCutSceneEditor::OnCutsceneFadectrlAddkey()
{
	CDlgCutSceneFadeKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneFadeKey>(m_pControlMediator_, m_fCurrentSelectedValue, 1.0f);
	if (pDlg)
	{
		float fTime, fAlpha;
		pDlg->GetFadeData(fTime, fAlpha);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->AddFadeControllerKey(m_nCurrentSelected, fTime, fAlpha, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneFadectrlDelkey()
{
	if (m_nCurrentSelected < 0)
		return;

	m_pCutSceneWrapper->RemoveFadeControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, this);
}

void CDlgCutSceneEditor::OnCutsceneFadectrlEditkey()
{
	float fTime, fAlpha;
	m_pCutSceneWrapper->GetFadeControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, fAlpha);

	CDlgCutSceneFadeKey* pDlg = CMFCHelper::CreateDLLDlgModalOKed<CDlgCutSceneFadeKey>(m_pControlMediator_, fTime, fAlpha);
	if (pDlg)
	{
		pDlg->GetFadeData(fTime, fAlpha);

		RememberLastControllerScrollPos();
		m_pCutSceneWrapper->ChangeFadeControllerKey(m_nCurrentSelected, m_fCurrentSelectedValue, fTime, fAlpha, this);
		SetLastControllerScrollPos();

		m_pControlMediator_->GetToolCmdMgr()->RecordCommand(NULL);
		delete pDlg;
	}
}

void CDlgCutSceneEditor::OnCutsceneFadectrlDelete()
{
	OnCtrlGeneralDelete();
}
