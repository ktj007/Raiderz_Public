#pragma once


#include "CSBuffInfo.h"


enum BUFF_EFFECT_TYPE
{
	BUFF_GAIN_EFFECT = 0,		// 버프 처음 걸렸을때
	BUFF_GAIN_LOOP_EFFECT,		// 버프 루프
	BUFF_GAIN_RELEASE_EFFECT,	// 버프 끝날때
	BUFF_HIT_EFFECT,			// 버프로 인해 주기적으로 맞을때

	BUFF_EFFECT_MAX
};

/// 투명 버프 걸렸을 때 알파값
static const float BUFF_INVISIBILITY_ALPHA = 0.3f;

//////////////////////////////////////////////////////////////////////////
/// 버프 이펙트 정보
//------------------------------------------------------------------------
// [2007/5/25 isnara] Talent Effect Data
#define BUFF_XML_ATTR_EFFECT_ID						"id"
#define BUFF_XML_ATTR_REFERENCE_ID					"reference_id"
#define BUFF_XML_TAG_BUFF_EFFECT					"BUFFEFFECT"
#define BUFF_XML_TAG_BUFF_GAIN_EFFECT				"GainEffect"
#define BUFF_XML_TAG_BUFF_GAIN_LOOP_EFFECT			"GainLoopEffect"
#define BUFF_XML_TAG_BUFF_GAIN_RELEASE_EFFECT		"GainReleaseEffect"
#define BUFF_XML_TAG_BUFF_HIT_EFFECT				"HitEffect"
#define BUFF_XML_TAG_BUFF_WEAPON_EFFECT				"WeaponEffect"
#define BUFF_XML_TAG_BUFF_EXTRA_ACTIVE_MODEL_ID		"ExtraActiveModelID"
#define BUFF_XML_TAG_BUFF_SHOW_HIT_REACTION			"ShowHitReaction"
#define BUFF_XML_TAG_BUFF_RIDENPCID					"RideNPCID"

class XBuffEffectInfo
{
public:
	BUFF_EFFECT_TYPE			m_nBuffEffectType;			// 버프 이펙트 타입
	
	bool						m_bBuffWeaponEffect;		// 칼에 붙이 이펙트인가

	tstring						m_strEffectName;			// 이펙트 이름
	tstring						m_strBoneName;				// 본 이름

	XBuffEffectInfo()
	{
		Init();
	}

	void Init()
	{
		m_nBuffEffectType		= BUFF_GAIN_EFFECT;
		m_bBuffWeaponEffect		= false;
		m_strEffectName.clear();
		m_strBoneName.clear();
	}

	XBuffEffectInfo& operator=(const XBuffEffectInfo& other)
	{
		m_nBuffEffectType		= other.m_nBuffEffectType;
		m_bBuffWeaponEffect		= other.m_bBuffWeaponEffect;
		m_strEffectName			= other.m_strEffectName;
		m_strBoneName			= other.m_strBoneName;

		return *this;
	}
};

class XBuffEffectData : public vector<XBuffEffectInfo *>
{
public:
	int							m_nBuffID;					// id
	vector<int>					m_vecRefID;					// refence id

	XBuffEffectData() 
	{
		m_nBuffID = 0;
		m_vecRefID.clear();
	}

	virtual ~XBuffEffectData()
	{
		vector<XBuffEffectInfo *>::iterator it = begin();
		while(it != end())
		{
			SAFE_DELETE(*it);
			it++;
		}

		clear();
	}

	int							GetEffectInfoList(BUFF_EFFECT_TYPE nType, vector<XBuffEffectInfo *> &BuffEffectInfo);
};


enum ENCHANT_BUFF_EFFECT_TYPE
{
	EBET_DUMMY_EFFECT = 0,		// 더미 이펙트
	EBET_TRAIL_EFFECT,			// 검광 텍스쳐
	EBET_HIT_EFFECT,			// 히트시...

	EBET_EFFECT_MAX
};

enum ENCHANT_BUFF_EFFECT_EQUIP_TYPE
{
	EBEET_NONE,
	EBEET_1HS,
	EBEET_1HB,
	EBEET_1HP,
	EBEET_2HD,
	EBEET_STA,
	EBEET_ARC,
	EBEET_2HB,
	EBEET_DWD,
	EBEET_DWP,

	EBEET_HEAD,
	EBEET_BODY,
	EBEET_HANDS,
	EBEET_LEG,
	EBEET_FEET,

