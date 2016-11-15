#pragma once

#include "MADODatabase.h"
#include <process.h>

const int MAX_STRING_LENGTH_TIME	= 32;

class MDBPressor
{
public:
	MDBPressor();
	virtual						~MDBPressor();

	virtual BOOL				Init(	CString strDataSource,
										CString strCatalog,
										CString strID,
										CString strPassword,
										CString strPoolSize );

	void						Destroy();

	virtual BOOL				StartPress();
	BOOL						IsEndPress()						{ return m_bIsEndPress;					}
	virtual void				EndPress();
	BOOL						IsEndTest()							{ return m_bEndTest;					}
	virtual void				CatchEndTest()						{ m_bEndTest	= FALSE;				}

	int							GetCurrentTestNum()					{ return m_nCurrentTestNum;				}
	const char*					GetStartTestTime()					{ return m_szStartTestTime;				}
	const char*					GetEndTestTime()					{ return m_szEndTestTime;				}
	void						SetTestQuery(CString strSQL)		{ m_strTestQueryString = strSQL;		}
	const char*					GetTestQuery()						{ return m_strTestQueryString.GetString();}
	void						SetExecuteCount(int nExecuteCount)	{ m_nExecuteCount = nExecuteCount;		}
	int							GetExecuteCount()					{ return m_nExecuteCount;				}
	int							GetCompleteCount()					{ return m_nCompleteCount;				}
	int							GetFailCount()						{ return m_nFailCount;					}
	double						GetElapsedTime()					{ return m_dElapsedTime;				}

	static MDBPressor*			GetInstance()						{ return m_pDBPressor;					}

protected:
	MADODatabase				m_MDatabase;
	BOOL						m_bIsEndPress;
	HANDLE						m_hExecuteThreadHandle;
	BOOL						m_bEndTest;
	char						m_szEndTestTime[MAX_STRING_LENGTH_TIME];
	LARGE_INTEGER				m_lnFreq, m_lnStart, m_lnEnd;
	double						m_dElapsedTime;
	int							m_nCompleteCount;
	int							m_nFailCount;

	virtual void				Reset();
	void						StartTimer();
	void						EndTimer();
	void						AsyncExecute();
	void						IncreaseCompleteCount()				{ ++m_nCompleteCount;					}
	void						IncreaseFailCount()					{ ++m_nFailCount;						}
	virtual BOOL				IsStopExecThread()					{ return m_bDestroyExecuteThread;		}

private:
	static int					m_nCurrentTestNum;
	static MDBPressor*			m_pDBPressor;
	char						m_szStartTestTime[MAX_STRING_LENGTH_TIME];
	BOOL						m_bDestroyExecuteThread;
	CString						m_strTestQueryString;
	int							m_nExecuteCount;
	int							m_nProcessCountPerSec;

	static unsigned __stdcall	ExecuteThread(void* pArg);
	static void					OnComplete(unsigned int nKey, MRecordset* pRs, char* szError);

};

inline MDBPressor* MGetDBPressor() { return MDBPressor::GetInstance();	}
