#pragma once
#ifndef _XSWORDTRAILSAMPLINGDATA_H
#define _XSWORDTRAILSAMPLINGDATA_H

#include "MXml.h"
#include "RTypes.h"
#include "MTstring.h"

#define SWORD_TRAIL_XML_TAG_MAIET			"maiet"
#define SWORD_TRAIL_MODEL					"Model"
#define SWORD_TRAIL_DATA					"Data"
#define SWORD_TRAIL_SAMPLINGDATA			"SamplingData"
#define SWORD_TRAIL_ANIMATION_NAME			"animation_name"
#define SWORD_TRAIL_START_FRAME				"startFrame"
#define SWORD_TRAIL_END_FRAME				"endFrame"
#define SWORD_TRAIL_FRAME_GAP				"frameGap"
#define SWORD_TRAIL_SAMPLING_MATRIX			"mat"
#define SWORD_TRAIL_NAME					"name"

struct SWORDTRAILEVENT_DATA
{
	int						nStartFrame;
	int						nEndFrame;
	int						nFrameGap;

	tstring					strParentsBoneName;		// 툴에서 사용

	void init()
	{
		nStartFrame			= -1;
		nEndFrame			= -1;
		nFrameGap			= 0;

		strParentsBoneName.clear();
	}

	SWORDTRAILEVENT_DATA()
	{
		init();
	}
};

struct SWORDTRAILSAMPLING_DATA
{
	int						nStartFrame;
	int						nEndFrame;
	int						nFrameGap;

	vector<rs3::RMatrix>			vecMatrix;

	SWORDTRAILSAMPLING_DATA()
	{
		nStartFrame			= 0;
		nEndFrame			= 0;
		nFrameGap			= 0;
		vecMatrix.clear();
	}
};

struct SWORDTRAIL_DATA
{
	tstring									strAnimationName;
	vector<SWORDTRAILSAMPLING_DATA>			vecSamplingData;
};

struct SWORDTRAIL_MODEL
{
	tstring									strModelName;
	map<tstring, SWORDTRAIL_DATA>			mapSwordTrailSamplingData;
};

class XSwordTrailSamplingData
{
protected:
	map<tstring, SWORDTRAIL_MODEL> m_mapSwordTrailSamplingModel;

private:
	void						ParseSwordTrailData(MXmlElement* pElement, MXml* pXml, SWORDTRAIL_MODEL& swordTrail_model);
	void						ParseSwordTrailSamplingData(MXmlElement* pElement, MXml* pXml, SWORDTRAIL_DATA& swordTrail_data);

public:
	XSwordTrailSamplingData();
	~XSwordTrailSamplingData();

	bool						Load(tstring strFileName);

	SWORDTRAIL_DATA*			Get(tstring strModelName, tstring strAnimationName);
};

#endif