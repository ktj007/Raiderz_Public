#include "stdafx.h"
#include "ZGameServerInfoLoader.h"
#include "ZGameServerInfoManager.h"
#include "MCsvParser.h"
#include "ZFieldInfoManager.h"
#include "ZServerInfoLoaderDef.h"
#include "MLocale.h"


bool ZGameServerInfoLoader::Load( ZGameServerInfoManager* mgr, const wchar_t* szFileName, ZFieldInfoManager* pFieldInfoManager )
{
	m_pManager = mgr;

	MXml xml;
	if (!xml.LoadFile(MLocale::ConvUTF16ToAnsi(szFileName).c_str())) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(SERVER_INFO_XML_TAG_MAIET).FirstChildElement().Element();

	if (Parse(pElement, pFieldInfoManager) == false)
		return false;

	RunPostfix(pFieldInfoManager);

	return true;
}

bool ZGameServerInfoLoader::LoadStream( ZGameServerInfoManager* mgr, const char* szStream, ZFieldInfoManager* pFieldInfoManager )
{
	m_pManager = mgr;

	MXml xml;
	if (!xml.LoadStream(szStream)) return false;

	MXmlHandle docHandle = xml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(SERVER_INFO_XML_TAG_MAIET).FirstChildElement().Element();

	if (Parse(pElement, pFieldInfoManager) == false)
		return false;

	RunPostfix(pFieldInfoManager);

	return true;
}

bool ZGameServerInfoLoader::Parse( MXmlElement* pElement, ZFieldInfoManager* pFieldInfoManager )
{
	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), SERVER_INFO_XML_TAG_GAME_SERVER))
		{
			int nID=0;
			if (_Attribute(&nID, pElement, SERVER_INFO_XML_ATTR_ID))
			{
				if (m_pManager->GetInfo(nID) != NULL)
				{
					return false;
				}

				ZGameServerInfo* pNewInfo = new ZGameServerInfo();
				pNewInfo->m_nID = nID;

				ParseGameServer(pNewInfo, pElement, pFieldInfoManager);

				if (m_pManager->Insert(pNewInfo) == false)
					return false;
			}
		}
	}

	return true;
}

void ZGameServerInfoLoader::ParseGameServer( ZGameServerInfo* pGameServerInfo, MXmlElement* pElement, ZFieldInfoManager* pFieldInfoManager )
{
	MXmlElement* pGameServerElement = pElement->FirstChildElement();
	for( pGameServerElement; pGameServerElement != NULL; pGameServerElement=pGameServerElement->NextSiblingElement() )
	{
		if (!_stricmp(pGameServerElement->Value(), SERVER_INFO_XML_TAG_SHARED_FIELD))
		{
			int fieldID=0;
			vector<int> vecChannels;
			if (!ParseGameServer_SharedField(pGameServerElement, pFieldInfoManager, fieldID, vecChannels))
				continue;

			InsertManagedSharedField(pGameServerInfo, fieldID, vecChannels);
		}
		else if (!_stricmp(pGameServerElement->Value(), SERVER_INFO_XML_TAG_RESERVED_FIELD))
		{
			int fieldID=0;
			vector<int> vecReservedChannels;
			if (!ParseGameServer_ReservedField(pGameServerElement, pFieldInfoManager, fieldID, vecReservedChannels))
				continue;

			InsertManagedReservedChannel(pGameServerInfo, fieldID, vecReservedChannels);
		}
	}
}

bool ZGameServerInfoLoader::ParseGameServer_SharedField(MXmlElement* pGameServerElement, ZFieldInfoManager* pFieldInfoManager, int& outFieldID, vector<int>& outChannels)
{
	_Attribute(&outFieldID, pGameServerElement, SERVER_INFO_XML_ATTR_ID);

	ZFieldInfo* pFieldInfo = pFieldInfoManager->Find(outFieldID);
	if (pFieldInfo == NULL) return false;
	if (pFieldInfo->m_bDynamic) return false;		// 동적 필드이면 제외

	string strChannels;
	if (!_Attribute(strChannels, pGameServerElement, SERVER_INFO_XML_ATTR_CHANNEL))
		return false;
	
	MCsvParser csv_parser;
	csv_parser.LoadFromStream(strChannels.c_str());
	int nColCount = csv_parser.GetColCount(0);

	char szChannel[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (!csv_parser.GetData(nCol, 0, szChannel, 1024))
			continue;

		int nChannelID = atoi(szChannel);
		if (nChannelID <= 0 || nChannelID > pFieldInfo->m_nDefaultChannelCount)
			continue;

		if (IsExistChannelAlready(outFieldID, nChannelID))
			continue;

		outChannels.push_back(nChannelID);
	}
	return true;
}

