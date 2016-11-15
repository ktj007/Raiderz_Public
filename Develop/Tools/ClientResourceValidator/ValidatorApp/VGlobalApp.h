#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;

namespace ValidatorApp {

public ref class CLoadingProgress
{
public:
	int				m_nIndex;
	String^			m_strText;

public:
	CLoadingProgress() 
	{
		m_nIndex		= 0;
	};

	~CLoadingProgress() {}

	virtual int GetHashCode() override 
	{
		return m_nIndex;
	}

	virtual String^ ToString() override 
	{
		return m_strText;
	}

};

public ref class CProgressCheck
{
public:
	int									m_nFullCompleteCount;
	int									m_nCurrCompleteCount;
	int									m_nMsgIndex;

	BackgroundWorker^					m_BackgrounWoker;

public:
	CProgressCheck() 
	{
		m_nFullCompleteCount = 0;
		m_nCurrCompleteCount = 0;
		m_nMsgIndex			 = 0;
	}

	~CProgressCheck() {}

	void SetInfo(BackgroundWorker^ worker, int nFullCount, int m_nMsgIndex)
	{
		m_BackgrounWoker = worker;
		m_nFullCompleteCount = nFullCount;
		m_nMsgIndex = m_nMsgIndex;
	}
};

void ReportProgress(BackgroundWorker^ worker, int nIndex, int nLoadingCompletePercent, String^ strText);

}