#include "stdafx.h"
#include "XDamageCaptionDraw.h"
#include "RCameraSceneNode.h"
#include "RSUtil.h"

//////////////////////////////////////////////////////////////////////////
// 텍스쳐 번호 순서
// 0 1 2 3 4 
// 5 6 7 8 9
// + 
//
//////////////////////////////////////////////////////////////////////////

#define TEX_WIDTH_COUNT 5.0f;
#define TEX_HEIGHT_COUNT 4.0f;

XDamageCaptionDraw::XDamageCaptionDraw()
: m_hTexture(R_NONE)
, m_bLink(false)
, m_nTexWidth(0)
, m_nTexHeight(0)
, m_fNumTexWidthGap(0.0f)
, m_fNumTexHeightGap(0.0f)
, m_nCaptionSize(1)
{

}

XDamageCaptionDraw::~XDamageCaptionDraw()
{

}

bool XDamageCaptionDraw::Create( wstring& strFileName, int nCaptionSize )
{
	m_hTexture = REngine::GetDevice().CreateTexture(MLocale::ConvUTF16ToAnsi(strFileName.c_str()).c_str());

	if(!m_hTexture)
		return false;

	m_nTexWidth = m_hTexture->GetInfo().nTextureWidth;
	m_nTexHeight = m_hTexture->GetInfo().nTextureHeight;

	m_fNumTexWidthGap = (float)m_nTexWidth / TEX_WIDTH_COUNT;
	m_fNumTexHeightGap = (float)m_nTexHeight / TEX_HEIGHT_COUNT;

	m_nCaptionSize = nCaptionSize;

	return true;
}

void XDamageCaptionDraw::Destroy()
{
	if (m_hTexture != R_NONE)
	{
		REngine::GetDevice().DeleteTexture(m_hTexture);
		m_hTexture = R_NONE;
	}
}

void XDamageCaptionDraw::Init()
{
	m_vecVertexBuffer.clear();
}

void XDamageCaptionDraw::SetDamageCaption( RMatrix& matRender, wstring& strText, DWORD dwColor /*= D3DCOLOR_XRGB(255,255,255)*/ )
{
	m_bLink = true;

	int nCount = strText.size();

	float fHalfCount = (float)nCount / TEX_HEIGHT_COUNT;
	int nOddNum = nCount % 2;
	float fAddHalf = 0.0f;
	if(nOddNum > 0)
		fAddHalf = (float)m_nCaptionSize / TEX_HEIGHT_COUNT;

	//float fHalfWidth = ((fHalfCount * m_nCaptionSize) + fAddHalf) * -2.0f;
	float fHalfWidth	= -( ( float )( m_nCaptionSize * nCount ) * 0.5f ); //+ ( nOddNum * m_nCaptionSize * 0.5f ) );
	//float fHalfHeight = (float)m_nCaptionSize / TEX_HEIGHT_COUNT;
	float fHalfHeight = ( float )m_nCaptionSize * 0.5f;

	for(int i = 0; i < nCount; ++i)
	{
		int nNum = 0;

		wchar_t chNum = strText[i];
		if(strText[i] == L'+')
		{
			// 숫자 이외의 문자면...
			nNum = 10;
		}
		else
		{
			nNum = _wtoi(&chNum);
		}

		float fx = fHalfWidth + (m_nCaptionSize * i);
		float fy = -fHalfHeight;

		SetDrawInfo(fx, fy, m_nCaptionSize, m_nCaptionSize, nNum, matRender, dwColor);
	}
}

