#pragma once

#include <string>
#include <map>
#include <vector>
#include "MXml.h"
//#include "M_EntityObject.h"

typedef std::map<std::string, int> NUMBERINGMAP;
class CORETOOL_API CXMLEditInfoFile
{
public:
	inline static CXMLEditInfoFile& GetReference()
	{
		static CXMLEditInfoFile mapInfo;
		return mapInfo;
	}

	bool LoadZoneInformationFile(const char *filename);
	bool SaveZoneInformationFile(const char *filename, bool _bForceObjSave = false, bool _bForceGriSave = false);

	bool LoadFieldInformationFile(const char *filename);
	bool SaveFieldInformationFile(const char *filename, bool _bForceSave = false);

	//static object counting
	int  GetNumbering(const std::string *pStrName);

	//Grass Info File
	void LoadGrassInfo(CTreeCtrl* _pTreeCtrl);
	void AddGrassBrush();
	bool ChangeGrassInfo();

	//Entity Object Counting
	template< typename T_EntityType >
	int GetNumAfterEntityAdded(typename T_EntityType::TYPE _type)
	{
		m_entityObjectNum_.m_bAdded = true;
		if(_type == T_EntityType::ENTITY_SPAWN)
		{
			m_entityObjectNum_.m_nSpawn += 1;
			return m_entityObjectNum_.m_nSpawn;
		}
		else if(_type == T_EntityType::ENTITY_MARKER)
		{
			m_entityObjectNum_.m_nMarker += 1;
			return m_entityObjectNum_.m_nMarker;
		}
		else if(_type == T_EntityType::ENTITY_SENSOR)
		{
			m_entityObjectNum_.m_nSensor += 1;
			return m_entityObjectNum_.m_nSensor;
		}
		else if(_type == T_EntityType::ENTITY_SOUND)
		{
			m_entityObjectNum_.m_nSound += 1;
			return m_entityObjectNum_.m_nSound;
		}
		else if(_type == T_EntityType::ENTITY_AREA)
		{
			m_entityObjectNum_.m_nArea += 1;
			return m_entityObjectNum_.m_nArea;
		}
		else if(_type == T_EntityType::ENTITY_ROOM)
		{
			m_entityObjectNum_.m_nRoom += 1;
			return m_entityObjectNum_.m_nRoom;
		}
		//invalid
		return -1;
	}

	template< typename T_EntityType >
	void ResetEntityNumbering(typename T_EntityType::TYPE _type, int _nNumToReset)
	{
		m_entityObjectNum_.m_bAdded = true;
		if(_type == T_EntityType::ENTITY_SPAWN)
		{
			m_entityObjectNum_.m_nSpawn = _nNumToReset;
		}
		else if(_type == T_EntityType::ENTITY_MARKER)
		{
			m_entityObjectNum_.m_nMarker = _nNumToReset;
		}
		else if(_type == T_EntityType::ENTITY_SENSOR)
		{
			m_entityObjectNum_.m_nSensor = _nNumToReset;
		}
		else if (_type == T_EntityType::ENTITY_AREA)
		{
			m_entityObjectNum_.m_nArea = _nNumToReset;
		}
		else if (_type == T_EntityType::ENTITY_ROOM)
		{
			m_entityObjectNum_.m_nRoom = _nNumToReset;
		}
	}

private:
	bool LoadNumbering(MXmlElement *pElement);
	bool SaveNumbering(MXmlElement *pElement);
	
	~CXMLEditInfoFile(void);
	CXMLEditInfoFile(void);

	//Static Object
	NUMBERINGMAP m_kMapNumbering;
	bool m_bObjectAdded_;

	//grass brush
	std::string m_strXmlGrass_;
	CTreeCtrl* m_pLoadedTree_;
	int m_grassNumbering_;
	bool m_bGrassChanged_;

	struct ENTITY_NUM_INFO {
		ENTITY_NUM_INFO()
			: m_nSpawn(0), m_nMarker(0), m_nSensor(0), m_nSound(0), m_nArea(0), m_nRoom(0), m_bAdded(false) {}

		int m_nSpawn;
		int m_nMarker;
		int m_nSensor;
		int m_nSound;
		int m_nArea;
		int m_nRoom;
		bool m_bAdded;
	} m_entityObjectNum_;
};
