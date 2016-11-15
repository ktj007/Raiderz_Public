#pragma once


#include "MDBProfileCountPerTime.h"
#include "MDBProfileTimeElapse.h"


namespace mdb
{
	class MDBProfiler
	{	
	public :
		MDBProfiler();
		~MDBProfiler();


		void AddExecute();
		void AddCompleted();


		DWORD GetExecCount();
		DWORD GetCompletedCount();


		void Update();


		void BeginOnCompleted();
		void EndOnCompleted();


		MDBProfileCountPerTime&	GetProfileExecute();
		MDBProfileCountPerTime&	GetProfileCompleted();
		MDBProfileTimeElapse&	GetProfileTimeElapsed();
		

	private :		
		MDBProfileCountPerTime	m_ProfileExecute;
		MDBProfileCountPerTime	m_ProfileCompleted;			
		MDBProfileTimeElapse	m_ProfileTimeElapse;
	};
}