#include "MRecordset.h"

using namespace System;
using namespace System::Data::SqlClient;
using namespace System::Data::SqlTypes;


MRecordset::MRecordset(SqlDataReader^ dr)
{
	m_DataReader = dr;
}

MRecordset::~MRecordset()
{
	delete m_DataReader;
}

bool MRecordset::GetBool(const char* szColName, bool* bValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetBool(nIndex, bValue);
	}
	return false;
}

bool MRecordset::GetBool(const int nColIndex, bool* bValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			*bValue = false;
		}
		else
		{
			SqlBoolean^ BolleanValue = m_DataReader->GetSqlBoolean(nColIndex);
			*bValue = BolleanValue->Value;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}


bool MRecordset::GetByte(const char* szColName, unsigned char* cValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetByte(nIndex, cValue);
	}
	return false;
}

bool MRecordset::GetByte(const int nColIndex, unsigned char* cValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			*cValue = 0;
		}
		else
		{
			SqlByte^ ByteValue = m_DataReader->GetSqlByte(nColIndex);
			*cValue = ByteValue->Value;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetAnsiChars(const char* szColName, char* pValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetAnsiChars(nIndex, pValue);
	}
	return false;
}

bool MRecordset::GetAnsiChars(const int nColIndex, char* pValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			memset(pValue, 0, strlen(pValue));
		}
		else
		{
			SqlString^ StringValue = m_DataReader->GetSqlString(nColIndex);
			char* pData = MStringToCharPointer(StringValue->Value);
			memcpy(pValue, pData, strlen(pData));
			MFreeCharPointer(pData);
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetUniChars(const char* szColName, wchar_t* pValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetUniChars(nIndex, pValue);
	}
	return false;
}

bool MRecordset::GetUniChars(const int nColIndex, wchar_t* pValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			memset(pValue, 0, wcslen(pValue));
		}
		else
		{
			SqlString^ StringValue = m_DataReader->GetSqlString(nColIndex);
			wchar_t* pData = MStringToWCharPointer(StringValue->Value);
			memcpy(pValue, pData, (wcslen(pData)*2));
			MFreeCharPointer(pData);
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetBinarys(const char* szColName, char* pValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetBinarys(nIndex, pValue);
	}
	return false;
}

bool MRecordset::GetBinarys(const int nColIndex, char* pValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			memset(pValue, 0, strlen(pValue));
		}
		else
		{
			SqlBinary^ BinaryValue = m_DataReader->GetSqlBinary(nColIndex);
			array<Char>^ Chars = gcnew array<Char>(BinaryValue->Length*2);
			for (int i = 0; i < BinaryValue->Length; i++)
			{
				Chars[i] = BinaryValue[i];
			}
			String^ strData = gcnew String(Chars);
			char* pData = MStringToCharPointer(strData);
			memcpy(pValue, pData, strlen(pData));
			MFreeCharPointer(pData);
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetInt16(const char* szColName, short* nValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetInt16(nIndex, nValue);
	}
	return false;
}

bool MRecordset::GetInt16(const int nColIndex, short* nValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			*nValue = 0;
		}
		else
		{
			SqlInt16^ Int16Value = m_DataReader->GetSqlInt16(nColIndex);
			*nValue = Int16Value->Value;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetInt32(const char* szColName, int* nValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetInt32(nIndex, nValue);
	}
	return false;
}

bool MRecordset::GetInt32(const int nColIndex, int* nValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			*nValue = 0;
		}
		else
		{
			SqlInt32^ Int32Value = m_DataReader->GetSqlInt32(nColIndex);
			*nValue = Int32Value->Value;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetInt64(const char* szColName, __int64* nValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetInt64(nIndex, nValue);
	}
	return false;
}

bool MRecordset::GetInt64(const int nColIndex, __int64* nValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			*nValue = 0;
		}
		else
		{
			SqlInt64^ Int64Value = m_DataReader->GetSqlInt64(nColIndex);
			*nValue = Int64Value->Value;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetFloat(const char* szColName, float* fValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetFloat(nIndex, fValue);
	}
	return false;
}

bool MRecordset::GetFloat(const int nColIndex, float* fValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			*fValue = 0.f;
		}
		else
		{
			SqlSingle^ SingleValue = m_DataReader->GetSqlSingle(nColIndex);
			*fValue = SingleValue->Value;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetDouble(const char* szColName, double* dbValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetDouble(nIndex, dbValue);
	}
	return false;
}

bool MRecordset::GetDouble(const int nColIndex, double* dbValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			*dbValue = 0.f;
		}
		else
		{
			SqlDouble^ DoubleValue = m_DataReader->GetSqlDouble(nColIndex);
			*dbValue = DoubleValue->Value;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetMoney(const char* szColName, char* pValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetMoney(nIndex, pValue);
	}
	return false;
}

bool MRecordset::GetMoney(const int nColIndex, char* pValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			memset(pValue, 0, strlen(pValue));
		}
		else
		{
			SqlMoney^ MoneyValue = m_DataReader->GetSqlMoney(nColIndex);
			Decimal^ DecimalValue = MoneyValue->Value;
			char* pData	= MStringToCharPointer(DecimalValue->ToString());
			memcpy(pValue, pData, strlen(pData));
			MFreeCharPointer(pData);
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetDecimal(const char* szColName, char* pValue)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetDecimal(nIndex, pValue);
	}
	return false;
}

bool MRecordset::GetDecimal(const int nColIndex, char* pValue)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			memset(pValue, 0, strlen(pValue));
		}
		else
		{
			SqlDecimal^ DecimalValue = m_DataReader->GetSqlDecimal(nColIndex);
			char* pData	= MStringToCharPointer(DecimalValue->ToString());
			memcpy(pValue, pData, strlen(pData));
			MFreeCharPointer(pData);
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

bool MRecordset::GetDateTime(const char* szColName, SYSTEMTIME* pDateTime)
{
	int nIndex = GetColumnIndex(szColName);
	if (nIndex != -1)
	{
		return this->GetDateTime(nIndex, pDateTime);
	}
	return false;
}

bool MRecordset::GetDateTime(const int nColIndex, SYSTEMTIME* pDateTime)
{
	try
	{
		if (m_DataReader->IsDBNull(nColIndex) == true)
		{
			memset(pDateTime, 0, sizeof(SYSTEMTIME));
		}
		else
		{
			DateTime^ datetime = m_DataReader->GetDateTime(nColIndex);
			pDateTime->wYear = (WORD)datetime->Year;
			pDateTime->wMonth = (WORD)datetime->Month;
			pDateTime->wDay = (WORD)datetime->Day;
			pDateTime->wDayOfWeek = (WORD)datetime->DayOfWeek;
			pDateTime->wHour = (WORD)datetime->Hour;
			pDateTime->wMinute = (WORD)datetime->Minute;
			pDateTime->wSecond = (WORD)datetime->Second;
			pDateTime->wMilliseconds = (WORD)datetime->Millisecond;
		}
	}
	catch (...)
	{
		return false;
	}
	return true;
}

int MRecordset::GetColumnIndex(const char* szColName)
{
	int nIndex;
	try
	{
		nIndex = m_DataReader->GetOrdinal(gcnew String(szColName));
	}
	catch (...)
	{
		nIndex = -1;
	}
	return nIndex;
}