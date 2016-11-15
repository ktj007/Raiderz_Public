#include "stdafx.h"
#include "XGlueGameOption.h"
#include "XConfig.h"
#include "XGameInterface.h"


#define _CHECK(_val,_min,_max)		(_val<_min && _val>_max)



void XGlueGameOption::DeclGlueFunc( MWLua* pLua )
{
	MWLua::class_<XGlueGameOption>(pLua->GetState(), "_gameoption")

		.def( "BeginModifyOption",					&XGlueGameOption::BeginModifyOption)
		.def( "EndModifyOption",					&XGlueGameOption::EndModifyOption)

		.def( "SaveOption",							&XGlueGameOption::SaveOption)
		.def( "LoadOption",							&XGlueGameOption::LoadOption)
		.def( "SetDefaultSetting",					&XGlueGameOption::SetDefaultSetting)

		// Game
		.def( "EnableAutoContinuousAttack",			&XGlueGameOption::EnableAutoContinuousAttack)
		.def( "IsEnableAutoContinuousAttack",		&XGlueGameOption::IsEnableAutoContinuousAttack)
		.def( "EnableCombatStanceSitting",			&XGlueGameOption::EnableCombatStanceSitting)
		.def( "IsEnableCombatStanceSitting",		&XGlueGameOption::IsEnableCombatStanceSitting)
		.def( "EnableAutoChangeToNormalStance",		&XGlueGameOption::EnableAutoChangeToNormalStance)
		.def( "IsEnableAutoChangeToNormalStance",	&XGlueGameOption::IsEnableAutoChangeToNormalStance)
		.def( "ShowBuffName",						&XGlueGameOption::ShowBuffName)
		.def( "IsShowBuffName",						&XGlueGameOption::IsShowBuffName)
		.def( "ShowHelpMessage",					&XGlueGameOption::ShowHelpMessage)
		.def( "IsShowHelpMessage",					&XGlueGameOption::IsShowHelpMessage)

		.def( "ShowChatMsg",						&XGlueGameOption::ShowChatMsg)
		.def( "IsShowChatMsg",						&XGlueGameOption::IsShowChatMsg)
		.def( "ShowWhisperMsg",						&XGlueGameOption::ShowWhisperMsg)
		.def( "IsShowWhisperMsg",					&XGlueGameOption::IsShowWhisperMsg)
		.def( "ShowPartyMsg",						&XGlueGameOption::ShowPartyMsg)
		.def( "IsShowPartyMsg",						&XGlueGameOption::IsShowPartyMsg)
		.def( "ShowGuildMsg",						&XGlueGameOption::ShowGuildMsg)
		.def( "IsShowGuildMsg",						&XGlueGameOption::IsShowGuildMsg)
		.def( "ShowBubbleChat",						&XGlueGameOption::ShowBubbleChat)
		.def( "IsShowBubbleChat",					&XGlueGameOption::IsShowBubbleChat)
		.def( "RejectParty",						&XGlueGameOption::RejectParty)
		.def( "IsRejectParty",						&XGlueGameOption::IsRejectParty)
		.def( "RejectGuild",						&XGlueGameOption::RejectGuild)
		.def( "IsRejectGuild",						&XGlueGameOption::IsRejectGuild)
		.def( "RejectDuel",							&XGlueGameOption::RejectDuel)
		.def( "IsRejectDuel",						&XGlueGameOption::IsRejectDuel)
		.def( "RejectTrading",						&XGlueGameOption::RejectTrading)
		.def( "IsRejectTrading",					&XGlueGameOption::IsRejectTrading)
		.def( "ShowMyPlayerName",					&XGlueGameOption::ShowMyPlayerName)
		.def( "IsShowMyPlayerName",					&XGlueGameOption::IsShowMyPlayerName)
		.def( "ShowMyPlayerGuildName",				&XGlueGameOption::ShowMyPlayerGuildName)
		.def( "IsShowMyPlayerGuildName",			&XGlueGameOption::IsShowMyPlayerGuildName)
		.def( "ShowOtherPlayerName",				&XGlueGameOption::ShowOtherPlayerName)
		.def( "IsShowOtherPlayerName",				&XGlueGameOption::IsShowOtherPlayerName)
		.def( "ShowOtherPlayerGuildName",			&XGlueGameOption::ShowOtherPlayerGuildName)
		.def( "IsShowOtherPlayerGuildName",			&XGlueGameOption::IsShowOtherPlayerGuildName)
		.def( "ShowNpcName",						&XGlueGameOption::ShowNpcName)
		.def( "IsShowNpcName",						&XGlueGameOption::IsShowNpcName)
		.def( "ShowNpcGuildName",					&XGlueGameOption::ShowNpcGuildName)
		.def( "IsShowNpcGuildName",					&XGlueGameOption::IsShowNpcGuildName)

		.def( "ShowIndicator",						&XGlueGameOption::ShowIndicator)
		.def( "IsShowIndicator",					&XGlueGameOption::IsShowIndicator)
		.def( "ShowMIniHPBar",						&XGlueGameOption::ShowMIniHPBar)
		.def( "IsShowMIniHPBar",					&XGlueGameOption::IsShowMIniHPBar)
		.def( "ShowCaption",						&XGlueGameOption::ShowCaption)
		.def( "IsShowCaption",						&XGlueGameOption::IsShowCaption)

		.def( "RecipeRemove",						&XGlueGameOption::RecipeRemove)
		.def( "IsRecipeRemove",						&XGlueGameOption::IsRecipeRemove)

		.def( "AutoPartyMatch",						&XGlueGameOption::AutoPartyMatch)
		.def( "IsAutoPartyMatch",					&XGlueGameOption::IsAutoPartyMatch)

		.def( "GetCrossHairComboListCount",			&XGlueGameOption::GetCrossHairComboListCount)
		.def( "GetCrossHairName",					&XGlueGameOption::GetCrossHairName)

		// Video
		.def( "SnapshotDisplayResolutionList",		&XGlueGameOption::SnapshotDisplayResolutionList)
		.def( "GetDisplayResolutionListCount",		&XGlueGameOption::GetDisplayResolutionListCount)
		.def( "GetDisplayResolutionList",			&XGlueGameOption::GetDisplayResolutionList)
		.def( "SetScreenResolution",				&XGlueGameOption::SetScreenResolution)
		.def( "GetScreenResolution",				&XGlueGameOption::GetScreenResolution)
		.def( "SetLighting",						&XGlueGameOption::SetLighting)
		.def( "GetLighting",						&XGlueGameOption::GetLighting)
		.def( "SetTextureMipmapBias",				&XGlueGameOption::SetTextureMipmapBias)
		.def( "GetTextureMipmapBias",				&XGlueGameOption::GetTextureMipmapBias)
		.def( "SetTextureReduction",				&XGlueGameOption::SetTextureReduction)
		.def( "GetTextureReduction",				&XGlueGameOption::GetTextureReduction)
		.def( "SetGrndObjVisibleRange",				&XGlueGameOption::SetGrndObjVisibleRange)
		.def( "GetGrndObjVisibleRange",				&XGlueGameOption::GetGrndObjVisibleRange)
		.def( "EnableNormalMapping",				&XGlueGameOption::EnableNormalMapping)
		.def( "IsEnableNormalMapping",				&XGlueGameOption::IsEnableNormalMapping)
		.def( "EnableFullScreen",					&XGlueGameOption::EnableFullScreen)
		.def( "IsEnableFullScreen",					&XGlueGameOption::IsEnableFullScreen)
		.def( "EnableHDR",							&XGlueGameOption::EnableHDR)
		.def( "IsEnableHDR",						&XGlueGameOption::IsEnableHDR)
		.def( "EnableDOF",							&XGlueGameOption::EnableDOF)
		.def( "IsEnableDOF",						&XGlueGameOption::IsEnableDOF)
		.def( "SetShadow",							&XGlueGameOption::SetShadow)
		.def( "GetShadow",							&XGlueGameOption::GetShadow)
		.def( "SetSSAO",							&XGlueGameOption::SetSSAO)
		.def( "GetSSAO",							&XGlueGameOption::GetSSAO)
		.def( "EnableDistortion",					&XGlueGameOption::EnableDistortion)
		.def( "IsEnableDistortion",					&XGlueGameOption::IsEnableDistortion)
		.def( "EnableMotionBlur",					&XGlueGameOption::EnableMotionBlur)
		.def( "IsEnableMotionBlur",					&XGlueGameOption::IsEnableMotionBlur)
		.def( "EnableSoftParticle",					&XGlueGameOption::EnableSoftParticle)
		.def( "IsEnableSoftParticle",				&XGlueGameOption::IsEnableSoftParticle)

		.def( "SetEncodingType",					&XGlueGameOption::SetEncodingType)
		.def( "GetEncodingType",					&XGlueGameOption::GetEncodingType)
		.def( "SetEncodingResolution",				&XGlueGameOption::SetEncodingResolution)
		.def( "GetEncodingResolution",				&XGlueGameOption::GetEncodingResolution)
		.def( "ShowUIWhenRecording",				&XGlueGameOption::ShowUIWhenRecording)
		.def( "IsShowUIWhenRecording",				&XGlueGameOption::IsShowUIWhenRecording)

		.def( "SetScreenShotFormatType",			&XGlueGameOption::SetScreenShotFormatType)
		.def( "GetScreenShotFormatType",			&XGlueGameOption::GetScreenShotFormatType)

		// Audio
		.def( "MuteAll",							&XGlueGameOption::MuteAll)
		.def( "IsMuteAll",							&XGlueGameOption::IsMuteAll)
		.def( "MuteGenericSound",					&XGlueGameOption::MuteGenericSound)
		.def( "IsMuteGenericSound",					&XGlueGameOption::IsMuteGenericSound)
		.def( "MuteBGMSound",						&XGlueGameOption::MuteBGMSound)
		.def( "IsMuteBGMSound",						&XGlueGameOption::IsMuteBGMSound)
		.def( "MuteEffectSound",					&XGlueGameOption::MuteEffectSound)
		.def( "IsMuteEffectSound",					&XGlueGameOption::IsMuteEffectSound)
		.def( "SetMasterVolumn",					&XGlueGameOption::SetMasterVolumn)
		.def( "GetMasterVolumn",					&XGlueGameOption::GetMasterVolumn)
		.def( "SetGenericVolumn",					&XGlueGameOption::SetGenericVolumn)
		.def( "GetGenericVolumn",					&XGlueGameOption::GetGenericVolumn)
		.def( "SetBGMVolumn",						&XGlueGameOption::SetBGMVolumn)
		.def( "GetBGMVolumn",						&XGlueGameOption::GetBGMVolumn)
		.def( "SetEffectVolumn",					&XGlueGameOption::SetEffectVolumn)
		.def( "GetEffectVolumn",					&XGlueGameOption::GetEffectVolumn)
		.def( "EnableHardwareAccelerate",			&XGlueGameOption::EnableHardwareAccelerate)
		.def( "IsEnableHardwareAccelerate",			&XGlueGameOption::IsEnableHardwareAccelerate)
		.def( "EnableReverb",						&XGlueGameOption::EnableReverb)
		.def( "IsEnableReverb",						&XGlueGameOption::IsEnableReverb)
		.def( "EnableCompressor",					&XGlueGameOption::EnableCompressor)
		.def( "IsEnableCompressor",					&XGlueGameOption::IsEnableCompressor)
		.def( "SetSpeakerMode",						&XGlueGameOption::SetSpeakerMode)
		.def( "GetSpeakerMode",						&XGlueGameOption::GetSpeakerMode)
		.def( "InActiveSound",						&XGlueGameOption::InActiveSound)
		.def( "IsInActiveSound",					&XGlueGameOption::IsInActiveSound)
		.def( "SetSystemMessage",					&XGlueGameOption::SetSystemMessage)
		.def( "GetSystemMessage",					&XGlueGameOption::GetSystemMessage)

		// Controls
		.def( "SetInvertedMouse",					&XGlueGameOption::SetInvertedMouse)
		.def( "IsInvertedMouse",					&XGlueGameOption::IsInvertedMouse)
		.def( "SetMouseSensitivity",				&XGlueGameOption::SetMouseSensitivity)
		.def( "GetMouseSensitivity",				&XGlueGameOption::GetMouseSensitivity)

		//Field Map
		.def( "ShowCraftIcon",						&XGlueGameOption::ShowCraftIcon)
		.def( "IsShowCraftIcon",					&XGlueGameOption::IsShowCraftIcon)
		.def( "ShowShopIcon",						&XGlueGameOption::ShowShopIcon)
		.def( "IsShowShopIcon",						&XGlueGameOption::IsShowShopIcon)
		.def( "ShowQuestIcon",						&XGlueGameOption::ShowQuestIcon)
		.def( "IsShowQuestIcon",					&XGlueGameOption::IsShowQuestIcon)
		.def( "ShowAreaIcon",						&XGlueGameOption::ShowAreaIcon)
		.def( "IsShowAreaIcon",						&XGlueGameOption::IsShowAreaIcon)

		// Cross Hair
		.def( "SetCrossHairCurrentType",			&XGlueGameOption::SetCrossHairCurrentType)
		.def( "GetCrossHairCurrentType",			&XGlueGameOption::GetCrossHairCurrentType)
		;
}



