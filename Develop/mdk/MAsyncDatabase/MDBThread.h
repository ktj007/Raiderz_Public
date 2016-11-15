#pragma once


#include <string>
using namespace std;

#include "MDatabase.h"


namespace mdb
{
	class MDBThreadTaskQ;
	class MDBThreadTaskResultQ;


	class MDBThread	
	{
	public :
		MDBThread();
		virtual ~MDBThread();


		bool					Create(const wstring& strDSN
									, MDBThreadTaskQ* pTaskQ
									, MDBThreadTaskResultQ* pTaskResultQ);

		HANDLE					GetHandle();
		DWORD					GetThreadID();

		
		bool					IsExit();
		void					SetExit();
		bool					IsExitCompleted();
		void					SetExitCompleted();		


		void					Run();


		void					Release();

	protected :
		void					DecDBReconnectCount();
		void					IncDBReconnectCount();
		int						GetDBReconnectCount();
		bool					IsDBConnected();

	private :
		static DWORD WINAPI		_WorkThread(LPVOID pContext);

	private :
		static const int		MAX_RECONNECT_COUNT = 3;

		HANDLE					m_hThread;
		DWORD					m_dwThreadID;

		MDatabase				m_DB;
		MDBThreadTaskQ*			m_pTaskQ;
		MDBThreadTaskResultQ*	m_pTaskResultQ;

		bool					m_bIsExit;
		bool					m_bIsExitCompleted;

		int						m_nDBReconnectCount;
	};
}