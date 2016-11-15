#include "StdAfx.h"
#include "RCollisionTree.h"
#include "RCollisionTreeBuilder.h"

#include <algorithm>
#include <string>
#include "MCollisionVersion.h"

#include "MDebug.h"
#include "MXml.h"

using namespace std;

namespace rs3 {

///////////////////////////////////////////////////////
// RCollisionTreeBuilderNode

bool PolygonGraterX(RCollisionTriangle* a,RCollisionTriangle *b)
{
	float fCenterA = a->v[0].x + a->v[1].x + a->v[2].x;
	float fCenterB = b->v[0].x + b->v[1].x + b->v[2].x;
	return (fCenterA>fCenterB);
}

bool PolygonGraterY(RCollisionTriangle* a,RCollisionTriangle *b)
{
	float fCenterA = a->v[0].y + a->v[1].y + a->v[2].y;
	float fCenterB = b->v[0].y + b->v[1].y + b->v[2].y;
	return (fCenterA>fCenterB);
}

bool PolygonGraterZ(RCollisionTriangle* a,RCollisionTriangle *b)
{
	float fCenterA = a->v[0].z + a->v[1].z + a->v[2].z;
	float fCenterB = b->v[0].z + b->v[1].z + b->v[2].z;
	return (fCenterA>fCenterB);
}

int RCollisionTreeBuilderNode::m_nMaxPolygonPerNode;

RCollisionTreeBuilderNode::RCollisionTreeBuilderNode()
{
}

RCollisionTreeBuilderNode::RCollisionTreeBuilderNode(RCollisionTreeBuilder* pBuilder)
{
	m_pBuilder = pBuilder;
}

RCollisionTreeBuilderNode::~RCollisionTreeBuilderNode()
{
	SAFE_DELETE(m_pLeft);
	SAFE_DELETE(m_pRight);
	for(size_t i=0;i<m_polygons.size();i++)
		delete m_polygons[i];
	m_polygons.clear();
}

void RCollisionTreeBuilderNode::Build()
{
	// 바운딩 박스를 만든다.
	m_bb.Initialize();
	for(size_t i=0;i<m_polygons.size();i++)
	{
//		_ASSERT(i!=43180);
		for(int j=0;j<3;j++)
			m_bb.Add(m_polygons[i]->v[j]);
//		_ASSERT(m_bb.vmin.y>1000);
	}

	// 일정개수 이하면 더이상 나누지 않고 leaf node 로 만든다.
	if((int)m_polygons.size()<m_nMaxPolygonPerNode)
	{
		m_pLeft = m_pRight = NULL;
		m_pBuilder->m_nCurrentPoly += (int)m_polygons.size();
		m_pBuilder->OnUpdateProgress(m_pBuilder->m_nTotalPoly, m_pBuilder->m_nCurrentPoly);
		return;
	}

	MVector3 diff = m_bb.vmax - m_bb.vmin;
	int nSortAxis =		diff.x > diff.y					 // 바운딩박스중 가장 큰 축을 찾는다..
		? diff.z > diff.x ? 2 : 0 
		: diff.z > diff.y ? 2 : 1;

	// 가장 긴 축으로 정렬한다.
	switch(nSortAxis)
	{
	case 0 : sort(m_polygons.begin(),m_polygons.end(),PolygonGraterX);break;
	case 1 : sort(m_polygons.begin(),m_polygons.end(),PolygonGraterY);break;
	case 2 : sort(m_polygons.begin(),m_polygons.end(),PolygonGraterZ);break;
	}

	m_pLeft = new RCollisionTreeBuilderNode(m_pBuilder);
	m_pRight = new RCollisionTreeBuilderNode(m_pBuilder);

	for(size_t i=0;i<m_polygons.size();i++)
	{
		if(i<m_polygons.size()/2) {
			m_pLeft->m_polygons.push_back(m_polygons[i]);
		}else{
			m_pRight->m_polygons.push_back(m_polygons[i]);
		}
	}
	m_polygons.clear();

	m_pLeft->Build();  
	m_pRight->Build();
}

int RCollisionTreeBuilderNode::GetNodeCount()
{
	if(IsLeafNode()) return 1;
	return 1 + m_pLeft->GetNodeCount() + m_pRight->GetNodeCount();
}

int RCollisionTreeBuilderNode::GetPolygonCount()
{
	if(IsLeafNode()) return static_cast<int>(m_polygons.size());
	return m_pLeft->GetPolygonCount() + m_pRight->GetPolygonCount();
}


///////////////////////////////////////////////////////
// RCollisionTreeBuilder

void RCollisionTreeBuilder::OnUpdateProgress(int nTotalPoly, int nCurrentPoly)
{

}

DWORD RCollisionTreeBuilder::GetZoneMaterialID(const std::string &strMaterialName)
{
	size_t si = 0;
	for ( si = 0; si<m_vecMaterialList.size(); ++si )
	{
		if ( m_vecMaterialList[si] == strMaterialName )
			return (DWORD)si;
	}

	// 찾지 못했을 때 ...
	m_vecMaterialList.push_back(strMaterialName);
	return (DWORD)si;
}

RCollisionTreeBuilder::RCollisionTreeBuilder(void)
{
	m_nTotalPoly = 0;
	m_nCurrentPoly = 0;

	m_rootNode = new RCollisionTreeBuilderNode(this);
	m_rootNode->m_pLeft = NULL;
	m_rootNode->m_pRight = NULL;
}

RCollisionTreeBuilder::~RCollisionTreeBuilder(void)
{
	SAFE_DELETE(m_rootNode->m_pLeft);
	SAFE_DELETE(m_rootNode->m_pRight);
	SAFE_DELETE(m_rootNode);
}

void RCollisionTreeBuilder::AddPolygon(const MVector3& v0, const MVector3& v1, const MVector3& v2, WORD wNodeFlag, WORD wUserFlag, DWORD dwMaterial)
{
	RCollisionTriangle *pTriangle = new RCollisionTriangle();
	
	pTriangle->v[0] = v0;
	pTriangle->v[1] = v1;
	pTriangle->v[2] = v2;

	_ASSERT(pTriangle->dwNodeAttribute==0);
	pTriangle->dwNodeAttribute = ( wUserFlag << 16) | wNodeFlag;
	pTriangle->dwUserMaterial = dwMaterial;

	m_rootNode->m_polygons.push_back(pTriangle);
}

void RCollisionTreeBuilder::CopyToCollisionTree(RCollisionTree *pCollisionTree, RCollisionTreeNode *pCNode, RCollisionTreeBuilderNode *pBNode)
{
	pCNode->m_bb = pBNode->m_bb;
	if (!pBNode->IsLeafNode())
	{
		pCNode->m_pLeft = ++pCollisionTree->m_pLoadingNode;
		pCNode->m_pRight = ++pCollisionTree->m_pLoadingNode;

		CopyToCollisionTree(pCollisionTree,pCNode->m_pLeft,pBNode->m_pLeft);
		CopyToCollisionTree(pCollisionTree,pCNode->m_pRight, pBNode->m_pRight);
		
		return;
	}

	pCNode->m_pLeft = NULL;
	pCNode->m_pRight = NULL;
	pCNode->m_nPolygonCount = pBNode->GetPolygonCount();
	pCNode->m_pTriangles = pCollisionTree->m_pLoadingTriangle;
	
	for ( int i = 0; i<pBNode->GetPolygonCount(); ++i )
	{
		pCNode->m_pTriangles[i].v[0] = pBNode->m_polygons[i]->v[0];
		pCNode->m_pTriangles[i].v[1] = pBNode->m_polygons[i]->v[1];
		pCNode->m_pTriangles[i].v[2] = pBNode->m_polygons[i]->v[2];
		pCNode->m_pTriangles[i].dwNodeAttribute = pBNode->m_polygons[i]->dwNodeAttribute;
		pCNode->m_pTriangles[i].dwUserMaterial = pBNode->m_polygons[i]->dwUserMaterial;
	}

	pCollisionTree->m_pLoadingTriangle += pCNode->m_nPolygonCount;
	
	return;
}

RCollisionTree* RCollisionTreeBuilder::CreateCollisionTree()
{
	RCollisionTree *pCollisionTree = new RCollisionTree();

	int nTriangle = m_rootNode->GetPolygonCount();
	int nNodeCount = m_rootNode->GetNodeCount();

	pCollisionTree->m_nTriangles = nTriangle;
	pCollisionTree->m_nNodeCount = nNodeCount;
	pCollisionTree->m_pTriangles = new RCollisionTriangle[nTriangle];
	pCollisionTree->m_pRootNode = new RCollisionTreeNode[nNodeCount];

	pCollisionTree->m_pLoadingTriangle = pCollisionTree->m_pTriangles;
	pCollisionTree->m_pLoadingNode = pCollisionTree->m_pRootNode;
	
	CopyToCollisionTree(pCollisionTree, pCollisionTree->m_pRootNode,m_rootNode);
	_ASSERT(pCollisionTree->m_pLoadingNode - pCollisionTree->m_pRootNode + 1 == pCollisionTree->m_nNodeCount);
	_ASSERT(pCollisionTree->m_pLoadingTriangle - pCollisionTree->m_pTriangles == pCollisionTree->m_nTriangles);

	return pCollisionTree;
}

bool RCollisionTreeBuilder::Save(const char* szFileName)
{
	FILE *file = NULL;
	if ( fopen_s(&file, szFileName,"wb+") != 0 )
		return false;

	RHEADER header(R_COL2_ID, R_COL2_VERSION);
	fwrite(&header, sizeof(RHEADER), 1, file);

	int nPolygonCount = m_rootNode->GetPolygonCount();
	fwrite(&nPolygonCount,sizeof(int), 1, file);

	mlog("collision save %d polygons\n",nPolygonCount);

	int nNodeCount = m_rootNode->GetNodeCount();
	fwrite(&nNodeCount,sizeof(int), 1, file);

	bool bOk = Save(file,m_rootNode);
	fclose(file);

	string strXmlFilename = szFileName;
	strXmlFilename += ".xml";

	bool bXMLTableSaveResult = SaveXml(strXmlFilename.c_str());
	_ASSERT(bXMLTableSaveResult);

	return bOk;
}

bool RCollisionTreeBuilder::SaveXml(const char *szFilename)
{
	int nMaterialNum = (int)m_vecMaterialList.size();

	// save material list
	MXml kXml;

	MXmlDeclaration* pDecl = kXml.CreateDeclaration("1.0", "UTF-8", "");
	kXml.Doc()->LinkEndChild(pDecl);

	MXmlElement *pRootElement = new MXmlElement("TABLE");
	kXml.Doc()->LinkEndChild(pRootElement);

	pRootElement->SetAttribute("size", nMaterialNum);
	for ( int i = 0; i<nMaterialNum; ++i )
	{
		MXmlElement *pChildMaterial = new MXmlElement("MATERIAL");
		pChildMaterial->SetAttribute("name", m_vecMaterialList[i]);
		pChildMaterial->SetAttribute("id", i);
		pRootElement->LinkEndChild(pChildMaterial);
	}

	return kXml.SaveFile(szFilename);
}

bool RCollisionTreeBuilder::Save(FILE *file, RCollisionTreeBuilderNode* pNode)
{
	fwrite(&pNode->m_bb,sizeof(pNode->m_bb),1,file);
	
	bool bLeaf = pNode->IsLeafNode();
	fwrite(&bLeaf,sizeof(bool), 1, file);

	if(!bLeaf)
	{
		if(!Save(file,pNode->m_pLeft)) return false;
		if(!Save(file,pNode->m_pRight)) return false;
		return true;
	}
    
	int nPolygon = pNode->GetPolygonCount();
	fwrite(&nPolygon, sizeof(int), 1, file);
	for(int i=0;i<nPolygon;i++)
	{
		fwrite(pNode->m_polygons[i],sizeof(RCollisionTriangle), 1, file);
	}

	return true;
}

void RCollisionTreeBuilder::BuildTree(int nMaxPolygonPerNode)
{ 
	RCollisionTreeBuilderNode::m_nMaxPolygonPerNode = nMaxPolygonPerNode;

	mlog("collision tree build : %d polygons\n",m_rootNode->m_polygons.size());

	m_nTotalPoly = (int)m_rootNode->m_polygons.size();
	m_rootNode->Build(); 
}


const MBox& RCollisionTreeBuilder::GetRootAABB()
{
	return m_rootNode->m_bb;
}

}
