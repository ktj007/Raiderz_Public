#include "stdafx.h"
#include "MXml.h"
#include "RPathSplitter.h"
#include "RTerrain.h"
#include "RTerrainPatch.h"
#include "RTerrainPVS.h"
#include "RRenderHelper.h"
#include "RTerrainQuadTree.h"
#include "RUtil.h"
#include "RXMLTemplate.h"
#include "RCameraSceneNode.h"
#include "RProfileInfo.h"
#include "RDirectionalLightSceneNode.h"
#include "REnvironmentSet.h"
#include "RCollisionTreeBuilder.h"
#include "Horizon.h"
#include "RGrassSystem.h"
#include "RShaderCompositeManager.h"
#include "RShadowStrategy.h"
#include "RShaderFX.h"
#include "RTerrainPassRenderer.h"
#include "RTerrainShader.h"
#include "RListener.h"

using namespace TXML;
using namespace std;

namespace rs3 {
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TerrainListener
class TerrainListener : public RListener
{
public:
	TerrainListener(RTerrain* pTerrain) : m_pTerrain( pTerrain) {}

	virtual void Update() override
	{
		if( m_pTerrain)
			m_pTerrain->ClearMaskTexture();
	}

	RTerrain* m_pTerrain;
};
// TerrainListener
//////////////////////////////////////////////////////////////////////////
// 디테일 거리를 상수로 고정하지 않고 조절 가능하게 변수로 제공하자.
float	RTerrain::m_fDetailDistance = 15000;
float	RTerrain::m_fDetailDistanceSq = 225000000;
float	RTerrain::m_fDetailDistanceHalf = 7500;
float	RTerrain::m_fDetailDistanceHalfSq = 56250000;

int RTerrain::CalcPriority()
{
	int nPriority;
	if (m_InfoDisplacementAndGrass.m_bLoadDone)
		nPriority = RBGWK_NORMAL;
	else
		nPriority = RBGWK_HIGHEST;

	return nPriority;
}

void RTerrain::OnLoadingComplete(RResource* pResource)
{
	_ASSERT( pResource == this );

	UpdateDirectionalLight(REngine::GetSceneManager().GetDefaultLight());
	UpdateAABB();

	SetRenderDetailMap(RTerrain::ONLY_SPLATTING);
	LoadBakedMap();	// Baked 맵을 만드는것이 아니라 읽어 들인다.
	SetRenderDetailMap(RTerrain::BAKED_AND_SPLATTING);

	// usable
	SetUsable(true);
	SetVisible(true);
}

/**
RTerrain : Patch를 여러개 모은 Terrain을 나타낸다.
*/

// 0x0002 
// 0x0003 : NORMALMAP Layer 추가 // <NORMALMAP>teress_ground_f01_nm.dds</NORMALMAP>
MImplementRTTI(RTerrain, RSceneNode);
DWORD RTerrain::m_dwVersion = ( 0x01 << 16 | 0x0003 );

int RTerrain::PATCH_SIZE = 32;
int RTerrain::LOD_LEVEL = 3;
const char* RTerrain::BAKED_MAP_FILENAME = "_baked.dds";

RTerrain::RTerrain() : RSceneNode(RRP_TERRAIN)
, m_bCreateNaviMesh(true)
, m_vLightDir( 0, 0, 1 )
, m_vAmbient( 1, 1, 1, 1 )
, m_vDiffuse( 1, 1, 1, 1 )
, m_bGridFlag( false)
, m_nMarkingLayerNumber(-1)
{
	Initialize();
	SetUsable(false);
	SetVisible(false);

	m_eDetailLevel = ONLY_BAKED;

	// 자동 업데이트
	SetAutoUpdate(true);

	m_pEventListener = new TerrainListener(this);
	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->AddListener( m_pEventListener );
}

RTerrain::~RTerrain()
{
	// TODO: terrain resource 다중상속을 제거하면서 레퍼런스카운트가 남는 문제도 해결하자
	m_nRefCount = 0; // warning 보기 싫어서 강제로 이렇게 두었다

	RDevice *pDevice = REngine::GetDevicePtr();

	// backgroundworkable 무효화
	//SAFE_DELETE(m_pLoadingWorkUnit);
	//RemoveLoadingController( this );

	m_arrCulledDetailHighPatch.clear();
	m_arrCulledDetailLowPatch.clear();
	m_arrCulledSimplePatch.clear();
	m_arrCulledPatch.clear();

	if ( m_pGrassSystem )
	{
		m_pGrassSystem->Destroy();
		delete m_pGrassSystem;
		m_pGrassSystem = NULL;
	}

	for ( int i = 0; i<m_iLayerNum; ++i )
		DeleteAlphaTexture(i);

	pDevice->DeleteTexture(m_hBaseMap);

	SAFE_DELETE(m_pQuadTree);
	SAFE_DELETE(m_pPVS);
	//SAFE_DELETE(m_pHorizonCull);

	SAFE_DELETE_ARRAY(m_pPatches);
	SAFE_DELETE_ARRAY(m_pNormalMap);
	SAFE_DELETE(m_pHeightField);
	
	for ( int i = 0; i<m_iIndexNum; ++i )
		REngine::GetDevice().DeleteIndexBuffer(m_phIndexBuffer[i]);

	SAFE_DELETE_ARRAY(m_phIndexBuffer);
	SAFE_DELETE_ARRAY(m_pPrimitiveNum);

	for ( int i = 0; i<m_iLayerNum; ++i )
	{
		REngine::GetDevice().DeleteTexture(m_pLayerInfo[i].hTileMap);
		REngine::GetDevice().DeleteTexture(m_pLayerInfo[i].hNormalMap);
		m_pLayerInfo[i].hTileMap = R_NONE;
		m_pLayerInfo[i].hNormalMap = R_NONE;
		DeleteAlphaTextureRaw(i);
	}

	// Fill() 에서 메모리 접근 오류 나는 것 방지.
	m_deqInfoAlphaMap.clear();
	m_deqInfoTileMap.clear();

	SAFE_DELETE_ARRAY(m_pLayerInfo);

	REngine::GetEventBroadcaster(RE_DEVICE_RESTORE)->RemoveListener( m_pEventListener );
	SAFE_DELETE(m_pEventListener);
}

void RTerrain::Cull(RCameraSceneNode* pCamera,const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar)
{
	RPFC_THISFUNC;

	m_arrCulledPatch.clear();
	m_arrCulledDetailHighPatch.clear();
	m_arrCulledDetailLowPatch.clear();
	m_arrCulledSimplePatch.clear();

	//if ( m_pHorizonCull )
	//	m_pHorizonCull->Clear();

	const RPlane* pCameraNearPlane = &pCamera->GetViewFrustum().GetPlanes()[RV_NEAR];
	RVector* pCameraPos = &pCamera->GetWorldPosition();

	CullTree(pCamera,m_pQuadTree, pCameraPos, pCameraNearPlane, cullDescs, pPlaneNear, pPlaneFar);
}

void RTerrain::CullTree(RCameraSceneNode* pCamera,RTerrainQuadTree *pNode, const RVector* pCameraPos, const RPlane* pCameraNearPlane,
	const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar)
{
	if ( pNode == NULL ) 
		return;

	if( GetUsable() == false )
		return;

	RBoundingBox* const pAABB = pNode->GetPatchAABB();
	_ASSERT(pAABB);

	if( VisibilityTestAABB(pAABB, cullDescs, pPlaneNear, pPlaneFar) )
	{
		const RPlane *pClipPlane = pCamera->GetUserClipPlane();
		int nClipPlaneNum = pCamera->GetUserClipPlaneNum();
		
		// isInPlanes는 nNum 이 0일때는 true를 리턴하지만, 명확하게 하기 위해 nClipPlaneNum을 if 에 넣었다.
		if ( nClipPlaneNum == 0 || pAABB->isInPlanes(pClipPlane, nClipPlaneNum))
		{
			std::vector<RTerrainPatch*>* pPatches = pNode->GetPatches();

			// Horizon Cull test : 나중에 PATCH 단위로 넣도록 고쳐야 할지도 모른다.
			if ( pPatches->empty() == false )
			{
#if 0
				if ( m_pHorizonCull 
					&& m_eDetailLevel != ONLY_BAKED
					)
				{
					RVector4 b1, b2, b3, b4;
					RVector4 t1, t2, t3, t4;

					const RMatrix &matWVP = pCamera->GetViewProjectionMatrix();
					const RVector *pMinimums = pNode->GetMinimumQuad();
					const RVector *pMaximums = pNode->GetMaximumQuad();

					matWVP.TransformVect(pMinimums[0], b1);
					matWVP.TransformVect(pMinimums[1], b2);
					matWVP.TransformVect(pMinimums[2], b3);
					matWVP.TransformVect(pMinimums[3], b4);

					matWVP.TransformVect(pMaximums[0], t1);
					matWVP.TransformVect(pMaximums[1], t2);
					matWVP.TransformVect(pMaximums[2], t3);
					matWVP.TransformVect(pMaximums[3], t4);

					bool below = m_pHorizonCull->ClipAndTestLine(t1, t2);
					below &= m_pHorizonCull->ClipAndTestLine(t2, t3);
					below &= m_pHorizonCull->ClipAndTestLine(t3, t4);
					below &= m_pHorizonCull->ClipAndTestLine(t4, t1);

					below &= m_pHorizonCull->ClipAndTestLine(t1, b1);
					below &= m_pHorizonCull->ClipAndTestLine(t2, b2);
					below &= m_pHorizonCull->ClipAndTestLine(t3, b3);
					below &= m_pHorizonCull->ClipAndTestLine(t4, b4);

					if ( below )
						return;

					if ( pBasePlane->GetDistanceTo(pSphere->center) > pSphere->radius + pCamera->GetNearZ() )
					{
						{
							m_pHorizonCull->ClipAndRenderLine(b1, b2);
							m_pHorizonCull->ClipAndRenderLine(b2, b3);
							m_pHorizonCull->ClipAndRenderLine(b3, b4);
							m_pHorizonCull->ClipAndRenderLine(b4, b1);
						}
					}
				}
#endif		
				int nX, nY, nZ;
				GetPatchXYZ(&nX,&nY,&nZ, pCameraPos);

				for ( std::vector<RTerrainPatch*>::iterator it = pPatches->begin(); it != pPatches->end(); ++it )
				{
					RTerrainPatch *pPatch = *it;
					int x = pPatch->GetX();
					int y = pPatch->GetY();

					if ( m_pPVS )
						if ( !m_pPVS->isVisible(nX,nY,nZ, x, y))
							continue;

					m_arrCulledPatch.push_back(pPatch);
				}

			} // end if ( not empty )

			if ( pNode->isLeaf())
				return;

			// Down -> Up sort rendering (약간 부족하다.)
			struct SNodeDistanceList
			{
				RTerrainQuadTree *pNode;
				float fDist;

				static bool EvalDistance(const SNodeDistanceList &a, const SNodeDistanceList &b)
				{
					return a.fDist < b.fDist;
				}
			};

			SNodeDistanceList NodeList[4];

			for ( int i = 0; i<4; ++i )
			{
				RTerrainQuadTree *pChildNode = pNode->GetChild(i);
				_ASSERT(pChildNode);
				RBoundingBox* const pChildAABB = pChildNode->GetPatchAABB();

				NodeList[i].pNode = pChildNode;
				NodeList[i].fDist = pCameraNearPlane->GetDistanceTo(pChildAABB->GetCenter());
			}

			// begin sort
			std::sort(&NodeList[0],&NodeList[4], SNodeDistanceList::EvalDistance);

			_ASSERT(NodeList[0].fDist<=NodeList[1].fDist);
			_ASSERT(NodeList[1].fDist<=NodeList[2].fDist);
			_ASSERT(NodeList[2].fDist<=NodeList[3].fDist);

			// Call
			CullTree(pCamera,NodeList[0].pNode, pCameraPos, pCameraNearPlane, cullDescs, pPlaneNear, pPlaneFar);
			CullTree(pCamera,NodeList[1].pNode, pCameraPos, pCameraNearPlane, cullDescs, pPlaneNear, pPlaneFar);
			CullTree(pCamera,NodeList[2].pNode, pCameraPos, pCameraNearPlane, cullDescs, pPlaneNear, pPlaneFar);
			CullTree(pCamera,NodeList[3].pNode, pCameraPos, pCameraNearPlane, cullDescs, pPlaneNear, pPlaneFar);
		}	// end of clip-plane enable test
	}	// end of view frustum test
}

bool RTerrain::VisibilityTestAABB(const RBox* pAABB, const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar)
{
	if( pPlaneNear && !pAABB->isInPlane( *pPlaneNear ) )
		return false;

	if( pPlaneFar && !pAABB->isInPlane( *pPlaneFar ) )
		return false;

	size_t cullDescsSize = cullDescs.size();
	if( cullDescsSize )
	{
		for( size_t i = 0; i < cullDescsSize; ++i )
		{
			if( pAABB->isInPlanes( cullDescs[i].pPlanes, cullDescs[i].nPlanes ) )
				return true;
		}

		return false;
	}

	return true;
}

void RTerrain::CalcLODLevel(const RVector *pCameraPos, int nX, int nY, int nZ)
{
	// 2 Pass
	// Culling 되어 있는 Index에 대하여, LOD레벨을 계산한다.
	for ( size_t si = 0; si<m_arrCulledPatch.size(); ++si )
	{
		RTerrainPatch *pPatch = m_arrCulledPatch[si];

		RBoundingBox *pBoundBox = pPatch->GetAABBBoundBox();
		RBoundingSphere *pSphere = pPatch->GetBoundSphere();

		int z = (pBoundBox->vmax.z - pBoundBox->vmin.z)/2.0f/m_iCount;
		int x = pPatch->GetX();
		int y = pPatch->GetY();

		int dX = abs(nX - x);
		int dY = abs(nY - y);

		int nLevel = max(dX, dY);
		nLevel /= m_nLODPatchCount;

		// nLevel은 연속적이어야 한다. 그렇지 않으면, Crack이 생길 수 있다.
		nLevel = ( m_iLODLevel <= nLevel ? m_iLODLevel-1 : nLevel );
		pPatch->SetLevel(nLevel);

		switch ( m_eDetailLevel )
		{
		case ONLY_BAKED:
			m_arrCulledSimplePatch.push_back(pPatch);
			break;
		case BAKED_AND_SPLATTING:
			switch( CalcDetail(pCameraPos, pSphere))
			{
			case TTL_DETAIL_HIGH:
				m_arrCulledDetailHighPatch.push_back(pPatch);
				break;
			case TTL_DETAIL_LOW:
				m_arrCulledDetailLowPatch.push_back(pPatch);
				break;
			case TTL_SIMPLE:
				m_arrCulledSimplePatch.push_back(pPatch);
				break;
			}
			break;
		case ONLY_SPLATTING:	/// 보통 툴에서 사용 용도.
			switch( CalcDetail(pCameraPos, pSphere))
			{
			case TTL_DETAIL_HIGH:
				m_arrCulledDetailHighPatch.push_back(pPatch);
				break;
			default:
				m_arrCulledDetailLowPatch.push_back(pPatch);
				break;
			}
			break;
		}
	}
	
	// 인접 LOD와 관련하여 계산한다.
	for ( size_t si = 0; si<m_arrCulledPatch.size(); ++si )
	{
		RTerrainPatch *pPatch = m_arrCulledPatch[si];

		int x = pPatch->GetX();
		int y = pPatch->GetY();
		int nIndex = x + y*GetWidthCount();

			// 레벨에 따른 Index 구하기
		int nLevel = pPatch->GetLevel();
		int nFlag = 0;

		int nearIndex[4];
		nearIndex[0] = ( x-1 < 0 ? nIndex : nIndex-1 );			// LEFT : (x-1) + y * m_iCount);
		nearIndex[1] = ( x+1 >= m_iCount ? nIndex : nIndex+1 );	// RIGHT : (x+1) + y * m_iCount);
		nearIndex[2] = ( y-1 < 0 ? nIndex : nIndex - m_iCount);	// DOWN : x + (y-1) * m_iCount);
		nearIndex[3] = ( y+1 >= m_iCount ? nIndex : nIndex + m_iCount); // UP : x + (y+1) * m_iCount);

		if ( m_pPatches[nearIndex[0]].GetLevel() < nLevel )
			nFlag |= LEFT;
		if ( m_pPatches[nearIndex[1]].GetLevel() < nLevel )
			nFlag |= RIGHT;
		if ( m_pPatches[nearIndex[2]].GetLevel() < nLevel )
			nFlag |= TOP;
		if ( m_pPatches[nearIndex[3]].GetLevel() < nLevel )
			nFlag |= BOTTOM;
			
		pPatch->SetNearFlag(nFlag);

		_ASSERT(nLevel>=0);
	}
}

bool RTerrain::GetPickVertical(RVector &rOut, RVector *pNormal, float x, float y)
{
	if(!GetUsable()) return false;
	if ( m_pHeightField == NULL )
		return false;


	float fHeight = 0.0f;
	bool bResult = m_pHeightField->GetHeightAt(&fHeight, pNormal, x, y);
	if (! bResult )
		return false;

	rOut.x = x;
	rOut.y = y;
	rOut.z = fHeight;

	return true;
}

bool RTerrain::GetPickVerticalWithHole( RVector &rOut, RVector *pNormal, float x, float y )
{
	if(!GetUsable()) return false;
	if ( m_pHeightField == NULL )
		return false;


	float fHeight = 0.0f;
	bool bResult = m_pHeightField->GetHeightAtWithHole(&fHeight, pNormal, x, y);
	if (! bResult )
		return false;

	rOut.x = x;
	rOut.y = y;
	rOut.z = fHeight;

	return true;
}

bool RTerrain::GetPickLine(RVector &rOut, RVector *pNormal, const RVector &p, const RVector &q)
{
	if(!GetUsable()) return false;
	if(m_pHeightField == NULL ) return false;

	bool bResult = m_pHeightField->GetPickLine(&rOut, pNormal, NULL, p, q);
	return bResult;
}

int RTerrain::GetVertexFromBound(std::vector<RVt_pos> &vecVertex, const RBoundingBox &aabb)
{
	if(!GetUsable())
		return 0;

	return RTerrainResource::GetVertexFromBound(vecVertex, aabb);
}

string RTerrain::GetNormalMapFileName(string strTileFileName)
{
	char _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
	_splitpath_s( strTileFileName.c_str(), NULL, 0, NULL, 0, _fname, _MAX_FNAME, _ext, _MAX_EXT);

	string strOut;

	string _normalmap;
	_normalmap += _fname;
	_normalmap += "_nm";
	_normalmap += _ext;

	if ( MIsExistFile( _normalmap.c_str()) == true)
		strOut = _normalmap;

	return strOut;
}

void RTerrain::SaveToElement(MXmlElement *pElement)
{
	char pcVersion[128];
	sprintf(pcVersion, "%d.%d", m_dwVersion >> 16, m_dwVersion & 0xffff );
	_SetAttribute(pElement,"version",pcVersion);

	_SetContents(pElement, "");
	_SetElementValue(pElement,"MAP_WIDTH","int",m_pHeightField->GetWidthMap());
	_SetElementValue(pElement,"MAP_HEIGHT","int",m_pHeightField->GetHeightMap());
	_SetElementValue(pElement,"MAP_SIZE","vec3", RVector( GetWidthSize(), GetHeightSize(), 0.0f));
	_SetElementValue(pElement,"MAP_NAME","string", m_strDisplacementMapName.c_str());
	_SetElementValue(pElement,"BASEMAP_NAME","string", m_strBaseMapName.c_str());
	_SetElementValue(pElement,"CREATE_NAVEMESH","bool", m_bCreateNaviMesh);

	//_SetElementValue(pElement,"PVS","string","terrain.pvs");

	// TODO: 라이트맵 로딩 안하게 - 포워드 렌더링을 삭제할때 완전히 지우자.
	//int nLightMapNum = (int)m_vecLightmap.size();
	//for ( int i = 0; i<nLightMapNum; ++i )
	//{
	//	MXmlElement *pLightMapElement = new MXmlElement("LIGHTMAP");
	//	pLightMapElement->SetAttribute("index", i);
	//	pLightMapElement->SetAttribute("number", nLightMapNum);
	//	_SetContents(pLightMapElement, m_vecLightmap[i].strLightMapName.c_str());
	//	
	//	pElement->LinkEndChild(pLightMapElement);
	//}

	if ( m_pGrassSystem )
		_SetElementValue(pElement,"GRASS","string", m_strGrassName.c_str());

	MXmlElement *pArrayElement = new MXmlElement("LAYER");
	_SetAttribute(pArrayElement,"type","array");
	_SetAttribute(pArrayElement,"size",m_iLayerNum);

	std::string strTileMapType;
	for ( int i = 0; i<m_iLayerNum; ++i )
	{
		MXmlElement *pVarElement = new MXmlElement("LAYERINFO");
		_SetAttribute(pVarElement,"index", i);

		//alphamap
		_SetElementValue(pVarElement,"ALPHAMAP", "string", m_pLayerInfo[i].alphaMapName.c_str());

		// tile map
		MXmlElement *pTileMapElement = new MXmlElement("TILEMAP");
		pVarElement->LinkEndChild(pTileMapElement);
		strTileMapType = m_pLayerInfo[i].bAlphaMaskForSpec ? "specular" : "blend";
		_SetAttribute(pTileMapElement, "type", strTileMapType);
		_SetContents(pTileMapElement, m_pLayerInfo[i].tileMapName);

		// normal map
		if( m_pLayerInfo[i].normalMapName.size() )
		{
			_SetElementValue(pVarElement,"NORMALMAP", "string", m_pLayerInfo[i].normalMapName.c_str());
		}
		
		// tiling
		_SetElementValue(pVarElement,"TILEUV" , "vec3", m_pLayerInfo[i].vTilingUV);

		pArrayElement->LinkEndChild(pVarElement);
	}
	pElement->LinkEndChild(pArrayElement);
}

int RTerrain::RenderTerrainPatch(RTerrainPatch *pPatch)
{
	RPFC_THISFUNC;

	int nLevel	= pPatch->GetLevel();
	_ASSERT(nLevel>=0);
	if(nLevel<0)
		return 0;

	int nFlag	= pPatch->GetNearFlag();
	int nLODIndex		= GetLodIndex(nLevel, nFlag);
	_ASSERT(nLODIndex>=0);
	if(nLODIndex<0)
		return 0;

	int nPrimitiveNum	= m_pPrimitiveNum[nLODIndex];

	REngine::GetDevice().SetIndexBuffer( m_phIndexBuffer[nLODIndex]);
	REngine::GetDevice().SetVertexBuffer( pPatch->GetVertexBuffer(), 0, 0);
	REngine::GetDevice().DrawIndexedPrimitive(
		RPT_TRIANGLESTRIP, pPatch->m_pVertexStart[nLevel], 0, pPatch->GetVertexNum(nLevel),
		0, nPrimitiveNum);

	REngine::GetResourceProfiler().AddPolygon(PPT_TERRAIN,nPrimitiveNum);
	return nPrimitiveNum;
}


void RTerrain::UpdateAABB()
{
	_ASSERT( IsLoadingSucceeded() );

	if ( m_pHeightField )
	{
		m_pHeightField->UpdateAABB();
		SetAABB( m_pHeightField->GetAABB() );
	}
}

void RTerrain::OnUpdateAABB()
{
	// 로딩중이면 넘어간다, 로딩끝나고 업데이트 한다
	if( false == IsLoadingSucceeded() )
		return;

	UpdateAABB();
}

void RTerrain::OnUpdate()
{
	RPFC_THISFUNC;

	// Grass-system update
	if ( m_pGrassSystem )
	{
		m_pGrassSystem->UpdateDXBuffers( this);
		m_pGrassSystem->Update(REngine::GetSceneManagerPtr());
	}

// 	// m_children 리스트에 주의 .
// 	RSceneNode::OnUpdate();
}

bool RTerrain::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	SetPathAndResourceName();
	dlog("create terrain : %s\n", m_strName.c_str() );
	