void XGlueGameOption::BeginModifyOption()
{
	XGetGameInterface().OPTIONS.BeginModifyOption();
}

void XGlueGameOption::EndModifyOption()
{
	XGetGameInterface().OPTIONS.EndModifyOption();
}

void XGlueGameOption::SaveOption()
{
	XGetGameInterface().OPTIONS.SaveOption();
}

void XGlueGameOption::LoadOption()
{
	XGetGameInterface().OPTIONS.LoadOption();
}

void XGlueGameOption::SetDefaultSetting()
{
	XGetGameInterface().OPTIONS.SetDefaultSetting();
}


// Game
bool XGlueGameOption::EnableAutoContinuousAttack( bool _val)
{
	XSETCFG_GAME_AUTONORMALATTACK( _val);
	return XGETCFG_GAME_AUTONORMALATTACK;
}

bool XGlueGameOption::IsEnableAutoContinuousAttack()
{
	return XGETCFG_GAME_AUTONORMALATTACK;
}

bool XGlueGameOption::EnableCombatStanceSitting( bool _val)
{
	XSETCFG_GAME_COMBATSTANCESITTING( _val);
	return XGETCFG_GAME_COMBATSTANCESITTING;
}

bool XGlueGameOption::IsEnableCombatStanceSitting()
{
	return XGETCFG_GAME_COMBATSTANCESITTING;
}

