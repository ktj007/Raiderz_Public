#pragma once

#include "CSCommonLib.h"
#include "CSDef.h"
#include "CSWorldInfo.h"
#include "MXml.h"
#include <map>
#include "MTString.h"
using namespace std;

struct CSZoneFileInfo
{
	int		nZoneID;		// 존 ID
	tstring	strName;
	tstring	strFile;
	CSZoneFileInfo()
	{
		nZoneID = -1;
	}
};

struct CSFieldFileInfo
{
	int		nFieldID;
	tstring	strName;
	tstring	strFile;
	bool	bDynamic;
	bool	bAILODEnable;				///< AI LOD를 사용할지 여부
	int		nDefaultChannelCount;		///< 서버 로딩시에 생성할 채널 개수
	int		nCapacity;					///< 한 서버당 들어가는 플레이어 수
	bool	bValidation;				///< Validator에서 체크할 지 여부
	bool	bShowMap;					///< 클라. 맵리스트에 보여줄지 여부
	CSFieldFileInfo()
	{
		nFieldID = -1;
		bDynamic = false;
		nDefaultChannelCount = DEFAULT_CHANNEL_COUNT;
		nCapacity = CHANNEL_PLAYER_LIMIT;
		bValidation = true;
		bAILODEnable = true;
		bShowMap = false;
	}
};

enum FIELD_TYPE
{
	FIELDTYPE_NONE = 0,
	FIELDTYPE_HUNTING_GROUND,
	FIELDTYPE_TOWN,

	FILEDTYPE_MAX
};


/// 존 리스트 (zonelist.xml에 있는 내용)
class CSCOMMON_API CSZoneList : public map<int, CSZoneFileInfo* > 
{
private:
	void ParseZone(MXmlElement* pElement, MXml* pXml);
	bool CheckOverlapZoneID(int nZoneID);
public:
	CSZoneList();
	virtual ~CSZoneList();
	void Clear();
	bool Load(const TCHAR* szFileName);
	CSZoneFileInfo* Find(int nID);
};

/// 필드 리스트 (fieldlist.xml에 있는 내용)
class CSCOMMON_API CSFieldList : public map<int, CSFieldFileInfo* > 
{
private:
	void ParseFieldList( MXmlElement* pElement, MXml* pXml );
	void ParseField(MXmlElement* pElement, MXml* pXml);
	void ParseField(MXmlElement* pElement, MXml* pXml, CSFieldFileInfo*& pNewInfo);
	void ParseFieldGroup(MXmlElement* pElement, MXml* pXml);
public:
	CSFieldList();
	virtual ~CSFieldList();
	void Clear();
	bool Load(const TCHAR* szFileName, bool bIncludeDevFieldList = false);

	void Insert(CSFieldFileInfo* pFieldFileInfo);
	CSFieldFileInfo* Find(int nID);
	bool IsExist(int nID);
};

struct PVP_AREA_INFO
{
	MRect	rcArea;
	uint8	nFaction1;
	uint8	nFaction2;
	tstring	strFactionName1;
	tstring	strFactionName2;

	PVP_AREA_INFO(): nFaction1(0), nFaction2(0) {}
};


/// 필드의 시간 정보
struct CSFieldTimeInfo
{
	bool				bFixed;				///< 시간 고정 여부
	GAME_TIME_TYPE		nFixedTime;			///< 고정된 시간

	CSFieldTimeInfo() : bFixed(false), nFixedTime(TIME_INVALID) {}
	void Clear() { bFixed = false; nFixedTime = TIME_INVALID; }
};


/// 클라이언트와 서버 필드 정보의 부모 클래스
class CSCOMMON_API CSFieldInfo
{
public:
	int						m_nFieldID;			///< 필드 ID
	int						m_nZoneID;			///< 존 ID

	tstring					m_strFieldName;		///< 필드 이름
	tstring					m_strFieldFile;		///< 필드 파일 이름

	bool					m_bBoundCheck;		///< 경계선 검사
	MRectF					m_rtBoundary;		///< 경계선 좌표

	vector<PVP_AREA_INFO>	m_PvPAreas;
	CSFieldTimeInfo			m_TimeInfo;			///< 시간 정보

	bool					m_bPotralTalentUsable;	///< 필드 이동 탤런트 사용 가능 여부

	CSFieldInfo() : m_nFieldID(0), m_nZoneID(0), m_bBoundCheck(false), m_bPotralTalentUsable(true) {}
	virtual ~CSFieldInfo() {}
};