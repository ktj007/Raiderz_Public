#include "StdAfx.h"
#include "GTrainingInfoMgr.h"
#include "GTrainingInfo.h"
#include "GTalentInfo.h"
#include "GConditionsInfoMgr.h"
#include "GDialogInfoMgr.h"
#include "GValidateLogger.h"

GTrainingInfoMgr::GTrainingInfoMgr()
{
}

GTrainingInfoMgr::~GTrainingInfoMgr()
{
	Clear();
}

void GTrainingInfoMgr::Clear()
{
	for_each(m_mapTrainingInfo.begin(), m_mapTrainingInfo.end(), DPSO());
	m_mapTrainingInfo.clear();
}

bool GTrainingInfoMgr::Load(const char* szFileName)
{
	_ASSERT(NULL != szFileName);
	if (NULL == szFileName) return false;

	_VLI(FILENAME_TRAINING);

	MXml xml;
	if (!xml.LoadFile(szFileName)) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(TRAINING_XML_TAG_MAIET).FirstChildElement().Element();

	for (pElement; pElement != NULL; pElement = pElement->NextSiblingElement())
	{
		if (!_stricmp(pElement->Value(), TRAINING_XML_TAG_TRAINING))
		{
			ParseTraining(pElement, &xml);
		}
	}

	return true;
}

void GTrainingInfoMgr::ParseTraining(MXmlElement* pElement, MXml* pXml)
{
	int nID;
	_Attribute(&nID, pElement, TRAINING_XML_ATTR_ID);

	_VLE(TRAINING_XML_TAG_TRAINING);
	_VLA(TRAINING_XML_ATTR_ID, nID);
	_VLP;

	GTrainingInfo* pExistInfo = Get(nID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo)
	{
		_VLOGGER->Log(TRAINING_XML_ATTR_ID IS_ALREADY_EXIST);
		return;
	}

	GTrainingInfo* pNewInfo = new GTrainingInfo();
	pNewInfo->m_nID = nID;

	int nDialogID;
	_Attribute(&nDialogID,	pElement, TRAINING_XML_ATTR_DIALOG);
	pNewInfo->m_pDialogInfo = gmgr.pDialogInfoMgr->Get(nDialogID);
	_ASSERT(NULL != pNewInfo->m_pDialogInfo);
	if (NULL == pNewInfo->m_pDialogInfo)
	{
		_VLOGGER->Log(TRAINING_XML_ATTR_DIALOG, IS_NOT_IN_DIALOG);
	}

	string strValue;
	if (true == _Attribute(strValue,	pElement, TRAINING_XML_ATTR_TYPE))
	{
		int nTrainingType = GStrings::StringToTrainingType(strValue.c_str());
		_VLOGGER->ValidateTrainingType(nTrainingType, TRAINING_XML_ATTR_TYPE);
		pNewInfo->nTrainingType = static_cast<TRAINING_TYPE>(nTrainingType);
	}

	MXmlElement* pChild = pElement->FirstChildElement();
	for (pChild; pChild != NULL; pChild = pChild->NextSiblingElement())
	{
		if (!_stricmp(pChild->Value(), TRAINING_XML_TAG_TALENT))
		{
			ParseTalent(pChild, pXml, pNewInfo);
		}
	}

	Insert(pNewInfo);
}

void GTrainingInfoMgr::ParseTalent(MXmlElement* pElement, MXml* pXml, GTrainingInfo* pTrainingInfo)
{
	string strValue;
	int nConditionsID;

	vector<int> vecTalentID;
	if (true == _Attribute(strValue,		pElement, TRAINING_XML_ATTR_ID))
	{		
		GStrings::StringNumbersToIntVector(strValue, vecTalentID);
	}

	_VLE(TRAINING_XML_TAG_TALENT);
	_VLA(TRAINING_XML_ATTR_ID, strValue);
	_VLP;

	if (true == _Attribute(&nConditionsID,	pElement, TRAINING_XML_ATTR_CONDITION))
	{
		GConditionsInfo* pConditionsInfo = gmgr.pCondtionsInfoMgr->Get(nConditionsID);
		if (NULL == pConditionsInfo)
		{
			_VLOGGER->Log("invalid condition id.\n");
			return;
		}

		for each (int nTalentID in vecTalentID)
		{
			GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
			_ASSERT(NULL != pTalentInfo);
			if (NULL == pTalentInfo)
			{
				_VLOGGER->Log(TRAINING_XML_ATTR_ID IS_NOT_IN_TALENT);
				continue;
			}

			pTrainingInfo->m_mapConditionsTalent.insert(map<GTalentInfo*, GConditionsInfo*>::value_type(pTalentInfo, pConditionsInfo));
		}		
	}
	else
	{
		for each (int nTalentID in vecTalentID)
		{
			GTalentInfo* pTalentInfo = gmgr.pTalentInfoMgr->Find(nTalentID);
			if (NULL == pTalentInfo)
			{
				_VLOGGER->Log(TRAINING_XML_ATTR_ID IS_NOT_IN_TALENT);
				continue;
			}

			pTrainingInfo->m_setNormalTalent.insert(pTalentInfo);
		}
	}
}

void GTrainingInfoMgr::Insert( GTrainingInfo* pTrainingInfo )
{
	_ASSERT(NULL != pTrainingInfo);
	if (NULL == pTrainingInfo) return;

	GTrainingInfo* pExistInfo = Get(pTrainingInfo->m_nID);
	_ASSERT(NULL == pExistInfo);
	if (NULL != pExistInfo) return;

	m_mapTrainingInfo.insert(MAP_TRAININGINFO::value_type(pTrainingInfo->m_nID, pTrainingInfo));
}

GTrainingInfo* GTrainingInfoMgr::Get( int nID )
{
	if (true == m_mapTrainingInfo.empty()) return NULL;

	MAP_TRAININGINFO::iterator itor = m_mapTrainingInfo.find(nID);
	if (m_mapTrainingInfo.end() == itor) return NULL;

	return (*itor).second;
}