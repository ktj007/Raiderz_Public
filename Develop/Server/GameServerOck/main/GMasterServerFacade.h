#pragma once

#include "STransData_M2G.h"
#include "GMasterNetClient.h"
#include "MUID.h"
#include "STypes.h"

class GEntityPlayer;
class GMasterNetClient;
class GClientConnectionKeeper;
class GManagedFieldInfoMgr;
class GFieldInfoMgr;
class GProxyFieldGroup;
class GProxyFieldManager;
class GProxyPartyManager;
class GProxyParty;
class GPartyRouter;
class GServerPartyRouter;
class GServerGateRouter;
class GServerPlayerRouter;
class GServerFieldRouter;
struct GGateInfo;
class SProxyPlayerManager;
class GMasterServerDisconnectHandler;
class GServerGuildRouter;
class GMoveServerRouter;

class GMasterServerFacade : public MTestMemPool<GMasterServerFacade>
{
protected:
	int							m_nGameServerID;

	minet::MNetClient*			m_pMasterNetClient;				///< 마스터 서버 클라이언트
	GClientConnectionKeeper*	m_pConnectionKeeper;
	
	GManagedFieldInfoMgr*		m_pManagedFieldInfoMgr;
	GProxyFieldManager*			m_pProxyFieldManager;	
	GProxyPartyManager*			m_pProxyPartyManager;
	SProxyPlayerManager*		m_pProxyPlayerManager;

	GServerPartyRouter*			m_pPartyRouter;
	GServerGateRouter*			m_pGateRouter;
	GServerPlayerRouter*		m_pPlayerRouter;
	GServerFieldRouter*			m_pFieldRouter;
	GServerGuildRouter*			m_pGuildRouter;
	GMoveServerRouter*			m_pMoveServerRouter;

	bool IsStandAloneMode();
	void InitForStandAlone();
	bool ConnectToMasterServer(const wchar_t* szIP, int nPort, const wchar_t* szNetworkCardIP = L"");

public:
	GMasterServerFacade();
	GMasterServerFacade(MNetClient* pNetClient);
	virtual ~GMasterServerFacade();

	void Init();	
	// Network
	bool CreateNetwork(const wchar_t* szMasterServerIP, int nMasterServerPort);
	void DestroyNetwork();
	void RunNetwork();
	void OnConnect();
	void OnDisconnect();
	void OnError(int nErrorCode);
	MCommand* MakeNewCommand(int nCmdID, int nParamCount, MCommandParameter* pCmdParam, ... );	
	MCommand* MakeNewCommand(int nCmdID);
	bool Route(MCommand* pCommand);
	void SetPartyRouter(GServerPartyRouter* pPartyRouter);
	void SetGateRouter(GServerGateRouter* pGateRouter);
	void SetPlayerRouter(GServerPlayerRouter* pPlayerRouter);
	void SetFieldRouter(GServerFieldRouter* pFieldRouter);
	void SetGuildRouter(GServerGuildRouter* pGuildRouter);
	void SetMoveServerRouter(GMoveServerRouter* pMoveServerRouter);

	// Logic --
	bool CreateManagedSharedFields(TDMG_MANAGED_SHARED_FIELD* pFIelds, int nFieldCount);	
	void AddManagedSharedFields(TDMG_SHARED_FIELD_INFO* pTDFieldInfo, int nFieldCount);
	void DelManagedSharedField(MUID uidField);

	void AddProxySharedFields(TDMG_SHARED_FIELD_INFO* pFieldInfos, int nFieldInfoCount, uint32 nChecksum);
	void DelProxySharedField(MUID uidField, uint32 nChecksum);
	void ClearProxySharedFields();

	void AddProxyFieldGroups(int nFieldGroupCount, TDMG_DYNAMIC_FIELD_GROUP_INFO* pTDFieldGroupList, int nFieldCount, TDMG_DYNAMIC_FIELD_NODE* pDynamicFieldList, uint32 nChecksum);
	void DelProxyFieldGroup(MUID uidFieldGroup, uint32 nChecksum);
	void ClearProxyFieldGroups();

	void UpdateProxyFieldInfo(MUID uidField, const TDMG_PROXY_FIELD_UPDATE_INFO* pTDProxyFieldUpdateInfo);

	void AddProxyParty(MUID uidParty, MUID uidLeader, wstring strLeaderName, int nLeaderCID);
	void RemoveProxyParty(MUID uidParty);
	void AddProxyPartyMember(MUID uidParty, MUID uidMember, wstring strMemberName, int nMemberCID);
	void RemoveProxyPartyMember(MUID uidParty, MUID uidMember);
	void ChangeProxyPartyLeader(MUID uidParty, MUID uidNewLeader);
	void ClearProxyParty(void);
	GProxyParty* FindProxyParty(MUID uidParty) const;
	void RestoreProxyParty(TD_PARTY_INFO* pPartyInfo, int nCount);

	// AppServer Sync
	void RouteRequestAppServerList(void);

	// For StandAlone
	void SetNetClient(minet::MNetClient* pNetClient);

	// Getter
	GManagedFieldInfoMgr* GetManagedFieldInfoMgr()	{ return m_pManagedFieldInfoMgr; }
	minet::MNetClient* GetNetClient()				{ return m_pMasterNetClient; }

	GProxyFieldManager* GetProxyFieldManager()		{ return m_pProxyFieldManager; }	
	GProxyPartyManager* GetProxyPartyManger()		{ return m_pProxyPartyManager;	}	
	SProxyPlayerManager* GetProxyPlayerManager()	{ return m_pProxyPlayerManager;	}	

	GServerPartyRouter* GetPartyRouter()			{ _ASSERT(m_pPartyRouter != NULL);		return m_pPartyRouter;		}
	GServerGateRouter*	GetGateRouter()				{ _ASSERT(m_pGateRouter != NULL);		return m_pGateRouter;		}
	GServerPlayerRouter* GetPlayerRouter()			{ _ASSERT(m_pPlayerRouter != NULL);		return m_pPlayerRouter;		}
	GServerFieldRouter*	GetFieldRouter()			{ _ASSERT(m_pFieldRouter != NULL);		return m_pFieldRouter;		}
	GServerGuildRouter* GetGuildRouter()			{ _ASSERT(m_pGuildRouter != NULL);		return m_pGuildRouter;		}
	GMoveServerRouter* GetMoveServerRouter()		{ _ASSERT(m_pMoveServerRouter != NULL);	return m_pMoveServerRouter;	}
};