bool ZGameServerInfoLoader::ParseGameServer_ReservedField(MXmlElement* pGameServerElement, ZFieldInfoManager* pFieldInfoManager, int& outFieldID, vector<int>& outReservedChannels)
{
	_Attribute(&outFieldID, pGameServerElement, SERVER_INFO_XML_ATTR_ID);

	ZFieldInfo* pFieldInfo = pFieldInfoManager->Find(outFieldID);
	if (pFieldInfo == NULL) return false;
	if (pFieldInfo->m_bDynamic) return false;		// 동적 필드이면 제외

	string strChannels;
	if (!_Attribute(strChannels, pGameServerElement, SERVER_INFO_XML_ATTR_CHANNEL))
		return false;

	MCsvParser csv_parser;
	csv_parser.LoadFromStream(strChannels.c_str());
	int nColCount = csv_parser.GetColCount(0);

	char szChannel[1024];
	for (int nCol = 0; nCol < nColCount; nCol++)
	{
		if (!csv_parser.GetData(nCol, 0, szChannel, 1024))
			continue;

		int nChannelID = atoi(szChannel);

		if (IsExistReservedChannelAlready(outFieldID, nChannelID))
			continue;

		outReservedChannels.push_back(nChannelID);
	}
	return true;
}

void ZGameServerInfoLoader::InsertManagedSharedField( ZGameServerInfo* pGameServerInfo, int nFieldID, vector<int> vecChannels )
{
	for (size_t i = 0; i < pGameServerInfo->m_vecManagedSharedField.size(); i++)
	{
		ZManagedSharedField* pManagedSharedField = &pGameServerInfo->m_vecManagedSharedField[i];
		if (pManagedSharedField->nFieldID != nFieldID)
			continue;
		
		for (size_t t = 0; t < vecChannels.size(); t++)
		{
			int nNewChannelID = vecChannels[t];

			// 중복 검사
			bool bExistChannel = false;
			for (size_t k = 0; k < pManagedSharedField->vecChannelID.size(); k++)
			{
				if (nNewChannelID == pManagedSharedField->vecChannelID[k])
				{
					bExistChannel = true;
					break;
				}
			}
			if (bExistChannel) continue;

			// 채널 추가
			pManagedSharedField->vecChannelID.push_back(nNewChannelID);
		}

		return;
	}

	/// 필드 정보가 없으면 새로 추가.
	ZManagedSharedField managedSharedField;
	managedSharedField.nFieldID = nFieldID;
	managedSharedField.vecChannelID = vecChannels;

	pGameServerInfo->m_vecManagedSharedField.push_back(managedSharedField);
}
void ZGameServerInfoLoader::InsertManagedReservedChannel(ZGameServerInfo* pGameServerInfo, int nFieldID, vector<int> vecReservedChannels)
{
	for (size_t i = 0; i < pGameServerInfo->m_vecManagedSharedField.size(); i++)
	{
		ZManagedSharedField* pManagedSharedField = &pGameServerInfo->m_vecManagedSharedField[i];
		if (pManagedSharedField->nFieldID != nFieldID)
			continue;

		for (size_t t = 0; t < vecReservedChannels.size(); t++)
		{
			int nNewReservedChannelID = vecReservedChannels[t];

			// 중복 검사
			bool bExistChannel = false;
			for (size_t k = 0; k < pManagedSharedField->vecReservedChannelID.size(); k++)
			{
				if (nNewReservedChannelID == pManagedSharedField->vecReservedChannelID[k])
				{
					bExistChannel = true;
					break;
				}
			}
			if (bExistChannel) continue;

			// 예약채널 추가
			pManagedSharedField->vecReservedChannelID.push_back(nNewReservedChannelID);
		}

		return;
	}

	/// 필드 정보가 없으면 새로 추가.
	ZManagedSharedField managedSharedField;
	managedSharedField.nFieldID = nFieldID;
	managedSharedField.vecReservedChannelID = vecReservedChannels;

	pGameServerInfo->m_vecManagedSharedField.push_back(managedSharedField);
}

