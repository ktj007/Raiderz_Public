#pragma once

#define _FLUSHCHECK_TIME 10.0f
#define _FLUSH_TIME 5000

class GPlayerDBCache;

class GDBCacheRegularFlush : public MTestMemPool<GDBCacheRegularFlush>
{
public:
	GDBCacheRegularFlush(void);
	~GDBCacheRegularFlush(void);

	void Update(float fDelta);

private:
	MRegulator m_Regulator;
	void CollectFlushNeedCache(vector<GPlayerDBCache*>& vec);
	void Flush(const vector<GPlayerDBCache*>& vecCache);
};
