
#pragma once

#include "MTime.h"
#include <vector>
using namespace std;




namespace mint3
{



/// class : MintProfiler
class MintProfiler
{
private:
	struct MProfilerUnitInfo
	{
		MWidget*	pWidget;
		string		strEvent;
		double		fTimeBegin;
		double		fTimeEnd;

		MProfilerUnitInfo( MWidget* _widget, const char* _event, double _begin, double _end) : pWidget( _widget), fTimeBegin( _begin), fTimeEnd( _end)
		{
			if ( _event != NULL)		strEvent = _event;
		}
	};


/// Member variables
private:
	string						m_strOutFileName;
	bool						m_bBeginUpdateProfile;
	bool						m_bBeginRenderProfile;
	double						m_fUpdateProfileTimerBegin;
	double						m_fUpdateProfileTimerEnd;
	double						m_fRenderProfileTimerBegin;
	double						m_fRenderProfileTimerEnd;
	vector< MProfilerUnitInfo>	m_UpdateProfile;
	vector< MProfilerUnitInfo>	m_RenderProfile;

	// Temporary
	MTimeChecker2				m_Time;
	double						m_fBeginWidgetTime;
	double						m_fBeginEventTime;



/// Member functions
public:
	MintProfiler();
	virtual ~MintProfiler();

	bool Create( const char* szOutFileName);

	void BeginUpdateProfile();
	bool IsBeginedUpdateProfile();
	void WriteUpdateProfileBegin( MWidget* pWidget);
	void WriteUpdateProfileBegin( const char* szEvent);
	void WriteUpdateProfileEnd( MWidget* pWidget);
	void WriteUpdateProfileEnd( const char* szEvent);
	void EndUpdateProfile();

	void BeginRenderProfile();
	void EndRenderProfile();
	bool IsBeginedRenderProfile();
	void WriteRenderProfileBegin( MWidget* pWidget);
	void WriteRenderProfileBegin( const char* szEvent);
	void WriteRenderProfileEnd( MWidget* pWidget);
	void WriteRenderProfileEnd( const char* szEvent);

	void OutputProfileText();

	void Clear();

	static MintProfiler* GetInstance();


protected:
	string GetName( MWidget* pWidget);
};



// inline : MGetMintProfiler
inline MintProfiler* MGetMintProfiler()
{
	return MintProfiler::GetInstance();
}


} // namespace mint3