#ifndef _G_VALIDATE_LOGGER_H_
#define _G_VALIDATE_LOGGER_H_

#include "CSValidateLogger.h"
#include "CSTypes.h"


#define CONDITION_BEGIN_DATE			L"시작일자"
#define CONDITION_END_DATE				L"종료일자"

#define NOT_EQUAL_SUM_TOTAL				L"%s의 %s 합이 %d와 다릅니다.\n"
#define NOT_EQUAL_FORMAT				L"%s와 %s의 포맷이 일치하지 않습니다.\n"
#define MUST_COUNT_IN_MAX				L"%s는 %d개를 넘을 수 없습니다.\n"
#define FAILED_LOAD_FILE				L"Failed To Load %s\n"
#define UNKNOWN_ENUM_VALUE				L"%s값 %s는 정의되지 않은 값입니다.\n"

class GValidateLogger : public CSValidateLogger
{
public:
	struct PUSHANDPOPELEMENT
	{
		PUSHANDPOPELEMENT(const ELEMENT& element);

		~PUSHANDPOPELEMENT();
	};

public:
	GValidateLogger();
	virtual ~GValidateLogger();

	void ValidateQuestID(int nID, const wchar_t* szName);
	void ValidateItemID(int nID, const wchar_t* szName);
	void ValidateNotTradableItemID(int nID, const wchar_t* szName);
	void ValidateTalentID(int nID, const wchar_t* szName);
	void ValidateLootID(int nID, const wchar_t* szName);
	void ValidateDialogID(int nID, const wchar_t* szName);
	void ValidateTradeID(int nID, const wchar_t* szName);
	void ValidateNPCID(int nID, const wchar_t* szName);
	void ValidateFieldID(int nID, const wchar_t* szName);
	void ValidateSoulBindingID(SBID nID, const wchar_t* szName);
	void ValidateSensorID(int nFieldID, int nSensorID, const wchar_t* szName);	
	void ValidateMarkerID(int nFieldID, int nMarkerID, const wchar_t* szName);
	void ValidateBuffID(int nID, const wchar_t* szName);
	void ValidateConditionID(int nID, const wchar_t* szName);
	void ValidateFactionID(int nID, const wchar_t* szName);
	void ValidateRecipeID(int nID, const wchar_t* szName);

	void ValidateConditionQuestState(int nValue, const wchar_t* szName);	
	void ValidateIElementType(int nValue, const wchar_t* szName);
	void ValidateDamageAttribType(int nValue, const wchar_t* szName);
	void ValidateQuestRewardType(int nValue, const wchar_t* szName);
	void ValidateConditionType(int nValue, const wchar_t* szName);
	void ValidateQuestTrialType(int nValue, const wchar_t* szName);

	void ValidateTNeedTalent(int TNeedTalent, const wchar_t* szName);
	void ValidateSumTotal(int nSum, int nTotal, const wchar_t* szCollection, const wchar_t* szItem);

	void LogNotEqualFormat(const wchar_t* szType1, const wchar_t* szType2);
	void LogMax(const wchar_t* szName, int nMax);
	void LogFailedLoad(const wchar_t* szFileName);	

	void ValidateEnumType(const wchar_t* szName, const wchar_t* szValue);

	void ValidateItemAddRemoveAmount(int nAmount, const wchar_t* szName);
	void ValidateItemMaxStackAmount(int nID, int nAmount, const wchar_t* szName);
};

#define _VLOGGER						gsys.pValidateLogger
#define _VLL(_Log)						_VLOGGER->Log(_Log)
#define _VLI(_File)						_VLOGGER->Init(_File)
#define _VLE(_Tag)						GValidateLogger::ELEMENT vle(_Tag)
#define _VLA(_AttrName, _AttrValue)		vle.Insert(GValidateLogger::ELEMENT::ATTRIBUTE(_AttrName, _AttrValue))
#define _VLP							GValidateLogger::PUSHANDPOPELEMENT vlp(vle)

#endif//_G_VALIDATE_LOGGER_H_