#include "stdafx.h"
#include "XEffectInfo.h"
#include "MTypes.h"
#include "MLocale.h"

XEffectInfo::XEffectInfo()
{
	m_fEffectDurationTime	= 0.0f;
	fWaterHeightMin			= 0.0f;
	fWaterHeightMax			= 0.0f;

	bWaterPos				= false;
	bCancelEnabled			= false;

}

XEffectInfo::~XEffectInfo( void )
{
	for(vector<XBaseEffectInfoItem *>::iterator it = m_vecEffectInfoItem.begin(); it != m_vecEffectInfoItem.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_vecEffectInfoItem.clear();
}

XEffectInfoMgr::~XEffectInfoMgr()
{
	Destroy();
}

bool XEffectInfoMgr::Load( tstring strFileName )
{
	MXml xml;

	if(!xml.LoadFile(MLocale::ConvTCHARToAnsi(strFileName.c_str()).c_str())) 
	{
		return false;
	}

	//------------------------------------------------------------------------
	// 로딩
	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(EFFECT_NORMALIZE_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), EFFECT_NORMALIZE_XML_TAG_EFFECT))
		{
			ParseEffect(pElement, &xml);
		}
	}

	return true;
}
void XEffectInfoMgr::ParseEffect(MXmlElement* pElement, MXml* pXml)
{
	XEffectInfo * pEffectInfo = new XEffectInfo;

	_Attribute(pEffectInfo->m_strEffectName,				pElement, EFFECT_NORMALIZE_XML_ATTR_NAME);
	_Attribute(&(pEffectInfo->m_fEffectDurationTime),		pElement, EFFECT_NORMALIZE_XML_ATTR_EFFECT_DURATIONTIME);
	_Attribute(&(pEffectInfo->bWaterPos),					pElement, EFFECT_NORMALIZE_XML_ATTR_WATER_POS);
	_Attribute(&(pEffectInfo->bCancelEnabled),				pElement, EFFECT_NORMALIZE_XML_ATTR_CANCEL_ENABLED);
	_Attribute(&(pEffectInfo->fWaterHeightMin),				pElement, EFFECT_NORMALIZE_XML_ATTR_WATER_HEIGHT_MIN);
	_Attribute(&(pEffectInfo->fWaterHeightMax),				pElement, EFFECT_NORMALIZE_XML_ATTR_WATER_HEIGHT_MAX);

	ParseEffectInfo(pElement, pXml, pEffectInfo);

	m_vecEffectInfo.push_back(pEffectInfo);
}

void XEffectInfoMgr::ParseEffectInfo( MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo )
{
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		// 사운드
		if(!_stricmp(pAttElement->Value(), EFFECT_NORMALIZE_XML_TAG_SOUND))
		{
			ParseSoundEffectInfo(pAttElement, pXml, pEffectInfo);
		}

		// 이펙트
		if(!_stricmp(pAttElement->Value(), EFFECT_NORMALIZE_XML_TAG_MODEL))
		{
			ParseModelEffectInfo(pAttElement, pXml, pEffectInfo);
		}

		// 카메라
		if(!_stricmp(pAttElement->Value(), EFFECT_NORMALIZE_XML_TAG_CAMERA))
		{
			ParseCameraEffectInfo(pAttElement, pXml, pEffectInfo);
		}

		// 효과
		if(!_stricmp(pAttElement->Value(), EFFECT_NORMALIZE_XML_TAG_POSTEFFECT))
		{
			ParsePostEffectInfo(pAttElement, pXml, pEffectInfo);
		}

		// 데칼
		if(!_stricmp(pAttElement->Value(), EFFECT_NORMALIZE_XML_TAG_DECAL))
		{
			ParseDecalEffectInfo(pAttElement, pXml, pEffectInfo);
		}

		// Illumination
		if(!_stricmp(pAttElement->Value(), EFFECT_NORMALIZE_XML_TAG_ILLUMINATION))
		{
			ParseIlluminationEffectInfo(pAttElement, pXml, pEffectInfo);
		}

		// 라이트
		if(!_stricmp(pAttElement->Value(), EFFECT_NORMALIZE_XML_TAG_EFFECTLIGHT))
		{
			ParseLightEffectInfo(pAttElement, pXml, pEffectInfo);
		}

		// AlphaTestFade
		if(!_stricmp(pAttElement->Value(), EFFECT_NORMALIZE_XML_TAG_ALPHATESTFADELIGHT))
		{
			ParseAlphaTestFadeEffectInfo(pAttElement, pXml, pEffectInfo);
		}
	}
}

