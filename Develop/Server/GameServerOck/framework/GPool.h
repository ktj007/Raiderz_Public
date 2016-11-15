#ifndef _GPOOL_H
#define _GPOOL_H

#include <list>
#include <vector>
using namespace std;

///
class GPoolNode
{
private:
	bool			m_bUsed;
	int				m_nPoolIndex;
protected:
	virtual void	OnUse()		= 0;
	virtual void	OnDrop()	= 0;
public:
					GPoolNode() : m_bUsed(false), m_nPoolIndex(-1) {}
	virtual			~GPoolNode() {}

	void			Use();
	void			Drop();
	void			SetPoolIndex(int n)		{ m_nPoolIndex=n; }
	int				GetPoolIndex()			{ return m_nPoolIndex; }
	bool			IsUsed()				{ return m_bUsed; }
};

/// 
template <typename T>
class GPool
{
protected:
	list<int>			m_NotUsed;
	vector<T>			m_List;
	int					m_nCursor;
	int					m_nSize;
	CRITICAL_SECTION	m_csLock;
	void				_Lock()		{ EnterCriticalSection( &m_csLock ); }
	void				_Unlock()	{ LeaveCriticalSection( &m_csLock ); }
public:
	GPool() : m_nSize(0), m_nCursor(0)
	{
		InitializeCriticalSection( &m_csLock );
	}

	~GPool()
	{
		DeleteCriticalSection( &m_csLock );
	}

	void Init(int nSize)
	{
		_Lock();

		m_List.clear();
		m_NotUsed.clear();
		m_nCursor = 0;

//		m_List.reserve(nSize);
		m_List.resize(nSize);
		m_nSize = nSize;

		for (int i = 0; i < nSize; i++)
		{
			m_List[i].SetPoolIndex(i);
		}

		_Unlock();
	}

	T* Use()
	{
		if (!m_NotUsed.empty())
		{
			_Lock();

			list<int>::iterator itorBegin = m_NotUsed.begin();
			int index = *itorBegin;
			m_NotUsed.erase(itorBegin);

			_ASSERT(m_List[index].IsUsed() == false);

			_Unlock();

			return &m_List[index];
		}

		if (m_nCursor >= m_nSize)
		{
			_Lock();

			T node;
			node.SetPoolIndex(m_nSize);
			m_List.push_back(node);

			m_nSize++;
			m_nCursor++;

			int index = m_nSize-1;

			_Unlock();

			return &m_List[index];
		}

		_Lock();

		int index = m_nCursor;
		m_nCursor++;

		_Unlock();
		
		return &m_List[index];
	}

	void Drop(T* pNode)
	{
		_Lock();

		int index = pNode->GetPoolIndex();
		m_NotUsed.push_back(index);

		_Unlock();
	}

	int GetSize()		{ return m_nSize; }
};



#endif