	if ( !LoadFromXMLElement(&element, bBackgroundCreation) )
		return false;

	return REngine::GetSceneManagerPtr()->AddLoadingList( this, bBackgroundCreation, this );
}

void RTerrain::SaveToXML(MXmlElement &element)
{
	SaveToElement(&element);
}

void RTerrain::SetPathAndResourceName()
{
	RWorldSceneNode* pRoot = NULL;
	pRoot = RSceneNode::GetRootSceneNode();	
	if( pRoot )
		SetPath( pRoot->GetXMLFilePath().c_str());
	
	char szFilename[MAX_PATH];
	if( pRoot )
		SplitPath(pRoot->GetXMLFileName().c_str(), NULL, szFilename);
	if ( char *p = strchr(szFilename,'.'))
		*p = '\0';

	SetResourceName(szFilename);
}

bool RTerrain::OnPick(RPICKINFO& pickInfo)
{
	RVector vOut;
	RVector vNormal;
	if ( GetPickLine(vOut, &vNormal, pickInfo.inPickOrigin, pickInfo.inPickTo))
	{
		pickInfo.outPosition = vOut;
		pickInfo.outNormal = vNormal;
		pickInfo.outNode = this;

		return true;
	}

	return false;
}

bool RTerrain::GetNormalVector( RVector* pPos, RVector* pOut)
{
	if( pPos && pOut)
	{
		// 피킹하여 노말을 얻음
		RPICKINFO pickInfo;
		pickInfo.inPickOrigin = *pPos + RVector(0,0,300000);
		pickInfo.inPickTo = *pPos + RVector(0,0,-30000);
		pickInfo.outNormal = RVector( 0,0,1);
		// 터레인의 함수 호출
		if ( OnPick( pickInfo))
		{
			// 노말라이즈!!!
			pickInfo.outNormal = pickInfo.outNormal.Normalize();
		}

		// 아웃풋으로 셋팅
		pOut->x = pickInfo.outNormal.x;
		pOut->y = pickInfo.outNormal.y;
		pOut->z = pickInfo.outNormal.z;
	
		return true;
	}

	return false;
}


