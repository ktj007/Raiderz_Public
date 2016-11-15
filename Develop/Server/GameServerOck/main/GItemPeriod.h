#ifndef _GITEM_PERIOD_H
#define _GITEM_PERIOD_H

class GItemPeriod
{
public :
	GItemPeriod();
	~GItemPeriod();

	void	Set(const bool bPeriod, const bool bStartCalculatingUsagePeriod, const int nPeriodSec, const int nUsageStartCharPlayTimeSec);
	bool	IsPeriod() const;
	bool	IsExpired(const int nCharPlayTimeSec) const;
	bool	IsStartedCalculatingUsagePeriod();
	int		GetRemainTimeSec(const int nCharPlayTimeSec) const;
	int		GetPeriodSec() const;
	int		GetUsageStartCharPlayTimeSec();
	void	StartCalculatingUsagePeriod();

private :
	bool	m_bStartCalculatingUsagePeriod;

	bool	m_bPeriod;
	int		m_nPeriodSec;
	int		m_nUsageStartCharPlayTimeSec;
};

#endif