#pragma once

namespace rs3 {

struct RRESOURCETYPE
{
	// -----------------------------------------
	// 씬 매니저로부터 관리되는것
	// -----------------------------------------
	enum SCENE_MANAGER_MANAGED
	{
		RR_MESH = 0,
		RR_TREE,
		RR_WATER,
		RR_MATERIAL,
		RR_ANIMATION,
		RR_GENERAL,

		// 끝
		RR_MAX
	};

	// -----------------------------------------
	// 씬 매니저로부터 관리되지 않는 리소스
	// -----------------------------------------
	enum UNMANAGED_RESOURCE
	{
		RR_UNKNOWN		= RR_MAX,
		RR_TEXTURE,			// RDevice로터 관리된다.
		RR_SHADERFX,		// RDevice로터 관리된다.
		RR_TERRAIN,
		RR_STATICMESH,

		RR_UNMANAGED_MAX
	};

	static const char* GetResoruceIDString( int _nId )
	{
		if( _nId<0 || _nId>RR_UNMANAGED_MAX)
			return NULL;

		static const char * pSzResourceID[] = {

			//-------------- managed
			"mesh",
			"tree",
			"water",
			"material",
			"animation",
			"general",

			"_managed_max",

			//-------------- unmanaged
			"texture",
			"shaderfx",
			"terrain",
			"staticmesh",

			"_unmanaged_max"
		};
		return pSzResourceID[ _nId ];
	}
};


}
