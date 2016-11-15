#pragma once

class GReserveCmd;

typedef map<int, GReserveCmd*> MAP_RESERVE_CMD;

class GReserveCmdManager : public MTestMemPool<GReserveCmdManager>
{
public:
	GReserveCmdManager(void);
	~GReserveCmdManager(void);

	void Update();

	void Reserve(vector<int>& vecCID, MCommand* pCmd);
	void Reserve(int nCID, MCommand* pCmd);
	void RunAndDeleteReserveCmd(int nCID);	
	void RunReserveCmd(int nCID);	
	void DeleteReserveCmd(int nCID);

	GReserveCmd* GetReserveCmd(int nCID);

private:
	void Clear();
	

private:
	MAP_RESERVE_CMD		m_mapReserveCmd;
};