bool XGlueGameOption::EnableAutoChangeToNormalStance( bool _val)
{
	XSETCFG_GAME_AUTOCHANGE_TONORMALSTANCE( _val);
	return XGETCFG_GAME_AUTOCHANGE_TONORMALSTANCE;
}

bool XGlueGameOption::IsEnableAutoChangeToNormalStance()
{
	return XGETCFG_GAME_AUTOCHANGE_TONORMALSTANCE;
}

bool XGlueGameOption::ShowBuffName( bool _val)
{
	XSETCFG_GAME_SHOWBUFFNAME( _val);
	return XGETCFG_GAME_SHOWBUFFNAME;
}

bool XGlueGameOption::IsShowBuffName()
{
	return XGETCFG_GAME_SHOWBUFFNAME;
}

bool XGlueGameOption::ShowHelpMessage( bool _val)
{
	XSETCFG_GAME_SHOWHELPMESSAGE( _val);
	return XGETCFG_GAME_SHOWHELPMESSAGE;
}

bool XGlueGameOption::IsShowHelpMessage()
{
	return XGETCFG_GAME_SHOWHELPMESSAGE;
}

bool XGlueGameOption::ShowChatMsg( bool _val)
{
	XSETCFG_GAME_SHOWCHATMSG( _val);
	return XGETCFG_GAME_SHOWCHATMSG;
}

