#pragma once

class GQuestUIShower;
class GQuestGiver;
class GQuestCompleter;
class GQuestRewarder;
class GQuestRewarderForDBTask;

class GQuestSharer;
class GQuestCanceler;
class GQuestFailer;
class GQuestAddItemCollector;
class GQuestRemoveItemCollector;

class GQuestVarUpdater;
class GQObjUpdater;
class GQObjUpdaterForDBTask;
class GQObjInteractor;

class GQuestReseter;

class GQuestSystemSetterForTest;


class GQuestSystem : public MTestMemPool<GQuestSystem>
{
friend class GQuestSystemSetterForTest;

public:
	GQuestSystem();
	~GQuestSystem();

	GQuestUIShower&				GetUIShower();
	GQuestGiver&				GetGiver();
	GQuestCompleter&			GetCompleter();
	GQuestRewarder&				GetRewarder();	
	GQuestRewarderForDBTask&	GetRewarderForDBTask();

	GQuestSharer&				GetSharer();
	GQuestCanceler&				GetCanceler();
	GQuestFailer&				GetFailer();
	GQuestAddItemCollector&		GetAddItemCollector();
	GQuestRemoveItemCollector&	GetRemoveItemCollector();

	GQuestVarUpdater&			GetQuestVarUpdater();
	GQObjUpdater&				GetQObjUpdater();
	GQObjUpdaterForDBTask&		GetQObjUpdaterForDBTask();
	GQObjInteractor&			GetQObjInteractor();

	GQuestReseter&				GetReseter();

	GQuestSystemSetterForTest&	GetSetterForTest();
	
private :
	GQuestUIShower*				m_pUIShower;
	GQuestGiver*				m_pGiver;
	GQuestCompleter*			m_pCompleter;
	GQuestRewarder*				m_pRewarder;
	GQuestRewarderForDBTask*	m_pRewardForDBTask;

	GQuestSharer*				m_pSharer;
	GQuestCanceler*				m_pCanceler;
	GQuestFailer*				m_pFailer;
	GQuestAddItemCollector*		m_pAddItemCollector;
	GQuestRemoveItemCollector*	m_pRemoveItemCollector;

	GQuestVarUpdater*			m_pQuestVarUpdater;
	GQObjUpdater*				m_pQObjUpdater;
	GQObjUpdaterForDBTask*		m_pQObjUpdaterForDBTask;
	GQObjInteractor*			m_pQObjInteractor;

	GQuestReseter*				m_pQuestReseter;

	GQuestSystemSetterForTest*	m_pSetterForTest;
};