void RTerrain::UpdateDirectionalLight(RDirectionalLightSceneNode *pLight)
{
	if( GetUsable() == false || pLight == NULL )
		return;

	UpdateDirectionalLight(pLight->GetDirection(), pLight->GetDiffuse(), pLight->GetAmbient(), pLight->GetPower(), pLight->GetSize());
}

void RTerrain::UpdateDirectionalLight(const RVector &vDirection, const RVector4 &vDiffuse, const RVector4 &vAmbient, float fPower, float fSize)
{
	// diffuse color
	RVector4 vDiffuseColor( fPower * vDiffuse.r, fPower * vDiffuse.g, fPower * vDiffuse.b, 1.f);
	RVector4 vAmbientColor( fPower * vAmbient.r, fPower * vAmbient.g, fPower * vAmbient.b, 1.f);

	// set grass system
	if( m_pGrassSystem )
	{
		m_pGrassSystem->SetDiffuseColor( vDiffuseColor );
		m_pGrassSystem->SetAmbientColor( vAmbientColor );
	}

	// 셰이더 적용 값
	m_vLightDir = vDirection;
	m_vDiffuse = vDiffuseColor;
	m_vAmbient = vAmbientColor;

	// 값 255 사이로 정규화
	vDiffuseColor *= ( 255.f);
	vAmbientColor *= ( 255.f);

	int reg_r = (int)( min( vDiffuseColor.x, 255.f) );
	int reg_g = (int)( min( vDiffuseColor.y, 255.f) );
	int reg_b = (int)( min( vDiffuseColor.z, 255.f) );
	m_dwDiffuseColor = D3DCOLOR_XRGB( reg_r, reg_g, reg_b );

	reg_r = (int)( min( vAmbientColor.x, 255.f) );
	reg_g = (int)( min( vAmbientColor.y, 255.f) );
	reg_b = (int)( min( vAmbientColor.z, 255.f) );
	m_dwAmbientColor = D3DCOLOR_XRGB( reg_r, reg_g, reg_b );
}

