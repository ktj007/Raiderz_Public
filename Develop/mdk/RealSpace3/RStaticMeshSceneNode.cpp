#include "stdafx.h"

#include "MXml.h"
#include "MDebug.h"
#include "RDevice.h"
#include "RVersions.h"
#include "RUtil.h"
#include "RXMLTemplate.h"
#include "RStaticMeshSceneNode.h"
#include "RProfileInfo.h"
#include "RSceneManager.h"

#include "RMaterial.h"
#include "RMaterialResource.h"
#include "RMaterialResourceMgr.h"
// #include "RRenderHelper.h"
// 
// #include "REnvironmentSet.h"
#include "RShaderComposite.h"
#include "RShaderCompositeManager.h"
#include "RCollisionTreeBuilder.h"
#include "FileInfo.h"
#include "RToken.h"

#include "RLightSceneNode.h"
#include "RPointLightSceneNode.h"
#include "RDirectionalLightSceneNode.h"
#include "RShadowStrategy.h"

#include "RStaticMeshRenderStrategy.h"
#include "RStaticMeshDepthRenderStrategy.h"
#include "RStaticMeshGStageRenderStrategy.h"
#include "RStaticMeshMStageRenderStrategy.h"

#include "RRenderHelper.h"
// 포탈 표시용
//#define DEBUG_POTAL_RENDERING

