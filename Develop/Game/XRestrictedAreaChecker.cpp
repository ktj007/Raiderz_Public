#include "stdafx.h"
#include "XRestrictedAreaChecker.h"
#include "XGameState.h"
#include "XStrings.h"

#define SHOW_MESSAGE_GAP_TIME	5.0f

XRestrictedAreaChecker::XRestrictedAreaChecker()
{
	Init();
}

void XRestrictedAreaChecker::Init()
{
	m_fPreRestrictedAreaOutTime = 0.0f;
	m_fRestrictedAreaOutTime = 0.0f;

	m_bStart = false;
	m_bEnd = false;
}

void XRestrictedAreaChecker::_UpdateRestrictedArea( float fDelta, vec3& vPos, XFieldInfo* pFieldInfo, CSFieldFileInfo* pFieldFileInfo )
{
	if(pFieldInfo  == NULL ||
		(pFieldInfo->m_rtBoundary.left == 0.0f &&
		pFieldInfo->m_rtBoundary.top == 0.0f &&
		pFieldInfo->m_rtBoundary.right == 0.0f &&
		pFieldInfo->m_rtBoundary.bottom == 0.0f))
		return;

	// 다이나믹 필드가 아니면 메세지를 보여주지 않습니다.
	if(pFieldFileInfo == NULL || pFieldFileInfo->bDynamic == false)
		return;

	// Boundary 체크
	bool bInCheck = false;

	
	MPoint my_pt = pFieldInfo->GetSectorIndex(vPos.x, vPos.y);
	MPoint boundary_pt_min = pFieldInfo->GetSectorIndex(pFieldInfo->m_rtBoundary.left + RESTRICTED_AREA_WARNING_DISTANCE, pFieldInfo->m_rtBoundary.top + RESTRICTED_AREA_WARNING_DISTANCE);
	MPoint boundary_pt_max = pFieldInfo->GetSectorIndex(pFieldInfo->m_rtBoundary.right - RESTRICTED_AREA_WARNING_DISTANCE, pFieldInfo->m_rtBoundary.bottom - RESTRICTED_AREA_WARNING_DISTANCE);

	if(my_pt.x <= boundary_pt_min.x || my_pt.x >= boundary_pt_max.x ||
		my_pt.y <= boundary_pt_min.y || my_pt.y >= boundary_pt_max.y)
		bInCheck = true;

	if(bInCheck == false)
	{
		Init();
		return;
	}

	if(m_bEnd == false &&
		m_fRestrictedAreaOutTime == 0.0f)
	{
		m_fRestrictedAreaOutTime = BATTLE_AREA_OUT_TIME;
		m_fPreRestrictedAreaOutTime = m_fRestrictedAreaOutTime;
		m_bStart = true;
	}

	m_fRestrictedAreaOutTime -= fDelta;

	if(m_fRestrictedAreaOutTime <= 0.0f)
	{
		m_fRestrictedAreaOutTime = 0.0f;
		m_bEnd = true;
		return;
	}

	// 5초 간격을 주자.
	float fGapTime = m_fPreRestrictedAreaOutTime - m_fRestrictedAreaOutTime;
	if(m_bStart == false &&						// 처음 시작은 통과
		fGapTime <= SHOW_MESSAGE_GAP_TIME)
		return;

	m_fPreRestrictedAreaOutTime = m_fRestrictedAreaOutTime;
	m_bStart = false;

	wstring strMsg = CSFormatString::Format(XGetStr(L"RESTRICTED_AREA_MSG"), FSParam((int)(m_fRestrictedAreaOutTime + 1.0f)));
	global.ui->OnPresentationUpper(strMsg);
}
