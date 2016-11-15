#include "stdafx.h"
#include "MPathAgentShape.h"


iShape* MPathAgentShape::CreateSquareShape(iPathEngine *pEngine, int nWidth, int nHeight)
{
	vector<SIntVector2> v;
	v.reserve(8);

	v.push_back(SIntVector2(nWidth/2, nHeight/2));
	v.push_back(SIntVector2(nWidth/2,-nHeight/2));
	v.push_back(SIntVector2(-nWidth/2,-nHeight/2));
	v.push_back(SIntVector2(-nWidth/2, nHeight/2));

	iShape *shape = pEngine->newShape((tSigned32)v.size(),(const tSigned32*)&(v[0]));
	return shape;
}

void MPathAgentShape::CreateSquareShape(iPathEngine* pEngine, int nSize)
{
	m_pShape = CreateSquareShape(pEngine, nSize, nSize);
	_ASSERT(m_pShape != NULL);
	m_nSize = nSize/2;
}

void MPathAgentShapeSet::AddShape(const MPathAgentShape& shape, const string& strName)
{
	_ASSERT(m_vShapes.size() == m_vName.size());
	
	m_vShapes.push_back(shape);
	m_vName.push_back(strName);
}

void MPathAgentShapeSet::CreateDefaultShapes(iPathEngine* pEngine)
{
	MPathAgentShape S, M, L;

	S.CreateSquareShape(pEngine, 100);
//	M.CreateSquareShape(pEngine, 160);
//	L.CreateSquareShape(pEngine, 500);
	AddShape(S, "small");
//	AddShape(M, "medium");
//	AddShape(L, "large");
}

MPathAgentShape* MPathAgentShapeSet::GetShape(const string& strName)
{
	for (size_t i=0; i<m_vShapes.size(); i++)
	{
		if (m_vName[i] == strName)
			return &(m_vShapes[i]);
	}
	return NULL;
}

void MPathAgentShapeSet::Clear()
{
	for (size_t i=0; i<m_vShapes.size(); i++)
	{
		delete m_vShapes[i].m_pShape;
	}	

	m_vShapes.clear();
	m_vName.clear();
}

MPathAgentShape* MPathAgentShapeSet::GetDefaultShape()
{
	if (m_vShapes.empty()) return NULL;

	return &(m_vShapes[0]);
}