bool XGlueGameOption::IsShowChatMsg()
{
	return XGETCFG_GAME_SHOWCHATMSG;
}

bool XGlueGameOption::ShowWhisperMsg( bool _val)
{
	XSETCFG_GAME_SHOWWHISPERMSG( _val);
	return XGETCFG_GAME_SHOWWHISPERMSG;
}

bool XGlueGameOption::IsShowWhisperMsg()
{
	return XGETCFG_GAME_SHOWWHISPERMSG;
}

bool XGlueGameOption::ShowPartyMsg( bool _val)
{
	XSETCFG_GAME_SHOWPARTYMSG( _val);
	return XGETCFG_GAME_SHOWPARTYMSG;
}

bool XGlueGameOption::IsShowPartyMsg()
{
	return XGETCFG_GAME_SHOWPARTYMSG;
}

bool XGlueGameOption::ShowGuildMsg( bool _val)
{
	XSETCFG_GAME_SHOWGUILDMSG( _val);
	return XGETCFG_GAME_SHOWGUILDMSG;
}

bool XGlueGameOption::IsShowGuildMsg()
{
	return XGETCFG_GAME_SHOWGUILDMSG;
}

bool XGlueGameOption::ShowBubbleChat( bool _val)
{
	XSETCFG_GAME_SHOWCHATBUBBLE( _val);
	return XGETCFG_GAME_SHOWCHATBUBBLE;
}

bool XGlueGameOption::IsShowBubbleChat()
{
	return XGETCFG_GAME_SHOWCHATBUBBLE;
}

bool XGlueGameOption::RejectParty( bool _val)
{
	XSETCFG_GAME_REJECT_PARTY( _val);
	return XGETCFG_GAME_REJECT_PARTY;
}

bool XGlueGameOption::IsRejectParty()
{
	return XGETCFG_GAME_REJECT_PARTY;
}

bool XGlueGameOption::RejectGuild( bool _val)
{
	XSETCFG_GAME_REJECT_GUILD( _val);
	return XGETCFG_GAME_REJECT_GUILD;
}

bool XGlueGameOption::IsRejectGuild()
{
	return XGETCFG_GAME_REJECT_GUILD;
}

bool XGlueGameOption::RejectDuel( bool _val)
{
	XSETCFG_GAME_REJECT_DUEL( _val);
	return XGETCFG_GAME_REJECT_DUEL;
}

bool XGlueGameOption::IsRejectDuel()
{
	return XGETCFG_GAME_REJECT_DUEL;
}

