#ifndef _CACHE_VALUE_TYPE_H
#define _CACHE_VALUE_TYPE_H


template<typename T>
class cache_value_type
{
public :
	cache_value_type(const T MaxModifyRange) 
		: m_Increase(0), m_Decrease(0), m_nModifyCnt(0), m_MaxModifyRange(MaxModifyRange) {}
	
	void Increase(const T IncValue)
	{
		if (!IncreaseModifyCnt(IncValue))
			return;

		m_Increase	+= IncValue;				
	}

	void Decrease(const T DecValue)
	{
		if (!IncreaseModifyCnt(DecValue))
			return;

		m_Decrease	+= DecValue;		
	}

	bool IsOverflowMaxModifyRange() const
	{ 
		if (m_MaxModifyRange <= m_Increase || m_MaxModifyRange <= m_Decrease)
			return true;

		if (m_MaxModifyRange - m_Increase <= m_Decrease) 
			return true;

		return false;
	}

	bool IncreaseModifyCnt(const T Value)	
	{ 
		if (0 == Value) 
			return false; 

		++m_nModifyCnt; 
		return true;
	}


	T		GetIncrease()						{ return m_Increase; }
	T		GetDecrease()						{ return m_Decrease; }

	T		GetMaxModifyRange()					{ return m_MaxModifyRange; }
	size_t	GetModifiedCnt()					{ return m_nModifyCnt; }

	T		GetIncrease()		const			{ return m_Increase; }
	T		GetDecrease()		const			{ return m_Decrease; }

	T		GetMaxModifyRange()	const			{ return m_MaxModifyRange; }
	size_t	GetModifiedCnt()	const			{ return m_nModifyCnt; }
	
	void	SetZero()							{ m_Increase = 0; m_Decrease = 0; m_nModifyCnt = 0; }

	
private :
	T		m_Increase;
	T		m_Decrease;
	size_t	m_nModifyCnt;
	const T m_MaxModifyRange;	
};

typedef cache_value_type<int>		cache_Money;
typedef cache_value_type<int>		cache_XP;
typedef cache_value_type<int16>		cache_StackAmt;
typedef cache_value_type<uint8>		cache_Dura;

#endif