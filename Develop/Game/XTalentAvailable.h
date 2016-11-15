#pragma once

class XTalentAvailable
{
public:
	enum Anailable_Result
	{
		RESULT_TRUE = 0,
		RESULT_FALSE,
		RESULT_FALSE_BUFF,
		RESULT_FALSE_STANCE,
		RESULT_FALSE_STA,
		RESULT_FALSE_EN,
		RESULT_FALSE_WEAPON,
		RESULT_FALSE_UNTRAINED,
		RESULT_FALSE_COOLTIME,
		RESULT_FALSE_PORTALTALENT
	};
private:
	Anailable_Result	m_nResult;

protected:
	bool CheckBuff( XTalentInfo* pTalentInfo );
	bool CheckStance( XTalentInfo* pTalentInfo );
	bool CheckSTA( XTalentInfo* pTalentInfo );
	bool CheckEN( XTalentInfo* pTalentInfo );
	bool CheckWeapon( XTalentInfo* pTalentInfo );
	bool CheckAlive( XTalentInfo* pTalentInfo );
	bool CheckTrainedTalent( XTalentInfo* pTalentInfo );
	bool CheckCoolTime( XTalentInfo* pTalentInfo );
	bool CheckGuard( XTalentInfo* pTalentInfo );
public:
	XTalentAvailable(void);
	~XTalentAvailable(void);

	bool IsAvailable(XTalentInfo* pTalentInfo);
	bool IsAvailable(int nTalentID);


	bool CheckPortalTalentUsable(XTalentInfo* pTalentInfo);
	Anailable_Result GetResult() { return m_nResult; }
};
