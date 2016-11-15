#pragma once

class GField;
class GQPTeamCountSender;
class GGlueQuestPVP;
class GNullGlueQuestPVP;
class GQPEEndInfo;
class GDBT_QPER_TOINVEN;
class GDBT_QPER_TOMAIL;

enum QUESTPVP_TEAM;




class GQuestPVP : public MTestMemPool<GQuestPVP>
{
public:
	typedef GNullGlueQuestPVP NullType;

	GQuestPVP(GField* pField);
	~GQuestPVP(void);	
	
	void Update(float fDelta);
	void BeginEvent(int nEventID);
	void EndEvent(int nEventID, QUESTPVP_TEAM nWinnerTeam);	

	void Reset();

public:
	vector<GEntityPlayer*> CollectTeam(QUESTPVP_TEAM nTeam);
	int	GetTeamCount(QUESTPVP_TEAM nTeam);	
	void RouteToQuestPVPTeam(MCommand* pNewCmd);
	void EnableMarkPos(int nNPCID);
	void DisableMarkPos(int nNPCID);

	void RewardEvent(GQPEEndInfo* pEndInfo);
	void MakeEventRewardDBT(GEntityPlayer* pPlayer, GQPEEndInfo* pEndInfo, GDBT_QPER_TOINVEN& dataToInven, GDBT_QPER_TOMAIL& dataToMail);

	bool IsDoingEvent();

	GGlueQuestPVP* GetDelegator(void);	

private:
	void CreateDelegator();		

private:
	GField*					m_pField;
	GQPTeamCountSender*		m_pQPTeamCountSender;
	GGlueQuestPVP*			m_pDelegator;

	int						m_nDoingEventID;
};
