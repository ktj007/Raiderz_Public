/// @file WS_Field.h
#pragma once

#include "stdafx.h"
#include "MTypes.h"
#include "WS_WorkSpaceManager.h"

#include "../../CSCommon/include/CSWorldInfo.h"


class CORETOOL_SOULHUNT_API CWorkSpaceField : public CWorkSpaceImpl<CWorkSpaceField>
{
public:
								CWorkSpaceField();
	virtual						~CWorkSpaceField(){}

	virtual const char*			GetWorkSpaceKeyword();	///< WorkSpace Keyword

	void						SetFieldCreationInfo( int _zoneID, int _fieldID, const char* _pSzFieldName );
	void						CreateField();
	bool						LoadField(int _nFieldId);

	void						SetWeatherValue(int _weatherKind, int _percent);
	void						GetWeatherValue(int _weatherKind, int& _refPercent);

	void						SetSectorHeight(float fSectorHeight);
	float						GetSectorHeight();

	void						RoundOffFieldMinMax(int& _nValue);

	bool						LoadCommonEntityXML(const char* _pSzName, bool bZoneMode);
	void						SaveCommonEntityXML(const char* _pSzName, bool bZoneMode);

	void						CreateCollisionTreeBuild();

	void						LoadBGMXML(const char* _pSzFileName);

	void 						RegisterToRenderSceneNode( rs3::RSceneNode* _pSceneNode);
	void 						ClearRenderSceneNodes();

	const char*					GameTime(int nGameTime);

	//////////////////////////////////////////////////////////////////////////
	//
	// Field Info
	//
	//////////////////////////////////////////////////////////////////////////
	struct PVP_AREA_INFO
	{
		MRect	rcArea;
		uint8	nFaction1;
		uint8	nFaction2;
		string	strFactionName1;
		string	strFactionName2;

		PVP_AREA_INFO(): nFaction1(0), nFaction2(0) {}
	};

	struct FIELD_TIME_INFO
	{
		bool				bFixed;
		GAME_TIME_TYPE		nFixedTime;

		FIELD_TIME_INFO() : bFixed(false), nFixedTime(TIME_INVALID) {}
		void Clear() { bFixed = false; nFixedTime = TIME_INVALID; }
	};

	struct FIELD_INFO
	{
		FIELD_INFO() : m_nMinX(0), m_nMaxX(0), m_nMinY(0), m_nMaxY(0), m_bTownField(false), m_bWeatherEffect(true), m_bReturnable(true), m_bTutorial(false)
			, m_nMaxGatherMine(0), m_nMaxGatherHerb(0), m_nMaxGatherRelic(0), m_nMaxGatherLumber(0), m_nQPVPNPC1(0), m_nQPVPNPC2(0), m_nQPVPRebirth1(0), m_nQPVPRebirth2(0)
			, m_fSectorHeight(0.0f), m_nSoundReverbType( 0)
		{}

		int m_nMinX;
		int m_nMaxX;
		int m_nMinY;
		int m_nMaxY;
		MRect m_rtBoundary;
		int m_nReferZone;
		bool m_bTownField;
		bool m_bWeatherEffect;
		bool m_bReturnable;
		bool m_bTutorial;

		int m_nMaxGatherMine;
		int m_nMaxGatherHerb;
		int m_nMaxGatherRelic;
		int m_nMaxGatherLumber;

		int m_nQPVPNPC1;
		int m_nQPVPNPC2;
		int m_nQPVPRebirth1;
		int m_nQPVPRebirth2;

		float m_fSectorHeight;
		FIELD_TIME_INFO	m_TimeInfo;

		std::string m_strFieldFile;
		std::string m_strFieldPath;

		vector< int> m_vFloors;

		std::string m_strBGM[BGM_MAX];
		std::vector<std::string> m_FieldPreLoadList;

		int m_nSoundReverbType;

		vector<PVP_AREA_INFO>	m_PvPAreas;

		void Clear()
		{
			m_PvPAreas.clear();
			m_nMinX = 0;
			m_nMaxX = 0;
			m_nMinY = 0;
			m_nMaxY = 0;
			m_bTownField = false;
			m_bWeatherEffect = true;
			m_bReturnable = true;
			m_bTutorial = false;
			m_nMaxGatherMine = 0;
			m_nMaxGatherHerb = 0;
			m_nMaxGatherRelic = 0;
			m_nMaxGatherLumber = 0;
			m_fSectorHeight = 0.0f;
			m_vFloors.clear();
			m_nSoundReverbType = 0;
			m_TimeInfo.Clear();

			for (int i=0; i<BGM_MAX; i++ )
			{
				m_strBGM[i].clear();
			}

			m_FieldPreLoadList.clear();
		}

	} m_fieldInfo;


	//////////////////////////////////////////////////////////////////////////
	//
	// Field Weather
	//
	//////////////////////////////////////////////////////////////////////////

	const static int WEATHER_DEFAULT = 1000;

protected:

	virtual void				InitWorkSpace(CWorkSpaceUiSetter& _rUiSetter, CWorkSpaceHelpersAndActions& _rHelperAndAction);	///< WorkSpace UI
	virtual void				HaltWorkSpace(CWorkSpaceHelpersAndActions& _rHelperAndAction);

	virtual const char*			GetFileExtension()	{ return "*.field.xml"; }
	virtual const char*			GetFileExtensionDesc()	{ return "필드 파일"; }

	virtual void				OnWBObjectEditMouseMessage(VIEW_MESSAGE::ID message);	///< Workbench Mouse Edit Message

	virtual MAP_EDIT_TYPE		OnOpenFile(const char* _pSzFileName);					///< 파일 열기
	virtual std::string			OnSaveFile(const char* _pSzFileName, bool bSaveAs);		///< 파일 저장
	virtual void				OnCloseFile() {}

	virtual void				OnBetweenCoreToolUpdateAndRS3Render();
	virtual void				OnResetResourceAfterSceneCleared(bool bOnDestroy);

	virtual void				OnWBDevAction(){}
	virtual void				OnWBHelperRender(){}

	virtual void				OnCreateWorkspaceFile();

	virtual bool				OnCheckAlwaysPickingWhenMoused();

private:

	MAP_EDIT_TYPE 				LoadField_(const char* _pSzFileName, bool _bLoadMapConfig, bool _bBroadCastLogicLoadMessage);
	bool						LoadFieldXML_(const char* _pSzName, const char* _pSzLoadAs = NULL);
	void						SaveFieldXML_(const char* _pSzName);

	bool						IsBGMEmpty();
	void						SaveBGMXML( MXmlElement* pElementFieldInfo );

	std::string					m_strZoneName_;

	int							m_nFromZoneID_;
	int							m_nFromFieldID_;

	void						SetFieldResourceWithLoadedZone_();
	void						SetCreatedFieldResourceWithLoadedZone_();
	void						SetEditableLoadedZone_();

	typedef						std::map<std::string, int> W_NAME_PERCENT;
	W_NAME_PERCENT				m_weatherNamePercent_;
	std::string					m_weatherNameTable_[WEATHER_NUM];
	void						InitWeatherInfo_();


	// 렌더링할 씬노드
	std::vector< rs3::RSceneNode* > m_vecRenderSceneNodes;
};
