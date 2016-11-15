#ifndef _CS_VALIDATE_LOGGER_H_
#define _CS_VALIDATE_LOGGER_H_

#include "CSCommonLib.h"

#include <deque>
#include <string>
#include "MTstring.h"
using namespace std;

#define PERCENT_S				_T("%s")
#define PERCENT_D				_T("%d")
#define IS						_T("이(가) ")
#define NOT_IN					_T("에 없습니다.\n")
#define EMPTY					_T(" 비어 있습니다.\n")
#define ZERO_OR_MINUS			_T(" 0이거나 음수 입니다.\n")
#define MINUS					_T(" 음수 입니다.\n")
#define INVALID_TYPE			_T(" 잘못된 종류 입니다.\n")
#define ALREADY_EXIST			_T(" 이미 있습니다.\n")
#define TOO_BIG					_T(" 너무 큽니다.\n")
#define TOO_SMALL				_T(" 너무 작습니다..\n")
#define NOT_TRUE				_T(" true가 아닙니다.\n")
#define NOT_FALSE				_T(" false가 아닙니다.\n")
#define FILED_OR_MARKER_OF		_T("의 필드나 마커가 ")
#define NPCID_OF				_T("의 NPCID가 ")
#define OVER_MAX_STACK_AMOUNT	_T("아이템 최대 스택 개수를 넘습니다.")
#define TRADABLEITEM			_T("거래 가능한 아이템입니다.\n")

#define IS_NOT_IN_FIELDGROUPLIST	IS FILENAME_FIELDGROUPLIST NOT_IN
#define IS_NOT_IN_FIELDLIST			IS PATH_SYSTEM PURE_FILENAME_FIELDLIST NOT_IN
#define IS_NOT_IN_FIELEXT			IS FILEEXT_FIELD NOT_IN
#define IS_NOT_IN_CONDITIONS		IS FILENAME_CONDITIONS NOT_IN
#define IS_NOT_IN_DIALOG			IS FILENAME_DIALOG NOT_IN
#define IS_NOT_IN_NPC				IS FILENAME_NPCINFO NOT_IN
#define IS_NOT_IN_ITEM				IS FILENAME_ITEMINFO NOT_IN
#define IS_NOT_IN_TALENT			IS FILENAME_TALENTINFO NOT_IN
#define IS_NOT_IN_LOOT				IS FILENAME_LOOT_INFO NOT_IN
#define IS_NOT_IN_TRADE				IS FILENAME_NPCSHOP NOT_IN
#define IS_NOT_IN_TRAINING			IS FILENAME_TRAINING NOT_IN
#define IS_NOT_IN_QUEST				IS FILENAME_QUEST NOT_IN
#define IS_NOT_IN_INN				IS FILENAME_INN NOT_IN
#define IS_NOT_IN_SOULBINDING		IS FILENAME_SOULBINDING NOT_IN
#define IS_NOT_IN_BUFFID			IS FILENAME_BUFFINFO NOT_IN
#define IS_NOT_IN_FACTION			IS FILENAME_FACTION NOT_IN
#define IS_NOT_IN_RECIPE			IS FILENAME_RECIPE NOT_IN

#define IS_NOT_IN_PERCENT_D_FILEEXT_FIELD		IS PERCENT_D FILEEXT_FIELD NOT_IN

#define IS_ZERO_OR_MINUS			IS ZERO_OR_MINUS
#define IS_MINUS					IS MINUS
#define IS_INVALID_TYPE				IS INVALID_TYPE
#define IS_ALREADY_EXIST			IS ALREADY_EXIST
#define IS_TOO_BIG					IS TOO_BIG
#define IS_TOO_SMALL				IS TOO_SMALL
#define IS_EMPTY					IS EMPTY
#define IS_NOT_TRUE					IS NOT_TRUE
#define IS_NOT_FALSE				IS NOT_FALSE
#define IS_OVER_MAX_STACK_AMOUNT	IS OVER_MAX_STACK_AMOUNT


