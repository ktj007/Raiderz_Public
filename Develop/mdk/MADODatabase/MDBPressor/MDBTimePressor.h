#pragma once

#include "MDBPressor.h"

class MDBTimePressor : public MDBPressor
{
public:
	MDBTimePressor();
	virtual						~MDBTimePressor();

	virtual BOOL				Init(	CString strDataSource,
										CString strCatalog,
										CString strID,
										CString strPassword,
										CString strPoolSize );

	BOOL						StartPress();
	void						EndPress();
	void						SetTimer(unsigned int nWholeTIme, unsigned int nTerm);
	virtual void				CatchEndTest();

	int							GetWholeTime()						{ return m_nWholeTime;					}
	int							GetTerm()							{ return m_nTerm;						}

	static MDBTimePressor*		GetInstance()						{ return m_pDBPressor;					}

protected:

private:
	static MDBTimePressor*		m_pDBPressor;

	unsigned int				m_nWholeTime;
	unsigned int				m_nTerm;
	BOOL						m_bIsStartExecute;
	HANDLE						m_hExecuteEvent;

	virtual void				Reset();

	virtual void				StartTimer();
	virtual void				EndTimer();

	static unsigned __stdcall	ExecuteThread(void* pArg);
	virtual BOOL				IsStopExecThread()					{ return MDBPressor::IsStopExecThread();}
	static void					OnComplete(unsigned int nKey, MRecordset* pRs, char* szError);

};

inline MDBTimePressor* MGetDBTimePressor() { return MDBTimePressor::GetInstance();	}
