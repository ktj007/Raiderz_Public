#include "stdafx.h"
#include "XModelEffect.h"

XModelEffect::XModelEffect()
: XEffectBase()
, m_vScale(1.0f, 1.0f, 1.0f)
, m_vInverseScale(1.0f, 1.0f, 1.0f)
{
	m_bImmediatlyDelete		= false;
}

XModelEffect::~XModelEffect()
{

}

bool XModelEffect::IsEffectLoop()
{
	return GetEffect()->IsLooping();
}

bool XModelEffect::IsEffectLoadingComplete()
{
	if(m_pEffect)
	{
		return m_pEffect->IsLoadingCompleted();
	}

	return false;
}

bool XModelEffect::IsEffectLive()
{
	// 이펙트가 살아있는가?
	REffectSceneNode::ControlEffectSceneNodeLife((REffectSceneNode**)&m_pEffect);
	if (m_pEffect == NULL)
	{
		return false;
	}

	return true;
}

void XModelEffect::SetScale( float scale )
{
	SetScale(vec3(scale, scale, scale));
}

void XModelEffect::SetScale( vec3 scale )
{
	m_vScale = scale;

	if(m_pEffect)
	{
		m_pEffect->SetScale(scale);
	}
}

void XModelEffect::SetInverseScale( vec3 inverseScale )
{
	m_vInverseScale = inverseScale;
}

void XModelEffect::SetImmediatlyDelete( bool bDelType )
{
	m_bImmediatlyDelete = bDelType;
}


void XModelEffect::StopXEffect()
{
	if(m_pEffect)
	{
		GetEffect()->Stop();
	}
}

void XModelEffect::SetLastFramePause( bool bPause )
{
	if(m_pEffect)
	{
		GetEffect()->SetLastFramePause(bPause);
	}
}