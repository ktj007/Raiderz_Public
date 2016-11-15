#ifndef _X_CONTAINER_H
#define _X_CONTAINER_H

template<class T>
class XContainer
{
	typedef	deque<T>	TDeque;
protected:
	MUID				m_uidOwner;

public:
	TDeque			m_vContains;

	XContainer()			{}
	virtual ~XContainer()	{}

	virtual void PushBackItem(T id)		{ m_vContains.push_back(id);	}
	virtual void PushFrontItem(T id)	{ m_vContains.push_front(id);	}
	virtual void PopBackItem()			{ m_vContains.pop_back();		}
	virtual void PopFrontItem()			{ m_vContains.pop_front();		}

	void	PopItem(T id)
	{
		TDeque::iterator itor;
		for (itor = m_vContains.begin(); itor != m_vContains.end(); ++itor) 
		{
			T curid = *itor;
			if (curid == id)
			{
				m_vContains.erase(itor);
				break;
			}
		}
	}

	T		GetFront()			{ return m_vContains.front();	}
	T		GetBack()			{ return m_vContains.back();	}

	virtual void DeleteAllItem()
	{
		TDeque::iterator itor = m_vContains.begin();
		while (itor != m_vContains.end())
		{
			itor = m_vContains.erase(itor);
		}
	}
};

class XIDContainer : public XContainer<int>
{
public:
	bool FindID(int nID)
	{
		for( size_t i =0; i < m_vContains.size(); ++i)
		{
			int id = m_vContains[i];
			if (id == nID)
			{
				return true;
			}
		}

		return false;
	}
};

#endif //_X_CONTAINER_H