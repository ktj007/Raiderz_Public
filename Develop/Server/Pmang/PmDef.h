#pragma once

#include "STransData.h"
#include "IPMSObject.h"
#include "PMSCodes.h"
#include "MTstring.h"
#include <tchar.h>
#include <atlconv.h>
#include <atlexcept.h>


enum PmRegionCode
{
	PRC_GYEONGGI = 0,		// 경기
	PRC_SEOUL,				// 서울
	PRC_INCHEON,			// 인천
	PRC_GANGWON,			// 강원
	PRC_CHUNGNAM,			// 충남
	PRC_CHUNGBUK,			// 충북
	PRC_DAEJEON,			// 대전
	PRC_GYEONGBUK,			// 경북
	PRC_GYEONGNAM,			// 경남
	PRC_DAEGU,				// 대구
	PRC_BUSAN,				// 부산
	PRC_ULSAN,				// 울산
	PRC_JEONBUK,			// 전북
	PRC_JEONNAM,			// 전남
	PRC_GWANGJU,			// 광주
	PRC_JEJU,				// 제주
	PRC_FOREIGN,			// 해외
	
	MAX_PRC,
};

enum PmAgeCode
{
	PAC_0TO13 = 0,	// 0세 ~ 13세
	PAC_14TO16,		// 14세 ~ 16세
	PAC_17TO19,		// 17세 ~ 19세
	PAC_20TO24,		// 20세 ~ 25세
	PAC_25TO29,		// 26세 ~ 29세
	PAC_30TO39,		// 30세 ~ 39세
	PAC_40TO99,		// 40세 ~ 99세

	MAX_PAC
};

enum PmGenderCode
{
	PGC_MALE = 0,	// 남자
	PGC_FEMALE,		// 여자

	MAX_PGC
};

struct PmStatIndex
{
	PmRegionCode	nRegionCode;
	PmAgeCode		nAgeCode;
	PmGenderCode	nGenderCode;
	int				nYearOfBirth;

	PmStatIndex& operator=(const PmStatIndex& r)
	{
		nRegionCode = r.nRegionCode;
		nAgeCode = r.nAgeCode;
		nGenderCode = r.nGenderCode;
		nYearOfBirth = r.nYearOfBirth;

		return *this;
	}

	bool IsValid() const
	{
		if (nRegionCode < PRC_GYEONGGI	|| MAX_PRC <= nRegionCode)	return false;
		if (nAgeCode	< PAC_0TO13		|| MAX_PAC <= nAgeCode)		return false;
		if (nGenderCode < PGC_MALE		|| MAX_PGC <= nGenderCode)	return false;

		if (nYearOfBirth < 0)	return false;

		return true;
	}
};

struct PmUserData
{
	wstring strUserSerial;
	wstring strUserID;	
	PmStatIndex	statIndex;

	wstring strPCBID;
	wstring strServiceType;
	int nUserType;


	// shallow copy해도 무리없다.
	PmUserData& operator=(const PmUserData& r)
	{
		strUserSerial = r.strUserSerial;
		strUserID = r.strUserID;
		statIndex = r.statIndex;

		strPCBID = r.strPCBID;
		strServiceType = r.strServiceType;
		nUserType = r.nUserType;

		return *this;
	}

	bool IsValid() const
	{
		if (strUserSerial.empty())	return false;
		if (strUserID.empty())		return false;

		if (!statIndex.IsValid())	return false;
		if (nUserType < 0)		return false;

		return true;
	}

	void Export(TD_PMANG_USER_DATA* pTd) const
	{
		wcsncpy_s(pTd->szUserSerial, strUserSerial.c_str(), _TRUNCATE);		
		wcsncpy_s(pTd->szUserID, strUserID.c_str(), _TRUNCATE);

		pTd->nRegionCode = (int8)statIndex.nRegionCode;
		pTd->nAgeCode = (int8)statIndex.nAgeCode;
		pTd->nGenderCode = (int8)statIndex.nGenderCode;
		pTd->nYearOfBirth = (int16)statIndex.nYearOfBirth;

		wcsncpy_s(pTd->szPCBID, strPCBID.c_str(), _TRUNCATE);
		wcsncpy_s(pTd->szServiceType, strServiceType.c_str(), _TRUNCATE);
		pTd->nUserType = (int16)nUserType;
	}	

	void Import(const TD_PMANG_USER_DATA* pTd)
	{
		strUserSerial = pTd->szUserSerial;
		strUserID = pTd->szUserID;

		statIndex.nRegionCode = (PmRegionCode)pTd->nRegionCode;
		statIndex.nAgeCode = (PmAgeCode)pTd->nAgeCode;
		statIndex.nGenderCode = (PmGenderCode)pTd->nGenderCode;
		statIndex.nYearOfBirth = pTd->nYearOfBirth;

		strPCBID = pTd->szPCBID;
		strServiceType = pTd->szServiceType;
		nUserType = pTd->nUserType;
	}
};
