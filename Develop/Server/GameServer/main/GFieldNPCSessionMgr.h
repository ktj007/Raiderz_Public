#pragma once

class GFieldNPCSession;

class GFieldNPCSessionMgr : public MTestMemPool<GFieldNPCSessionMgr>
{
public:
	GFieldNPCSessionMgr(void);
	~GFieldNPCSessionMgr(void);

	bool Add( GFieldNPCSession* pSession );
	void Remove(MUID uid);
	size_t GetQty() const;
	void Update(float fDelta);
	void Clear();
	GFieldNPCSession* Find(const char* pszName);
	GFieldNPCSession* Find(MUID uid);

private:
	typedef map<MUID,GFieldNPCSession*>  SessionMap;

	SessionMap m_mapSessions;
};