namespace rs3 {

RRsRenderHelper RStaticMeshSceneNode::m_rsRenderer;

MImplementRTTI(RStaticMeshSceneNode, RSceneNode);

RStaticMeshSceneNode::RStaticMeshSceneNode()
: RSceneNode(RRP_STATICMESH)
, m_nCurrentCell(0)
, m_unRenderCounter(0)
, m_nCurrentLightCount(0)
, m_pRenderStrategy(NULL)
, m_pDepthRenderStrategy(NULL)
, m_pGStageRenderStrategy(NULL)
, m_pCurrentRenderStrategy(NULL)
, m_pMStageRenderStrategy(NULL)
, m_bEnablePicking(false)
{
	SetUsable(false);
	SetVisible(false);
	m_bPointColor = false;

	m_fRemainedBlendingTime = -1;

	m_pRenderStrategy = new RStaticMeshRenderStrategy(this);
	m_pDepthRenderStrategy = new RStaticMeshDepthRenderStrategy(this);
	m_pGStageRenderStrategy = new RStaticMeshGStageRenderStrategy(this);
	m_pMStageRenderStrategy = new RStaticMeshMStageRenderStrategy(this);
}

RStaticMeshSceneNode::~RStaticMeshSceneNode()
{
	delete m_pRenderStrategy;
	delete m_pDepthRenderStrategy;
	delete m_pGStageRenderStrategy;
	delete m_pMStageRenderStrategy;
}

RShaderCompositeManager* RStaticMeshSceneNode::m_pShaderCompositeManager = NULL;
REnvironmentSet* RStaticMeshSceneNode::m_pEnvironmentSet = NULL;

bool RStaticMeshSceneNode::CreateSharedResources()
{
	m_pShaderCompositeManager = REngine::GetSceneManager().GetShaderCompositeManager();
//	m_pShaderCompositeManager는 엔진 초기화 RActor::CreateCheckShader()에서 Create()됨

	return true;
}

void RStaticMeshSceneNode::Destroy()
{
}


//RPOLYGONINFO *g_pTestPickInfo = NULL;

int aabbmethod = 1;

void RStaticMeshSceneNode::Render( RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;
	REngine::GetDevice().SetVertexFormat(NULL);

	m_nRenderedTriangles = 0;

	m_bPointColor = false;
	for( int i = 0; i < 2; ++i )
	{
		for( size_t j = 0; j < m_renderNodes[i].size(); ++j )
		{
			bool bRender = true;
			RRENDERINFO& renderInfo = m_renderNodes[i][j];
			RGeometry* pGeometry = renderInfo.pGeometry;
			RRsAABBTree* pAABBTree = renderInfo.pAABBTree;
			RRsNode* pNode = renderInfo.pNode;

			REngine::GetDevice().SetFvF(pGeometry->m_dwFVF);
			REngine::GetDevice().SetVertexBuffer(pGeometry->m_hVertexBuffer,0,0);

			if( m_pCurrentRenderStrategy )
			{
				if( bRender = m_pCurrentRenderStrategy->CheckAndSetTechnique( pCurrentCamera, &pAABBTree->m_renderInfo, pAABBTree->m_nLightmapID, ((RCameraSceneNode*)pCurrentCamera)->GetFogEnable()))
					m_pCurrentRenderStrategy->BeginTechnique();
			}

			if( bRender)
			{
				int nRenderTriangles = pNode->m_nIndicies / 3;

				REngine::GetDevice().SetIndexBuffer( pNode->m_hIB );
				bool bOk = REngine::GetDevice().DrawIndexedPrimitive(
					RPT_TRIANGLELIST,
					pAABBTree->m_nBaseIndex,
					pNode->m_nMinIndex,
					pNode->m_nMaxIndex - pNode->m_nMinIndex + 1,
					0,
					nRenderTriangles );

				m_nRenderedTriangles += nRenderTriangles;
				REngine::GetResourceProfiler().AddPolygon(PPT_AABBTREE, nRenderTriangles);
				_ASSERT(bOk);

				if( m_pCurrentRenderStrategy )
					m_pCurrentRenderStrategy->EndTechnique();
			}
		}
		m_bPointColor = true;
	}

#ifdef DEBUG_POTAL_RENDERING
	// 디버그용 포탈을 그린다.
	REngine::GetDevice().SetDepthEnable( false );
	RRenderHelper::SetRenderState();
	RRenderHelper::m_batchLineDrawer.Render( RMatrix::IDENTITY, 0xffffffff );
	REngine::GetDevice().SetDepthEnable( true );
#endif

//	REngine::GetDevice().SetShader(R_NONE);

	/*
	// 테스트
	if(g_pTestPickInfo)
	{
		REngine::GetDevice().SetShader(R_NONE);

		int nBaseIndex = g_pTestPickInfo->nBaseIndex;

		RSector *pSector = m_sectors[0];
		RRenderHelper::RenderLine(RMatrix::IDENTITY,pSector->m_pVertices[nBaseIndex ].pos,pSector->m_pVertices[nBaseIndex+1].pos,0xffff0000);
		RRenderHelper::RenderLine(RMatrix::IDENTITY,pSector->m_pVertices[nBaseIndex+1 ].pos,pSector->m_pVertices[nBaseIndex+2].pos,0xffff0000);
		RRenderHelper::RenderLine(RMatrix::IDENTITY,pSector->m_pVertices[nBaseIndex+2 ].pos,pSector->m_pVertices[nBaseIndex].pos,0xffff0000);

		// normal
		for(int i=0;i<3;i++)
		RRenderHelper::RenderLine(RMatrix::IDENTITY,pSector->m_pVertices[nBaseIndex +i].pos,
			pSector->m_pVertices[nBaseIndex +i].pos+pSector->m_pVertices[nBaseIndex +i].normal*20.f,0xffffff00);

	}
	*/
}

void RStaticMeshSceneNode::RenderRaw( RCameraSceneNode* pCurrentCamera)
{
	if( !GetUsable() ) return;

	m_pCurrentRenderStrategy = NULL;
	Render( pCurrentCamera);
}

void RStaticMeshSceneNode::OnRenderShadowMap( RCameraSceneNode* pCurrentCamera, RShadowStrategy* pShadowStrategy )
{
	if( !GetUsable() ) return;

	m_pCurrentRenderStrategy = m_pDepthRenderStrategy;

	RShaderCompositeManager::SetTransformConstant(GetWorldTransform(), pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix());

	REngine::GetDevice().SetBlending(RBLEND_NONE);

	Render( pCurrentCamera);
}

void RStaticMeshSceneNode::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMothod)
{
	if( !GetUsable() )
		return;

//	m_pEnvironmentSet = m_pShaderCompositeManager->UpdateEnvironmentSet();

	// Visiblity 초기화
	RShaderCompositeManager::m_cVisiblity.SetFloat(1.f);
	RShaderCompositeManager::SetTransformConstant(GetWorldTransform(), pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix());

	//////////////////////////////////////////////////////////////////////////
	// 렌더
	m_pCurrentRenderStrategy = m_pRenderStrategy;
	Render( pCurrentCamera);

	//////////////////////////////////////////////////////////////////////////
	// state 복구
	RestoreRenderState();
}

