#include "stdafx.h"
#include "XEffectBaseInvoker.h"

XEffectBaseInvoker::XEffectBaseInvoker()
{
	m_mapEffectHandlers.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::value_type( CET_MODEL,		OnEffect_Model) );
	m_mapEffectHandlers.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::value_type( CET_SOUND,		OnEffect_Sound) );
	m_mapEffectHandlers.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::value_type( CET_CAMERA,		OnEffect_Camera) );
	m_mapEffectHandlers.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::value_type( CET_POSTEFFECT,	OnEffect_PostEffect) );
	m_mapEffectHandlers.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::value_type( CET_DECALEFFECT, OnEffect_DecalEffect) );
	m_mapEffectHandlers.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::value_type( CET_ILLUMINATION, OnEffect_IlluminationEffect) );
	m_mapEffectHandlers.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::value_type( CET_LIGHTEFFECT, OnEffect_LightEffect) );
	m_mapEffectHandlers.insert( map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER>::value_type( CET_ALPHATESTFADE, OnEffect_AlphaTestFadeEffect) );

	InitReservedData();
}

XEffectBaseInvoker::~XEffectBaseInvoker( void )
{

}

void XEffectBaseInvoker::OnEffect_Sound( XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_Sound * pEffectInfoItem_Sound = static_cast<XEffectInfoItem_Sound*>(_data.pEffectInfoItem);

	tstring strSoundResourceName	= pEffectInfoItem_Sound->strResourceName.empty() ? EffectInvoker.GetReserved_SoundName(_data.strEventName, strParam, _data.nDamageFlags) : pEffectInfoItem_Sound->strResourceName;
	tstring strSoundBoneName		= pEffectInfoItem_Sound->strSoundBoneName.empty() ? strBoneName : pEffectInfoItem_Sound->strSoundBoneName;
	float fDurationTime				= (fEffectDurationTime > 0.0f) ? fEffectDurationTime : pEffectInfoItem_Sound->fDurationTime;

	_data.strResourceName		= strSoundResourceName;
	_data.strBoneName			= strSoundBoneName;

	XEffectInfoItem_Sound soundInfo = *pEffectInfoItem_Sound;
	soundInfo.fDurationTime			= fDurationTime;

	EFFECT_CREATE_RESULT_DATA createResultData;
	MUID uidSound = EffectInvoker.SetSound(_data, soundInfo, createResultData);

	if(uidSound != MUID::ZERO)
	{
		outData.type			= _data.pEffectInfoItem->eEffectType;
		outData.uidEffect		= uidSound;
		outData.strResourceName	= strSoundResourceName;
		outData.strBoneName		= strSoundBoneName;
		outData.bLoop			= createResultData.bLoop;
	}
}

void XEffectBaseInvoker::OnEffect_Model( XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_Model * pEffectInfoItem_Model = static_cast<XEffectInfoItem_Model*>(_data.pEffectInfoItem);

	tstring strModelResourceName = pEffectInfoItem_Model->strResourceName.empty() ? EffectInvoker.GetReserved_ModelName(_data.strEventName) : pEffectInfoItem_Model->strResourceName;
	tstring strModelBoneName		= pEffectInfoItem_Model->strModelBoneName.empty() ? strBoneName : pEffectInfoItem_Model->strModelBoneName;
	float fDurationTime = (fEffectDurationTime > 0.0f) ? fEffectDurationTime : pEffectInfoItem_Model->fDurationTime;

	_data.strResourceName		= strModelResourceName;
	_data.strBoneName			= strModelBoneName;

	XEffectInfoItem_Model modelInfo = *pEffectInfoItem_Model;
	modelInfo.fDurationTime			= fDurationTime;

	EFFECT_CREATE_RESULT_DATA createResultData;
	MUID uidModel = EffectInvoker.SetModel(_data, modelInfo, createResultData);

	if(uidModel != MUID::ZERO)
	{
		outData.type			= _data.pEffectInfoItem->eEffectType;
		outData.uidEffect		= uidModel;
		outData.strResourceName	= strModelResourceName;
		outData.strBoneName		= strModelBoneName;
		outData.bFollow			= pEffectInfoItem_Model->bFollow;
		outData.bTerrainAlign	= pEffectInfoItem_Model->bTerrainAlign;
		outData.bTerrainPos		= pEffectInfoItem_Model->bTerrainPos;
		outData.bLoop			= createResultData.bLoop;
	}
}

