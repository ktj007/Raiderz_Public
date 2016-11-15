
#include "stdafx.h"
#include "XGameInterfaceOption.h"
#include "XBaseApplication.h"
#include "XCameraManager.h"
#include "XController.h"
#include "RShadowStrategy.h"
//#include "XWidgetNameDef.h"
//#include "XGame.h"
//#include "XFeedbackSystem.h"
#include "RShadowStrategy.h"
#include "XPath.h"
#include "XGameInterface.h"
#include "RTreeSceneNode.h"


// XGameInterfaceOption
XGameInterfaceOption::XGameInterfaceOption()
{
	m_bModifyOption = false;
}


// ~XGameInterfaceOption
XGameInterfaceOption::~XGameInterfaceOption()
{
}


// LoadOption
bool XGameInterfaceOption::LoadOption()
{
	if ( XGetConfig()->LoadXmlFile( XPath::GetConfigFileFullPath().c_str()) == false)
	{
#ifndef _PUBLISH
		if ( XGetConfig()->LoadXmlFile( XPath::GetConfigRelFileFullPath().c_str()) == false)
			return false;
#endif
	}


	ValidateOption();

	UpdateSystemOption();

	return true;
}


// SaveOption
void XGameInterfaceOption::SaveOption()
{
	if ( m_bModifyOption == true)		return;

	if ( XGetConfig()->SaveXmlFile( XPath::GetConfigFileFullPath().c_str()) == false)
	{
		mlog( "[ERROR] %s 파일을 저장할 수 없습니다.\n", XPath::GetConfigFileFullPath());
		assert( 0);
	}
}


// ValidateOption
bool XGameInterfaceOption::ValidateOption()
{
#ifndef _PUBLISH

//	return true;
#endif

#define _CHECK(_val,_min,_max)		((_val)<(_min) || (_val)>(_max))

	if ( XGETCFG_VIDEO_WIDTH != -1  &&  _CHECK( XGETCFG_VIDEO_WIDTH, 1024, 10000))
		XSETCFG_VIDEO_WIDTH( 1024);
	if ( XGETCFG_VIDEO_HEIGHT != -1  &&  _CHECK( XGETCFG_VIDEO_HEIGHT, 720, 10000))
		XSETCFG_VIDEO_HEIGHT( 720);
	if ( _CHECK( XGETCFG_VIDEO_LIGHTING, 0, 2))							XSETCFG_VIDEO_LIGHTING( 2);
	if ( _CHECK( XGETCFG_VIDEO_TEXTUREMIPMAPBIAS, -2.0f, 2.0f))			XSETCFG_VIDEO_TEXTUREMIPMAPBIAS( 0.0f);
	if ( _CHECK( XGETCFG_VIDEO_TEXTUREREDUCTION, 0, 4))					XSETCFG_VIDEO_TEXTUREREDUCTION( 0);
	if ( _CHECK( XGETCFG_VIDEO_GRNDOBJVISIBLERANGE, 1000, 9000))		XSETCFG_VIDEO_GRNDOBJVISIBLERANGE( 5000);
	if ( _CHECK( XGETCFG_VIDEO_SHADOW, 0, 5))							XSETCFG_VIDEO_SHADOW( 2);
	if ( _CHECK( XGETCFG_VIDEO_SSAO, 0, 3))								XSETCFG_VIDEO_SSAO( 0);
	if ( _CHECK( XGETCFG_VIDEO_MOVIEENCODINGRES, 0, 3))					XSETCFG_VIDEO_MOVIEENCODINGRES( 0);

	if ( _CHECK( XGETCFG_AUDIO_MASTERVOLUME, 0.0f, 1.0f))				XSETCFG_AUDIO_MASTERVOLUME( 1.0f);
	if ( _CHECK( XGETCFG_AUDIO_BGMVOLUME, 0.0f, 1.0f))					XSETCFG_AUDIO_BGMVOLUME( 1.0f);
	if ( _CHECK( XGETCFG_AUDIO_EFFECTVOLUME, 0.0f, 1.0f))				XSETCFG_AUDIO_EFFECTVOLUME( 1.0f);

	if ( _CHECK( XGETCFG_CONTROL_MOUSESENSITIVITY, 0, 100))				XSETCFG_CONTROL_MOUSESENSITIVITY( 50);

	return true;
}



void XGameInterfaceOption::GetScreenResolution( int* w, int* h)
{
	int _w = XGETCFG_VIDEO_WIDTH;
	int _h = XGETCFG_VIDEO_HEIGHT;
	if ( _w == -1  ||  _h == -1)
	{
		bool bFind = false;

		int scr_width = GetSystemMetrics( SM_CXSCREEN);
		int scr_height = GetSystemMetrics( SM_CYSCREEN);
		float fRatio = ( (float)scr_width / (float)scr_height) * 768.0f;
		int _w_min = (int)( fRatio - 10.0f);
		int _w_max = (int)( fRatio + 10.0f);

		vector< MSIZE> vDisplayResInfo;
		GetDisplayResInfo( vDisplayResInfo);

		for ( vector< MSIZE>::iterator itr = vDisplayResInfo.begin();  itr != vDisplayResInfo.end();  itr++)
		{
			if ( (*itr).h == 768  &&  (*itr).w > _w_min  &&  (*itr).w < _w_max)
			{
				bFind = true;

				_w = (*itr).w;
				_h = 768;
				break;
			}
		}

		if ( bFind == false)
		{
			_w = 1024;
			_h = 768;
		}
	}

	*w = _w;
	*h = _h;
}



// UpdateSystemOption
// 옵션에 따라서 게임 시스템을 재설정해야 하는 경우 여기서 설정한다
void XGameInterfaceOption::UpdateSystemOption()
{
	// Display
	if ( global.app  &&  global.camera)
	{
		int w, h;
		GetScreenResolution( &w, &h);

		if ( rs3::REngine::GetConfig().m_nWidth != w  ||  rs3::REngine::GetConfig().m_nHeight != h  ||  rs3::REngine::GetConfig().m_bFullScreen != XGETCFG_VIDEO_FULLSCREEN)
		{
			rs3::REngine::GetConfig().m_nWidth = w;
			rs3::REngine::GetConfig().m_nHeight = h;
			rs3::REngine::GetConfig().m_bFullScreen = XGETCFG_VIDEO_FULLSCREEN;
			global.camera->ResetFOV();
			global.app->ResetDevice();
		}
	}

	
	// Video
	if ( global.smgr != NULL)
		global.smgr->SetRenderTargetGrade( XGETCFG_VIDEO_LIGHTING);

	REngine::GetConfig().m_fGlobalTextureMipmapLodBias = XGETCFG_VIDEO_TEXTUREMIPMAPBIAS;

	if ( global.smgr != NULL  &&  global.smgr->GetCurrentTerrain())
	{
		int _range = XGETCFG_VIDEO_GRNDOBJVISIBLERANGE;
		global.smgr->GetCurrentTerrain()->SetGrassLodParams( (float)_range, (float)( _range + 2000));
		global.smgr->GetCurrentTerrain()->SetDetailDistance( (float)_range * 3.0f);
		RTreeSceneNode::SetLodFarDist( (float)_range * 3.0f);
	}
		
	if ( global.camera)
	{
		RSceneManager* pMng = (RSceneManager*)global.smgr;

		if ( pMng)
		{
			if ( pMng->IsEnableHDR() != XGETCFG_VIDEO_HDR) 
				pMng->SetHDREnable( XGETCFG_VIDEO_HDR);

			if ( pMng->IsEnableDOF() != XGETCFG_VIDEO_DOF)
			{
				pMng->SetDOFEnable( XGETCFG_VIDEO_DOF);

				if ( pMng->IsEnableDOF() == true)
				{
					pMng->SetDOFFocalDist( XCONST::DOF_FOCUS_DIST);
					pMng->SetDOFFocalRange( XCONST::DOF_FOCUS_RANGE);
					pMng->SetDOFCOCSize( XCONST::DOF_COC_SIZE);
				}
			}

			if( pMng->GetSSAO() != XGETCFG_VIDEO_SSAO)
				pMng->SetSSAO( XGETCFG_VIDEO_SSAO);

			SetShadow( XGETCFG_VIDEO_SHADOW);
		}
	}

	if ( global.smgr)
	{
		global.smgr->SetDistortionEnable( XGETCFG_VIDEO_DISTORTION);
		global.smgr->SetScreenMotionBlurEnable( XGETCFG_VIDEO_MOTIONBLUR);
		global.smgr->SetAccumMotionBlurEnable( XGETCFG_VIDEO_MOTIONBLUR);
		global.smgr->SetPixelMotionBlurEnable( XGETCFG_VIDEO_MOTIONBLUR);
		global.smgr->SetSoftParticleEnable( XGETCFG_VIDEO_SOFTPARTICLE);
	}


	// Sound
	if ( global.sound)
	{
		global.sound->SetMute( RSD_CHANNELGROUP_MASTER, XGETCFG_AUDIO_MUTE);
		global.sound->SetMute( RSD_CHANNELGROUP_GENERIC, XGETCFG_AUDIO_MUTEGENERICSOUND);
		global.sound->SetMute( RSD_CHANNELGROUP_BGM, XGETCFG_AUDIO_MUTEBGMSOUND);
		global.sound->SetMute( RSD_CHANNELGROUP_EFFECT, XGETCFG_AUDIO_MUTEEFFECTSOUND);
		global.sound->SetVolume( RSD_CHANNELGROUP_MASTER, XGETCFG_AUDIO_MASTERVOLUME);
		global.sound->SetVolume( RSD_CHANNELGROUP_GENERIC, XGETCFG_AUDIO_GENERICVOLUME);
		global.sound->SetVolume( RSD_CHANNELGROUP_BGM, XGETCFG_AUDIO_BGMVOLUME);
		global.sound->SetVolume( RSD_CHANNELGROUP_EFFECT, XGETCFG_AUDIO_EFFECTVOLUME);
		global.sound->SetHardwareAccelerate( XGETCFG_AUDIO_HARDWAREACC );
		global.sound->EnableReverb( XGETCFG_AUDIO_REVERB);
		global.sound->EnableCompressor( XGETCFG_AUDIO_COMPRESSOR);
 		global.sound->SilenceWhenInactivate( XGETCFG_AUDIO_INACTIVESOUND);
	}


	// Controls
	ApplyControlKeyMap();
}


