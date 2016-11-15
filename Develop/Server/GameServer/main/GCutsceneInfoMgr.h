#pragma once

class GCutsceneInfo : public MTestMemPool<GCutsceneInfo>
{
public:
	int		nID;
	bool	bRepeatable;
	int		nFieldIntro;

	GCutsceneInfo() : nID(INVALID_ID), bRepeatable(true), nFieldIntro(INVALID_ID) {}
};

#define CUTSCENE_XML_TAG_MAIET			"maiet"
#define CUTSCENE_XML_TAG_CUTSCENE		"CUTSCENE"
#define CUTSCENE_XML_ATTR_ID			"id"
#define CUTSCENE_XML_ATTR_REPEATABLE	"repeatable"
#define CUTSCENE_XML_ATTR_FIELDINTRO	"field_intro"

class GCutsceneInfoMgr : public MTestMemPool<GCutsceneInfoMgr>
{
	typedef map<int, GCutsceneInfo*> MAP_CUTSCENE;
public:
	GCutsceneInfoMgr();
	~GCutsceneInfoMgr();

	void			Clear();
	virtual bool	Load(const wchar_t* szFileName);
	void			Insert(GCutsceneInfo* pCutsceneInfo);
	GCutsceneInfo*	Find(int nID);
	int				GetFieldIntroCutscneneID( int nFieldID );

	bool			IsExist(int nID);

private:
	void			Parse_Cutscene(MXmlElement* pElement, MXml* pXml);
	MAP_CUTSCENE	m_mapCutsceneInfo;
};
