#pragma once

class GReserveCmd;

typedef map<CID, GReserveCmd*> MAP_RESERVE_CMD;

class GReserveCmdManager : public MTestMemPool<GReserveCmdManager>
{
public:
	GReserveCmdManager(void);
	~GReserveCmdManager(void);

	void Update();

	void Reserve(vector<CID>& vecCID, MCommand* pCmd);
	void Reserve(CID nCID, MCommand* pCmd);
	void RunAndDeleteReserveCmd(CID nCID);	
	void RunReserveCmd(CID nCID);	
	void DeleteReserveCmd(CID nCID);

	GReserveCmd* GetReserveCmd(CID nCID);

private:
	void Clear();
	

private:
	MAP_RESERVE_CMD		m_mapReserveCmd;
};
