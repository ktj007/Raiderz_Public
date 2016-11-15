#ifndef _XSTRCMD_H
#define _XSTRCMD_H

#include <string>
#include <list>
#include <vector>
#include <map>
using namespace std;

#define		XSTRCMD_TEXSIZE				2048
#define		XSTRCMD_NAMESIZE			256
#define		XSTRCMD_LIMIT_REPEAT_COUNT	3

#define ARGVNoMin	(-1)
#define ARGVNoMax	(-1)

struct XStrCmdArgvInfo
{
	wchar_t	*cargv[256];
	int		cargc;
	wchar_t	argbuf[2048];		// 실제 argv 값이 들어가는 buf
};

class XStrCmd;
class XStrCmdManager;

typedef map<std::wstring, XStrCmd*>	XStrCmdMap;
typedef bool(XStrCmdProc)(const wchar_t* line, const int argc, wchar_t **const argv);

/// 채팅 명령어
class XStrCmd
{
private:
	int					m_nID;
	wchar_t				m_szName[XSTRCMD_NAMESIZE];
	wchar_t				m_szUsage[XSTRCMD_TEXSIZE];
	wchar_t				m_szHelp[XSTRCMD_TEXSIZE];
	int					m_nMinArgs;
	int					m_nMaxArgs;
	unsigned long int	m_nFlag;
	XStrCmdProc*		m_fnProc;
protected:
public:
						XStrCmd(int					nID, 
								const wchar_t*			szName, 
								int					nMinArgs=ARGVNoMin, 
								int					nMaxArgs=ARGVNoMax, 
                                const wchar_t*			szUsage=NULL, 
								const wchar_t*			szHelp=NULL);
	virtual				~XStrCmd() { }
	bool				OnProc(const wchar_t* line, const int argc, wchar_t **const argv);
	void				SetProc(XStrCmdProc* fnProc)	{ m_fnProc = fnProc; }
	const int			GetID()							{ return m_nID; }
	const wchar_t*			GetName()						{ return m_szName; }
	const wchar_t*			GetUsage()						{ return m_szUsage; }
	const wchar_t*			GetHelp()						{ return m_szHelp; }
	XStrCmdProc*		GetProc()						{ return m_fnProc; }
	int					GetMinArgs()					{ return m_nMinArgs; }
	int					GetMaxArgs()					{ return m_nMaxArgs; }
	unsigned long int	GetFlag()						{ return m_nFlag; }
};

/// 채팅 명령어 관리자
class XStrCmdManager
{
private:
	XStrCmdMap				m_CmdMap;
	map<wstring, wstring>		m_AliasMap;
	XStrCmd*				MakeArgv(wchar_t* szLine, XStrCmdArgvInfo* pAI);
public:
							XStrCmdManager();
	virtual					~XStrCmdManager();
	void					Clear();
	void					AddCommand(int					nID,
									   const wchar_t*			szName, 
                                       XStrCmdProc*			fnProc, 
                                       int					nMinArgs=ARGVNoMin, 
                                       int					nMaxArgs=ARGVNoMax,
                                       const wchar_t*			szUsage=NULL, 
                                       const wchar_t*			szHelp=NULL);
	void					AddAlias(const wchar_t* szNewCmdName, const wchar_t* szTarCmdName);
	bool					DoCommand(wchar_t* szLine);
	XStrCmd*				GetCommandByName(const wchar_t* szName);
	XStrCmd*				GetCommandByID(int nID);			// 순차검색하므로 느리다.
	XStrCmdMap::iterator	GetCmdBegin()	{ return m_CmdMap.begin(); }
	XStrCmdMap::iterator	GetCmdEnd()		{ return m_CmdMap.end(); }
	int						GetCmdCount()	{ return (int)m_CmdMap.size(); }
};

#endif