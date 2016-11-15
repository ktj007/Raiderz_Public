#include "stdafx.h"
#include "XRealSound.h"
#include "Main.h"
#include "MainDlg.h"




bool XSoundAdapter::getPosition( FMOD_VECTOR* pPos)
{
	CMainDlg* pDlg = (CMainDlg*)theApp.GetMainWnd();
	if ( m_nNum > 0  &&  pDlg != NULL)
	{
		POINT pt = pDlg->GetPosition( m_nNum - 1);

		pPos->x = (float)pt.x;
		pPos->y = (float)pt.y;
		pPos->z =0.0f;

		return true;
	}
	else
	{
		pPos->x = 0.0f;
		pPos->y = 0.0f;
		pPos->z = 0.0f;

		return true;
	}

	return false;
}
