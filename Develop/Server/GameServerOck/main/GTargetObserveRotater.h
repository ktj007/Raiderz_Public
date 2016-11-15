#pragma once

class GEntityNPC;
class GTask;

class GTargetObserveRotater
{
public:
	GTargetObserveRotater(void);
	~GTargetObserveRotater(void);

	bool IsNeedRotate(GEntityNPC* pOwnerNPC);
	bool HasRotateTalent( GEntityNPC* pOwnerNPC );

	GTask* GetRotateTask(GEntityNPC* pOwnerNPC);
};
