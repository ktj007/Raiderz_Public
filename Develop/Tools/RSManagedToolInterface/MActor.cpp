#include "stdafx.h"
#include "MFramework.h"
#include "MActor.h"
#include "MSceneNode.h"
#include "MMesh.h"

using namespace RSManaged;
using namespace System;


MActor::MActor()
{
	m_pSN = new rs3::RActor();
}

MActor::MActor(RSceneNode* pRealSceneNode) : MSceneNode(pRealSceneNode)
{
	
}

MActor::~MActor()
{
	
}

bool MActor::Create(MMesh* pMesh)
{
	if (!Valid) return false;
	return (m_pActor->Create(pMesh->NativeMesh));
}

rs3::RActor* MActor::get_NativeActor()
{
	return m_pActor;
}

bool MActor::get_Valid()
{
	if (m_pActor == NULL) return false;
	return true;
}

void MActor::Destroy()
{
	m_pActor->Destroy();
}

bool MActor::SetAnimation(RSManaged::RAniPartsMode animode,System::String* ani_name)
{
	char* szName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ani_name);

//	return m_pActor->SetAnimation((rs3::RAniPartsMode)animode, szName);
	return m_pActor->SetAnimation(szName);
}

//void MActor::Frame(float fTime)
//{
//	m_pActor->Frame(fTime);
//}
//
//void MActor::Render(float fTime)
//{
//	m_pActor->Render(fTime);
//}
