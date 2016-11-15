#include "stdafx.h"
#include "M_ToolStaticMeshSceneNode.h"
#include "RRenderHelper.h"

namespace rs3 {

RToolStaticMeshSceneNode::RToolStaticMeshSceneNode() :
	RStaticMeshSceneNode(), m_bRenderDebug(false), m_bRenderAll(false)
{
}

RToolStaticMeshSceneNode::~RToolStaticMeshSceneNode()
{
}

void RToolStaticMeshSceneNode::OnRender( RCameraSceneNode* pCurrentCamera, int nSubMaterialID, RRENDERINGMETHOD renderMethod)
{
	if( !GetUsable() ) return;

	RStaticMeshSceneNode::OnRender( pCurrentCamera, nSubMaterialID, renderMethod);

	if( m_bRenderDebug )
		RenderDebug();
}

void RToolStaticMeshSceneNode::Cull(RCameraSceneNode* pCamera,const vector<RCULLDESC>& cullDescs, const RPlane* pPlaneNear, const RPlane* pPlaneFar)
{
	if( !m_bRenderAll )
		RStaticMeshSceneNode::Cull(pCamera, cullDescs, pPlaneNear, pPlaneFar );
	else
		CullNone();
}

void RToolStaticMeshSceneNode::CullNone()
{
	ClearRenderNodes();

	m_nRenderedNodes = 0;
	m_nRenderedCells = 0;

	m_unRenderCounter++;

	m_pCullingNearPlane = NULL;
	m_pCullingFarPlane = NULL;

	m_pCullingPlanes = NULL;
	m_nCullingPlanes = 0;

	m_pUserCullingPlanes = NULL;
	m_nUserCullingPlanes = 0;

	for( int i = 0; i < GetCellCount(); ++i )
	{
		m_nRenderedCells++;

		RCell* pCell = GetCell(i);
		for(RCell::IDGEOMETRYMAP::iterator itr = pCell->m_mapGeometryList.begin(); itr != pCell->m_mapGeometryList.end(); itr++)
		{
			RGeometry* pGeometry = itr->second;
			CullGeometry( pGeometry );
		}
	}
}

void RToolStaticMeshSceneNode::RenderDebug()
{
	RRenderHelper::SetRenderState();
	REngine::GetDevice().SetBlending( RBLEND_SRCALPHA, RBLEND_INVSRCALPHA );

	// cell
	for( int i = 0; i < GetCellCount(); ++i )
	{
		RCell* pCell = GetCell(i);
		for( int j = 0; j < (int)pCell->m_cellFaces.size(); ++j )
		{
			RCellFace* pCellFace = pCell->m_cellFaces[j];

			for( int k = 0; k < pCellFace->nVerts; ++k )
				RRenderHelper::m_batchLineDrawer.Add( pCellFace->pVerts[k], pCellFace->pVerts[(k+1)%pCellFace->nVerts] );

			RRenderHelper::RenderTriangles( RMatrix::IDENTITY, pCellFace->pVerts, pCellFace->nVerts-2, 0x30FF00FF, RPT_TRIANGLEFAN );
		}
	}
	RRenderHelper::m_batchLineDrawer.Render( RMatrix::IDENTITY, 0xFFFFFF00 );

	// portal
	for( int i = 0; i < GetCellCount(); ++i )
	{
		RCell* pCell = GetCell(i);
		for( RPortalList::iterator it = pCell->m_portals.begin(); it != pCell->m_portals.end(); ++it )
		{
			RPortal* pPortal = it->first;

			for( int j = 0; j < pPortal->m_nVertices; ++j )
				RRenderHelper::m_batchLineDrawer.Add( pPortal->m_pVertices[j], pPortal->m_pVertices[(j+1)%pPortal->m_nVertices] );
		}
	}
	REngine::GetDevice().SetDepthEnable( false );
	RRenderHelper::m_batchLineDrawer.Render( RMatrix::IDENTITY, 0x3000FFFF );
	REngine::GetDevice().SetDepthEnable( true );

	RRenderHelper::EndRenderState();
}

void RToolStaticMeshSceneNode::RenderRawAll(  RCameraSceneNode* pCurrentCamera)
{
	m_pCurrentRenderStrategy = NULL;
	CullNone();
	Render( pCurrentCamera);
}

}