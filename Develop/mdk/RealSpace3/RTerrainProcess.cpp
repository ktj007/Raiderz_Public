#include "stdafx.h"
#include "RTerrain.h"
#include "RTerrainPatch.h"
#include "RDeviceD3D.h"
#include "RProfileInfo.h"

#include "REnvironmentSet.h"
#include "RCameraSceneNode.h"
#include "RShadowStrategy.h"
#include "RTerrainShader.h"
#include "RShaderFX.h"

#include "RShaderCompositeManager.h"
#include "RUtil.h"
#include "RTerrainPassRenderer.h"
#include "RRenderHelper.h"



namespace rs3 {


////////////////////////////////////////////////////////////////////////////////////

void RTerrain::SetRenderState( RCameraSceneNode* pCurrentCamera, const RMatrix &viewMatrix, const RMatrix &projMatrix, RTerrainShader* pTerrainShader)
{
	RPFC_THISFUNC;
	RMatrix matWVP( (*m_pTerrain->m_matWorld) * viewMatrix * projMatrix );

	if( pTerrainShader)
	{
		if( pCurrentCamera->GetFogEnable())
		{
			float fNear = pCurrentCamera->GetFogNear();
			float fFar = pCurrentCamera->GetFogFar();
			float fDist = fFar - fNear;
			fDist = ( fDist == 0.0f ? 0 : 1.0f/fDist );
			pTerrainShader->m_scFogFactor.SetVector4(fNear, fFar, fDist, 0.0f);

			DWORD dwFogColor = pCurrentCamera->GetFogColor();
			RVector vecFogColor;
			DWColorToFloat3( &vecFogColor, dwFogColor );
			pTerrainShader->m_scFogColor.SetVector3( vecFogColor );

		}
		else
		{
			pTerrainShader->m_scFogFactor.SetVector4( 0, 0, 0, 0);
		}

		pTerrainShader->m_scWorld.SetMatrix((*m_pTerrain->m_matWorld));
		pTerrainShader->m_scWorldViewProj.SetMatrix( matWVP );
		pTerrainShader->m_scWorldView.SetMatrix( (*m_pTerrain->m_matWorld) * viewMatrix );
		pTerrainShader->m_scFarDist.SetFloat(pCurrentCamera->GetFarZ());
	}
}


void RTerrain::SetLayerMaskTexture( RTerrainPatch* pDrawPatch, int nLayerCount)
{
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	pDevice->SetTexture( TS_MASKMAP0, pDrawPatch->GetLayerMaskTexture(0));
	// 두번째 마스크는 레이어 카운트가 6 이상이 때만 쓰인다. 5장까지는 마스크 하나로 커버 가능.
	if( nLayerCount > 5)
		pDevice->SetTexture( TS_MASKMAP1, pDrawPatch->GetLayerMaskTexture(1));
}


void RTerrain::SetLayerTileTexture( LayerInfo *pInfo, int nLayerIdx, bool bNormalMap)
{
	// Set Render State
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	pDevice->SetTexture( TS_DIFFUSEMAP0+nLayerIdx, pInfo->hTileMap);

	if( bNormalMap )
		pDevice->SetTexture( TS_NORMALMAP0+nLayerIdx , pInfo->hNormalMap);
}


void RTerrain::RenderSingleLayer( RTechnique* pTechnique, std::vector<RTerrainPatch*>& arrPatch, RTerrainShader* pShader)
{
	RPFC_THISFUNC;
	_ASSERT( pTechnique);

	// Set Render State
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	RShaderCompositeManager::m_cAlphaRef.SetFloat( 0);
	pDevice->SetTextureAddressClamp(TS_DIFFUSE);
	pDevice->SetTextureFilter(TS_DIFFUSE, RTF_BILINEAR);

	// Set Texture State
	pDevice->SetTexture(TS_DIFFUSE, m_pTerrain->m_hBaseMap);

	RMatrix matTransform;
	matTransform.MakeIdentity();
	matTransform._11 = 1.0f/m_pTerrain->GetWidthSize();
	matTransform._22 =-1.0f/m_pTerrain->GetHeightSize();
	matTransform._42 = 1.0f;
	if( pShader)
	{
		pShader->m_scTextureTransform.SetMatrix(matTransform);;
	}
	else
	{
		// Set Texture Transform
		RShaderCompositeManager::m_cTextureTransform.SetMatrix(matTransform);
		RShaderCompositeManager::m_cTextureFactor.SetVector4(1, 1, 1, 1);
	}


	pDevice->SetFvF( RTerrainPatch::RVt_Terrain::FVF );

	pTechnique->Begin(NULL, TECH_PRESERVE_NOT);
	pTechnique->BeginPass();

	// Draw Primitives
	//for ( size_t si = 0; si<m_pTerrain->m_arrCulledSimplePatch.size(); ++si)
	for ( size_t si = 0; si < arrPatch.size(); ++si)
	{
		//RTerrainPatch *pDrawPatch = m_pTerrain->m_arrCulledSimplePatch[si];
		RTerrainPatch *pDrawPatch = arrPatch[si];
		m_pTerrain->RenderTerrainPatch(pDrawPatch);
	}

	pTechnique->EndPass();
	pTechnique->End();

	if( pShader)
	{

	}
	else
	{
		RShaderCompositeManager::m_cTextureTransform.SetMatrix(RMatrix::IDENTITY);
	}
}

void RTerrain::RenderBakedMapMStage( RCameraSceneNode* pCurrentCamera, bool bLit, RTerrainShader* pShader)
{
	RPFC_THISFUNC;
	RShaderCompositeManager* pCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	if(pCompositeManager->GetViewLightingOnly()) return;

	// Set Render State
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	RTechnique technique;
	if( pShader)
	{
		RShaderFX* pFX = pShader->GetShaderForBakedMapMStage( true);
		if( pFX)
		{
			technique = pFX->GetTechnique( "MainTechnique");
			RenderSingleLayer( &technique, m_pTerrain->m_arrCulledSimplePatch, NULL);
		}
	}
	
	
}


void RTerrain::RenderBakedMapGStage( RCameraSceneNode* pCurrentCamera, bool bFullMap, RTerrainShader* pShader)
{
	RPFC_THISFUNC;
	RShaderCompositeManager* pCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	if(pCompositeManager->GetViewLightingOnly()) return;

	// Set Render State
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	// Draw Primitives
	RTechnique technique;
	if( pShader)
	{
		RShaderFX* pFX = pShader->GetShaderForBakedMapGStage( REngine::GetSceneManager().GetRenderingStrategy()->GetMRTCount(), false );
		if( pFX)
		{
			technique = pFX->GetTechnique( "MainTechnique");
			RenderSingleLayer( &technique, bFullMap ? m_pTerrain->m_arrCulledPatch : m_pTerrain->m_arrCulledSimplePatch, pShader);
		}
	}
}


void RTerrain::RenderGrid( RCameraSceneNode* pCurrentCamera, RTerrainShader* pShader)
{
	RPFC_THISFUNC;
	RShaderCompositeManager* pCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	if(pCompositeManager->GetViewLightingOnly()) return;

	// Set Render State
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	// Draw Primitives
	RTechnique technique;
	if( pShader)
	{
		RShaderFX* pFX = pShader->GetShaderForGrid( 1, false );
		if( pFX)
		{
			// 그리드를 위한 셋팅
			RFILLMODE fillOrg = pDevice->GetFillMode();
			pDevice->SetFillMode( RFILL_WIRE );
			/// 바이어스를 걸지 않으면 스트립으로 인한 긴 선이 보인다.
			/// 도대체 바이어스랑 무슨 상관인지 모르겠지만, 조선시대때부터 M_toolTerrain에 존재했던 것을 그대로 가져오자.
			pDevice->SetDepthBias( -0.000001f, -0.15f );
			technique = pFX->GetTechnique( "MainTechnique");
			RenderSingleLayer( &technique, m_pTerrain->m_arrCulledPatch, pShader);
			// 셋팅 돌려 놓기
			pDevice->SetFillMode( fillOrg);
			pDevice->SetDepthBias( 0, 0 );
		}
	}
}


void RTerrain::RenderMarkingLayer( RCameraSceneNode* pCurrentCamera, RTerrainShader* pTerrainShader, int nMarkingLayerNumber)
{
	RPFC_THISFUNC;

	// Set Render State
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	RShaderCompositeManager* pCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	if(pCompositeManager->GetViewLightingOnly())
		return;

	CheckAndCreateMaskTexture( m_pTerrain->m_arrCulledPatch, pTerrainShader);
	SetRenderStateForSplattingLayerOnePass();
	pDevice->SetBlending( RBLEND_ONE, RBLEND_ONE);	/// 마스킹 모드에서는 Add Color
	RenderMarkingLayerOnePass( m_pTerrain->m_arrCulledPatch, pTerrainShader, 1, nMarkingLayerNumber);
	pDevice->SetBlending( RBLEND_NONE, RBLEND_NONE);
	RestoreRenderState();

}


void RTerrain::RenderSplattingLayerOnePassMStage( RCameraSceneNode* pCurrentCamera, RTerrainShader* pTerrainShader)
{
	RDeviceD3D *pDevice = (RDeviceD3D*)REngine::GetDevicePtr();
	_ASSERT(pDevice);

	RShaderCompositeManager* pCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	if(pCompositeManager->GetViewLightingOnly())
	{
		return;
	}


	CheckAndCreateMaskTexture( m_pTerrain->m_arrCulledDetailHighPatch, pTerrainShader);
	SetRenderStateForSplattingLayerOnePass();
	RenderSplattingLayerOnePass( true, m_pTerrain->m_arrCulledDetailHighPatch, pTerrainShader, REngine::GetSceneManager().GetRenderingStrategy()->GetMRTCount(), TTL_COUNT, -1);
	RestoreRenderState();
}


int RTerrain::GetMaskTextureCount( int nLayerCount)
{
	int nMaskTextureCount = 0;
	/// 베이스 타일 포함하여 5장까지 마스크 한장으로 커버 가능
	if( nLayerCount <= 5)
		nMaskTextureCount = 1;
	/// 원래는 9장까지 커버 가능하나 노말맵을 염두해두어 7장으로 제한
	else if( nLayerCount <= MAX_LAYER_PER_PATCH)	
		nMaskTextureCount = 2;
	/// 허용 레이어 수를 넘으면 예전 방식으로 랜더 해라
	else
		nMaskTextureCount = 0;

	return nMaskTextureCount;
}


void RTerrain::ClearMaskTexture()
{
	for ( int si = 0; si< (GetWidthCount()*GetHeightCount()); ++si )
	{
		ClearMaskTexture( si);
	}
}


void RTerrain::ClearMaskTexture( int nPatch)
{
	if( m_pPatches && nPatch < (GetWidthCount()*GetHeightCount()))
	{
		RTerrainPatch *pDrawPatch = &m_pPatches[nPatch];
		pDrawPatch->DeleteLayerMaskTexture();
	}
}


void RTerrain::MakeMaskTexture( int nPatchIdx, RTerrainShader* pTerrainShader)
{
	RPFC_THISFUNC;

	if( !m_pPatches || (nPatchIdx>=m_iPatchNum) || !m_pLayerInfo || !pTerrainShader)
		return;

	if(!m_pPatches[nPatchIdx].IsNeedUpdateLayerMaskTexture())
		return;
	
	int nLayerIdx = 0;

	DWORD dwMaxLayerWidth = 0;
	DWORD dwMaxLayerHeight = 0;
	
	DWORD dwMaxWidth = 0;
	DWORD dwMaxHeight = 0;

	int nLayerCount = 0;
	for( nLayerIdx = 0; nLayerIdx<m_iLayerNum; nLayerIdx++)
	{
		/// m_pLayerInfo[].phAlphaMask[]를 검사하는 것이 바로 확인 가능하지만, one-pass로 랜더 시 phAlphaMask가 필요가 없다. 
		/// IsAlphaTextureUsed(DataRaw가 변경 시 마다 플래그 설정 됨.)를 통해 체크 한다. 
		if ( IsAlphaTextureUsed( nPatchIdx, nLayerIdx))
		{
			LayerInfo *pInfo = &(m_pLayerInfo[nLayerIdx]);
			++nLayerCount;
			// 사용되는 레이어 중 최대 해상도의 값을 취하여 그 기준으로 알파 마스크를 만들어야 한다. 
			dwMaxLayerWidth = max( pInfo->dwAlphaPatchWidth, dwMaxLayerWidth);
			dwMaxLayerHeight = max( pInfo->dwAlphaPatchHeight, dwMaxLayerHeight);
			dwMaxWidth = max( pInfo->dwAlphaWidth, dwMaxWidth);
			dwMaxHeight = max( pInfo->dwAlphaHeight, dwMaxHeight);
		}
	}

	int nMaskTextureCount = GetMaskTextureCount(nLayerCount);

	/// 텍스쳐 생성 맻 랜더. 원래는 nMaskTextureCount만큼 생성을 해야하지만 현재는 9장 지원을 위해 무조건 2장 만든다.
	/// 8장 이하 지원이면 두번째 마스킹 텍스쳐를 NULL로 설정하여도 문제가 없지만
	/// 9장 지원 시 두번째 마스킹 텍스쳐를 NULL로 셋팅하면 9번째 타일의 마스킹인 mask1.a가 1로 읽히게 된다.
	if( m_pPatches[nPatchIdx].CreateLayerMaskTexture( nMaskTextureCount, dwMaxLayerWidth, dwMaxLayerHeight))
	{
		RTexture* pTargetOrg0 = REngine::GetDevice().GetRenderTarget(0);
		RTexture* pTargetOrg1 = REngine::GetDevice().GetRenderTarget(1);
		RTexture* pTargetOrg2 = REngine::GetDevice().SetRenderTarget(2, NULL);
		RTexture* pTargetOrg3 = REngine::GetDevice().SetRenderTarget(3, NULL);

		/// MRT로 두장을 동시에 만들어 낸다. 마스크가 하나만 쓰인다면 RT1은 NULL로 될 것이므로 신경 쓰지 않아도 될 것 같음.
		REngine::GetDevice().SetRenderTarget( 0, m_pPatches[nPatchIdx].GetLayerMaskTexture(0));
		REngine::GetDevice().SetRenderTarget( 1, m_pPatches[nPatchIdx].GetLayerMaskTexture(1));
		// 입력 소스로 될 텍스쳐 스테이지들을 비워줌.
		for( int i=0; i<MAX_LAYER_PER_PATCH-1; i++)
		{
			RDeviceD3D *pDevice = (RDeviceD3D*)REngine::GetDevicePtr();
			_ASSERT( pDevice);
			REngine::GetDevice().SetTexture( i, NULL);
			REngine::GetDevice().SetTextureAddressClamp(i);
			REngine::GetDevice().SetTextureFilter( i, RTF_BILINEAR);
			REngine::GetDevice().SetTextureMaxAnisotropy( i, 1);
		}
		// 알파 텍스쳐들을 입력 소스로
		int nCurrStage = 0;
		bool bFirstLayerSkipped = false;
		for( nLayerIdx = 0; nLayerIdx<m_iLayerNum; nLayerIdx++)
		{
			LayerInfo *pInfo = &(m_pLayerInfo[nLayerIdx]);
			if( IsAlphaTextureUsed( nPatchIdx, nLayerIdx))
			{
				// 첫번째(베이스)로 사용되는 타일 알파는 스킵. 바탕으로 깔려 무조건 가중치 1로 먹게 되기 때문이다.
				if( nCurrStage == 0 && !bFirstLayerSkipped)
				{
					bFirstLayerSkipped = true;
					continue;
				}
				/// 알파 텍스쳐 업데이트가 필요하면 그때 업데이트를 해주자.
				/// 업데이트 필요 설정의 경우
				/// 1. 생성이 처음 됨. 초기값이 T
				/// 2. pAlphaTextureRaw가 변경 된 경우. pAlphaTextureRaw를 변경 하는 곳에서 T로 설정 함.
				if( pInfo->bNeedUpdateAlphaTexture)
				{
					/// 전체 사이즈의 텍스쳐가 존재 하면 크기를 비교하여 재활용 여부를 선택하여 제거
					if( pInfo->pAlphaTexture)
					{
						RTextureInfo pAlphaTexture = pInfo->pAlphaTexture->GetInfo();
						if( pAlphaTexture.nTextureWidth != pInfo->dwAlphaWidth || pAlphaTexture.nTextureHeight != pInfo->dwAlphaHeight)
						{
							REngine::GetDevice().DeleteTexture(pInfo->pAlphaTexture);
							pInfo->pAlphaTexture = NULL;
						}
					}
					/// 텍스쳐가 없으면 텍스쳐를 생성
					if( !pInfo->pAlphaTexture)
					{
						RFORMAT alphaTextureFormat = RFMT_ARGB32;
						pInfo->pAlphaTexture = REngine::GetDevice().CreateTexture( pInfo->dwAlphaWidth, pInfo->dwAlphaHeight, alphaTextureFormat);
					}
					/// raw 데이터를 복사
					if(pInfo->pAlphaTexture)
						REngine::GetDevice().UpdateTexture( pInfo->pAlphaTexture, 0, 0, 0, pInfo->dwAlphaWidth, pInfo->dwAlphaHeight, 0, 0, pInfo->dwAlphaWidth, pInfo->dwAlphaHeight, (void*)(pInfo->pAlphaTextureRaw));
					/// 처리 완료 플래그
					pInfo->bNeedUpdateAlphaTexture = false;
				}
				/// 전체 사이즈의 텍스쳐를 셋팅하여 패치에 해당하는 일부를 그리게 하자.
				REngine::GetDevice().SetTexture( nCurrStage, pInfo->pAlphaTexture);

				nCurrStage++;
				// 텍스쳐 스테이지 6장 다 설정 되었으면 루프 종료. 총 사용 레이어는 7장이지만 베이스 레이어의 알파는 셋팅 되지 않으므로 6장으로 체크
				if( nCurrStage >= MAX_LAYER_PER_PATCH-1)
					break;
			}	
		}

		// 전체 텍스쳐의 일부만 랜더링 하는 것이므로 UV를 계산하여 그려줘야 함.
		float fStartX = (float)m_pPatches[nPatchIdx].GetX() / (float)GetWidthCount();
		float fStartY = (float)m_pPatches[nPatchIdx].GetY() / (float)GetHeightCount();
		float fGapX = 1.0f / (float)GetWidthCount();
		float fGapY = 1.0f / (float)GetHeightCount();
		// 텍스쳐는 상하좌우 한 픽셀씩의 마진을 두어 만들어져 있으므로 이 역시 반영해야 함.
		float fMarginX = 1.0f / (float)dwMaxWidth;
		float fMarginY = 1.0f / (float)dwMaxHeight;

		REngine::GetDevice().SetDepthEnable(false, false);
		//REngine::GetDevice().SetCullMode(RCULL_NONE);

		RTechnique tech = pTerrainShader->GetShaderForMakeMask()->GetTechnique("MainTechnique");
		pTerrainShader->m_scLayerCount.SetInt( nLayerCount);
		tech.Begin(NULL, TECH_PRESERVE_NOT);
		tech.BeginPass();
		RRenderHelper::RenderQuad(0, 0, (float)dwMaxLayerWidth, (float)dwMaxLayerHeight, fStartX-fMarginX, fStartY-fMarginY, fStartX+fGapX+fMarginX, fStartY+fGapY+fMarginY);
		tech.EndPass();
		tech.End();

		REngine::GetDevice().SetDepthEnable(true, true);
		REngine::GetDevice().SetCullMode(RCULL_CW);

		REngine::GetDevice().SetRenderTarget( 0, pTargetOrg0);
		REngine::GetDevice().SetRenderTarget( 1, pTargetOrg1);
		REngine::GetDevice().SetRenderTarget( 2, pTargetOrg2);
		REngine::GetDevice().SetRenderTarget( 3, pTargetOrg3);

		// 텍스쳐 스테이지들을 기본 설정으로
		for( int i=0; i<MAX_LAYER_PER_PATCH-1; i++)
		{
			REngine::GetDevice().SetTextureAddressWrap(i);
			REngine::GetDevice().SetTextureFilter( i, REngine::GetConfig().GetTextureFilter() );
			REngine::GetDevice().SetTextureMaxAnisotropy( i, REngine::GetConfig().GetTextureMaxAnisotropy() );
		}
	}

	m_pPatches[nPatchIdx].NeedUpdateLayerMaskTexture(false);
}


void RTerrain::CheckAndCreateMaskTexture( std::vector<RTerrainPatch*>& arrPatch, RTerrainShader* pTerrainShader)
{
	RPFC_THISFUNC;

	RDeviceD3D *pDevice = (RDeviceD3D*)REngine::GetDevicePtr();
	_ASSERT(pDevice);

	// one pass 랜더는 전용 셰이더만 존재하고 조합으로는 존재하지 않으니 반드시 필요.
	if( !pTerrainShader)
		return;

	/// 마스크 텍스쳐 업데이트
	for (size_t si = 0; si < arrPatch.size(); ++si)
	{
		RTerrainPatch *pDrawPatch = arrPatch[si];
		int nPatchIdx = pDrawPatch->GetY() * m_pTerrain->GetWidthCount() + pDrawPatch->GetX(); 
		/// 패치의 알파 마스크 텍스쳐가 업데이트가 필요하면 해준다.
		/// 필요 경우 1 : 생성이 애초에 되어 있지 않을 시
		/// 필요 경우 2 : 툴 등에서 업데이트 필요 플래그를 설정 한 경우
		if( pDrawPatch->IsNeedUpdateLayerMaskTexture() && pTerrainShader->GetShaderForMakeMask())
		{
			// 패치의 모든 레이어 정보가 합쳐진 마스크 텍스쳐 생성
			MakeMaskTexture( nPatchIdx, pTerrainShader);
		}
	}

	// MakeMaskTexture에서 랜더 쿼드를 호출하여 FVF가 변경되므로 다시 터레인의 것으로 돌려주자.
	//pDevice->SetFvF(RTerrainPatch::RVt_Terrain::FVF);
}


void RTerrain::SetRenderStateForSplattingLayerOnePass()
{
	RDeviceD3D *pDevice = (RDeviceD3D*)REngine::GetDevicePtr();
	_ASSERT(pDevice);

	pDevice->SetTextureFilter(TS_MASKMAP0, RTF_BILINEAR);
	pDevice->SetTextureFilter(TS_MASKMAP1, RTF_BILINEAR);
	pDevice->SetTextureAddressClamp(TS_MASKMAP0);
	pDevice->SetTextureAddressClamp(TS_MASKMAP1);

	// NVIDIA 스펙에 맞춰. ATI 포함.
	for( int i=0; i<MAX_LAYER_PER_PATCH; i++)
	{
		pDevice->SetTextureAddressWrap(TS_DIFFUSEMAP0+i);
		pDevice->SetTextureAddressWrap(TS_NORMALMAP0+i);
		pDevice->SetTextureFilter( TS_DIFFUSEMAP0+i, REngine::GetConfig().GetTextureFilter() );
		pDevice->SetTextureMaxAnisotropy( TS_DIFFUSEMAP0+i, REngine::GetConfig().GetTextureMaxAnisotropy() );
		pDevice->SetTextureFilter( TS_NORMALMAP0+i, REngine::GetConfig().GetTextureFilter() );
		pDevice->SetTextureMaxAnisotropy( TS_NORMALMAP0+i, REngine::GetConfig().GetTextureMaxAnisotropy() );

	}

	pDevice->SetFvF(RTerrainPatch::RVt_Terrain::FVF);
}


void RTerrain::RestoreRenderState()
{
	RDeviceD3D *pDevice = (RDeviceD3D*)REngine::GetDevicePtr();
	_ASSERT(pDevice);

// 	/// restore to default value
// 	if ( m_pTerrain->m_bAnisotropyFilter )
// 	{
// 		// NVIDIA 스펙에 맞춰. ATI 포함.
// 		for( int i=0; i<MAX_LAYER_PER_PATCH; i++)
// 		{
// 			pDevice->SetTextureFilter(TS_DIFFUSEMAP0+i, RTF_TRILINEAR);
// 			pDevice->SetTextureFilter(TS_NORMALMAP0+i, RTF_TRILINEAR);
// 			pDevice->SetTextureMaxAnisotropy( TS_DIFFUSEMAP0+i, 1);
// 			pDevice->SetTextureMaxAnisotropy( TS_NORMALMAP0+i, 1);
// 		}
// 	}

	pDevice->SetTextureAddressWrap(TS_MASKMAP0);
	pDevice->SetTextureAddressWrap(TS_MASKMAP1);
}


void RTerrain::RenderMarkingLayerOnePass( std::vector<RTerrainPatch*>& arrPatch, RTerrainShader* pTerrainShader, int nRenderTargetCount, int nMarkinglayer)
{
	RenderSplattingLayerOnePass( false, arrPatch, pTerrainShader, nRenderTargetCount, TTL_COUNT, nMarkinglayer);
}


void RTerrain::RenderSplattingLayerOnePass( bool bLighting, std::vector<RTerrainPatch*>& arrPatch, RTerrainShader* pTerrainShader, int nRenderTargetCount, TERRAIN_TILE_LEVEL tileLevel, int nMarkingLayer)
{
	RPFC_THISFUNC;

	RDeviceD3D *pDevice = (RDeviceD3D*)REngine::GetDevicePtr();
	_ASSERT(pDevice);

	// one pass 랜더는 전용 셰이더만 존재하고 조합으로는 존재하지 않으니 반드시 필요.
	if( !pTerrainShader)
		return;

	RTechnique OldTechnique;

	/// 상수 설정 비용을 줄이기 위해 변화가 있는 지 비교 후 반영.
	/// RShaderConst내에서 배열은 중복 체크를 하지 않는다. 배열은 중복하여 동일 값이 들어 갈 일이 거의 없어, 오히려 중복 체크가 더 손해다,
	/// 하지만, 지형 스플래팅 관련 값들은 겹칠 일이 많이 존재한다. 이를 위해 RShaderConst 내부에서 중복 체크를 하기보다는 지형에서 알아서 해 주자.
	RVector4 vTileTextureScaleAndTranslateArrayPrev[MAX_LAYER_PER_PATCH];
	for( int i=0; i<MAX_LAYER_PER_PATCH; ++i)
	{
		vTileTextureScaleAndTranslateArrayPrev[i] = RVector4(-1,-1,-1,-1);	/// 존재 할 수 없는 값
	}

	for (size_t si = 0; si < arrPatch.size(); ++si)
	{
		RTerrainPatch *pDrawPatch = arrPatch[si];
		int nPatchIdx = pDrawPatch->GetY() * m_pTerrain->GetWidthCount() + pDrawPatch->GetX(); 

		if( pDrawPatch->GetLayerMaskTexture(0))
		{
			int nLayerIdx=0;
			int nLayerCount=0;
			bool bMarkingSetted=0;	/// 문법적으로 말이 안되는 단어지만 의미를 직관적으로 하기 위해 ed를 붙임 ㅋ
			RVector4 vTileTextureScaleAndTranslateArray[MAX_LAYER_PER_PATCH];
			int nSpecularArray[MAX_LAYER_PER_PATCH];
			int nNormalmapArray[MAX_LAYER_PER_PATCH];
			for( nLayerIdx = 0; nLayerIdx<m_iLayerNum; nLayerIdx++)
			{
				if ( IsAlphaTextureUsed( nPatchIdx, nLayerIdx) )
				{
					LayerInfo *pInfo = &(m_pLayerInfo[nLayerIdx]);
					bool bNormalMap = (tileLevel == TTL_DETAIL_HIGH && pInfo->hNormalMap && REngine::GetSceneManager().GetShaderCompositeManager()->GetUseNormalMap());
					/// 타일링 관련 (디퓨즈, 노말) 텍스쳐 설정
					SetLayerTileTexture( pInfo, nLayerCount, bNormalMap);
					/// 타일 텍스쳐 스케일링은 레이어 마다 다르다.
					vTileTextureScaleAndTranslateArray[nLayerCount] = RVector4( 1.0f/pInfo->vTilingUV.x, 1.0f/pInfo->vTilingUV.y, 0, 0);
					nSpecularArray[nLayerCount] = pInfo->bAlphaMaskForSpec;
					nNormalmapArray[nLayerCount] = bNormalMap;

					/// 마킹 할 레이어 패치 기준 인덱스를 지정. 
					if( nMarkingLayer == nLayerIdx)
					{
						pTerrainShader->m_csMarkinglayer.SetFloat( nLayerCount);
						bMarkingSetted = true;
					}

					// 노말맵 사용을 염두해두어 7장의 텍스쳐 레이어 사용이 리미트~
					if( ++nLayerCount >= MAX_LAYER_PER_PATCH)
						break;
				}
			}
			/// 
			if( nMarkingLayer>=0 && !bMarkingSetted)
				pTerrainShader->m_csMarkinglayer.SetFloat( -1);

			/// 마스크 텍스쳐 두장 설정
			SetLayerMaskTexture( pDrawPatch, nLayerCount);

			if( nLayerCount > 0)
			{
				/// 타일링 UV는 레이어 마다 다르다.
				if( memcmp( vTileTextureScaleAndTranslateArray, vTileTextureScaleAndTranslateArrayPrev, sizeof(RVector4)*MAX_LAYER_PER_PATCH) != 0)
					pTerrainShader->m_scTileTextureScaleAndTranslateArray.SetVector4Array( vTileTextureScaleAndTranslateArray, nLayerCount);
				memcpy( vTileTextureScaleAndTranslateArrayPrev, vTileTextureScaleAndTranslateArray, sizeof(RVector4)*MAX_LAYER_PER_PATCH);	/// 값 보존
				/// 스페큘라 사용 유무도 레이어 마다 다르다.
				//if( memcmp( nSpecularArray, nSpecularArrayPrev, sizeof(int)*MAX_LAYER_PER_PATCH) != 0)
				//	pTerrainShader->m_scSpecularArray.SetIntArray( nSpecularArray, nLayerCount);
				//memcpy( nSpecularArrayPrev, nSpecularArray, sizeof(int)*MAX_LAYER_PER_PATCH);	/// 값 보존
				/// 알파 텍스쳐는 가장 큰 사이즈 기준으로 하자. 즉, 마스킹 텍스쳐의 것을 그대로 사용하면 될 듯.
				RTextureInfo textureInfo = pDrawPatch->GetLayerMaskTexture(0)->GetInfo();
				RVector2 vAlphaUVOffset(1.0f/(float)textureInfo.nTextureWidth, 1.0f/(float)textureInfo.nTextureHeight);
				RVector4 vAlphaTextureScaleAndTranslate( 1.0f - vAlphaUVOffset.x*2.f, 1.0f - vAlphaUVOffset.y*2.f, vAlphaUVOffset.x, vAlphaUVOffset.y);
				pTerrainShader->m_scAlphaTextureScaleAndTranslate.SetVector4( vAlphaTextureScaleAndTranslate);	/// Vector4는 내부서 중복 체크 됨.

				//RTechnique* CurrentTechnique = &TechniqueArray[nLayerCount-1]; 
				RTechnique CurrentTechnique;
				if( RShaderFX* pFX = pTerrainShader->GetShaderForSplattingLayerOnePass( bLighting, nLayerCount, nRenderTargetCount, nSpecularArray, nNormalmapArray, (bool)(nMarkingLayer>=0)))
				{
					if( pFX)
						CurrentTechnique = pFX->GetTechnique( "MainTechnique");
				}

				if( false == OldTechnique.IsEqual( CurrentTechnique ) )
				{
					if(OldTechnique.m_hTechnique)
					{
						OldTechnique.EndPass();
						OldTechnique.End();
					}
					CurrentTechnique.Begin(NULL, TECH_PRESERVE_NOT);
					CurrentTechnique.BeginPass();
				}
				
				CurrentTechnique.CommitChanges();
				m_pTerrain->RenderTerrainPatch(pDrawPatch);
				OldTechnique = CurrentTechnique;
			}
		}
	}

	// 테크닉 꺼주자
	if(OldTechnique.m_hTechnique)
	{
		OldTechnique.EndPass();
		OldTechnique.End();
	}

}



/// 이곳에 들어오는 경우는 deferred G stage 뿐일 껄...
void RTerrain::RenderSplattingLayerOnePassGStage( RCameraSceneNode* pCurrentCamera, RTerrainShader* pTerrainShader)
{
	RPFC_THISFUNC;

	RDeviceD3D *pDevice = (RDeviceD3D*)REngine::GetDevicePtr();
	_ASSERT(pDevice);

	if(!pTerrainShader)
		return;

	RShaderCompositeManager* pCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	if(pCompositeManager->GetViewLightingOnly())
	{
		return;
	}

	// 알파 마스크 체크 및 생성
	CheckAndCreateMaskTexture( m_pTerrain->m_arrCulledDetailHighPatch, pTerrainShader);
	CheckAndCreateMaskTexture( m_pTerrain->m_arrCulledDetailLowPatch, pTerrainShader);
	// 패치 실제 랜더
	SetRenderStateForSplattingLayerOnePass();
	RenderSplattingLayerOnePass( false, m_pTerrain->m_arrCulledDetailHighPatch, pTerrainShader, REngine::GetSceneManager().GetRenderingStrategy()->GetMRTCount(), TTL_DETAIL_HIGH, -1);
	RenderSplattingLayerOnePass( false, m_pTerrain->m_arrCulledDetailLowPatch, pTerrainShader, REngine::GetSceneManager().GetRenderingStrategy()->GetMRTCount(), TTL_DETAIL_LOW, -1);
	RestoreRenderState();
}













// 터레인이 다중 경로로 그리게 되면서 알파의 정보들 때문에...
void RTerrain::RenderDepthInfo()
{
	_ASSERT( "포워드에서만 돌아가야 돼요. 이제 포워드 버릴거니 루틴 모두 제거해BOA요");
}

};