// 해상도 정보
class XRESOLUTIONINFO : public DEVMODE
{
public:
	XRESOLUTIONINFO()
	{
		ZeroMemory( this, sizeof( XRESOLUTIONINFO));
		dmSize = sizeof( XRESOLUTIONINFO);
	}

	bool operator==(const DEVMODE& other) const
	{
		return ( dmBitsPerPel == other.dmBitsPerPel  &&
			dmDisplayFlags == other.dmDisplayFlags);
	}
};

// GetDisplayResInfo
bool XGameInterfaceOption::GetDisplayResInfo( vector< MSIZE>& info)
{
	info.clear();


	// 현재 해상도 정보 얻어오기
	XRESOLUTIONINFO cur;
	if ( EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &cur) == 0)
	{
		mlog( "[ERROR] 현재 디스플레이 정보를 얻어올 수 없습니다.\n");
		assert( 0);
		return false;
	}


	// 지원 가능한 해상도 얻어오기
	XRESOLUTIONINFO dm;
	int index = 0;
	while ( EnumDisplaySettings( NULL, index++, &dm) != 0)
	{
		if ( dm.dmPelsWidth < 1024  ||  dm.dmPelsHeight < 768)		continue;

		if ( cur == dm)
		{
			bool bPut = true;
			for each ( MSIZE res  in info)
			{
				if ( dm.dmPelsWidth == res.w  &&  dm.dmPelsHeight == res.h)
				{
					bPut = false;
					break;
				}
			}

			if ( bPut == true)		info.push_back( MSIZE( dm.dmPelsWidth, dm.dmPelsHeight));
		}
	}


	// Sorting (bubble sorting)
	if ( info.empty() == false)
	{
		size_t _size = info.size();
		for ( size_t i = 0;  i < _size;  i++)
		{
			for ( size_t j = i;  j < _size;  j++)
			{
				if ( (info[ i].w > info[ j].w)  ||  (info[ i].w == info[ j].w  &&  info[ i].h > info[ j].h))
				{
					MSIZE _tmp( info[ i]);
					info[ i].w = info[ j].w;
					info[ i].h = info[ j].h;
					info[ j].w = _tmp.w;
					info[ j].h = _tmp.h;
				}
			}
		}
	}


	return true;
}


// SetScreenResolution
void XGameInterfaceOption::SetScreenResolution( int w, int h)
{
	if ( global.app == NULL  ||  global.camera == NULL)		return;

	if ( _CHECK( w, 1024, 10000))			w = 1024;
	if ( _CHECK( h, 768, 10000))			h = 768;

	rs3::REngine::GetConfig().m_nWidth = w;
	rs3::REngine::GetConfig().m_nHeight = h;
	rs3::REngine::GetConfig().m_bFullScreen = XGETCFG_VIDEO_FULLSCREEN;
	global.camera->ResetFOV();
	global.app->ResetDevice();

	XSETCFG_VIDEO_WIDTH( w);
	XSETCFG_VIDEO_HEIGHT( h);
}


// SetLighting
void XGameInterfaceOption::SetLighting( int nLighting)
{
	if ( _CHECK( nLighting, 0, 2))		nLighting = 2;

	if ( global.smgr != NULL)
	{
		global.smgr->SetRenderTargetGrade( nLighting);

		XSETCFG_VIDEO_LIGHTING( global.smgr->GetRenderTargetGrade());
	}
}


// SetTextureMipmapBias
void XGameInterfaceOption::SetTextureMipmapBias( float fBias)
{
	if ( _CHECK( fBias, -2.0f, 2.0f))		fBias = 0.0f;

	rs3::REngine::GetConfig().m_fGlobalTextureMipmapLodBias = fBias;

	XSETCFG_VIDEO_TEXTUREMIPMAPBIAS( fBias);
}


// SetGrndObjVisibleRange
void XGameInterfaceOption::SetGrndObjVisibleRange( int nRange)
{
	if ( _CHECK( nRange, 1000, 9000))		nRange = 5000;

	if ( global.smgr != NULL  &&  global.smgr->GetCurrentTerrain())
	{
		global.smgr->GetCurrentTerrain()->SetGrassLodParams( (float)nRange, (float)( nRange + 2000));
		global.smgr->GetCurrentTerrain()->SetDetailDistance( (float)nRange * 3.0f);
		RTreeSceneNode::SetLodFarDist( (float)nRange * 3.0f);
	}

	XSETCFG_VIDEO_GRNDOBJVISIBLERANGE( nRange);
}


// EnableNormalMapping
void XGameInterfaceOption::EnableNormalMapping( bool bEnable)
{
	RSceneManager* pMng = (RSceneManager*)global.smgr;
	if ( pMng == NULL)			return;

	pMng->UseNormalMap( bEnable);

	XSETCFG_VIDEO_NORMALMAPPING( bEnable);
}


// SetFullScreen
void XGameInterfaceOption::SetFullScreen( bool bSet)
{
	/*
	int w, h;
	XGetGameInterface().OPTIONS.GetScreenResolution( &w, &h);

	rs3::REngine::GetConfig().m_nWidth = w;
	rs3::REngine::GetConfig().m_nHeight = h;
	rs3::REngine::GetConfig().m_bFullScreen = bSet;

	global.app->ResetDevice();

	XSETCFG_VIDEO_FULLSCREEN( bSet);
	*/
	
	// ToggleFullScreen()이라는 것이 존재한다 따로 할 필요 없...			_by tripleJ 110524
	// 설정하고자 하는 상태의 반대로 변수 셋팅 ToggleFullScreen()안에서 현재 값의 반대로 세팅하기 때문이다.
	REngine::GetConfig().m_bFullScreen		= !bSet;
	global.app->ToggleFullScreen();
}


// EnableHDR
void XGameInterfaceOption::EnableHDR( bool bEnable)
{
	RSceneManager* pMng = (RSceneManager*)global.smgr;
	if ( pMng == NULL)			return;

	pMng->SetHDREnable( bEnable);

	XSETCFG_VIDEO_HDR( pMng->IsEnableHDR());
}


// EnableDOF
void XGameInterfaceOption::EnableDOF( bool bEnable)
{
	RSceneManager* pMng = (RSceneManager*)global.smgr;
	if ( pMng == NULL)			return;

	pMng->SetDOFEnable( bEnable);
	if ( pMng->IsEnableDOF() == true)
	{
		pMng->SetDOFFocalDist( XCONST::DOF_FOCUS_DIST);
		pMng->SetDOFFocalRange( XCONST::DOF_FOCUS_RANGE);
		pMng->SetDOFCOCSize( XCONST::DOF_COC_SIZE);
	}

	XSETCFG_VIDEO_DOF( pMng->IsEnableDOF());
}

