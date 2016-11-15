#include "stdafx.h"
#include "MMD5.h"
#include "MTest.h"
#include "PmAuthenticator.h"
#include "PmLoginStreamParser.h"
#include "PmLoginTimeChecker.h"
#include "MStringUtil.h"
#include <iomanip>
#include <regex>
#include "MLocale.h"

SUITE(Authentication)
{
	class TestAuthentication
	{
	public:
		wstring m_strUserSerial;
		wstring m_strUserID;
		time_t m_timeStamp;
		wstring m_strOptionData1;
		wstring m_strOptionData2;
		wstring m_strOptionDataN;

		PmRegionCode m_nLocationCode;
		PmAgeCode m_nAgeCode;
		PmGenderCode m_nSexCode;
		int m_nYearOfBirth;

		wstring m_strServerKey;
		int m_nToleranceSeconds;


		TestAuthentication()
		{
			m_strServerKey = L"TestServerKey";
			m_strServerKey = L"shsxk!@rptxld";

			m_strUserSerial = L"100000000";
			m_strUserID = L"TestUserID";
			time(&m_timeStamp);
			m_strOptionData1 = L"NEOWIZ_PCBID";
			m_strOptionData2 = L"SERVICE_TYPE";
			m_strOptionDataN = L"1000";			

			m_nLocationCode = PRC_SEOUL;
			m_nAgeCode = PAC_20TO24;
			m_nSexCode = PGC_FEMALE;
			m_nYearOfBirth = 1990;

			m_nToleranceSeconds = 1;
		}

		wstring MakeHashKeyStream()
		{			
			// "UserSerial|UserID|TimeStamp|NEOWIZ_PCBIC|SERVICE_TYPE|USER_TYPE"

			wstringstream stream; 
			
			stream << m_strUserSerial << L"|" << m_strUserID << L"|" << m_timeStamp << MakeAppendedStream();

			return stream.str();
		}

		wstring MakeAppendedStream()
		{
			wstringstream stream;

			stream << L"|" << m_strOptionData1 << L"|" << m_strOptionData2 << L"|" << m_strOptionDataN;

			return stream.str();
		}

		wstring MakeHashedValue()
		{
			wstring strHashKeyStream = MakeHashKeyStream();

			wstring strInput = strHashKeyStream + L"|" + m_strServerKey;
			BYTE szOutput[16];
			
			MMD5::md5_string((BYTE*)MLocale::ConvUTF16ToAnsi(strInput.c_str()).c_str(), strInput.length(), szOutput);			
			wstring strHexString = cml2::DataToHexStringL(szOutput, sizeof(szOutput));

			return strHexString;
		}

		wstring MakeLoginStream()
		{
			// "UserSerial|UserID|TimeStamp|NEOWIZ_PCBIC|SERVICE_TYPE|USER_TYPE|HashedValue StatIndex"

			wstringstream stream;
			
			stream << MakeHashKeyStream() << L"|" << MakeHashedValue() << L" " << MakeStatIndex();
			
			return stream.str();
		}

		wstring MakeStatIndex()
		{
			// "LLASYYYY"
			
			wstringstream stream;
			
			stream.fill(L'0');

			stream	<< setw(2) << m_nLocationCode 
					<< m_nAgeCode 
					<< m_nSexCode 
					<< setw(4) << m_nYearOfBirth;

			wstring test = stream.str();
			return stream.str();
		}
	};

	TEST_FIXTURE(TestAuthentication, VerifyAuthentication)
	{
		wstring strHashKeyStream = MakeHashKeyStream();
		wstring strExpected = MakeHashedValue();

		PmAuthenticator authenticator(m_strServerKey);

		CHECK(authenticator.Verify(strExpected, strHashKeyStream));
	}

	TEST_FIXTURE(TestAuthentication, VerifyParsing)
	{
		wstring strLoginStream = MakeLoginStream();		

		PmLoginStreamParser parser(strLoginStream);

		CHECK(parser.Parse());
	}

	TEST_FIXTURE(TestAuthentication, GetHashData)
	{
		wstring strLoginStream = MakeLoginStream();
		wstring strHashKeyStream = MakeHashKeyStream();
		wstring strOptionStream = MakeAppendedStream();
		wstring strHashedValue = MakeHashedValue();		

		PmLoginStreamParser parser(strLoginStream);

		ASSERT_CHECK(parser.Parse());

		CHECK(strLoginStream == parser.GetLoginStream());
		CHECK(strHashKeyStream == parser.GetHashKeyStream());		
		CHECK(m_timeStamp == parser.GetTimeStamp());
		CHECK(strHashedValue == parser.GetHashedValue());
		
		CHECK(strOptionStream == parser.GetOptionStream());

		
		PmUserData userData = parser.GetUserData();
		
		CHECK(m_strUserSerial == userData.strUserSerial);
		CHECK(m_strUserID == userData.strUserID);			
	}

	TEST_FIXTURE(TestAuthentication, VerifyTimeStamp)
	{	
		time_t timeCurrent = m_timeStamp;		

		PmLoginTimeChecker checker(m_nToleranceSeconds);
		
		CHECK(checker.Verify(m_timeStamp, timeCurrent));

		timeCurrent += m_nToleranceSeconds;
		CHECK(checker.Verify(m_timeStamp, timeCurrent));

		timeCurrent += 1;
		CHECK(!checker.Verify(m_timeStamp, timeCurrent));
	}
}
