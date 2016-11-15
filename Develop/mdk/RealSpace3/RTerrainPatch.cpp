#include "stdafx.h"
#include "RTerrain.h"
#include "RTerrainPatch.h"
#include "RUtil.h"
#include <assert.h>
#include <float.h>
#include "MVector2.h"
#include "MMathLinear.h"	// 최소제곱합 등 선형대수 라이브러리 헤더

#include "RDeviceD3D.h"

namespace rs3 {

	/**
	RTerrain Patch : 각 단위 Patch를 나타낸다.
	*/
	RTerrainPatch::RTerrainPatch()
		: m_pDevice(NULL)
		, m_pVertexArray(NULL)
		, m_pVertexNum(NULL)
		, m_pVertexStart(NULL)
		, m_bNeedUpdateLayerMaskTexture(true)
		, m_pTerrain( NULL )
	{
		m_iLevel = -1;
		m_iNearFlag = 0;

		m_nX = -1;
		m_nY = -1;

		m_pLayerMask[0] = m_pLayerMask[1] = NULL;

		m_iWidthCount = m_iHeightCount = m_iVertexBufferNum = 0;
		m_hVertexBuffer = R_NONE;

		m_kAABB.Initialize();
	}

	RTerrainPatch::~RTerrainPatch()
	{
		m_pDevice->DeleteVertexBuffer(m_hVertexBuffer);
		DeleteLayerMaskTexture();

		SAFE_DELETE_ARRAY(m_pVertexNum);
		SAFE_DELETE_ARRAY(m_pVertexStart);
		SAFE_DELETE_ARRAY(m_pVertexArray);
	}

	void RTerrainPatch::Init(RTerrainResource *pTerrain)
	{
		m_pTerrain = pTerrain;
	}

	void RTerrainPatch::UpdateVertexBuffer()
	{
		assert(m_hVertexBuffer==R_NONE );
		assert(m_pVertexArray != NULL);

		m_hVertexBuffer = m_pDevice->CreateVertexBuffer(m_iVertexBufferNum*sizeof(RVt_Terrain), sizeof(RVt_Terrain), m_pVertexArray);
		_ASSERT(m_hVertexBuffer);
	}


	bool RTerrainPatch::CreateVertexBuffer(int nLODLevel, int nWidth)
	{
		/**
		분명 공식이 있을 것 같은데, 공식을 유도해내지를 못하겠다. 흑, 수학적 실력 부족 -_-;
		*/
		if ( m_hVertexBuffer != R_NONE )
		{
			m_pDevice->DeleteVertexBuffer(m_hVertexBuffer);
			m_hVertexBuffer = R_NONE;
		}

		int nVertexNum = 0;
		for ( int i = 0; i<nLODLevel; ++i )
		{
			int w = (nWidth)/(int)pow(2.f,i) + 1;
			nVertexNum += ( w*w );
			if ( i != 0 )
				nVertexNum += ( w * 4 );
		}

		m_iVertexBufferNum = nVertexNum;

		SAFE_DELETE_ARRAY(m_pVertexArray);
		SAFE_DELETE_ARRAY(m_pVertexNum);
		SAFE_DELETE_ARRAY(m_pVertexStart);

		m_pVertexArray = new RVt_Terrain[nVertexNum];
		m_pVertexNum = new int[nLODLevel];
		m_pVertexStart = new int[nLODLevel];

		return true;
	}

