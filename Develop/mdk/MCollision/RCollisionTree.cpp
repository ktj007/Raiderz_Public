#include "StdAfx.h"
#include "RCollisionTree.h"
#include "MCollisionVersion.h"

#include "MFileSystem.h"
#include "RCollision.h"
#include "MDebug.h"
#include "MXml.h"

namespace rs3 {


/////////////////////////////////////////////////////////////////
// RCollisionTree

RCollisionTree::RCollisionTree(void) : m_pTriangles(NULL), m_nTriangles(0), m_pRootNode(NULL), m_nNodeCount(0)
{
}

RCollisionTree::~RCollisionTree(void)
{
	Destroy();
}

void RCollisionTree::Destroy()
{
	SAFE_DELETE_ARRAY(m_pRootNode);
	SAFE_DELETE_ARRAY(m_pTriangles);
}

class RCollisionTreeImpl
{

public:
	RCollisionTree *m_pCollisionTree;

	virtual bool Load(MFileBuffer &file, RCollisionTreeNode *pNode) = 0;
	bool Save(FILE *fp, RCollisionTreeNode *pNode);
};

class RCollisionTreeImpl_v0 : public RCollisionTreeImpl
{
public:
	virtual bool Load(MFileBuffer &file, RCollisionTreeNode *pNode)
	{
		if(!file.Read(&pNode->m_bb, sizeof(pNode->m_bb))) 
			return false;

		bool bLeaf;
		if(!file.Read(&bLeaf,sizeof(bool))) 
			return false;

		if(!bLeaf)
		{
			pNode->m_pLeft = ++m_pCollisionTree->m_pLoadingNode;
			pNode->m_pRight = ++m_pCollisionTree->m_pLoadingNode;

			if(!Load(file, pNode->m_pLeft)) 
				return false;
			if(!Load(file, pNode->m_pRight)) 
				return false;

			return true;
		}

		pNode->m_pLeft = pNode->m_pRight = NULL;
		if(!file.Read(&pNode->m_nPolygonCount, sizeof(int))) 
			return false;

		MVector3 *pTriangles = new MVector3[pNode->m_nPolygonCount*3];
		if( !file.Read( pTriangles, sizeof(MVector3)*pNode->m_nPolygonCount*3))
		{
			delete [] pTriangles;
			return false;
		}

		pNode->m_pTriangles = m_pCollisionTree->m_pLoadingTriangle;
		for ( int i = 0; i<pNode->m_nPolygonCount; ++i )
		{
			m_pCollisionTree->m_pLoadingTriangle->v[0] = pTriangles[i*3];
			m_pCollisionTree->m_pLoadingTriangle->v[1] = pTriangles[i*3+1];
			m_pCollisionTree->m_pLoadingTriangle->v[2] = pTriangles[i*3+2];
			m_pCollisionTree->m_pLoadingTriangle->dwNodeAttribute = RCollisionTriangle::MESH;

			++m_pCollisionTree->m_pLoadingTriangle;
		}
		_ASSERT(m_pCollisionTree->m_pLoadingTriangle == (pNode->m_pTriangles + pNode->m_nPolygonCount));

		delete [] pTriangles;

		return true;
	}
};

class RCollisionTreeImpl_v1 : public RCollisionTreeImpl
{
	virtual bool Load(MFileBuffer &file, RCollisionTreeNode *pNode)
	{
		if(!file.Read(&pNode->m_bb, sizeof(pNode->m_bb))) 
			return false;

		bool bLeaf;
		if(!file.Read(&bLeaf,sizeof(bool))) 
			return false;

		if(!bLeaf)
		{
			pNode->m_pLeft = ++m_pCollisionTree->m_pLoadingNode;
			pNode->m_pRight = ++m_pCollisionTree->m_pLoadingNode;

			if(!Load(file, pNode->m_pLeft)) 
				return false;
			if(!Load(file, pNode->m_pRight)) 
				return false;

			return true;
		}

		pNode->m_pLeft = pNode->m_pRight = NULL;
		if(!file.Read(&pNode->m_nPolygonCount, sizeof(int))) 
			return false;
		
		pNode->m_pTriangles = m_pCollisionTree->m_pLoadingTriangle;
		if( !file.Read( m_pCollisionTree->m_pLoadingTriangle, sizeof(RCollisionTriangle)*pNode->m_nPolygonCount))
			return false;

		// 맨앞의 폴리곤에 대해서만 dwNodeAttribute 가 0인지 확인한다.
		_ASSERT( (m_pCollisionTree->m_pLoadingTriangle + 0 )->dwNodeAttribute != 0 );

		m_pCollisionTree->m_pLoadingTriangle += pNode->m_nPolygonCount;
		return true;
	}
};

bool RCollisionTree::Load(const char* szFileName)
{
	// .cl2 로드
	MFileBuffer file;
	if(!file.Open(szFileName)) 
		return false;

	RHEADER header;
	if(!file.Read(&header,sizeof(RHEADER))) 
		return false;

	RCollisionTreeImpl *pImpl = NULL;
	if(header.dwID!=R_COL2_ID )
		return false;
	
	//mlog("cl2 filename : %s , version : %d\n", szFileName, header.dwVersion);
	switch ( header.dwVersion )
	{
	case 0:
		pImpl = new RCollisionTreeImpl_v0();
	break;
	case 1:
		pImpl = new RCollisionTreeImpl_v1();
	break;
	default:
		return false;
	}

	pImpl->m_pCollisionTree = this;

	file.Read(&m_nTriangles,sizeof(int));
	file.Read(&m_nNodeCount,sizeof(int));

	m_pTriangles = new RCollisionTriangle[m_nTriangles];
	m_pRootNode = new RCollisionTreeNode[m_nNodeCount];
	
	m_pLoadingNode = NULL;
	m_pLoadingTriangle = NULL;

	m_pLoadingNode = m_pRootNode;
	m_pLoadingTriangle = m_pTriangles;

	if ( !pImpl->Load(file, m_pRootNode))
	{
		delete pImpl;
		return false;
	}

	_ASSERT(m_pLoadingNode - m_pRootNode +1 == m_nNodeCount);
	_ASSERT(m_pLoadingTriangle - m_pTriangles == m_nTriangles);
	
	delete pImpl;
	
	m_strCurrentFile.clear();
	m_strCurrentFile = szFileName;

	file.Close();
	
	// .cl2.xml 로드
	m_vecMaterialTable.clear();

	string strTableFileName = szFileName;
	strTableFileName += ".xml";

	MXml kXml;
	if ( !kXml.LoadFile(strTableFileName.c_str()))
	{
//		mlog(".cl2.xml 을 찾을 수 없습니다. (material-name을 얻어올 수 없습니다.)\n");
		return true;
	}

	MXmlElement *pRootElement = kXml.Doc()->FirstChildElement("TABLE");
	
	int nTableSize = 0;
	pRootElement->Attribute("size",&nTableSize);
	if ( nTableSize == 0 )
	{
		mlog(".cl2.xml 의 Material 정보가 없습니다\n");
		return true;
	}


	m_vecMaterialTable.resize(nTableSize);
	for ( MXmlElement *pChildElement = pRootElement->FirstChildElement("MATERIAL");
		pChildElement != NULL; pChildElement = pChildElement->NextSiblingElement("MATERIAL"))
	{
		const char *pcName = pChildElement->Attribute("name");
		int nID = -1;
		pChildElement->Attribute("id",&nID);
		_ASSERT(nID>=0);
		_ASSERT(nID<nTableSize);

		m_vecMaterialTable[nID] = (pcName);
	}

	return true;
}

void RCollisionTree::PickTriangle(RCollisionTreeNode *pNode, RCollisionPickInfo &pickInfo, DWORD dwAttributeMask
								  , bool (*IntersectFunc)(const MVector3&, const MVector3&, MVector3&, MVector3&, MVector3&, MVector3*) )
{
	// bb와 교차하면 섹터안의 폴리곤과 교차검사한다
	if(pNode->m_bb.ContainsLineSegment(pickInfo.m_inPickOrigin,pickInfo.m_inPickTo)) 
	{
		if(pNode->IsLeafNode()) 
		{
			RCollisionTriangle *pTriangle = pNode->m_pTriangles;
			for(int i = 0;i<pNode->m_nPolygonCount;i++,pTriangle++) 
			{
				MVector3 pickPos;
				if( !(dwAttributeMask & pTriangle->dwNodeAttribute))
					continue;

				if ((*IntersectFunc)(pickInfo.m_inPickOrigin,pickInfo.m_tempPickDir,
					pTriangle->v[0],pTriangle->v[1],pTriangle->v[2],&pickPos))
				{
					MVector3 vLine = pickPos - pickInfo.m_inPickOrigin;
					float LengthSq = vLine.LengthSq();
					if(LengthSq <= pickInfo.m_tempToDistance && pickInfo.m_outDistanceSq>=LengthSq
						&& vLine.DotProduct(pickInfo.m_tempPickDir) >= 0.0f 
						)
					{
						// face-normal을 구하여 대입한다.
						MVector3 V1 = pTriangle->v[1] - pTriangle->v[0];
						MVector3 V2 = pTriangle->v[2] - pTriangle->v[1];
						MVector3 N = V1.CrossProduct(V2);

						pickInfo.m_outPosition = pickPos;
						pickInfo.m_outDistanceSq = LengthSq;
						pickInfo.m_outNormal = N.Normalize();
						pickInfo.m_outNodeAttr = static_cast<WORD>(pTriangle->dwNodeAttribute & 0x0000ffff);
						pickInfo.m_outUserAttr = static_cast<WORD>(pTriangle->dwNodeAttribute >> 16);
						pickInfo.m_outMaterial = pTriangle->dwUserMaterial;

						if ( !m_vecMaterialTable.empty())
							pickInfo.m_outMaterialName = GetMaterialName(pTriangle->dwUserMaterial).c_str();
					}
				}
			}
			return;
		}
		else
		{
			PickTriangle(pNode->m_pLeft, pickInfo, dwAttributeMask, IntersectFunc);
			PickTriangle(pNode->m_pRight, pickInfo, dwAttributeMask, IntersectFunc);
		}
	}
}

bool RCollisionTree::Pick(RCollisionPickInfo& pickInfo, DWORD dwAttributeMask, bool bNoCull)
{
	pickInfo.m_outDistanceSq = FLT_MAX;
	pickInfo.m_outNodeAttr = RCollisionTriangle::NONE;
	pickInfo.m_outUserAttr = 0;

	pickInfo.m_outMaterial = -1;
	pickInfo.m_outMaterialName = NULL;

	pickInfo.m_tempPickDir = (pickInfo.m_inPickTo - pickInfo.m_inPickOrigin);
	pickInfo.m_tempToDistance = pickInfo.m_tempPickDir.LengthSq();
	pickInfo.m_tempPickDir /= pickInfo.m_tempToDistance;

	if(!m_pRootNode) 
		return false;	// 정보가없다.
	
	// Intersect 함수의 function-pointer를 넘긴다.
	if ( bNoCull )
		PickTriangle(m_pRootNode, pickInfo, dwAttributeMask, MMath::IsIntersectNoCull);
	else
		PickTriangle(m_pRootNode, pickInfo, dwAttributeMask, MMath::IsIntersect);

	//_ASSERT((pickInfo.m_outPosition - pickInfo.m_inPickOrigin).DotProduct(pickInfo.m_tempPickDir) >= 0.0f );
	if ( pickInfo.m_outDistanceSq == FLT_MAX )
		return false;

	return true;
}

void RCollisionTree::GetCollidablePolygons(RCollision &collision, DWORD dwAttributeMask)
{
	if (m_pRootNode == NULL)
		return;

	//_ASSERT(m_pRootNode);	// 로드가 되지 않았을 확률이 높다
	GetCollidablePolygons(m_pRootNode, collision, dwAttributeMask);
}

void RCollisionTree::GetCollidablePolygons(RCollisionTreeNode *pNode,RCollision &collision, DWORD dwAttributeMask)
{
	if(collision.IsCollidable(pNode->m_bb))	// 잠재적으로 충돌가능성이 있는 노드만 체크
	{
		if(pNode->IsLeafNode()) {
			for(int i = 0;i<pNode->m_nPolygonCount;i++) 
			{
				if ( pNode->m_pTriangles[i].dwNodeAttribute & dwAttributeMask )
				{
					collision.CheckPolygon( pNode->m_pTriangles[i].v[0],
						pNode->m_pTriangles[i].v[1],
						pNode->m_pTriangles[i].v[2]);
				}
			}
		}
		else 
		{
			GetCollidablePolygons(pNode->m_pLeft  ,collision, dwAttributeMask);
			GetCollidablePolygons(pNode->m_pRight ,collision, dwAttributeMask);
		}
	}
}

const MBox& RCollisionTree::GetRootAABB()
{
	_ASSERT(m_pRootNode);
	return m_pRootNode->m_bb;
}

void RCollisionTree::CallbackTree(ICallback *piCallback)
{
	CallBackTreeNode(m_pRootNode, piCallback);
}

void RCollisionTree::CallBackTreeNode(rs3::RCollisionTreeNode *pNode, ICallback *piCallback)
{
	if ( pNode->IsLeafNode())
	{
		piCallback->OnNode(pNode);
		for ( int i = 0; i<pNode->m_nPolygonCount; ++i )
			piCallback->OnTriangle(&pNode->m_pTriangles[i]);
	}
	else
	{
		CallBackTreeNode(pNode->m_pLeft, piCallback);
		CallBackTreeNode(pNode->m_pRight, piCallback);
	}
}

}
