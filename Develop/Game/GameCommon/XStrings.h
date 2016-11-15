#ifndef _XSTRINGS_H
#define _XSTRINGS_H

#include "CStringResMgr.h"
#include "CSStrings.h"
#include "CSChrInfo.h"
#include "MLocale.h"

class XStrings : public CSStrings
{
private:	
	static vector<tstring>	m_vecStrChatDrainType;

	static vector<tstring>	m_vecStrItemTierKey;			///< ItemTier의 스트링 키
	static vector<tstring>	m_vecStrWeaponTypeKey;			///< WeaponType의 스트링 키
	static vector<tstring>	m_vecStrArmorTypeKey;			///< ArmorType의 스트링 키
	static vector<tstring>	m_vecStrSoundType;
	static vector<tstring>	m_vecStrVoiceType;				///< VoiveType의 스트링키
	static vector<tstring>	m_vecStrVoiceSay;					///< Voive의 스트링
	static vector<tstring>	m_vecKorStrFactionRelationType;	///< 팩션 관계 타입

public:
	static	bool Init();	
	static const TCHAR*	DrainTypeToChatString(int nDrainType);
	static const TCHAR*	WeaponTypeKey(int nWeaponType);
	static const TCHAR*	ArmorTypeKey(int nWeaponType);
	static const TCHAR*	ItemTierKey(int nItemTier);

	static tstring Sex(SEX nSex);
	static tstring Race(RACE nRace);
	static void	SaveCommandResultTable();
	static void SaveCommandResultSoundTable();
	static int StringToSoundType(const TCHAR* szValue);
	static const TCHAR* SoundTypeToString(int nSoundType);
	static int StringToVoiceType(const TCHAR* szValue);
	static const TCHAR* VoiceTypeToString(int nSoundType);
	static const TCHAR* VoiceSayToString(int nVoiceSay);

	static const TCHAR*	FacionRelationTypeToString(int nType);

	static tstring GetStringForNumber(int nNumber);

	static void GetCommaStringSplitter(const tstring& strValue, vector<tstring>& vecOut);
};





class XStringResMgr : public CStringResMgr
{
public:
	typedef map<tstring, CStringRes<int>*>		result_map;

protected:
	result_map		m_mapResultStrings;	

	virtual bool OnInit(const CLangNode& node) override;
	virtual const TCHAR* GetStringIncludedKeyString(const TCHAR* szKey);

public:
	XStringResMgr();
	virtual ~XStringResMgr();

	virtual void Clear(void);

	static XStringResMgr* GetInstance();

	const TCHAR* GetCmdResultString(int nKey);	

	void TransKeyStr(TCHAR* poutDestStr, const int nDestStrSize, const TCHAR* szSrcStr);
	void TransKeyStr(tstring& outDestStr, const TCHAR* szSrcStr);

	tstring TransKeyStrWithParam(tstring strKeyWithParam);		// 서버에서 보내주는 FormatString 문자열
};

inline const TCHAR* XGetStr(tstring& strKey)
{
	return XStringResMgr::GetInstance()->GetString(strKey);
}

inline const TCHAR* XGetStr(const TCHAR* szKey)
{
	return XStringResMgr::GetInstance()->GetString(szKey);
}

inline const TCHAR* XCmdResultStr(int nKey)
{
	return XStringResMgr::GetInstance()->GetCmdResultString(nKey); 
}

inline bool _Attribute_TransKey(std::tstring& strOutValue, MXmlElement* pElement, const TCHAR* name, MXml* pXml)
{
	string strValue;
	if (_Attribute(strValue, pElement, MLocale::ConvTCHARToAnsi(name).c_str(), pXml) == false) return false;

	XStringResMgr::GetInstance()->TransKeyStr(strOutValue, MLocale::ConvAnsiToTCHAR(strValue.c_str()).c_str());
	return true;
}

inline const std::tstring FormatString(tstring format, FSParam param)
{
	return CSFormatString::Format(format, param);
}

//
//inline const std::wstring FormatKey(wstring key, FSParam param)
//{
//	return CSFormatString::Format(XGetStr(key), param);
//}

#endif // _XSTRINGS_H