#define BIGGER_THAN						_T("%s이(가) %s보다 더 큽니다.\n")
#define MUCH_THAN						_T("%s이(가) %s보다 더 많습니다.\n")
#define BIGGER_OR_EQUAL_THAN			_T("%s이(가) %s보다 더 크거나 같습니다.\n")
#define TEXT_EXCEEDED_ALLOWED_LENGTH	_T("%s의 길이가 %d(bytes)를 초과하였습니다.\n")
#define THE_VALUE_MUST_BE_IN_MINMAX		_T("%s값 %.3f는(은) %.3f(min)와 %.3f(max) 사이값이어야 합니다.\n")
#define THE_VALUE_MUST_BE_IN_MINMAX_D	_T("%s값 %d는(은) %d(min)와 %d(max) 사이값이어야 합니다.\n")


class CSLogListener;

class CSCOMMON_API CSValidateLogger
{
public:
	struct ELEMENT
	{
		ELEMENT(tstring strTag)
		: m_strTag(strTag)
		{			
		}

		struct ATTRIBUTE
		{
			ATTRIBUTE(tstring strName, tstring strValue)
			: m_strName(strName)
			, m_strValue(strValue)
			{
			};

			ATTRIBUTE(tstring strName, int nValue)
			: m_strName(strName)
			{
				TCHAR szNumber[512];
				_itot_s(nValue, szNumber, 512, 10);				
				m_strValue = szNumber;
			};

			tstring m_strName;
			tstring m_strValue;
		};

		void Insert(const ATTRIBUTE& attribute)
		{
			m_qAttribute.push_back(attribute);
		}

		tstring m_strTag;
		deque<ATTRIBUTE> m_qAttribute;
	};
private:
	CSLogListener*	m_pListener;
	bool m_bValidateResult;
	bool m_bLogging;
protected:
	deque<ELEMENT> m_qElement;
	void PushElement(const ELEMENT& element);
	void PopElement();
	tstring m_strFileName;

	void SetValidateResultFailed() { m_bValidateResult = false; }
public:
	CSValidateLogger();
	virtual ~CSValidateLogger();

	void Init(const TCHAR* szFileName);
	void SetListener(CSLogListener* pListener) { m_pListener = pListener; }
	void Log(const TCHAR* szAddtionLog, ...);
	bool GetValidateResult() { return m_bValidateResult; }
	void SetLogging(bool bLogging) { m_bLogging = bLogging; }

	void ValidateMinMax(const TCHAR* szValueName, float fValue, float fMinValue, float fMaxValue);
	void ValidateMinMax( const TCHAR* szValueName, int nValue, int nMinValue, int nMaxValue );
	void ValidateMinus(int nValue, const TCHAR* szName);
	void ValidateMinus(float fValue, const TCHAR* szName);
	void ValidateMinusORZero(int nValue, const TCHAR* szName);
	void ValidateMinusORZero(float fValue, const TCHAR* szName);
	void ValidateBiggerThan(int nSmallValue, int nBigValue, const TCHAR* szSmallName, const TCHAR* szBigName);
	void ValidateBiggerThan(float fSmallValue, float fBigValue, const TCHAR* szSmallName, const TCHAR* szBigName);
	void ValidateBiggerOrEqualThan(float fSmallValue, float fBigValue, const TCHAR* szSmallName, const TCHAR* szBigName);
	void ValidateMuchThan(int nSmallValue, int nBigValue, const TCHAR* szSmallName, const TCHAR* szBigName);	
	
	void ValidateEmptyText(const tstring& strText, const TCHAR* szName);
	void ValidateLengthText(const tstring& strText, int nMaxLength, const TCHAR* szName);	

	void ValidateSex(int nValue, const TCHAR* szName);
	void ValidateRace(int nValue, const TCHAR* szName);

	void ValidateTrue(bool bBool, const TCHAR* szName);
	void ValidateFalse(bool bBool, const TCHAR* szName);

	void ValidateTooBig(int nValue, int nMaxValue, const TCHAR* szValueName);
	void ValidateID(int nID);
};


#endif//_CS_VALIDATE_LOGGER_H_