	EBEET_MAX
};

#define BUFF_XML_TAG_ENCHANT_BUFF_EFFECT					"ENCHANT_BUFFEFFECT"
#define BUFF_XML_TAG_ENCHANT_BUFF_DUMMY_EFFECT				"DummyEffect"
#define BUFF_XML_TAG_ENCHANT_BUFF_TRAIL_EFFECT				"TrailEffect"
#define BUFF_XML_TAG_ENCHANT_BUFF_HIT_EFFECT				"HitEffect"
#define BUFF_XML_TAG_ENCHANT_BUFF_DEFAULT					"DEFAULT"
#define BUFF_XML_TAG_ENCHANT_BUFF_EQUIP						"EQUIP"
#define BUFF_XML_ATTR_TRAIL_EFFECT_TEXTURE_NAME				"TexName"

//////////////////////////////////////////////////////////////////////////
// 강화 버프 이펙트 정보

class XEnchantBuffEffectInfo
{
public:
	ENCHANT_BUFF_EFFECT_TYPE	m_nEnchantBuffEffectType;

	tstring						m_strEffectName;			// 이펙트 이름
	tstring						m_strBoneName;				// 본 이름
	tstring						m_strTrailTexName;			// 검광 텍스쳐 이름

	XEnchantBuffEffectInfo()
	{
		Init();
	}

	virtual ~XEnchantBuffEffectInfo()
	{

	}

	void Init()
	{
		m_nEnchantBuffEffectType		= EBET_DUMMY_EFFECT;
		m_strEffectName.clear();
		m_strBoneName.clear();
		m_strTrailTexName.clear();
	}

	XEnchantBuffEffectInfo& operator=(const XEnchantBuffEffectInfo& other)
	{
		m_nEnchantBuffEffectType		= other.m_nEnchantBuffEffectType;
		m_strEffectName					= other.m_strEffectName;
		m_strBoneName					= other.m_strBoneName;
		m_strTrailTexName				= other.m_strTrailTexName;

		return *this;
	}
};

class XEnchantBuffEffectEquipType : public vector<XEnchantBuffEffectInfo *>
{
private:
	ENCHANT_BUFF_EFFECT_EQUIP_TYPE	Slot2EquipType(WEAPON_TYPE eRType, WEAPON_TYPE eLType, int nPartsSlotType);
	ENCHANT_BUFF_EFFECT_EQUIP_TYPE	WeaponType2EquipType(WEAPON_TYPE nWeapoinType);

public:
	ENCHANT_BUFF_EFFECT_EQUIP_TYPE m_eEquipType;

	XEnchantBuffEffectEquipType()
	{
		m_eEquipType = EBEET_NONE;
	}

	virtual ~XEnchantBuffEffectEquipType()
	{
		vector<XEnchantBuffEffectInfo *>::iterator it = begin();
		while(it != end())
		{
			SAFE_DELETE(*it);
			it++;
		}

		clear();
	}

	XEnchantBuffEffectEquipType& operator=(XEnchantBuffEffectEquipType& other)
	{
		clear();
		for(vector<XEnchantBuffEffectInfo *>::iterator itCopy = other.begin(); itCopy != other.end(); ++itCopy)
		{
			XEnchantBuffEffectInfo* pEnchantBuffEffectInfo = new XEnchantBuffEffectInfo;

			*pEnchantBuffEffectInfo = *(*itCopy);

			push_back(pEnchantBuffEffectInfo);
		}

		m_eEquipType			= other.m_eEquipType;

		return *this;
	}

	XEnchantBuffEffectEquipType& operator+=(XEnchantBuffEffectEquipType& other)
	{
		for(vector<XEnchantBuffEffectInfo *>::iterator itCopy = other.begin(); itCopy != other.end(); ++itCopy)
		{
			XEnchantBuffEffectInfo* pEnchantBuffEffectInfo = new XEnchantBuffEffectInfo;

			*pEnchantBuffEffectInfo = *(*itCopy);

			push_back(pEnchantBuffEffectInfo);
		}

		m_eEquipType			= other.m_eEquipType;

		return *this;
	}

	int							GetEffectInfoList(ENCHANT_BUFF_EFFECT_TYPE nType, WEAPON_TYPE eRType, WEAPON_TYPE eLType, int nPartsSlotType, vector<XEnchantBuffEffectInfo *> &BuffEffectInfo);

};

