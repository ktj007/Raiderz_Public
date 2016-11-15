#ifndef _X_BPART_NON_PLAYER_H
#define _X_BPART_NON_PLAYER_H

#include "XNonPlayer.h"

class XBPartNonPlayer : public XNonPlayer
{
	MDeclareRTTI;

protected:

	vec3 m_vRenderPos;
	vec3 m_vRenderDir;
	vec3 m_vRenderUp;

	MMatrix m_matRender;

	string m_strMeshName;

	float m_fScale;

	bool m_bOnlyOne;

public:
	vec3 m_vServerPos;

	bool m_bBPartLootable;

public:
	XBPartNonPlayer(MUID uid, UIID nUIID);
	virtual ~XBPartNonPlayer();

	virtual XObjectID				GetType() { return XOBJ_BPART_NPC; }
	
	void							InBPartNPC(XNPCInfo* pNPCInfo, TD_UPDATE_CACHE_PBART* pBPartNPCCacheInfo, bool bAppearEffect = true, bool bLoadingAsync = true, MMatrix* pMatRender = NULL);
	virtual void					OnEvent(XEvent& msg);

	virtual void					Despawn();
};








#endif