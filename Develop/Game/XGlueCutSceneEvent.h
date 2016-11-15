#ifndef _XGLUE_CUTSCENE_EVENT_H
#define _XGLUE_CUTSCENE_EVENT_H

#pragma once
#include "XUISystem.h"

///////////////////////////////////////////////////////////////////////////
/// 이벤트를 위한 데이터 클래스
class XGlueCutSceneEvent
{
private:
	MWLua *					m_pMWLua;
public:
	XGlueCutSceneEvent();
	~XGlueCutSceneEvent();
	void Init(MWLua* pLua);


	void			OnShowCaptionScript(const char* chScript, int nFontHeight)	{ WCALL2(m_pMWLua, "shCutSceneEvent_ShowCaptionScript", chScript, nFontHeight, WNULL); }
	void			OnHideCaptionScript()										{ WCALL(m_pMWLua, "shCutSceneEvent_HideCaptionScript", WNULL); }

	void			OnShowLetterBox(int nScreenW, int nScreenH, int nLetterBoxH) { WCALL3(m_pMWLua, "shCutSceneEvent_ShowLetterBox", nScreenW, nScreenH, nLetterBoxH, WNULL); }
	void			OnHideLetterBox()											{ WCALL(m_pMWLua, "shCutSceneEvent_HideLetterBox", WNULL); }

	void			OnShowKeyEvent(const char* chKyeEvent, int nFontPosX, int nFontPosY)	{ WCALL3(m_pMWLua, "shCutSceneEvent_ShowKeyEvent", chKyeEvent, nFontPosX, nFontPosY, WNULL); }
	void			OnHideKeyEvent()										{ WCALL(m_pMWLua, "shCutSceneEvent_HideKeyEvent", WNULL); }
};

#endif
