#pragma once


#include <string>
#include <map>
using namespace std;


namespace mdb
{
	class MDBThread;
	class MDBThreadTaskQ;
	class MDBThreadTaskResultQ;


	class MDBThreadPool
	{
	public :
		MDBThreadPool();
		~MDBThreadPool();


		bool			Create(const DWORD dwThreadCount
							, const wstring& strDSN
							, MDBThreadTaskQ* pTaskQ
							, MDBThreadTaskResultQ* pTaskResultQ);


		size_t			GetThreadCount();

		void			SetExit();
		bool			IsExitComplete();


		void			Release();


	private :
		typedef map<HANDLE, MDBThread*> MDB_THREAD_MAP;

		MDB_THREAD_MAP	m_ThreadMap;
	};
}