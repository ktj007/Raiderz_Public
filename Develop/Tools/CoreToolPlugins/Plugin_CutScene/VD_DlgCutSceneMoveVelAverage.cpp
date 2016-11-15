// VD_DlgCutSceneMoveVelAverage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Plugin_CutScene.h"
#include "VD_DlgCutSceneMoveVelAverage.h"

#include "WS_CutSceneXCutSceneWrapper.h"

// CDlgCutSceneMoveVelAverage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgCutSceneMoveVelAverage, CDialog)

CDlgCutSceneMoveVelAverage::CDlgCutSceneMoveVelAverage(CXCutSceneWrapper* pCutSceneWrapper, int nCurrentSelected, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCutSceneMoveVelAverage::IDD, pParent)
{
	m_pCutSceneWrapper = pCutSceneWrapper;
	m_nIntervalType = 0;
	m_fAverVel = 0.f;
	m_fIntervalStart = 0.f;
	m_fIntervalEnd = 0.f;

	m_nCurrentSelected = nCurrentSelected;
}

CDlgCutSceneMoveVelAverage::~CDlgCutSceneMoveVelAverage()
{
}

void CDlgCutSceneMoveVelAverage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_CUTSCENE_MOVE_INTERVAL_ALL, m_nIntervalType);
	DDX_Text(pDX, IDC_EDIT_MOVE_INTERVAL_INPUT, m_fAverVel);
	DDX_Text(pDX, IDC_EDIT_MOVE_INTERVAL_START, m_fIntervalStart);
	DDX_Text(pDX, IDC_EDIT_MOVE_INTERVAL_END, m_fIntervalEnd);
}

BOOL CDlgCutSceneMoveVelAverage::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetEnableIntervalAll();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CDlgCutSceneMoveVelAverage, CDialog)
	ON_BN_CLICKED(IDC_RADIO_CUTSCENE_MOVE_INTERVAL_ALL, &CDlgCutSceneMoveVelAverage::OnBnClickedRadioCutsceneMoveIntervalAll)
	ON_BN_CLICKED(IDC_RADIO_CUTSCENE_MOVE_INTERVAL_PART, &CDlgCutSceneMoveVelAverage::OnBnClickedRadioCutsceneMoveIntervalPart)
	ON_BN_CLICKED(IDC_BUTTON_CUTSCENE_CALC_MOVE_INTERVAL_VEL, &CDlgCutSceneMoveVelAverage::OnBnClickedButtonCutsceneCalcMoveIntervalVel)
END_MESSAGE_MAP()


// CDlgCutSceneMoveVelAverage 메시지 처리기입니다.


void CDlgCutSceneMoveVelAverage::SetEnableIntervalAll()
{
	m_nIntervalType = 0;
	GetDlgItem(IDC_EDIT_MOVE_INTERVAL_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MOVE_INTERVAL_END)->EnableWindow(FALSE);
}

void CDlgCutSceneMoveVelAverage::SetEnableIntervalPart()
{
	m_nIntervalType = 1;
	GetDlgItem(IDC_EDIT_MOVE_INTERVAL_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_MOVE_INTERVAL_END)->EnableWindow(TRUE);
}

void CDlgCutSceneMoveVelAverage::OnBnClickedRadioCutsceneMoveIntervalAll()
{
	SetEnableIntervalAll();
}

void CDlgCutSceneMoveVelAverage::OnBnClickedRadioCutsceneMoveIntervalPart()
{
	SetEnableIntervalPart();
}

void CDlgCutSceneMoveVelAverage::OnBnClickedButtonCutsceneCalcMoveIntervalVel()
{
	UpdateData(TRUE);
	m_fAverVel = 10.f;
	if (0 == m_nIntervalType)
	{
		// 전체 구간을 선택해서 평균을 내는 경우
		std::vector< rs3::RVector3 > vLocations;
		std::vector< float > vTimes;
		m_pCutSceneWrapper->GetCurrentCutSceneObjectMoveKeyInfo(m_nCurrentSelected, vLocations, vTimes);

		int nSize = (int)vTimes.size();
		if (nSize > 1)
		{
			int nLastIdx = nSize - 1;
			float fLast = vTimes[nLastIdx];
			float fFirst = vTimes[0];
			float fIntervalTime = fLast - fFirst;

			float fAccumedLength = 0.f;
			for (int i = 0; i < nLastIdx; ++i)
			{
				float fDist = (vLocations[i+1] - vLocations[i]).Length();
				fAccumedLength = fDist + fAccumedLength;
			}

			m_fAverVel = (fAccumedLength / fIntervalTime) * 0.01f;
		}
	}
	else if (m_fIntervalEnd > m_fIntervalStart)
	{
		// 구간 선택
		// 전체 구간을 선택해서 평균을 내는 경우
		std::vector< rs3::RVector3 > vLocations;
		std::vector< float > vTimes;
		m_pCutSceneWrapper->GetCurrentCutSceneObjectMoveKeyInfo(m_nCurrentSelected, vLocations, vTimes);

		float fIntervalTime = -1.f;
		int nFirstIdx = -1;
		int nLastIdx = -1;

		CalcPart(fIntervalTime, nFirstIdx, nLastIdx, vTimes);
		if (fIntervalTime > 0.f && nFirstIdx >= 0 && nLastIdx >= 0)
		{
			float fAccumedLength = 0.f;
			for (int i = nFirstIdx; i < nLastIdx; ++i)
			{
				float fDist = (vLocations[i+1] - vLocations[i]).Length();
				fAccumedLength = fDist + fAccumedLength;
			}

			m_fAverVel = (fAccumedLength / fIntervalTime) * 0.01f;
		}

	}

	UpdateData(FALSE);
}

void CDlgCutSceneMoveVelAverage::CalcPart( float& _interval, int& _first, int& _last, const std::vector<float>& _times )
{
	float fFirst = -1.f;
	float fLast = -1.f;

	int nFirstIdx = -1;
	int nLastIdx = -1;

	int nSize = (int)_times.size();
	for (int i = 0; i < nSize; ++i)
	{
		if (_times[i] >= m_fIntervalStart && fFirst < 0.f)
		{
			fFirst = _times[i];
			nFirstIdx = i;
		}

		if (_times[i] <= m_fIntervalEnd)
		{
			fLast = _times[i];
			nLastIdx = i;
		}
	}

	_interval = fLast - fFirst;
	_first = nFirstIdx;
	_last = nLastIdx;
}
