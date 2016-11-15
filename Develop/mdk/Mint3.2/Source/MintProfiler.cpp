#include "stdafx.h"
#include "MintProfiler.h"



namespace mint3
{


//	MintProfiler
MintProfiler::MintProfiler()
{
	m_Time.Start();

	m_bBeginUpdateProfile = false;
	m_bBeginRenderProfile = false;
	m_fUpdateProfileTimerBegin = 0.0;
	m_fUpdateProfileTimerEnd = 0.0;
	m_fRenderProfileTimerBegin = 0.0;
	m_fRenderProfileTimerEnd = 0.0;
}


// ~MintProfiler
MintProfiler::~MintProfiler()
{
	Clear();
}


// Create
bool MintProfiler::Create( const char* szOutFileName)
{
	m_strOutFileName = szOutFileName;

	Clear();

	return true;
}


// BeginUpdateProfile
void MintProfiler::BeginUpdateProfile()
{
	if ( m_bBeginUpdateProfile == true)			return;

	m_UpdateProfile.clear();

	m_bBeginUpdateProfile = true;
	m_fUpdateProfileTimerBegin = m_Time.Check();
}


// EndUpdateProfile
void MintProfiler::EndUpdateProfile()
{
	if ( m_bBeginUpdateProfile == false)		return;
	m_bBeginUpdateProfile = false;
	m_fUpdateProfileTimerEnd = m_Time.Check();
}


// IsBeginedUpdateProfile
bool MintProfiler::IsBeginedUpdateProfile()
{
	return m_bBeginUpdateProfile;
}


// WriteUpdateProfileBegin
void MintProfiler::WriteUpdateProfileBegin( MWidget* pWidget)
{
	if ( m_bBeginUpdateProfile == true)
		m_fBeginWidgetTime = m_Time.Check();
}


// WriteUpdateProfileBegin
void MintProfiler::WriteUpdateProfileBegin( const char* szEvent)
{
	if ( m_bBeginUpdateProfile == true)
		m_fBeginEventTime = m_Time.Check();
}


// WriteUpdateProfileEnd
void MintProfiler::WriteUpdateProfileEnd( MWidget* pWidget)
{
	if ( m_bBeginUpdateProfile == true)
		m_UpdateProfile.push_back( MProfilerUnitInfo( pWidget, NULL, m_fBeginWidgetTime, m_Time.Check()));
}


// WriteUpdateProfileEnd
void MintProfiler::WriteUpdateProfileEnd( const char* szEvent)
{
	if ( m_bBeginUpdateProfile == true)
		m_UpdateProfile.push_back( MProfilerUnitInfo( NULL, szEvent, m_fBeginEventTime, m_Time.Check()));
}


// BeginRenderProfile
void MintProfiler::BeginRenderProfile()
{
	if ( m_bBeginRenderProfile == true)			return;

	m_RenderProfile.clear();

	m_bBeginRenderProfile = true;
	m_fRenderProfileTimerBegin = m_Time.Check();
}


// EndRenderProfile
void MintProfiler::EndRenderProfile()
{
	if ( m_bBeginRenderProfile == false)		return;
	m_bBeginRenderProfile = false;
	m_fRenderProfileTimerEnd = m_Time.Check();
}


// IsBeginedRenderProfile
bool MintProfiler::IsBeginedRenderProfile()
{
	return m_bBeginRenderProfile;
}


// WriteRenderProfileBegin
void MintProfiler::WriteRenderProfileBegin( MWidget* pWidget)
{
	if ( m_bBeginRenderProfile == true)
		m_fBeginWidgetTime = m_Time.Check();
}


// WriteRenderProfileBegin
void MintProfiler::WriteRenderProfileBegin( const char* szEvent)
{
	if ( m_bBeginRenderProfile == true)
		m_fBeginEventTime = m_Time.Check();
}


// WriteRenderProfileEnd
void MintProfiler::WriteRenderProfileEnd( MWidget* pWidget)
{
	if ( m_bBeginRenderProfile == true)
		m_RenderProfile.push_back( MProfilerUnitInfo( pWidget, NULL, m_fBeginWidgetTime, m_Time.Check()));
}


// WriteRenderProfileEnd
void MintProfiler::WriteRenderProfileEnd( const char* szEvent)
{
	if ( m_bBeginRenderProfile == true)
		m_RenderProfile.push_back( MProfilerUnitInfo( NULL, szEvent, m_fBeginEventTime, m_Time.Check()));
}


// Clear
void MintProfiler::Clear()
{
	m_UpdateProfile.clear();
	m_RenderProfile.clear();
	m_bBeginUpdateProfile = false;
	m_bBeginRenderProfile = false;
	m_fUpdateProfileTimerBegin = 0.0;
	m_fUpdateProfileTimerEnd = 0.0;
	m_fRenderProfileTimerBegin = 0.0;
	m_fRenderProfileTimerEnd = 0.0;
}


// OutputProfileText
void MintProfiler::OutputProfileText()
{
	FILE* pFile;
	fopen_s( &pFile, m_strOutFileName.c_str(), "w");
	if ( pFile == NULL)				return;


	fprintf_s( pFile, "Mint Profiler\n\n");


	// Update profile
	if ( m_UpdateProfile.empty() == false)
	{
		fprintf_s( pFile, "[ Update profile ]\n\n");
		fprintf_s( pFile, "* Total window count : %d\n", m_UpdateProfile.size());
		fprintf_s( pFile, "* Start of update time : %lf msec\n", m_fUpdateProfileTimerBegin);

		for ( vector< MProfilerUnitInfo>::iterator itr = m_UpdateProfile.begin();  itr != m_UpdateProfile.end();  itr++)
		{
			double fBegin = (*itr).fTimeBegin;
			double fEnd = (*itr).fTimeEnd;
			double fElapsed = fEnd - fBegin;

			if ( (*itr).pWidget != NULL)
			{
				string strName = GetName( (*itr).pWidget);
				fprintf_s( pFile, " [%c]  %lf msec (%lf ~ %lf)   %s\n", (fElapsed >= 3.0) ? 'v' : ' ', fElapsed, fBegin, fEnd, strName.c_str());
			}
			else if ( (*itr).strEvent.empty() == false)
			{
				fprintf_s( pFile, " [%c]  %lf msec (%lf ~ %lf)        Event : %s\n", (fElapsed >= 3.0) ? 'v' : ' ', fElapsed, fBegin, fEnd, (*itr).strEvent.c_str());
			}
		}
		fprintf_s( pFile, "* End of update time : %lf msec\n", m_fUpdateProfileTimerEnd);
		fprintf_s( pFile, "* Total update time : %lf msec\n", m_fUpdateProfileTimerEnd - m_fUpdateProfileTimerBegin);
	}


	fprintf_s( pFile, "\n\n\n");


	// Render profile
	if ( m_RenderProfile.empty() == false)
	{
		fprintf_s( pFile, "[ Render profile ]\n\n");
		fprintf_s( pFile, "* Total window count : %d\n", m_RenderProfile.size());
		fprintf_s( pFile, "* Start of render time : %lf msec\n", m_fRenderProfileTimerBegin);

		for ( vector< MProfilerUnitInfo>::iterator itr = m_RenderProfile.begin();  itr != m_RenderProfile.end();  itr++)
		{
			double fBegin = (*itr).fTimeBegin;
			double fEnd = (*itr).fTimeEnd;
			double fElapsed = fEnd - fBegin;

			if ( (*itr).pWidget != NULL)
			{
				string strName = GetName( (*itr).pWidget);
				fprintf_s( pFile, " [%c]  %lf msec (%lf ~ %lf)   %s\n", (fElapsed >= 3.0) ? 'v' : ' ', fElapsed, fBegin, fEnd, strName.c_str());
			}
			else if ( (*itr).strEvent.empty() == false)
			{
				fprintf_s( pFile, " [%c]  %lf msec (%lf ~ %lf)        Event : %s\n", (fElapsed >= 3.0) ? 'v' : ' ', fElapsed, fBegin, fEnd, (*itr).strEvent.c_str());
			}
		}
		fprintf_s( pFile, "* End of render time : %lf msec\n", m_fRenderProfileTimerEnd);
		fprintf_s( pFile, "* Total render time : %lf msec\n", m_fRenderProfileTimerEnd - m_fRenderProfileTimerBegin);
	}


	fclose( pFile);
}


// GetInstance
MintProfiler* MintProfiler::GetInstance()
{
	static MintProfiler instance;
	return &instance;
}


// GetName
string MintProfiler::GetName( MWidget* pWidget)
{
	string strFullName = (pWidget->GetName().empty() ? "<noname>" : pWidget->GetName()) + "(" + pWidget->GetClassName() + ")";

	MWidget* _widget = pWidget;
	while ( _widget->GetParent() != NULL)
	{
		string strName = (_widget->GetParent()->GetName().empty() ? "<noname>" : _widget->GetParent()->GetName()) + "(" + _widget->GetParent()->GetClassName() + ")";
		strFullName = strName + "/" + strFullName;
		_widget = _widget->GetParent();
	}

	return strFullName;
}


} // namespace mint3

