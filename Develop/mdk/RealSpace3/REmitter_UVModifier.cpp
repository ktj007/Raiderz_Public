#include "stdafx.h"
#include "REmitter.h"
#include "RXMLTemplate.h"
#include "RUtil.h"

namespace rs3 {

REmitter::REmitterUVAniModifier::REmitterUVAniModifier( REmitter* pEmitter ) :
	  m_pEmitter(pEmitter)
	, m_nCountX(1)
	, m_nCountY(1)
	, m_nMaxFrame(1)
	, m_fFrameInterval(0.1f)
	, m_bLoop(false)
	, m_fBlend(1.f)
	, m_UV(RVector4(0.f, 0.f, 0.f, 0.f))
	, m_UV2(RVector4(0.f, 0.f, 0.f, 0.f))
	, m_ScaleX(1.f)
	, m_ScaleY(1.f)
{
}

void REmitter::REmitterUVAniModifier::Update( RParticle* pParticle )
{
	RPFC_THISFUNC;

	int frame, frame2;
	if( m_bLoop )
	{
		float rate = pParticle->m_fTime / m_fFrameInterval;
		m_fBlend = 1.f - ( rate - floorf(rate) );

		frame = (int)rate + 1;
		if( frame >= m_nMaxFrame )
		{
			frame = frame % m_nMaxFrame;
		}
	}
	else
	{
		// 프레임 간격을 파티클 라이프와 맞춘다.
		float rate = pParticle->m_fTime / pParticle->m_fLife * m_nMaxFrame;
		m_fBlend = 1.f - ( rate - floorf(rate) );

		frame = (int)rate + 1;
		if( frame >= m_nMaxFrame )
		{
			frame = m_nMaxFrame - 1;
			m_fBlend = 0.f;
		}
	}

	if( frame != pParticle->m_nCurrentFrame )
	{
		pParticle->m_nPrevFrame = pParticle->m_nCurrentFrame;
		pParticle->m_nCurrentFrame = frame;
	}
	frame2 = pParticle->m_nPrevFrame;


	int nX = frame % m_nCountX;
	int nY = frame / m_nCountX;
	m_UV.x = nX * m_ScaleX;
	m_UV.z = m_UV.x + m_ScaleX;
	m_UV.y = nY * m_ScaleY;
	m_UV.w = m_UV.y + m_ScaleY;

	int nX2 = frame2 % m_nCountX;
	int nY2 = frame2 / m_nCountX;
	m_UV2.x = nX2 * m_ScaleX;
	m_UV2.z = m_UV2.x + m_ScaleX;
	m_UV2.y = nY2 * m_ScaleY;
	m_UV2.w = m_UV2.y + m_ScaleY;
}

RVector4 REmitter::REmitterUVAniModifier::GetVertexScale( const RVector2 *pTexPivot )
{
	return RVector4( pTexPivot->x, pTexPivot->y, 1.f - pTexPivot->x, 1.f - pTexPivot->y );
}

PVERTEXFORMAT* REmitter::REmitterUVAniModifier::FillVertex(PVERTEXFORMAT* pVertex, const RVector* pPos, RParticle* pParticle)
{
	RPFC_THISFUNC;

	_ASSERT( m_pEmitter );
	Update( pParticle );
	return m_pEmitter->FillParticleVertexApplyVisibilityResult( pVertex, pPos, &m_UV, &m_UV2, pParticle->m_vColor, m_fBlend );
}

bool REmitter::REmitterUVAniModifier::SaveToXML( MXmlElement &element )
{
	REmitter::REmitterUVAniDesc desc;
	GetDesc(desc);

	TXML::_SetElementValue( &element, "DIVIDE_X", "int", desc.nCountX );
	TXML::_SetElementValue( &element, "DIVIDE_Y", "int", desc.nCountY );
	TXML::_SetElementValue( &element, "MAX_FRAME", "int", desc.nMaxFrame );
	TXML::_SetElementValue( &element, "DELAY", "float", desc.fFrameInterval );
	TXML::_SetElementValue( &element, "LOOP", "int", desc.bLoop ? 1 : 0 );
	return true;
}

bool REmitter::REmitterUVAniModifier::CreateFromXML( MXmlElement &element )
{
	REmitter::REmitterUVAniDesc desc;

	desc.nCountX = TXML::_GETElement<int>::Value( &element, "DIVIDE_X", "int" );
	desc.nCountY = TXML::_GETElement<int>::Value( &element, "DIVIDE_Y", "int" );
	desc.nMaxFrame = TXML::_GETElement<int>::Value( &element, "MAX_FRAME", "int" );
	desc.fFrameInterval = TXML::_GETElement<float>::Value( &element, "DELAY", "float" );
	desc.bLoop = TXML::_GETElement<int>::Value( &element, "LOOP", "int" ) ? true : false;

	SetDesc( desc );
	return true;
}

void REmitter::REmitterUVAniModifier::SetDesc( const REmitter::REmitterUVAniDesc& desc )
{
	m_nCountX = desc.nCountX;
	if( m_nCountX < 1 )
		m_nCountX = 1;

	m_nCountY = desc.nCountY;
	if( m_nCountY < 1 )
		m_nCountY = 1;

	m_nMaxFrame = desc.nMaxFrame;
	if( m_nMaxFrame < 1 )
		m_nMaxFrame = 1;

	if( m_nMaxFrame > m_nCountX * m_nCountY )
		m_nMaxFrame = m_nCountX * m_nCountY;

	m_fFrameInterval = desc.fFrameInterval;
	if( m_fFrameInterval < 0.001f )
		m_fFrameInterval = 0.001f;

	m_ScaleX = 1.f / m_nCountX;
	m_ScaleY = 1.f / m_nCountY;

	m_bLoop = desc.bLoop;
}

void REmitter::REmitterUVAniModifier::GetDesc( REmitter::REmitterUVAniDesc& desc )
{
	desc.nCountX = m_nCountX;
	desc.nCountY = m_nCountY;
	desc.nMaxFrame = m_nMaxFrame;
	desc.fFrameInterval = m_fFrameInterval;
	desc.bLoop = m_bLoop;
}

void REmitter::SetUVModifierDefault()
{
	SAFE_DELETE( m_pUVModifier );

	m_sEmitterDesc.eUVModifierType = EMITTER_UVMOD_DEFAULT;
}

void REmitter::SetUVModifierAnimation( const REmitterUVAniDesc* pDesc )
{
	if( !m_pUVModifier )
		m_pUVModifier = new REmitterUVAniModifier(this);

	if( pDesc )
		m_pUVModifier->SetDesc( *pDesc );

	m_sEmitterDesc.eUVModifierType = EMITTER_UVMOD_ANIMATION;
}

bool REmitter::GetUVModifierAnimationDesc( REmitterUVAniDesc& desc )
{
	if( !m_pUVModifier )
		return false;

	m_pUVModifier->GetDesc( desc );
	return true;
}

}