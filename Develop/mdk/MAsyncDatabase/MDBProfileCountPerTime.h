#pragma once


#include "MDBProfileConst.h"


namespace mdb
{
	class MDBProfileCountPerTime
	{
	public :
		MDBProfileCountPerTime();
		~MDBProfileCountPerTime();


		void	AddCount();

		void	CheckSaveCount();

		DWORD	GetLastCount();
		float	GetAvgCount();
		DWORD	GetMaxCount();
		DWORD	GetMinCount();


	private :
		bool	IsSaveCount();
		void	SetMinMaxCount();
		void	SaveCount();
		void	MoveNextArrayIndex();


	private :
		// completed count.
		DWORD		m_dwLastSaveCountTime;
		DWORD		m_dwCountPerSecond;
		DWORD		m_dwLastCountPerSecond;
		DWORD		m_dwArrayIndex;
		DWORD		m_CountArray[MDB_PROFILE_SAVE_COUNT];
		DWORD		m_dwMaxCountPerSecond;
		DWORD		m_dwMinCountPerSecond;
	};
}