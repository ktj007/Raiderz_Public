#pragma once

#include "XInteractionEffect.h"

class XCheckTargetableEffect
{
protected:
	bool	m_bShowLootbleTargetEffect;
	XInteractionEffect*	m_pInteractionEffect;

	void	CheckTargetableMapObject(XObject* pOwner);
	void	CheckTargetableNPC(XObject* pOwner);
	void	CheckTargetableNetPlayer(XObject* pOwner);

	void	CreateTargetEffect(INTERACTION_EFFECT_TYPE eEffectType);
	void	DestroyTargetEffect();

	void	EnableGlowEffect( XObject* pOwner, vec4 vColor = XCONST::INTERACTION_TARGET_EDGE_COLOR );
	void	DisableGlowEffect( XObject* pOwner );

public:
	XCheckTargetableEffect(XObject* pOwner);
	virtual ~XCheckTargetableEffect();

	void	Destroy();

	void	Update(XObject* pOwner);

	bool	IsShowLootbleTargetEffect() const { return m_bShowLootbleTargetEffect; }
	void	SetShowLootbleTargetEffect(bool val) { m_bShowLootbleTargetEffect = val; }

	void	DisableTargetEffect(XObject* pOwner);
};