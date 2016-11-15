#pragma once

#include "MXml.h"
#include "MTypes.h"
#include "XPostEffect.h"
#include "MTstring.h"

#define EFFECT_NORMALIZE_XML_TAG_MAIET					"maiet"

#define EFFECT_NORMALIZE_XML_TAG_EFFECT					"Effect"
#define EFFECT_NORMALIZE_XML_TAG_SOUND					"Sound"
#define EFFECT_NORMALIZE_XML_TAG_MODEL					"Model"
#define EFFECT_NORMALIZE_XML_TAG_CAMERA					"Camera"
#define EFFECT_NORMALIZE_XML_TAG_POSTEFFECT				"PostEffect"
#define EFFECT_NORMALIZE_XML_TAG_DECAL					"DecalEffect"
#define EFFECT_NORMALIZE_XML_TAG_ILLUMINATION			"IlluminationEffect"
#define EFFECT_NORMALIZE_XML_TAG_EFFECTLIGHT			"LightEffect"
#define EFFECT_NORMALIZE_XML_TAG_ALPHATESTFADELIGHT		"AlphaTestFadeEffect"

#define EFFECT_NORMALIZE_XML_ATTR_NAME					"Name"
#define EFFECT_NORMALIZE_XML_ATTR_FILENAME				"FileName"
#define EFFECT_NORMALIZE_XML_ATTR_BONENAME				"BoneName"
#define EFFECT_NORMALIZE_XML_ATTR_FOLLOW				"Follow"
#define EFFECT_NORMALIZE_XML_ATTR_IMMEDIATLY_DELETE		"ImmediatlyDelete"
#define EFFECT_NORMALIZE_XML_ATTR_DISTANCE				"Distance"
#define EFFECT_NORMALIZE_XML_ATTR_DURATIONTIME			"DurationTime"
#define EFFECT_NORMALIZE_XML_ATTR_EFFECT_DURATIONTIME	"EffectDurationTime"
#define EFFECT_NORMALIZE_XML_ATTR_TERRAIN_ALIGN			"TerrainAlign"
#define EFFECT_NORMALIZE_XML_ATTR_TERRAIN_POS			"TerrainPos"
#define EFFECT_NORMALIZE_XML_ATTR_WATER_POS				"WaterPos"
#define EFFECT_NORMALIZE_XML_ATTR_CANCEL_ENABLED		"CancelEnabled"
#define EFFECT_NORMALIZE_XML_ATTR_WATER_HEIGHT_MIN		"WaterHeightMin"
#define EFFECT_NORMALIZE_XML_ATTR_WATER_HEIGHT_MAX		"WaterHeightMax"
#define EFFECT_NORMALIZE_XML_ATTR_ALIGN					"Align"
#define EFFECT_NORMALIZE_XML_ATTR_AUTOSCALE				"AutoScale"
#define EFFECT_NORMALIZE_XML_ATTR_SCALE					"Scale"
#define EFFECT_NORMALIZE_XML_ATTR_POSTEFFECT_TYPE		"Type"
#define EFFECT_NORMALIZE_XML_ATTR_PLAYTIME				"PlayTime"
#define EFFECT_NORMALIZE_XML_ATTR_SCATTIME				"ScatTime"
#define EFFECT_NORMALIZE_XML_ATTR_SPEED					"Speed"
#define EFFECT_NORMALIZE_XML_ATTR_SCAREFCTORTYPE		"ScarFactorType"
#define EFFECT_NORMALIZE_XML_ATTR_USETYPE				"UseType"
#define EFFECT_NORMALIZE_XML_ATTR_START_VISIBILITY_TIME	"StartVisibilityTime"
#define EFFECT_NORMALIZE_XML_ATTR_END_VISIBILITY_TIME	"EndVisibilityTime"
#define EFFECT_NORMALIZE_XML_ATTR_END_LAST_FRAME_PAUSE "LastFramePause"
#define EFFECT_NORMALIZE_XML_ATTR_MOTION_BLUR_MODE		"Mode"
#define EFFECT_NORMALIZE_XML_ATTR_SIZE_X				"SizeX"
#define EFFECT_NORMALIZE_XML_ATTR_SIZE_Y				"SizeY"
#define EFFECT_NORMALIZE_XML_ATTR_X						"X"
#define EFFECT_NORMALIZE_XML_ATTR_Y						"Y"
#define EFFECT_NORMALIZE_XML_ATTR_Z						"Z"
#define EFFECT_NORMALIZE_XML_ATTR_START_TIME			"StartTime"
#define EFFECT_NORMALIZE_XML_ATTR_START_DURATION_TIME	"StartDurationTime"
#define EFFECT_NORMALIZE_XML_ATTR_END_TIME				"EndTime"
#define EFFECT_NORMALIZE_XML_ATTR_END_DURATION_TIME		"EndDurationTime"
#define EFFECT_NORMALIZE_XML_ATTR_ACTORNODENAME			"ActorNodeName"
#define EFFECT_NORMALIZE_XML_ATTR_LOOP					"Loop"
#define EFFECT_NORMALIZE_XML_ATTR_RADIUS				"Radius"
#define EFFECT_NORMALIZE_XML_ATTR_ATTENUATION_START		"AttenuationStart"
#define EFFECT_NORMALIZE_XML_ATTR_INTENSITY				"Intensity"
#define EFFECT_NORMALIZE_XML_ATTR_ATTACKLIGHT			"AttackLight"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_R				"Color_R"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_G				"Color_G"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_B				"Color_B"
#define EFFECT_NORMALIZE_XML_ATTR_MIN					"Min"
#define EFFECT_NORMALIZE_XML_ATTR_MAX					"Max"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_1_R				"Color1_R"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_1_G				"Color1_G"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_1_B				"Color1_B"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_2_R				"Color2_R"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_2_G				"Color2_G"
#define EFFECT_NORMALIZE_XML_ATTR_COLOR_2_B				"Color2_B"
#define EFFECT_NORMALIZE_XML_ATTR_SIZE_1				"Size1"
#define EFFECT_NORMALIZE_XML_ATTR_SIZE_2				"Size2"
#define EFFECT_NORMALIZE_XML_ATTR_MESHNODENAME			"MeshNodeName"
#define EFFECT_NORMALIZE_XML_ATTR_INVERSE				"Inverse"

