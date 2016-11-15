#include "stdafx.h"
#include "XActorLoader.h"
#include "XCharacter.h"

//////////////////////////////////////////////////////////////////////////
// RActorLoader
XActorLoader::XActorLoader(void)
{

}

XActorLoader::~XActorLoader()
{

}

XCharacter* XActorLoader::CreateActor()
{
	return new XCharacter();
}

void XActorLoader::DeleteActor(XCharacter* pActor, bool bBackgroundCreation)
{
	PFC_THISFUNC;
	SAFE_DELETE(pActor);
}

void XActorLoader::Destroy()
{

}

