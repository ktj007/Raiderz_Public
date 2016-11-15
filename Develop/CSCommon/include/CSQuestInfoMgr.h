#pragma once

#include "CSCommonLib.h"

class CSQuestInfo;
class CSQObjectiveInfo;

typedef map<int, CSQuestInfo*> MAP_QUESTINFO;
typedef multimap<int, CSQObjectiveInfo*> MULTIMAP_QOBJECTIVEINFO;


class CSCOMMON_API CSQuestInfoMgr
{
public:
	class IVisitor {
		friend class CSQuestInfoMgr;
		virtual void OnVisit(const CSQuestInfo& questInfo,bool& continueVisit) = 0;
	};

public:
	CSQuestInfoMgr();
	virtual ~CSQuestInfoMgr();

	void Clear();
	bool Load(const TCHAR* szFileName);
	bool Reload(const TCHAR* szFileName);

	virtual CSQuestInfo* NewQuestInfo() = 0;
	virtual CSQObjectiveInfo* NewQObjectiveInfo() = 0;
	void Insert(CSQuestInfo* pQuestInfo);
	bool IsExist(int nQuestID);		

	vector<CSQObjectiveInfo*> GetQObjectiveByParam1(int nQObjectiveType, int nQObjectiveParam1);
	vector<int> GetAllQuestID();

	bool IsCollectItem(int nItemID);
	bool IsCollectItem(const vector<int>& vecQuestID, int nItemID);

	bool IsDestroyNPC(int nNPCID);	
	bool IsExistIncompleteDialog(int nQusetID);

	void MakeQOInfoCache_Destroy(int nParam1, CSQObjectiveInfo* pQObjectiveInfo);
	void MakeQOInfoCache_Collect(int nParam1, CSQObjectiveInfo* pQObjectiveInfo);
	void MakeQOInfoCache_Scout(int nParam1, CSQObjectiveInfo* pQObjectiveInfo);
	void MakeQOInfoCache_Escort(int nParam1, CSQObjectiveInfo* pQObjectiveInfo);
	virtual void Cooking()	{}
	void Visit(IVisitor& visitor) const;

protected :
	virtual void ParseQuest(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pInfo);
	virtual void ParseQRewards(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo);
	CSQuestInfo* Get(int nID);	

private:
	void ParseQObjectives(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo);
	void ParseQObjective(MXmlElement* pElement, MXml* pXml, CSQObjectiveInfo* pQObjectiveInfo);	
	void ParseQReward(MXmlElement* pElement, MXml* pXml, CSQuestInfo* pQuestInfo);	

protected:
	MAP_QUESTINFO			m_mapQuestInfo;	

private:
	MULTIMAP_QOBJECTIVEINFO	m_multimapDestroyQObjective;
	MULTIMAP_QOBJECTIVEINFO	m_multimapCollectQObjective;
	MULTIMAP_QOBJECTIVEINFO	m_multimapScoutQObjective;
	MULTIMAP_QOBJECTIVEINFO	m_multimapEscortQObjective;
};