void RTerrain::RenderCullRawMap()
{
	if( GetUsable() == false ) return;
	if( GetVisible() == false ) return;
	if( GetOcclued() == true ) return;

	REngine::GetDevice().SetFvF(RTerrainPatch::RVt_Terrain::FVF);

	for ( size_t si = 0; si<m_arrCulledPatch.size(); ++si)
	{
		RTerrainPatch *pDrawPatch = m_arrCulledPatch[si];

		int nDrawNum = RenderTerrainPatch(pDrawPatch);
		_ASSERT(nDrawNum!=0);
	}
}

void RTerrain::OnRenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy )
{
	RTerrainShader* pShader = NULL;
	//RTerrainPassRenderer* pTerrainPassRenderer = ((RTerrainPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_TERRAIN)));
	//if( pTerrainPassRenderer)
	//	pShader = pTerrainPassRenderer->GetTerrainShader();

	SetRenderState( pCurrentCamera, pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetProjectionMatrix(), pShader);

	REngine::GetDevice().SetFvF(RTerrainPatch::RVt_Terrain::FVF);
	REngine::GetDevice().SetCullMode(RCULL_NONE);

	RTechnique technique;
	// 그림자 맵 용은 따로 지형에 특화되거나 한 것이 없다. 셰이더 조합을 그냥 따라가도록 하자.
	//if( pShader)
	//{
	//	pShader->m_scWorldViewProj.SetMatrix(pCurrentCamera->GetViewProjectionMatrix());
	//	pShader->m_scWorldView.SetMatrix(pCurrentCamera->GetViewMatrix());
	//
	//	RShaderFX* pFX = pShader->GetShaderForShadowMap();
	//	if( pFX)
	//		technique = pFX->GetTechnique( "MainTechnique");
	//}
	//else
	{
		RShaderCompositeManager::SetTransformConstant( (*m_pTerrain->m_matWorld)/*RMatrix::IDENTITY*/, pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix());
		RShaderCompositeManager* m_pShaderCompositeManager = REngine::GetSceneManager().GetShaderCompositeManager();
		RSCID rscComponents = RSC_POSITION | m_pShaderCompositeManager->GetVertexTexcoordComponent() | pShadowStrategy->GetDepthWriteComponent();
		RShaderFX* pShader = m_pShaderCompositeManager->GetShader( rscComponents, false );
		if( pShader )
		{
			technique = pShader->GetTechniqueByIndex(0);

			technique.Begin(NULL, TECH_PRESERVE_NOT);
			technique.BeginPass();
			RenderCullRawMap();
			technique.EndPass();
			technique.End();	
		}
	}

	REngine::GetDevice().SetCullMode(RCULL_CW);
}