bool XGlueGameOption::RejectTrading( bool _val)
{
	XSETCFG_GAME_REJECT_TRADE( _val);
	return XGETCFG_GAME_REJECT_TRADE;
}

bool XGlueGameOption::IsRejectTrading()
{
	return XGETCFG_GAME_REJECT_TRADE;
}

bool XGlueGameOption::ShowMyPlayerName( bool _val)
{
	XSETCFG_GAME_MYPLAYERNAME( _val);
	return XGETCFG_GAME_MYPLAYERNAME;
}

bool XGlueGameOption::IsShowMyPlayerName()
{
	return XGETCFG_GAME_MYPLAYERNAME;
}

bool XGlueGameOption::ShowMyPlayerGuildName( bool _val)
{
	XSETCFG_GAME_MYPLAYERGUILD( _val);
	return XGETCFG_GAME_MYPLAYERGUILD;
}

bool XGlueGameOption::IsShowMyPlayerGuildName()
{
	return XGETCFG_GAME_MYPLAYERGUILD;
}

bool XGlueGameOption::ShowOtherPlayerName( bool _val)
{
	XSETCFG_GAME_PLAYERNAME( _val);
	return XGETCFG_GAME_PLAYERNAME;
}

bool XGlueGameOption::IsShowOtherPlayerName()
{
	return XGETCFG_GAME_PLAYERNAME;
}

bool XGlueGameOption::ShowOtherPlayerGuildName( bool _val)
{
	XSETCFG_GAME_PLAYERGUILD( _val);
	return XGETCFG_GAME_PLAYERGUILD;
}

bool XGlueGameOption::IsShowOtherPlayerGuildName()
{
	return XGETCFG_GAME_PLAYERGUILD;
}

bool XGlueGameOption::ShowNpcName( bool _val)
{
	XSETCFG_GAME_NPCNAME( _val);
	return XGETCFG_GAME_NPCNAME;
}

bool XGlueGameOption::IsShowNpcName()
{
	return XGETCFG_GAME_NPCNAME;
}

bool XGlueGameOption::ShowNpcGuildName( bool _val)
{
	XSETCFG_GAME_NPCGUILD( _val);
	return XGETCFG_GAME_NPCGUILD;
}

bool XGlueGameOption::IsShowNpcGuildName()
{
	return XGETCFG_GAME_NPCGUILD;
}

bool XGlueGameOption::ShowIndicator( bool _val)
{
	XSETCFG_GAME_INDICATOR( _val);
	return XGETCFG_GAME_INDICATOR;
}

bool XGlueGameOption::IsShowIndicator()
{
	return XGETCFG_GAME_INDICATOR;
}

bool XGlueGameOption::ShowMIniHPBar( bool _val)
{
	XSETCFG_GAME_MINIHPBAR( _val);
	return XGETCFG_GAME_MINIHPBAR;
}

bool XGlueGameOption::IsShowMIniHPBar()
{
	return XGETCFG_GAME_MINIHPBAR;
}

bool XGlueGameOption::ShowCaption( bool _val)
{
	XSETCFG_GAME_CAPTION( _val);
	return XGETCFG_GAME_CAPTION;
}

bool XGlueGameOption::IsShowCaption()
{
	return XGETCFG_GAME_CAPTION;
}

bool XGlueGameOption::RecipeRemove( bool _val)
{
	XSETCFG_GAME_RECIPE_REMOVE( _val);
	return XGETCFG_GAME_RECIPE_REMOVE;
}

bool XGlueGameOption::IsRecipeRemove()
{
	return XGETCFG_GAME_RECIPE_REMOVE;
}

bool XGlueGameOption::AutoPartyMatch( bool _val)
{
	XSETCFG_GAME_USE_AUTOPARTY( _val);
	return XGETCFG_GAME_USE_AUTOPARTY;
}

bool XGlueGameOption::IsAutoPartyMatch()
{
	return XGETCFG_GAME_USE_AUTOPARTY;
}

// Video
void XGlueGameOption::SnapshotDisplayResolutionList()
{
	XGetGameInterface().OPTIONS.GetDisplayResInfo( m_vDisplayResInfo);
}

int XGlueGameOption::GetDisplayResolutionListCount()
{
	return (int)m_vDisplayResInfo.size();
}

MWLua::ret_int2 XGlueGameOption::GetDisplayResolutionList( int i)
{
	return MWLua::ret_int2( m_vDisplayResInfo[ i].w, m_vDisplayResInfo[ i].h);
}

MWLua::ret_int2 XGlueGameOption::SetScreenResolution( int _w, int _h)
{
	XGetGameInterface().OPTIONS.SetScreenResolution( _w, _h);

	int w, h;
	XGetGameInterface().OPTIONS.GetScreenResolution( &w, &h);
	return MWLua::ret_int2( w, h);
}

