#ifndef _RCAMERA_ARCBALL_SCENE_NODE_H
#define _RCAMERA_ARCBALL_SCENE_NODE_H

#include "RCameraSceneNode.h"

namespace rs3 {

class RS_API RCameraArcBallSceneNode : public RCameraSceneNode
{
protected:
	RVector m_at;
	float m_fDist;

	virtual void OnPreRender( RCameraSceneNode* pCurrentCamera);
	virtual void OnMessage(char* szMsg, void* param);
public:
	RCameraArcBallSceneNode(void);
	virtual ~RCameraArcBallSceneNode(void);

	void Reset();
	void Zoom(float n);
	
	void RotationH(float n);
	void RotationV(float n);
	void MoveAtPoint(const RVector3 &vec);

	const rs3::RMatrix& GetProjectionMatrix() const
	{
		return m_projMatrix;
	}
};


}


#endif