void XEffectInfoMgr::ParseSoundEffectInfo( MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo )
{
	XEffectInfoItem_Sound * pSoundEvent = new XEffectInfoItem_Sound;

	pSoundEvent->eEffectType = CET_SOUND;
	_Attribute(pSoundEvent->strResourceName,		pElement, EFFECT_NORMALIZE_XML_ATTR_FILENAME);
	_Attribute(pSoundEvent->strSoundBoneName,		pElement, EFFECT_NORMALIZE_XML_ATTR_BONENAME);
	_Attribute(&(pSoundEvent->fDurationTime),		pElement, EFFECT_NORMALIZE_XML_ATTR_DURATIONTIME);

	pEffectInfo->m_vecEffectInfoItem.push_back(pSoundEvent);
}

void XEffectInfoMgr::ParseModelEffectInfo( MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo )
{
	XEffectInfoItem_Model * pModelEvent = new XEffectInfoItem_Model;

	pModelEvent->eEffectType = CET_MODEL;
	_Attribute(pModelEvent->strResourceName,		pElement, EFFECT_NORMALIZE_XML_ATTR_FILENAME);
	_Attribute(pModelEvent->strModelBoneName,		pElement, EFFECT_NORMALIZE_XML_ATTR_BONENAME);
	_Attribute(&(pModelEvent->bFollow),				pElement, EFFECT_NORMALIZE_XML_ATTR_FOLLOW);
	_Attribute(pModelEvent->strResourceName,		pElement, EFFECT_NORMALIZE_XML_ATTR_FILENAME);
	_Attribute(&(pModelEvent->bTerrainAlign),		pElement, EFFECT_NORMALIZE_XML_ATTR_TERRAIN_ALIGN);
	_Attribute(&(pModelEvent->bTerrainPos),			pElement, EFFECT_NORMALIZE_XML_ATTR_TERRAIN_POS);
	_Attribute(&(pModelEvent->bAlign),				pElement, EFFECT_NORMALIZE_XML_ATTR_ALIGN);
	_Attribute(&(pModelEvent->bAutoScale),			pElement, EFFECT_NORMALIZE_XML_ATTR_AUTOSCALE);
	_Attribute(&(pModelEvent->fDurationTime),		pElement, EFFECT_NORMALIZE_XML_ATTR_DURATIONTIME);
	_Attribute(&(pModelEvent->fStartVisibilityTime),pElement, EFFECT_NORMALIZE_XML_ATTR_START_VISIBILITY_TIME);
	_Attribute(&(pModelEvent->fEndVisibilityTime),	pElement, EFFECT_NORMALIZE_XML_ATTR_END_VISIBILITY_TIME);
	_Attribute(&(pModelEvent->bLastFramePause),		pElement, EFFECT_NORMALIZE_XML_ATTR_END_LAST_FRAME_PAUSE);
	_Attribute(&(pModelEvent->fx),					pElement, EFFECT_NORMALIZE_XML_ATTR_X);
	_Attribute(&(pModelEvent->fy),					pElement, EFFECT_NORMALIZE_XML_ATTR_Y);
	_Attribute(&(pModelEvent->fz),					pElement, EFFECT_NORMALIZE_XML_ATTR_Z);

	float fScale = 1.0f;
	_Attribute(&fScale,								pElement, EFFECT_NORMALIZE_XML_ATTR_SCALE);
	pModelEvent->vScale = vec3(fScale, fScale, fScale);

	pEffectInfo->m_vecEffectInfoItem.push_back(pModelEvent);
}

void XEffectInfoMgr::ParseCameraEffectInfo( MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo )
{
	XEffectInfoItem_Camera * pCameraEvent = new XEffectInfoItem_Camera;

	pCameraEvent->eEffectType = CET_CAMERA;
	_Attribute(pCameraEvent->strResourceName,		pElement, EFFECT_NORMALIZE_XML_ATTR_FILENAME);
	_Attribute(&(pCameraEvent->nDistance),			pElement, EFFECT_NORMALIZE_XML_ATTR_DISTANCE);
	_Attribute(&(pCameraEvent->fDurationTime),		pElement, EFFECT_NORMALIZE_XML_ATTR_DURATIONTIME);

	pEffectInfo->m_vecEffectInfoItem.push_back(pCameraEvent);
}

