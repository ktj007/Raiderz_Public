#pragma once

#include <Windows.h>
#include <vcclr.h>

#define MStringToCharPointer(stringname)	(char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(stringname).ToPointer()
#define MStringToWCharPointer(stringname)	(wchar_t*)System::Runtime::InteropServices::Marshal::StringToHGlobalUni(stringname).ToPointer()
#define MFreeCharPointer(stringname)		System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)stringname))

class MRecordset
{
public:
	MRecordset(System::Data::SqlClient::SqlDataReader^ dr);
	virtual ~MRecordset();

	bool GetBool(const char* szColName, bool* bValue);
	bool GetBool(const int nColIndex, bool* bValue);

	bool GetByte(const int nColIndex, unsigned char* cValue);
	bool GetByte(const char* szColName, unsigned char* cValue);

	bool GetAnsiChars(const char* szColName, char* pValue);
	bool GetAnsiChars(const int nColIndex, char* pValue);

	bool GetUniChars(const char* szColName, wchar_t* pValue);
	bool GetUniChars(const int nColIndex, wchar_t* pValue);
	
	bool GetBinarys(const char* szColName, char* pValue);
	bool GetBinarys(const int nColIndex, char* pValue);
	
	bool GetInt16(const int nColIndex, short* nValue);
	bool GetInt16(const char* szColName, short* nValue);

	bool GetInt32(const int nColIndex, int* nValue);
	bool GetInt32(const char* szColName, int* nValue);

	bool GetInt64(const int nColIndex, __int64* nValue);
	bool GetInt64(const char* szColName, __int64* nValue);

	bool GetFloat(const char* szColName, float* fValue);
	bool GetFloat(const int nColIndex, float* fValue);

	bool GetDouble(const int nColIndex, double* dValue);
	bool GetDouble(const char* szColName, double* dValue);

	bool GetDecimal(const int nColIndex, char* pValue);
	bool GetDecimal(const char* szColName, char* pValue);

	bool GetMoney(const int nColIndex, char* pValue);
	bool GetMoney(const char* szColName, char* pValue);

	bool GetDateTime(const int nColIndex, SYSTEMTIME* pDateTime);
	bool GetDateTime(const char* szColName, SYSTEMTIME* pDateTime);

	bool NextRecord()		{ return m_DataReader->Read(); }
	bool NextTable()		{ return m_DataReader->NextResult(); }

private:
	gcroot<System::Data::SqlClient::SqlDataReader^> m_DataReader;
	int GetColumnIndex(const char* szColName);
};
