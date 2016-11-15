#ifndef _G_TRAINING_INFO_MGR_H_
#define _G_TRAINING_INFO_MGR_H_

class GTrainingInfo;

#define TRAINING_XML_TAG_MAIET				"maiet"
#define TRAINING_XML_TAG_TRAINING			"TRAINING"
#define TRAINING_XML_TAG_TALENT				"TALENT"
#define TRAINING_XML_ATTR_ID				"id"
#define TRAINING_XML_ATTR_TYPE				"type"
#define TRAINING_XML_ATTR_DIALOG			"dialog"
#define TRAINING_XML_ATTR_CONDITION			"condition"

typedef map<int, GTrainingInfo*> MAP_TRAININGINFO;

class GTrainingInfoMgr
{
private:
	MAP_TRAININGINFO	m_mapTrainingInfo;

private:
	void ParseTraining(MXmlElement* pElement, MXml* pXml);
	void ParseTalent(MXmlElement* pElement, MXml* pXml, GTrainingInfo* pTrainingInfo);

public:
	GTrainingInfoMgr();
	~GTrainingInfoMgr();

	void Clear();

	bool Load(const char* szFileName);
	void Insert(GTrainingInfo* pTrainingInfo);
	GTrainingInfo* Get(int nID);
	MAP_TRAININGINFO& GetContainer()	{ return m_mapTrainingInfo; }	
};

#endif//_G_TRAINING_INFO_MGR_H_
