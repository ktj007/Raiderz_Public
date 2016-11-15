#include "StdAfx.h"

#include "RCameraAnimationController.h"
#include "RCameraSceneNode.h"

namespace rs3 {

void RCameraSceneNodeController::RemoveFromCamera()
{
	if (m_pCameraSceneNode)
	{
		RCameraSceneNode::CAMERA_CONTROLLER_MAP::iterator itr = m_pCameraSceneNode->m_mapCameraController[m_eType].find( GetCameraControllerID() );

		if( itr != m_pCameraSceneNode->m_mapCameraController[m_eType].end() )
		{
			OnRemoveFromCamera();
			m_pCameraSceneNode->m_mapCameraController[m_eType].erase(itr);

			m_pCameraSceneNode = NULL;
		}
	}
}

}