int RStaticMeshSceneNode::FindCell(const RVector& vecPosition)
{
	// 0 번은 글로벌 셀이므로 1번부터 검사후
	if(m_cells.empty()) return 0;

	for(int i=1;i<(int)m_cells.size();++i)
	{
		RCell *pCell = m_cells[i];
		
		if(pCell->Contains(vecPosition))
			return i;
	}

	// 아무데도 속해있지 않으면 global cell로 판정
	return 0;
}

bool RStaticMeshSceneNode::Pick(RRsNode *pNode, RSTATICMESHPICKINFO& info, RVertexOffsetInfo& VertexInfo)
{
	// optimize 이노드의 바운딩박스까지의 거리가 현재까지 pick된 거리보다 멀면 검사할필요없다.
	// bb와 교차하면 섹터안의 폴리곤과 교차검사한다
	if(pNode->m_bb.ContainsLineSegment(info.m_pickOrigin,info.m_pickTo)) 
	{
		if(pNode->IsLeafNode()) 
		{
			bool bPickFound = false;
			for(int i = 0; i < pNode->m_nPolygon; ++i) 
			{
				RVector pickPos;
				for (int j = 0; j < pNode->m_pPolygonInfo[i].nVertices-2; ++j)
				{
					bool	bPicked;
					char*	pVertex = pNode->m_pPolygonInfo[i].pVertices + ( j * VertexInfo.size + VertexInfo.pos );

					// 삼각형 구축이 PAN 방식임에 유의
					if(info.m_bBackFaceCull)
						bPicked = MMath::IsIntersectNoCull(info.m_pickOrigin,info.m_pickDir,
							*(RVector*)(pNode->m_pPolygonInfo[i].pVertices),
							*(RVector*)(pVertex + (VertexInfo.size * 1)),
							*(RVector*)(pVertex + (VertexInfo.size * 2)), &pickPos);
					else
						bPicked = MMath::IsIntersect(info.m_pickOrigin,info.m_pickDir,
							*(RVector*)(pNode->m_pPolygonInfo[i].pVertices),
							*(RVector*)(pVertex + (VertexInfo.size * 1)),
							*(RVector*)(pVertex + (VertexInfo.size * 2)),&pickPos);

					if(bPicked) 
					{
						float lengthSq = (pickPos-info.m_pickOrigin).LengthSq();
						if(info.m_fPickDistanceSq>lengthSq) {
							bPickFound = true;
							info.m_pPickInfo->outPosition = pickPos;
							info.m_fPickDistanceSq = lengthSq;
						}
					}
				}
			}
			return bPickFound;

		}
		else
		{
			bool bPickedLeft	= Pick( pNode->m_pChildren[0], info, VertexInfo );
			bool bPickedRight	= Pick( pNode->m_pChildren[1], info, VertexInfo );

			return bPickedLeft || bPickedRight;
		}
	}

	return false;
}

bool RStaticMeshSceneNode::OnPick(RPICKINFO& pickInfo)
{
	RSTATICMESHPICKINFO info;

	info.m_bPickFound = false;

	RMatrix matInverse;
	GetWorldTransform().GetInverse(&matInverse);
	matInverse.TransformVect(pickInfo.inPickOrigin, info.m_pickOrigin);
	matInverse.TransformVect(pickInfo.inPickTo, info.m_pickTo);

//	m_pickOrigin = origin;
//	m_pickTo = to;
	info.m_pickDir = (info.m_pickTo-info.m_pickOrigin).Normalize();
	info.m_pPickInfo = &pickInfo;
	info.m_fPickDistanceSq = (pickInfo.inPickTo-pickInfo.inPickOrigin).LengthSq();

	// 모든 섹터에 대해 검사한다.
	for(int i=0;i<(int)m_cells.size();++i)
	{
		RCell *pCell = m_cells[i];
		m_pCurrentCell = pCell;

		for(RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin(); itr != pCell->m_mapGeometryList.end(); ++itr)
		{
			RGeometry* pGeometry = itr->second;
			for(int j = 0; j < pGeometry->m_nTrees; ++j)
			{
				RRsAABBTree	*pRsAABBTree = pGeometry->m_pTrees[j];
				info.m_bPickFound |= Pick(pRsAABBTree->m_pRootNode,info, pGeometry->m_VertexInfo);
			}
		}
	}
	if(info.m_bPickFound)
	{
		pickInfo.outNode = this;
		GetWorldTransform().TransformVect(pickInfo.outPosition);
	}
	return info.m_bPickFound;
}

