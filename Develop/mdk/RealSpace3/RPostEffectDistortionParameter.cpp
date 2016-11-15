#include "StdAfx.h"

#include "RPostEffectDistortionParameter.h"
#include "RXMLTemplate.h"

namespace rs3 {

RDistortionParameter::RDistortionParameter()
{
	fDir0X = -0.05f;
	fDir0Y = 0.02f;
	fDir1X = 0.03f;
	fDir1Y = 0.014f;
	fScale =0.02f;
	fSpeed0 =0.1f;
	fSpeed1 =0.1f;
	bRender = true;
	strcpy_s(strNormalMapFileName, DISTORTION_DEFAUL_NORMALMAP);
	pNormalMap = NULL;
	fRange =0.0f;
	fVisibility =1.0f;
	fAttenuationBegin =0.0f;
}

RDistortionParameter::~RDistortionParameter()
{
	REngine::GetDevice().DeleteTexture(pNormalMap);
}

RDistortionParameter& RDistortionParameter::operator=(const RDistortionParameter& param)
{
	fDir0X = param.fDir0X;
	fDir0Y = param.fDir0Y;
	fDir1X = param.fDir1X;
	fDir1Y = param.fDir1Y;
	fScale = param.fScale;
	fSpeed0 = param.fSpeed0;
	fSpeed1 = param.fSpeed1;
	bRender = param.bRender;
	strcpy_s(strNormalMapFileName, param.strNormalMapFileName);

	if( param.pNormalMap==NULL )
		pNormalMap =NULL;
	else
		LoadNormalMap(false);

	fRange = param.fRange;
	fVisibility = param.fVisibility;
	fAttenuationBegin = param.fAttenuationBegin;

	return *this;
}

void RDistortionParameter::LoadNormalMap(bool bBackground)
{
	if( strcmp(strNormalMapFileName, DISTORTION_DEFAUL_NORMALMAP) != 0 )
	{
		_ASSERT( NULL == pNormalMap );
		pNormalMap = REngine::GetDevice().CreateTextureForBuildLater(strNormalMapFileName, RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE, RFMT_NONE, bBackground);		
	}
}

void RDistortionParameter::ReplaceNormalMap()
{
	if( strcmp(strNormalMapFileName, DISTORTION_DEFAUL_NORMALMAP) != 0 )
	{
		REngine::GetDevice().DeleteTexture(pNormalMap);
		pNormalMap=REngine::GetDevice().CreateTexture( strNormalMapFileName, RTF_LINEAR, RCT_AUTOGENMIPMAP | RCT_UNREDUCIBLE );
	}		
}

void RDistortionParameter::CreateToXML(MXmlElement &element)
{
	using namespace TXML;

	if( MXmlElement *pDistortionAttribute = element.FirstChildElement("DISTORTION_ATTRIBUTE") )
	{
		fDir0X = _GETElement<float>::Value(pDistortionAttribute,"DIRECTION0_X","float");
		fDir0Y = _GETElement<float>::Value(pDistortionAttribute,"DIRECTION0_Y","float");
		fDir1X = _GETElement<float>::Value(pDistortionAttribute,"DIRECTION1_X","float");
		fDir1Y = _GETElement<float>::Value(pDistortionAttribute,"DIRECTION1_Y","float");
		fScale = _GETElement<float>::Value(pDistortionAttribute,"SCALE","float");
		fSpeed0 = _GETElement<float>::Value(pDistortionAttribute,"SPEED0","float");
		fSpeed1 = _GETElement<float>::Value(pDistortionAttribute,"SPEED1","float");
		bRender = _GETElement<bool>::Value(pDistortionAttribute,"RENDERABLE","bool");
		_Contents(strNormalMapFileName, pDistortionAttribute->FirstChildElement("NORMALMAP"));
		fRange = _GETElement<float>::Value(pDistortionAttribute,"RANGE","float");
		fAttenuationBegin = _GETElement<float>::Value(pDistortionAttribute,"ATTENUATIONBEGIN","float");
	}		
}

void RDistortionParameter::SaveToXML(MXmlElement &element)
{
	using namespace TXML;

	MXmlElement *pDistortionElement = new MXmlElement("DISTORTION_ATTRIBUTE");
	element.LinkEndChild(pDistortionElement);
	_SetElementValue(pDistortionElement,_T("DIRECTION0_X"), _T("float"), fDir0X);
	_SetElementValue(pDistortionElement,_T("DIRECTION0_Y"), _T("float"), fDir0Y);
	_SetElementValue(pDistortionElement,_T("DIRECTION1_X"), _T("float"), fDir1X);
	_SetElementValue(pDistortionElement,_T("DIRECTION1_Y"), _T("float"), fDir1Y);
	_SetElementValue(pDistortionElement,_T("SCALE"), _T("float"), fScale);
	_SetElementValue(pDistortionElement,_T("SPEED0"), _T("float"), fSpeed0);
	_SetElementValue(pDistortionElement,_T("SPEED1"), _T("float"), fSpeed1);
	_SetElementValue(pDistortionElement,_T("RENDERABLE"), _T("bool"), bRender);
	_SetElementValue(pDistortionElement,_T("NORMALMAP"), _T("string"), strNormalMapFileName);
	_SetElementValue(pDistortionElement,_T("RANGE"), _T("float"), fRange);
	_SetElementValue(pDistortionElement,_T("ATTENUATIONBEGIN"), _T("float"), fAttenuationBegin);
}
	
}