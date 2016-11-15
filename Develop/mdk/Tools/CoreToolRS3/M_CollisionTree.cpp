#include "stdafx.h"
#include "M_CollisionTree.h"

#include "RDeviceD3D.h"
#include "RSceneNode.h"
#include "RCollisionTree.h"
#include "RCollisionTreeBuilder.h"
#include "RHeightField.h"

#include "RRenderHelper.h"
#include "RSceneNodePolygonCollector_Collision.h"

using namespace rs3;

bool SceneNodeCollisionCollector::OnPreOrderVisit( const RSceneNode* pSceneNode )
{
	if( pSceneNode->QueryAttribute( rs3::RSNA_DO_NOT_SAVE_COLLISION ) )
		return false;

	RSceneNodePolygonCollector_Collision collector(m_pBuilder);

	pSceneNode->CollectStaticCollisionPolygons( &collector );
	return true;
}

CCollisionTree::CCollisionTree()
{
	m_pCollisionTree	= NULL;
	m_bRender			= false;
}

CCollisionTree::~CCollisionTree()
{
	Destroy();
}

void CCollisionTree::Destroy()
{
	SAFE_DELETE(m_pCollisionTree);
}

bool CCollisionTree::Load(const char *pcCl2Filename)
{
	Destroy();

	m_pCollisionTree = new RCollisionTree();
	if ( m_pCollisionTree->Load(pcCl2Filename) == false )
	{
		Destroy();
		return false;
	}

	return true;
}

void CCollisionTree::Render(RDevice *pDevice)
{
	if (m_bRender == false)
		return;
	if (m_pCollisionTree == NULL)
		return;

	class CCallBackRender : public RCollisionTree::ICallback
	{
	private:
		RDevice *m_pDevice;
		vector<RVector3> m_vecPolygons;
		//vector<RBoundingBox> m_vecAABBs;

	public:
		CCallBackRender(RDevice *pDevice, int nReseverSize = 1000)
		{
			m_pDevice = pDevice;
			m_vecPolygons.reserve(nReseverSize);
			//m_vecAABBs.reserve(nReseverSize);
		}
		~CCallBackRender() {}

		void OnNode(const RCollisionTreeNode *pNode)
		{
			//m_vecAABBs.push_back(pNode->GetBoundBox());
		}
		void OnTriangle(const RCollisionTriangle *pTriangle)
		{
			m_vecPolygons.push_back(pTriangle->v[0]);
			m_vecPolygons.push_back(pTriangle->v[1]);
			m_vecPolygons.push_back(pTriangle->v[2]);
		}
		void Draw()
		{
			m_pDevice->SetFvF(D3DFVF_XYZ);
			m_pDevice->SetTransform(RST_WORLD, RMatrix::IDENTITY);

			size_t offset = 0;
			size_t size = m_vecPolygons.size();

			RRenderHelper::SetRenderState();
			m_pDevice->SetDepthBias(0, -10.f);
			m_pDevice->SetBlending(RBLEND_SRCALPHA, RBLEND_INVSRCALPHA);
			m_pDevice->SetDepthEnable(true, false);
			while ( offset < size )
			{
				const int DRAW_TRIANGLE_NUM = 3000;
				int nDrawTriangleCount = min(DRAW_TRIANGLE_NUM, int(size - offset));

				m_pDevice->SetFillMode(RFILL_SOLID);
				m_pDevice->SetTextureFactor(D3DCOLOR_ARGB(128, 84, 144, 225));
				m_pDevice->DrawPrimitiveUP(RPT_TRIANGLELIST, nDrawTriangleCount/3,&m_vecPolygons[offset], sizeof(RVector3));
				m_pDevice->SetFillMode(RFILL_WIRE);
				m_pDevice->SetTextureFactor(D3DCOLOR_XRGB(50, 50, 50));
				m_pDevice->DrawPrimitiveUP(RPT_TRIANGLELIST, nDrawTriangleCount/3,&m_vecPolygons[offset], sizeof(RVector3));

				offset += nDrawTriangleCount;
			}

			//for ( size_t si = 0; si<m_vecAABBs.size(); ++si )
			//	RRenderHelper::RenderBox(RMatrix::IDENTITY, m_vecAABBs[si], D3DCOLOR_XRGB(128, 0, 128));

			RRenderHelper::EndRenderState();
			m_pDevice->SetDepthBias(0, 0);
			m_pDevice->SetFillMode(RFILL_SOLID);
		}
	};

	CCallBackRender query(pDevice, m_pCollisionTree->GetTriagleCount());
	
	m_pCollisionTree->CallbackTree(&query);
	query.Draw();
}