void XDamageCaptionDraw::SetDrawInfo( float fx, float fy, float fWidth, float fHeight, int nIndex, RMatrix& matRender, DWORD& dwColor )
{
	vec3 v[4] = {0,};

	v[0].x = fx;
	v[0].y = fy;
	v[0].z = 0.0f;

	v[1].x = fx ;
	v[1].y = fy + fHeight;
	v[1].z = 0.0f;

	v[2].x = fx + fWidth;
	v[2].y = fy;
	v[2].z = 0.0f;

	v[3].x = fx + fWidth;
	v[3].y = fy + fHeight;
	v[3].z = 0.0f;
	
	// 버택스 버퍼 입력
	RLVertex rTempVertex;

	for(int i = 0; i < 4; i++)
	{
		matRender.TransformVect(v[i], rTempVertex.p);
		rTempVertex.color	= dwColor;
		GetTextureIndex(nIndex, i, rTempVertex.tu, rTempVertex.tv);

		if(m_bLink)
		{
			if(SetLinkVertex())
				m_vecVertexBuffer.push_back(rTempVertex);

			m_bLink = false;
		}

		m_vecVertexBuffer.push_back(rTempVertex);
	}
}

void XDamageCaptionDraw::GetTextureIndex( int nTextureIndex, int nVertexIndex, float& fu, float& fv )
{
	int nLeftIndex = nTextureIndex % 5;
	int nDownIndex = nTextureIndex / 5;

	switch(nVertexIndex)
	{
	case 0:
		{
			fu = nLeftIndex * m_fNumTexWidthGap;
			fv = nDownIndex * m_fNumTexHeightGap;
		}
		break;
	case 1:
		{
			fu = nLeftIndex * m_fNumTexWidthGap;
			fv = (nDownIndex + 1) * m_fNumTexHeightGap;
		}
		break;
	case 2:
		{
			fu = (nLeftIndex + 1) * m_fNumTexWidthGap;
			fv = nDownIndex * m_fNumTexHeightGap;
		}
		break;
	case 3:
		{
			fu = (nLeftIndex + 1) * m_fNumTexWidthGap;
			fv = (nDownIndex + 1) * m_fNumTexHeightGap;
		}
		break;
	}

	if(fu != 0.0f)
		fu /= (float)m_nTexWidth;

	if(fv != 0.0f)
		fv /= (float)m_nTexHeight;
}

struct customVERTEX			{ FLOAT x, y, z, rhw;		DWORD color;		FLOAT tu, tv;				};
void XDamageCaptionDraw::Render()
{
	if(m_vecVertexBuffer.empty())
		return;


	RCameraSceneNode *pCamera = global.smgr->GetPrimaryCamera();
	if ( pCamera == NULL)	return;

	// 버택스 입력
	customVERTEX* pVertex = new customVERTEX[m_vecVertexBuffer.size()];
	vector<RLVertex>::iterator it;
	int i;
	for(it = m_vecVertexBuffer.begin(), i = 0; it != m_vecVertexBuffer.end(); ++it, i++)
	{
		RVector2 vScreen( 0, 0);
		rs3::GetScreenFromWorld( &vScreen, &(it->p), &REngine::GetDevice().GetViewport(), &RMatrix::IDENTITY, &pCamera->GetViewMatrix(), &pCamera->GetProjectionMatrix());

		pVertex[i].x = vScreen.x;
		pVertex[i].y = vScreen.y;
		pVertex[i].z = 0;

		pVertex[i].rhw = 1.0f;
		
		pVertex[i].color = it->color;
		
		pVertex[i].tu = it->tu;
		pVertex[i].tv = it->tv;
	}

	REngine::GetDevice().ShaderOff();
	REngine::GetDevice().SetCullMode(RCULL_NONE);
	REngine::GetDevice().SetFvF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE|D3DFVF_TEX1);
	REngine::GetDevice().SetLighting(false);
	REngine::GetDevice().SetTransform(RST_WORLD, RMatrix::IDENTITY);

	REngine::GetDevice().SetTexture(0, m_hTexture);
	REngine::GetDevice().DrawPrimitiveUP(RPT_TRIANGLESTRIP, m_vecVertexBuffer.size() - 2, (LPVOID)pVertex, sizeof(customVERTEX));
	
	SAFE_DELETE_ARRAY(pVertex);
}

bool XDamageCaptionDraw::SetLinkVertex()
{
	if(m_vecVertexBuffer.empty())
		return false;

	int nLastIndex = m_vecVertexBuffer.size() - 1;
	m_vecVertexBuffer.push_back(m_vecVertexBuffer[nLastIndex]);

	return true;
}