// SetSSAO
void XGameInterfaceOption::SetSSAO( int nSet)
{
	global.smgr->SetSSAO( nSet);

	XSETCFG_VIDEO_SSAO( global.smgr->GetSSAO());
}

// SetShadow
void XGameInterfaceOption::SetShadow( int nShadow, bool bSave)
{
	RShadowDesc desc;

	switch ( nShadow)
	{
	case 0 :
		desc.eMethod = SHADOW_NONE;
		global.smgr->SetRealtimeShadowMethod( desc);
		break;

	case 1 :
		desc.eMethod = SHADOW_PROJECTION;
		desc.fSplitDistance[0] = 2000.f;
		desc.fSplitDistance[1] = 2000.f;
		desc.fSplitDistance[2] = 2000.f;
		desc.fSplitDistance[3] = 2000.f;
		desc.nTextureNum = 1;
		desc.nTextureSize = 512;
		global.smgr->SetRealtimeShadowMethod( desc);
		break;

	case 2 :
		desc.eMethod = SHADOW_ESM;
		desc.fSplitDistance[0] = 2000.f;
		desc.fSplitDistance[1] = 2000.f;
		desc.fSplitDistance[2] = 2000.f;
		desc.fSplitDistance[3] = 2000.f;
		desc.nTextureNum = 1;
		desc.nTextureSize = 512;
		global.smgr->SetRealtimeShadowMethod( desc);
		break;

	case 3 :
		desc.eMethod = SHADOW_PSSM;
		desc.fSplitDistance[0] = 200.f;
		desc.fSplitDistance[1] = 700.f;
		desc.fSplitDistance[2] = 7000.f;
		desc.fSplitDistance[3] = 7000.f;
		desc.nTextureNum = 3;
		desc.nTextureSize = 512;
		global.smgr->SetRealtimeShadowMethod( desc);
		break;

	case 4 :
		desc.eMethod = SHADOW_PSSM;
		desc.fSplitDistance[0] = 200.f;
		desc.fSplitDistance[1] = 700.f;
		desc.fSplitDistance[2] = 7000.f;
		desc.fSplitDistance[3] = 7000.f;
		desc.nTextureNum = 3;
		desc.nTextureSize = 1024;
		global.smgr->SetRealtimeShadowMethod( desc);
		break;

	case 5 :
		desc.eMethod = SHADOW_PSSM;
		desc.fSplitDistance[0] = 150.f;
		desc.fSplitDistance[1] = 400.f;
		desc.fSplitDistance[2] = 1500.f;
		desc.fSplitDistance[3] = 9000.f;
		desc.nTextureNum = 4;
		desc.nTextureSize = 1024;
		global.smgr->SetRealtimeShadowMethod( desc);
		break;
	}


	if ( bSave == true)
		XSETCFG_VIDEO_SHADOW( nShadow);
}

void XGameInterfaceOption::SetDistortion( bool bSet)
{
	global.smgr->SetDistortionEnable( bSet);
	XSETCFG_VIDEO_DISTORTION( global.smgr->IsEnableDistortion());
}


void XGameInterfaceOption::SetMotionBlur( bool bSet)
{
	global.smgr->SetScreenMotionBlurEnable( bSet);
	global.smgr->SetAccumMotionBlurEnable( bSet);
	global.smgr->SetPixelMotionBlurEnable( bSet);
	XSETCFG_VIDEO_MOTIONBLUR( bSet);
}

void XGameInterfaceOption::SetSoftParticle( bool bSet)
{
	global.smgr->SetSoftParticleEnable( bSet);
	XSETCFG_VIDEO_SOFTPARTICLE( bSet);
}

// MuteAll
void XGameInterfaceOption::MuteAll( bool bMute)
{
	global.sound->SetMute( RSD_CHANNELGROUP_MASTER, bMute);
	XSETCFG_AUDIO_MUTE( bMute);
}


// MuteGenericSound
void XGameInterfaceOption::MuteGenericSound( bool bMute)
{
	global.sound->SetMute( RSD_CHANNELGROUP_GENERIC, bMute);
	XSETCFG_AUDIO_MUTEGENERICSOUND( bMute);
}


// MuteBgmSound
void XGameInterfaceOption::MuteBgmSound( bool bMute)
{
	global.sound->SetMute( RSD_CHANNELGROUP_BGM, bMute);
	XSETCFG_AUDIO_MUTEBGMSOUND( bMute);
}


// MuteEffectSound
void XGameInterfaceOption::MuteEffectSound( bool bMute)
{
	global.sound->SetMute( RSD_CHANNELGROUP_EFFECT, bMute);
	XSETCFG_AUDIO_MUTEEFFECTSOUND( bMute);
}


// SetMasterVolume
void XGameInterfaceOption::SetMasterVolume( float fVolume)
{
	if ( _CHECK( fVolume, 0.0f, 1.0f))				fVolume = 1.0f;

	global.sound->SetVolume( RSD_CHANNELGROUP_MASTER, fVolume);

	XSETCFG_AUDIO_MASTERVOLUME( fVolume);
}


// SetGenericVolume
void XGameInterfaceOption::SetGenericVolume( float fVolume)
{
	if ( _CHECK( fVolume, 0.0f, 1.0f))				fVolume = 1.0f;

	global.sound->SetVolume( RSD_CHANNELGROUP_GENERIC, fVolume);

	XSETCFG_AUDIO_GENERICVOLUME( fVolume);
}


// SetBgmVolume
void XGameInterfaceOption::SetBgmVolume( float fVolume)
{
	if ( _CHECK( fVolume, 0.0f, 1.0f))				fVolume = 1.0f;

	global.sound->SetVolume( RSD_CHANNELGROUP_BGM, fVolume);

	XSETCFG_AUDIO_BGMVOLUME( fVolume);
}


// SetEffectVolume
void XGameInterfaceOption::SetEffectVolume( float fVolume)
{
	if ( _CHECK( fVolume, 0.0f, 1.0f))				fVolume = 1.0f;

	global.sound->SetVolume( RSD_CHANNELGROUP_EFFECT, fVolume);
	XSETCFG_AUDIO_EFFECTVOLUME( fVolume);
}


void XGameInterfaceOption::EnableHardwareAccelerate( bool bEnable)
{
	global.sound->SetHardwareAccelerate( bEnable);
	XSETCFG_AUDIO_HARDWAREACC( global.sound->IsHardwareAccelerate());
}


void XGameInterfaceOption::EnableReverb( bool bEnable)
{
	global.sound->EnableReverb( bEnable);
	XSETCFG_AUDIO_REVERB( global.sound->IsEnabledReverb());
}


void XGameInterfaceOption::EnableCompressor( bool bEnable)
{
	global.sound->EnableCompressor( bEnable);
	XSETCFG_AUDIO_COMPRESSOR( global.sound->IsEnabledCompressor());
}


void XGameInterfaceOption::SetSpeakerMode( int nSpeakerMode)
{
	if ( _CHECK( nSpeakerMode, 0, 4))				nSpeakerMode = 1;

	global.sound->SetSpeakerMode( nSpeakerMode);
	XSETCFG_AUDIO_SPEAKERMODE( global.sound->GetSpeakerMode());
}


void XGameInterfaceOption::MuteInActiveSound( bool bMute)
{
	global.sound->SilenceWhenInactivate( bMute);
	XSETCFG_AUDIO_INACTIVESOUND( bMute);
}

void XGameInterfaceOption::SetSystemMessage(int nMode)
{
	XSETCFG_AUDIO_SYSTEMMESSAGE( nMode );
}


