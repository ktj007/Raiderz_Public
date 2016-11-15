#include "stdafx.h"

#include "RDebugPassRenderer.h"
#include "RRenderHelper.h"
#include "RFont.h"
#include "RSUtil.h"

namespace rs3 {


RDebugPassRenderer::RDebugPassRenderer()
: m_fElapsedTime(0.f)
, m_pFont(NULL)
{
	m_matTextReflectAndScale.MakeIdentity();
}

RDebugPassRenderer::~RDebugPassRenderer()
{
}

void RDebugPassRenderer::Init()
{
}

void RDebugPassRenderer::Destroy()
{
	for(std::list< SDebugPolycon >::iterator itr = m_listDebugPolycon.begin(); itr != m_listDebugPolycon.end(); ++itr)
	{
		SAFE_DELETE(itr->m_pVertex);
	}

	m_listDebugPolycon.clear();

	for (std::map<string, SDebugBox* >::iterator itr = m_mapDebugBoxes.begin(); itr != m_mapDebugBoxes.end(); ++itr )
	{
		delete itr->second;
	}
	m_mapDebugBoxes.clear();

	for ( std::map<string, SDebugLine* >::iterator itr = m_mapDebugLines.begin(); itr != m_mapDebugLines.end(); ++itr )
	{
		delete itr->second;
	}
	m_mapDebugLines.clear();

	for ( std::map<string,  SDebugSphere* >::iterator itr = m_mapDebugSpheres.begin(); itr != m_mapDebugSpheres.end(); ++itr )
	{
		delete itr->second;
	}
	m_mapDebugSpheres.clear();

	for ( std::map<string, SDebugCapsule* >::iterator itr = m_mapDebugCapsule.begin(); itr != m_mapDebugCapsule.end(); ++itr )
	{
		delete itr->second;
	}
	m_mapDebugCapsule.clear();

	for ( std::map<string, SDebugOBox* >::iterator itr = m_mapDebugOBox.begin(); itr != m_mapDebugOBox.end(); ++itr )
	{
		delete itr->second;
	}
	m_mapDebugOBox.clear();
}




void RDebugPassRenderer::RenderMStageOnPass( RCameraSceneNode* pCurrentCamera,  SCENE_SET* pRenderingList, RRENDERINGMETHOD renderMethod)
{
	RenderOnPass( pCurrentCamera, pRenderingList, renderMethod);
}

void RDebugPassRenderer::RenderOnPass( RCameraSceneNode* pCurrentCamera, SCENE_SET* pCulledList, RRENDERINGMETHOD eMothod)
{
	RRenderHelper::SetRenderState();

	for (std::list< SDebugBox >::iterator itr = m_listDebugBoxes.begin(); itr != m_listDebugBoxes.end(); )
	{
		if ( itr->m_fLifeTime <= 0 )
		{
			itr = m_listDebugBoxes.erase(itr);
		}
		else
		{
			if (itr->m_bRenderBox)
			{
				RRenderHelper::RenderBox(itr->m_matWorld, itr->m_box, itr->m_dwColor);
			}

			if (itr->m_bRenderAxis)
			{
				float fSize = 30.f;
				float fCorn = 0.5f;
				if (false == itr->m_bRenderBox)
				{
					float fRelatedSize = (itr->m_box.vmax - itr->m_box.vmin).Length();
					fCorn *= (fSize/ fRelatedSize);
					fSize = fRelatedSize;
				}

				RRenderHelper::RenderAxis(itr->m_matWorld, fSize, fCorn);
			}
			itr->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for (std::list< SDebugLine >::iterator itr = m_listDebugLines.begin(); itr != m_listDebugLines.end(); )
	{
		if ( itr->m_fLifeTime <= 0 )
		{
			itr = m_listDebugLines.erase(itr);
		}
		else
		{
			RRenderHelper::RenderLine(RMatrix::IDENTITY, itr->m_vStartPos, itr->m_vEndPos, itr->m_dwColor);
			itr->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for ( std::list< SDebugSphere >::iterator itr = m_listDebugSpheres.begin(); itr != m_listDebugSpheres.end(); )
	{
		if ( itr->m_fLifeTime <= 0.0f )
		{
			itr = m_listDebugSpheres.erase(itr);
		}
		else
		{
			RRenderHelper::RenderSphere(itr->m_matWorld, itr->m_fRadius, itr->m_dwColor);
			itr->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for ( std::list< SDebugCapsule >::iterator itr = m_listDebugCapsule.begin(); itr != m_listDebugCapsule.end(); )
	{
		if ( itr->m_fLifeTime <= 0.0f )
		{
			itr = m_listDebugCapsule.erase(itr);			
		}
		else
		{
			RRenderHelper::RenderCapsule(itr->m_Capsule, itr->m_dwColor);
			itr->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for ( std::list< SDebugOBox >::iterator itr = m_listDebugOBox.begin(); itr != m_listDebugOBox.end(); )
	{
		if ( itr->m_fLifeTime <= 0.0f )
		{
			itr = m_listDebugOBox.erase(itr);
		}
		else
		{
			RRenderHelper::RenderObb(itr->m_OBox, itr->m_dwColor);
			itr->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}


	for(std::list< SDebugPolycon >::iterator itr = m_listDebugPolycon.begin(); itr != m_listDebugPolycon.end();)
	{
		if ( itr->m_fLifeTime <= 0.0f )
		{
			SAFE_DELETE(itr->m_pVertex);
			itr = m_listDebugPolycon.erase(itr);
		}
		else
		{
			RRenderHelper::RenderTriangles(itr->m_matWorld, itr->m_pVertex, itr->m_nTriangleCount, itr->m_dwColor);
			itr->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for(std::list< SDebugPlane >::iterator itr = m_listDebugPlane.begin(); itr != m_listDebugPlane.end();)
	{
		if ( itr->m_fLifeTime <= 0.0f )
		{
			itr = m_listDebugPlane.erase(itr);
		}
		else
		{
			RRenderHelper::RenderPlane(itr->m_matWorld, itr->m_vAxisX, itr->m_vAxisY, itr->m_fSize, itr->m_fSize);
			itr->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	//////                매번 생성하는게 아닌 추적하고 싶은 정보들
	////////////////////////////////////////////////////////////////////////////////////	

	for (std::map<string, SDebugBox* >::iterator itr = m_mapDebugBoxes.begin(); itr != m_mapDebugBoxes.end(); )
	{
		if ( itr->second->m_fLifeTime <= 0 )
		{
			delete itr->second;
			itr = m_mapDebugBoxes.erase(itr);
		}
		else
		{
			if (itr->second->m_bRenderBox)
			{
				RRenderHelper::RenderBox(itr->second->m_matWorld, itr->second->m_box, itr->second->m_dwColor);
			}

			if (itr->second->m_bRenderAxis)
			{
				float fSize = 30.f;
				float fCorn = 0.5f;
				if (false == itr->second->m_bRenderBox)
				{
					float fRelatedSize = (itr->second->m_box.vmax - itr->second->m_box.vmin).Length();
					fCorn *= (fSize/ fRelatedSize);
					fSize = fRelatedSize;
				}

				RRenderHelper::RenderAxis(itr->second->m_matWorld, fSize, fCorn);
			}
			itr->second->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for ( std::map<string, SDebugLine* >::iterator itr = m_mapDebugLines.begin(); itr != m_mapDebugLines.end(); )
	{
		if ( itr->second->m_fLifeTime <= 0 )
		{
			delete itr->second;
			itr = m_mapDebugLines.erase(itr);
		}
		else
		{
			RRenderHelper::RenderLine(RMatrix::IDENTITY, itr->second->m_vStartPos, itr->second->m_vEndPos, itr->second->m_dwColor);
			itr->second->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for ( std::map<string,  SDebugSphere* >::iterator itr = m_mapDebugSpheres.begin(); itr != m_mapDebugSpheres.end(); )
	{
		if ( itr->second->m_fLifeTime <= 0.0f )
		{
			delete itr->second;
			itr = m_mapDebugSpheres.erase(itr);
		}
		else
		{
			RRenderHelper::RenderSphere(itr->second->m_matWorld, itr->second->m_fRadius, itr->second->m_dwColor);
			itr->second->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for ( std::map<string, SDebugCapsule* >::iterator itr = m_mapDebugCapsule.begin(); itr != m_mapDebugCapsule.end(); )
	{
		if ( itr->second->m_fLifeTime <= 0.0f )
		{
			delete itr->second;
			itr = m_mapDebugCapsule.erase(itr);
		}
		else
		{
			RRenderHelper::RenderCapsule(itr->second->m_Capsule, itr->second->m_dwColor);
			itr->second->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}

	for ( std::map<string, SDebugOBox* >::iterator itr = m_mapDebugOBox.begin(); itr != m_mapDebugOBox.end(); )
	{
		if ( itr->second->m_fLifeTime <= 0.0f )
		{
			delete itr->second;
			itr = m_mapDebugOBox.erase(itr);
		}
		else
		{
			RRenderHelper::RenderObb(itr->second->m_OBox, itr->second->m_dwColor);
			itr->second->m_fLifeTime -= m_fElapsedTime;
			++itr;
		}
	}


	RRenderHelper::EndRenderState();

	// debug font
	RDevice* pDevice = REngine::GetDevicePtr();
	RFont* pFont = m_pFont;
	if ( !pFont || !pDevice || !pCurrentCamera )
	{
		if (!m_listDebugText.empty())
			m_listDebugText.clear();
	}
	else
	{
		RVector2 vScreen;
		for(std::list< SDebugText >::iterator itr = m_listDebugText.begin(); itr != m_listDebugText.end();)
		{
			if ( itr->m_fLifeTime <= 0.0f )
			{
				itr = m_listDebugText.erase(itr);
			}
			else
			{
				GetScreenFromWorld(&vScreen, &itr->m_vPos, pDevice);
				if (rs3::RFont::BeginFont( ) )
				{
					pFont->DrawText_UNICODE(vScreen.x, vScreen.y, MLocale::ConvAnsiToUTF16(itr->m_strText.c_str()).c_str(), -1, itr->m_dwColor);
					rs3::RFont::EndFont();
				}

				itr->m_fLifeTime -= m_fElapsedTime;
				++itr;
			}
		}
	}

	m_pFont = NULL;
}

void RDebugPassRenderer::Update(float fTimeDelta)
{
	m_fElapsedTime = fTimeDelta;
}

void RDebugPassRenderer::AddDebugBox(const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor)
{
	m_listDebugBoxes.push_back( SDebugBox(_refMatWorld, _fBoxSize, _fLifeTime, _dwColor, false) );
}

void RDebugPassRenderer::AddDebugBox(const RMatrix& _refMatWorld, RBoundingBox& bbox, float _fLifeTime, DWORD _dwColor)
{
	m_listDebugBoxes.push_back( SDebugBox(_refMatWorld, bbox, _fLifeTime, _dwColor, false) );
}

void RDebugPassRenderer::AddDebugBoxAndAxis(const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor)
{
	m_listDebugBoxes.push_back( SDebugBox(_refMatWorld, _fBoxSize, _fLifeTime, _dwColor, true) );
}

void RDebugPassRenderer::AddDebugAxis(const RMatrix& _refMatWorld, float _fAxisSize, float _fLifeTime, DWORD _dwColor)
{
	m_listDebugBoxes.push_back( SDebugBox(_refMatWorld, _fAxisSize, _fLifeTime, _dwColor, true, false) );
}

void RDebugPassRenderer::AddDebugSphere(const RMatrix& _refMatWorld, float _fRadius, float _fLifeTime, DWORD _dwColor)
{
	m_listDebugSpheres.push_back( SDebugSphere(_refMatWorld, _fRadius, _fLifeTime, _dwColor) );
}

void RDebugPassRenderer::AddDebugLine( const RVector& _refVecStart, const RVector& _refVecEnd, float _fLifeTime, DWORD _dwColor )
{
	m_listDebugLines.push_back( SDebugLine( _refVecStart, _refVecEnd, _fLifeTime, _dwColor ) );
}

void RDebugPassRenderer::AddDebugCapsule(MCapsule &_capsule, float _fLifeTime, DWORD _dwColor)
{
	m_listDebugCapsule.push_back( SDebugCapsule(_capsule, _fLifeTime, _dwColor) );
}

void RDebugPassRenderer::AddDebugOBox(MOBox &_obox, float _fLifeTime, DWORD _dwColor)
{
	m_listDebugOBox.push_back( SDebugOBox(_obox, _fLifeTime, _dwColor) );
}

void RDebugPassRenderer::AddDebugPolycon( const RMatrix& _refMatWorld, RVector* pVector, int nVertexCount, float _fLifeTime, DWORD _dwColor /*= 0xffffffff*/ )
{
	m_listDebugPolycon.push_back(SDebugPolycon(_refMatWorld, pVector, nVertexCount, _fLifeTime, _dwColor));
}

void RDebugPassRenderer::AddDebugPlane( const RMatrix& _refMatWorld, MPlane& _refPlane, float fSize, float _fLifeTime, DWORD _dwColor /*= 0xffffffff*/ )
{
	m_listDebugPlane.push_back(SDebugPlane(_refMatWorld, _refPlane, fSize, _fLifeTime, _dwColor));
}

void RDebugPassRenderer::AddDebugText( const RVector& vPos, const char* pSzText, float _fLifeTime, DWORD _dwColor /*= 0xffffffff*/ )
{
	m_listDebugText.push_back( SDebugText(vPos, pSzText, _fLifeTime, _dwColor) );
}

////////////////////////////////////////////////////////////////////////////////////
//////                매번 생성하는게 아닌 추적하고 싶은 정보들
////////////////////////////////////////////////////////////////////////////////////	

void RDebugPassRenderer::ShowDebugBox(const string& strName, const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor)
{
	map<string, SDebugBox*>::iterator itr = m_mapDebugBoxes.find(strName);
	if (itr == m_mapDebugBoxes.end())	// 없으니 추가
	{
		m_mapDebugBoxes.insert(make_pair(strName, new SDebugBox(_refMatWorld, _fBoxSize, _fLifeTime, _dwColor, false) ));	
	}
	else	// 정보 갱신
	{
		*itr->second = SDebugBox(_refMatWorld, _fBoxSize, _fLifeTime, _dwColor, false);
	}	
}

void RDebugPassRenderer::ShowDebugBox(const string& strName, const RMatrix& _refMatWorld, RBoundingBox& bbox, float _fLifeTime, DWORD _dwColor)
{
	map<string, SDebugBox*>::iterator itr = m_mapDebugBoxes.find(strName);
	if (itr == m_mapDebugBoxes.end())	// 없으니 추가
	{
		m_mapDebugBoxes.insert(make_pair(strName, new SDebugBox(_refMatWorld, bbox, _fLifeTime, _dwColor, false) ));	
	}
	else	// 정보 갱신
	{
		*itr->second = SDebugBox(_refMatWorld, bbox, _fLifeTime, _dwColor, false);
	}
}

void RDebugPassRenderer::ShowDebugBoxAndAxis(const string& strName, const RMatrix& _refMatWorld, float _fBoxSize, float _fLifeTime, DWORD _dwColor)
{
	map<string, SDebugBox*>::iterator itr = m_mapDebugBoxes.find(strName);
	if (itr == m_mapDebugBoxes.end())	// 없으니 추가
	{
		m_mapDebugBoxes.insert(make_pair(strName, new SDebugBox(_refMatWorld, _fBoxSize, _fLifeTime, _dwColor, true) ));	
	}
	else	// 정보 갱신
	{
		*itr->second = SDebugBox(_refMatWorld, _fBoxSize, _fLifeTime, _dwColor, true);
	}	
}

void RDebugPassRenderer::ShowDebugAxis(const string& strName, const RMatrix& _refMatWorld, float _fAxisSize, float _fLifeTime, DWORD _dwColor)
{
	map<string, SDebugBox*>::iterator itr = m_mapDebugBoxes.find(strName);
	if (itr == m_mapDebugBoxes.end())	// 없으니 추가
	{
		m_mapDebugBoxes.insert(make_pair(strName, new SDebugBox(_refMatWorld, _fAxisSize, _fLifeTime, _dwColor, true, false) ));	
	}
	else	// 정보 갱신
	{
		*itr->second = SDebugBox(_refMatWorld, _fAxisSize, _fLifeTime, _dwColor, true, false);
	}	
}

void RDebugPassRenderer::ShowDebugSphere(const string& strName, const RMatrix& _refMatWorld, float _fRadius, float _fLifeTime, DWORD _dwColor)
{
	map<string, SDebugSphere*>::iterator itr = m_mapDebugSpheres.find(strName);
	if (itr == m_mapDebugSpheres.end())	// 없으니 추가
	{
		m_mapDebugSpheres.insert(make_pair(strName, new SDebugSphere(_refMatWorld, _fRadius, _fLifeTime, _dwColor) ));	
	}
	else	// 정보 갱신
	{
		*itr->second = SDebugSphere(_refMatWorld, _fRadius, _fLifeTime, _dwColor);
	}	
}

void RDebugPassRenderer::ShowDebugLine(const string& strName,  const RVector& _refVecStart, const RVector& _refVecEnd, float _fLifeTime, DWORD _dwColor )
{
	map<string, SDebugLine*>::iterator itr = m_mapDebugLines.find(strName);
	if (itr == m_mapDebugLines.end())	// 없으니 추가
	{
		m_mapDebugLines.insert(make_pair(strName, new SDebugLine( _refVecStart, _refVecEnd, _fLifeTime, _dwColor ) ));	
	}
	else	// 정보 갱신
	{
		*itr->second = SDebugLine( _refVecStart, _refVecEnd, _fLifeTime, _dwColor );
	}
}

void RDebugPassRenderer::ShowDebugCapsule(const string& strName, MCapsule &_capsule, float _fLifeTime, DWORD _dwColor)
{
	map<string, SDebugCapsule*>::iterator itr = m_mapDebugCapsule.find(strName);
	if (itr == m_mapDebugCapsule.end())	// 없으니 추가
	{
		m_mapDebugCapsule.insert(make_pair(strName, new SDebugCapsule(_capsule, _fLifeTime, _dwColor)) );	
	}
	else	// 정보 갱신
	{
		*itr->second = SDebugCapsule(_capsule, _fLifeTime, _dwColor);
	}	
}

void RDebugPassRenderer::ShowDebugOBox(const string& strName, MOBox &_obox, float _fLifeTime, DWORD _dwColor)
{
	map<string, SDebugOBox*>::iterator itr = m_mapDebugOBox.find(strName);
	if (itr == m_mapDebugOBox.end())	// 없으니 추가
	{
		m_mapDebugOBox.insert(make_pair(strName, new SDebugOBox(_obox, _fLifeTime, _dwColor) ));	
	}
	else	// 정보 갱신
	{
		*itr->second = SDebugOBox(_obox, _fLifeTime, _dwColor);
	}
}


}
