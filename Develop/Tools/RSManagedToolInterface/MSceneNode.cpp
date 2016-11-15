#include "stdafx.h"
#include "MSceneNode.h"
#include "Vector3.h"

using namespace RSManaged;
using namespace System;

MSceneNode::MSceneNode() : m_pSN(NULL)
{

}

MSceneNode::MSceneNode(RSceneNode* pRealSceneNode)
{
	m_pSN = pRealSceneNode;
}

MSceneNode::~MSceneNode()
{
	m_pSN = NULL;
}

bool MSceneNode::AddChild(MSceneNode* pChild)
{
	return m_pSN->AddChild(pChild ? pChild->m_pSN : 0);	
}

bool MSceneNode::RemoveChild(MSceneNode* pChild)
{
	return m_pSN->RemoveChild(pChild ? pChild->m_pSN : 0);	
}

bool MSceneNode::RemoveFromParent()
{
	return m_pSN->RemoveFromParent();
}

//void MSceneNode::SetHierachy(bool bUse)
//{
//	m_pSN->SetHierachy(bUse);
//}

void MSceneNode::UpdateTransform()
{
	m_pSN->UpdateTransform();
}

rs3::RSceneNode* MSceneNode::get_NativeSceneNode()
{
	return m_pSN;
}

bool MSceneNode::get_Visible()
{
	return m_pSN->GetVisible();
}

void MSceneNode::set_Visible(bool bVisible)
{
	m_pSN->SetVisible(bVisible);
}

bool MSceneNode::get_Valid()
{
	if (m_pSN == NULL) return false;
	return true;
}

void MSceneNode::Message(System::String* msg, System::String* param)
{
	char* szMsg = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(msg);
	char* szParam = (char*)(void*)
		System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(param);

	m_pSN->Message(szMsg, (void*)szParam);
}