void RStaticMeshSceneNode::UpdateAABB()
{
	// obb -> aabb

	RMatrix tm = GetWorldTransform();

	RBoundingBox box;
	box.Initialize();

	// TODO: tm transform 으로 정리
	for(int i=0;i<(int)m_cells.size();++i)
	{
		for(RCell::IDGEOMETRYMAP::iterator itr = m_cells[i]->m_mapGeometryList.begin(); itr != m_cells[i]->m_mapGeometryList.end(); ++itr)
		{
			RGeometry* pGeometry = itr->second;
			for(int j = 0; j < pGeometry->m_nTrees; ++j)
			{
				RBoundingBox& bbCell = pGeometry->m_pTrees[j]->m_pRootNode->m_bb;

				for(int k=0;k<8;++k)
				{
					RVector v;
					v.x = ((k&1)==0) ? bbCell.vmin.x : bbCell.vmax.x;
					v.y = ((k&2)==0) ? bbCell.vmin.y : bbCell.vmax.y;
					v.z = ((k&4)==0) ? bbCell.vmin.z : bbCell.vmax.z;
					v = v*tm;
					box.Add(v);
				}
			}
		}
	}
	SetAABB( box );
}

void RStaticMeshSceneNode::OnUpdateAABB()
{
	UpdateAABB();
}

int	RStaticMeshSceneNode::CalcPriority()
{
	return RBGWK_HIGHEST;
}

void RStaticMeshSceneNode::OnLoadingComplete(RResource* pResource)
{
	if( pResource==this && pResource->IsLoadingSucceeded() )
	{
		SetUsable(true);
		SetVisible(true);
		UpdateAABB();
	}
}

bool RStaticMeshSceneNode::CreateFromXML(MXmlElement &element, bool bBackgroundCreation)
{
	if(!CreateSharedResources()) return false;

	string strFileName;
	MXmlElement *pElement = element.FirstChildElement("FILENAME");
	_Contents(strFileName,pElement);

	//피킹가능한가 로드
	bool bEnablePicking = TXML::_GETElement<bool>::Value(pElement,"ENABLE_PICKING","bool");
	setEnalbePicking(bEnablePicking);

	// 내 뿌리를 찾아서 Path를 얻어야 한다.
	RWorldSceneNode* pRoot = NULL;
	if(!pRoot)
		pRoot = RSceneNode::GetRootSceneNode();
	if( pRoot)
		strFileName = pRoot->GetXMLFilePath() + strFileName ;

	// staticmeshResource를 백그라운드 로딩중에 해제하는경우 뻗는 문제가 있다
	// staticmesh 는 앞으로 지원예정이 없으므로 강제로 백그라운드 로딩 기능은 없앤다
	bBackgroundCreation = false;

	// description 
	char xmlName[_MAX_PATH];
	sprintf( xmlName, "%s.xml", strFileName.c_str() );
	if( !OpenDescription(xmlName, bBackgroundCreation) )
	{
		mlog( "Error while loading %s\n", xmlName );
		return false;
	}

	m_strFileName = strFileName;

	return REngine::GetSceneManagerPtr()->AddLoadingList( this, bBackgroundCreation, this );
}

void RStaticMeshSceneNode::SaveToXML(MXmlElement &element)
{
	MXmlElement *pElement = new MXmlElement("FILENAME");
	
	char drive[_MAX_DRIVE],dir[_MAX_DIR],filename[_MAX_FNAME],ext[_MAX_EXT];
	_splitpath(m_strFileName.c_str(),drive,dir,filename,ext);

	char szFileName[256];
	sprintf(szFileName,"%s%s",filename,ext);

	_SetContents(pElement, szFileName);

	TXML::_SetElementValue(pElement,"ENABLE_PICKING","bool",m_bEnablePicking);
	
	element.LinkEndChild(pElement);
}

