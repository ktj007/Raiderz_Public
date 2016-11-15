#ifndef _G_DIALOG_INFO_MGR_H_
#define _G_DIALOG_INFO_MGR_H_

class GDialogInfo;
class GDialogSelectInfo;
class GDialogSayInfo;

typedef map<int, GDialogInfo*> MAP_DIALOGINFO;

class GDialogInfoMgr : public MTestMemPool<GDialogInfoMgr>
{
private:
	MAP_DIALOGINFO	m_mapDialogInfo;
	int				m_nNowTextID;

private:
	void ParseDialog(MXmlElement* pElement, MXml* pXml);
	void ParseSay(MXmlElement* pElement, MXml* pXml, GDialogInfo* pDialogInfo);
	void ParseSay(MXmlElement* pElement, MXml* pXml, GDialogSelectInfo* pDialogSelectInfo);
	void ParseSelect(MXmlElement* pElement, MXml* pXml, GDialogSayInfo* pDialogSayInfo);
	void ParseItem(MXmlElement* pElement, MXml* pXml, GDialogSelectInfo* pDialogSelectInfo);

	int MakeNewTextID();

public:
	GDialogInfoMgr();
	~GDialogInfoMgr();

	void Clear();

	bool Load(const wchar_t* szFileName);
	void Insert(GDialogInfo* pDialogInfo);
	GDialogInfo* Get(int nID);
	bool IsExist(int nDialogID);

	MAP_DIALOGINFO& GetContainer()	{ return m_mapDialogInfo; }		
};

#endif//_G_DIALOG_INFO_MGR_H_
