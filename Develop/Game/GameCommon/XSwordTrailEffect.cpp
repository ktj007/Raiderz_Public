#include "stdafx.h"
#include "XSwordTrailEffect.h"
#include "XCharacter.h"
#include "REngine.h"
#include "XDef.h"
#include "MLocale.h"

#define SWORD_TRAIL_EFFECT_TENSION	0.7f
#define SWORD_TRAIL_EFFECT_GRAIN	128

//////////////////////////////////////////////////////////////////////////
// XSwordTrailEffect
XSwordTrailEffect::XSwordTrailEffect()
{
	m_hTexture			= R_NONE;
	m_nVertexCount		= 0;

	m_vecVertexBuffer.clear();
	m_vecSwordTrailStartSampling.clear();
	m_vecSwordTrailEndSampling.clear();
}

XSwordTrailEffect::~XSwordTrailEffect()
{
	DestroySwordTrailEffect();
}

bool XSwordTrailEffect::CreateSwordTrailEffect( XCharacter* pCharacter, SWORDTRAILSAMPLING_DATA* pSamplingData, tstring strSwordTrailTexture )
{
	if(pCharacter == NULL ||
		CalSwordTrailPosition(pCharacter, pSamplingData, m_vecSwordTrailStartSampling, m_vecSwordTrailEndSampling) == false)
	{
		return false;
	}

	if (m_hTexture == R_NONE)
	{
		m_hTexture = REngine::GetDevice().CreateTexture(MLocale::ConvTCHARToAnsi(strSwordTrailTexture.c_str()).c_str());
	}

	return true;
}

void XSwordTrailEffect::RenderSwordTrailEffect(DWORD dwFadeValue)
{
	// 버퍼가 비워져 있으면...
	if(m_vecVertexBuffer.empty() || m_nVertexCount < 4)
	{
		return;
	}

	// 버택스 입력
	RLVertex* pVertex = new RLVertex[m_vecVertexBuffer.size()];
	vector<RLVertex>::iterator it;
	int i;
	for(it = m_vecVertexBuffer.begin(), i = 0; it != m_vecVertexBuffer.end(); ++it, i++)
	{
		pVertex[i] = (*it);
	}

	//------------------------------------------------------------------------
	// 렌더링
	REngine::GetDevice().SetCullMode(RCULL_NONE);
	REngine::GetDevice().SetLighting(false);
	REngine::GetDevice().SetBlending(RBLEND_ONE, RBLEND_ONE);
	REngine::GetDevice().SetDepthEnable(true, false);
	REngine::GetDevice().SetTransform(RST_WORLD, RMatrix::IDENTITY);
	REngine::GetDevice().ShaderOff();
	REngine::GetDevice().SetTexture(0, m_hTexture);
	REngine::GetDevice().SetFvF(RLVertexType);

	REngine::GetDevice().SetTextureStageState(0,RTSS_COLOROP, RTOP_MODULATE);
	REngine::GetDevice().SetTextureStageState(0,RTSS_COLORARG1, RTA_TEXTURE);
	REngine::GetDevice().SetTextureStageState(0,RTSS_COLORARG2, RTA_TFACTOR);
	REngine::GetDevice().SetTextureStageState(1,RTSS_COLOROP, RTOP_DISABLE);

	REngine::GetDevice().SetTextureStageState(0,RTSS_ALPHAOP, RTOP_MODULATE);
	REngine::GetDevice().SetTextureStageState(0,RTSS_ALPHAARG1, RTA_TEXTURE);
	REngine::GetDevice().SetTextureStageState(0,RTSS_ALPHAARG2, RTA_DIFFUSE);
	REngine::GetDevice().SetTextureStageState(1,RTSS_ALPHAOP, RTOP_DISABLE);

	REngine::GetDevice().SetTextureFactor(D3DCOLOR_ARGB(dwFadeValue, dwFadeValue, dwFadeValue, dwFadeValue));

	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, m_nVertexCount - 2, (LPVOID)pVertex, sizeof(RLVertex));

	REngine::GetDevice().SetBlending(RBLEND_NONE, RBLEND_NONE);
	REngine::GetDevice().SetDepthEnable(true, true);
	REngine::GetDevice().SetCullMode(RCULL_CW);

	SAFE_DELETE_ARRAY(pVertex);
}

void XSwordTrailEffect::DestroySwordTrailEffect()
{
	m_vecVertexBuffer.clear();
	m_vecSwordTrailStartSampling.clear();
	m_vecSwordTrailEndSampling.clear();

	if (m_hTexture != R_NONE)
	{
		REngine::GetDevice().DeleteTexture(m_hTexture);
		m_hTexture = R_NONE;
	}
}

void XSwordTrailEffect::UpdateVertex( int nVectexCount, MMatrix& matWorldObject )
{
	int nStartCount = 0;
	int nEndCount = 0;
	nStartCount = nEndCount = nVectexCount;;
	int nVertexFullCount	= nStartCount + nEndCount;

	if(m_nVertexCount != nVertexFullCount &&
		nVertexFullCount >= 4)
	{
		m_nVertexCount = nVertexFullCount;

		// 버택스 버퍼 비우기
		m_vecVertexBuffer.clear();

		// 버택스 버퍼 입력
		RLVertex rTempVertex;
		DWORD dwColor = 0xffffffff;

		float fuGap = 1.0f / nStartCount;
		int nGapCnt = 0;
		int nSamplingCount = nStartCount;
		if(nStartCount > nEndCount)
			nSamplingCount = nEndCount;

		for(int i = 0; i < nSamplingCount; i++)
		{
			matWorldObject.TransformVect(m_vecSwordTrailStartSampling[i], rTempVertex.p);
			rTempVertex.color	= dwColor;
			rTempVertex.tu		= fuGap * nGapCnt;
			rTempVertex.tv		= 0.f;
			m_vecVertexBuffer.push_back(rTempVertex);

			matWorldObject.TransformVect(m_vecSwordTrailEndSampling[i], rTempVertex.p);
			rTempVertex.color	= dwColor;
			rTempVertex.tu		= fuGap * nGapCnt;
			rTempVertex.tv		= 1.0f;
			m_vecVertexBuffer.push_back(rTempVertex);
			nGapCnt++;
		}
	}
}