void RStaticMeshSceneNode::Cull(RCameraSceneNode* pCamera,const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar)
{
	RPFC_THISFUNC;

	ClearRenderNodes();

//	m_nCullTestCount = 0;

	m_nRenderedNodes = 0;
	m_nRenderedCells = 0;

	m_pCullingNearPlane = pPlaneNear;
	m_pCullingFarPlane = pPlaneFar;

	++m_unRenderCounter;

	m_currentCameraPosition = pCamera->GetWorldPosition();	// 카메라가 위치한 섹터를 찾는다
	m_nCurrentCell = FindCell(m_currentCameraPosition);


	// culling planes 초기화

	// TODO: 컬링 평면을 로컬 좌표계로 옮기는 것을 일단 주석처리. 이게 제대로 적용되려면 포탈, 셀도 같이 옮겨져야 함. - charmke
	// 처음 컬링 평면은 viewfrustum 이다.
	//const RViewFrustum& viewFrustum = pCurrentCamera->GetViewFrustum();
	//RMatrix matInv;
	//GetWorldTransform().GetInverse(&matInv);

	//RPlane viewFrustumPlanes[4];	// aabbtree의 로컬좌표계에서의 view-frustum 평면들
	//for(int i=0;i<4;i++)
	//{
	//	m_matInv.TransformPlane(viewFrustum.GetPlanes()[i],viewFrustumPlanes[i]);
	//}

	//RPlane clipPlanes[6];
	m_nUserCullingPlanes = pCamera->GetUserClipPlaneNum();
	if( m_nUserCullingPlanes )
	{
	//	for( int i = 0; i < m_nUserCullingPlanes; ++i )
	//		matInv.TransformPlane( pCurrentCamera->GetUserClipPlane()[i], clipPlanes[i] );

	//	m_pUserCullingPlanes = &clipPlanes;
		m_pUserCullingPlanes = pCamera->GetUserClipPlane();
	}
	else
	{
		m_pUserCullingPlanes = NULL;
	}

	for( size_t i = 0; i < cullDescs.size(); ++i )
	{
		const RCULLDESC& cullDesc = cullDescs[i];
		CullRecurse( m_cells[m_nCurrentCell], cullDesc.pPlanes, cullDesc.nPlanes );
	}

	// 카메라가 추적하는 대상이 있다면 그 대상위치에서 한번 더 컬링
	RVector camTargetPos;
	if( pCamera->GetTargetPosition( camTargetPos ) )
	{
		int nCurrentCell = FindCell( camTargetPos );
		if( nCurrentCell != m_nCurrentCell )
		{
			m_nCurrentCell = nCurrentCell;
			for( size_t i = 0; i < cullDescs.size(); ++i )
			{
				const RCULLDESC& cullDesc = cullDescs[i];
				CullRecurse( m_cells[m_nCurrentCell], cullDesc.pPlanes, cullDesc.nPlanes );
			}
		}
	}

//	mlog3("Cull Count: %d\n", m_nCullTestCount );
}

void RStaticMeshSceneNode::CullRecurse(RCell *pCell,const RPlane *pPortalPlanes, int nPlanes)
{
	RPFC_THISFUNC;
	++m_nRenderedCells;

	m_pCullingPlanes = pPortalPlanes;
	m_nCullingPlanes = nPlanes;

	for(RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin(); itr != pCell->m_mapGeometryList.end(); ++itr)
	{
		RGeometry* pGeometry = itr->second;
		CullGeometry( pGeometry );
	}

	// 현재의 섹터에 연결된 (보이는) 포탈이 있으면 추가
	for(RPortalList::iterator i = pCell->m_portals.begin(); i != pCell->m_portals.end(); ++i )
	{
		RPortal *pPortal = i->first;

		// 만약 카메라가 포탈의 뒤에 있다면 보이지 않는 것이다
		if( pPortal->m_plane.GetDistanceTo( m_currentCameraPosition) <= 0 ) continue;

		// 새 포탈 평면을 구한다.
		vector<RPlane> newPortalPlanes;
		if( ClipPlanes( m_currentCameraPosition, pPortalPlanes, nPlanes, pPortal->m_pVertices, pPortal->m_nVertices, newPortalPlanes, true ) )
		{
			// 무한 루프 방지. 카메라가 있는 셀을 다시 만나면 컬링을 중단한다.
			if( i->second != m_cells[m_nCurrentCell] )
			{
				// 재귀호출로 연결된 섹터를 컬링한다.
				CullRecurse( i->second, &newPortalPlanes[0], newPortalPlanes.size() );
			}
		}
	}
}

