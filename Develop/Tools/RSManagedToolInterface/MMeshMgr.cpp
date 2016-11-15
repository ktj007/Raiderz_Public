#include "stdafx.h"
#include "MMeshMgr.h"
#include "MMesh.h"

using namespace RSManaged;
using namespace System;


MMeshMgr::MMeshMgr()
{
	m_pMeshMgr = new rs3::RMeshMgr();
}

MMeshMgr::~MMeshMgr()
{
	delete m_pMeshMgr;
}

MMesh* MMeshMgr::Add(System::String* name, System::String* modelname)
{
	char* szName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);
	char* szModelName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(modelname);

	if (modelname == "") szModelName = NULL;
	RMesh* pMesh = m_pMeshMgr->Add(szName, szModelName);
	return new MMesh(pMesh);
}

int MMeshMgr::AddXml(System::String* name, System::String* modelname, bool autoload)
{
	char* szName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);
	char* szModelName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(modelname);

	return m_pMeshMgr->AddXml(szName, szModelName, autoload);
}

int MMeshMgr::LoadXmlList(System::String* name)
{
	char* szName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

	return m_pMeshMgr->LoadXmlList(szName);
}

void MMeshMgr::Del(int id)
{
	m_pMeshMgr->Del(id);
}

void MMeshMgr::Del(MMesh* pMesh)
{
	
}

int MMeshMgr::LoadList(System::String* name)
{
	char* szName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

	return m_pMeshMgr->LoadList(szName);
}

int MMeshMgr::SaveList(System::String* name)
{
	char* szName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

	return m_pMeshMgr->SaveList(szName);
}

void MMeshMgr::DelAll()
{
	m_pMeshMgr->DelAll();
}

MMesh* MMeshMgr::GetFast(int id)
{
	RMesh* pMesh = m_pMeshMgr->GetFast(id);
	return new MMesh(pMesh);
}

MMesh* MMeshMgr::Get(System::String* name)
{
	char* szName = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name);

	RMesh* pMesh = m_pMeshMgr->Get(szName);
	return new MMesh(pMesh);
}
