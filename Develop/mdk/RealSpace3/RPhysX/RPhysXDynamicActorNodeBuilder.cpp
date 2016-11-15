#include "StdAfx.h"

#pragma push_macro("min")
#pragma push_macro("max")
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include "NxPhysics.h"

#include "NxController.h"
#include "NxControllerManager.h"

#pragma pop_macro("min")
#pragma pop_macro("max")

#include "RActor.h"
#include "RMeshNode.h"
#include "RPhysXDynamicActorNodeBuilder.h"

#include "REngine.h"
#include "RSceneManager.h"
#include "RDebugPassRenderer.h"

namespace rs3 {

RPhysXDynamicActorNodeBuilder::~RPhysXDynamicActorNodeBuilder()
{
	for (std::list<RPhysXDynamicActorNodeInfo*>::iterator itr = m_listInfos.begin(); itr != m_listInfos.end(); ++itr)
	{
		delete *itr;
	}
}

bool RPhysXDynamicActorNodeBuilder::Build( RPhysXActorController* pPhysXController )
{
	DWORD dwAttr = RPhysXActorController::APPLY_ACTOR_POS | RPhysXActorController::APPLY_ACTOR_AXIS;
	for (std::list<RPhysXDynamicActorNodeInfo*>::iterator itr = m_listInfos.begin(); itr != m_listInfos.end(); ++itr)
	{
		RPhysXDynamicActorNodeInfo* pActorNodeInfo = *itr;
		PushBackNxUpdateInfo(pPhysXController, pActorNodeInfo->m_strActorNodeName, pActorNodeInfo->m_pNxActor, NULL, dwAttr);
	}

	RefreshNxUpdateInfoMap(pPhysXController);
	return true;
}

}
