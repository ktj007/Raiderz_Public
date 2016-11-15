#pragma once

#include "XGlueFunction.h"


/// class : XGlueGameOption
class XGlueGameOption : public XGlueFunction
{
/// Member variables
protected:
	vector< MSIZE>			m_vDisplayResInfo;



/// Member functions
public:
	virtual void DeclGlueFunc( MWLua* pLua);

	void BeginModifyOption();
	void EndModifyOption();

	void SaveOption();
	void LoadOption();

	void SetDefaultSetting();


	// Game
	bool EnableAutoContinuousAttack( bool _val);
	bool IsEnableAutoContinuousAttack();
	bool EnableCombatStanceSitting( bool _val);
	bool IsEnableCombatStanceSitting();
	bool EnableAutoChangeToNormalStance( bool _val);
	bool IsEnableAutoChangeToNormalStance();

	bool ShowBuffName( bool _val);
	bool IsShowBuffName();

	bool ShowHelpMessage( bool _val);
	bool IsShowHelpMessage();

	bool ShowChatMsg( bool _val);
	bool IsShowChatMsg();

	bool ShowWhisperMsg( bool _val);
	bool IsShowWhisperMsg();

	bool ShowPartyMsg( bool _val);
	bool IsShowPartyMsg();

	bool ShowGuildMsg( bool _val);
	bool IsShowGuildMsg();
	
	bool ShowBubbleChat( bool _val);
	bool IsShowBubbleChat();

	bool RejectParty( bool _val);
	bool IsRejectParty();

	bool RejectGuild( bool _val);
	bool IsRejectGuild();

	bool RejectDuel( bool _val);
	bool IsRejectDuel();

	bool RejectTrading( bool _val);
	bool IsRejectTrading();

	bool ShowMyPlayerName( bool _val);
	bool IsShowMyPlayerName();

	bool ShowMyPlayerGuildName( bool _val);
	bool IsShowMyPlayerGuildName();

	bool ShowOtherPlayerName( bool _val);
	bool IsShowOtherPlayerName();

	bool ShowOtherPlayerGuildName( bool _val);
	bool IsShowOtherPlayerGuildName();

	bool ShowNpcName( bool _val);
	bool IsShowNpcName();

	bool ShowNpcGuildName( bool _val);
	bool IsShowNpcGuildName();

	bool ShowIndicator( bool _val);
	bool IsShowIndicator();

	bool ShowMIniHPBar( bool _val);
	bool IsShowMIniHPBar();

	bool ShowCaption( bool _val);
	bool IsShowCaption();

	bool RecipeRemove( bool _val);
	bool IsRecipeRemove();

	bool AutoPartyMatch( bool _val);
	bool IsAutoPartyMatch();

	int	GetCrossHairComboListCount();
	const char* GetCrossHairName(int nIndex);

	// Video
	void SnapshotDisplayResolutionList();
	int GetDisplayResolutionListCount();
	MWLua::ret_int2 GetDisplayResolutionList( int i);

	MWLua::ret_int2 SetScreenResolution( int _w, int _h);
	MWLua::ret_int2 GetScreenResolution();
	
	int SetLighting( int nLighting);
	int GetLighting();

	float SetTextureMipmapBias( float _val);
	float GetTextureMipmapBias();

	int SetTextureReduction( int _val);
	int GetTextureReduction();

	int SetGrndObjVisibleRange( int _val);
	int GetGrndObjVisibleRange();

	bool EnableNormalMapping( bool _val);
	bool IsEnableNormalMapping();

	bool EnableFullScreen( bool _val);
	bool IsEnableFullScreen();

	bool EnableHDR( bool _val);
	bool IsEnableHDR();

	bool EnableDOF( bool _val);
	bool IsEnableDOF();

	int SetShadow( int _val);
	int GetShadow();

	int SetSSAO( int _val);
	int GetSSAO();

	bool EnableDistortion( bool _val);
	bool IsEnableDistortion();

	bool EnableMotionBlur( bool _val);
	bool IsEnableMotionBlur();

	bool EnableSoftParticle( bool _val);
	bool IsEnableSoftParticle();

	const char* SetEncodingType( const char* szType);
	const char* GetEncodingType();

	int SetEncodingResolution( int _val);
	int GetEncodingResolution();

	bool ShowUIWhenRecording( bool _val);
	bool IsShowUIWhenRecording();

	const char* SetScreenShotFormatType(const char* szType);
	const char* GetScreenShotFormatType();

	// Audio
	bool MuteAll( bool _val);
	bool IsMuteAll();

	bool MuteGenericSound( bool _val);
	bool IsMuteGenericSound();

	bool MuteBGMSound( bool _val);
	bool IsMuteBGMSound();

	bool MuteEffectSound( bool _val);
	bool IsMuteEffectSound();

	float SetMasterVolumn( float _val);
	float GetMasterVolumn();

	float SetGenericVolumn( float _val);
	float GetGenericVolumn();

	float SetBGMVolumn( float _val);
	float GetBGMVolumn();

	float SetEffectVolumn( float _val);
	float GetEffectVolumn();

	bool EnableHardwareAccelerate( bool _val);
	bool IsEnableHardwareAccelerate();

	bool EnableReverb( bool _val);
	bool IsEnableReverb();

	bool EnableCompressor( bool _val);
	bool IsEnableCompressor();

	int SetSpeakerMode( int _val);
	int GetSpeakerMode();

	bool InActiveSound(bool _val);
	bool IsInActiveSound();

	int SetSystemMessage(int _val);
	int GetSystemMessage();


	// Controls
	bool SetInvertedMouse( bool _val);
	bool IsInvertedMouse();

	float SetMouseSensitivity( float _val);
	float GetMouseSensitivity();

	//Field Map
	bool ShowCraftIcon( bool _val);
	bool IsShowCraftIcon();

	bool ShowShopIcon( bool _val);
	bool IsShowShopIcon();

	bool ShowQuestIcon( bool _val);
	bool IsShowQuestIcon();

	bool ShowAreaIcon( bool _val);
	bool IsShowAreaIcon();

	const char* SetCrossHairCurrentType(const char* szType);
	const char* GetCrossHairCurrentType();
};
