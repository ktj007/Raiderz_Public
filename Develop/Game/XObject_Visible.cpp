#include "stdafx.h"
#include "XObject_Visible.h"
#include "XGlobal.h"
#include "XAnim.h"

//MImplementRTTI(XObject_Visible, XObject);
//
//
//XObject_Visible::XObject_Visible() : XObject(), m_pSceneNode(NULL), m_nCurrAniID(0)
//{
//
//
//}
//
//XObject_Visible::~XObject_Visible()
//{
//	if (m_bCreated)
//	{
//		Destroy();
//	}
//}
//
//bool XObject_Visible::OnCreate()
//{
//	m_vPos = vec3::IDENTITY;
//	m_vDir = -vec3::AXISY;
//	m_vVel = vec3::IDENTITY;
//
//	// TODO: 메쉬가 하드코딩되어 있음
//	RMesh* pMesh = global.mmgr->Get(XCONST::MY_MODEL);
//	if (pMesh)
//	{
//		m_pSceneNode = m_Actor.Create( pMesh );
//		SetAnimation(string(ANIM_NAME_IDLE));
//
//		if (m_pSceneNode)
//		{
//			global.smgr->AddChild(m_pSceneNode);
//		}
//	}
//
//	// test
//	m_vPos = XCONST::START_POS;
//
//	SetPosition(m_vPos);
//	SetDirection(m_vDir);
//	SetAnimation(string(ANIM_NAME_IDLE));
//
//	return true;
//}
//
//void XObject_Visible::OnDestroy()
//{
//	if (m_pSceneNode)
//	{
//		global.smgr->RemoveChild(m_pSceneNode);
//		delete m_pSceneNode; m_pSceneNode = NULL;
//	}
//	XObject::OnDestroy();
//}
//
//void XObject_Visible::OnRender()
//{
//
//}
//
//void XObject_Visible::SetAnimation(string& strAniName)
//{
//	if (strAniName == m_strLastAni) return;
//
//	m_Actor.SetAnimation(AMode_All, (char*)strAniName.c_str());
//	m_strLastAni = strAniName;
//	m_nCurrAniID = 0;
//}
//
//void XObject_Visible::OnUpdate(float fDelta)
//{
//
//}
//
//void XObject_Visible::SetPosition(vec3& pos)
//{
//	m_vPos = pos;
//
//	MMatrix mat;
//	MakeModelMatrix(&mat, m_vPos, m_vDir, vec3(0,0,1));
//	m_Actor.SetWorldMatrix(mat);
//}
//
//void XObject_Visible::SetDirection(vec3& dir)
//{
//	m_vDir = dir;
//
//	MMatrix mat;
//	MakeModelMatrix(&mat, m_vPos, m_vDir, vec3(0,0,1));
//	m_Actor.SetWorldMatrix(mat);
//}
//
//
//
//
//
//
//
//
//