MWLua::ret_int2 XGlueGameOption::GetScreenResolution()
{
	int w, h;
	XGetGameInterface().OPTIONS.GetScreenResolution( &w, &h);
	return MWLua::ret_int2( w, h);
}

int XGlueGameOption::SetLighting( int _val)
{
	XGetGameInterface().OPTIONS.SetLighting( _val);
	return GetLighting();
}

int XGlueGameOption::GetLighting()
{
	return XGETCFG_VIDEO_LIGHTING;
}

float XGlueGameOption::SetTextureMipmapBias( float _val)
{
	XGetGameInterface().OPTIONS.SetTextureMipmapBias( _val);
	return GetTextureMipmapBias();
}

float XGlueGameOption::GetTextureMipmapBias()
{
	return XGETCFG_VIDEO_TEXTUREMIPMAPBIAS;
}

int XGlueGameOption::SetTextureReduction( int _val)
{
	XGetGameInterface().OPTIONS.SetTextureReduction( _val);
	return GetTextureReduction();
}

int XGlueGameOption::GetTextureReduction()
{
	return XGETCFG_VIDEO_TEXTUREREDUCTION;
}

int XGlueGameOption::SetGrndObjVisibleRange( int _val)
{
	XGetGameInterface().OPTIONS.SetGrndObjVisibleRange( _val);

	XSETCFG_VIDEO_GRNDOBJVISIBLERANGE( _val);
	return XGETCFG_VIDEO_GRNDOBJVISIBLERANGE;
}

int XGlueGameOption::GetGrndObjVisibleRange()
{
	return XGETCFG_VIDEO_GRNDOBJVISIBLERANGE;
}

bool XGlueGameOption::EnableNormalMapping( bool _val)
{
	XGetGameInterface().OPTIONS.EnableNormalMapping( _val);
	return XGETCFG_VIDEO_NORMALMAPPING;
}

bool XGlueGameOption::IsEnableNormalMapping()
{
	return XGETCFG_VIDEO_NORMALMAPPING;
}

bool XGlueGameOption::EnableFullScreen( bool _val)
{
	XGetGameInterface().OPTIONS.SetFullScreen( _val);
	return XGETCFG_VIDEO_FULLSCREEN;
}

bool XGlueGameOption::IsEnableFullScreen()
{
	return XGETCFG_VIDEO_FULLSCREEN;
}

bool XGlueGameOption::EnableHDR( bool _val)
{
	XGetGameInterface().OPTIONS.EnableHDR( _val);
	return XGETCFG_VIDEO_HDR;
}

bool XGlueGameOption::IsEnableHDR()
{
	return XGETCFG_VIDEO_HDR;
}

bool XGlueGameOption::EnableDOF( bool _val)
{
	XGetGameInterface().OPTIONS.EnableDOF( _val);
	return XGETCFG_VIDEO_DOF;
}

bool XGlueGameOption::IsEnableDOF()
{
	return XGETCFG_VIDEO_DOF;
}

int XGlueGameOption::SetShadow( int _val)
{
	XGetGameInterface().OPTIONS.SetShadow( _val);
	return XGETCFG_VIDEO_SHADOW;
}

int XGlueGameOption::GetShadow()
{
	return XGETCFG_VIDEO_SHADOW;
}

int XGlueGameOption::SetSSAO( int _val)
{
	XGetGameInterface().OPTIONS.SetSSAO( _val);
	return XGETCFG_VIDEO_SSAO;
}

int XGlueGameOption::GetSSAO()
{
	return XGETCFG_VIDEO_SSAO;
}

bool XGlueGameOption::EnableDistortion( bool _val)
{
	XGetGameInterface().OPTIONS.SetDistortion( _val);
	return XGETCFG_VIDEO_DISTORTION;
}

bool XGlueGameOption::IsEnableDistortion()
{
	return XGETCFG_VIDEO_DISTORTION;
}

bool XGlueGameOption::EnableMotionBlur( bool _val)
{
	XGetGameInterface().OPTIONS.SetMotionBlur( _val);
	return XGETCFG_VIDEO_MOTIONBLUR;
}

bool XGlueGameOption::IsEnableMotionBlur()
{
	return XGETCFG_VIDEO_MOTIONBLUR;
}

bool XGlueGameOption::EnableSoftParticle( bool _val)
{
	XGetGameInterface().OPTIONS.SetSoftParticle( _val);
	return XGETCFG_VIDEO_SOFTPARTICLE;
}

bool XGlueGameOption::IsEnableSoftParticle()
{
	return XGETCFG_VIDEO_SOFTPARTICLE;
}

