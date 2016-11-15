#ifndef _XEFFECT_BASE_INVOKER_H
#define _XEFFECT_BASE_INVOKER_H

#pragma once

#include "XEffectInfo.h"

class XEffectBaseInvoker;

struct EFFECT_RESULT_DATA
{
	CUSTOM_EFFECT_TYPE	type;

	MUID				uidEffect;

	bool				bFollow;
	bool				bTerrainAlign;
	bool				bTerrainPos;
	bool				bLoop;

	tstring				strResourceName;
	tstring				strBoneName;

	EFFECT_RESULT_DATA()
	{
		type			= CET_NONE;

		uidEffect.SetZero();

		bFollow			= false;
		bTerrainAlign	= false;
		bTerrainPos		= false;
		bLoop			= false;

		strResourceName.clear();
		strBoneName.clear();
	}
};

struct CHECK_EFFECT_DATA
{
	tstring				strEffectName;
	tstring				strResourceName;
	tstring				strBoneName;
};

struct EFFECT_TRANSEMISION_DATA
{
	tstring				strResourceName;
	tstring				strBoneName;
	tstring				strEventName;

	XBaseEffectInfoItem* pEffectInfoItem;

	RMatrix				matWorldMatrix;

	bool				bPostData;
	bool				bWaterPos;					// 물
	bool				bCancelEnabled;
	bool				bAttackByMy;

	DWORD				dwColor;

	vec3				vSize;
	vec3				vDecalSize;


	float				fWaterHeightMin;			// 수면에 나올 이펙트 최소 높이. aterPos = true 일 경우에만 활성화 됩니다. min 0.0f와 max 0.0f 값이면 높이 제한이 없는 겁니다.
	float				fWaterHeightMax;			// 수며에 나올 이펙트 최대 높이

	int					nPartsSlotType;

	unsigned int		nDamageFlags;

	EFFECT_TRANSEMISION_DATA()
	{
		pEffectInfoItem	= NULL;

		matWorldMatrix.MakeIdentity();
		bPostData = false;

		dwColor = 0xFFFFFFFF;
		
		vSize = vec3::ZERO;
		vDecalSize = vec3::ZERO;

		fWaterHeightMin			= 0.0f;
		fWaterHeightMax			= 0.0f;

		bWaterPos				= false;
		bCancelEnabled			= false;
		bAttackByMy				= false;

		nPartsSlotType = 0;
		nDamageFlags = 0;
	}
};

struct EFFECT_POST_TRANSFORM
{
	RMatrix				matWorldMatrix;
	DWORD				dwColor;
	vec3				vSize;
	vec3				vDecalSize;

	bool				bAttackByMy;

	unsigned int		nDamageFlags;

	EFFECT_POST_TRANSFORM()
	{
		matWorldMatrix.MakeIdentity();
		dwColor = 0xFFFFFFFF;
		vSize = vec3::ZERO;
		vDecalSize = vec3::ZERO;
		bAttackByMy = false;
		nDamageFlags = 0;
	}
};

struct EFFECT_CREATE_RESULT_DATA
{
	bool				bLoop;

	EFFECT_CREATE_RESULT_DATA()
	{
		bLoop			= false;
	}
};

//////////////////////////////////////////////////////////////////////////
typedef void(*EFFECTHANDLER) (XEffectBaseInvoker&, EFFECT_TRANSEMISION_DATA&, tstring&, tstring&, const float, EFFECT_RESULT_DATA& );

class XEffectBaseInvoker
{
protected:
	//////////////////////////////////////////////////////////////////////////
	// 예약 정보
	struct RESERVED_EFFECT_DATA
	{
		tstring						strEffectName;
		tstring						strParam;
		tstring						strBoneName;

		tstring						strSoundResourceName;
		tstring						strModelResourceName;
		tstring						strCameraResourceName;

		RESERVED_EFFECT_DATA() {}
		RESERVED_EFFECT_DATA(TCHAR* str1, TCHAR* str2, TCHAR* str3, TCHAR* str4, TCHAR* str5, TCHAR* str6)
			: strEffectName(str1), strParam(str2), strBoneName(str3), strSoundResourceName(str4), strModelResourceName(str5), strCameraResourceName(str6)
		{}
	};
	// 예약 정보
	//////////////////////////////////////////////////////////////////////////

#define SET_RESERVED_EFFECT_DATA(str1, str2, str3, str4, str5, str6) \
	{																							\
		RESERVED_EFFECT_DATA stReservedEffectData(str1, str2, str3, str4, str5, str6);			\
		m_vecResrvedEffectData.push_back(stReservedEffectData);									\
	}

protected:
	vector<RESERVED_EFFECT_DATA>	m_vecResrvedEffectData;
	map<CUSTOM_EFFECT_TYPE, EFFECTHANDLER> m_mapEffectHandlers;

private:
	// 디폴트
	void					InitReservedData();

protected:
	virtual tstring			GetReserved_Param(tstring& strEffectName)								{ return _T(""); }
	virtual tstring			GetReserved_BoneName(tstring& strEffectName)							{ return _T(""); }
	virtual tstring			GetReserved_SoundName(tstring& strEffectName, tstring& strEffectparam, unsigned int nFlags)	{ return _T(""); }
	virtual tstring			GetReserved_ModelName(tstring& strEffectName)							{ return _T(""); }

	virtual MUID			SetSound(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Sound& soundInfo, EFFECT_CREATE_RESULT_DATA& resultData)			{ return MUID::ZERO; }
	virtual MUID			SetModel(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Model& modelInfo, EFFECT_CREATE_RESULT_DATA& resultData)			{ return MUID::ZERO; }
	virtual void			SetCamera(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_Camera& cameraInfo)			{}
	virtual void			SetPostEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_PostEffect& postInfo) {}
	virtual MUID			SetDecalEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_DecalEffect& decalInfo) { return MUID::ZERO; }
	virtual MUID			SetIlluminationEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_IlluminationEffect& illuminationInfo) { return MUID::ZERO; }
	virtual MUID			SetLightEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_LightEffect& lightInfo) { return MUID::ZERO; }
	virtual void			SetAlphaTestFadeEffect(EFFECT_TRANSEMISION_DATA& _data, XEffectInfoItem_AlphaTestFadeEffect& fadeInfo) { return; }

public:
	XEffectBaseInvoker();
	virtual ~XEffectBaseInvoker(void);

	static void				OnEffect_Sound(XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData);
	static void				OnEffect_Model(XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData);
	static void				OnEffect_Camera(XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData);
	static void				OnEffect_PostEffect(XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData);
	static void				OnEffect_DecalEffect(XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData);
	static void				OnEffect_IlluminationEffect(XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData);
	static void				OnEffect_LightEffect(XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData);
	static void				OnEffect_AlphaTestFadeEffect(XEffectBaseInvoker& EffectInvoker, EFFECT_TRANSEMISION_DATA& _data, tstring& strBoneName, tstring& strParam, const float fEffectDurationTime, EFFECT_RESULT_DATA& outData);

	static bool				CheckReservedEffect(tstring& strEffectName);
};


#endif // _XEFFECT_BASE_INVOKER_H