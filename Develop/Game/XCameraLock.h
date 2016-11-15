#pragma once

#include "XCameraSceneNodeController.h"

#include "XBaseCameraSceneNode.h"

class XBaseCameraSceneNode;

class XCameraLock
{
private:
	MUID					m_uidTarget;
	MUID					m_uidOwner;
	wstring					m_strBoneName;

	XPolarCoord				m_BackViewPolarCoord;

	float					m_fElapsedTime;
	float					m_fMoveTime;
	float					m_fDurationTime;

	float					m_fDis;

	vec3					m_vStartPos;
	vec3					m_vStartDir;
	vec3					m_vEndPos;
	vec3					m_vEndDir;

	bool					m_bCameraLock;
	bool					m_bCameraLockMove;
	bool					m_bCameraLockEnd;

	// µð¹ö±ë¿ë
	bool					m_bWithOutLimitTime;

private:
	void					Init();

	bool					GetCameraLockPolarCoord(MUID uidOwner, MUID uidTarget, wstring strBoneName, vec3& vPosition, vec3& vDirection);

	void					UpdateDurationTime(float fDelta, XPolarCoord* pCur, RCameraSceneNode* pCameraSceneNode);

public:
	XCameraLock(void);
	virtual ~XCameraLock(void);

	void					StartEndUpdate(float fDelta, XPolarCoord* pCur, RCameraSceneNode* pCameraSceneNode, vec3 vTar, vec3& vPosition, vec3& vDirection);
	void					CalcPosDirForCameraLock(float fDelta, XPolarCoord* pCur, RCameraSceneNode* pCameraSceneNode, vec3& vPosition, vec3& vDirection);

	void					Start(RCameraSceneNode* pCameraSceneNode, XPolarCoord* pCur, MUID& uidOwner, MUID& uidTarget, wstring& strBoneName, float& fDurationTime);
	void					End( XPolarCoord* pCur, RCameraSceneNode* pCameraSceneNode );
	//void					ForceEnd();

	bool					IsCameraLockActive();
	bool					IsCameraLockStartEnd();
	bool					IsCameraLock()		{ return m_bCameraLock; }
};