void RTerrain::OnAddScene()
{
	_ASSERT( NULL != RSceneNode::GetRootSceneNode() );
	RSceneNode::GetRootSceneNode()->SetTerrain(this);	
}

void RTerrain::OnRemoveScene()
{
	_ASSERT( NULL != RSceneNode::GetRootSceneNode() );
	RSceneNode::GetRootSceneNode()->SetTerrain(NULL);	
}

bool RTerrain::CreateBakedMap()
{
	_ASSERT( m_pHeightField );

	RDevice* pDevice = REngine::GetDevicePtr();
	_ASSERT( NULL != pDevice );

	RSceneManager* pSceneManager = REngine::GetSceneManagerPtr();
	_ASSERT( NULL != pSceneManager );

	// 기본 렌더 메서드가 디퍼드여야 한다. - charmke
	_ASSERT( RENDERING_DEFERRED == pSceneManager->GetRealtimeRenderingMethod() );
	if( RENDERING_DEFERRED != pSceneManager->GetRealtimeRenderingMethod() )
		return false;

	RTerrainPassRenderer* pTerrainPassRenderer = static_cast<RTerrainPassRenderer*>( REngine::GetSceneManager().GetPassRenderer( RRP_TERRAIN ) );
	_ASSERT( pTerrainPassRenderer );
	if( NULL == pTerrainPassRenderer )
		return false;

	RTerrainShader* pShader = pTerrainPassRenderer->GetTerrainShader();
	_ASSERT( pShader );
	if( NULL == pShader )
		return false;

	RBoundingBox aabb = GetAABB();
	float fXLength = aabb.vmax.x - aabb.vmin.x;
	float fYLength = aabb.vmax.y - aabb.vmin.y;
	float fZLength = aabb.vmax.z - aabb.vmin.z;

	RCameraSceneNode camera;
	RMatrix matProjection;
	matProjection.MakeIdentity();
	matProjection.SetOrthoOffCenterRH(0, fXLength, 0, fYLength, 0, fZLength + 200.f);
	camera.SetUserProjection( matProjection );
	camera.Set( RVector(0, 0, aabb.vmax.z + 200.f), RVector( 0.f, 0.f, -1.f ) );
	camera.UpdateViewFrustrum();

	int nSize = m_iCount * m_iCount;
	std::vector<RTerrainPatch*> arrCulledPatch;
	arrCulledPatch.resize( nSize );

	for( int i = 0; i < nSize; ++i )
	{
		arrCulledPatch[i] = &m_pPatches[i];

		m_pPatches[i].SetLevel(0);
		m_pPatches[i].SetNearFlag(0);
	}

	if( m_hBaseMap != R_NONE )
	{
		pDevice->DeleteTexture( m_hBaseMap );
		m_hBaseMap = R_NONE;
	}
	m_nBaseMapWidth = GetWidthMap() * 2;
	m_nBaseMapHeight = GetHeightMap() * 2;
	m_hBaseMap = pDevice->CreateRenderTargetTexture( m_nBaseMapWidth, m_nBaseMapHeight, RFMT_ARGB32 );
	RTexture* pDepthBuffer = pDevice->CreateRenderTargetTexture( m_nBaseMapWidth, m_nBaseMapHeight, RFMT_D16 );

	RTexture* hBeforeTarget = pDevice->GetRenderTarget( 0 );
	pDevice->SetRenderTarget( 0, m_hBaseMap );
	RTexture* hBeforeDepthTarget = pDevice->SetDepthStencilBuffer( pDepthBuffer );

	pDevice->Clear( true, true, false );
	pDevice->BeginScene();
	SetRenderState( &camera, camera.GetViewMatrix(), camera.GetProjectionMatrix(), pShader );
	CheckAndCreateMaskTexture( arrCulledPatch, pShader);
	SetRenderStateForSplattingLayerOnePass();
	RenderSplattingLayerOnePass( false, arrCulledPatch, pShader, 1, TTL_COUNT, -1);
	RestoreRenderState();
	pDevice->EndScene();

	pDevice->SetRenderTarget( 0, hBeforeTarget );
	pDevice->SetDepthStencilBuffer( hBeforeDepthTarget );
	pDevice->DeleteTexture( pDepthBuffer );
	pDepthBuffer = NULL;
	return true;
}