class XEnchantBuffEffectData : public vector<XEnchantBuffEffectEquipType *>
{
public:
	int							m_nBuffID;					// id
	vector<int>					m_vecRefID;					// refence id

	XEnchantBuffEffectEquipType m_DefaultEffect;

	XEnchantBuffEffectData() 
	{
		m_nBuffID = 0;
		m_vecRefID.clear();
	}

	virtual ~XEnchantBuffEffectData()
	{
		vector<XEnchantBuffEffectEquipType *>::iterator it = begin();
		while(it != end())
		{
			SAFE_DELETE(*it);
			it++;
		}

		clear();
	}

	int							GetEffectInfoList(ENCHANT_BUFF_EFFECT_TYPE nType, WEAPON_TYPE eRType, WEAPON_TYPE eLType, int nPartsSlotType, vector<XEnchantBuffEffectInfo *> &BuffEffectInfo);
	tstring						GetAllDummyEffectName();
};

//////////////////////////////////////////////////////////////////////////
class XBuffInfoMgr;


class XBuffInfo : public CSBuffInfo
{
private:
	friend class XBuffInfoMgr;
	using CSBuffInfo::m_strName;		// 문자열테이블 참조하도록, 직접참조를 막는다.
	
	tstring		m_szDesc;				///< 설명(클라), 직접참조를 막는다.

public:
	XBuffInfo(); 
	void				InitSuffix();

	bool IsInvisibilityToNPC();
	bool IsInvisibility();

	const TCHAR*			GetName(void);
	const TCHAR*			GetDesc(void);

	virtual void CopyAll(CSBuffInfo* pTarget) override;

public:
	bool					m_bExistModValue;

	MOTION_FACTOR_GROUP		m_MotionFactorGroup;

	XBuffEffectData			m_BuffEffect;				// 버프 이펙트
	XEnchantBuffEffectData	m_EnchantBuffEffect;		// 강화 버프 이펙트
	
	tstring					m_szIcon;				// 아이콘	

	bool					m_bLogable;
	bool					m_bShowHitReaction;

	int						m_nExtraActiveModelID;			// 마법진에 사용하는 모델(NPC) ID
};



typedef map<tstring, uint32> MAP_BUFF_EFFECT;

/// 스킬 정보 관리자
class XBuffInfoMgr : public CSBuffInfoMgr
{
private:
	ENCHANT_BUFF_EFFECT_EQUIP_TYPE		String2EnchantBuffEffectEquipType(string& strType);

private:
	void					RefenceBuffEffectInfo();
	void					RefenceEnchantBuffEffectInfo();

	void					RecursiveRefenceBuffEffectInfo(XBuffInfo* pBuffInfo, XBuffInfo* pRefBuffInfo);
	void					RecursiveRefenceEnchantBuffEffectInfo(XBuffInfo* pBuffInfo, XBuffInfo* pRefBuffInfo);

	void					SetRefenceBuffEffectInfo(XBuffInfo* pBuffInfo, XBuffInfo* pRefBuffInfo);
	void					SetRefenceEnchantBuffEffectInfo(XBuffInfo* pBuffInfo, XBuffInfo* pRefBuffInfo);

	void					GetReferenceIDList(vector<tstring>& vecList, vector<int>& vecRefList);

protected:
	bool					LoadBuffInfo(tstring strFileName);
	bool					LoadBuffEffectInfo(tstring strFileName);
	bool					LoadEnchantBuffEffectInfo(tstring strFileName);

	void					ParseBuff(MXmlElement* pElement, MXml* pXml);
	void					ParseBuffEffect(MXmlElement* pElement, MXml* pXml);
	void					ParseEnchantBuffEffect(MXmlElement* pElement, MXml* pXml);
	void					ParseEnchantBuffEffectInfo(MXmlElement* pElement, MXml* pXml, XEnchantBuffEffectEquipType* pEffect);
public:
	virtual ~XBuffInfoMgr();

	bool					Load(const TCHAR* szBuffInfoFileName, const TCHAR* szBuffEffectFileName, const TCHAR* szEnchantBuffEffectFileName);
	bool					ReLoad(const TCHAR* szBuffInfoFileName, const TCHAR* szBuffEffectFileName, const TCHAR* szEnchantBuffEffectFileName);
	void					Clear();
	XBuffInfo*				Get(int nID);
	bool					Exist(int nID);
};	

