#pragma once

#include "BaseObject.h"
#include "EffectBaseReflection.h"
#include "REmitter.h"

using namespace rs3;

class CEmitterPropertyRef : public CRTTIClass<CEmitterPropertyRef, CEffectBasePropertyRef, SPID_EMITTER>
{
public:
	CEmitterPropertyRef() : m_pEmitter(NULL) {}

	virtual void SetEffect( REffectBase* pEffect )
	{
		CEffectBasePropertyRef::SetEffect( pEffect );
		m_pEmitter = MDynamicCast(REmitter, pEffect);
		_ASSERT( m_pEmitter );
	}

	bool	GetExplosion() { return m_pEmitter->IsExplosion(); }
	void	SetExplosion(bool b) { m_pEmitter->SetExplosion(b); }

	static void RegisterReflection()
	{
		RegisterProperty<bool>("Explosion", &GetExplosion, &SetExplosion, 0, 0, "Explosion", "이미터 속성");
	}

private:
	REmitter* m_pEmitter;
};