#pragma once

class GPlayerQuest;

class GGlueQuest : public MTestMemPool<GGlueQuest>
{
public:
	GGlueQuest(GPlayerQuest* pOwner);
	virtual ~GGlueQuest();

	GPlayerQuest* GetOwnerQuest() const;
	bool IsInvalidOwner() const;

	const string GetLuaTableName();

public:
	virtual bool IsNull(void) const;
	void UpdateQuestVar(int nVar);	
	int  GetQuestVar(void);

protected:	
	GPlayerQuest* m_pOwner;
};
