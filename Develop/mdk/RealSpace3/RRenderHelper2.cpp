#include "stdafx.h"
#include "MMath.h"
#include "RDevice.h"
#include "RTexture.h"
#include "RRenderHelper2.h"
#include "RVType.h"

namespace rs3 {
namespace RenderHelper {

//////////////////////////////////////////////////////////////////////////
// RHelperMesh

void RHelperMesh::SetRenderState(RDevice *pDevice, bool bRenderWire /* = false */)
{
	pDevice->SetTexture(0, R_NONE);
	pDevice->SetTexture(1, R_NONE);
	pDevice->SetLighting(false);
	pDevice->ShaderOff();

	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(0 ,RTSS_COLORARG1, RTA_TFACTOR);
	pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_TFACTOR);
	pDevice->SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
	pDevice->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);
}

void RHelperMesh::ResetRenderState(RDevice *pDevice)
{
	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_MODULATE);
	pDevice->SetTextureStageState(0 ,RTSS_COLORARG1, RTA_DIFFUSE);
	pDevice->SetTextureStageState(0 ,RTSS_COLORARG2, RTA_TEXTURE);
	pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_MODULATE);
	pDevice->SetTextureStageState(0, RTSS_ALPHAARG1, RTA_DIFFUSE);
	pDevice->SetTextureStageState(0, RTSS_ALPHAARG2, RTA_TEXTURE);

	pDevice->SetTextureStageState(1, RTSS_COLOROP, RTOP_DISABLE);
	pDevice->SetTextureStageState(1, RTSS_ALPHAOP, RTOP_DISABLE);
}

void RHelperMesh::Draw(RDevice *pDevice, const RMatrix &matWorld, DWORD dwColor)
{
	pDevice->SetTransform(RST_WORLD, matWorld);
	pDevice->SetTextureFactor(dwColor);

	pDevice->SetFvF(D3DFVF_XYZ);
	pDevice->DrawIndexedPrimitiveUP(RPT_TRIANGLELIST, 0, m_kVertices.size(), m_kIndices.size()/3,(void*)&m_kIndices[0], (void*)&m_kVertices[0], sizeof(RVector3));
}


//////////////////////////////////////////////////////////////////////////
// RSphere3D ( from : http://www.codeproject.com/KB/WPF/XamlUVSphere.aspx )

RSphere3D::RSphere3D(float radius, int seperators)
: m_fRadius(radius), m_nSeparators(seperators)
{
	CreateGeomerty();
}

void RSphere3D::CreateGeomerty()
{
	int e;
	float segmentRad = MMath::PI / 2 / (m_nSeparators + 1);
	int numberOfSeparators = 4 * m_nSeparators + 4;

	m_kVertices.clear();
	m_kIndices.clear();

	for (e = -m_nSeparators; e <= m_nSeparators; e++)
	{
		float r_e = m_fRadius * cos(segmentRad*e);
		float y_e = m_fRadius * sin(segmentRad*e);

		for (int s = 0; s <= (numberOfSeparators-1); s++)
		{
			float z_s = r_e * sin(segmentRad*s) * (-1);
			float x_s = r_e * cos(segmentRad*s);

			m_kVertices.push_back(RVector3(x_s, y_e, z_s));
		}
	}

	m_kVertices.push_back(RVector3(0, m_fRadius, 0));
	m_kVertices.push_back(RVector3(0, -1*m_fRadius, 0));

	for (e = 0; e < 2 * m_nSeparators; e++)
	{
		for (int i = 0; i < numberOfSeparators; i++)
		{
			m_kIndices.push_back(e * numberOfSeparators + i);
			m_kIndices.push_back(e * numberOfSeparators + i + 
				numberOfSeparators);
			m_kIndices.push_back(e * numberOfSeparators + (i + 1) % 
				numberOfSeparators + numberOfSeparators);

			m_kIndices.push_back(e * numberOfSeparators + (i + 1) % 
				numberOfSeparators + numberOfSeparators);
			m_kIndices.push_back(e * numberOfSeparators + 
				(i + 1) % numberOfSeparators);
			m_kIndices.push_back(e * numberOfSeparators + i);
		}
	}

	for (int i = 0; i < numberOfSeparators; i++)
	{
		m_kIndices.push_back(e * numberOfSeparators + i);
		m_kIndices.push_back(e * numberOfSeparators + (i + 1) % 
			numberOfSeparators);
		m_kIndices.push_back(numberOfSeparators * (2 * m_nSeparators + 1));
	}

	for (int i = 0; i < numberOfSeparators; i++)
	{
		m_kIndices.push_back(i);
		m_kIndices.push_back((i + 1) % numberOfSeparators);
		m_kIndices.push_back(numberOfSeparators * (2 * m_nSeparators + 1) + 1);
	}
}

