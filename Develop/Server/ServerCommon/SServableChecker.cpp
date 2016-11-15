#include "StdAfx.h"
#include "SServableChecker.h"
#include "MTime.h"

float SServableChecker::TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC = 5.0f;	///< 주기적으로 체크하는 시간.

SServableChecker::SServableChecker()
: m_bIsServable(false)
{
	m_pServableCheckerRglt = new MRegulator(TIME_UPDATE_SERVABLECHECKE_ELAPSEDTIME_SEC);
}

SServableChecker::~SServableChecker()
{
	SAFE_DELETE(m_pServableCheckerRglt);
}

void SServableChecker::Update(const float fDelta)
{
	if (!m_pServableCheckerRglt->IsReady(fDelta))
		return;

	if (Check(fDelta))	SetServable(true);
	else				SetServable(false);
}