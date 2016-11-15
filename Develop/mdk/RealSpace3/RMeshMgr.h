#ifndef _RMeshMgr_h
#define _RMeshMgr_h

#include "MDebug.h"

#include "RMesh.h"
#include <vector>
#include "RResourceManager.h"

#include "RResourceID.h"

namespace rs3 {

class RS_API RMeshMgr : public RResourceManager
{
public:

	// 리소스 매니저 타입 설정
	DECLARE_RES_MGR_ID( RRESOURCETYPE::RR_MESH );


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 생성 / 소멸
	//
	RMeshMgr();
	virtual ~RMeshMgr();

	virtual void				Init( RSceneManager* pSceneManager ){ m_pSceneManager = pSceneManager; }
	virtual void				Destroy();
};

}

#endif//_RMeshMgr_h
