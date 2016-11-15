#pragma once

#include "ZTestHelper.h"
#include "MockLink.h"
#include "MockFieldInfoManager.h"
#include "ZGameServerInfoManager.h"
#include "ZGameServerObjectManager.h"
#include "ZLoginServerInfoManager.h"
#include "ZLoginServerFacade.h"
#include "ZAppServerInfoManager.h"
#include "ZAppServerObjectManager.h"
#include "ZTestWrapper.h"
#include "ZFieldManager.h"
#include "ZPlayerManager.h"
#include "ZPartyManager.h"


//////////////////////////////////////////////////////////////////////////

struct FBaseGameServer
{
	FBaseGameServer()
	{

	}
	virtual ~FBaseGameServer()
	{

	}

	// 만약 해당 ServerID가 있으면 그 ServerID에 필드 정보를 추가한다.
	ZGameServerInfo* AddGameServerInfo(int nServerID, int nFieldID, int nChannelIDCount, int nFirstChannelID, ...);

	ZGameServerObject* AddGameServerObject(ZGameServerInfo* pGameServerInfo);

	ZGameServerObject* AddGameServerObject(int nServerID=0, int nFieldID=0);

	ZTestMgrWrapper<ZGameServerInfoManager>		m_ServerInfoManagerWrapper;
	ZTestMgrWrapper<ZGameServerObjectManager>	m_ServerObjectManagerWrapper;
};


//////////////////////////////////////////////////////////////////////////

struct FBaseField
{
	FBaseField()
	{

	}
	virtual ~FBaseField()
	{

	}

	ZSharedField* AddSharedField(int nFieldID, int nChannelID = 1, int nGameServerID = 1, bool bIsDefaultChannel = true);
	ZSharedField* AddSharedField(ZFieldInfo* pFieldInfo, int nChannelID = 1, int nGameServerID = 1, bool bIsDefaultChannel = true);
	ZDynamicFieldMaster* AddFieldGroup(int nFieldGroupID, MUID uidOwner, int nGameServerID = 1, DYNAMIC_FIELD_TYPE eType = DFT_TRIAL);

	ZTestMgrWrapper<ZFieldManager>	m_FieldManagerWrapper;
};

struct FBaseFieldInfo
{
	FBaseFieldInfo()
	{

	}
	virtual ~FBaseFieldInfo()
	{

	}

	ZFieldInfo* AddFieldInfo(int nFieldID = 1, const wchar_t* szFieldName = L"TestField", const wchar_t* szFieldFile = L"TestField", bool bDynamic = false, int nDefaultChannelCount = 1);
	void AddFieldGroupInfo(int nFieldGroupID, int nFirstDynamicFieldID);

	ZTestMgrWrapper2<ZFieldInfoManager, MockFieldInfoManager>	m_FieldInfoManagerWrapper;
};


//////////////////////////////////////////////////////////////////////////
struct FBasePlayer
{
	FBasePlayer()
	{

	}
	virtual ~FBasePlayer()
	{

	}

	ZPlayer* AddNewPlayer(MUID uidPlayer=MUID::ZERO, AID nAID=0, std::wstring strUserID=L"");
	ZPlayer* AddNewPlayerInWorld(int nGameServerID=0, AID nAID=0, std::wstring strUserID=L"", CID nCID=0, MUID uidPlayer=MUID::ZERO, wstring strPlayerName=L"", int nGID=0);

	ZTestMgrWrapper<ZPlayerManager>	m_PlayerManagerWrapper;
};
//////////////////////////////////////////////////////////////////////////
struct FBaseParty
{
	FBaseParty()
	{

	}
	virtual ~FBaseParty()
	{

	}

	ZTestMgrWrapper<ZPartyManager>	m_PartyManagerWrapper;
};
//////////////////////////////////////////////////////////////////////////
struct FBaseMockLink
{
	FBaseMockLink()
	{

	}
	virtual ~FBaseMockLink()
	{
		ZTestHelper::ClearLinks();
	}

	MockLink* NewLink(MUID uidNew=MUID::Invalid())
	{
		return ZTestHelper::NewLink(uidNew);
	}
};

//////////////////////////////////////////////////////////////////////////
struct FBaseServerField : public FBaseGameServer, public FBaseFieldInfo, public FBaseField
{
	FBaseServerField() : m_nFieldID(0), m_nServerID(0), m_uidClient(MUID::ZERO)
	{

	}
	virtual ~FBaseServerField()
	{

	}

	// 기본적으로 생성되는 것
	// FIeldInfo - 110(channel 1)
	//
	// GameServerInfo - 1
	// GameServerObject -1

	void InitDefaultGameServer(int nServerID=1, int nFieldID=110, int nChannelCount=1);

	int		m_nFieldID;
	int		m_nServerID;
	MUID	m_uidClient;

};

//////////////////////////////////////////////////////////////////////////
struct FBaseMockLink2 : public FBaseMockLink, FBaseServerField
{
	FBaseMockLink2()
	{

	}
	virtual ~FBaseMockLink2()
	{

	}

};

//////////////////////////////////////////////////////////////////////////

struct FBaseLoginServer
{
	FBaseLoginServer()
	{

	}
	virtual ~FBaseLoginServer()
	{

	}

	// 만약 해당 ServerID가 있으면 그 ServerID에 필드 정보를 추가한다.
	ZLoginServerInfo* AddLoginServerInfo( int nServerID=0 );

	MUID Connected( ZLoginServerInfo* pLoginServerInfo );
	MUID Connected( int nServerID=0 );

	ZTestMgrWrapper<ZLoginServerInfoManager>	m_ServerInfoManagerWrapper;
	ZTestMgrWrapper<ZLoginServerFacade>	m_ServerObjectManagerWrapper;
};


//////////////////////////////////////////////////////////////////////////

struct FBaseAppServer
{
	FBaseAppServer()
	{

	}
	virtual ~FBaseAppServer()
	{

	}

	// 만약 해당 ServerID가 있으면 그 ServerID에 필드 정보를 추가한다.
	ZAppServerInfo* AddAppServerInfo( int nServerID );

	MUID AddAppServerObject( ZAppServerInfo* pAppServerInfo );

	MUID AddAppServerObject( int nServerID );

	ZTestMgrWrapper<ZAppServerInfoManager>	m_ServerInfoManagerWrapper;
	ZTestMgrWrapper<ZAppServerObjectManager>	m_ServerObjectManagerWrapper;
};