XEffectInfo * XEffectInfoMgr::GetEffectInfo( tstring& strEffectName )
{
	for(vector<XEffectInfo *>::iterator it = m_vecEffectInfo.begin(); it != m_vecEffectInfo.end(); it++)
	{
		if((*it)->m_strEffectName == strEffectName)
		{
			return (*it);
		}
	}

	return NULL;
}

void XEffectInfoMgr::ParsePostEffectInfo( MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo )
{
	XEffectInfoItem_PostEffect * pPostEffectEvent = new XEffectInfoItem_PostEffect;

	pPostEffectEvent->eEffectType = CET_POSTEFFECT;

	string strPostEffectType;
	_Attribute(strPostEffectType,					pElement, EFFECT_NORMALIZE_XML_ATTR_POSTEFFECT_TYPE);
	if (strPostEffectType == EFFECT_NORMALIZE_XML_VALUE_MOTIONBLUR)
		pPostEffectEvent->nPostEffectType = PEK_MOTION_BLUR;
	else if(strPostEffectType == EFFECT_NORMALIZE_XML_VALUE_OBJECTMOTIONBLUR)
		pPostEffectEvent->nPostEffectType = PEK_OBJECT_MOTION_BLUR;
	else if(strPostEffectType == EFFECT_NORMALIZE_XML_VALUE_OBJECTACCUMBLUR)
		pPostEffectEvent->nPostEffectType = PEK_OBJECT_ACCUM_BLUR;
	else if(strPostEffectType == EFFECT_NORMALIZE_XML_VALUE_PIXELOBJECTMOTIONBLUR)
		pPostEffectEvent->nPostEffectType = PEK_PIXEL_OBJECT_MOTION_BLUR;

	_Attribute(pPostEffectEvent->fPlayTime,			pElement, EFFECT_NORMALIZE_XML_ATTR_PLAYTIME);
	_Attribute(pPostEffectEvent->fScatTime,			pElement, EFFECT_NORMALIZE_XML_ATTR_SCATTIME);
	_Attribute(pPostEffectEvent->fSpeed,			pElement, EFFECT_NORMALIZE_XML_ATTR_SPEED);
	_Attribute(pPostEffectEvent->nDistance,			pElement, EFFECT_NORMALIZE_XML_ATTR_DISTANCE);

	string strScarFactortType;
	_Attribute(strScarFactortType,					pElement, EFFECT_NORMALIZE_XML_ATTR_SCAREFCTORTYPE, "SAT_NORMAL");
	if(strScarFactortType == EFFECT_NORMALIZE_XML_VALUE_SAT_NORMAL)
		pPostEffectEvent->nScarFactorType = PESAT_NORMAL;
	else if(strScarFactortType == EFFECT_NORMALIZE_XML_VALUE_SAT_FAST_SCAT)
		pPostEffectEvent->nScarFactorType = PESAT_FAST_SCAT;
	else if(strScarFactortType == EFFECT_NORMALIZE_XML_VALUE_SAT_SLOW_SCAT)
		pPostEffectEvent->nScarFactorType = PESAT_SLOW_SCAT;
	else if(strScarFactortType == EFFECT_NORMALIZE_XML_VALUE_SAT_GATTER)
		pPostEffectEvent->nScarFactorType = PESAT_GATTER;

	string strUseType;
	_Attribute(strUseType,							pElement, EFFECT_NORMALIZE_XML_ATTR_USETYPE);
	if(strUseType == EFFECT_NORMALIZE_XML_VALUE_MYSELF)
		pPostEffectEvent->nUseType = PEUT_MYSELF;
	else if(strUseType == EFFECT_NORMALIZE_XML_VALUE_ALL)
		pPostEffectEvent->nUseType = PEUT_ALL;

	string strBlurMode;
	_Attribute(strBlurMode,							pElement, EFFECT_NORMALIZE_XML_ATTR_MOTION_BLUR_MODE);
	if(strBlurMode == EFFECT_NORMALIZE_XML_VALUE_HALF)
		pPostEffectEvent->nBlurType = PBM_HALF;
	if(strBlurMode == EFFECT_NORMALIZE_XML_VALUE_FULL)
		pPostEffectEvent->nBlurType = PBM_FULL;

	pEffectInfo->m_vecEffectInfoItem.push_back(pPostEffectEvent);
}