const char* XGlueGameOption::SetEncodingType( const char* szType)
{
	XSETCFG_VIDEO_MOVIEENCODINGTYPE(string(szType));
	return GetEncodingType();
}

const char* XGlueGameOption::GetEncodingType()
{
	gvar.Temp.szTemp = XGETCFG_VIDEO_MOVIEENCODINGTYPE;
	return gvar.Temp.szTemp.c_str();
}

int XGlueGameOption::SetEncodingResolution( int _val)
{
	if ( _CHECK( _val, 0, 3))				_val = 0;

	XSETCFG_VIDEO_MOVIEENCODINGRES( _val);
	return XGETCFG_VIDEO_MOVIEENCODINGRES;
}

int XGlueGameOption::GetEncodingResolution()
{
	return XGETCFG_VIDEO_MOVIEENCODINGRES;
}

bool XGlueGameOption::ShowUIWhenRecording( bool _val)
{
	XSETCFG_VIDEO_SHOWUIWHENREC( _val);
	return XGETCFG_VIDEO_SHOWUIWHENREC;
}

bool XGlueGameOption::IsShowUIWhenRecording()
{
	return XGETCFG_VIDEO_SHOWUIWHENREC;
}


// Audio
bool XGlueGameOption::MuteAll( bool _val)
{
	XGetGameInterface().OPTIONS.MuteAll( _val);
	return XGETCFG_AUDIO_MUTE;
}

bool XGlueGameOption::IsMuteAll()
{
	return XGETCFG_AUDIO_MUTE;
}

bool XGlueGameOption::MuteGenericSound( bool _val)
{
	XGetGameInterface().OPTIONS.MuteGenericSound( _val);
	return XGETCFG_AUDIO_MUTEGENERICSOUND;
}

bool XGlueGameOption::IsMuteGenericSound()
{
	return XGETCFG_AUDIO_MUTEGENERICSOUND;
}

bool XGlueGameOption::MuteBGMSound( bool _val)
{
	XGetGameInterface().OPTIONS.MuteBgmSound( _val);
	return XGETCFG_AUDIO_MUTEBGMSOUND;
}

bool XGlueGameOption::IsMuteBGMSound()
{
	return XGETCFG_AUDIO_MUTEBGMSOUND;
}

bool XGlueGameOption::MuteEffectSound( bool _val)
{
	XGetGameInterface().OPTIONS.MuteEffectSound( _val);
	return XGETCFG_AUDIO_MUTEEFFECTSOUND;
}

bool XGlueGameOption::IsMuteEffectSound()
{
	return XGETCFG_AUDIO_MUTEEFFECTSOUND;
}

float XGlueGameOption::SetMasterVolumn( float _val)
{
	XGetGameInterface().OPTIONS.SetMasterVolume( _val);
	return XGETCFG_AUDIO_MASTERVOLUME;
}

float XGlueGameOption::GetMasterVolumn()
{
	return XGETCFG_AUDIO_MASTERVOLUME;
}

float XGlueGameOption::SetGenericVolumn( float _val)
{
	XGetGameInterface().OPTIONS.SetGenericVolume( _val);
	return XGETCFG_AUDIO_GENERICVOLUME;
}

float XGlueGameOption::GetGenericVolumn()
{
	return XGETCFG_AUDIO_GENERICVOLUME;
}

float XGlueGameOption::SetBGMVolumn( float _val)
{
	XGetGameInterface().OPTIONS.SetBgmVolume( _val);
	return XGETCFG_AUDIO_BGMVOLUME;
}

float XGlueGameOption::GetBGMVolumn()
{
	return XGETCFG_AUDIO_BGMVOLUME;
}

float XGlueGameOption::SetEffectVolumn( float _val)
{
	XGetGameInterface().OPTIONS.SetEffectVolume( _val);
	return XGETCFG_AUDIO_EFFECTVOLUME;
}

float XGlueGameOption::GetEffectVolumn()
{
	return XGETCFG_AUDIO_EFFECTVOLUME;
}

bool XGlueGameOption::EnableHardwareAccelerate( bool _val)
{
	XGetGameInterface().OPTIONS.EnableHardwareAccelerate( _val);
	return XGETCFG_AUDIO_HARDWAREACC;
}

bool XGlueGameOption::IsEnableHardwareAccelerate()
{
	return XGETCFG_AUDIO_HARDWAREACC;
}

bool XGlueGameOption::EnableReverb( bool _val)
{
	XGetGameInterface().OPTIONS.EnableReverb( _val);
	return XGETCFG_AUDIO_REVERB;
}

bool XGlueGameOption::IsEnableReverb()
{
	return XGETCFG_AUDIO_REVERB;
}

