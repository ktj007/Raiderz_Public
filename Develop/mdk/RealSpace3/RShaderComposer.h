#pragma once

#include <string>
#include <list>
#include <set>
#include <map>

using namespace std;

namespace rs3 {

class RPathSplitter;
class RShaderComposer;

class RShaderComponent
{
	friend RShaderComposer;

protected:
	string	m_strName;
	bool	m_bInputStruct;
	bool	m_bOutputStruct;
	bool	m_bMainVS;
	bool	m_bMainPS;
	string	m_strShaderString;
	string	m_strErrorString;
	list<string>	m_listDefines;
	list<string>	m_listDependencies;
	map<string,string>	m_mapExports;	// first = export name , second = variable type

	int		m_nCurrentLine;
	string	m_strCurrentLine;
	DWORD	m_dwCRC;

	bool ParseInterface(istringstream& iss, int& nLine);

public:

	RShaderComponent();
	~RShaderComponent();

	bool Create(const string& strStringBuffer);									///< string 栏肺何磐 积己
	bool CreateFromFile(const char* szFileName);	///< 颇老肺何磐 积己

	bool HasInputStruct() { return m_bInputStruct; }
	bool HasOutputStruct() { return m_bOutputStruct; }
	bool HasVS() { return m_bMainVS; }
	bool HasPS() { return m_bMainPS; }
	bool HasExport(const string& strExport);
	string GetExportName(const string& strExport) { return m_strName + "_export_" + strExport; }

	const string& GetName()					{ return m_strName; }
	const string& GetShaderString()			{ return m_strShaderString; }
	const list<string>& GetDefines()		{ return m_listDefines; }
	const list<string>& GetDependencies()	{ return m_listDependencies; }
	
	int				GetErrorLine()		{ return m_nCurrentLine; }
	const string	GetErrorString()	{ return m_strErrorString; }
	const string&	GetErrorLineString() { return m_strCurrentLine; }

	DWORD			GetCRC()			{ return m_dwCRC; }
};

inline string TrimLeft(const string& str)
{ 
	size_t n = str.find_first_not_of(" \t\v\n\r"); 
	return n == string::npos ? "" : str.substr(n); 
}

inline string TrimRight(const string& str)
{ 
	size_t n = str.find_last_not_of(" \t\v\n\r"); 
	return n == string::npos ? "" : str.substr(0, n + 1); 
}

inline bool StringReplace(string& str, const string& strFind, const string& strReplace)
{
	size_t n = str.find(strFind);
	if(n!=string::npos)
	{
		str = str.substr(0, n) + strReplace + str.substr( n+ strFind.length());
		return true;
	}
	return false;
}


class RShaderComposer {
	typedef list< RShaderComponent* > ComponentList;
	ComponentList m_components;

	int		m_nTexCoord;
	int		m_nTexCoordIn;
	string	m_strErrorString;
	string	m_strShader;
	set<string>	m_setDefines;

	void Add(string strToAdd) { m_strShader+= strToAdd; }

public:
	int	GetComponentCount()			{ return (int)m_components.size(); }
	bool AddComponent( RShaderComponent* pComponet );

	bool Make();

	const string& GetErrorString()	{ return m_strErrorString; }
	const string& GetShaderString() { return m_strShader; }

	int	GetTexCoordCount()			{ return m_nTexCoord;	}
}
;

}