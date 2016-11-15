#include "stdafx.h"
#include "XMyCharacterAlphaMgr.h"
#include "XCameraManager.h"
#include "XModuleEntity.h"

void XMyCharacterAlphaMgr::Check( float fCameraDist, XModuleEntity* pModuleEntity )
{
	if (m_bAlphaState)
	{
		if (fCameraDist >= CONST_ALPHA_DIST())
		{
			pModuleEntity->StartFade(1.0f, CONST_FADE_TIME());
			m_bAlphaState = false;
		}
	}
	else
	{
		if (fCameraDist < CONST_ALPHA_DIST())
		{
			pModuleEntity->StartFade(CONST_ALPHA_VALUE(), CONST_FADE_TIME());
			m_bAlphaState = true;
		}
	}
}