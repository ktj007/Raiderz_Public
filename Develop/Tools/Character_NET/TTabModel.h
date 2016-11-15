#pragma once

enum MODLE_TYPE
{
	MODEL_TYPE_NONE = 0,
	MODEL_TYPE_PLAYER,
	MODEL_TYPE_MONSTER,
	MODEL_TYPE_NPC,
	MODEL_TYPE_MAPOBJECT
};

enum PLAYER_TYPE
{
	PLAYER_TYPE_NONE = 0,
	PLAYER_TYPE_FEMALE,
	PLAYER_TYPE_MALE
};

class ModelList
{
public:
	typedef std::vector<tstring> MODELLIST;
	MODELLIST m_Index;
};
typedef std::map<int, ModelList> MODELMAP;

class TTabModel
{
protected:
	MODELMAP		m_ModelMap;
	MODLE_TYPE		m_ModelType;
	PLAYER_TYPE		m_PlayerType;

protected:
	void			FindModelFiles( tstring &Path,ModelList & modellist);

public:
	TTabModel(void);
	virtual ~TTabModel(void);

	void			InitModelList();

	const MODELMAP&	GetMap() { return m_ModelMap; }

	void			SetModelType(String^ Name);
	MODLE_TYPE		GetModelType()	{ return m_ModelType; }

	void			SetPlayerType(String^ nodeName);
	PLAYER_TYPE		GetPlayerType()	{ return m_PlayerType;}
};
