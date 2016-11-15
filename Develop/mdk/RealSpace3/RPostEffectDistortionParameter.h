#pragma once

#include "MXml.h"

namespace rs3 {

#define DISTORTION_DEFAUL_NORMALMAP "Distortion_nm.dds"

class RTexture;

class RS_API RDistortionParameter
{
public:
	RDistortionParameter();
	~RDistortionParameter();

	void LoadNormalMap(bool bBackground);
	void ReplaceNormalMap();
	void CreateToXML(MXmlElement &element);
	void SaveToXML(MXmlElement &element);

	RDistortionParameter& operator=(const RDistortionParameter& param);

	float fDir0X;
	float fDir0Y;
	float fDir1X;
	float fDir1Y;
	float fScale;
	float fSpeed0;
	float fSpeed1;
	bool bRender;
	char strNormalMapFileName[50];
	RTexture* pNormalMap;
	float fRange;
	float fVisibility;
	float fAttenuationBegin;
};

}