bool RTerrain::LoadBakedMap()
{
	if ( m_hBaseMap != R_NONE ) {
		REngine::GetDevice().DeleteTexture(m_hBaseMap);
		m_hBaseMap = R_NONE;
	}

	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice!=NULL);

	_ASSERT(false == m_strBaseMapName.empty());
	m_hBaseMap = pDevice->CreateTexture( m_strBaseMapName.c_str(), RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE );
	if( !m_hBaseMap )
	{
		mlog("Failed to load baked map texture.\n");
		return false;
	}

	return true;
}

bool RTerrain::LoadFromXMLElement(MXmlElement *pRoot, bool bBackgroundCreation)
{
	if ( pRoot == NULL )
		return false;

	SetUsable(!bBackgroundCreation);
	SetVisible(!bBackgroundCreation);
	SetRenderDetailMap(ONLY_BAKED);

	// 워커와 워커블 확인
//	_ASSERT(m_pLoadingWorkUnit == NULL);	// NULL이아니면 두번 요청한 경우(허용되지 않음)

//	m_pLoadingWorkUnit = new RTerrainLoadingWorkUnit(this);

	//////////////////////////////////////////////////////////////////////////
	bool bMakeNaveMesh = _GETElement<bool>::Value(pRoot,"CREATE_NAVEMESH","bool");
	setCreateNaviMesh(bMakeNaveMesh);

	int nWidth, nHeight;
	nWidth = _GETElement<int>::Value(pRoot,"MAP_WIDTH","int");
	nHeight = _GETElement<int>::Value(pRoot,"MAP_HEIGHT","int");
	int nCount = min(nWidth,nHeight)/32;
	if ( nCount == 0 )
		nCount = 1;

	// 현 상황에서 min는 의미 없음. 정방형으로 한정지으므로 ...
	int nMapSize = min(nWidth, nHeight);
	int nPatchSize = PATCH_SIZE;
	m_iCount = nMapSize/nPatchSize;

	_ASSERT( fmod((float)nMapSize/(float)nPatchSize, 1.0f) == 0.0f);
	m_iPatchNum = m_iCount * m_iCount;

	RVector vMapSize;
	_GetElementValue(&vMapSize,pRoot, "MAP_SIZE","vec3");
	vMapSize.z = 1.0f;
	
	char pStr[MAX_PATH];
	memset(pStr, 0, sizeof(pStr));
	_Contents(pStr, pRoot->FirstChildElement("MAP_NAME"));

	m_strDisplacementMapName = pStr;
	// Displacement map 생성
	m_InfoDisplacementAndGrass.m_strDisplacementFileName = pStr;
	m_InfoDisplacementAndGrass.m_vMapSize = vMapSize;
	m_InfoDisplacementAndGrass.m_nPatchSize = nPatchSize;
	m_InfoDisplacementAndGrass.m_nLodLevel = LOD_LEVEL;
	m_InfoDisplacementAndGrass.m_bFillDone = false;
	m_InfoDisplacementAndGrass.m_bLoadDone = false;

	_Contents(m_strBaseMapName, pRoot->FirstChildElement("BASEMAP_NAME"));

	//////////////////////////////////////////////////////////////////////////

	// Layer
	int iSize = 0;
	MXmlElement *pVarElement = pRoot->FirstChildElement("LAYER");
	_Attribute(&iSize, pVarElement,"size");
	
	_ASSERT(m_iPatchNum);
	CreateLayer(iSize, m_iPatchNum);

	MXmlElement *pLAYERElement = pVarElement->FirstChildElement("LAYERINFO");
	for ( int i = 0; i<iSize; ++i )
	{
		int iIndex;
		_Attribute(&iIndex,pLAYERElement,"index");       
		_Contents(pStr, pLAYERElement->FirstChildElement("ALPHAMAP"));
		m_pLayerInfo[iIndex].alphaMapName = pStr;

		//////////////////////////////////////////////////////////////////////////
		// alpha map info
		m_deqInfoAlphaMap.push_back( ALPHA_MAP_INFO(iIndex, pStr) );
	
		_GetElementValue(&vMapSize,pLAYERElement, "TILEUV","vec3");

		//////////////////////////////////////////////////////////////////////////
		// tile map info
		MXmlElement* pTileMapElemet = pLAYERElement->FirstChildElement("TILEMAP");
		bool bUseSpecularTest = _Attribute(pStr, pTileMapElemet, "type");
		if(bUseSpecularTest)
		{
			if( _stricmp( pStr, "specular") != 0 )
				bUseSpecularTest = false;
		}

		_Contents(pStr, pTileMapElemet);
		m_pLayerInfo[iIndex].tileMapName = pStr;

		//////////////////////////////////////////////////////////////////////////
		// normal map info
		pStr[0] = '\0';
		_Contents(pStr, pLAYERElement->FirstChildElement("NORMALMAP"));
		m_pLayerInfo[iIndex].normalMapName = pStr;

		m_deqInfoTileMap.push_back( TILE_MAP_INFO(iIndex, m_pLayerInfo[iIndex].tileMapName.c_str(), m_pLayerInfo[iIndex].normalMapName.c_str(), vMapSize, bUseSpecularTest));

		pLAYERElement = pLAYERElement->NextSiblingElement();
	}

	// TODO: 라이트맵 로딩 안하게 - 포워드 렌더링을 삭제할때 완전히 지우자.
	//MXmlElement *pMapElement = pRoot->FirstChildElement("LIGHTMAP");
	//
	//int nLightMapNum = 0;
	//if ( pMapElement && pMapElement->Attribute("number",&nLightMapNum) == NULL )
	//{
	//	mlog("not have light map\n");
	//	nLightMapNum = 0;
	//}
	//ResizeLightmap(nLightMapNum);

	//for ( ;pMapElement != NULL; pMapElement = pMapElement->NextSiblingElement("LIGHTMAP"))
	//{
	//	int nIndex = 0;
	//	pMapElement->Attribute("index",&nIndex);
	//	if ( _Contents(pStr, pMapElement))
	//	{
	//		m_deqInfoLightMap.push_back( LIGHTMAP_INFO(nIndex, pStr));
	//		mlog("light map : %s \n", pStr);
	//		
	//		m_vecLightmap[nIndex].strLightMapName = pStr;
	//	}
	//}

	//if ( nLightMapNum )
	//	SetCurrentLightMap(true, 0);

	// Grass 파일이 있으면 읽기
	MXmlElement *pGrassElement = pRoot->FirstChildElement("GRASS");
	if( pGrassElement )
	{
		_Contents(pStr, pGrassElement);

		m_strGrassName = pStr;
		std::string strPathName(m_strPathName);

		strPathName += pStr;

		m_InfoDisplacementAndGrass.m_strGrassFileName = strPathName;
	}

//	AddLoadingController( this );
// 	if( bBackgroundCreation && bg_worker.IsEnabled())
// 	{
// 		bg_worker.AddToReadyQueue(m_pLoadingWorkUnit);
// 	}
// 	else
// 	{
// 		m_pLoadingWorkUnit->WorkInTheForeground();
// 		SAFE_DELETE(m_pLoadingWorkUnit);
// 	}

	return true;
}

