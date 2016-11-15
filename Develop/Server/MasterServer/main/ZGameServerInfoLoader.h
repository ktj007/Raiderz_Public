#pragma once

#include "MXml.h"

class ZGameServerInfoManager;
class ZGameServerInfo;
class ZFieldInfoManager;
class ZFieldInfo;

class ZGameServerInfoLoader
{
protected:
	class ZLoaderPostfix
	{
	private:
		void SetupChannelID( ZGameServerInfoManager* pGameServerInfoManager, ZFieldInfo* pFieldInfo );
		void SortGameServerFields( ZGameServerInfo* pGameServerInfo );

		vector<int> FindNotExistChannelID( ZGameServerInfoManager* pGameServerInfoManager, ZFieldInfo* pFieldInfo );
		void FillChannelID( ZGameServerInfoManager* pGameServerInfoManager, ZFieldInfo* pFieldInfo, vector<int> &vecNotFoundChannelID );
	public:
		void Run(ZGameServerInfoManager* pGameServerInfoManager, ZFieldInfoManager* pFieldInfoManager);
	};
protected:
	ZGameServerInfoManager*	m_pManager;
	bool ParseGameServer_ReservedField(MXmlElement* pGameServerElement, ZFieldInfoManager* pFieldInfoManager, int& outFieldID, vector<int>& outReservedChannels);
	bool ParseGameServer_SharedField(MXmlElement* pGameServerElement, ZFieldInfoManager* pFieldInfoManager, int& outFieldID, vector<int>& outChannels);
	void ParseGameServer(ZGameServerInfo* pGameServerInfo, MXmlElement* pElement, ZFieldInfoManager* pFieldInfoManager);
	bool Parse(MXmlElement* pElement, ZFieldInfoManager* pFieldInfoManager);

	void InsertManagedSharedField(ZGameServerInfo* pGameServerInfo, int nFieldID, vector<int> vecChannels);
	void InsertManagedReservedChannel(ZGameServerInfo* pGameServerInfo, int nFieldID, vector<int> vecReservedChannels);

	bool IsExistChannelAlready(int nFieldID, int nChannelID);
	bool IsExistReservedChannelAlready(int nFieldID, int nChannelID);

	void RunPostfix(ZFieldInfoManager* pFieldInfoManager);
public:
	ZGameServerInfoLoader() : m_pManager(NULL) {}
	bool Load(ZGameServerInfoManager* mgr, const wchar_t* szFileName, ZFieldInfoManager* pFieldInfoManager);
	bool LoadStream(ZGameServerInfoManager* mgr, const char* szStream, ZFieldInfoManager* pFieldInfoManager);
};