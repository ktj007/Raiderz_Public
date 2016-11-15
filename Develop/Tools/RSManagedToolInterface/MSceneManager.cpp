#include "stdafx.h"
#include "MSceneManager.h"
#include "MSceneNode.h"
#include "MLightSceneNode.h"
#include "MCameraSceneNode.h"
#include <vcclr.h> // for PtrToStringChars

using namespace RSManaged;
using namespace System;

MSceneManager::MSceneManager() : MSceneNode(), m_pActiveCamera(NULL)
{

}

void MSceneManager::Set(RSceneManager* pSM)
{
	m_pSM = pSM;
	m_pSN = m_pSM;
}

void MSceneManager::Clear()
{
	m_pSM->Clear();
}

bool MSceneManager::Create()
{
	return m_pSM->Create();
}

void MSceneManager::Destroy()
{
	m_pSM->Destroy();
}


void MSceneManager::SetCurrentPass(RSManaged::RRENDERPASS pass)
{
	m_pSM->SetCurrentPass((rs3::RRENDERPASS)pass);
}

RSManaged::RRENDERPASS MSceneManager::GetCurrentPass()
{
	rs3::RRENDERPASS ret = m_pSM->GetCurrentPass();
	return (RSManaged::RRENDERPASS)ret;
}

void MSceneManager::Update()
{
	m_pSM->Update();
}

void MSceneManager::Render()
{
	m_pSM->Render();
}

bool MSceneManager::LoadXML(System::String* szFileName)
{

	return false;
}

bool MSceneManager::SaveXML(System::String* szFileName)
{

	return false;
}


MSceneNode* MSceneManager::CreateSceneNode(System::String* szNodeName)
{
	char* str = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(szNodeName);

	rs3::RSceneNode* pSceneNode = m_pSM->CreateSceneNode(str);
	return new MSceneNode(pSceneNode);
}

void MSceneManager::AddLight(MLightSceneNode* pLight)
{
	rs3::RLightSceneNode* pNode = (rs3::RLightSceneNode*)pLight->NativeSceneNode;
	m_pSM->AddChild(pNode);
}

MCameraSceneNode* MSceneManager::SetActiveCamera(MCameraSceneNode *pCamera)
{
	m_pActiveCamera = pCamera;
	rs3::RCameraSceneNode* node = m_pSM->SetPrimaryCamera(pCamera ? (rs3::RCameraSceneNode*)pCamera->NativeSceneNode : 0);
	return m_pActiveCamera;
}

MCameraSceneNode* MSceneManager::GetActiveCamera()
{
	return m_pActiveCamera;
}


//void MSceneManager::AttachAABBTreeNode(MSceneNode* pAABBTreeNode)
//{
//	rs3::RAABBTreeSceneManager* pSM = (rs3::RAABBTreeSceneManager*)m_pSM;
//	pSM->AttachAABBTreeNode(pAABBTreeNode ? (rs3::RAABBTreeSceneNode*)pAABBTreeNode->NativeSceneNode : 0);
//}