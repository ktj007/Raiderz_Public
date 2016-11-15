#pragma once

#include "XCharacterParts.h"

class XCharacterBParts : public XCharacterParts
{
protected:
	virtual RActorNode* CreatePartsNode( RMesh* pMesh, const CSItemData* pItemData, tstring& strNodeName, tstring& strParentName, tstring& strActorNodeName, bool bNotifyToController = true );

public:
	XCharacterBParts(XCharacter* pCharacter);
};