void XEffectInfoMgr::ParseDecalEffectInfo( MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo )
{
	XEffectInfoItem_DecalEffect * pDecalEffect = new XEffectInfoItem_DecalEffect;

	pDecalEffect->eEffectType = CET_DECALEFFECT;

	string strDecalEffectType;
	_Attribute(strDecalEffectType,					pElement, EFFECT_NORMALIZE_XML_ATTR_POSTEFFECT_TYPE);
	if(strDecalEffectType == EFFECT_NORMALIZE_XML_VALUE_PROJECTION_DECAL)
		pDecalEffect->nDecalEffectType = DET_PROJECTION;
	else if(strDecalEffectType == EFFECT_NORMALIZE_XML_VALUE_POLYGON_DECAL)
		pDecalEffect->nDecalEffectType = DET_POLYGON;

	_Attribute(pDecalEffect->strResourceName,		pElement, EFFECT_NORMALIZE_XML_ATTR_FILENAME);
	_Attribute(pDecalEffect->strModelBoneName,		pElement, EFFECT_NORMALIZE_XML_ATTR_BONENAME);
	_Attribute(&(pDecalEffect->fSizeX),				pElement, EFFECT_NORMALIZE_XML_ATTR_SIZE_X);
	_Attribute(&(pDecalEffect->fSizeY),				pElement, EFFECT_NORMALIZE_XML_ATTR_SIZE_Y);
	_Attribute(&(pDecalEffect->bFollow),			pElement, EFFECT_NORMALIZE_XML_ATTR_FOLLOW);
	_Attribute(&(pDecalEffect->fDurationTime),		pElement, EFFECT_NORMALIZE_XML_ATTR_DURATIONTIME);
	_Attribute(&(pDecalEffect->fStartVisibilityTime),pElement, EFFECT_NORMALIZE_XML_ATTR_START_VISIBILITY_TIME);
	_Attribute(&(pDecalEffect->fEndVisibilityTime),	pElement, EFFECT_NORMALIZE_XML_ATTR_END_VISIBILITY_TIME);

	pEffectInfo->m_vecEffectInfoItem.push_back(pDecalEffect);
}

void XEffectInfoMgr::ParseIlluminationEffectInfo(MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo)
{
	XEffectInfoItem_IlluminationEffect * pIlluminationEffect = new XEffectInfoItem_IlluminationEffect;

	pIlluminationEffect->eEffectType = CET_ILLUMINATION;

	_Attribute(pIlluminationEffect->strModelActorNodeName,	pElement, EFFECT_NORMALIZE_XML_ATTR_ACTORNODENAME);
	_Attribute(&(pIlluminationEffect->fStartTime),			pElement, EFFECT_NORMALIZE_XML_ATTR_START_TIME);
	_Attribute(&(pIlluminationEffect->fStartDurationTime),	pElement, EFFECT_NORMALIZE_XML_ATTR_START_DURATION_TIME);
	_Attribute(&(pIlluminationEffect->fEndTime),			pElement, EFFECT_NORMALIZE_XML_ATTR_END_TIME);
	_Attribute(&(pIlluminationEffect->fEndDurationTime),	pElement, EFFECT_NORMALIZE_XML_ATTR_END_DURATION_TIME);
	_Attribute(&(pIlluminationEffect->fDurationTime),		pElement, EFFECT_NORMALIZE_XML_ATTR_DURATIONTIME);
	_Attribute(&(pIlluminationEffect->fMin),				pElement, EFFECT_NORMALIZE_XML_ATTR_MIN);
	_Attribute(&(pIlluminationEffect->fMax),				pElement, EFFECT_NORMALIZE_XML_ATTR_MAX);
	_Attribute(&(pIlluminationEffect->bLoop),				pElement, EFFECT_NORMALIZE_XML_ATTR_LOOP);

	pEffectInfo->m_vecEffectInfoItem.push_back(pIlluminationEffect);
}

