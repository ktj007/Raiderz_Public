#ifndef _XTALENT_EFFECT_PRE_LOADER_H
#define	_XTALENT_EFFECT_PRE_LOADER_H

#pragma once

//////////////////////////////////////////////////////////////////////////
class XTalentEffectPreLoader
{
private:
	void				PreLoaderTalentEffectInfo(XTalentInfo* pTalentInfo);
	void				PreLoaderTalentEventEffect(XTalentInfo* pTalentInfo);
	void				PreLoaderTalentEventEffectInvoke(CSTalentEventInfo& eventInfo);

public:
	XTalentEffectPreLoader();
	virtual ~XTalentEffectPreLoader();

	void				PreLoadTalentEffect(int nTalentID);
};

#endif // _XTALENT_EFFECT_PRE_LOADER_H
