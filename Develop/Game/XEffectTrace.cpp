#include "stdafx.h"
#include "XEffectTrace.h"
#include "XEngine.h"

XEffectTrace::XEffectTrace() : m_hTexture(R_NONE)
{
	m_fTraceMaxLength		= 0.0f;
}

void XEffectTrace::Create(vec3 vStartPos, float fTraceMaxLength )
{
	m_vecTraceMovePos.push_back(vStartPos);

	m_vStartPos = vStartPos;
	m_fTraceMaxLength = fTraceMaxLength;

	if (m_hTexture == R_NONE)
	{
		m_hTexture = global.engine->CreateTexture(FILENAME_EFFECT_TRACE_PARTICLE);
	}
}

void XEffectTrace::Destroy()
{
	if (m_hTexture != R_NONE)
	{
		global.engine->DeleteTexture(m_hTexture);
		m_hTexture = R_NONE;
	}
}

void XEffectTrace::Update(vec3& vProjectilePos)
{
	m_vecHorizonVertexBuffer.clear();
	m_vecVerticalVertexBuffer.clear();

	if (m_fTraceMaxLength <= 0) return;

	m_vecTraceMovePos.push_back(vProjectilePos);


	UpdateHorizonPolygon();
	UpdateVerticalPolygon();
}

void XEffectTrace::Render()
{
	if (m_fTraceMaxLength <= 0.0f) return;
	if(m_vecHorizonVertexBuffer.empty()) return;
	if(m_vecVerticalVertexBuffer.empty()) return;

	RDevice* pDev = REngine::GetDevicePtr();
	_ASSERT( pDev );

	// 버택스 입력
	RLVertex* pHorizonVertex = new RLVertex[m_vecHorizonVertexBuffer.size()];
	RLVertex* pVerticalVertex = new RLVertex[m_vecVerticalVertexBuffer.size()];
	vector<RLVertex>::iterator it;
	int i;
	for(it = m_vecHorizonVertexBuffer.begin(), i = 0; it != m_vecHorizonVertexBuffer.end(); ++it, i++)
	{
		pHorizonVertex[i] = (*it);
	}
	for(it = m_vecVerticalVertexBuffer.begin(), i = 0; it != m_vecVerticalVertexBuffer.end(); ++it, i++)
	{
		pVerticalVertex[i] = (*it);
	}

	pDev->SetTransform(RST_WORLD,RMatrix::IDENTITY);

	pDev->SetLighting(false);
	pDev->SetFvF(RLVertexType);

	pDev->SetBlending( RBLEND_ONE, RBLEND_ONE);
	pDev->SetDepthEnable( true, false);

	pDev->SetCullMode(RCULL_NONE);

	pDev->ShaderOff();
	pDev->SetTexture( 0, m_hTexture);

	pDev->SetTextureStageState( 0,RTSS_COLOROP, RTOP_MODULATE );
	pDev->SetTextureStageState( 0,RTSS_COLORARG1, RTA_TEXTURE );
	pDev->SetTextureStageState( 0,RTSS_COLORARG2, RTA_DIFFUSE );
	pDev->SetTextureStageState( 1,RTSS_COLOROP, RTOP_DISABLE );

	pDev->SetTextureStageState( 0,RTSS_ALPHAOP, RTOP_MODULATE );
	pDev->SetTextureStageState( 0,RTSS_ALPHAARG1, RTA_TEXTURE );
	pDev->SetTextureStageState( 0,RTSS_ALPHAARG2, RTA_DIFFUSE );
	pDev->SetTextureStageState( 1,RTSS_ALPHAOP, RTOP_DISABLE );

	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, m_vecHorizonVertexBuffer.size() - 2, (LPVOID)pHorizonVertex, sizeof(RLVertex));
	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, m_vecVerticalVertexBuffer.size() - 2, (LPVOID)pVerticalVertex, sizeof(RLVertex));

	pDev->SetTexture( 0, R_NONE);

	pDev->SetBlending( RBLEND_NONE, RBLEND_NONE);
	pDev->SetDepthEnable( true, true);
	pDev->SetCullMode(RCULL_CW);

	SAFE_DELETE(pHorizonVertex);
	SAFE_DELETE(pVerticalVertex);
}

