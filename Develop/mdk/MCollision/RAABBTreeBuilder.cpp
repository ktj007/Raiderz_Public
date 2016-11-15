#include "StdAfx.h"
#include "RAABBTreeBuilder.h"
#include "MDebug.h"
#include <algorithm>

using namespace std;

namespace rs3 {



bool AABBTreeBuilder_PolygonGraterX(RAABBTreePolygon* a,RAABBTreePolygon* b)
{
	return (a->m_center.x>b->m_center.x);
}

bool AABBTreeBuilder_PolygonGraterY(RAABBTreePolygon* a,RAABBTreePolygon* b)
{
	return (a->m_center.y>b->m_center.y);
}

bool AABBTreeBuilder_PolygonGraterZ(RAABBTreePolygon* a,RAABBTreePolygon* b)
{
	return (a->m_center.z>b->m_center.z);
}

///////////////////////////////////////////////////////
// RAABBTreeNode

int RAABBTreeNode::GetNodeCount()
{
	if(IsLeafNode()) return 1;
	return 1 + m_pChildren[0]->GetNodeCount() + m_pChildren[1]->GetNodeCount();
}

int RAABBTreeNode::GetPolygonCount()
{
	if(IsLeafNode()) return static_cast<int>(m_polygons.size());
	return m_pChildren[0]->GetPolygonCount() + m_pChildren[1]->GetPolygonCount();
}

int RAABBTreeNode::GetVertexCount()
{
	if(IsLeafNode()) 
	{
		int nCount = 0;
		for(size_t i=0;i< m_polygons.size(); i++)
			nCount+= m_polygons[i]->GetVertexCount();
		return nCount;
	}
	return m_pChildren[0]->GetVertexCount() + m_pChildren[1]->GetVertexCount();
}


///////////////////////////////////////////////////////
// RAABBTreeBuilder
RAABBTreeBuilder::RAABBTreeBuilder()
: m_bBuilt(false)
{
}

RAABBTreeBuilder::~RAABBTreeBuilder()
{
}

void RAABBTreeBuilder::Build(RAABBTreeNode* pNode)
{
	// 바운딩 박스를 만든다.
	pNode->m_bb.Initialize();
	for(size_t i=0;i<pNode->m_polygons.size();i++)
	{
		RAABBTreePolygon* pPolygon = pNode->m_polygons[i];
		for(int j=0;j<pPolygon->GetVertexCount();j++)
			pNode->m_bb.Add(pPolygon->GetVertexPosition(j));
	}

	// 일정개수 이하면 더이상 나누지 않고 leaf node 로 만든다.
	if(pNode->m_polygons.size()<m_nPolygonPerNodes)
	{
		pNode->m_pChildren[0] = pNode->m_pChildren[1] = NULL;
		return;
	}

	MVector3 diff = pNode->m_bb.vmax - pNode->m_bb.vmin;
	int nSortAxis =		diff.x > diff.y					 // 바운딩박스중 가장 큰 축을 찾는다..
		? diff.z > diff.x ? 2 : 0 
		: diff.z > diff.y ? 2 : 1;

	// 가장 긴 축으로 정렬한다.
	switch(nSortAxis)
	{
	case 0 : sort(pNode->m_polygons.begin(),pNode->m_polygons.end(),AABBTreeBuilder_PolygonGraterX);break;
	case 1 : sort(pNode->m_polygons.begin(),pNode->m_polygons.end(),AABBTreeBuilder_PolygonGraterY);break;
	case 2 : sort(pNode->m_polygons.begin(),pNode->m_polygons.end(),AABBTreeBuilder_PolygonGraterZ);break;
	}

	pNode->m_pChildren[0] = NewNode();
	pNode->m_pChildren[1] = NewNode();

	for(size_t i=0;i<pNode->m_polygons.size();i++)
	{
		if(i<pNode->m_polygons.size()/2) {
			pNode->m_pChildren[0]->m_polygons.push_back(pNode->m_polygons[i]);
		}else{
			pNode->m_pChildren[1]->m_polygons.push_back(pNode->m_polygons[i]);
		}
	}
	pNode->m_polygons.clear();

	Build(pNode->m_pChildren[0]);  
	Build(pNode->m_pChildren[1]);
}

void RAABBTreeBuilder::BeginBuild()
{
	m_pRootNode = NewNode();
	m_pRootNode->m_pChildren[0] = m_pRootNode->m_pChildren[1] = NULL;
}

void RAABBTreeBuilder::AddPolygon(RAABBTreePolygon* pPolygon)
{
	pPolygon->m_center = MVector3(0,0,0);
	for(int i=0;i<pPolygon->GetVertexCount();i++)
		pPolygon->m_center+=pPolygon->GetVertexPosition(i);

	pPolygon->m_center *= 1.f/(float)pPolygon->GetVertexCount();

	m_pRootNode->m_polygons.push_back(pPolygon);
}

void RAABBTreeBuilder::EndBuild(int nPolygonPerNodes)
{ 
	if(m_bBuilt)
	{
//		_ASSERT( !m_bBuilt );
		mlog("already built : %d polygons\n",m_pRootNode->GetPolygonCount() );
		return ;
	}

	m_nPolygonPerNodes = nPolygonPerNodes;
	mlog("collision tree build : %d polygons\n",m_pRootNode->m_polygons.size());

	m_bBuilt = true;
	Build(m_pRootNode);
}

}