void RStaticMeshSceneNode::CullGeometry(RGeometry* pGeometry)
{
	// V3에서 새로 생긴 규칙 FVF가 0 이면 InVisible 그룹 이다. 그리지 않는다.
	if( pGeometry->m_dwFVF == 0 )
		return;

	for(int j = 0; j < pGeometry->m_nTrees; ++j)
		CullNode( pGeometry, pGeometry->m_pTrees[j], pGeometry->m_pTrees[j]->m_pRootNode, true );
}

void RStaticMeshSceneNode::CullNode(RGeometry* pGeometry, RRsAABBTree* pAABBTree, RRsNode* pNode, bool bCulling)
{
	bool bCullingTest = bCulling;
	if( bCulling )
	{
	//	m_nCullTestCount++;

		int cullResult = CullTest( pNode );
		if( cullResult == CULL_INSIDE )
		{
			bCullingTest = false;
		}
		else
		if( cullResult == CULL_OUTSIDE )
		{
			return;
		}
	}

	if(pNode->IsLeafNode())
	{
		// 노드 단위 중복 렌더링 체크
		if( pNode->m_unRenderCounter == m_unRenderCounter )
			return;

		pNode->m_unRenderCounter = m_unRenderCounter;

		if( pNode->m_nIndicies )
		{
			RRENDERINFO renderInfo;
			renderInfo.pGeometry = pGeometry;
			renderInfo.pAABBTree = pAABBTree;
			renderInfo.pNode = pNode;

			bool bPointColor = (pGeometry->m_VertexInfo.color >= 0) ? true : false;

			if( bPointColor )
				m_renderNodes[1].push_back( renderInfo );
			else
				m_renderNodes[0].push_back( renderInfo );

			++m_nRenderedNodes;
		}
	}
	else
	{
		CullNode(pGeometry, pAABBTree, pNode->m_pChildren[0], bCullingTest);
		CullNode(pGeometry, pAABBTree, pNode->m_pChildren[1], bCullingTest);
	}
}

int RStaticMeshSceneNode::CullTest(RRsNode* pNode)
{
	const RBox& bb = pNode->m_bb;

	for( int i = 0; i < m_nCullingPlanes; ++i )
	{
		if( bb.GetDistance( m_pCullingPlanes[i] ) < 0.f )
			return CULL_OUTSIDE;
	}

	for( int i = 0; i < m_nUserCullingPlanes; ++i )
	{
		if( bb.GetDistance( m_pUserCullingPlanes[i] ) < 0.f )
			return CULL_OUTSIDE;
	}

	if( m_pCullingNearPlane && bb.GetDistance( *m_pCullingNearPlane ) < 0.f )
		return CULL_OUTSIDE;

	if( m_pCullingFarPlane && bb.GetDistance( *m_pCullingFarPlane ) < 0.f )
		return CULL_OUTSIDE;

	for( int i = 0; i < m_nCullingPlanes; ++i )
	{
		if( bb.GetDistanceReverse( m_pCullingPlanes[i] ) > 0.f )
			return CULL_PARTIAL;
	}

	for( int i = 0; i < m_nUserCullingPlanes; ++i )
	{
		if( bb.GetDistanceReverse( m_pUserCullingPlanes[i] ) > 0.f )
			return CULL_PARTIAL;
	}

	if( m_pCullingNearPlane && bb.GetDistanceReverse( *m_pCullingNearPlane ) > 0.f )
		return CULL_PARTIAL;

	if( m_pCullingFarPlane && bb.GetDistanceReverse( *m_pCullingFarPlane ) > 0.f )
		return CULL_PARTIAL;

	return CULL_INSIDE;
}

