#include "stdafx.h"
#include "TDecalEffect.h"
#include "TCharacter.h"

TDecalEffect::TDecalEffect()
: XEffectBase()
, m_eType(DET_NONE)
, m_hTexture(R_NONE)
, m_fSize(0.0f)
, m_vPos(vec3::ZERO)
, m_bFollow(false)
{
	m_strBoneName.clear();

	RVt_pos_tex1 pos;
	pos.vPos = vec3(1, -1, 0);
	pos.u	 = 0;
	pos.v	 = 0;
	m_vVertexArray.push_back(pos);

	pos.vPos = vec3(-1, -1, 0);
	pos.u	 = 1;
	pos.v	 = 0;
	m_vVertexArray.push_back(pos);

	pos.vPos = vec3(1, 1, 0);
	pos.u	 = 0;
	pos.v	 = 1;
	m_vVertexArray.push_back(pos);

	pos.vPos = vec3(-1, -1, 0);
	pos.u	 = 1;
	pos.v	 = 0;
	m_vVertexArray.push_back(pos);

	pos.vPos = vec3(-1, 1, 0);
	pos.u	 = 1;
	pos.v	 = 1;
	m_vVertexArray.push_back(pos);

	pos.vPos = vec3(1, 1, 0);
	pos.u	 = 0;
	pos.v	 = 1;
	m_vVertexArray.push_back(pos);

	m_dwColor = D3DCOLOR_ARGB(200, 255, 255, 255);
}

TDecalEffect::~TDecalEffect()
{
	if (m_hTexture != R_NONE)
	{
		REngine::GetDevice().DeleteTexture(m_hTexture);
		m_hTexture = R_NONE;
	}
}

bool TDecalEffect::Create( MUID& uid, const string& str, BYTE byDecalEffectType, const vec3& vPos, const float fSize )
{
	m_UID = uid;
	m_strEffectFileName = str;
	m_eType = (DECALEFFECT_TYPE)byDecalEffectType;
	m_vPos = vPos;
	m_fSize = fSize;

	// 사이즈가 최소한 1보다 작을 수 없다.
	if(m_fSize < 1.0f)
		m_fSize = 1.0f;

	// 임시로 바닥 텍스쳐를 정했다.
	if (m_hTexture == R_NONE)
	{
		m_hTexture = REngine::GetDevice().CreateTexture(str.c_str());
	}

	return false;
}

void TDecalEffect::UpdateXEffect( float fDelta )
{
	UpdateFollow();
	UpdateDurationTime(fDelta);
	UpdateEffectFader(fDelta);
}

void TDecalEffect::SetDecalEffectData( const vec3& vPos, const float fSize )
{
	m_fSize = fSize;
}

void TDecalEffect::SetDecalEffectColor( DWORD dwColor )
{
	m_dwColor = dwColor;
}

void TDecalEffect::StopXEffect()
{
}

void TDecalEffect::UpdateFollow()
{
	if(m_bFollow)
	{
		TCharacter* pObject = g_pMainApp->GetActor();
		if(pObject == NULL)
		{
			// 없어졌다.
			m_bDelMe = true;
			return;
		}

		vec3 vWorldPosition = pObject->GetPosition();

		pObject->GetActorNodePos(m_strBoneName.c_str(), vWorldPosition, E_TS_WORLD);

		SetDecalEffectData(vWorldPosition, m_fSize);
	}
}

void TDecalEffect::RenderXEffect()
{
	if (m_hTexture == R_NONE)
		return;

	vec3 vPos = m_vPos;

	RDevice* pDev = REngine::GetDevicePtr();
	_ASSERT( pDev );
	if (pDev == NULL)	return;
	if (m_vVertexArray.size() < 3 ) return;

	RMatrix matScale;
	matScale.MakeIdentity();
	matScale.SetTranslation(m_vPos);
	matScale.SetScale(m_fSize);

	// SetRender State
	pDev->SetTransform(RST_WORLD, RMatrix::IDENTITY);

	pDev->SetCullMode( RCULL_NONE );
	pDev->ShaderOff();
	pDev->SetLighting(false);
	pDev->SetFvF(RVt_pos_tex1::FVF);

	pDev->SetTextureFilter(0, RTF_LINEAR);
	pDev->SetFogEnable(false);

	pDev->SetAlphaTestEnable(false);
	pDev->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
	pDev->SetTransform(RST_WORLD, matScale);
	pDev->SetDepthBias(-0.0001f);

	pDev->SetTexture( 0, m_hTexture);
	pDev->SetTexture( 1, R_NONE);

	pDev->SetTextureStageState( 0, RTSS_COLOROP, RTOP_MODULATE);
	pDev->SetTextureStageState( 0, RTSS_COLORARG1, RTA_TEXTURE);
	pDev->SetTextureStageState( 0, RTSS_COLORARG2, RTA_TFACTOR);

	pDev->SetTextureStageState( 0, RTSS_ALPHAOP, RTOP_MODULATE);
	pDev->SetTextureStageState( 0, RTSS_ALPHAARG1, RTA_TEXTURE );
	pDev->SetTextureStageState( 0, RTSS_ALPHAARG2, RTA_TFACTOR);

	pDev->SetTextureStageState( 1, RTSS_COLOROP, RTOP_DISABLE);
	pDev->SetTextureStageState( 1, RTSS_ALPHAOP, RTOP_DISABLE);

	REngine::GetDevice().SetTextureFactor(m_dwColor);

	pDev->DrawPrimitiveUP(RPT_TRIANGLELIST, 2,
		(LPVOID)&m_vVertexArray[0], sizeof(RVt_pos_tex1));

	pDev->SetTexture( 0, R_NONE);
	pDev->SetTransform(RST_WORLD, RMatrix::IDENTITY);
	pDev->SetDepthBias(0);
}