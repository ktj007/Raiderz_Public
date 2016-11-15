#include "stdafx.h"
#include "XCharacterCaption.h"
#include "XCharacter.h"
#include "XMyPlayer.h"

#include "RCameraSceneNode.h"
#include "XSystem.h"

XCaption::XCaption( MID nID, rs3::RSceneNode* pSceneNode ) 
: m_nID(nID)
, m_pSceneNode(pSceneNode)
, m_dwColor(0xffffffff)
, m_bVisible(true)
, m_bPassRender( false)
, m_vPosition(vec3::ZERO)
, m_bSelfDelete(false)
, m_matTextReflectAndScale
(
 0.24f,    0.f,    0.f,   0.f,
 0.f,     -0.24f,  0.f,   0.f,
 0.f,      0.f,    0.24f, 0.f,
 0.f,      0.f,    0.f,   1.f
 )
{

}

void XCaption::SetScale(float _fScale)
{
	m_matTextReflectAndScale._11 = _fScale;
	m_matTextReflectAndScale._22 = -(_fScale);
	m_matTextReflectAndScale._33 = _fScale;
}

////////////////////////////////////////////////////////////////////////////////

// 생성자 / 소멸자
#define CHARACTER_CPATION_PASS_RENDER_LEN	85.0f
XCharacterCaption::XCharacterCaption(MID nID, rs3::RSceneNode* pSceneNode) : XCaption(nID, pSceneNode)
, m_pActorNode(NULL)
, m_fOffSet(0)
, m_fBackupActorNodeHeight(0.0f)
{
}

XCharacterCaption::~XCharacterCaption()
{
}

bool XCharacterCaption::Update(vec3& vCamPos, RMatrix& viewInverse)
{
	//position update
	if (m_pActorNode)
	{
		bool bOk = false;
		if(static_cast<XCharacter*>(m_pSceneNode)->GetActorNodePos(MLocale::ConvAnsiToUTF16(m_pActorNode->GetNodeName().c_str()).c_str(), m_vPosition, E_TS_WORLD))
		{
			if(static_cast<XCharacter*>(static_cast<RActorNode*>(m_pActorNode)->m_pActor)->GetDoNotAnimationUpdated() == false)
			{
				m_fBackupActorNodeHeight = m_vPosition.z - m_pSceneNode->GetPosition().z;
				bOk = true;
			}
		}

		if(bOk == false)
		{
			// 애니메이션 업데이트를 못하는 상황이면 캡션이 남아있는 현상이 있으므로
			// Actor 위치에서 백업 해놓은 값으로 보간처리한다.
			m_vPosition = m_pSceneNode->GetPosition();
			m_vPosition.z += m_fBackupActorNodeHeight;
		}
	}
	else
		m_vPosition = m_pSceneNode->GetPosition();

	m_vPosition.z += m_fOffSet;

	float fDist = vCamPos.DistanceToSq( m_vPosition );
	const float XCHAR_SQ_DIS = CHARACTER_CPATION_PASS_RENDER_LEN * CHARACTER_CPATION_PASS_RENDER_LEN;
	if( fDist < XCHAR_SQ_DIS )	m_bPassRender = true;
	else						m_bPassRender = false;

	if (GetVisible() == false || m_pSceneNode->IsPrimaryCamRenerable() == false)
		return true;

	//color
	DWORD dwColor = GetColor();
	float fVisibility = m_pSceneNode->GetVisibility();

	if( fVisibility < 1.0f )
	{
		DWORD dwAlpha = (dwColor >> 24) & 0xff;
		dwAlpha = (DWORD)(dwAlpha * fVisibility);
		dwColor &= 0x00ffffff;
		dwColor |= (dwAlpha << 24);
	}
	else if( fDist < XCHAR_SQ_DIS )
	{
		DWORD dwAlpha = (dwColor >> 24) & 0xff;
		dwAlpha /= 4;
		dwColor &= 0x00ffffff;
		dwColor |= (dwAlpha << 24);
	}

	SetupRenderInfo(viewInverse, dwColor);

	REngine::GetDevice().SetDepthEnable(true);

	return true;
}