bool RStaticMeshSceneNode::ClipPlanes(const RVector& pos, const RPlane *pClipPlanes, int nClipPlanes,
	const RVector* pPlaneVertices, int nPlaneVertices, vector<RPlane> &outClippedPlanes, bool bDebugRender )
{
	// 불필요한 memcpy를 막고자 두개의 버퍼를 두었다
	vector<RVector> pTempVertices[2];

	for( int i = 0; i < nPlaneVertices; ++i )
		pTempVertices[1].push_back( pPlaneVertices[i] );

	vector<RVector>* pSrc = &pTempVertices[1];
	vector<RVector>* pDest = &pTempVertices[0];

	for( int i = 0; i < nClipPlanes; ++i )
	{
		MMath::ClipVertices( *pDest, *pSrc, pClipPlanes[i] );

		// 두개의 버텍스 포인터를 swap
		pSrc = pDest;
		if( pDest == &pTempVertices[0] )
			pDest = &pTempVertices[1];
		else
			pDest = &pTempVertices[0];

		if( pSrc->empty() )
			break;
	}

	outClippedPlanes.clear();
	size_t srcSize = pSrc->size();
	if( srcSize )
	{
		// 버텍스들로 평면을만든다
		for( size_t i = 0; i < srcSize; ++i )
		{
			RVector *pV1 = &(*pSrc)[i];
			RVector *pV2 = &(*pSrc)[(i+1)%srcSize];
			outClippedPlanes.push_back( RPlane( *pV2, *pV1, pos ) );
		}

	#ifdef DEBUG_POTAL_RENDERING
		if( bDebugRender )
		{
			// 디버그용 : 포탈을 그린다.
			for( size_t i = 0; i < srcSize; i++ )
			{
				RRenderHelper::m_batchLineDrawer.Add( (*pSrc)[i], (*pSrc)[(i+1)%srcSize] );
			}
		}
	#endif

		return true;
	}

	return false;
}

void RStaticMeshSceneNode::ClearRenderNodes()
{
	m_renderNodes[0].clear();
	m_renderNodes[1].clear();
}

void RStaticMeshSceneNode::RestoreRenderState()
{
	RPFC_THISFUNC;
	RDevice* pDevice = REngine::GetDevicePtr();

	pDevice->SetBlending(RBLEND_NONE, RBLEND_NONE);
	pDevice->SetTexture(AS_DEPTHMAP,NULL);
	pDevice->SetTexture(AS_CLOUDMAP,NULL);
}

void RStaticMeshSceneNode::GetVertexFromBound(RRsNode* pNode, std::vector<RVt_pos>* vecVertex, const RBoundingBox &aabb, RVertexOffsetInfo& VertexInfo)
{
	RPFC_THISFUNC;
	if(!pNode->m_bb.ContainsAABB(aabb))
		return;

	if(pNode->IsLeafNode())
	{
		for(int i = 0;i<pNode->m_nPolygon;++i) {
			for (int j=0;j<pNode->m_pPolygonInfo[i].nVertices-2; ++j)
			{
				char*	pVertex = pNode->m_pPolygonInfo[i].pVertices + ( j * VertexInfo.size + VertexInfo.pos );

				// 삼각형 구축이 FAN 방식임에 유의
				RVector &vP0 = *(RVector*)(pNode->m_pPolygonInfo[i].pVertices);
				RVector &vP1 = *(RVector*)(pVertex + (VertexInfo.size * 1));
				RVector &vP2 = *(RVector*)(pVertex + (VertexInfo.size * 2));
		
				bool bResult = IsIntersect(aabb, vP0, vP1, vP2);
				if ( bResult )
				{
					RVt_pos vertex;
					m_matWorld->TransformVect(vP0, vertex.vPos);
					//vertex.vPos = vP0;
					(*vecVertex).push_back(vertex);
					m_matWorld->TransformVect(vP1, vertex.vPos);
					//vertex.vPos = vP1;
					(*vecVertex).push_back(vertex);
					m_matWorld->TransformVect(vP2, vertex.vPos);
					//vertex.vPos = vP2;
					(*vecVertex).push_back(vertex);
				}
			}
		}
	}
	else
	{
		GetVertexFromBound((RRsNode*)pNode->m_pChildren[0], vecVertex, aabb, VertexInfo);
		GetVertexFromBound((RRsNode*)pNode->m_pChildren[1], vecVertex, aabb, VertexInfo);
	}
}

int RStaticMeshSceneNode::GetVertexFromBound(std::vector<RVt_pos>* vecVertex, const RBoundingBox &aabb)
{
	RBoundingBox trBox;
	RMatrix invWorld;
	GetWorldTransform().GetInverse(&invWorld);
	TransformBox(&trBox,aabb,invWorld);

	for(int i=0;i<(int)m_cells.size();++i)
	{
		RCell *pCell = m_cells[i];
		for(RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin(); itr != pCell->m_mapGeometryList.end(); ++itr)
		{
			RGeometry* pGeometry = itr->second;
			for(int j=0;j<pGeometry->m_nTrees;++j)
			{
				RRsAABBTree	*pRsAABBTree = pGeometry->m_pTrees[j];
				GetVertexFromBound(pRsAABBTree->m_pRootNode, vecVertex, trBox, pGeometry->m_VertexInfo);
			}
		}
	}
	return (*vecVertex).size();
}

