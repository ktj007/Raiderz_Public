#pragma once

//////////////////////////////////////////////////////////////////////////
class GEntityPlayer;
class GCondition_Leaf;
class GCondition_Composite;
class CSCondition_Component;

class GConditionChecker
{
protected:
	bool Level(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Quest(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool QuestVar(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool QuestObjective_Complete(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool QuestObjective_Incomplete(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Item(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool NoItem(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool FewItem(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool ManyItem(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);

	bool Equip(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Race(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Sex(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Date(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Talent(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Talent_Style_TP(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Faction(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool Kill( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer );
	bool QPVP_Team( GCondition_Leaf* pCondition, GEntityPlayer* pPlayer );

	bool CheckLeaf(GCondition_Leaf* pCondition, GEntityPlayer* pPlayer);
	bool CheckComposite(GCondition_Composite* pCondition, GEntityPlayer* pPlayer, vector<GCondition_Leaf*>& vecFailConditionLeaf);
public:
	bool Check(CSCondition_Component* pCondition, GEntityPlayer* pPlayer, vector<GCondition_Leaf*>& vecFailConditionLeaf=vector<GCondition_Leaf*>());
};

