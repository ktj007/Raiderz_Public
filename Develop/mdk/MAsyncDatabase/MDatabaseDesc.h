#pragma once


#include <string>
using namespace std;

#include "MADBLib.h"

namespace mdb
{
	struct MADB_API MDatabaseDesc
	{
		MDatabaseDesc() {}
		MDatabaseDesc(string strDriver, string server, string database_name, string user_name, string password);
		MDatabaseDesc(wstring strDriver, wstring server, wstring database_name, wstring user_name, wstring password);

		string BuildDSNString() const;
		wstring BuildDSNStringW() const;

		//string BuildDSNString();

		wstring strServer;
		wstring strDatabaseName;
		wstring strUserName;
		wstring strPassword;
		wstring strDriver;
	};
}
