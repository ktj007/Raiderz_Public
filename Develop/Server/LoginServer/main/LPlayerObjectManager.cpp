#include "stdafx.h"
#include "LPlayerObjectManager.h"
#include "LPlayerObject.h"

LPlayerObjectManager::LPlayerObjectManager() : SCommObjectManager()
{

}

LPlayerObjectManager::~LPlayerObjectManager()
{

}

SCommObject* LPlayerObjectManager::NewObject( MUID& uid )
{
	SCommObject* pNewPlayerObject = new LPlayerObject(uid);	
	pNewPlayerObject->Create();
	return pNewPlayerObject;
}

LPlayerObject* LPlayerObjectManager::GetPlayer( const MUID& uidPlayer )
{
	return static_cast<LPlayerObject*>(GetClient(uidPlayer));
}
