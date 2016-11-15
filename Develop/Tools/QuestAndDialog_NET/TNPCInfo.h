#ifndef _TNPCINFO_H
#define _TNPCINFO_H

//#include "MZFileSystem.h"
#include "TDialogInfo.h"
#include "CSChrInfo.h"

typedef map<int, TDialogInfo*> DIALOGINFOMAP;

/// NPC Á¤º¸
class TNPCInfo : public CSNPCInfo
{
protected:
	void ParseDialog(MXmlElement* pElement, MXml* pXml, DIALOGINFOMAP& dialogInfoMap);
	void ParseCheck(MXmlElement* pElement, MXml* pXml, TDialogNode* pDialogNode, TDialogInfo* pDialogInfo);
	void ParseCase(MXmlElement* pElement, MXml* pXml, TDialogNode* pDialogNode, int nDialogNodeType, TDialogInfo* pDialogInfo);
	void ParseSay(MXmlElement* pElement, MXml* pXml, TDialogNode* pDialogNode, TDialogInfo* pDialogInfo);
	void ParseSelect(MXmlElement* pElement, MXml* pXml, TDialogNode* pDialogNode,TDialogInfo* pDialogInfo);

	void LoadOnScreenDialog();
	void LoadOffScreenDialog();
	void SaveOnScreenDialogInfoToXml(MXmlElement* pElementMaietStrings);
	void SaveOffScreenDialogInfoToXml(MXmlElement* pElementMaietStrings);

public:
	DIALOGINFOMAP	m_mapOnScreenDialog;
	DIALOGINFOMAP	m_mapOffScreenDialog;

	TDialogInfo*			FindOnScreenDialogNode(int nDialogID);
	TDialogInfo*			FindOffScreenDialogNode(int nDialogID);

	TNPCInfo()
	{
		
	}

	void Clear()
	{
		while (false == m_mapOnScreenDialog.empty())
		{
			DIALOGINFOMAP::iterator itor = m_mapOnScreenDialog.begin();
			delete (*itor).second;
			m_mapOnScreenDialog.erase(itor);
		}

		while (false == m_mapOffScreenDialog.empty())
		{
			DIALOGINFOMAP::iterator itor = m_mapOffScreenDialog.begin();
			delete (*itor).second;
			m_mapOffScreenDialog.erase(itor);
		}
	}

	void LoadDialog();
	void SaveDialogToXml(MXmlElement* pElementMaietOnScreen, MXmlElement* pElementMaietOffScreen);
};

typedef map<string, vector<TNPCInfo*>> TNPCINFO_MAP_BYNAME;

class TNPCInfoMgr : public map<int, TNPCInfo*>
{
private:

protected:
	TNPCINFO_MAP_BYNAME m_mapNPCInfoByName;

	void ParseNPC(MXmlElement* pElement, MXml* pXml);

public:
	TNPCInfoMgr();
	virtual ~TNPCInfoMgr();
	void Clear();
	bool Load(const char* szFileName);
	TNPCInfo* Get(int nID);
	bool Exist(int nID);

	bool AddDialog(int nNPCID, bool bOnScreen, int nDialogID);
	void DeleteDialog(int nNPCID, bool bOnScreen, int nDialogID);

	void DeleteDialogNode(TDialogNode* pDialogNode);

	void FindNPCByName(const char* szNPCName, vector<int>& vecFindedNPC);

	void SaveDialogToXml();
};


#endif