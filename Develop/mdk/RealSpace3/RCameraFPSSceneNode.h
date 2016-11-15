#ifndef _RCAMERA_FPS_SCENE_NODE_H
#define _RCAMERA_FPS_SCENE_NODE_H

#include "RCameraSceneNode.h"


namespace rs3 {

class RS_API RCameraFPSSceneNode : public RCameraSceneNode
{
protected:
	unsigned long int m_nLastTime;
	float		m_fRotateSpeed;
	float		m_fMoveSpeed;
	float		m_fCameraX;
	float		m_fCameraZ;
	RVector		m_vDir;

	virtual void OnPreRender( RCameraSceneNode* pCurrentCamera);
	bool IsKeyDown(int vKey);
public:
	RCameraFPSSceneNode(void);
	virtual ~RCameraFPSSceneNode(void);
	void SetRotateSpeed(float fSpeed)		{ m_fRotateSpeed = fSpeed; }
	void SetMoveSpeed(float fSpeed)			{ m_fMoveSpeed = fSpeed; }
};


}


#endif
