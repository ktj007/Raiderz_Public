#ifndef _RPOLARCAMERA_SCENE_NODE_H
#define _RPOLARCAMERA_SCENE_NODE_H

#include "RCameraSceneNode.h"

namespace rs3 {

class RS_API RPolarCameraSceneNode : public RCameraSceneNode
{
protected:
	RPolarCoord		m_PolarCoord;
	RVector3		m_vLookAt;

public:
	RPolarCameraSceneNode(void);
	virtual ~RPolarCameraSceneNode(void);

	void UpdateCamera();
	void SetCoord(RPolarCoord* pCoord);
	void LookAt(RVector3& at);

	RVector3 GetLookAt()	{ return m_vLookAt; }
	RPolarCoord GetCoord()	{ return m_PolarCoord; }
};


}


#endif