void XCharacterCaption::SetBoneNode( const rs3::RSceneNode* _pActorNdoe )
{
	m_pActorNode = const_cast<rs3::RSceneNode*>(_pActorNdoe);
	RActorNode* pActorNode = static_cast<RActorNode*>(m_pActorNode);

	// 이번 매번 등록하여야 합니다.
	// 애니메이션 변경시 클리어가 되므로 계속 등록 바랍니다. (필요한 애니메이션에 등록을 한다가 원칙입니다.)
	static_cast<XCharacter*>(pActorNode->m_pActor)->AddActiveActorNode(pActorNode);
}
////////////////////////////////////////////////////////////////////////////////
XDamageCaption::XDamageCaption( MID nID, rs3::RSceneNode* pSceneNode ) : XCombatCaption(nID, pSceneNode)
{
	m_bCritical = false;
}

DWORD XDamageCaption::UpdateColor( const float fElapsedTime )
{
	// color
	DWORD dwColor = GetColor();
	if (fElapsedTime > 0.5f)
	{
		unsigned char alpha = 255 - (unsigned char)(255 * (fElapsedTime - 0.5f) * 2);

		dwColor &= 0x00ffffff;
		dwColor = (alpha << 24) | dwColor;
	}


	return dwColor;
}

void XDamageCaption::UpdateScale( const float fElapsedTime )
{
	// scale
	float FIRST_SCALE = 0.5f;
	float INC_MAX_SCALE = 1.0f;
	float DEC_MAX_SCALE = 1.0f;

	if (m_bCritical)
	{
		FIRST_SCALE = 0.7f;

		INC_MAX_SCALE = 2.5f;
		DEC_MAX_SCALE = 1.6f;
	}

	
	const float LAST_SCALE = FIRST_SCALE + INC_MAX_SCALE - DEC_MAX_SCALE;


	float fScale = GetScale();
	if (fElapsedTime < 0.1f)
	{
		float fScaleDelta = (fElapsedTime * 10.0f);
		float fIncScale = (fScaleDelta * INC_MAX_SCALE);

		fScale = (FIRST_SCALE + fIncScale);
	}
	else if (fElapsedTime >= 0.1f && fElapsedTime < 0.2f)
	{
		float fScaleDelta = ((fElapsedTime-0.1f) * 5.0f);
		float fDecScale = (fScaleDelta * DEC_MAX_SCALE);

		fScale = (FIRST_SCALE + INC_MAX_SCALE) - fDecScale;
	}
	else fScale = LAST_SCALE;

	SetScale(fScale);
}

void XDamageCaption::UpdatePosition( const float fElapsedTime, const float fDelta )
{
	if (fElapsedTime > 0.8f)
	{
		float fOffset = (fElapsedTime - 0.8f) * (-25.f) + 35.0f;
		m_vPosition.z += (fOffset * fDelta);
	}
	else if (fElapsedTime > 0.1f)
	{
		m_vPosition.z += (35.0f * fDelta);
	}

	/*
	// pos
	if (fElapsedTime > 0.8f)
	{
		float fOffset = (fElapsedTime - 0.8f) * (-25.f) + 5.0f;
		m_vPosition.z += (fOffset * fDelta);
	}
	else if (fElapsedTime > 0.5f)
	{
		m_vPosition.z += (20.0f * fDelta);
	}
	*/
}

void XDamageCaption::SetupEnemy( const wchar_t* szDamage, vec3& pos, bool bCritical/*=false*/, uint32 nColor/*=0xFFFFFFFF*/ )
{
	static bool bLeftShift		= false;
	SetColor(nColor);

	m_vPosition = pos;
	m_bCritical = bCritical;
	SetText(szDamage);
	SetVisible(true);

	// 위치 변경
	vec3 vChrDir = gvar.Game.pMyPlayer->GetDirection();
	vec3 vCross = CrossProduct(vChrDir, UP_VECTOR);
	vCross.Normalize();

	float fRand		= 0.0f;
	if( bLeftShift )
		fRand = -10.0f;
	else
		fRand = 10.0f;

	bLeftShift		= !bLeftShift;

	m_vPosition	+= ( vCross * fRand );
}

void XDamageCaption::SetupMine( const wchar_t* szDamage, vec3& pos, bool bCritical/*=false*/, uint32 nColor/*=0xFFFF0000*/ )
{
	SetColor(nColor);

	m_vPosition = pos;
	m_bCritical = bCritical;
	SetText(szDamage);
	SetVisible(true);
}