bool XGlueGameOption::EnableCompressor( bool _val)
{
	XGetGameInterface().OPTIONS.EnableCompressor( _val);
	return XGETCFG_AUDIO_COMPRESSOR;
}

bool XGlueGameOption::IsEnableCompressor()
{
	return XGETCFG_AUDIO_COMPRESSOR;
}

int XGlueGameOption::SetSpeakerMode( int _val)
{
	XGetGameInterface().OPTIONS.SetSpeakerMode( _val);
	return XGETCFG_AUDIO_SPEAKERMODE;
}

int XGlueGameOption::GetSpeakerMode()
{
	return XGETCFG_AUDIO_SPEAKERMODE;
}

bool XGlueGameOption::InActiveSound(bool _val)
{
	XGetGameInterface().OPTIONS.MuteInActiveSound(_val);
	return XGETCFG_AUDIO_INACTIVESOUND;
}

bool XGlueGameOption::IsInActiveSound()
{
	return XGETCFG_AUDIO_INACTIVESOUND;
}

int XGlueGameOption::SetSystemMessage(int _val)
{
	XGetGameInterface().OPTIONS.SetSystemMessage(_val);
	return XGETCFG_AUDIO_SYSTEMMESSAGE;
}

int XGlueGameOption::GetSystemMessage()
{
	return XGETCFG_AUDIO_SYSTEMMESSAGE;
}


// Controls
bool XGlueGameOption::SetInvertedMouse( bool _val)
{
	XSETCFG_CONTROL_INVERTEDMOUSE( _val);
	return XGETCFG_CONTROL_INVERTEDMOUSE;
}

bool XGlueGameOption::IsInvertedMouse()
{
	return XGETCFG_CONTROL_INVERTEDMOUSE;
}

float XGlueGameOption::SetMouseSensitivity( float _val)
{
	if ( _CHECK( _val, 0, 100))				_val = 50;

	XSETCFG_CONTROL_MOUSESENSITIVITY( _val);
	return XGETCFG_CONTROL_MOUSESENSITIVITY;
}

float XGlueGameOption::GetMouseSensitivity()
{
	return XGETCFG_CONTROL_MOUSESENSITIVITY;
}

bool XGlueGameOption::ShowCraftIcon( bool _val)
{
	XSETCFG_GAME_SHOWCRAFTICONE( _val);
	return XGETCFG_GAME_SHOWCRAFTICON;
}

bool XGlueGameOption::IsShowCraftIcon()
{
	return XGETCFG_GAME_SHOWCRAFTICON;
}

bool XGlueGameOption::ShowShopIcon( bool _val)
{
	XSETCFG_GAME_SHOWSHOPICON( _val);
	return XGETCFG_GAME_SHOWSHOPICON;
}

bool XGlueGameOption::IsShowShopIcon()
{
	return XGETCFG_GAME_SHOWSHOPICON;
}

bool XGlueGameOption::ShowQuestIcon( bool _val)
{
	XSETCFG_GAME_SHOWQUESTICON( _val);
	return XGETCFG_GAME_SHOWQUESTICON;
}

bool XGlueGameOption::IsShowQuestIcon()
{
	return XGETCFG_GAME_SHOWQUESTICON;
}

bool XGlueGameOption::ShowAreaIcon( bool _val)
{
	XSETCFG_GAME_SHOWAREAICON( _val);
	return XGETCFG_GAME_SHOWAREAICON;
}

bool XGlueGameOption::IsShowAreaIcon()
{
	return XGETCFG_GAME_SHOWAREAICON;
}

const char* XGlueGameOption::SetScreenShotFormatType( const char* szType )
{
	XSETCFG_VIDEO_SCREENSHOTFORMAT(string(szType));
	return GetScreenShotFormatType();
}

const char* XGlueGameOption::GetScreenShotFormatType()
{
	gvar.Temp.szTemp = XGETCFG_VIDEO_SCREENSHOTFORMAT;
	return gvar.Temp.szTemp.c_str();
}

const char* XGlueGameOption::SetCrossHairCurrentType( const char* szType )
{
	XSETCFG_GAME_CROSSHAIR(string(szType));
	return GetCrossHairCurrentType();
}

const char* XGlueGameOption::GetCrossHairCurrentType()
{
	gvar.Temp.szTemp = XGETCFG_GAME_CROSSHAIR;
	return gvar.Temp.szTemp.c_str();
}

int XGlueGameOption::GetCrossHairComboListCount()
{
	return global.ui->GetCrossHairListCount();
}

const char* XGlueGameOption::GetCrossHairName( int nIndex )
{
	gvar.Temp.szTemp = MLocale::ConvUTF16ToAnsi(global.ui->GetCrossHairName(nIndex));
	return gvar.Temp.szTemp.c_str();
}
