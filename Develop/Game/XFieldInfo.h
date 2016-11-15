#ifndef _XFIELD_INFO_H
#define _XFIELD_INFO_H

#include "XDef.h"
#include "CSFieldInfo.h"
#include "XSensorInfo.h"
#include "XCylinder.h"


class XFieldInfoMgr;
namespace rs3 { class RResource; }

struct AREA_INFO
{
	int			nID;	
	bool		bPath;
	bool		bEnterLocal;
	vector<XCylinder>	veccylinder;

	wstring		GetName();

private:
	friend		XFieldInfoMgr;
	wstring		strName;		// 문자열키입니다. 직접 참조를 막습니다.
};


struct FIELDSOUND_INFO
{
	wstring		strName;
	vec3		vPos;
	float		fMinDist;
	float		fMaxDist;
	bool		bMuteAmbient;
	int			nReverb;
	bool		bContact;

	FIELDSOUND_INFO() : vPos( vec3::ZERO), fMinDist( 100.0f), fMaxDist( 1000.0f), bMuteAmbient( false), nReverb( -1), bContact( false)		{}
};

struct NPC_UIMAP_INFO
{
	enum INFO_TYPE
	{
		TYPE_CRAFT = 0,
		TYPE_SHOP,

		TYPE_CNT
	};

	int		nNPCID;
	vec3	vPos;

	wstring strTypeName;
	wstring strIconName;

	string strLookPart;
	string strLookState;

	NPC_UIMAP_INFO() : vPos( vec3::ZERO)	{}
};

typedef map<unsigned long, MARKER_INFO>		MARKER_INFO_MAP_BY_ID;
typedef map<wstring, MARKER_INFO>			MARKER_INFO_MAP_BY_NAME;
typedef map<unsigned long, XSensorInfo>		SENSOR_INFO_MAP_BY_ID;
typedef vector<AREA_INFO>					AREA_INFO_VECTOR;
typedef vector<NPC_UIMAP_INFO>				NPC_UIMAP_INFO_VECTOR;



/// 필드 정보( 존 정보 + 엔티티 정보 )
class XFieldInfo : public CSFieldInfo
{
private:
	friend class XFieldInfoMgr;
	friend class MockWorld;
	using CSFieldInfo::m_strFieldName;

	static XSensorInfo	m_NullSensorInfo;

	void Cook_PvPAreas();
	void Cook_Sensors();
public:
	wstring			m_strZoneName;		///< 존 이름
	wstring			m_strZoneFile;		///< 존 파일 이름

	float			m_fMinX;
	float			m_fMaxX;
	float			m_fMinY;
	float			m_fMaxY;

	bool			m_bInstance;		///< 인스턴스 필드인지 여부
	bool			m_bWeatherEffect;	///< 환경 이펙트를 보여줄지 여부(비, 눈 등)
	bool			m_bTutorial;		///< 튜토리얼 필드인지 여부(도움말 표시, 채널 이동 불가)


	float			fGravity;
	wstring			strSkyModelName;
//	MAP_FOG_INFO	Fog;

	vector< int>	m_vFloors;

	wstring			strBGMSoundName[BGM_MAX];

	int				m_nSoundReverbType;


	MARKER_INFO_MAP_BY_ID	m_mapMarkerByID;
	SENSOR_INFO_MAP_BY_ID	m_mapSensorByID;
	map<wstring, vector<int>>		m_mapGroupSensor;

	AREA_INFO_VECTOR			m_Areas;
	vector< FIELDSOUND_INFO>	m_vecSound;
	NPC_UIMAP_INFO_VECTOR		m_NpcUiInfo[NPC_UIMAP_INFO::TYPE_CNT];
	std::vector< std::string >	m_ParsedPreLoadResources;

	XFieldInfo()  
	{ 
		m_fMinX = 0.0f;
		m_fMinY = 0.0f;
		m_fMaxX = 600000.0f;
		m_fMaxY = 600000.0f;
		fGravity = GRAVITY;
		m_bInstance = false;
		m_bWeatherEffect = true;
		m_nSoundReverbType = 0;
		m_bTutorial = false;
	}
	virtual ~XFieldInfo()
	{
		// TODO : 로딩되어있던 필드 스크립트 삭제.
		if (global.script) 
			global.script->GetGlueFieldEvent().SetNULL(this);

		m_vFloors.clear();
	}

	void Cook()
	{
		Cook_PvPAreas();
		Cook_Sensors();
	}

	XSensorInfo& GetSensorInfo(unsigned long nID);

	MPoint		GetSectorIndex(float fPosX, float fPosY);

	vec3 GetNPCPos( int nNPCID, bool bFilter=true );
	wstring GetName();
};


/// 필드 정보 관리자
class XFieldInfoMgr
{
private:
	CSZoneList		m_ZoneList;
	CSFieldList		m_FieldList;
	std::map< int, XFieldInfo*> m_mapSimpleFieldInfo;

	std::map< std::wstring, rs3::RResource* > m_PreLoadResources;

	void ParseFieldInfo(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo);
	void ParseNpcFieldInfo(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo){}
	void ParseMarker(MXmlElement* pElement, MXml* pXml, XFieldInfo* pAreaInfo);
	void ParseSensor(MXmlElement* pElement, MXml* pXml, XFieldInfo* pAreaInfo);
	void ParseEnv(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo);
	void ParsePvparea( MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo );
	void ParseArea( MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo );
	void ParseBGM(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo);
	void ParseFloors(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo);
	void ParseSoundReverbType(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo);
	void ParsePreLoad(MXmlElement* pElement, MXml* pXml, XFieldInfo* pFieldInfo);

	XFieldInfoMgr(XFieldInfoMgr& other);

	bool LoadField(const wchar_t* szFileName, XFieldInfo* pFieldInfo);

	XFieldInfo* GetFieldInfo(int nFieldID);

public:
	XFieldInfoMgr();
	virtual ~XFieldInfoMgr();
	void Clear();
	bool Load(const wchar_t* szZoneList, const wchar_t* szFieldList);

	XFieldInfo* NewFieldInfo(int nFieldID);
	XFieldInfo* NewFieldInfo(const wchar_t* szFieldName);
	XFieldInfo* NewFieldInfoFromZone(const wchar_t* szZoneName);

	bool LoadFieldSound( XFieldInfo* pFieldInfo);

	XFieldInfo* GetSimpleFieldInfo( int nFieldID);		///< 맵정보를 로드합니다. 사용시에 주의해주세요.
	wstring		GetZoneNameFromFieldID( int nFieldID);

	CSFieldList& GetFieldList()	{ return m_FieldList; }

	const wchar_t*		GetFieldName( int nFieldID );

	void LoadFieldPreLoadResources(XFieldInfo* pFieldInfo);
	bool IsFieldPreLoadResource(const wstring& _resName);
	void ClearFieldPreLoadResources();
};

#endif