bool RTerrain::IsLoadingCompleted()
{
	// TODO : IsLoadingCompleted()가 RSceneNode, RResource 두군데 모두 있음.
	return RResource::IsLoadingSucceeded();
}

void RTerrain::RenderGStageInferred( RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;

	RTerrainShader* pShader = NULL;
	RTerrainPassRenderer* pTerrainPassRenderer = ((RTerrainPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_TERRAIN)));
	if( pTerrainPassRenderer)
		pShader = pTerrainPassRenderer->GetTerrainShader();

	if(!pShader)
		return;

	RTechnique technique;
	RShaderFX* pFX = pShader->GetShaderForBakedMapGStage( 2, true);
	if( pFX)
	{
		SetRenderState( pCurrentCamera, pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetProjectionMatrix(), pShader);
		REngine::GetDevice().SetFvF(RTerrainPatch::RVt_Terrain::FVF);
		technique = pFX->GetTechnique( "MainTechnique");
		technique.Begin(NULL, TECH_PRESERVE_NOT);
		technique.BeginPass();
		for ( size_t si = 0; si<m_arrCulledPatch.size(); ++si)
		{
			RTerrainPatch *pDrawPatch = m_arrCulledPatch[si];
			int nDrawNum = RenderTerrainPatch(pDrawPatch);
			_ASSERT(nDrawNum!=0);
		}
		technique.EndPass();
		technique.End();
	}
}

void RTerrain::RenderGStageDeferred( RCameraSceneNode* pCurrentCamera)
{
 	RPFC_THISFUNC;

	RTerrainShader* pShader = NULL;
	RTerrainPassRenderer* pTerrainPassRenderer = ((RTerrainPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_TERRAIN)));
	if( pTerrainPassRenderer)
		pShader = pTerrainPassRenderer->GetTerrainShader();
 
 	SetRenderState( pCurrentCamera, pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetProjectionMatrix(), pShader);
 
	// 스플래팅 레이어 랜더
	RenderSplattingLayerOnePassGStage( pCurrentCamera, pShader);
	// 베이크드 레이어 랜더
	RenderBakedMapGStage( pCurrentCamera, false, pShader);

}