void ZGameServerInfoLoader::RunPostfix(ZFieldInfoManager* pFieldInfoManager)
{
	ZLoaderPostfix runner;
	runner.Run(m_pManager, pFieldInfoManager);
}

bool ZGameServerInfoLoader::IsExistChannelAlready( int nFieldID, int nChannelID )
{
	for (ZGameServerInfoMap::iterator itorGameServerInfo = m_pManager->GetServerInfoMap().begin();
		itorGameServerInfo != m_pManager->GetServerInfoMap().end(); ++itorGameServerInfo)
	{
		ZGameServerInfo* pGameServerInfo = (*itorGameServerInfo).second;
		for (size_t i = 0; i < pGameServerInfo->m_vecManagedSharedField.size(); i++)
		{
			if (pGameServerInfo->m_vecManagedSharedField[i].nFieldID != nFieldID)
				continue;

			vector<int>::iterator itor = find(pGameServerInfo->m_vecManagedSharedField[i].vecChannelID.begin(), 
				pGameServerInfo->m_vecManagedSharedField[i].vecChannelID.end(), nChannelID);

			if (itor != pGameServerInfo->m_vecManagedSharedField[i].vecChannelID.end())
			{
				return true;
			}
		}
	}
	return false;
}

bool ZGameServerInfoLoader::IsExistReservedChannelAlready( int nFieldID, int nChannelID )
{
	for (ZGameServerInfoMap::iterator itorGameServerInfo = m_pManager->GetServerInfoMap().begin();
		itorGameServerInfo != m_pManager->GetServerInfoMap().end(); ++itorGameServerInfo)
	{
		ZGameServerInfo* pGameServerInfo = (*itorGameServerInfo).second;
		for (size_t i = 0; i < pGameServerInfo->m_vecManagedSharedField.size(); i++)
		{
			if (pGameServerInfo->m_vecManagedSharedField[i].nFieldID != nFieldID)
				continue;

			vector<int>::iterator itor = find(pGameServerInfo->m_vecManagedSharedField[i].vecChannelID.begin(), 
				pGameServerInfo->m_vecManagedSharedField[i].vecChannelID.end(), nChannelID);

			if (itor != pGameServerInfo->m_vecManagedSharedField[i].vecChannelID.end())
			{
				return true;
			}
		}
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////

void ZGameServerInfoLoader::ZLoaderPostfix::Run( ZGameServerInfoManager* m_pManager, ZFieldInfoManager* pFieldInfoManager )
{
	for (ZFieldInfoManager::ZFieldInfoMap::iterator itorFieldInfo = pFieldInfoManager->GetFieldInfoMap().begin(); 
		itorFieldInfo != pFieldInfoManager->GetFieldInfoMap().end(); ++itorFieldInfo)
	{
		ZFieldInfo* pFieldInfo = (*itorFieldInfo).second;

		SetupChannelID(m_pManager, pFieldInfo);
	}

	for (ZGameServerInfoMap::iterator itorGameServerInfo = m_pManager->GetServerInfoMap().begin();
		itorGameServerInfo != m_pManager->GetServerInfoMap().end(); ++itorGameServerInfo)
	{
		ZGameServerInfo* pGameServerInfo = (*itorGameServerInfo).second;
		SortGameServerFields(pGameServerInfo);
	}
}

void ZGameServerInfoLoader::ZLoaderPostfix::SetupChannelID( ZGameServerInfoManager* pGameServerInfoManager, ZFieldInfo* pFieldInfo )
{
	if (pFieldInfo->m_bDynamic) return;

	vector<int> vecNotFoundChannelID = FindNotExistChannelID(pGameServerInfoManager, pFieldInfo);
	if (vecNotFoundChannelID.empty()) return;

	FillChannelID(pGameServerInfoManager, pFieldInfo, vecNotFoundChannelID);
}

void ZGameServerInfoLoader::ZLoaderPostfix::FillChannelID( ZGameServerInfoManager* pGameServerInfoManager, ZFieldInfo* pFieldInfo, vector<int> &vecNotFoundChannelID )
{
	if (pFieldInfo->m_bDynamic) return;

	ZGameServerInfoMap::iterator itorBegin = pGameServerInfoManager->GetServerInfoMap().begin();
	if (itorBegin == pGameServerInfoManager->GetServerInfoMap().end()) return;

	ZGameServerInfo* pGameServerInfo = (*itorBegin).second;

	bool bExistManaged = false;
	for (size_t i = 0; i < pGameServerInfo->m_vecManagedSharedField.size(); i++)
	{
		ZManagedSharedField& managedSharedField = pGameServerInfo->m_vecManagedSharedField[i];
		if (pFieldInfo->m_nFieldID == managedSharedField.nFieldID)
		{
			bExistManaged = true;

			managedSharedField.vecChannelID.insert(managedSharedField.vecChannelID.end(),
				vecNotFoundChannelID.begin(), vecNotFoundChannelID.end());

			sort(managedSharedField.vecChannelID.begin(), managedSharedField.vecChannelID.end());
			break;
		}
	}

	if (!bExistManaged)
	{
		ZManagedSharedField newManagedSharedField;
		newManagedSharedField.nFieldID = pFieldInfo->m_nFieldID;

		newManagedSharedField.vecChannelID.insert(newManagedSharedField.vecChannelID.end(),
			vecNotFoundChannelID.begin(), vecNotFoundChannelID.end());

		sort(newManagedSharedField.vecChannelID.begin(), newManagedSharedField.vecChannelID.end());

		pGameServerInfo->m_vecManagedSharedField.push_back(newManagedSharedField);
	}
}

vector<int> ZGameServerInfoLoader::ZLoaderPostfix::FindNotExistChannelID( ZGameServerInfoManager* pGameServerInfoManager, ZFieldInfo* pFieldInfo )
{
	vector<int> vecNotFoundChannelID;
	vector<int> vecFoundChannelID;

	for (ZGameServerInfoMap::iterator itorServerInfo = pGameServerInfoManager->GetServerInfoMap().begin();
		itorServerInfo != pGameServerInfoManager->GetServerInfoMap().end(); ++itorServerInfo)
	{
		ZGameServerInfo* pServerInfo = (*itorServerInfo).second;
		for (size_t i = 0; i < pServerInfo->m_vecManagedSharedField.size(); i++)
		{
			ZManagedSharedField& managedSharedField = pServerInfo->m_vecManagedSharedField[i];
			if (pFieldInfo->m_nFieldID == managedSharedField.nFieldID)
			{
				vecFoundChannelID.insert(vecFoundChannelID.end(), 
					managedSharedField.vecChannelID.begin(), managedSharedField.vecChannelID.end());
			}
		}
	}

	sort (vecFoundChannelID.begin(), vecFoundChannelID.end());

	for (int nChannelID = 1; nChannelID <= pFieldInfo->m_nDefaultChannelCount; nChannelID++)
	{
		bool bExist = false;
		for (size_t j = 0; j < vecFoundChannelID.size(); j++)
		{
			if (vecFoundChannelID[j] == nChannelID)
			{
				bExist = true;
				break;
			}
		}

		if (!bExist)
		{
			vecNotFoundChannelID.push_back(nChannelID);
		}
	}

	return vecNotFoundChannelID;
}

void ZGameServerInfoLoader::ZLoaderPostfix::SortGameServerFields( ZGameServerInfo* pGameServerInfo )
{
	struct ManagedSharedField_IsLess
	{
		bool operator() (ZManagedSharedField& lhs, ZManagedSharedField& rhs) const
		{
			return lhs.nFieldID < rhs.nFieldID;
		}
	};
	sort(pGameServerInfo->m_vecManagedSharedField.begin(), pGameServerInfo->m_vecManagedSharedField.end(), ManagedSharedField_IsLess());
}