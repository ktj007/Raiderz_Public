#pragma once

class GEntityPlayer;
class GClientMailRouter;
class GMail;
class GItem;

class GMailItemTaker : public MTestMemPool<GMailItemTaker>
{
public:
	GMailItemTaker(void);
	virtual ~GMailItemTaker(void);

	virtual bool		Take(GEntityPlayer* pPlayer, MUID uidMail, int nMailSlotID);
	bool				DelTakenItemForDBTask(GEntityPlayer* pPlayer, int64 nMailUID, int nMailSlotID, int nNewMailThumbnailItemID);

	void				SetClientRouter(GClientMailRouter* pNewClientMailRouter);

private:
	GClientMailRouter*	m_pClientRouter;

	bool				Check(GEntityPlayer* pPlayer, GMail* pMail, GItem* pItem);
	bool				Apply(GEntityPlayer* pPlayer, GMail* pMail, GItem* pItem);

};