void XSwordTrailEffect::CalSwordTrailEffectVertexCount( int nCurrentFrame, int nStartFrame, int nEndFrame, int nFrameGap, int& nOutVertexCount )
{
	if(m_vecSwordTrailStartSampling.empty() ||
		m_vecSwordTrailEndSampling.empty())
		return;

	int nCurrentVertexCount = 0;
	int nMaxFrame = nEndFrame - nStartFrame;
	int nFrameGapCount		= nMaxFrame / nFrameGap;
	int nFrameGapOrgin		= nStartFrame + nFrameGapCount * nFrameGap;

	if(nCurrentFrame <= nFrameGapOrgin)
	{
		// 동일한 프레임 간격
		int nLimiteFrame = nFrameGapOrgin - nStartFrame;
		int nVertexSize = SWORD_TRAIL_EFFECT_GRAIN * nFrameGapCount;

		float fVertexSizeByOneFrame = (float)nVertexSize / (float)nLimiteFrame;

		nCurrentVertexCount = (int)(fVertexSizeByOneFrame * (nCurrentFrame - nStartFrame) + 0.5f);

		if(nCurrentVertexCount > nVertexSize)
			nCurrentVertexCount = nVertexSize;
	}
	else
	{
		int nLimiteFrame = nMaxFrame - nFrameGapCount * nFrameGap;

		float fVertexSizeByOneFrame = (float)SWORD_TRAIL_EFFECT_GRAIN / (float)nLimiteFrame;

		nCurrentVertexCount = (int)(fVertexSizeByOneFrame * (nCurrentFrame - nFrameGapOrgin));

		if(nCurrentVertexCount > SWORD_TRAIL_EFFECT_GRAIN)
			nCurrentVertexCount = SWORD_TRAIL_EFFECT_GRAIN;

		nCurrentVertexCount += SWORD_TRAIL_EFFECT_GRAIN * nFrameGapCount;
	}

	int nStartSamplingSize = m_vecSwordTrailStartSampling.size();
	if(nCurrentVertexCount > nStartSamplingSize)
	{
		//mlog("문제가 있다. start - %d,  end - %d,  gap - %d(%d < %d)\n", m_nStartFrame, m_nEndFrame, m_nFrameGapControl, nCurrentVertexCount, nStartSamplingSize);
		nCurrentVertexCount = nStartSamplingSize;
	}

	if(nCurrentVertexCount < 0)
		nCurrentVertexCount = 0;

	nOutVertexCount = nCurrentVertexCount;
}

bool XSwordTrailEffect::CalSwordTrailPosition( XCharacter* pCharacter, SWORDTRAILSAMPLING_DATA* pSamplingData, vector<vec3>& vecStart, vector<vec3>& vecEnd )
{
	if(pCharacter)
	{
		int nSamplingMatrixCount = pSamplingData->vecMatrix.size();
		RMatrix matStart, matEnd;

		RActorNode* pStartActorNode = pCharacter->GetActorNodeTransform(SWORDTRAIL_START_NODE_NAME, matStart, E_TS_LOCAL);
		RActorNode* pEndActorNode = pCharacter->GetActorNodeTransform(SWORDTRAIL_END_NODE_NAME, matEnd, E_TS_LOCAL);
		if(pStartActorNode && pEndActorNode)
		{
			vector<vec3> vecSwordTrailStart;
			for(vector<RMatrix>::iterator itStart = pSamplingData->vecMatrix.begin(); itStart != pSamplingData->vecMatrix.end(); itStart++)
			{
				vecSwordTrailStart.push_back((matStart * (*itStart)).GetTranslation());
			}

			MMath::CardinalInterpoltation(vecSwordTrailStart, SWORD_TRAIL_EFFECT_TENSION, SWORD_TRAIL_EFFECT_GRAIN, m_vecSwordTrailStartSampling);

			//////////////////////////////////////////////////////////////////////////
			vector<vec3> vecSwordTrailEnd;
			for(vector<RMatrix>::iterator itStart = pSamplingData->vecMatrix.begin(); itStart != pSamplingData->vecMatrix.end(); itStart++)
			{
				vecSwordTrailEnd.push_back((matEnd * (*itStart)).GetTranslation());
			}

			MMath::CardinalInterpoltation(vecSwordTrailEnd, SWORD_TRAIL_EFFECT_TENSION, SWORD_TRAIL_EFFECT_GRAIN, m_vecSwordTrailEndSampling);

			return true;
		}
	}

	return false;
}

vec3 XSwordTrailEffect::GetVertexSwordTrailSampling( int nIndex )
{
	if(nIndex < 0)
		return m_vecSwordTrailStartSampling[0];

	if(nIndex >= (int)m_vecSwordTrailStartSampling.size())
		return m_vecSwordTrailStartSampling[m_vecSwordTrailStartSampling.size() - 1];

	return m_vecSwordTrailStartSampling[nIndex];
}