#define EFFECT_NORMALIZE_XML_VALUE_MOTIONBLUR			"motionblur"
#define EFFECT_NORMALIZE_XML_VALUE_OBJECTMOTIONBLUR		"objectblur"
#define EFFECT_NORMALIZE_XML_VALUE_OBJECTACCUMBLUR		"accumblur"
#define EFFECT_NORMALIZE_XML_VALUE_PIXELOBJECTMOTIONBLUR	"pixelobjectblur"
#define EFFECT_NORMALIZE_XML_VALUE_SAT_NORMAL			"SAT_NORMAL"
#define EFFECT_NORMALIZE_XML_VALUE_SAT_FAST_SCAT		"SAT_FAST_SCAT"
#define EFFECT_NORMALIZE_XML_VALUE_SAT_SLOW_SCAT		"SAT_SLOW_SCAT"
#define EFFECT_NORMALIZE_XML_VALUE_SAT_GATTER			"SAT_GATTER"
#define EFFECT_NORMALIZE_XML_VALUE_MYSELF				"myself"
#define EFFECT_NORMALIZE_XML_VALUE_ALL					"all"
#define EFFECT_NORMALIZE_XML_VALUE_HALF					"half"
#define EFFECT_NORMALIZE_XML_VALUE_FULL					"full"

#define EFFECT_NORMALIZE_XML_VALUE_PROJECTION_DECAL		"projection"
#define EFFECT_NORMALIZE_XML_VALUE_POLYGON_DECAL		"polygon"

enum CUSTOM_EFFECT_TYPE
{
	CET_NONE = 0,
	CET_MODEL,
	CET_SOUND,
	CET_CAMERA,
	CET_POSTEFFECT,					// 일회성 이펙트
	CET_DECALEFFECT,
	CET_ILLUMINATION,
	CET_LIGHTEFFECT,
	CET_ALPHATESTFADE,				// 일회성 이펙트

	CET_MAX
};

enum POSTEFFECT_SAT
{
	PESAT_NORMAL,
	PESAT_FAST_SCAT,
	PESAT_SLOW_SCAT,
	PESAT_GATTER,
};

enum POSTEFFECT_USE_TYPE
{
	PEUT_NONE,
	PEUT_MYSELF,
	PEUT_ALL,
};