void XEffectBaseInvoker::OnEffect_Camera( XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_Camera * pEffectInfoItem_Camera = static_cast<XEffectInfoItem_Camera*>(_data.pEffectInfoItem);

	float fDurationTime = (fEffectDurationTime > 0.0f) ? fEffectDurationTime : pEffectInfoItem_Camera->fDurationTime;

	_data.strResourceName		= pEffectInfoItem_Camera->strResourceName;

	XEffectInfoItem_Camera cameraInfo = *pEffectInfoItem_Camera;
	cameraInfo.fDurationTime = fDurationTime;

	EffectInvoker.SetCamera(_data, cameraInfo);
}

void XEffectBaseInvoker::InitReservedData()
{
	SET_RESERVED_EFFECT_DATA(_T("foot_left"),		_T("s"),	_T("Bip01 L Foot"),		_T("L"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("foot_right"),		_T("s"),	_T("Bip01 R Foot"),		_T("R"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("foot_jump_up"),	_T(""),		_T("Bip01 R Foot"),		_T("jumpup"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("foot_jump_down"),	_T(""),		_T("Bip01 R Foot"),		_T("jumpdown"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("foot_back"),		_T(""),		_T("Bip01 R Foot"),		_T("back"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("foot_draw"),		_T(""),		_T("Bip01 R Foot"),		_T("draw"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("foot_attack"),		_T(""),		_T("Bip01 R Foot"),		_T("attack"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("body_fall"),		_T(""),		_T("dummy_ef_body"),	_T(""),			_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("shake_armor"),		_T(""),		_T("dummy_ef_body"),	_T(""),			_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("shake_weapon"),	_T(""),		_T("dummy_ef_body"),	_T(""),			_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("swing1"),			_T(""),		_T(""),					_T("swing1"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("swing2"),			_T(""),		_T(""),					_T("swing2"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("swing3"),			_T(""),		_T(""),					_T("swing3"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("swing4"),			_T(""),		_T(""),					_T("swing4"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("swing5"),			_T(""),		_T(""),					_T("swing4"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("weaponfall"),		_T(""),		_T(""),					_T("weaponfall"),_T(""),	_T(""));
	SET_RESERVED_EFFECT_DATA(_T("impact"),			_T(""),		_T(""),					_T(""),			_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("ss1"),				_T(""),		_T(""),					_T("ss1"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("ss2"),				_T(""),		_T(""),					_T("ss2"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("ss3"),				_T(""),		_T(""),					_T("ss3"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("ss4"),				_T(""),		_T(""),					_T("ss4"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("t_ss1"),			_T(""),		_T(""),					_T("t_ss1"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("t_ss2"),			_T(""),		_T(""),					_T("t_ss2"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("sd"),				_T(""),		_T(""),					_T(""),			_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("sd_swing"),		_T(""),		_T(""),					_T("sd_swing"),	_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("sd_hit"),			_T(""),		_T(""),					_T(""),			_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("as_up"),			_T(""),		_T(""),					_T("up"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("as_down"),			_T(""),		_T(""),					_T("down"),		_T(""),		_T(""));
	SET_RESERVED_EFFECT_DATA(_T("as_run"),			_T(""),		_T(""),					_T("run"),		_T(""),		_T(""));
}

void XEffectBaseInvoker::OnEffect_PostEffect( XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_PostEffect * pEffectInfoItem_PostEffect = static_cast<XEffectInfoItem_PostEffect*>(_data.pEffectInfoItem);

	float fPlayTime				= (fEffectDurationTime > 0.0f) ? fEffectDurationTime : pEffectInfoItem_PostEffect->fPlayTime;

	XEffectInfoItem_PostEffect postEffectInfo = *pEffectInfoItem_PostEffect;
	pEffectInfoItem_PostEffect->fPlayTime = fPlayTime;

	EffectInvoker.SetPostEffect(_data, postEffectInfo);
}

void XEffectBaseInvoker::OnEffect_DecalEffect( XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_DecalEffect * pEffectInfoItem_DecalEffect = static_cast<XEffectInfoItem_DecalEffect*>(_data.pEffectInfoItem);

	tstring strDecalResourceName = pEffectInfoItem_DecalEffect->strResourceName;
	float fDurationTime			= (fEffectDurationTime > 0.0f) ? fEffectDurationTime : pEffectInfoItem_DecalEffect->fDurationTime;

	_data.strResourceName		= strDecalResourceName;
	_data.strBoneName			= strBoneName;

	XEffectInfoItem_DecalEffect decalEffect = *pEffectInfoItem_DecalEffect;
	decalEffect.fDurationTime	= fDurationTime;

	MUID uidModel = EffectInvoker.SetDecalEffect(_data, decalEffect);

	if(uidModel != MUID::ZERO)
	{
		outData.type			= _data.pEffectInfoItem->eEffectType;
		outData.uidEffect		= uidModel;
		outData.strResourceName	= _data.strResourceName;
		outData.strBoneName		= strBoneName;
		outData.bFollow			= pEffectInfoItem_DecalEffect->bFollow;
	}
}

void XEffectBaseInvoker::OnEffect_IlluminationEffect( XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_IlluminationEffect * pEffectInfoItem_IlluminationEffect = static_cast<XEffectInfoItem_IlluminationEffect*>(_data.pEffectInfoItem);

	tstring strDecalResourceName = pEffectInfoItem_IlluminationEffect->strResourceName.empty() ? _data.strEventName : pEffectInfoItem_IlluminationEffect->strResourceName;
	tstring strModelBoneName		= pEffectInfoItem_IlluminationEffect->strModelActorNodeName.empty() ? strBoneName : pEffectInfoItem_IlluminationEffect->strModelActorNodeName;
	float fDurationTime			= (fEffectDurationTime > 0.0f) ? fEffectDurationTime : pEffectInfoItem_IlluminationEffect->fDurationTime;

	_data.strResourceName		= strDecalResourceName;
	_data.strBoneName			= strModelBoneName;

	XEffectInfoItem_IlluminationEffect illuminationEffectInfo = *pEffectInfoItem_IlluminationEffect;
	illuminationEffectInfo.fDurationTime = fDurationTime;

	MUID uidModel = EffectInvoker.SetIlluminationEffect(_data, illuminationEffectInfo);

	if(uidModel != MUID::ZERO)
	{
		outData.type			= _data.pEffectInfoItem->eEffectType;
		outData.uidEffect		= uidModel;
		outData.strResourceName	= _data.strResourceName;
		outData.strBoneName		= strBoneName;
	}
}

void XEffectBaseInvoker::OnEffect_LightEffect( XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_LightEffect * pEffectInfoItem_LightEffect = static_cast<XEffectInfoItem_LightEffect*>(_data.pEffectInfoItem);

	float fDurationTime			= (fEffectDurationTime > 0.0f) ? fEffectDurationTime : pEffectInfoItem_LightEffect->fDurationTime;
	tstring strModelBoneName		= pEffectInfoItem_LightEffect->strModelBoneName.empty() ? strBoneName : pEffectInfoItem_LightEffect->strModelBoneName;

	_data.strBoneName			= strModelBoneName;

	XEffectInfoItem_LightEffect lightEffectInfo = *pEffectInfoItem_LightEffect;
	lightEffectInfo.fDurationTime = fDurationTime;

	MUID uidModel = EffectInvoker.SetLightEffect(_data, lightEffectInfo);

	if(uidModel != MUID::ZERO)
	{
		outData.type			= _data.pEffectInfoItem->eEffectType;
		outData.uidEffect		= uidModel;
		outData.strBoneName		= strBoneName;
	}
}

void XEffectBaseInvoker::OnEffect_AlphaTestFadeEffect( XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData )
{
	if(_data.pEffectInfoItem == NULL)
		return;

	XEffectInfoItem_AlphaTestFadeEffect * pEffectInfoItem_AlphaTestFadeEffect = static_cast<XEffectInfoItem_AlphaTestFadeEffect*>(_data.pEffectInfoItem);
	XEffectInfoItem_AlphaTestFadeEffect fadeEffectInfo = *pEffectInfoItem_AlphaTestFadeEffect;

	EffectInvoker.SetAlphaTestFadeEffect(_data, fadeEffectInfo);
}

bool XEffectBaseInvoker::CheckReservedEffect( tstring& strEffectName )
{
	XEffectBaseInvoker checkInvoker;

	for(vector<RESERVED_EFFECT_DATA>::iterator it = checkInvoker.m_vecResrvedEffectData.begin(); it != checkInvoker.m_vecResrvedEffectData.end(); it++)
	{
		if((*it).strEffectName == strEffectName)
		{
			return true;
		}
	}

	return false;
}
