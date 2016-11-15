#pragma once

class GReserveCmd : public MTestMemPool<GReserveCmd>
{
public:
	GReserveCmd(MCommand* pCmd);
	~GReserveCmd(void);

public:
	void Insert(MCommand* pCmd);
	void Run();
	bool IsTimeOver(uint32 nNowTime);

private:
	void Clear();

private:
	vector<MCommand*> m_vecCmd;
	uint32 m_nReserveTime;
};
