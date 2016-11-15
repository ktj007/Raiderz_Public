#include "StdAfx.h"
#include "XCameraInteraction.h"
#include "XCameraManager.h"
#include "XModuleEntity.h"

#define INTERACTION_CAMERA_MOVE_TIME 0.4f
#define INTERACTION_CAMERA_MOVE_FONT 200.0f
#define INTERACTION_CAMERA_MOVE_DOWN 20.0f
#define INTERACTION_CAMERA_MOVE_SIDE 150.0f
#define INTERACTION_CAMERA_MOVE_OFFSET_RIGHT 10.0f
#define INTERACTION_CAMERA_TARGET_FONT 20.0f
#define INTERACTION_CAMERA_TARGET_DOWN 10.0f

XCameraInteraction::XCameraInteraction( void )
: m_bStart(false)
, m_bEnd(false)
, m_bDone(false)
, m_bActive(false)
, m_fElapsedTime(0.0f)
, m_fMoveTime(0.0f)
{

}

XCameraInteraction::~XCameraInteraction( void )
{

}

void XCameraInteraction::UpdateCamera( float fDelta, vec3 vOwnerPos, vec3& vPosition, vec3& vDirection )
{
	if(m_bStart == false &&
		m_bEnd == false)
		return;

	m_fElapsedTime += fDelta;
	float fd = m_fElapsedTime / m_fMoveTime;

	if(fd >= 1.0f)
		fd = 1.0f;

	vec3 vStartPos = vOwnerPos + m_vStartLocalPos;
	vec3 vTargetPos = vOwnerPos + m_vTargetLocalPos;
	vStartPos.InterpolateTo(vTargetPos, fd, m_vCurrentPos);
	m_vStartDir.InterpolateTo(m_vTargetDir, fd, m_vCurrentDir);

	vPosition = m_vCurrentPos;
	vDirection = m_vCurrentDir;

	if(m_bEnd && fd >= 1.0f)
	{
		m_bDone = true;
		m_bEnd = false;
		m_bActive = false;
	}
}

void XCameraInteraction::Init( RCameraSceneNode* pCameraSceneNode, vec3 vOwnerPos )
{
	m_vStartLocalPos = pCameraSceneNode->GetPosition() - vOwnerPos;
	m_vStartDir = pCameraSceneNode->GetDirection();

	m_vTargetLocalPos = m_vStartLocalPos;
	m_vTargetDir = m_vStartDir;

	m_vCurrentPos = vOwnerPos + m_vStartLocalPos;
	m_vCurrentDir = m_vStartDir;

	//////////////////////////////////////////////////////////////////////////
	// 기본 데이터
	m_bStart		= true;
	m_fElapsedTime	= 0.0f;
	m_bDone			= false;
	m_bEnd			= false;
	m_fMoveTime		= INTERACTION_CAMERA_MOVE_TIME;
}

void XCameraInteraction::Start( MUID uidTarget, vec3 vOwnerPos, RCameraSceneNode* pCameraSceneNode )
{
	if (gg.omgr == NULL) return;
	XObject* pObject = gg.omgr->Find(uidTarget);
	if (pObject == NULL) return;

	Init(pCameraSceneNode, vOwnerPos);

	vec3 vTargetPos = pObject->GetPosition();
	vec3 vTargetStartPos = vTargetPos + vec3(0, 0, XCONST::CAMERA_TARGET_HEIGHT);

	//////////////////////////////////////////////////////////////////////////
	// 카메라위치 결정
	vec3 vDir = (vOwnerPos - vTargetPos);
	XMath::NormalizeZ0(vDir);
	
	vec3 vTargetCalPos = vTargetStartPos + (vDir * INTERACTION_CAMERA_MOVE_FONT);

	//////////////////////////////////////////////////////////////////////////
	// 방향 결정
	CAMERA_POSITION_TYPE eCPT = GetPositionType(vDir);

	// 우측
	vec3 vRight = vDir.CrossProduct(vec3::AXISZ);
	if(eCPT == CPT_RIGHT)
		vRight *= -1.0f;

	vTargetCalPos += (vRight * INTERACTION_CAMERA_MOVE_SIDE) + vec3(0,0,-INTERACTION_CAMERA_MOVE_DOWN);
	m_vTargetLocalPos = vTargetCalPos - vOwnerPos; 

	//////////////////////////////////////////////////////////////////////////
	// 바라볼 방향
	float fOffset = 0.0f;
	if(eCPT == CPT_RIGHT)
		fOffset = INTERACTION_CAMERA_MOVE_OFFSET_RIGHT;
	m_vTargetDir = (vTargetStartPos + (vDir * INTERACTION_CAMERA_TARGET_FONT) + (vRight * fOffset) + vec3(0,0,-INTERACTION_CAMERA_TARGET_DOWN)) - vTargetCalPos;
	m_vTargetDir.Normalize();

	m_bActive = true;
}

void XCameraInteraction::End(vec3 vOwnerPos)
{
	m_bStart		= false;
	m_bEnd			= true;
	m_fElapsedTime	= 0.0f;

	float fMoveLen_org = ((vOwnerPos + m_vStartLocalPos) - (vOwnerPos + m_vTargetLocalPos)).Length();

	vec3 vTempPos = m_vCurrentPos;
	vec3 vTempDir = m_vCurrentDir;

	m_vTargetLocalPos = m_vStartLocalPos;
	m_vTargetDir = m_vStartDir;

	m_vStartLocalPos = vTempPos - vOwnerPos;
	m_vStartDir = vTempDir;

	m_vCurrentPos = vOwnerPos + m_vStartLocalPos;
	m_vCurrentDir = m_vStartDir;

	float fMoveLen_cur = ((vOwnerPos + m_vStartLocalPos) - (vOwnerPos + m_vTargetLocalPos)).Length();

	if(fMoveLen_cur <= 0.0f ||
		fMoveLen_org <= 0.0f)
		m_fMoveTime = 0.0f;
	else
		m_fMoveTime = (INTERACTION_CAMERA_MOVE_TIME * fMoveLen_cur) / fMoveLen_org;
}

XCameraInteraction::CAMERA_POSITION_TYPE XCameraInteraction::GetPositionType( vec3 vOwnerDir )
{
	vec3 vCross = vOwnerDir.CrossProduct(m_vStartDir);
	if(vCross.z > 0.0f)
		return CPT_LEFT;

	return CPT_RIGHT;
}

void XCameraInteraction::ForceEnd()
{
	m_bStart = false;
	m_bEnd = false;
	m_bDone = false;
	m_bActive = false;
}
