#include "StdAfx.h"
#include "RSceneNodeControllerFactory.h"
#include "RActorNodeConstraintController.h"

namespace rs3 {

const char *RCID_ACTORNODECONSTRAINT = "ACTORNODECONSTRAINTCONTROLLER";

RSceneNodeController* RSceneNodeControllerFactory::CreateSceneNodeController(const char* _pSzControllerName)
{
	RSceneNodeController *pNewController = NULL;
	if(stricmp(_pSzControllerName,RCID_ACTORNODECONSTRAINT)==0)
		pNewController = new RActorNodeConstraintController;
	else
	{
		_ASSERT(0);
	}

	return pNewController;
}

}