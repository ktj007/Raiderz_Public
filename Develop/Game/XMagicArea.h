#ifndef _XMAGICAREA_H
#define _XMAGICAREA_H

#include "XObject.h"
#include "XMagicAreaEffect.h"

class XModuleEntity;
class XModuleBuff;
class XNPCInfo;

class XMagicArea : public XObject, public MMemPool<XMagicArea>
{
	MDeclareRTTI;
private:
	XMagicAreaEffect			m_Effect;

protected:
	// npc model
	XNPCInfo*					m_pNPCInfo;

	XModuleEntity*				m_pModuleEntity;

	bool						m_bModelLoadingComplete;
	bool						m_bDespawn;
	bool						m_bStarted;
	bool						m_bUseRemainTime;

	float						m_fRemainTime;

private:
	void						AniSpawn();
	void						AniIdle();
	void						AniAttack();
	void						AniDespawn();

	void						DoModelLoadingComplete();

	bool						CheckModelLoadingComplete();
	void						CheckAnimationState();
	void						CheckDebugMagicArea();
	void						CheckRemainTime(float fDelta);

	void						SetupModel( XBuffInfo* pBuffInfo );

protected:
	virtual void				RegisterModules();
	virtual void				UnregisterModules();
	virtual void				OnUpdate(float fDelta);
	virtual bool				OnCreate();
	virtual void				OnDestroy();
	virtual void				OnRender();

public:
	XMagicArea(MUID uid);
	virtual ~XMagicArea();

	virtual XObjectID			GetType() { return XOBJ_MAGIC_AREA; }

	void						Setup(vec3& vPos, int nBuffID, float fReMainTime, bool bStarted = false);


	void						Despawn();
	void						TrapAction();
};

#endif // _XMAGICAREA_H
