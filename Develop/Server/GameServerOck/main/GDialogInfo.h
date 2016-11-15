#ifndef _GDIALOG_INFO_H
#define _GDIALOG_INFO_H

struct INTERACTION_PROGRESS;
class GDialogSayInfo;
class GDialogSelectInfo;
class GItemAddRemoveInfo;

class GDialogInfo : public MTestMemPool<GDialogInfo>
{
public:
	int		m_nID;
	wstring	m_strText;
	int		m_nTextID;	
	int		m_nExit;

	GDialogSayInfo*	m_pDialogSayInfo;

public:
	GDialogInfo();
	virtual ~GDialogInfo();
};

class GDialogSayInfo : public MTestMemPool<GDialogSayInfo>
{
public:
	wstring m_strText;
	int		m_nTextID;

	vector<GDialogSelectInfo*> m_vecDialogSelectInfo;
	
public:
	GDialogSayInfo();
	virtual ~GDialogSayInfo();
};

class GDialogSelectInfo : public MTestMemPool<GDialogSelectInfo>
{
public:
	wstring	m_strText;
	int		m_nTextID;
	int		m_nExit;

	GDialogSayInfo*		m_pDialogSayInfo;
	GItemAddRemoveInfo*	m_pItemAddRemoveInfo;

public:
	GDialogSelectInfo();
	virtual ~GDialogSelectInfo();
};


#endif // _GDIALOG_INFO_H