void RTerrain::RenderGStage( RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;
	if( GetUsable() == false ) return;
	if( GetVisible() == false ) return;
	if( GetOcclued() == true ) return;

	if ( m_pGrassSystem )
		m_pGrassSystem->RenderGStage( renderMethod, pCurrentCamera);	


	switch( renderMethod)
	{
	case RENDERING_INFERRED:
		RenderGStageInferred( pCurrentCamera);
		break;
	case RENDERING_DEFERRED:
		RenderGStageDeferred( pCurrentCamera);
		break;
	}
	
}

void RTerrain::RenderMStage( RCameraSceneNode* pCurrentCamera, RRENDERINGMETHOD renderMethod)
{
	RPFC_THISFUNC;

	if( renderMethod != RENDERING_DEFERRED )
	{
		RTerrainShader* pShader = NULL;
		RTerrainPassRenderer* pTerrainPassRenderer = ((RTerrainPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_TERRAIN)));
		if( pTerrainPassRenderer)
			pShader = pTerrainPassRenderer->GetTerrainShader();

		SetRenderState( pCurrentCamera, pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetProjectionMatrix(), pShader);

		// 오버드로우를 최소화 하기 위해 풀부터 렌더링한다.
		if ( m_pGrassSystem )
			m_pGrassSystem->RenderMStage( pCurrentCamera);

		// 터레인에서(Terrain.fx)는 뷰 스페이스 라이트 맵 샘플러 스테이지를 11로 사용
		RTexture* pViewLightMap = REngine::GetDevice().GetTexture( AS_VIEWSPACE_LIGHTMAP);
		REngine::GetDevice().SetTexture( TS_VIEWSPACE_LIGHTMAP, pViewLightMap);
		REngine::GetDevice().SetTextureAddressClamp(TS_VIEWSPACE_LIGHTMAP);
		REngine::GetDevice().SetTextureFilter(TS_VIEWSPACE_LIGHTMAP, RTF_POINT);
		// 뷰 스페이스 라이트맵을 VPOS로 샘플링 하기 위해서 화면 사이즈 설정 필요
		if( pShader)
		{
			RTextureInfo info = pViewLightMap->GetInfo();
			pShader->m_scResolution.SetVector2( RVector2( info.nTextureWidth, info.nTextureHeight));
		}

		RenderBakedMapMStage( pCurrentCamera, true, pShader);
		RenderSplattingLayerOnePassMStage( pCurrentCamera, pShader);


		// 뷰 스페이스 노말을 위에서 변경했으므로 다시 돌려놓자
		REngine::GetDevice().SetTexture( AS_VIEWSPACE_LIGHTMAP, pViewLightMap);
		REngine::GetDevice().SetTextureAddressClamp(AS_VIEWSPACE_LIGHTMAP);
		REngine::GetDevice().SetTextureFilter(AS_VIEWSPACE_LIGHTMAP, RTF_POINT);	
	}

	if( m_bGridFlag || m_nMarkingLayerNumber > -1)
	{
		RTerrainShader* pShader = NULL;
		RTerrainPassRenderer* pTerrainPassRenderer = ((RTerrainPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_TERRAIN)));
		if( pTerrainPassRenderer)
			pShader = pTerrainPassRenderer->GetTerrainShader();

		SetRenderState( pCurrentCamera, pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetProjectionMatrix(), pShader);

		if( m_bGridFlag)
			RenderGrid( pCurrentCamera, pShader);

		if( m_nMarkingLayerNumber > -1)
			RenderMarkingLayer( pCurrentCamera, pShader, m_nMarkingLayerNumber);
	}

	
}



void RTerrain::RenderZStage( RRENDERINGMETHOD renderMethod, RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;
	if( GetUsable() == false ) return;
	if( GetVisible() == false ) return;
	if( GetOcclued() == true ) return;

	RShaderCompositeManager* pCompositeManager = REngine::GetSceneManagerPtr()->GetShaderCompositeManager();
	if(pCompositeManager->GetViewLightingOnly()) return;

	// Set Render State
	RDevice *pDevice = REngine::GetDevicePtr();
	_ASSERT(pDevice);

	RTerrainShader* pShader = NULL;
	RTerrainPassRenderer* pTerrainPassRenderer = ((RTerrainPassRenderer*)(REngine::GetSceneManager().GetPassRenderer( RRP_TERRAIN)));
	if( pTerrainPassRenderer)
		pShader = pTerrainPassRenderer->GetTerrainShader();

	SetRenderState( pCurrentCamera, pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetProjectionMatrix(), pShader);

	// Draw Primitives
	RTechnique technique;
	if( pShader)
	{
		RShaderFX* pFX = pShader->GetShaderForGrid( 1, false );
		if( pFX)
		{
			technique = pFX->GetTechnique( "MainTechnique");
			RenderSingleLayer( &technique, m_pTerrain->m_arrCulledPatch, pShader);
		}
	}

}



bool RTerrain::CalculateLOD( RCameraSceneNode* pCamera, const RSphere* pBoundingSphere)
{
	RPFC_THISFUNC;

	if( !GetVisible() || !GetUsable() || GetOcclued() )
		return false;

	int nX,nY,nZ;
	GetPatchXYZ(&nX,&nY,&nZ, &pCamera->GetWorldPosition());
	CalcLODLevel(&pCamera->GetWorldPosition(), nX, nY, nZ);

	return true;
}


inline TERRAIN_TILE_LEVEL RTerrain::CalcDetail(const RVector *pCameraPos, const RBoundingSphere *pSphere)
{
	if( m_fDetailDistance <= 0)
		return TTL_SIMPLE;

	float fLengthSq = (*pCameraPos - pSphere->center).LengthSq();
	float fSphereRadiusSq = pSphere->radius * pSphere->radius;
	float fDetailDistanceSq = m_fDetailDistanceSq + fSphereRadiusSq;

	// If true, It's detial.
	// Baked인지 아닌지를 먼저 판별 후
	if( fLengthSq < fDetailDistanceSq)
	{
		// 디테일의 레벨을 결정.
		float fDetailDistanceSq = m_fDetailDistanceHalfSq + fSphereRadiusSq;
		if( fLengthSq < fDetailDistanceSq)
			return TTL_DETAIL_HIGH;
		else
			return TTL_DETAIL_LOW;
	}
	else
		return TTL_SIMPLE;

	//return ( (*pCameraPos - pSphere->center).LengthSq() < (m_fDetailDistance*m_fDetailDistance) ? true : false );	// 베이크드 맵의 범위가 넓어진다. 패치의 크기와 상관 없이 결정 되므로 이 방법은...
}



}
