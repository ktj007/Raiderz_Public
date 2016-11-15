#pragma once

enum DUMMY_COMMAND_TYPE
{
	DUMMY_NOT			= -1,
	DUMMY_IF			= 0,
	DUMMY_ONCOMMAND,
	DUMMY_POST,
};
class XBirdClient;

//------------------------------------------------------------------------
// class XDummy_ParamValue
class XDummy_ParamValue
{
public:
	MCommandParameterType	m_nType;
	string					m_strValue;

	vector<XDummy_ParamValue> m_vecBlob;
	int						m_nBlobDataSize;
};
//------------------------------------------------------------------------
// class XDummy_Cmd
class XDummy_Cmd
{
public:
	int						m_CmdID;
	DUMMY_COMMAND_TYPE		m_cmdType;

	bool					m_bEnd;

protected:
	int						GetCommandIDByName(string strCmdName);

public:
	XDummy_Cmd();
	virtual ~XDummy_Cmd() {}

	void					ParseEnd(MXmlElement* pElement);

	virtual void			OnCommand(XBirdClient * pClient, MCommand * pCmd) {}
	virtual void			OnCompare(XBirdClient * pClient, MCommand * pCmd) {}
	virtual void			PostStart(XBirdClient * pClient) {}

	virtual DUMMY_COMMAND_TYPE		GetType() {return DUMMY_NOT; }
};

//------------------------------------------------------------------------
// class XDummy_If_Divide
class XDummy_If_Divide
{
public:
	XDummy_If_Divide() {}
	virtual ~XDummy_If_Divide();

	vector<XDummy_Cmd *>	m_vecNextCmd;

	bool					ParseDivide(MXmlElement* pElement);
};

//------------------------------------------------------------------------
// class XDummy_If
class XDummy_If : public XDummy_Cmd
{
public:
	enum DUMMY_IF_COMPARE
	{
		ERROR_COMPARE		= -1,
		EQUAL				= 0,
		LARGE				= 1,
		SMALL				= 2,
		LARGE_EQUAL			= 3,
		SMALL_EQUAL			= 4,
	};

	int						m_nCompare;
	int						m_nFirstParamIndex;
	XDummy_ParamValue		m_SecondValue;

	XDummy_If_Divide *		m_pThen;
	XDummy_If_Divide *		m_pElse;

private:
	DUMMY_IF_COMPARE		GetCompare(string strCompare);

public:
	XDummy_If();
	virtual ~XDummy_If();

	bool					ParseConditional(MXmlElement* pElement);

	virtual void			OnCompare(XBirdClient * pClient, MCommand * pCmd);

	virtual DUMMY_COMMAND_TYPE		GetType() {return DUMMY_IF; }
};

//------------------------------------------------------------------------
// class XDummy_OnCommand
class XDummy_OnCommand : public XDummy_Cmd
{
public:
	vector<XDummy_Cmd *>	m_vecNextCmd;

private:
	bool					ParseNextCommand(MXmlElement* pElement);
	bool					ParseConditionalCommand(MXmlElement* pElement);

public:
	XDummy_OnCommand() : XDummy_Cmd() {}
	virtual ~XDummy_OnCommand();

	bool					ParseOnCommand(MXmlElement* pElement);

	virtual void			OnCommand(XBirdClient * pClient, MCommand * pCmd);

	virtual DUMMY_COMMAND_TYPE		GetType() {return DUMMY_ONCOMMAND; }
};

//------------------------------------------------------------------------
// class XDummy_Post
class XDummy_Post : public XDummy_Cmd
{
private:
	vector<XDummy_ParamValue> m_vecPostParam;

private:
	bool					ParseParam(MXmlElement* pElement);

public:
	XDummy_Post() : XDummy_Cmd() {}
	virtual ~XDummy_Post() {}

	bool					ParsePost(MXmlElement* pElement);

	virtual void			PostStart(XBirdClient * pClient);

	virtual DUMMY_COMMAND_TYPE		GetType() {return DUMMY_POST; }
};

//------------------------------------------------------------------------
// class XDummyClient
class XDummyPattern
{
private:
	wstring					m_strName;

	vector<XDummy_Cmd *>	m_vecStart_Cmd;

private:

public:
	XDummyPattern();
	virtual ~XDummyPattern();

	bool					ParseDummyClient(MXmlElement* pElement, MXml* pXml);

	void					OnCommand(XBirdClient* pBirdClient, MCommand * pCmd);

	wstring					GetName() { return m_strName; }

};

class XDummyPatternMgr : public map<wstring, XDummyPattern*>
{
public:
	XDummyPatternMgr() {}
	virtual ~XDummyPatternMgr()
	{
		iterator itDummyClient = begin();
		while (itDummyClient != end())
		{
			SAFE_DELETE(itDummyClient->second);
			itDummyClient++;
		}
	}
	XDummyPattern* GetPattern(wstring strPatternName)
	{
		iterator itor = find(strPatternName);
		if (itor != end())
		{
			return (*itor).second;
		}
		return NULL;
	}
	void AddPattern(wstring strPatternName, XDummyPattern* pPattern)
	{
		insert(value_type(strPatternName, pPattern));
	}
};