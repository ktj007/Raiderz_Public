#include "stdafx.h"
#include "XSwordTrailSamplingData.h"
#include "MLocale.h"

XSwordTrailSamplingData::XSwordTrailSamplingData()
{
	m_mapSwordTrailSamplingModel.clear();
}

XSwordTrailSamplingData::~XSwordTrailSamplingData()
{
	m_mapSwordTrailSamplingModel.clear();
}

bool XSwordTrailSamplingData::Load(tstring strFileName)
{
	MXml xml;
	if(!xml.LoadFile(MLocale::ConvTCHARToAnsi(strFileName.c_str()).c_str())) 
	{
		dlog("¿¡·¯ : Failed To Load swordtrailsampling.xml\n");
		return false;
	}

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(SWORD_TRAIL_XML_TAG_MAIET).FirstChildElement().Element();

	for(pElement; pElement != NULL; pElement=pElement->NextSiblingElement())
	{
		if(!_stricmp(pElement->Value(), SWORD_TRAIL_MODEL))
		{
			SWORDTRAIL_MODEL swordTail_Model;
			_Attribute(swordTail_Model.strModelName,	pElement, SWORD_TRAIL_NAME);

			ParseSwordTrailData(pElement, &xml, swordTail_Model);

			m_mapSwordTrailSamplingModel.insert(map<tstring, SWORDTRAIL_MODEL>::value_type(swordTail_Model.strModelName, swordTail_Model));
		}
	}

	return true;
}

void XSwordTrailSamplingData::ParseSwordTrailData( MXmlElement* pElement, MXml* pXml, SWORDTRAIL_MODEL& swordTrail_model )
{
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		if(!_stricmp(pAttElement->Value(), SWORD_TRAIL_DATA))
		{
			SWORDTRAIL_DATA swordTrail_data;
			_Attribute(swordTrail_data.strAnimationName,	pAttElement, SWORD_TRAIL_ANIMATION_NAME);

			ParseSwordTrailSamplingData(pAttElement, pXml, swordTrail_data);

			swordTrail_model.mapSwordTrailSamplingData.insert(map<tstring, SWORDTRAIL_DATA>::value_type(swordTrail_data.strAnimationName, swordTrail_data));
		}
	}
}

void XSwordTrailSamplingData::ParseSwordTrailSamplingData( MXmlElement* pElement, MXml* pXml, SWORDTRAIL_DATA& swordTrail_data )
{
	MXmlElement* pAttElement = pElement->FirstChildElement();
	for(pAttElement; pAttElement != NULL; pAttElement = pAttElement->NextSiblingElement())
	{
		SWORDTRAILSAMPLING_DATA sampling_data;

		if(!_stricmp(pAttElement->Value(), SWORD_TRAIL_SAMPLINGDATA))
		{
			_Attribute(&(sampling_data.nStartFrame),	pAttElement, SWORD_TRAIL_START_FRAME);
			_Attribute(&(sampling_data.nEndFrame),		pAttElement, SWORD_TRAIL_END_FRAME);
			_Attribute(&(sampling_data.nFrameGap),		pAttElement, SWORD_TRAIL_FRAME_GAP);

			MXmlElement* pAttElement2 = pAttElement->FirstChildElement();
			for(pAttElement2; pAttElement2 != NULL; pAttElement2 = pAttElement2->NextSiblingElement())
			{
				if(!_stricmp(pAttElement2->Value(), SWORD_TRAIL_SAMPLING_MATRIX))
				{
					MMatrix mat;
					mat.MakeZero();
					mat.MakeIdentity();
					char szBuffer[512] = "";
					_Contents(szBuffer, pAttElement2);

					sscanf_s(szBuffer,"%f %f %f %f %f %f %f %f %f %f %f %f", 
						&mat._11, &mat._12, &mat._13, 
						&mat._21, &mat._22, &mat._23,
						&mat._31, &mat._32, &mat._33,
						&mat._41, &mat._42, &mat._43);

					sampling_data.vecMatrix.push_back(mat);
				}
			}
		}

		swordTrail_data.vecSamplingData.push_back(sampling_data);
	}
}

SWORDTRAIL_DATA* XSwordTrailSamplingData::Get( tstring strModelName, tstring strAnimationName )
{
	map<tstring, SWORDTRAIL_MODEL>::iterator itFindModel = m_mapSwordTrailSamplingModel.find(strModelName);
	if(itFindModel != m_mapSwordTrailSamplingModel.end())
	{
		map<tstring, SWORDTRAIL_DATA>::iterator itFindData = itFindModel->second.mapSwordTrailSamplingData.find(strAnimationName);
		if(itFindData != itFindModel->second.mapSwordTrailSamplingData.end())
		{
			return &(itFindData->second);
		}
	}

	return NULL;
}
