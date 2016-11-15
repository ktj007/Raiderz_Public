#pragma once


struct XMYBUFFITEM
{
	uint32		_begin_t;					// 시작 시간
	uint32		_lost_t;					// 종료 시간

	XMYBUFFITEM( uint32 _begin, uint32 _lost) : _begin_t( _begin), _lost_t( _lost)  {}
};

struct XMYBUFFLIST
{
	BUFF_STACK_TYPE				eStackType;
	vector<XMYBUFFITEM>			vecBuffItem;

	XMYBUFFLIST(BUFF_STACK_TYPE _type, uint32 _begin, uint32 _lost)
	{
		eStackType				= _type;

		PushBuffItem(_begin, _lost);
	}

	void PushBuffItem(uint32 _begin, uint32 _lost)
	{
		vecBuffItem.push_back(XMYBUFFITEM(_begin, _lost));
	}

	void PopBuffItem()
	{
		// 항상 앞쪽부터 뺀다.
		vector<XMYBUFFITEM>::iterator it = vecBuffItem.begin();
		if(it != vecBuffItem.end())
			vecBuffItem.erase(it);
	}
};


class XMyBuffContainer
{
public:
	map< int, XMYBUFFLIST>		m_vContains;

private:
	void GetCalRemainedTimeBuffItem(XMYBUFFLIST* pBuffList, uint32& _begin, uint32& _lost)
	{
		int nSize = pBuffList->vecBuffItem.size();
		if(nSize == 0)
			return;

		switch(pBuffList->eStackType)
		{
		case BUFFSTACK_NONE:
		case BUFFSTACK_STACK_UPDATE:
			{
				// 제일 마지막 아이템 시간을 주면 된다
				_begin = pBuffList->vecBuffItem[nSize - 1]._begin_t;
				_lost = pBuffList->vecBuffItem[nSize - 1]._lost_t;
				return;
			}
		case BUFFSTACK_STACK:
			{
				// 제일 처음 아이템 시간을 주면 된다.
				_begin = pBuffList->vecBuffItem[0]._begin_t;
				_lost = pBuffList->vecBuffItem[0]._lost_t;
				return;
			}
		}
	}

public:
	XMyBuffContainer()				{}
	virtual ~XMyBuffContainer ()	{ Clear(); }

	size_t Size()					{ return m_vContains.size(); }

	void Clear()					{ m_vContains.clear(); }

	XMYBUFFLIST* GetBuffContain(int id)
	{
		map< int, XMYBUFFLIST>::iterator itFind = m_vContains.find(id);
		if(itFind != m_vContains.end())
			return &(itFind->second);
		return NULL;
	}

	void PushBackItem( int id, BUFF_STACK_TYPE stackType, float remained)
	{
		if ( remained <= 0.0f)
		{
			m_vContains.insert( map< int, XMYBUFFLIST>::value_type( id, XMYBUFFLIST( stackType, 0, 0)) );
		}
		else
		{
			uint32 _begin_t = global.system->GetTimer().GetNowGlobalTime();
			uint32 _lost_t = global.system->GetTimer().GetNowGlobalTime() + (uint32)(remained * 1000.0f);
			m_vContains.insert( map< int, XMYBUFFLIST>::value_type( id, XMYBUFFLIST( stackType, _begin_t, _lost_t)));
		}		
	}

	void PopItem( int id)
	{
		map< int, XMYBUFFLIST>::iterator itr = m_vContains.find( id);
		if ( itr == m_vContains.end())  return;
		m_vContains.erase( id);
	}

	void PushStackItem(int id, float remained)
	{
		XMYBUFFLIST* pBuffList = GetBuffContain(id);
		if(pBuffList == NULL)
			return;

		if ( remained <= 0.0f)
		{
			pBuffList->PushBuffItem(0, 0);
		}
		else
		{
			uint32 _begin_t = global.system->GetTimer().GetNowGlobalTime();
			uint32 _lost_t = global.system->GetTimer().GetNowGlobalTime() + (uint32)(remained * 1000.0f);
			pBuffList->PushBuffItem(_begin_t, _lost_t);
		}
	}

	void PopStackItem(int id)
	{
		XMYBUFFLIST* pBuffList = GetBuffContain(id);
		if(pBuffList == NULL)
			return;

		pBuffList->PopBuffItem();
	}

	const wchar_t* GetItemIDList( wstring& strText)
	{
		wchar_t buff[ 512] = {0,};
		bool first = true;
		for ( map< int, XMYBUFFLIST>::iterator itr = m_vContains.begin();  itr != m_vContains.end();  itr++)
		{
			_itow_s( (*itr).first, buff, 10);
			if ( first == true)
			{
				strText = L"";
				first = false;
			}
			else
				strText += L",";
			strText += buff;
		}
		return strText.c_str();
	}

	int GetItemID( int index)
	{
		map< int, XMYBUFFLIST>::iterator itr = m_vContains.begin();
		for ( int i = 0;  i < index;  i++, itr++)	;
		return (*itr).first;
	}

	bool FindID( int nID)	{ return ( m_vContains.find( nID) != m_vContains.end()) ? true : false; }

	long GetRemainedTime( int nID)
	{
		map< int, XMYBUFFLIST>::iterator itr = m_vContains.find( nID);
		if ( itr == m_vContains.end())  return false;

		uint32 _begin_t = 0;
		uint32 _lost_t = 0;
		GetCalRemainedTimeBuffItem(&(itr->second), _begin_t, _lost_t);
		if ( _begin_t == 0  &&  _lost_t == 0)	return -1;

		uint32 _curr_t = global.system->GetTimer().GetNowGlobalTime();
		if ( _curr_t > _lost_t)					return 0;

		return (long)(_lost_t - _curr_t);
	}

	float GetRemainedRatio( int nID)
	{
		map< int, XMYBUFFLIST>::iterator itr = m_vContains.find( nID);
		if ( itr == m_vContains.end())  return false;

		uint32 _begin_t = 0;
		uint32 _lost_t = 0;
		GetCalRemainedTimeBuffItem(&(itr->second), _begin_t, _lost_t);

		if ( _begin_t == 0  &&  _lost_t == 0)	return 1.0;

		uint32 _curr_t = global.system->GetTimer().GetNowGlobalTime();
		if ( _curr_t > _lost_t)					return 0.0f;

		return ((float)(_lost_t - _curr_t) / (float)(_lost_t - _begin_t));
	}

	bool IsValidTime( int nID)
	{
		map< int, XMYBUFFLIST>::iterator itr = m_vContains.find( nID);
		if ( itr == m_vContains.end())  return false;

		uint32 _begin_t = 0;
		uint32 _lost_t = 0;
		GetCalRemainedTimeBuffItem(&(itr->second), _begin_t, _lost_t);
		if ( _begin_t == 0  &&  _lost_t == 0)	return true;

		uint32 _curr_t = global.system->GetTimer().GetNowGlobalTime() + 200;
		if ( _curr_t < _lost_t)					return true;

		return false;
	}

	int GetItemCount(int nID)
	{
		XMYBUFFLIST* pBuffList = GetBuffContain(nID);
		if(pBuffList == NULL)
			return 0;

		return pBuffList->vecBuffItem.size();
	}
};