#include "stdafx.h"
#include "XSoundNonPlayer.h"
#include "XNonPlayer.h"
#include "XNPCInfo.h"
#include "XStrings.h"



XSoundNonPlayer::XSoundNonPlayer()
{
	m_bInteraction = false;
}


void XSoundNonPlayer::PlayVoice( const MUID& uID, const char* szType)
{
	XNonPlayer* pNPC = gg.omgr->FindNPC( uID);
	if ( pNPC == NULL)
	{
		m_bInteraction = false;
		return;
	}

	XNPCInfo* pNPCInfo = pNPC->GetInfo();
	if ( pNPCInfo == NULL  ||  pNPCInfo->eVoiceType == VOICE_TYPE_NONE)
		return;



	// Get sound type
	string strType;

	if ( szType != NULL)
	{
		strType = szType;

		if ( stricmp( szType, "BYE") == 0)		m_bInteraction = false;
	}
	else
	{
		if ( m_bInteraction == true)			return;
		m_bInteraction = true;

		switch ( pNPC->GetIConState())
		{
		case NIS_QUEST_BEGIN :	strType = "QUESTBEGIN";		break;
		case NIS_QUEST_END :	strType = "QUESTEND";		break;
		default :				strType = "HELLO";			break;
		}
	}

	if ( strType.empty() == true)
		return;



	// Select sound
	INTERACTION_VOICE_SAY eSayType = VOICE_SAY_NONE;

	if ( strType == string( "SHOP"))				eSayType = static_cast<INTERACTION_VOICE_SAY>( RandomNumber( VOICE_SAY_SHOP1, VOICE_SAY_SHOP3));
	else if ( strType == string( "QUESTBEGIN"))		eSayType = static_cast<INTERACTION_VOICE_SAY>( RandomNumber( VOICE_SAY_QUEST1, VOICE_SAY_QUEST3));
	else if ( strType == string( "QUESTEND"))		eSayType = static_cast<INTERACTION_VOICE_SAY>( RandomNumber( VOICE_SAY_QUEST_REPORT1, VOICE_SAY_QUEST_REPORT3));
	else if ( strType == string( "CRAFTDONE"))		eSayType = static_cast<INTERACTION_VOICE_SAY>( RandomNumber( VOICE_SAY_DONE_CRAFT1, VOICE_SAY_DONE_CRAFT3));
	else if ( strType == string( "HELLO"))			eSayType = static_cast<INTERACTION_VOICE_SAY>( RandomNumber( VOICE_SAY_HELLO1, VOICE_SAY_HELLO3));
	else if ( strType == string( "BYE"))			eSayType = static_cast<INTERACTION_VOICE_SAY>( RandomNumber( VOICE_SAY_BYE1, VOICE_SAY_BYE4));

	if ( eSayType >= VOICE_SAY_MAX  ||  eSayType <= VOICE_SAY_NONE ) return;


	// Get sound name
	char szSoundName[ 256] = {0,};
	sprintf_s( szSoundName, "%s_%s", MLocale::ConvUTF16ToAnsi(XStrings::VoiceTypeToString( pNPCInfo->eVoiceType)).c_str()
		, MLocale::ConvUTF16ToAnsi(XStrings::VoiceSayToString(eSayType)).c_str());

	if ( global.sound  &&  global.sound->IsExistSample( szSoundName) == false)
	{
		sprintf_s( szSoundName, "%s_%s", MLocale::ConvUTF16ToAnsi(XStrings::VoiceTypeToString( pNPCInfo->eVoiceType - (pNPCInfo->eVoiceType % 3))).c_str()
			, MLocale::ConvUTF16ToAnsi(XStrings::VoiceSayToString(eSayType)).c_str());
		if ( global.sound->IsExistSample( szSoundName) == false)
		{
			_ASSERT(0);
			return;
		}
	}

	//mlog( "Play sound : ID : %d, name : %s \n", pNPCInfo->nID, szSoundName);

	// Play sound
	if ( global.sound)
	{
		global.sound->StopDynamic3D( uID, true);
		global.sound->PlayDynamic3D( szSoundName, NULL, 0, uID);
	}
}


XSoundNonPlayer* XSoundNonPlayer::GetInstance()
{
	static XSoundNonPlayer instance;
	return &instance;
}
