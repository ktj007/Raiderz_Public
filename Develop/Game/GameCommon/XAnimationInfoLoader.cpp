#include "stdafx.h"
#include "XAnimationInfoLoader.h"
#include "XAnimationBlendInfo.h"
#include "XOverlayAnimationInfo.h"
#include "XAnimationHitSoundInfo.h"
#include "XReActionAnimationInfo.h"
#include "MLocale.h"

XAnimationInfoLoader::XAnimationInfoLoader()
{

}

XAnimationInfoLoader::~XAnimationInfoLoader()
{

}

bool XAnimationInfoLoader::LoadXml( tstring strFileName, XAnimationBlendInfo* pBlendInfo, XOverlayAnimationInfo* pOverlayInfo, XAnimationHitSoundInfo* pHitSoundInfo, XReActionAnimationInfo* pReActionInfo )
{
	MXml xml;

	if (!xml.LoadFile(MLocale::ConvTCHARToAnsi(strFileName.c_str()).c_str())) 
	{
		// 파일이 없을 수 있다.
		//mlog("Failed To Load %s\n", strFileName.c_str());
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(ANIMATION_INFO_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if(!_stricmp(pElement->Value(), ANIMATION_INFO_XML_TAG_ANIMATION))
		{
			tstring strAniName;
			_Attribute( strAniName,		pElement, ANIMATION_INFO_XML_ATTR_ANIMATION_NAME );
	
			MXmlElement* pAttElement = pElement->FirstChildElement();
			for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
			{
				if (!_stricmp(pAttElement->Value(), ANIMATION_BLEND_INFO_XML_TAG_TRANSITION))
				{
					ParseBlendInfo(pAttElement, &xml, strAniName, pBlendInfo);
				}
				else if (!_stricmp(pAttElement->Value(), ANIMATION_OVERLAY_XML_TAG_OVAERLAYANIMATION))
				{
					ParseOverlayInfo(pAttElement, &xml, strAniName, pOverlayInfo);
				}
				else if(!_stricmp(pAttElement->Value(), ANIMATION_HIT_SOUND_XML_TAG_HITSOUND))
				{
					ParseHitSoundInfo(pAttElement, &xml, strAniName, pHitSoundInfo);
				}
				else if(!_stricmp(pAttElement->Value(), ANIMATION_REACTION_XML_TAG_REACTIONANIMATION))
				{
					ParseReActionInfo(pAttElement, &xml, strAniName, pReActionInfo);
				}
			}
		}
	}

	return true;
}

void XAnimationInfoLoader::ParseOverlayInfo( MXmlElement* pElement, MXml* pXml, tstring& strAniName, XOverlayAnimationInfo* pOverlayInfo )
{
	OVERLAY_ANIMATION_LIST overlayList;
	overlayList.strBaseAnimation = strAniName;

	OVERLAY_ANIMATION_INFO aniInfo;

	_Attribute(aniInfo.strOverlayAnimation,			pElement, ANIMATION_OVERLAY_XML_ATTR_NAME);
	_Attribute(aniInfo.strBoneName,					pElement, ANIMATION_OVERLAY_XML_ATTR_BONE_NAM);

	overlayList.vecOverlayAnimationList.push_back(aniInfo);

	pOverlayInfo->Set(strAniName, overlayList);
}

void XAnimationInfoLoader::ParseBlendInfo( MXmlElement* pElement, MXml* pXml, tstring& strAniName, XAnimationBlendInfo* pBlendInfo )
{
	tstring targetBuff;
	_Attribute( targetBuff,		pElement, ANIMATION_BLEND_INFO_XML_ATTR_TARGET );

	float fDuration = BLEND_DATA::DEFAULT_BLEND_DURATION;
	_Attribute( &fDuration,		pElement, ANIMATION_BLEND_INFO_XML_ATTR_DURATION );

	// 0초는 읽지 않습니다.
	if(fDuration == 0.0f)
		return;

	BLEND_DATA blendData( fDuration );
	if( pBlendInfo->IsNotDefaultValue( blendData ) )
	{
		pBlendInfo->SetAnimationTransData( strAniName, targetBuff, fDuration );
	}
}

void XAnimationInfoLoader::ParseHitSoundInfo( MXmlElement* pElement, MXml* pXml, tstring& strAniName, XAnimationHitSoundInfo* pHitSoundInfo )
{
	tstring strSoundName;
	_Attribute( strSoundName,		pElement, ANIMATION_HIT_SOUND_XML_ATTR_SOUND_NAME );

	pHitSoundInfo->Set(strAniName, strSoundName);
}

void XAnimationInfoLoader::ParseReActionInfo( MXmlElement* pElement, MXml* pXml, tstring& strAniName, XReActionAnimationInfo* pReActionInfo )
{
	tstring strReActionName;
	_Attribute( strReActionName,		pElement, ANIMATION_REACTION_XML_ATTR_REACTION_NAME );

	ReAction_DATA reAction_Data;
	reAction_Data.strReActionName = strReActionName;

	_Attribute( &reAction_Data.fWeight,		pElement, ANIMATION_REACTION_XML_ATTR_WEIGHT );
	_Attribute( &reAction_Data.fSpeed,		pElement, ANIMATION_REACTION_XML_ATTR_SPEED );
	_Attribute( reAction_Data.strWeaponBoneName,	pElement, ANIMATION_REACTION_XML_WEAPON_BONE_NAME );
	_Attribute( &reAction_Data.bDisable,	pElement, ANIMATION_REACTION_XML_ATTR_DISABLE );

	pReActionInfo->Set(strAniName, reAction_Data);
}
