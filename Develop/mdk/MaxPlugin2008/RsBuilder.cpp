#include "StdAfx.h"
#include "RsBuilder.h"

RRsNode::~RRsNode()
{
	SAFE_DELETE(m_pChildren[0]);
	SAFE_DELETE(m_pChildren[1]);
}

int RRsNode::GetIndicesCount()
{
	if(IsLeafNode()) {
		int nCount=0;
		for(size_t i=0;i<m_polygons.size();i++)
		{
			nCount+=(m_polygons[i]->GetVertexCount()-2)*3;
		}
		return nCount;
	}
	return ((RRsNode*)m_pChildren[0])->GetIndicesCount() + 
			((RRsNode*)m_pChildren[1])->GetIndicesCount();
}

bool RRsNode::Save(FILE *file)
{
	fwrite(&m_bb,sizeof(m_bb),1,file);

	bool bLeaf = IsLeafNode();
	fwrite(&bLeaf,sizeof(bool),1,file);

	if(!bLeaf) {
		((RRsNode*)m_pChildren[0])->Save(file);
		((RRsNode*)m_pChildren[1])->Save(file);
	}

	int nFace = m_polygons.size();
	fwrite(&nFace,sizeof(int),1,file);
	for(vector<RAABBTreePolygon*>::iterator i=m_polygons.begin();i!=m_polygons.end();i++)
	{
		rpolygon *f = (rpolygon*)*i;

		_ASSERT(f->pMaterial!=NULL);
		int nMaterial = f->pMaterial->GetIndex();
		_ASSERT(nMaterial>=0);
		fwrite(&nMaterial,sizeof(int),1,file);
		fwrite(&f->dwFlags,sizeof(DWORD),1,file);
		fwrite(&f->nCount,sizeof(int),1,file);
		fwrite(&f->vi,sizeof(int),1,file);	// base index ÀúÀå
	}
	return true;
}


RsBuilder::RsBuilder(void)
{
}

RsBuilder::~RsBuilder(void)
{
	SAFE_DELETE(m_pRootNode);
}

void RsBuilder::BuildVertex(RRsNode* pNode)
{
	int nFace = pNode->m_polygons.size();
	for(vector<RAABBTreePolygon*>::iterator i=pNode->m_polygons.begin();i!=pNode->m_polygons.end();i++)
	{
		rpolygon *f = (rpolygon*)*i;
		f->vi = m_vertices.size();
		for(int j = 0 ; j < f->nCount; j++)
		{
			rvertex *pv = &f->v[j];
			m_vertices.push_back(pv);
		}
	}

	if(pNode->m_pChildren[0]) 
		BuildVertex((RRsNode*)pNode->m_pChildren[0]);
	if(pNode->m_pChildren[1]) 
		BuildVertex((RRsNode*)pNode->m_pChildren[1]);
}

void RsBuilder::BuildVertex()
{
	BuildVertex((RRsNode*)m_pRootNode);
}

bool RsBuilder::Save(FILE *file)
{
	fwrite(&m_nMaterial,sizeof(int),1,file);
	fwrite(&m_nLightMapID,sizeof(int),1,file);

	int nVertices = m_pRootNode->GetVertexCount();
	fwrite(&nVertices,sizeof(int),1,file);
	((RRsNode*)m_pRootNode)->Save(file);
	return true;
}