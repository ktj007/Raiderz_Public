#include "stdafx.h"
#include "PmDsnFactory.h"
#include "nwzCrypto.h"
#include "..\..\MDK\MAsyncDatabase\MDatabaseDesc.h"
#include "MLocale.h"

#pragma comment(lib, "nwzCryptoLib.lib")


PmDsnFactory::PmDsnFactory(const mdb::MDatabaseDesc& AccDesc
						 , const mdb::MDatabaseDesc& GameDesc
						 , const mdb::MDatabaseDesc& LogDesc) 
						 : SBaseDsnFactory(NEOWIZ, AccDesc, GameDesc, LogDesc)
{
}

PmDsnFactory::~PmDsnFactory()
{
}

mdb::MDatabaseDesc PmDsnFactory::GetAccountDSN()
{
	return mdb::MDatabaseDesc(m_AccDesc.strDriver
		, m_AccDesc.strServer
		, m_AccDesc.strDatabaseName
		, Decrypt(m_AccDesc.strUserName)
		, Decrypt(m_AccDesc.strPassword));
}

mdb::MDatabaseDesc PmDsnFactory::GetGameDSN()
{
	return mdb::MDatabaseDesc(m_GameDesc.strDriver
		, m_GameDesc.strServer
		, m_GameDesc.strDatabaseName
		, Decrypt(m_GameDesc.strUserName)
		, Decrypt(m_GameDesc.strPassword));
}

mdb::MDatabaseDesc PmDsnFactory::GetLogDSN()
{
	return mdb::MDatabaseDesc(m_LogDesc.strDriver
		, m_LogDesc.strServer
		, m_LogDesc.strDatabaseName
		, Decrypt(m_LogDesc.strUserName)
		, Decrypt(m_LogDesc.strPassword));
}

std::wstring PmDsnFactory::Decrypt(const std::wstring& str)
{	
	wchar_t szOutput[1024] = {0,};

#ifdef _UNICODE
	decryptPwdUC(str.c_str(), str.length(), szOutput, 1024, 0);
#else		
	string strAnsi = MLocale::ConvUTF16ToAnsi(str.c_str()).c_str();
	decryptPwd(strAnsi.c_str(), strAnsi.length(), szOutput, 1024, 0);
#endif
	
	return MLocale::ConvTCHARToUTF16(szOutput).c_str();
}
