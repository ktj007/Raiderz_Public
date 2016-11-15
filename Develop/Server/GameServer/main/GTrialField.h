#ifndef _G_TRIAL_FIELD_H_
#define _G_TRIAL_FIELD_H_

#include "GDynamicField.h"
#include "MMemPool.h"

class GTrialFieldBoundary;

class GTrialField : public GDynamicField
{
private:
	uint32	m_nExpiredTime;
	GTrialFieldBoundary* m_pBoundary;

private:
	bool IsExpiredCounting()					{ return m_nExpiredTime > 0; }
	void RouteExpiring(vector<MUID> vecPlayers);

	virtual void		Update(float fDelta) override;
protected:
	virtual void AddEntity(GEntity* pEntity);
	virtual void RemoveEntity(GEntity* pEntity);

public:
	GTrialField(MUID& uid, MUID uidGroup);
	virtual ~GTrialField();

	virtual bool		Create(const GFieldInfo* pFieldInfo) override;

	void StartExpiredCount(uint32 nExpiredTime);
	void CancelExpiredCount();

	bool IsExpired();
};

class GTrialFieldMLeaf : public GTrialField, public MMemPool<GTrialFieldMLeaf>
{
public:
	GTrialFieldMLeaf(MUID& uid, MUID uidGroup) : GTrialField(uid, uidGroup) {}
	virtual ~GTrialFieldMLeaf() {}
};

#endif