void RStaticMeshSceneNode::OnAddScene()
{
	if( RSceneNode::GetRootSceneNode() )
	{
		RSceneNode::GetRootSceneNode()->SetStaticMeshSceneNode( this);	
	}
}

void RStaticMeshSceneNode::OnRemoveScene()
{
	if( RSceneNode::GetRootSceneNode() )
	{
		RSceneNode::GetRootSceneNode()->SetStaticMeshSceneNode(NULL);	
	}
}

void RStaticMeshSceneNode::CollectStaticCollisionPolygons(RRsNode* pNode, RSceneNodePolygonCollector* pCollector, RVertexOffsetInfo& VertexInfo) const
{
	if(pNode->IsLeafNode())
	{
		for(int i = 0;i<pNode->m_nPolygon;++i) 
		{
			for (int j=0;j<pNode->m_pPolygonInfo[i].nVertices-2;++j)
			{
				// Set_Collision 이 없으면 수집하지 않는다.
				if( ( pNode->m_pPolygonInfo[i].dwFlags & RM_FLAG_COLLISION_MESH ) == false )
					continue;

				const string &strMaterialName = m_vecMaterials[ pNode->m_pPolygonInfo[i].nMaterial]->GetName();
				DWORD nZoneMaterialID = pCollector->GetZoneMaterialID(strMaterialName);

				char*	pVertex = pNode->m_pPolygonInfo[i].pVertices + ( j * VertexInfo.size + VertexInfo.pos );
			
				// 삼각형 구축이 PAN 방식임에 유의
				pCollector->OnAddPolygon(
					*(RVector*)(pNode->m_pPolygonInfo[i].pVertices),
					*(RVector*)(pVertex + (VertexInfo.size * 1)),
					*(RVector*)(pVertex + (VertexInfo.size * 2)), RCollisionTriangle::MESH, 0, nZoneMaterialID);
			}
		}
	}
	else
	{
		CollectStaticCollisionPolygons((RRsNode*)pNode->m_pChildren[0], pCollector, VertexInfo);
		CollectStaticCollisionPolygons((RRsNode*)pNode->m_pChildren[1], pCollector, VertexInfo);
	}
}

void RStaticMeshSceneNode::CollectStaticCollisionPolygons(RSceneNodePolygonCollector* pCollector) const
{
	for(size_t i=0;i<m_cells.size();++i)
	{
		RCell* pCell = m_cells[i];
		for(RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin(); itr != pCell->m_mapGeometryList.end(); ++itr)
		{
			RGeometry* pGeometry = itr->second;
			for(int j=0;j<pGeometry->m_nTrees;++j)
			{
				RRsAABBTree* pTree = pGeometry->m_pTrees[j];
				CollectStaticCollisionPolygons((RRsNode*)pTree->m_pRootNode, pCollector, pGeometry->m_VertexInfo);
			}
		}
	}
}


void RStaticMeshSceneNode::RenderGStage( RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;
	if( !GetUsable() ) return;


	RShaderCompositeManager::m_cVisiblity.SetFloat(1.f);
	RShaderCompositeManager::SetTransformConstant(GetWorldTransform(), pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix());

	m_pCurrentRenderStrategy = m_pGStageRenderStrategy;
	Render( pCurrentCamera);

	RestoreRenderState();
}

void RStaticMeshSceneNode::RenderMStage( RCameraSceneNode* pCurrentCamera)
{
	RPFC_THISFUNC;
	if( !GetUsable() ) return;


	// Visiblity 초기화
	RShaderCompositeManager::m_cVisiblity.SetFloat(1.f);
	RShaderCompositeManager::SetTransformConstant(GetWorldTransform(), pCurrentCamera->GetViewMatrix(), pCurrentCamera->GetViewProjectionMatrix());

	m_pCurrentRenderStrategy = m_pMStageRenderStrategy;
	Render( pCurrentCamera);

	RestoreRenderState();
}

}
