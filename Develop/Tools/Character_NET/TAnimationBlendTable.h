#ifndef _TANIMATION_BLEND_TABLE_H
#define _TANIMATION_BLEND_TABLE_H

#pragma once

#include "RMeshAnimationMgr.h"

class TAnimationBlendTable
{
private:
	TCharacter*			m_pActor;
	TAnimationBlendInfo* m_pAnimationBlendInfo;

	vector<ANIMATION_BLEND_LIST>	m_vecAnimationBlendList;

private:
	void				SetBlendAnimationData(string strSourceAni, string strTargetAni, float fTime);
	void				DelBlendAnimationData(string strSourceAni, string strTargetAni);
	void				ReSetBlendAnimationData(string strSourceAni, string strTargetAni, string strModifyTargetAni, float fModifyTime);

public: 
	TAnimationBlendTable();
	virtual ~TAnimationBlendTable();

	void				InsertBlendAnimationData(string strSourceAni, string strTargetAni, float fTime);
	void				DeleteBlendAnimationData(string strSourceAni, string strTargetAni);
	void				ModifyBlendAnimationData(string strSourceAni, string strTargetAni, string strModifyTargetAni, float fModifyTime);

	void				SetActor(TCharacter* pActor);

	ANIMATION_BLEND_LIST * GetAnimationBlendList(string& strSource);
	ANIMATION_BLEND_DATA * GetAnimationBlendData(string& strSource, string& strTarget);

	void				SaveBlendInfo();
};

#endif