void XEffectInfoMgr::ParseLightEffectInfo( MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo )
{
	XEffectInfoItem_LightEffect * pLightEffect = new XEffectInfoItem_LightEffect;

	pLightEffect->eEffectType = CET_LIGHTEFFECT;

	_Attribute(pLightEffect->strModelBoneName,		pElement, EFFECT_NORMALIZE_XML_ATTR_BONENAME);
	_Attribute(&(pLightEffect->fRadius),			pElement, EFFECT_NORMALIZE_XML_ATTR_RADIUS);
	_Attribute(&(pLightEffect->fDurationTime),		pElement, EFFECT_NORMALIZE_XML_ATTR_DURATIONTIME);
	_Attribute(&(pLightEffect->fAttenuationStart),	pElement, EFFECT_NORMALIZE_XML_ATTR_ATTENUATION_START);
	_Attribute(&(pLightEffect->fIntensity),			pElement, EFFECT_NORMALIZE_XML_ATTR_INTENSITY);
	_Attribute(&(pLightEffect->bFollow),			pElement, EFFECT_NORMALIZE_XML_ATTR_FOLLOW);
	_Attribute(&(pLightEffect->bAttackLight),		pElement, EFFECT_NORMALIZE_XML_ATTR_ATTACKLIGHT);

	int nR, nG, nB;
	nR = nG = nB = 255;
	_Attribute(&nR,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_R);
	_Attribute(&nG,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_G);
	_Attribute(&nB,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_B);
	pLightEffect->dwColor = D3DCOLOR_ARGB(255, nR, nG, nB);

	pEffectInfo->m_vecEffectInfoItem.push_back(pLightEffect);
};

void XEffectInfoMgr::ParseAlphaTestFadeEffectInfo( MXmlElement* pElement, MXml* pXml, XEffectInfo* pEffectInfo )
{
	XEffectInfoItem_AlphaTestFadeEffect * pFadeEffect = new XEffectInfoItem_AlphaTestFadeEffect;

	pFadeEffect->eEffectType = CET_ALPHATESTFADE;

	tstring strMeshNodeName;
	_Attribute(strMeshNodeName,						pElement, EFFECT_NORMALIZE_XML_ATTR_MESHNODENAME);
	GetCommaStringSplitter(strMeshNodeName, pFadeEffect->vecstrMeshNodeName);


	_Attribute(&(pFadeEffect->fStartDurationTime),	pElement, EFFECT_NORMALIZE_XML_ATTR_START_DURATION_TIME);
	_Attribute(&(pFadeEffect->fEndDurationTime),	pElement, EFFECT_NORMALIZE_XML_ATTR_END_DURATION_TIME);

	int nR, nG, nB;
	nR = nG = nB = 0;
	_Attribute(&nR,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_1_R);
	_Attribute(&nG,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_1_G);
	_Attribute(&nB,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_1_B);
	pFadeEffect->dwFadeEffectColor1 = D3DCOLOR_ARGB(0, nR, nG, nB);

	nR = nG = nB = 0;
	_Attribute(&nR,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_2_R);
	_Attribute(&nG,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_2_G);
	_Attribute(&nB,									pElement, EFFECT_NORMALIZE_XML_ATTR_COLOR_2_B);
	pFadeEffect->dwFadeEffectColor2 = D3DCOLOR_ARGB(0, nR, nG, nB);

	_Attribute(&(pFadeEffect->fFadeEffectSize1),	pElement, EFFECT_NORMALIZE_XML_ATTR_SIZE_1);
	_Attribute(&(pFadeEffect->fFadeEffectSize2),	pElement, EFFECT_NORMALIZE_XML_ATTR_SIZE_2);

	_Attribute(&(pFadeEffect->bInverse),			pElement, EFFECT_NORMALIZE_XML_ATTR_INVERSE);

	pEffectInfo->m_vecEffectInfoItem.push_back(pFadeEffect);
}

bool XEffectInfoMgr::Reload( tstring strFileName )
{
	Destroy();

	return Load(strFileName);
}

void XEffectInfoMgr::Destroy()
{
	for(vector<XEffectInfo *>::iterator it = m_vecEffectInfo.begin(); it != m_vecEffectInfo.end(); it++)
	{
		SAFE_DELETE(*it);
	}

	m_vecEffectInfo.clear();
}
