#include "stdafx.h"
#include "XModuleSound.h"
#include "XNonPlayer.h"
#include "XMyPlayer.h"

#define BASE_COMPARE_DISTANCE	400.0f
#define HUGE_COMPARE_DISTANCE	800.0f

XModuleSound::XModuleSound( XObject* pOwner/*=NULL*/ )
: XModule(pOwner)
, m_bModelSoundPlaying(false)
{

}

XModuleSound::~XModuleSound( void )
{

}

void XModuleSound::OnUpdate( float fDelta )
{
	PFC_THISFUNC;
	CheckPlayModelSound(fDelta);
}

// TODO : 엔티티 사운드 사운드 매니저에 등록해놓고 채널을 지켜보다가 플레이가 아니면
//		: 플레이 시키도록 (루프 사운드 아님)
// XModuleEntity -> XModuleSound으로 옮김
void XModuleSound::CheckPlayModelSound( float fDelta )
{
	// NPC만 작동
	if (m_pOwner->GetEntityType() != ETID_NPC)
		return;

	XNonPlayer* pNPC = AsNPC(m_pOwner);
	if(pNPC == NULL) return;
	if (pNPC->IsDead() == true) return;
	if (gvar.Game.pMyPlayer == NULL ) return;


	XNPCInfo* pInfo = pNPC->GetInfo();
	if ( pInfo  &&  pInfo->SoundInfo.EntitySound.empty() == false)
	{
		m_bModelSoundPlaying = true;

		if ( global.sound)
			global.sound->PlayDynamic3D( MLocale::ConvUTF16ToAnsi(pInfo->SoundInfo.EntitySound.c_str()).c_str(), NULL, 0, m_pOwner->GetUID());
	}
}