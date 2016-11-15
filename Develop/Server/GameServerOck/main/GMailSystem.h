#pragma once

#include "CCommandResultTable.h"

class GEntityPlayer;
class GClientMailRouter;
class GServerMailRouter;
class GMailItemPutUper;
class GMailItemPutDowner;
class GMailItemTaker;
class GMailMoneyTaker;
struct TD_MAIL_POST_REQ_INFO;
struct TD_MAIL_MAILBOX_MAIL_INFO;
struct TD_MAIL_CONTENT_INFO;
struct TD_ITEM;
struct TD_MAIL_RESERVE_ITEM;

class GMailSystem : public MTestMemPool<GMailSystem>
{
public:
	GMailSystem(void);
	virtual ~GMailSystem(void);

	void				OpenMailbox(GEntityPlayer* pPlayer);
	void				ClearExpiredMail(GEntityPlayer* pPlayer);
	void				GetDBMailList(GEntityPlayer* pPlayer, bool bOpenMaibox=false);
	void				RouteOpenMailbox(GEntityPlayer* pPlayer, int64 nReqMailUIDTop=0, int64 nReqMailUIDBottom=0);
	
	virtual bool		PostMail(GEntityPlayer* pPlayer, const TD_MAIL_POST_REQ_INFO* pInfo, wstring strMailText);
	virtual void		UpdateMailBox(GEntityPlayer* pPlayer, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo);
	void				RouteNotifyMail(GEntityPlayer* pPlayer, bool bIsNew, const vector<int64>& vecMailUID);
	
	virtual void		RetryTransNewMail(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo);
	virtual void		RouteNewMailToReceiver(CID nCID, const TD_MAIL_MAILBOX_MAIL_INFO* pInfo);
	virtual void		RouteMailPostRes(GEntityPlayer* pPlayer, CCommandResultTable nResult);

	virtual void		PutUpItem(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem);
	virtual void		PutDownItem(GEntityPlayer* pPlayer, const TD_MAIL_RESERVE_ITEM* pItem);	

	void				DeleteMail(GEntityPlayer* pPlayer, MUID uidMail);

	virtual void		ReadMail(GEntityPlayer* pPlayer, MUID uidMail);

	void				SetClientRouter(GClientMailRouter* pNewClientMailRouter);
	void				SetServerRouter(GServerMailRouter* pNewServerMailRouter);
	void				SetMailItemPutUper(GMailItemPutUper* pNewItemPutUper);
	void				SetMailItemPutDowner(GMailItemPutDowner* pNewItemPutDowner);
	void				SetMailItemTaker(GMailItemTaker* pNewItemTaker);
	void				SetMailMoneyTaker(GMailMoneyTaker* pNewMoneyTaker);

	virtual void		RouteTakeAppendedMoneyRes(GEntityPlayer* pPlayer, MUID uidMail);

	GMailItemTaker*		GetItemTaker()	{ return m_pItemTaker; }
	GMailMoneyTaker*	GetMoneyTaker()	{ return m_pMoneyTaker; }

	int					CalcPostage(int nSlotCount);
	MAIL_TYPE			GetMailType(const GEntityPlayer* pPlayer);

private:
	GClientMailRouter*	m_pClientRouter;
	GServerMailRouter*	m_pServerRouter;

	GMailItemPutUper*	m_pItemPutUper;
	GMailItemPutDowner*	m_pItemPutDowner;

	GMailItemTaker*		m_pItemTaker;
	GMailMoneyTaker*	m_pMoneyTaker;

};
