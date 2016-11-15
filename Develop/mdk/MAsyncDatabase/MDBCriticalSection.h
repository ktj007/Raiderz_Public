#pragma once


namespace mdb
{
	class MDBCriticalSecton
	{
	public :
		explicit MDBCriticalSecton()
		{
			InitializeCriticalSection(&m_CS);
		}

		~MDBCriticalSecton()
		{
			DeleteCriticalSection(&m_CS);
		}

		void Lock()			{ EnterCriticalSection(&m_CS); }
		void Unlock()		{ LeaveCriticalSection(&m_CS); }

	private :
		CRITICAL_SECTION m_CS;
	};


	class MDBGuard
	{
	public :
		explicit MDBGuard(MDBCriticalSecton* pCS) : m_pCS(pCS) 
		{
			_ASSERT(NULL != m_pCS);

			if (NULL != m_pCS)
			{
				m_pCS->Lock();
			}			
		}

		~MDBGuard()
		{
			_ASSERT(NULL != m_pCS);

			if (NULL != m_pCS)
			{
				m_pCS->Unlock();
			}			
		}


	private :
		MDBGuard() { _ASSERT(0); }


	private :
		MDBCriticalSecton* m_pCS;
	};
}