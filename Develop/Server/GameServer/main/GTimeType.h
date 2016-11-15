#ifndef _GTIME_TYPE_H
#define _GTIME_TYPE_H


class GTimeType
{
public :
	GTimeType(const time_t tOffset) : m_tOffset(tOffset) {}
	virtual ~GTimeType() {}

	virtual time_t GetSec() const = 0;

	time_t GetOffset() const { return m_tOffset; }

private :
	time_t m_tOffset;
};

class GTimeSec : public GTimeType
{
public :
	GTimeSec(const time_t nSec) : GTimeType(nSec) {}

	time_t GetSec() const  override 
	{
		return GetOffset();
	}
};

class GTimeDay : public GTimeType
{
public :
	GTimeDay(const time_t nDay) : GTimeType(nDay) {}

	time_t GetSec() const override
	{
		return ONE_DAY * GetOffset();
	}	

	static const time_t ONE_DAY = 24 * 60 * 60;
};


#endif