// SetDefaultSetting
void XGameInterfaceOption::SetDefaultSetting()
{
	if ( XGETCFG_GAME_COMBATSTANCESITTING != false)			XSETCFG_GAME_COMBATSTANCESITTING(false);
	if ( XGETCFG_GAME_AUTOCHANGE_TONORMALSTANCE != false)	XSETCFG_GAME_AUTOCHANGE_TONORMALSTANCE(false);
	if ( XGETCFG_GAME_AUTONORMALATTACK != true)			XSETCFG_GAME_AUTONORMALATTACK( true);
	if ( XGETCFG_GAME_SHOWBUFFNAME != true)				XSETCFG_GAME_SHOWBUFFNAME( true);
	if ( XGETCFG_GAME_SHOWHELPMESSAGE != true)			XSETCFG_GAME_SHOWHELPMESSAGE( true);
	if ( XGETCFG_GAME_SHOWCHATMSG != true)				XSETCFG_GAME_SHOWCHATMSG( true);
	if ( XGETCFG_GAME_SHOWWHISPERMSG != true)			XSETCFG_GAME_SHOWWHISPERMSG( true);
	if ( XGETCFG_GAME_SHOWPARTYMSG != true)				XSETCFG_GAME_SHOWPARTYMSG( true);
	if ( XGETCFG_GAME_SHOWGUILDMSG != true)				XSETCFG_GAME_SHOWGUILDMSG( true);
	if ( XGETCFG_GAME_SHOWCHATBUBBLE != true)			XSETCFG_GAME_SHOWCHATBUBBLE( true);
	if ( XGETCFG_GAME_USE_AUTOPARTY!= true)				XSETCFG_GAME_USE_AUTOPARTY( true);
	if ( XGETCFG_GAME_REJECT_PARTY!= false)				XSETCFG_GAME_REJECT_PARTY( false);
	if ( XGETCFG_GAME_REJECT_GUILD!= false)				XSETCFG_GAME_REJECT_GUILD( false);
	if ( XGETCFG_GAME_REJECT_DUEL!= false)				XSETCFG_GAME_REJECT_DUEL( false);
	if ( XGETCFG_GAME_REJECT_TRADE != false)			XSETCFG_GAME_REJECT_TRADE( false);
	if ( XGETCFG_GAME_PLAYERNAME != true)				XSETCFG_GAME_PLAYERNAME( true);
	if ( XGETCFG_GAME_PLAYERGUILD != true)				XSETCFG_GAME_PLAYERGUILD( true);
	if ( XGETCFG_GAME_MYPLAYERNAME != false)			XSETCFG_GAME_MYPLAYERNAME( false);
	if ( XGETCFG_GAME_MYPLAYERGUILD != false)			XSETCFG_GAME_MYPLAYERGUILD( false);
	if ( XGETCFG_GAME_NPCNAME != true)					XSETCFG_GAME_NPCNAME( true);
	if ( XGETCFG_GAME_NPCGUILD != true)					XSETCFG_GAME_NPCGUILD( true);

	if ( XGETCFG_GAME_INDICATOR != true)				XSETCFG_GAME_INDICATOR( true);
	if ( XGETCFG_GAME_MINIHPBAR != true)				XSETCFG_GAME_MINIHPBAR( true);
	if ( XGETCFG_GAME_CAPTION != true)					XSETCFG_GAME_CAPTION( true);
	if ( XGETCFG_GAME_RECIPE_REMOVE != false)			XSETCFG_GAME_RECIPE_REMOVE( false);

	if ( XGETCFG_VIDEO_TEXTUREREDUCTION != 0)			XSETCFG_VIDEO_TEXTUREREDUCTION( 0);
	if ( XGETCFG_VIDEO_LIGHTING != 2)					XSETCFG_VIDEO_LIGHTING( 2);
	if ( XGETCFG_VIDEO_TEXTUREMIPMAPBIAS != 0.0f)		XSETCFG_VIDEO_TEXTUREMIPMAPBIAS( 0.0f);
	if ( XGETCFG_VIDEO_BRIGHTNESS != 50)				XSETCFG_VIDEO_BRIGHTNESS( 50);
	if ( XGETCFG_VIDEO_VISIBILITYRANGE != 50)			XSETCFG_VIDEO_VISIBILITYRANGE( 50);
	if ( XGETCFG_VIDEO_GRNDOBJVISIBLERANGE != 7000)		XSETCFG_VIDEO_GRNDOBJVISIBLERANGE( 7000);
	if ( XGETCFG_VIDEO_NORMALMAPPING != true)			XSETCFG_VIDEO_NORMALMAPPING( true);
	if ( XGETCFG_VIDEO_ANTIALIASING != true)			XSETCFG_VIDEO_ANTIALIASING( true);
	if ( XGETCFG_VIDEO_HDR != true)						XSETCFG_VIDEO_HDR( true);
	if ( XGETCFG_VIDEO_DOF != true)						XSETCFG_VIDEO_DOF( true);
	if ( XGETCFG_VIDEO_SHADOW != 2)						XSETCFG_VIDEO_SHADOW( 2);
	if ( XGETCFG_VIDEO_SSAO != 0)						XSETCFG_VIDEO_SSAO( 0);
	if ( XGETCFG_VIDEO_DISTORTION != true)				XSETCFG_VIDEO_DISTORTION( true);
	if ( XGETCFG_VIDEO_MOTIONBLUR != true)				XSETCFG_VIDEO_MOTIONBLUR( true);
	if ( XGETCFG_VIDEO_SOFTPARTICLE != true )			XSETCFG_VIDEO_SOFTPARTICLE( true);
	if ( XGETCFG_VIDEO_WEATHEREFFECT != true)			XSETCFG_VIDEO_WEATHEREFFECT( true);
	if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE != string( "MJPEG"))	XSETCFG_VIDEO_MOVIEENCODINGTYPE( string( "MJPEG"));
	if ( XGETCFG_VIDEO_MOVIEENCODINGRES != 0)			XSETCFG_VIDEO_MOVIEENCODINGRES( 0);
	if ( XGETCFG_VIDEO_SHOWUIWHENREC != true)			XSETCFG_VIDEO_SHOWUIWHENREC( true);
	if ( XGETCFG_VIDEO_SCREENSHOTFORMAT != string("JPEG(*.jpg)"))		XSETCFG_VIDEO_SCREENSHOTFORMAT( string("JPEG(*.jpg)"));

	if ( XGETCFG_AUDIO_MUTE != false)					XSETCFG_AUDIO_MUTE( false);
	if ( XGETCFG_AUDIO_MUTEGENERICSOUND != false)		XSETCFG_AUDIO_MUTEGENERICSOUND( false);
	if ( XGETCFG_AUDIO_MUTEBGMSOUND != false)			XSETCFG_AUDIO_MUTEBGMSOUND( false);
	if ( XGETCFG_AUDIO_MUTEEFFECTSOUND != false)		XSETCFG_AUDIO_MUTEEFFECTSOUND( false);
	if ( XGETCFG_AUDIO_MASTERVOLUME != 1.0f)			XSETCFG_AUDIO_MASTERVOLUME( 1.0f);
	if ( XGETCFG_AUDIO_GENERICVOLUME != 1.0f)			XSETCFG_AUDIO_GENERICVOLUME( 1.0f);
	if ( XGETCFG_AUDIO_BGMVOLUME != 1.0f)				XSETCFG_AUDIO_BGMVOLUME( 1.0f);
	if ( XGETCFG_AUDIO_EFFECTVOLUME != 1.0f)			XSETCFG_AUDIO_EFFECTVOLUME( 1.0f);
	if ( XGETCFG_AUDIO_HARDWAREACC != true)				XSETCFG_AUDIO_HARDWAREACC( true);
	if ( XGETCFG_AUDIO_REVERB != true)					XSETCFG_AUDIO_REVERB( true);
	if ( XGETCFG_AUDIO_COMPRESSOR != true)				XSETCFG_AUDIO_COMPRESSOR( true);
	if ( XGETCFG_AUDIO_SPEAKERMODE != 1)				XSETCFG_AUDIO_SPEAKERMODE( 1);
	if ( XGETCFG_AUDIO_INACTIVESOUND != true)			XSETCFG_AUDIO_INACTIVESOUND( true);
	
	if ( XGETCFG_CONTROL_INVERTEDMOUSE != false)		XSETCFG_CONTROL_INVERTEDMOUSE( false);
	if ( XGETCFG_CONTROL_MOUSESENSITIVITY != 50)		XSETCFG_CONTROL_MOUSESENSITIVITY( 50);


	if ( XGETCFG_GAME_SHOWCRAFTICON != true)			XSETCFG_GAME_SHOWCRAFTICONE( true);
	if ( XGETCFG_GAME_SHOWSHOPICON != true)				XSETCFG_GAME_SHOWSHOPICON( true);
	if ( XGETCFG_GAME_SHOWQUESTICON != true)			XSETCFG_GAME_SHOWQUESTICON( true);
	if ( XGETCFG_GAME_SHOWAREAICON != true)				XSETCFG_GAME_SHOWAREAICON( true);

	if ( XGETCFG_GAME_CROSSHAIR != string ( "Default"))	XSETCFG_GAME_CROSSHAIR( string( "Default"));


	SetDefaultControlKey();

	UpdateSystemOption();
}


