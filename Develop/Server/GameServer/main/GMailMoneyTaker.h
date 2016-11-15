#pragma once

class GClientMailRouter;

class GMailMoneyTaker : public MTestMemPool<GMailMoneyTaker>
{
public:
	GMailMoneyTaker(void);
	virtual ~GMailMoneyTaker(void);

	virtual bool TakeAppendedMoney(GEntityPlayer* pPlayer, MUID uidMail);

	void SetClientRouter(GClientMailRouter* pNewRouter);

private:
	GClientMailRouter* m_pMailRouter;
};
