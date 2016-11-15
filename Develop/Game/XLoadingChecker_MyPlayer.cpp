#include "stdafx.h"
#include "XLoadingChecker_MyPlayer.h"
#include "XMyPlayer.h"
#include "XGame.h"
#include "XGlobal.h"
#include "XModuleEntity.h"
#include "XCharacter.h"

bool XLoadingChecker_MyPlayer::CheckLoaded()
{
	if (gg.game == NULL) return true;
	if (gg.omgr == NULL) return true;

	XMyPlayer* pMyPlayer = gg.omgr->FindMyPlayer();
	if (pMyPlayer == NULL) return true;

	XModuleEntity* pEntity = pMyPlayer->GetModuleEntity();
	_ASSERT(pEntity);

	// XCharacter 파츠를 업데이트 해주어야 캐릭터리소스 로딩을 확인할수 있다.
	XCharacter* pChar = pEntity->GetActor();
	_ASSERT(pChar);
	pChar->UpdatePartsNode();

	// Entity로부터 로딩이 다 되었는지 확인하기
	pEntity->CheckLoadingCompleted();
	if (pEntity->IsLoadingCompleted() == false) return false;

	return true;
}