////////////////////////////////////////////////////////////////////////// Functions
void DrawIcon3Ds(RDevice *pDevice, const RVector3 *pPositions, int nCount, float width, float height, RTexture *pTexture, DWORD dwColor)
{
	if ( nCount == 0 )
		return;

	RMatrix matView = pDevice->GetTransform(RST_VIEW);

	RVector vBaseXAxis( matView._11, matView._21, matView._31);
	RVector vBaseYAxis( matView._12, matView._22, matView._32);

	float w = width/2;
	float h = height/2;

	vBaseXAxis.FastNormalize();
	vBaseYAxis.FastNormalize();

	vBaseXAxis *= w;
	vBaseYAxis *= h;

	std::vector<RVt_pos_tex1> vertices;
	std::vector<WORD> indices;

	vertices.resize(4*nCount);
	indices.resize(6*nCount);

	for ( int i = 0; i<nCount; ++i )
	{
		int ix = i * 4;
		const RVector &vPos = pPositions[i];

		vertices[ix].vPos = vPos - vBaseXAxis + vBaseYAxis;
		vertices[ix].u = 0.0f;	vertices[ix].v = 0.0f;
		++ix;

		vertices[ix].vPos = vPos + vBaseXAxis + vBaseYAxis;
		vertices[ix].u = 1.0f;	vertices[ix].v = 0.0f;
		++ix;

		vertices[ix].vPos = vPos - vBaseXAxis - vBaseYAxis;
		vertices[ix].u = 0.0f;	vertices[ix].v = 1.0f;
		++ix;

		vertices[ix].vPos = vPos + vBaseXAxis - vBaseYAxis;
		vertices[ix].u = 1.0f;	vertices[ix].v = 1.0f;
	}

	for ( int i = 0; i<nCount; ++i )
	{
		int ix = i * 6;
		int vt = i * 4;
		indices[ix++] = vt;
		indices[ix++] = vt+1;
		indices[ix++] = vt+2;
		indices[ix++] = vt+2;
		indices[ix++] = vt+1;
		indices[ix] = vt+3;
	}

	pDevice->SetTextureStageState(0 ,RTSS_COLORARG2, RTA_TEXTURE);
	pDevice->SetTextureStageState(0, RTSS_ALPHAARG2, RTA_TEXTURE);
	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_MODULATE);
	pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_MODULATE);
	pDevice->SetTextureFactor(dwColor);

	pDevice->SetTransform(RST_WORLD, RMatrix::IDENTITY);
	pDevice->SetFvF(RVt_pos_tex1::FVF);
	pDevice->SetTexture(0, pTexture);
	pDevice->DrawIndexedPrimitiveUP(RPT_TRIANGLELIST, 0, vertices.size(), nCount*2,static_cast<void*>(&indices[0]),static_cast<void*>(&vertices[0]), sizeof(RVt_pos_tex1), RFMT_INDEX16);

	pDevice->SetTextureStageState(0, RTSS_COLOROP, RTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, RTSS_ALPHAOP, RTOP_SELECTARG1);
}

};	// RenderHelper namepsace

};	// re3 namespace
