#include "stdafx.h"
#include "MActorMgr.h"

using namespace RSManaged;
using namespace System;


MActorMgr::MActorMgr()
{
	m_pActorMgr = new rs3::RActorMgr();
}

MActorMgr::~MActorMgr()
{
	delete m_pActorMgr;
}

int MActorMgr::Add(MMesh* pMesh)
{
	if (!pMesh->Valid) return -1;

	return m_pActorMgr->Add(pMesh->NativeMesh);
}

int MActorMgr::Add(MActor* pActor)
{
	if (!pActor->Valid) return -1;

	return m_pActorMgr->Add(pActor->NativeActor);
}

void MActorMgr::Del(int id)
{
	m_pActorMgr->Del(id);
}

void MActorMgr::Del(MActor* pActor)
{
	m_pActorMgr->Del(pActor->NativeActor);
}

void MActorMgr::DelAll()
{
	m_pActorMgr->DelAll();
}

void MActorMgr::Frame(float fTime)
{
	m_pActorMgr->Frame(fTime);
}

void MActorMgr::Frame(float fTime, int id)
{
	m_pActorMgr->Frame(fTime, id);
}

MActor* MActorMgr::GetFast(int id)
{
	rs3::RActor* pActor = m_pActorMgr->GetFast(id);
	return new MActor(pActor);
}
