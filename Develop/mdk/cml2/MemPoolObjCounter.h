#pragma once

// 메모리 풀(MMemPool)의 수를 세기 위해 만든 풀 카운팅 클래스.
#include <assert.h>
#include "MSync.h"
#include <string>
#include <map>

class CML2_API MemPoolObjCounter : public MClassLevelLockable<MemPoolObjCounter>
{
public:
	class IMemPool
	{
		friend class MemPoolObjCounter;
		virtual long Count() = 0;
	};

	class IVisitor
	{
		friend class MemPoolObjCounter;
		virtual void OnVisit(const std::string& objName,const int& objSize,const int& objCnt) = 0;
	};

public:
	MemPoolObjCounter() { m_startCheck = true; }
	virtual ~MemPoolObjCounter() { }

	void StopChecking() { m_startCheck = false; }

	template<class T>
	void Register(IMemPool* const p)
	{
		assert(NULL!=p);
		Lock lock;
		if (m_objs.end() != m_objs.find(p))
		{
			return;
		}

		MEM_POOL pool;
		pool.name = typeid(T).name();
		pool.objSize = sizeof(T);
		pool.pool = p;
		m_objs.insert(OBJS::value_type(p, pool));
	}

	void Visit(IVisitor& visitor)
	{
		if (false == m_startCheck)
		{
			return;
		}

		OBJS clon;
		{
			Lock lock;
			clon = m_objs;
		}
		for each (const OBJS::value_type pair in clon)
		{
			const MEM_POOL& obj(pair.second);
			assert(NULL!=obj.pool);
			visitor.OnVisit(obj.name, obj.objSize, obj.pool->Count());
		}
	}

	static MemPoolObjCounter& Instance()
	{
		return mempoolCounter;
	}

private:
	struct MEM_POOL
	{
		std::string	name;
		IMemPool*	pool;
		int			objSize;
	};
	typedef std::map<IMemPool*,MEM_POOL> OBJS;
	OBJS	m_objs;
	bool	m_startCheck;

	static MemPoolObjCounter mempoolCounter;
};