// SetDefaultControlKey
void XGameInterfaceOption::SetDefaultControlKey()
{
	XSETCFG_KEY_FORWARD(		"KEY_W",			string( ""));
	XSETCFG_KEY_BACKWARD(		"KEY_S",			string( ""));
	XSETCFG_KEY_LEFT(			"KEY_A",			string( ""));
	XSETCFG_KEY_RIGHT(			"KEY_D",			string( ""));
	XSETCFG_KEY_ACTION(			"KEY_LBUTTON",		string( ""));
	XSETCFG_KEY_ALTACTION(		"KEY_RBUTTON",		string( ""));
	XSETCFG_KEY_JUMP(			"KEY_SPACE",		string( ""));
	XSETCFG_KEY_CHANGESTANCE(	"KEY_F",			string( ""));
	XSETCFG_KEY_CHANGEWEAPON(	"KEY_TAB",			string( ""));
	XSETCFG_KEY_SIT(			"KEY_Z",			string( ""));
	XSETCFG_KEY_AUTORUN(		"KEY_R",			string( ""));
	XSETCFG_KEY_NPCINTERACTION(	"KEY_E",			string( ""));
	XSETCFG_KEY_PCINTERACTION(	"KEY_G",			string( ""));
	XSETCFG_KEY_LOCKTARGET(		"KEY_Y",			string( ""));
	XSETCFG_KEY_MAINMENU(		"KEY_ESCAPE",		string( ""));
	XSETCFG_KEY_CHARACTER(		"KEY_H",			string( ""));
	XSETCFG_KEY_INVENTORY(		"KEY_I",			string( ""));
	XSETCFG_KEY_TALENT(			"KEY_T",			string( ""));
	XSETCFG_KEY_JOURNAL(		"KEY_J",			string( ""));
	XSETCFG_KEY_SOCIAL(			"KEY_P",			string( ""));
	XSETCFG_KEY_MAP(			"KEY_M",			string( ""));
	XSETCFG_KEY_CONTROLSHELP1(	"KEY_F1",			string( ""));
	XSETCFG_KEY_CONTROLSHELP2(	"KEY_F2",			string( ""));
	XSETCFG_KEY_TOGGLEMOUSE(	"KEY_CONTROL",		string( ""));
	XSETCFG_KEY_TOGGLE_FREECAMERA("KEY_MBUTTON",	string( ""));
	XSETCFG_KEY_GUARD(			"KEY_Q",			string( ""));
	XSETCFG_KEY_DODGE(			"KEY_SHIFT",		string( ""));
	XSETCFG_KEY_PALETTE0(		"KEY_1",			string( ""));
	XSETCFG_KEY_PALETTE1(		"KEY_2",			string( ""));
	XSETCFG_KEY_PALETTE2(		"KEY_3",			string( ""));
	XSETCFG_KEY_PALETTE3(		"KEY_4",			string( ""));
	XSETCFG_KEY_PALETTE4(		"KEY_5",			string( ""));
	XSETCFG_KEY_PALETTE5(		"KEY_6",			string( ""));
	XSETCFG_KEY_PALETTE6(		"KEY_7",			string( ""));
	XSETCFG_KEY_PALETTE7(		"KEY_8",			string( ""));
	XSETCFG_KEY_PALETTE8(		"KEY_9",			string( ""));
	XSETCFG_KEY_PALETTE9(		"KEY_0",			string( ""));
	XSETCFG_KEY_PALETTESET0(	"KEY_1",			string( "shift"));
	XSETCFG_KEY_PALETTESET1(	"KEY_2",			string( "shift"));
	XSETCFG_KEY_PALETTESET2(	"KEY_3",			string( "shift"));
	XSETCFG_KEY_PALETTESET3(	"KEY_4",			string( "shift"));
	XSETCFG_KEY_PALETTESET4(	"KEY_5",			string( "shift"));

	ApplyControlKeyMap();
}


