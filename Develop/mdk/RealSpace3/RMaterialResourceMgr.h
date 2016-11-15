#pragma once

#include <map>
#include <string>

#include "RResourceID.h" //RESOURCETYPE
#include "RResourceManager.h"

namespace rs3 {

	class RS_API RMaterialResourceMgr : public RResourceManager
	{

	public:
		RMaterialResourceMgr(){}
		virtual ~RMaterialResourceMgr(){}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		// Resource Manager ±¸Çö
		//
		DECLARE_RES_MGR_ID( RRESOURCETYPE::RR_MATERIAL );

		virtual void		Init( RSceneManager* pSceneManager ){}
		virtual void		Destroy();
		virtual void		Clear();
	};

}	//namespace rs3