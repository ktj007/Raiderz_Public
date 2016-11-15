#ifndef _GEFF_REMAIN_TIME_SEQ_BUILDER_H
#define _GEFF_REMAIN_TIME_SEQ_BUILDER_H


class GEffRemainTimeSqlBuilder
{
public :
	enum EFF_REMAIN_COUNT
	{
		MAX_EFF_REMAIN_COUNT = 10
	};

	bool		Set(const int idx, const int ORD_NUM, const int EFF_TYPE, const int ID, const int EXPI_CHAR_PTM);
	CString&	BuildSQL(const int64 GSN, const int64 CSN);
	void		Reset();


private :
	class GEffRemainTime
	{
	public :
		GEffRemainTime() : m_ORD_NUM(0), m_EFF_TYPE(0), m_ID(0), m_EXPI_CHAR_PTM(0) {}

		void Set(const int ORD_NUM, const int EFF_TYPE, const int ID, const int EXPI_CHAR_PTM)
		{
			m_ORD_NUM		= ORD_NUM;
			m_EFF_TYPE		= EFF_TYPE;
			m_ID			= ID;
			m_EXPI_CHAR_PTM = EXPI_CHAR_PTM;
		}

		int m_ORD_NUM;
		int m_EFF_TYPE;
		int m_ID;
		int m_EXPI_CHAR_PTM;
	};

protected :
	GEffRemainTime	m_EffRemainTime[MAX_EFF_REMAIN_COUNT];
	CString			m_strSQL;
};


#endif