void XEffectTrace::UpdateHorizonPolygon()
{
	vec3 vPre = m_vStartPos;
	vec3 vCurr = m_vStartPos;
	vec3 vXAxis(vec3::AXISY);
	int nCount = 0;
	float fuGap = 1.0f / (m_vecTraceMovePos.size() - 1);
	for(vector<vec3>::iterator it = m_vecTraceMovePos.begin(); it != m_vecTraceMovePos.end(); ++it)
	{
		vCurr = *it;
		if(vPre != vCurr)
		{
			vec3 dir			= (*it - vPre).Normalize();
			vXAxis				= -dir;
			RVector vYAxis2(0.0f, 0.0f, 2.0f);
			RVector vYAxis		= vXAxis.CrossProduct(vYAxis2);
			vYAxis.Normalize();
			vYAxis *= 2.0f;

			RLVertex stVertex;
			// 1 : left-top
			stVertex.p = vPre;
			stVertex.p += vYAxis;//( -vXAxis + vYAxis );
			stVertex.color = 0xffffffff;
			stVertex.tu = fuGap * nCount;
			stVertex.tv = 0.f;
			m_vecHorizonVertexBuffer.push_back(stVertex);

			// 2 : left-bottom
			stVertex.p = vPre;
			stVertex.p += (-vYAxis);//( -vXAxis - vYAxis );
			stVertex.color = 0xffffffff;
			stVertex.tu = fuGap * nCount;
			stVertex.tv = 1.f;
			m_vecHorizonVertexBuffer.push_back(stVertex);

			++nCount;
		}

		vPre = vCurr;
	}

	// 마지막 마무리
	RVector vYAxis2(0.0f, 0.0f, 2.0f);
	RVector vYAxis		= vXAxis.CrossProduct(vYAxis2);
	vYAxis.Normalize();

	RLVertex stVertex;
	// 1 : left-top
	stVertex.p = vPre;
	stVertex.p += vYAxis;//( -vXAxis + vYAxis );
	stVertex.color = 0xffffffff;
	stVertex.tu = fuGap * nCount;
	stVertex.tv = 0.f;
	m_vecHorizonVertexBuffer.push_back(stVertex);

	// 2 : left-bottom
	stVertex.p = vPre;
	stVertex.p += (-vYAxis);//( -vXAxis - vYAxis );
	stVertex.color = 0xffffffff;
	stVertex.tu = fuGap * nCount;
	stVertex.tv = 1.f;
	m_vecHorizonVertexBuffer.push_back(stVertex);


}

void XEffectTrace::UpdateVerticalPolygon()
{
	vec3 vPre = m_vStartPos;
	vec3 vCurr = m_vStartPos;
	vec3 vXAxis(vec3::AXISY);
	int nCount = 0;
	float fuGap = 1.0f / (m_vecTraceMovePos.size() - 1);
	for(vector<vec3>::iterator it = m_vecTraceMovePos.begin(); it != m_vecTraceMovePos.end(); ++it)
	{
		vCurr = *it;
		if(vPre != vCurr)
		{
			vec3 dir			= (*it - vPre).Normalize();
			vXAxis				= -dir;
			RVector vYAxis2(0.0f, 0.0f, 2.0f);
			vYAxis2.Normalize();
			vYAxis2 *= 2.0f;

			RLVertex stVertex;
			// 1 : left-top
			stVertex.p = vPre;
			stVertex.p += vYAxis2;//( -vXAxis + vYAxis );
			stVertex.color = 0xffffffff;
			stVertex.tu = fuGap * nCount;
			stVertex.tv = 0.f;
			m_vecVerticalVertexBuffer.push_back(stVertex);

			// 2 : left-bottom
			stVertex.p = vPre;
			stVertex.p += (-vYAxis2);//( -vXAxis - vYAxis );
			stVertex.color = 0xffffffff;
			stVertex.tu = fuGap * nCount;
			stVertex.tv = 1.f;
			m_vecVerticalVertexBuffer.push_back(stVertex);

			++nCount;
		}

		vPre = vCurr;
	}

	// 마지막 마무리
	RVector vYAxis2(0.0f, 0.0f, 2.0f);
	RVector vYAxis		= vXAxis.CrossProduct(vYAxis2);
	vYAxis.Normalize();

	RLVertex stVertex;
	// 1 : left-top
	stVertex.p = vPre;
	stVertex.p += vYAxis2;//( -vXAxis + vYAxis );
	stVertex.color = 0xffffffff;
	stVertex.tu = fuGap * nCount;
	stVertex.tv = 0.f;
	m_vecVerticalVertexBuffer.push_back(stVertex);

	// 2 : left-bottom
	stVertex.p = vPre;
	stVertex.p += (-vYAxis2);//( -vXAxis - vYAxis );
	stVertex.color = 0xffffffff;
	stVertex.tu = fuGap * nCount;
	stVertex.tv = 1.f;
	m_vecVerticalVertexBuffer.push_back(stVertex);


}
