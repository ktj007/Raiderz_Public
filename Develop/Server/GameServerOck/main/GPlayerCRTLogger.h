#pragma once

class GEntityPlayer;
enum CCommandResultTable;

class GPlayerCRTLogger
{
private:
	set<CCommandResultTable> m_setLoggableCRT;

private:
	void RouteLogCRTList(GEntityPlayer* pPlayer);

public:
	void Init();

	void Insert(GEntityPlayer* pPlayer, CCommandResultTable nCRT);
	void Delete(GEntityPlayer* pPlayer, CCommandResultTable nCRT);	

	void Log(GEntityPlayer* pPlayer, CCommandResultTable nCRT);

	set<CCommandResultTable>& GetContainer() { return m_setLoggableCRT; }
};
