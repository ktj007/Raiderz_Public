#ifndef _MCOMMANDPROFILER_H
#define _MCOMMANDPROFILER_H

#include "limits.h"
#include "MCommandManager.h"
#include "MTime.h"

namespace minet {

enum _ProfileItemType
{
	_PIT_SEND=0,
	_PIT_RECV,
	_PIT_MAX
};

struct MCommandProfileItem
{
	// 측정에 필요한 값
	MTimeChecker2		TimeChecker;	

	// 측정값
	float		fTotalTime;
	float		fMinTime;
	float		fMaxTime;

	unsigned int		nCount[_PIT_MAX];
	unsigned int		nBytes[_PIT_MAX];

	MCommandProfileItem()
	{
		Clear();
	}
	void Clear()
	{
		fTotalTime = 0.0f;
		fMinTime = 9999.9f;
		fMaxTime = 0.0f;

		nCount[_PIT_SEND] = 0;
		nCount[_PIT_RECV] = 0;
		nBytes[_PIT_SEND] = 0;
		nBytes[_PIT_RECV] = 0;
	}
};

#define MAX_COMMANDPROFILER_CMD_COUNT		65536		// 16비트

class MCommand;

class MCommandProfiler
{
private:
	string						m_strFolder;
	string						m_strFileName;

	bool						m_bInited;
	MCommandManager*			m_pCM;
	float						m_fTotalOnProcessTime;

	unsigned int				m_nTotalCmdCount[_PIT_MAX];
	unsigned int				m_nTotalBytes[_PIT_MAX];
	MTimeChecker2				m_timeTotal;

	vector<MCommandProfileItem>	m_Items;
	void PrintTitle(FILE* fp);
	void PrintSummary(FILE* fp);
	void PrintCmdItems(FILE* fp, FILE* fpCSV);
	void Reset();
public:
	MCommandProfiler();
	virtual ~MCommandProfiler();
	void Init(MCommandManager* pCM);
	void Analysis(const char* szFolder, const char* szFileName, bool bSaveCSV);
	void OnCommandBegin(MCommand* pCmd);
	void OnCommandEnd(MCommand* pCmd);
	void OnSend(MCommand* pCmd);
	void OnRecv(MCommand* pCmd);
	bool IsInited() { return m_bInited; }
};



} // namespace minet

#endif