bool XDamageCaption::OnUpdate( float fDelta, RMatrix& viewInverse )
{
	if (m_fElapsedTime > 1.0f) return false;

	UpdateScale(m_fElapsedTime);
	UpdatePosition(m_fElapsedTime, fDelta);
	DWORD dwColor = UpdateColor(m_fElapsedTime);

	SetupRenderInfo(viewInverse, dwColor);

	return true;
}

bool XDamageCaption::Update( vec3& vCamPos, RMatrix& viewInverse )
{
	return XCombatCaption::Update(vCamPos, viewInverse);
}
////////////////////////////////////////////////////////////////////////////////


bool XCombatCaption::Update( vec3& vCamPos, RMatrix& viewInverse )
{
	float fNowTime = 0.0f;
	if (global.system) fNowTime = global.system->GetTimer().GetTime();

	m_fElapsedTime = fNowTime - m_fStartTime;
	const float fDelta = fNowTime - m_fLastTime;
	m_fLastTime = fNowTime;

	return OnUpdate(fDelta, viewInverse);

}

void XCombatCaption::preRender()
{
	REngine::GetDevice().SetDepthEnable(false);
}

void XCombatCaption::postRender()
{
	REngine::GetDevice().SetDepthEnable(true);
}

XCombatCaption::XCombatCaption( MID nID, rs3::RSceneNode* pSceneNode ) : XCaption(nID, pSceneNode), m_fElapsedTime(0.0f)
{
	if (global.system)
		m_fStartTime = global.system->GetTimer().GetTime();
	else m_fStartTime = 0.0f;

	m_fLastTime = m_fStartTime;
	m_bSelfDelete = true;
}

//////////////////////////////////////////////////////////////////////////
XDamageTextureCaption::XDamageTextureCaption( MID nID, rs3::RSceneNode* pSceneNode )
 : XDamageCaption(nID, pSceneNode)
{

}

void XDamageTextureCaption::UpdateScale( const float fElapsedTime )
{
	// scale
	float FIRST_SCALE = 0.5f;
	float INC_MAX_SCALE = 0.8f;
	float DEC_MAX_SCALE = 0.8f;

	if (m_bCritical)
	{
		FIRST_SCALE = 0.7f;

		INC_MAX_SCALE = 2.0f;
		DEC_MAX_SCALE = 1.6f;
	}


	const float LAST_SCALE = FIRST_SCALE + INC_MAX_SCALE - DEC_MAX_SCALE;


	float fScale = GetScale();
	if (fElapsedTime < 0.1f)
	{
		float fScaleDelta = (fElapsedTime * 10.0f);
		float fIncScale = (fScaleDelta * INC_MAX_SCALE);

		fScale = (FIRST_SCALE + fIncScale);
	}
	else if (fElapsedTime >= 0.1f && fElapsedTime < 0.2f)
	{
		float fScaleDelta = ((fElapsedTime-0.1f) * 5.0f);
		float fDecScale = (fScaleDelta * DEC_MAX_SCALE);

		fScale = (FIRST_SCALE + INC_MAX_SCALE) - fDecScale;
	}
	else fScale = LAST_SCALE;

	SetScale(fScale);
}

bool XDamageTextureCaption::Update( vec3& vCamPos, RMatrix& viewInverse )
{
	float fDist = vCamPos.DistanceToSq( m_vPosition );
	const float XCHAR_SQ_DIS = SHOW_DAMAGE_COUNT_DISTANCE * SHOW_DAMAGE_COUNT_DISTANCE;
	if( fDist < XCHAR_SQ_DIS )	m_bPassRender = false;
	else						m_bPassRender = true;

	return XDamageCaption::Update(vCamPos, viewInverse);
}

bool XDamageTextureCaption::CheckRenderByCamera( RCameraSceneNode* pCam )
{
	float fRadius = (GetText().size() / 2.0f) * XCONST::CAPTION_DAMAGE_FONT_SIZE * m_matTextReflectAndScale._11;
	RSphere testSphere(m_vPosition, fRadius);

	return pCam->GetViewFrustum().TestSphere(testSphere);
}
