#pragma once

#include "GFieldGrid.h"

class GTrialField;

class GTrialFieldBoundary : public GFieldGrid::BoundChecker::Lisener, public MTestMemPool<GTrialFieldBoundary>
{
public:
	GTrialFieldBoundary(GTrialField* pOwner);
	~GTrialFieldBoundary(void);

	void Update(float fDelta);

	bool IsExpired() const;
private:
	void StartExpired();
	void StopExpired();
	void Update_Expired( float fDelta );
private:
	virtual void OnChangeOutters(int nPlayerQtyInBoundary) override;
private:
	GTrialField* m_pOwner;
	MRegulator m_rgrExpired;
	bool m_bExpired;
};
