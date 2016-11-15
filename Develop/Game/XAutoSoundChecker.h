#ifndef _XAUTO_SOUND_CHECKER_H
#define	_XAUTO_SOUND_CHECKER_H

#pragma once


class XAutoSoundChecker
{
public:
	XAutoSoundChecker();
	~XAutoSoundChecker();

	void				HitSoundChecker(XObject* pObject, XTalentInfo* pTalentInfo, XObject* pDestObject = NULL);

	void				GetTalentEffectInfo( XTalentInfo* pTalentInfo, vector<XTalentEffectInfo *>& vecEffectInfoByType );
};

#endif // _XAUTO_SOUND_CHECKER_H
