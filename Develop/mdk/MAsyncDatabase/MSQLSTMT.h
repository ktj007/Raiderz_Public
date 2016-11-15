#pragma once

#include "MADBLib.h"
#include "sqltypes.h"

namespace mdb
{
	class MDatabase;

	class MADB_API MSQLSTMT
	{
	public :
		MSQLSTMT();
		~MSQLSTMT();

		bool Init(MDatabase* pDatabase);
		bool Close();

		SQLHSTMT GetSTMT();

	protected :
		SQLHSTMT		m_hStmt;
	};
}