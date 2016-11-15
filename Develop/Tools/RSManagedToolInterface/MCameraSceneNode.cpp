#include "stdafx.h"
#include "MCameraSceneNode.h"
#include "NativeConverter.h"

using namespace RSManaged;
using namespace System;


MCameraSceneNode::MCameraSceneNode() : MSceneNode()
{
	m_pSN = new rs3::RCameraSceneNode();
}

MCameraSceneNode::MCameraSceneNode(RSceneNode* pRealSceneNode) : MSceneNode(pRealSceneNode)
{
	
}

MCameraSceneNode::~MCameraSceneNode()
{

}

void MCameraSceneNode::Set(Vector3 pos, Vector3 dir, Vector3 up)
{
	rs3::RCameraSceneNode* pCamera = (rs3::RCameraSceneNode*)m_pSN;
	pCamera->Set(NativeConverter::getNativeVector(pos),
				 NativeConverter::getNativeVector(dir),
				 NativeConverter::getNativeVector(up));
}

void MCameraSceneNode::SetFov(float fFovY)
{
	rs3::RCameraSceneNode* pCamera = (rs3::RCameraSceneNode*)m_pSN;
	pCamera->SetFov(fFovY);
}

void MCameraSceneNode::SetFov(float fFovY,float fAspectRatio)
{
	rs3::RCameraSceneNode* pCamera = (rs3::RCameraSceneNode*)m_pSN;
	pCamera->SetFov(fFovY, fAspectRatio);
}

void MCameraSceneNode::SetNearFarZ(float fNearZ,float fFarZ)
{
	rs3::RCameraSceneNode* pCamera = (rs3::RCameraSceneNode*)m_pSN;
	pCamera->SetNearFarZ(fNearZ, fFarZ);
}


Vector3 MCameraSceneNode::get_Position()
{
	return NativeConverter::getNETVector(((rs3::RCameraSceneNode*)m_pSN)->GetPosition());
}

Vector3 MCameraSceneNode::get_Direction()
{
	return NativeConverter::getNETVector(((rs3::RCameraSceneNode*)m_pSN)->GetDirection());
}



////////////////////////////////////////////////////////////////////////////////////////////////


MPolarCameraSceneNode::MPolarCameraSceneNode()
{
	m_pSN = new rs3::RPolarCameraSceneNode();
}

MPolarCameraSceneNode::MPolarCameraSceneNode(RSceneNode* pRealSceneNode) : MCameraSceneNode(pRealSceneNode)
{

}

MPolarCameraSceneNode::~MPolarCameraSceneNode()
{

}

void MPolarCameraSceneNode::UpdateCamera()
{
	rs3::RPolarCameraSceneNode* pCamera = (rs3::RPolarCameraSceneNode*)m_pSN;
	pCamera->UpdateCamera();
}

void MPolarCameraSceneNode::set_Coord(RSMPolarCoord pc)
{
	rs3::RPolarCameraSceneNode* pCamera = (rs3::RPolarCameraSceneNode*)m_pSN;
	RPolarCoord _pc = NativeConverter::getNativePolarCoord(pc);
	pCamera->SetCoord(&_pc);
}


RSMPolarCoord MPolarCameraSceneNode::get_Coord()
{
	rs3::RPolarCameraSceneNode* pCamera = (rs3::RPolarCameraSceneNode*)m_pSN;
	return NativeConverter::getNETPolarCoord(pCamera->GetCoord());
}

void MPolarCameraSceneNode::set_LookAt(Vector3 at)
{
	rs3::RPolarCameraSceneNode* pCamera = (rs3::RPolarCameraSceneNode*)m_pSN;
	MVector3 _at = NativeConverter::getNativeVector(at);
	pCamera->LookAt(_at);
}

Vector3 MPolarCameraSceneNode::get_LookAt()
{
	rs3::RPolarCameraSceneNode* pCamera = (rs3::RPolarCameraSceneNode*)m_pSN;
	return NativeConverter::getNETVector(pCamera->GetLookAt());
}