enum DECALEFFECT_TYPE
{
	DET_NONE,
	DET_PROJECTION,
	DET_POLYGON
};

enum POSTEFFECT_BLUR_MODE // PIXEL_MOTION_BLUR_MODE 참고
{
	PBM_NONE,
	PBM_HALF,
	PBM_FULL
};
//////////////////////////////////////////////////////////////////////////
// 이벤트 정보
class XBaseEffectInfoItem
{
public:
	CUSTOM_EFFECT_TYPE			eEffectType;
	
	tstring						strResourceName;			// 사용할 리소스 이름

	XBaseEffectInfoItem() 
	{
		strResourceName.clear();
		eEffectType				= CET_NONE;
	}

	virtual ~XBaseEffectInfoItem()	{}
};

class XEffectInfoItem_Model : public XBaseEffectInfoItem
{
public:
	tstring						strModelBoneName;

	vec3						vScale;						// 스케일 값

	float						fDurationTime;				// 지속시간
	float						fStartVisibilityTime;		// 시작시 나타나는 시간
	float						fEndVisibilityTime;			// 끝날때 사라지는 시간
	float						fx, fy, fz;					// 로컬 좌표

	bool						bFollow;					// 이펙트에서 따라다니는가... 기본(true)
	bool						bTerrainAlign;				// 지형 Up 방향
	bool						bTerrainPos;				// 지형
	bool						bAlign;
	bool						bImmediatlyDelete;			// 즉시 삭제
	bool						bAutoScale;					// 
	bool						bLastFramePause;			// 종료시 멈춘다.

	XEffectInfoItem_Model()
	{
		strModelBoneName.clear();

		vScale					= vec3(1.0f, 1.0f, 1.0f);

		fDurationTime			= 0.0f;
		fStartVisibilityTime	= 0.0f;
		fEndVisibilityTime		= 0.0f;
		fx = fy = fz			= 0.0f;

		bFollow					= true;
		bImmediatlyDelete		= false;
		bTerrainAlign			= false;
		bTerrainPos				= false;
		bAlign					= true;
		bAutoScale				= false;
		bLastFramePause			= false;
	}
};

class XEffectInfoItem_Sound : public XBaseEffectInfoItem
{
public:
	tstring						strSoundBoneName;

	float						fDurationTime;				// 지속시간

	XEffectInfoItem_Sound() 
	{
		strSoundBoneName.clear();

		fDurationTime			= 0.0f;
	}
};

class XEffectInfoItem_Camera : public XBaseEffectInfoItem
{
public:
	int							nDistance;
	float						fDurationTime;

	XEffectInfoItem_Camera() 
	{
		nDistance				= 0;
		fDurationTime			= 0.0f;
	}
};

class XEffectInfoItem_PostEffect : public XBaseEffectInfoItem
{
public:
	POST_EFFECT_KIND			nPostEffectType;
	float						fPlayTime;
	float						fScatTime; 
	float						fSpeed;
	POSTEFFECT_SAT				nScarFactorType;
	POSTEFFECT_USE_TYPE			nUseType;
	int							nDistance;
	POSTEFFECT_BLUR_MODE		nBlurType;

	XEffectInfoItem_PostEffect()
	{
		nPostEffectType			= PEK_NONE;
		fPlayTime				= 0.0f;
		fScatTime				= 0.0f;
		fSpeed					= 0.0f;
		nScarFactorType			= PESAT_NORMAL;
		nDistance				= 0;
		nUseType				= PEUT_NONE;
		nBlurType				= PBM_HALF;
	}
};

class XEffectInfoItem_DecalEffect : public XBaseEffectInfoItem
{
public:
	DECALEFFECT_TYPE			nDecalEffectType;
	
	tstring						strModelBoneName;

	bool						bFollow;					// 이펙트에서 따라다니는가... 기본(false)
	float						fSizeX;						// X 사이즈
	float						fSizeY;						// Y 사이즈
	float						fSizeZ;						// Z 사이즈
	float						fDurationTime;				// 지속시간
	float						fStartVisibilityTime;		// 시작시 나타나는 시간
	float						fEndVisibilityTime;			// 끝날때 사라지는 시간

