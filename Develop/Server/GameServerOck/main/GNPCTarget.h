#pragma once

class GEntityNPC;
class GEntityActor;

class GNPCTarget : public MTestMemPool<GNPCTarget>
{
public:
	GNPCTarget(GEntityNPC* pOwner);
	~GNPCTarget(void);

	MUID GetTargetUID() const;
	void SetTargetUID(MUID uidTarget);
	void ClearTargetUID();
	GEntityActor* GetTarget() const;

private:
	GEntityNPC* m_pOwner;
	MUID m_uidTarget;
};
