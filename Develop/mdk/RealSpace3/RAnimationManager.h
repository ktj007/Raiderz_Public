#pragma once

#include <map>
#include <string>

#include "RResourceID.h" //RESOURCETYPE
#include "RResourceManager.h"

namespace rs3 {

class RS_API RAnimationManager : public RResourceManager
{

public:
	RAnimationManager(){}
	virtual ~RAnimationManager(){}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Resource Manager ±¸Çö
	//
	DECLARE_RES_MGR_ID( RRESOURCETYPE::RR_ANIMATION);

	virtual void		Init( RSceneManager* pSceneManager ){}
	virtual void		Destroy();
	virtual void		Clear();
};

}	//namespace rs3