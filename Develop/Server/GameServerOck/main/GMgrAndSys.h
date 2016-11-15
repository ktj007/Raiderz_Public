#ifndef _G_MGR_AND_SYS_H_
#define _G_MGR_AND_SYS_H_

class GPlayerObjectManager;
class GDBManager;
class GScriptManager;
class GItemManager;
class GNPCInfoMgr;
class GNPCMgr;
class GFieldMgr;
class GTalentInfoMgr;
class GPlayerDB;
class GQuestInfoMgr;
class GFieldInfoMgr;
class GEnvManager;

class GPartySystem;
class GTradeSystem;
class GTradeLogic;
class GNPCTradeSystem;
class GItemSystem;
class GQuestSystem;
class GTrainingSystem;
class GDialogSystem;
class GChainSystem;
class GFieldSystem;
class GLootSystem;

class GMgrAndSys
{
public:
	GDBManager*					m_pDBMgr;
	GPlayerObjectManager*		m_pPlayerObjectMgr;	
	GScriptManager*				m_pScriptMgr;				
	GItemManager*				m_pItemMgr;			
	GNPCInfoMgr*				m_pNPCInfoMgr;
	GNPCMgr*					m_pNPCMgr;
	GFieldMgr*					m_pFieldMgr;
	GTalentInfoMgr*				m_pTalentInfoMgr;	
	GQuestInfoMgr*				m_pQuestInfoMgr;
	GFieldInfoMgr*				m_pFieldInfoMgr;
	GEnvManager*				m_pEnvMgr;		
	GPartySystem*				m_pPartySys;


	GPlayerDB*					m_pPlayerDB;
	GTradeSystem*				m_pTradeSys;
	GTradeLogic*				m_pTradeLogic;
	GNPCTradeSystem*			m_pNPCTradeSys;
	GItemSystem*				m_pItemSys;
	GQuestSystem*				m_pQuestSys;
	GTrainingSystem*			m_pTrainingSys;
	GDialogSystem*				m_pDialogSys;
	GChainSystem*				m_pChainSys;
	GFieldSystem*				m_pFieldSys;
	GLootSystem*				m_pLootSys;

public:
	GMgrAndSys();
	~GMgrAndSys();

	// 테스트 할때 Mock 매니저와 시스템을 사용하기 위해서만 사용		

	void Clear();
	bool IsValid();
};

//extern GMgrAndSys gMgrAndSys;

#endif//_G_MGR_AND_SYS_H_