	bool RTerrainPatch::CreateDisplacementMapFromMemory(float* const pDisplacementMap, RVector3* const pNormalMap, int nPitch, int nLODLevel, RECT *pRect,
		int nMaskWidthNum, int nMaskHeightNum )
	{
		assert(m_hVertexBuffer==R_NONE);
		assert(m_pVertexArray != NULL);
		assert(m_pTerrain != NULL );

		int nWidth = pRect->right - pRect->left;
		int nHeight = pRect->bottom - pRect->top;

		RVt_Terrain *pVertex = NULL;

		int nStartVertex;
		if ( nLODLevel == 0 )
		{
			nStartVertex = 0;
			m_iWidthCount = nWidth;
			m_iHeightCount = nHeight;
		}
		else
		{
			nStartVertex = m_pVertexStart[nLODLevel-1] + m_pVertexNum[nLODLevel-1];
		}

		int nWStep = (int)(pow(2.f,nLODLevel));
		int nHStep = (int)(pow(2.f,nLODLevel));

		pVertex = &m_pVertexArray[nStartVertex];
		{
			float fXScale = m_pTerrain->GetXScale();
			float fYScale = m_pTerrain->GetYScale();

			int iVertexNum = 0;

			std::vector<RVector> vertices;
			if ( nLODLevel == 0 )
				vertices.reserve((pRect->bottom - pRect->top+1)*(pRect->right-pRect->left+1));

			for ( int h = pRect->top; h<= pRect->bottom; h+=nHStep )
			{
				for ( int w = pRect->left; w<= pRect->right; w+=nWStep )
				{
					// TODO : (0,0) - (nWStep,nHStep) 의 최대값 또는 평균값을 취하도록 고쳐야 한다.
					int nIndex = h * nPitch + w;
					float fh = pDisplacementMap[nIndex];

					RVt_Terrain kVertex;

					kVertex.vPos.x = w*fXScale;
					kVertex.vPos.y = h*fYScale;
					kVertex.vPos.z = fh;
					kVertex.vNormal = pNormalMap[nIndex];

					kVertex.u = ( w - pRect->left )/(float)(nWidth);
					kVertex.v = ( h - pRect->top  )/(float)(nHeight);

					*pVertex = kVertex;

					++pVertex;
					++iVertexNum;

					// LOD Level 0 일때만 Bounding 정보를 만들므로, vertex를 수집한다.
					if ( nLODLevel == 0 )
						vertices.push_back(kVertex.vPos);

				} // end w
			} // end h

			// Bounding Object 생성 ( LOD Level 0 일 때만 기준으로)
			if ( nLODLevel == 0 )
			{
				_ASSERT(!vertices.empty());

				m_kAABB.vmin = vertices[0];
				m_kAABB.vmax = vertices[0];

				for ( size_t si = 1; si<vertices.size(); ++si )
					m_kAABB.Add(vertices[si]);
				
				m_kSphere.center = (m_kAABB.vmin + m_kAABB.vmax)*.5f;
				m_kSphere.radius = (m_kSphere.center - m_kAABB.vmax).Length();
#if 0	// make obb
				MMathLinear::LeastSquareSums sums;
				sums.clear();
				
				MVector3 vNormal, vCenter;
				MMathLinear::calculateLeastSquareSums(&sums, &vertices[0], (int)vertices.size());
				MMathLinear::calculateLeastSquarePlane(sums, vNormal, vCenter);

				MVector3 R = MVector3::AXISY.CrossProduct(vNormal);
				MVector3 S = vNormal.CrossProduct(MVector3::AXISX);
				R.Normalize();
				S.Normalize();
				vNormal.Normalize();

				m_kObb.vAxis[0] = R;
				m_kObb.vAxis[1] = S;
				m_kObb.vAxis[2] = vNormal;

				MVector3 vMin( FLT_MAX, FLT_MAX, FLT_MAX);
				MVector3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);

				for ( size_t si = 0; si<vertices.size(); ++si )
				{
					const MVector3 &p = vertices[si];

					float dx = m_kObb.vAxis[0].DotProduct(p);
					float dy = m_kObb.vAxis[1].DotProduct(p);
					float dz = m_kObb.vAxis[2].DotProduct(p);

					vMin.x = min(vMin.x, dx);
					vMin.y = min(vMin.y, dy);
					vMin.z = min(vMin.z, dz);

					vMax.x = max(vMax.x, dx);
					vMax.y = max(vMax.y, dy);
					vMax.z = max(vMax.z, dz);
				}

				MVector3 vHalf = (vMax + vMin)*.5f;

				m_kObb.vPosition = vHalf.x*m_kObb.vAxis[0] + vHalf.y*m_kObb.vAxis[1] + vHalf.z*m_kObb.vAxis[2];
				m_kObb.vRange = (vMax - vMin)*.5f;
#endif
			}
			else
			{
				// 이음새를 위한 Vertex를 추가한다. ( LOD 레벨 0을 제외한 ... )
				// L, R
				for ( int w = pRect->left; w<=pRect->right; w+=nWidth )
					for ( int h = pRect->top + nHStep/2; h<pRect->bottom ; h+=nHStep )
					{
						int nIndex = h * nPitch + w;
						float fh = pDisplacementMap[nIndex];

						RVt_Terrain kVertex;
						kVertex.vPos.x = w*fXScale;
						kVertex.vPos.y = h*fYScale;
						kVertex.vPos.z = fh;
						kVertex.vNormal = pNormalMap[nIndex];

						kVertex.u = ( w - pRect->left )/(float)(nWidth);
						kVertex.v = ( h - pRect->top  )/(float)(nHeight);

						*pVertex = kVertex;
						++pVertex;
						++iVertexNum;
					}

					// T, B
					for ( int h = pRect->top; h<=pRect->bottom ; h+=nHeight )
						for ( int w = pRect->left + nWStep/2; w<pRect->right; w+=nWStep )
						{
							int nIndex = h * nPitch + w;
							float fh = pDisplacementMap[nIndex];

							RVt_Terrain kVertex;
							kVertex.vPos.x = w*fXScale;
							kVertex.vPos.y = h*fYScale;
							kVertex.vPos.z = fh;
							kVertex.vNormal = pNormalMap[nIndex];

							kVertex.u = ( w - pRect->left )/(float)(nWidth);
							kVertex.v = ( h - pRect->top  )/(float)(nHeight);

							*pVertex = kVertex;
							++pVertex;
							++iVertexNum;
						}
			}

			// 이음새에 대한 정보를 넣는다.
			m_pVertexStart[nLODLevel] = nStartVertex;
			m_pVertexNum[nLODLevel] = iVertexNum;

		}
		return true;
	}

	bool RTerrainPatch::CreateLayerMaskTexture( int nMaskTextureCount, DWORD dwMaxLayerWidth, DWORD dwMaxLayerHeight)
	{
		RPFC_THISFUNC;

		if(!m_pDevice || dwMaxLayerWidth<=0 || dwMaxLayerHeight<=0)
			return false;

		RFORMAT alphaTextureFormat = RFMT_ARGB32;

		if( nMaskTextureCount <= 2)
		{
			for( int nMaskTextureIdx = 0; nMaskTextureIdx < nMaskTextureCount; nMaskTextureIdx++)
			{
				// 마스크 텍스쳐가 존재 하면 크기를 비교 해보고 기존 것을 사용할 지 새로 만들 지를 결졍 한다.
				if( m_pLayerMask[nMaskTextureIdx])
				{
					RTextureInfo info = m_pLayerMask[nMaskTextureIdx]->GetInfo();
					if( info.nTextureWidth == dwMaxLayerWidth && info.nTextureHeight == dwMaxLayerHeight)
						continue;
				}
				m_pLayerMask[nMaskTextureIdx] = m_pDevice->CreateRenderTargetTexture( dwMaxLayerWidth, dwMaxLayerHeight, alphaTextureFormat);
				_ASSERT( m_pLayerMask[nMaskTextureIdx]);
			}
		}

		return (bool)( m_pLayerMask[0] != NULL);
	}

	bool RTerrainPatch::DeleteLayerMaskTexture()
	{
		if(!m_pDevice)
			return false;

		for( int nMaskTextureIdx = 0; nMaskTextureIdx < 2; nMaskTextureIdx++)
		{
			if( m_pLayerMask[nMaskTextureIdx])
			{
				m_pDevice->DeleteTexture( m_pLayerMask[nMaskTextureIdx]);
				m_pLayerMask[nMaskTextureIdx] = NULL;
			}
		}

		NeedUpdateLayerMaskTexture(true);

		return true;
	}



	RTexture* RTerrainPatch::GetLayerMaskTexture( int nMaskTextureIdx)
	{
		if( nMaskTextureIdx < 2)
			return m_pLayerMask[nMaskTextureIdx];
		else
			return NULL;
	}
}