	XEffectInfoItem_DecalEffect()
	{
		nDecalEffectType		= DET_PROJECTION;
		fDurationTime			= 0.0f;						// 0은 계속 그린다.
		fStartVisibilityTime	= 0.0f;
		fEndVisibilityTime		= 0.0f;
		fSizeX					= 0.0f;
		fSizeY					= 0.0f;
		fSizeZ					= 40.0f;
		bFollow					= false;
	}
};

class XEffectInfoItem_IlluminationEffect : public XBaseEffectInfoItem
{
public:
	tstring						strModelActorNodeName;

	float						fStartTime;
	float						fStartDurationTime; 
	float						fEndTime; 
	float						fEndDurationTime;

	float						fDurationTime;				// 지속시간

	float						fMin;						// min 세기
	float						fMax;						// max 세기

	bool						bLoop;

	XEffectInfoItem_IlluminationEffect()
	{
		fDurationTime			= 0.0f;						// 0은 계속 그린다.

		fStartTime				= 0.0f;
		fStartDurationTime		= 0.0f;
		fEndTime				= 0.0f;
		fEndDurationTime		= 0.0f;

		fMin					= 1.0f;
		fMax					= 4.0f;

		bLoop					= false;
	}
};

class XEffectInfoItem_LightEffect : public XBaseEffectInfoItem
{
public:
	tstring						strModelBoneName;

	float						fRadius;
	float						fDurationTime;
	float						fAttenuationStart;
	float						fIntensity;

	DWORD						dwColor;

	bool						bFollow;
	bool						bAttackLight;

	XEffectInfoItem_LightEffect()
	{
		dwColor					= 0;
		fRadius					= 1.0f;
		fDurationTime			= 0.0f;						// 0은 계속 그린다.
		fAttenuationStart		= 1.0f;
		fIntensity				= 1.0f;

		bFollow					= false;
		bAttackLight			= false;
	}	
};

class XEffectInfoItem_AlphaTestFadeEffect : public XBaseEffectInfoItem
{
public:
	vector<tstring>				vecstrMeshNodeName;
	
	float						fStartDurationTime; 
	float						fEndDurationTime;

	DWORD						dwFadeEffectColor1;
	DWORD						dwFadeEffectColor2;

	float						fFadeEffectSize1;
	float						fFadeEffectSize2;

	bool						bInverse;


	XEffectInfoItem_AlphaTestFadeEffect()
	{
		fStartDurationTime		= 0.0f;
		fEndDurationTime		= 0.0f;

		dwFadeEffectColor1		= 0;
		dwFadeEffectColor2		= 0;

		fFadeEffectSize1		= 0.1f;
		fFadeEffectSize2		= 0.15f;

		bInverse				= false;
	}
};

// 이벤트 정보
//////////////////////////////////////////////////////////////////////////

class XEffectInfo
{
public:
	XEffectInfo();
	virtual ~XEffectInfo(void);

	vector<XBaseEffectInfoItem *>		m_vecEffectInfoItem;

	tstring						m_strEffectName;
	float						m_fEffectDurationTime;		// 이펙트 전제 지속시간

	bool						bWaterPos;					// 물
	bool						bCancelEnabled;

	float						fWaterHeightMin;			// 수면에 나올 이펙트 최소 높이. aterPos = true 일 경우에만 활성화 됩니다. min 0.0f와 max 0.0f 값이면 높이 제한이 없는 겁니다.
	float						fWaterHeightMax;			// 수며에 나올 이펙트 최대 높이

};

class XEffectInfoMgr 
{
public:
	vector<XEffectInfo *>		m_vecEffectInfo;

private:
	void						ParseEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);
	void						ParseSoundEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);
	void						ParseModelEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);
	void						ParseCameraEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);
	void						ParsePostEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);
	void						ParseDecalEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);
	void						ParseIlluminationEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);
	void						ParseLightEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);
	void						ParseAlphaTestFadeEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo);

	void						Destroy();

protected:
	void						ParseEffect(MXmlElement* pElement, MXml* pXml);
	virtual	void				GetCommaStringSplitter(const tstring& strValue, vector<tstring>& vecOut) {}

public:
	XEffectInfoMgr() {}
	virtual ~XEffectInfoMgr();

	bool						Load(tstring strFileName);
	bool						Reload(tstring strFileName);

	XEffectInfo *				GetEffectInfo(tstring& strEffectName);


};