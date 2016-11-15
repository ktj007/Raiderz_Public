#include "stdafx.h"
#include "LCommObjectManager.h"
#include "LCommObject.h"

LCommObjectManager::LCommObjectManager() : SCommObjectManager()
{

}

LCommObjectManager::~LCommObjectManager()
{

}

SCommObject* LCommObjectManager::NewObject( MUID& uid )
{
	SCommObject* pNewPlayerObject = new LCommObject(uid);	
	pNewPlayerObject->Create();
	return pNewPlayerObject;
}
