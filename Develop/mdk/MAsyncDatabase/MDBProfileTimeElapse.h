#pragma once


#include "MDBProfileConst.h"


namespace mdb
{
	class MDBProfileTimeElapse
	{
	public :
		MDBProfileTimeElapse();
		~MDBProfileTimeElapse();


		void	Begin();
		void	End();

		DWORD	GetLastBeginTime();
		DWORD	GetLastEndTime();
		float	GetAvgElapseTime();


	private :
		void	SetMinMaxElapseTime();
		void	SaveElapseTime();
		DWORD	GetElapseTime();
		void	MoveNextArrayInexe();


	private :
		DWORD	m_dwBgineTime;
		DWORD	m_dwEndTime;
		DWORD	m_dwArrayIndex;
		DWORD	m_ElapseTimeArray[MDB_PROFILE_SAVE_COUNT];
		DWORD	m_dwMaxElapseTime;
		DWORD	m_dwMinElapseTime;
	};
}