// ApplyControlKeyMap
void XGameInterfaceOption::ApplyControlKeyMap()
{
	if ( gg.controller == NULL)
		return;

	string strCode, strExt;
	XGETCFG_KEY_FORWARD( strCode, strExt);			gg.controller->SetVirtualKey( "FORWARD", strCode, strExt);
	XGETCFG_KEY_BACKWARD( strCode, strExt);			gg.controller->SetVirtualKey( "BACKWARD", strCode, strExt);
	XGETCFG_KEY_LEFT( strCode, strExt);				gg.controller->SetVirtualKey( "LEFT", strCode, strExt);
	XGETCFG_KEY_RIGHT( strCode, strExt);			gg.controller->SetVirtualKey( "RIGHT", strCode, strExt);
	XGETCFG_KEY_ACTION( strCode, strExt);			gg.controller->SetVirtualKey( "ACTION", strCode, strExt);
	XGETCFG_KEY_ALTACTION( strCode, strExt);		gg.controller->SetVirtualKey( "ALTACTION", strCode, strExt);
	XGETCFG_KEY_JUMP( strCode, strExt);				gg.controller->SetVirtualKey( "JUMP", strCode, strExt);
	XGETCFG_KEY_CHANGESTANCE( strCode, strExt);		gg.controller->SetVirtualKey( "CHANGESTANCE", strCode, strExt);
	XGETCFG_KEY_CHANGEWEAPON( strCode, strExt);		gg.controller->SetVirtualKey( "CHANGEWEAPON", strCode, strExt);
	XGETCFG_KEY_SIT( strCode, strExt);				gg.controller->SetVirtualKey( "SIT", strCode, strExt);
	XGETCFG_KEY_AUTORUN( strCode, strExt);			gg.controller->SetVirtualKey( "AUTORUN", strCode, strExt);
	XGETCFG_KEY_NPCINTERACTION( strCode, strExt);	gg.controller->SetVirtualKey( "NPCINTERACTION", strCode, strExt);
	XGETCFG_KEY_PCINTERACTION( strCode, strExt);	gg.controller->SetVirtualKey( "PCINTERACTION", strCode, strExt);
	XGETCFG_KEY_LOCKTARGET( strCode, strExt);		gg.controller->SetVirtualKey( "LOCKTARGET", strCode, strExt);
	XGETCFG_KEY_MAINMENU( strCode, strExt);			gg.controller->SetVirtualKey( "MAINMENU", strCode, strExt);
	XGETCFG_KEY_CHARACTER( strCode, strExt);		gg.controller->SetVirtualKey( "CHARACTER", strCode, strExt);
	XGETCFG_KEY_INVENTORY( strCode, strExt);		gg.controller->SetVirtualKey( "INVENTORY", strCode, strExt);
	XGETCFG_KEY_TALENT( strCode, strExt);			gg.controller->SetVirtualKey( "TALENT", strCode, strExt);
	XGETCFG_KEY_JOURNAL( strCode, strExt);			gg.controller->SetVirtualKey( "JOURNAL", strCode, strExt);
	XGETCFG_KEY_SOCIAL( strCode, strExt);			gg.controller->SetVirtualKey( "SOCIAL", strCode, strExt);
	XGETCFG_KEY_MAP( strCode, strExt);				gg.controller->SetVirtualKey( "MAP", strCode, strExt);
	XGETCFG_KEY_CONTROLSHELP1( strCode, strExt);	gg.controller->SetVirtualKey( "CONTROLSHELP1", strCode, strExt);
	XGETCFG_KEY_CONTROLSHELP2( strCode, strExt);	gg.controller->SetVirtualKey( "CONTROLSHELP2", strCode, strExt);
	XGETCFG_KEY_TOGGLEMOUSE( strCode, strExt);		gg.controller->SetVirtualKey( "TOGGLEMOUSE", strCode, strExt);
	XGETCFG_KEY_TOGGLE_FREECAMERA( strCode, strExt);gg.controller->SetVirtualKey( "TOGGLE_FREECAMERA", strCode, strExt);
	XGETCFG_KEY_GUARD( strCode, strExt);			gg.controller->SetVirtualKey( "GUARD", strCode, strExt);
	XGETCFG_KEY_DODGE( strCode, strExt);			gg.controller->SetVirtualKey( "DODGE", strCode, strExt);
	XGETCFG_KEY_PALETTE0( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE0", strCode, strExt);
	XGETCFG_KEY_PALETTE1( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE1", strCode, strExt);
	XGETCFG_KEY_PALETTE2( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE2", strCode, strExt);
	XGETCFG_KEY_PALETTE3( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE3", strCode, strExt);
	XGETCFG_KEY_PALETTE4( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE4", strCode, strExt);
	XGETCFG_KEY_PALETTE5( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE5", strCode, strExt);
	XGETCFG_KEY_PALETTE6( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE6", strCode, strExt);
	XGETCFG_KEY_PALETTE7( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE7", strCode, strExt);
	XGETCFG_KEY_PALETTE8( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE8", strCode, strExt);
	XGETCFG_KEY_PALETTE9( strCode, strExt);			gg.controller->SetVirtualKey( "PALETTE9", strCode, strExt);
	XGETCFG_KEY_PALETTESET0( strCode, strExt);		gg.controller->SetVirtualKey( "PALETTESET0", strCode, strExt);
	XGETCFG_KEY_PALETTESET1( strCode, strExt);		gg.controller->SetVirtualKey( "PALETTESET1", strCode, strExt);
	XGETCFG_KEY_PALETTESET2( strCode, strExt);		gg.controller->SetVirtualKey( "PALETTESET2", strCode, strExt);
	XGETCFG_KEY_PALETTESET3( strCode, strExt);		gg.controller->SetVirtualKey( "PALETTESET3", strCode, strExt);
	XGETCFG_KEY_PALETTESET4( strCode, strExt);		gg.controller->SetVirtualKey( "PALETTESET4", strCode, strExt);
}


// ChangeControlKey
void XGameInterfaceOption::ChangeControlKey( const char* szName, int nCode, bool bShift, bool bCtrl, bool bAlt)
{
	if ( szName == NULL  ||  (nCode <= 0  || nCode >= KEY_KEY_CODES_COUNT))		return;

	string strCode = gg.controller->GetCodeString( nCode).c_str();
	if ( strCode.empty())		return;

	string strExt;
	if ( bShift == true)		strExt = "shift";
	else if ( bCtrl == true)	strExt = "ctrl";
	else if ( bAlt == true)		strExt = "alt";


	// 기존에 이미 해당 코드를 사용하고 있는 가상 키를 삭제한다
	XVirtualKey nVKey = gg.controller->FindVirtualKey( nCode, bShift, bCtrl, bAlt);
	if ( nVKey > VKEY_NA  &&  nVKey < VKEY_COUNT)
	{
		gg.controller->ClearVirtualKey( nVKey);

		SetControlKeyConfig( gg.controller->GetVirtualKeyName( nVKey).c_str(), string(""), string(""));
	}

	SetControlKeyConfig( szName, strCode, strExt);

	ApplyControlKeyMap();
}


// ChangeControlKey
void XGameInterfaceOption::ChangeControlKey( const char* szName, int nCode)
{
	ChangeControlKey( szName, nCode, false, false, false);
}


// SetControlKeyConfig
void XGameInterfaceOption::SetControlKeyConfig( const string& strName, const string& strCode, const string& strExt)
{
	if ( strName == "FORWARD")				XSETCFG_KEY_FORWARD( strCode, strExt);
	else if ( strName == "BACKWARD")		XSETCFG_KEY_BACKWARD( strCode, strExt);
	else if ( strName == "LEFT")			XSETCFG_KEY_LEFT( strCode, strExt);
	else if ( strName == "RIGHT")			XSETCFG_KEY_RIGHT( strCode, strExt);
	else if ( strName == "ACTION")			XSETCFG_KEY_ACTION( strCode, strExt);
	else if ( strName == "ALTACTION")		XSETCFG_KEY_ALTACTION( strCode, strExt);
	else if ( strName == "JUMP")			XSETCFG_KEY_JUMP( strCode, strExt);
	else if ( strName == "CHANGESTANCE")	XSETCFG_KEY_CHANGESTANCE( strCode, strExt);
	else if ( strName == "CHANGEWEAPON")	XSETCFG_KEY_CHANGEWEAPON( strCode, strExt);
	else if ( strName == "SIT")				XSETCFG_KEY_SIT( strCode, strExt);
	else if ( strName == "AUTORUN")			XSETCFG_KEY_AUTORUN( strCode, strExt);
	else if ( strName == "NPCINTERACTION")	XSETCFG_KEY_NPCINTERACTION( strCode, strExt);
	else if ( strName == "PCINTERACTION")	XSETCFG_KEY_PCINTERACTION( strCode, strExt);
	else if ( strName == "LOCKTARGET")		XSETCFG_KEY_LOCKTARGET( strCode, strExt);
	else if ( strName == "MAINMENU")		XSETCFG_KEY_MAINMENU( strCode, strExt);
	else if ( strName == "CHARACTER")		XSETCFG_KEY_CHARACTER( strCode, strExt);
	else if ( strName == "INVENTORY")		XSETCFG_KEY_INVENTORY( strCode, strExt);
	else if ( strName == "TALENT")			XSETCFG_KEY_TALENT( strCode, strExt);
	else if ( strName == "JOURNAL")			XSETCFG_KEY_JOURNAL( strCode, strExt);
	else if ( strName == "SOCIAL")			XSETCFG_KEY_SOCIAL( strCode, strExt);
	else if ( strName == "MAP")				XSETCFG_KEY_MAP( strCode, strExt);
	else if ( strName == "CONTROLSHELP1")	XSETCFG_KEY_CONTROLSHELP1( strCode, strExt);
	else if ( strName == "CONTROLSHELP2")	XSETCFG_KEY_CONTROLSHELP2( strCode, strExt);
	else if ( strName == "TOGGLEMOUSE")		XSETCFG_KEY_TOGGLEMOUSE( strCode, strExt);
	else if	( strName == "TOGGLE_FREECAMERA") XSETCFG_KEY_TOGGLE_FREECAMERA( strCode, strExt);
	else if	( strName == "GUARD")			XSETCFG_KEY_GUARD( strCode, strExt);
	else if	( strName == "DODGE")			XSETCFG_KEY_DODGE( strCode, strExt);
	else if ( strName == "PALETTE0")		XSETCFG_KEY_PALETTE0( strCode, strExt);
	else if ( strName == "PALETTE1")		XSETCFG_KEY_PALETTE1( strCode, strExt);
	else if ( strName == "PALETTE2")		XSETCFG_KEY_PALETTE2( strCode, strExt);
	else if ( strName == "PALETTE3")		XSETCFG_KEY_PALETTE3( strCode, strExt);
	else if ( strName == "PALETTE4")		XSETCFG_KEY_PALETTE4( strCode, strExt);
	else if ( strName == "PALETTE5")		XSETCFG_KEY_PALETTE5( strCode, strExt);
	else if ( strName == "PALETTE6")		XSETCFG_KEY_PALETTE6( strCode, strExt);
	else if ( strName == "PALETTE7")		XSETCFG_KEY_PALETTE7( strCode, strExt);
	else if ( strName == "PALETTE8")		XSETCFG_KEY_PALETTE8( strCode, strExt);
	else if ( strName == "PALETTE9")		XSETCFG_KEY_PALETTE9( strCode, strExt);
	else if ( strName == "PALETTESET0")		XSETCFG_KEY_PALETTESET0( strCode, strExt);
	else if ( strName == "PALETTESET1")		XSETCFG_KEY_PALETTESET1( strCode, strExt);
	else if ( strName == "PALETTESET2")		XSETCFG_KEY_PALETTESET2( strCode, strExt);
	else if ( strName == "PALETTESET3")		XSETCFG_KEY_PALETTESET3( strCode, strExt);
	else if ( strName == "PALETTESET4")		XSETCFG_KEY_PALETTESET4( strCode, strExt);
}

void XGameInterfaceOption::SetTextureReduction( int nTextureReduction )
{
	if ( _CHECK( nTextureReduction, 0, 4))		nTextureReduction = 0;

	XSETCFG_VIDEO_TEXTUREREDUCTION( nTextureReduction);
}


/*
void XGameInterfaceOption::RefreshOptionVideoUI()
{
vector<XRESOLUTIONINFO> display;
GetDisplayResInfo( display);


MComboBox* pCombo;
pCombo = (MComboBox*)global.mint->FindWidget( WIDGET_NAME_OPTION_COMBO_SCREEN_RESOLUTION);
if ( pCombo)
{
pCombo->ResetContent();
pCombo->SetCurSel( -1);

for each ( XRESOLUTIONINFO res  in display)
{
char szBuff[ 32];
sprintf_s( szBuff, "%d x %d", (int)res.dmPelsWidth, (int)res.dmPelsHeight);
pCombo->AddString( szBuff);

if ( XGETCFG_VIDEO_WIDTH == (int)res.dmPelsWidth  ||  XGETCFG_VIDEO_HEIGHT == (int)res.dmPelsHeight)
pCombo->SetCurSel( pCombo->GetCount() - 1);
}
}

pCombo = (MComboBox*)global.mint->FindWidget( WIDGET_NAME_OPTION_COMBO_MOVIE_ENCODING_TYPE);
if ( pCombo)
{
pCombo->ResetContent();
pCombo->AddString( "MP4V");
pCombo->AddString( "MJPG");

if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "MP4V")			pCombo->SetCurSel( 0);
else if ( XGETCFG_VIDEO_MOVIEENCODINGTYPE == "MJPG")	pCombo->SetCurSel( 1);
else													pCombo->SetCurSel( 0);
}

pCombo = (MComboBox*)global.mint->FindWidget( WIDGET_NAME_OPTION_COMBO_MOVIE_ENCODING_RES);
if ( pCombo)
{
pCombo->ResetContent();
pCombo->AddString( "원본 크기");
pCombo->AddString( "75% 크기");
pCombo->AddString( "50% 크기");
pCombo->AddString( "25% 크기");
}

_SET_SLIDER( "sld_TextureMipmapBias",		(int)( XGETCFG_VIDEO_TEXTUREMIPMAPBIAS * -25.0f + 50.0f));
_SET_SLIDER( "sld_TextureReduction",		(int)( XGETCFG_VIDEO_TEXTUREREDUCTION * 20.0f));
_SET_SLIDER( "sld_VisibilityRange",			XGETCFG_VIDEO_VISIBILITYRANGE);
_SET_SLIDER( "sld_Brightness",				XGETCFG_VIDEO_BRIGHTNESS);
_SET_CHECKBOX( "btn_Antialiasing",			XGETCFG_VIDEO_ANTIALIASING);
_SET_CHECKBOX( "btn_FullScreen",			XGETCFG_VIDEO_FULLSCREEN);
_SET_CHECKBOX( "btn_EnableHDR",				XGETCFG_VIDEO_HDR);
_SET_CHECKBOX( "btn_EnableDOF",				XGETCFG_VIDEO_DOF);
_SET_CHECKBOX( "btn_ShowShadow",			XGETCFG_VIDEO_SHADOW);
_SET_CHECKBOX( "btn_ShowWeatherEffect",		XGETCFG_VIDEO_WEATHEREFFECT);
_SET_SLIDER( "sld_GrndObjVisibleRange",		((XGETCFG_VIDEO_GRNDOBJVISIBLERANGE - 2000) / 80));
_SET_COMBOBOX( "cmb_MovieEncodingRes",		XGETCFG_VIDEO_MOVIEENCODINGRES);
_SET_CHECKBOX( "btn_ShowUIWhenRecoding",	XGETCFG_VIDEO_SHOWUIWHENREC);
}

void XGameInterfaceOption::RefreshOptionSoundUI()
{
_SET_CHECKBOX( "btn_Mute",						XGETCFG_AUDIO_MUTE);
_SET_CHECKBOX( "btn_MuteBgmSound",				XGETCFG_AUDIO_MUTEBGMSOUND);
_SET_CHECKBOX( "btn_MuteEffectSound",			XGETCFG_AUDIO_MUTEEFFECTSOUND);
_SET_ENABLE( "btn_MuteBgmSound",				!XGETCFG_AUDIO_MUTE);
_SET_ENABLE( "btn_MuteEffectSound",				!XGETCFG_AUDIO_MUTE);
_SET_SLIDER( "sld_MasterVolume",				(int)( XGETCFG_AUDIO_MASTERVOLUME * 100.0f));
_SET_SLIDER( "sld_BgmVolume",					(int)( XGETCFG_AUDIO_BGMVOLUME * 100.0f));
_SET_SLIDER( "sld_EffectVolume",				(int)( XGETCFG_AUDIO_EFFECTVOLUME * 100.0f));
_SET_CHECKBOX( "btn_EnableBattleSound",			XGETCFG_AUDIO_BATTLESOUND);
_SET_CHECKBOX( "btn_EnableEnvironmentSound",	XGETCFG_AUDIO_ENVSOUND);
_SET_CHECKBOX( "btn_EnableAlertSound",			XGETCFG_AUDIO_ALERTSOUND);
}

void XGameInterfaceOption::RefreshOptionControlKeyUI()
{
MComboBox* pCombo = (MComboBox*)global.mint->FindWidget( WIDGET_NAME_OPTION_COMBO_PALLETE);
if ( pCombo == NULL)		return;

pCombo->ResetContent();
pCombo->AddString( "숫자 키");
pCombo->AddString( "'F1~F10' 키");

wstring strCode, strExt;
XGETCFG_KEY_PALETTE0( strCode, strExt);
if ( strCode == "KEY_F1")		pCombo->SetCurSel( 1);
else							pCombo->SetCurSel( 0);


pCombo = (MComboBox*)global.mint->FindWidget( WIDGET_NAME_OPTION_COMBO_PALLETE_SET);
pCombo->ResetContent();
pCombo->AddString( "Shift + '1~5' 키");
pCombo->AddString( "Ctrl + '1~5' 키");
pCombo->AddString( "Alt + '1~5' 키");
pCombo->AddString( "Shift + 'F1~F5' 키");
pCombo->AddString( "Ctrl + 'F1~F5' 키");
pCombo->AddString( "Alt + 'F1~F5' 키");

XGETCFG_KEY_PALETTESET0( strCode, strExt);
if ( strCode == "KEY_1"  &&  strExt == "ctrl")			pCombo->SetCurSel( 1);
else if ( strCode == "KEY_1"  &&  strExt == "alt")		pCombo->SetCurSel( 2);
else if ( strCode == "KEY_F1"  &&  strExt == "shift")	pCombo->SetCurSel( 3);
else if ( strCode == "KEY_F1"  &&  strExt == "ctrl")	pCombo->SetCurSel( 4);
else if ( strCode == "KEY_F1"  &&  strExt == "alt")		pCombo->SetCurSel( 5);
else													pCombo->SetCurSel( 0);

}
*/
// void XGameInterfaceOption::RotateMiniMap( bool bRotate)
// {
// 	XSETCFG_GAME_ROTATEMINIMAP( bRotate);
// }
// 
// void XGameInterfaceOption::ShowBuffName( bool bShow)
// {
// 	XSETCFG_GAME_SHOWBUFFNAME( bShow);
// }
// 
// void XGameInterfaceOption::EnableAutoAttack( bool bEnable)
// {
// 	XSETCFG_GAME_AUTONORMALATTACK( bEnable);
// }
// 
// void XGameInterfaceOption::ShowHelmet( bool bShow)
// {
// 	XSETCFG_GAME_SHOWHELMET( bShow);
// }
// 
// void XGameInterfaceOption::ShowMantle( bool bShow)
// {
// 	XSETCFG_GAME_SHOWMANTLE( bShow);
// }
// 
// void XGameInterfaceOption::ShowChatMsg( bool bShow)
// {
// 	XSETCFG_GAME_SHOWCHATMSG( bShow);
// }
// 
// void XGameInterfaceOption::ShowSystemMsg( bool bShow)
// {
// 	XSETCFG_GAME_SHOWSYSTEMMSG( bShow);
// }
// 
// void XGameInterfaceOption::ShowWhisperMsg( bool bShow)
// {
// 	XSETCFG_GAME_SHOWWHISPERMSG( bShow);
// }
// 
// void XGameInterfaceOption::ShowPartyMsg( bool bShow)
// {
// 	XSETCFG_GAME_SHOWPARTYMSG( bShow);
// }
// 
// void XGameInterfaceOption::ShowGuildMsg( bool bShow)
// {
// 	XSETCFG_GAME_SHOWGUILDMSG( bShow);
// }
// 
// void XGameInterfaceOption::ShowChatBubble( bool bShow)
// {
// 	XSETCFG_GAME_SHOWCHATBUBBLE( bShow);
// }
// 
// void XGameInterfaceOption::EnableAbuseFiltering( bool bEnable)
// {
// 	XSETCFG_GAME_ABUSEFILTERING( bEnable);
// }
// 
// void XGameInterfaceOption::ShowPlayerName( bool bShow)
// {
// 	XSETCFG_GAME_PLAYERNAME( bShow);
// }
// 
// void XGameInterfaceOption::ShowPlayerGuild( bool bShow)
// {
// 	XSETCFG_GAME_PLAYERGUILD( bShow);
// }
// 
// void XGameInterfaceOption::ShowPlayerTitle( bool bShow)
// {
// 	XSETCFG_GAME_PLAYERTITLE( bShow);
// }
// 
// void XGameInterfaceOption::ShowMyPlayerName( bool bShow)
// {
// 	XSETCFG_GAME_MYPLAYERNAME( bShow);
// }
// 
// void XGameInterfaceOption::ShowNpcName( bool bShow)
// {
// 	XSETCFG_GAME_NPCNAME( bShow);
// }
// 
// void XGameInterfaceOption::ShowNpcGuild( bool bShow)
// {
// 	XSETCFG_GAME_NPCGUILD( bShow);
// }
// 
// void XGameInterfaceOption::SetInvertedMouse( bool bSet)
// {
// 	XSETCFG_GAME_INVERTEDMOUSE( bSet);
// }
// 
// void XGameInterfaceOption::SetMouseSensitivity( int nValue)
// {
// 	XSETCFG_GAME_MOUSESENSITIVITY( nValue);
// }
// 
// void XGameInterfaceOption::RejectParty( bool bReject)
// {
// 	XSETCFG_GAME_REJECT_PARTY( bReject);
// }
// 
// void XGameInterfaceOption::RejectDuel( bool bReject)
// {
// 	XSETCFG_GAME_REJECT_DUEL( bReject);
// }
// 
// void XGameInterfaceOption::RejectTrade( bool bReject)
// {
// 	XSETCFG_GAME_REJECT_TRADE( bReject);
// }
// 
// void XGameInterfaceOption::SetTextureReduction( int nValue)
// {
// 	float nReduction = (float)nValue / 20.0f;
// 
// 	XSETCFG_VIDEO_TEXTUREREDUCTION( nReduction);
// 
// 	rs3::REngine::GetConfig().m_nTextureReduction = nReduction;
// }
// 
// void XGameInterfaceOption::SetScreenVisibilityRange( int nValue)
// {
// 	XSETCFG_VIDEO_VISIBILITYRANGE( nValue);
// }
// 
// void XGameInterfaceOption::SetScreenBrightness( int nValue)
// {
// 	XSETCFG_VIDEO_BRIGHTNESS( nValue);
// }
// 
// void XGameInterfaceOption::SetAntialiasing( bool bSet)
// {
// 	XSETCFG_VIDEO_ANTIALIASING( bSet);
// }
// 
// void XGameInterfaceOption::ShowShadow( bool bShow)
// {
// 	XSETCFG_VIDEO_SHADOW( bShow);
// }
// 
// void XGameInterfaceOption::ShowWeatherEffect( bool bShow)
// {
// 	XSETCFG_VIDEO_WEATHEREFFECT( bShow);
// }
// 
// void XGameInterfaceOption::ShowUIWhenRecoding( bool bShow)
// {
// 	XSETCFG_VIDEO_SHOWUIWHENREC( bShow);
// }
// 
// void XGameInterfaceOption::EnableBattleSound( bool bEnable)
// {
// 	XSETCFG_AUDIO_BATTLESOUND( bEnable);
// }
// 
// void XGameInterfaceOption::EnableEnvironmentSound( bool bEnable)
// {
// 	XSETCFG_AUDIO_ENVSOUND( bEnable);
// }
// 
// void XGameInterfaceOption::EnableAlertSound( bool bEnable)
// {
// 	XSETCFG_AUDIO_ALERTSOUND( bEnable);
// }
// 
// void XGameInterfaceOption::ChangeControlKeyPalette( int index)
// {
// 	switch ( index)
// 	{
// 	// Number Key
// 	case 0 :
// 		_set_control_key( "PALETTE0", "KEY_1", wstring( ""));
// 		_set_control_key( "PALETTE1", "KEY_2", wstring( ""));
// 		_set_control_key( "PALETTE2", "KEY_3", wstring( ""));
// 		_set_control_key( "PALETTE3", "KEY_4", wstring( ""));
// 		_set_control_key( "PALETTE4", "KEY_5", wstring( ""));
// 		_set_control_key( "PALETTE5", "KEY_6", wstring( ""));
// 		_set_control_key( "PALETTE6", "KEY_7", wstring( ""));
// 		_set_control_key( "PALETTE7", "KEY_8", wstring( ""));
// 		_set_control_key( "PALETTE8", "KEY_9", wstring( ""));
// 		_set_control_key( "PALETTE9", "KEY_0", wstring( ""));
// 		break;
// 
// 	// Fn Key
// 	case 1 :
// 		_set_control_key( "PALETTE0", "KEY_F1", wstring( ""));
// 		_set_control_key( "PALETTE1", "KEY_F2", wstring( ""));
// 		_set_control_key( "PALETTE2", "KEY_F3", wstring( ""));
// 		_set_control_key( "PALETTE3", "KEY_F4", wstring( ""));
// 		_set_control_key( "PALETTE4", "KEY_F5", wstring( ""));
// 		_set_control_key( "PALETTE5", "KEY_F6", wstring( ""));
// 		_set_control_key( "PALETTE6", "KEY_F7", wstring( ""));
// 		_set_control_key( "PALETTE7", "KEY_F8", wstring( ""));
// 		_set_control_key( "PALETTE8", "KEY_F9", wstring( ""));
// 		_set_control_key( "PALETTE9", "KEY_F10", wstring( ""));
// 		break;
// 
// 	default :
// 		return;
// 	}
// 
// 
// 	ApplyControlKeyMap();
// }
// 
// void XGameInterfaceOption::ChangeControlKeyPaletteSet( int index)
// {
// 	switch ( index)
// 	{
// 	// Shift + Number Key
// 	case 0 :
// 		_set_control_key( "PALETTESET0", "KEY_1", "shift");
// 		_set_control_key( "PALETTESET1", "KEY_2", "shift");
// 		_set_control_key( "PALETTESET2", "KEY_3", "shift");
// 		_set_control_key( "PALETTESET3", "KEY_4", "shift");
// 		_set_control_key( "PALETTESET4", "KEY_5", "shift");
// 		break;
// 
// 	// Ctrl + Number Key
// 	case 1 :
// 		_set_control_key( "PALETTESET0", "KEY_1", "ctrl");
// 		_set_control_key( "PALETTESET1", "KEY_2", "ctrl");
// 		_set_control_key( "PALETTESET2", "KEY_3", "ctrl");
// 		_set_control_key( "PALETTESET3", "KEY_4", "ctrl");
// 		_set_control_key( "PALETTESET4", "KEY_5", "ctrl");
// 		break;
// 
// 	// Alt + Number Key
// 	case 2 :
// 		_set_control_key( "PALETTESET0", "KEY_1", "alt");
// 		_set_control_key( "PALETTESET1", "KEY_2", "alt");
// 		_set_control_key( "PALETTESET2", "KEY_3", "alt");
// 		_set_control_key( "PALETTESET3", "KEY_4", "alt");
// 		_set_control_key( "PALETTESET4", "KEY_5", "alt");
// 		break;
// 
// 
// 	// Shift + Fn Key
// 	case 3 :
// 		_set_control_key( "PALETTESET0", "KEY_F1", "shift");
// 		_set_control_key( "PALETTESET1", "KEY_F2", "shift");
// 		_set_control_key( "PALETTESET2", "KEY_F3", "shift");
// 		_set_control_key( "PALETTESET3", "KEY_F4", "shift");
// 		_set_control_key( "PALETTESET4", "KEY_F5", "shift");
// 		break;
// 
// 	// Ctrl + Fn Key
// 	case 4 :
// 		_set_control_key( "PALETTESET0", "KEY_F1", "ctrl");
// 		_set_control_key( "PALETTESET1", "KEY_F2", "ctrl");
// 		_set_control_key( "PALETTESET2", "KEY_F3", "ctrl");
// 		_set_control_key( "PALETTESET3", "KEY_F4", "ctrl");
// 		_set_control_key( "PALETTESET4", "KEY_F5", "ctrl");
// 		break;
// 
// 	// Alt + Fn Key
// 	case 5 :
// 		_set_control_key( "PALETTESET0", "KEY_F1", "alt");
// 		_set_control_key( "PALETTESET1", "KEY_F2", "alt");
// 		_set_control_key( "PALETTESET2", "KEY_F3", "alt");
// 		_set_control_key( "PALETTESET3", "KEY_F4", "alt");
// 		_set_control_key( "PALETTESET4", "KEY_F5", "alt");
// 		break;
// 
// 	default :
// 		return;
// 	}
// 
// 
// 	ApplyControlKeyMap();
// }
// 
// void XGameInterfaceOption::SetShowFeedback( bool bShow )
// {
// 	XSETCFG_GAME_SHOW_FEEDBACK( bShow);
// 
// 
// 	if (global.feedback)
// 	{
// 		global.feedback->SetShowFeedback(bShow);
// 	}
// }
