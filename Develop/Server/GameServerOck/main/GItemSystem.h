#ifndef _GITEM_SYSTEM_H
#define _GITEM_SYSTEM_H

#include "CSItemData.h"

class GEntityPlayer;
class GEntityNPC;
class GItemData;
class GItem;

enum CCommandResultTable;
struct TD_ITEM;
class GEntityActor;
class GQuestSystem;
class GItemEquiper;
class GItemUnequiper;
class GItemAdder;
class GItemRemover;
class GItemDurabilityUpdater;
class GItemUser;
class GItemActableChecker;
class GItemMover;
class GItemMoverForDBTask;
class GItemEquiperForDBTask;
class GItemAddAndRemoverForInfo;
class GItemSystemSetterForTest;
class GItemSorter;
class GItemEnchant;
class GItemDyer;

class GItemSystem : public MTestMemPool<GItemSystem>
{
friend class GItemSystemSetterForTest;

public:
	GItemSystem();
	~GItemSystem();

	GItemEquiper&			GetEquiper();
	GItemUnequiper&			GetUnequiper();
	GItemAdder&				GetAdder();
	GItemRemover&			GetRemover();	
	GItemUser&				GetUser();
	GItemDurabilityUpdater&	GetDurabilityUpdater();	
	GItemMover&				GetMover();
	GItemActableChecker&	GetActableChecker();
	GItemAddAndRemoverForInfo& GetAddAndRemoverForInfo();
	GItemSorter&			GetSorter();
	GItemEnchant&			GetEnchant();
	GItemDyer&				GetDyer();

	GItemMoverForDBTask&	GetMoverForDBTask();
	GItemEquiperForDBTask&	GetEquiperForDBTask();
	GItemSystemSetterForTest&	GetSetterForTest();
	
private:
	GItemEquiper*				m_pEquiper;
	GItemUnequiper*				m_pUnequiper;
	GItemAdder*					m_pAdder;
	GItemRemover*				m_pRemover;
	GItemUser*					m_pUser;	
	GItemMover*					m_pMover;
	GItemDurabilityUpdater*		m_pDurabilityUpdater;
	GItemActableChecker*		m_pActableChecker;
	GItemAddAndRemoverForInfo*	m_pAddAndRemoverForInfo;
	GItemSorter*				m_pSorter;
	GItemEnchant*				m_pEnchant;
	GItemDyer*					m_pDyer;

	GItemMoverForDBTask*		m_pMoverForDBTask;
	GItemEquiperForDBTask*		m_pEquiperForDBTask;

	GItemSystemSetterForTest*	m_pSetterForTest;
};


#endif