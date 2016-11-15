#include "stdafx.h"
#include "XDummyCollision.h"
#include "XCollisionTree.h"

XDummyCollisionMgr* XDummyCollisionMgr::GetInstance()
{
	static XDummyCollisionMgr mgr;
	return &mgr;
}

XDummyCollision* XDummyCollisionMgr::Get( int nFieldID )
{
	iterator itor = find(nFieldID);
	if (itor != end())
	{
		return (*itor).second;
	}
	return NULL;
}

void XDummyCollisionMgr::Load( int nFieldID, const char* szCollisionFileName )
{
	if (Get(nFieldID) != NULL) return;

	XDummyCollision* pNewDummyCollision = new XDummyCollision();
	pNewDummyCollision->Load(szCollisionFileName);

	insert(value_type(nFieldID, pNewDummyCollision));
}

XDummyCollisionMgr::~XDummyCollisionMgr()
{
	for (iterator itor = begin(); itor != end(); ++itor)
	{
		XDummyCollision* pDummyCollision = (*itor).second;
		delete pDummyCollision;
	}
	clear();
}
//////////////////////////////////////////////////////////////////////////
XDummyCollision::XDummyCollision()
{
	m_pCollisionTree = new XCollisionTree();
}

XDummyCollision::~XDummyCollision()
{
	if (m_pCollisionTree)
	{
		m_pCollisionTree->Destroy();
	}
	SAFE_DELETE(m_pCollisionTree);
}

void XDummyCollision::Load( const char* szFileName )
{
	m_pCollisionTree->Load(szFileName);
}

bool XDummyCollision::Pick( RCollisionPickInfo& pickInfo )
{
	if (m_pCollisionTree)
	{
		return m_pCollisionTree->Pick(pickInfo, RCollisionTriangle::ALL);
	}
	return false;
}