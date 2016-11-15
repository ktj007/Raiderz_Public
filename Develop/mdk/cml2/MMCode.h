#pragma once

//
// MMCode ( Maiet entertainment Markup Code )

#include <string>
#include <vector>
#include <map>
#include <list>
#include "RLib.h"

using namespace std;

namespace mmcode
{

// 제어코드--
class MControlCodeProc
{
public:
	virtual ~MControlCodeProc() {}
	virtual void Proc(const string& strCodeParam, void* pParam=0) = 0;
};

class MControlCode
{
private:
	string				m_strTag;
	MControlCodeProc*	m_pProc;
public:
	MControlCode(const string& strTag, MControlCodeProc* pProc=0) : m_pProc(pProc), m_strTag(strTag) {}
	virtual ~MControlCode() { if (m_pProc) delete m_pProc;	}
	void Proc(const string& strCodeParam, void* pParam=0)
	{
		if (m_pProc) m_pProc->Proc(strCodeParam, pParam);
	}
};

// 치환자--

class MReplaceCharsProc
{
public:
	virtual ~MReplaceCharsProc() {}
	virtual string Proc(void* pParam=0) = 0;
};


class MReplaceChars
{
private:
	string				m_strReplace;
	MReplaceCharsProc*	m_pProc;
public:
	MReplaceChars(const string& strReplace, MReplaceCharsProc* pProc=0) : m_pProc(pProc), m_strReplace(strReplace) {}
	virtual ~MReplaceChars() { if (m_pProc) delete m_pProc;	}

	string Proc(void* pParam=0) 
	{
		if (m_pProc) return m_pProc->Proc(pParam);
		return m_strReplace;
	}
};

// 태그 --
class MTag
{
private:
	string				m_strTag;
public:
	MTag(const string& strTag) : m_strTag(strTag) {}
	virtual ~MTag() { }
	const string& GetName() const { return m_strTag; }
	string& GetName() { return m_strTag; }
};


// 태그 관리자 --
class CML2_API MTagManager
{
public:
	typedef map<string, MReplaceChars*>	ReplaceCharsMap;
private:
	typedef map<string, MControlCode*>	ControlCodeMap;
	ControlCodeMap						m_ControlCodes;
	ReplaceCharsMap						m_ReplaceChars;

	typedef map<string, MTag*>			TagMap;
	TagMap								m_Tags;

public:
	virtual ~MTagManager();
	void AddControlCode(const string& strName, MControlCode* pHandler);
	void RemoveControlCode(const string& name);
	MControlCode* FindControlCode(const string& name);

	void AddReplaceChars(const string& strName, MReplaceChars* pHandler);
	void RemoveReplaceChars(const string& name);
	MReplaceChars* FindReplaceChars(const string& name);

	void AddTag(const string& strName, MTag* pHandler);
	void RemoveTag(const string& name);
	MTag* FindTag(const string& name);

	ReplaceCharsMap::iterator ReplaceCharsBegin() { return m_ReplaceChars.begin(); }
	ReplaceCharsMap::iterator ReplaceCharsEnd() { return m_ReplaceChars.end(); }
};


class MControlCodeTransformer;

class CML2_API MControlCodeTransResult
{
	friend class MControlCodeTransformer;
private:
	struct _Code
	{
		MControlCode*		m_pCode;
		string				m_strParam;
		_Code(MControlCode* code, const string& param) : m_pCode(code), m_strParam(param) {}
	};

	vector<_Code>			m_Codes;
	void AddCode(MControlCode* pCode, const string& param);

public:
	string					m_strOutput;
	void Proc(void* pParam=0);
};

class CML2_API MControlCodeTransformer
{
private:
	string	m_strIn;
	MTagManager&		m_Mgr;
public:
	MControlCodeTransformer(const string& in, MTagManager& mgr) : m_strIn(in), m_Mgr(mgr) {}
	virtual ~MControlCodeTransformer() {}
	void Input(const string& in) { m_strIn = in; }
	MControlCodeTransResult transform();
};

class CML2_API MReplaceCharsTransformer
{
private:
	string	m_strIn;
	MTagManager&		m_Mgr;
public:
	MReplaceCharsTransformer(const string& in, MTagManager& mgr) : m_strIn(in), m_Mgr(mgr) {}
	virtual ~MReplaceCharsTransformer() {}
	void Input(const string& in) { m_strIn = in; }
	string transform(void* pParam=0);
};

class CML2_API MSpliterCharsTransformer
{
private:
	string	m_strIn;
	MTagManager&		m_Mgr;
public:
	MSpliterCharsTransformer(const string& in, MTagManager& mgr) : m_strIn(in), m_Mgr(mgr) {}
	virtual ~MSpliterCharsTransformer() {}
	void Input(const string& in) { m_strIn = in; }
	void Splite(list<string>& strlist);
};


class CML2_API MNode
{
public:
	MTag*	m_pTag;
	string	m_strParam;
	string	m_strContents;
	MNode(MTag* pTag, const string& param, const string& contents) : m_pTag(pTag), m_strParam(param), m_strContents(contents) {}
	MNode(const string& text) : m_pTag(NULL), m_strContents(text) {}
	bool IsPlaneText() { return (m_pTag == NULL); }
	const string& GetPlaneText() { return m_strContents; }
};

class CML2_API MMMCodeTransResult
{
private:
public:
	vector<MNode>	m_Nodes;
	string			m_strOutput;

	void AddText(const string& text)
	{
		m_Nodes.push_back(MNode(text));
		m_strOutput += text;
	}

	void AddNode(MTag* pTag, const string& param, const string& contents)
	{
		m_Nodes.push_back(MNode(pTag, param, contents));
		m_strOutput += contents;
	}
};

class CML2_API MMMCodeTransformer
{
private:
	string	m_strIn;
	MTagManager&		m_Mgr;
public:
	MMMCodeTransformer(const string& in, MTagManager& mgr) : m_strIn(in), m_Mgr(mgr) {}
	virtual ~MMMCodeTransformer() {}
	void Input(const string& in) { m_strIn = in; }
	MMMCodeTransResult transform();
};


}