#pragma once

#include "RActor.h"
#include "MTstring.h"

using namespace rs3;

class XCharacter;
class XCharacterParts;

struct XCoverMesh
{
	tstring				strCoverMeshSlot;
	bool				bSub;
	int					nIndex;

	XCoverMesh()
	{
		strCoverMeshSlot.clear();
		bSub			= false;
		nIndex			= 1;					// 1번은 무조건? 일단 Sub에만 적용합니다.
	}
};

class XCoverMeshList
{
private:
	XCharacter*					m_pCharacter;
	vector<XCoverMesh>			m_vecCoverMeshList;

	void AddCoverMeshList( tstring strUserProperty, bool bSub, int nIndex = 1 );
	void ShowCoverMesh( tstring strUserProperty );
	void HideCoverMesh();
public:
	XCoverMeshList(XCharacter* pCharacter) : m_pCharacter(pCharacter) {}
	void SetCoverMesh( RMeshNode* pMeshNode, bool bCover );
};
