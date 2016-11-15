#include "StdAfx.h"
#include "TAnimationResourceController.h"
#include "RFile.h"

TAnimationResourceController::TAnimationResourceController( RSceneManager* pSceneManager )
: RAnimationResource("", pSceneManager)
{

}

int TAnimationResourceController::GetAnimationMaxFrame( string strAniFileName )
{
	RFile rf;
	if(!rf.Open(strAniFileName.c_str())) 
	{
		mlog_tool("%s file not found!! \r", strAniFileName.c_str() );
		return false;
	}

	LoadAniHeader(rf, strAniFileName.c_str());

	rf.Close();
	return GetMaxFrame();
}