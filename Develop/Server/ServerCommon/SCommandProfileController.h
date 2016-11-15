#pragma once

#include "MNetServer.h"

class SCommandProfileController : public MTestMemPool<SCommandProfileController>
{
public:
	SCommandProfileController(minet::MCommandCommunicator* pCC, wstring strLogFolder, wstring strLogFileName, bool bIsAutoSave, int AutoSaveTickHour = 0);
	~SCommandProfileController();
	void Start();
	void Save(bool bIsFinish = false);
	void Update(float fDelta);

	bool IsAutoSave()	{ return (m_bIsAutoSave && (0 <= m_nAutoSaveTickHour)); }
	bool IsStarted()	{ return m_bIsStarted; }

private:
	bool				m_bIsStarted;

	minet::MCommandCommunicator*	m_pCC;
	wstring							m_strLogFolder;
	wstring							m_strLogFileName;
	const bool						m_bIsAutoSave;
	const int						m_nAutoSaveTickHour;
	double							m_dElapsedTime;

	wstring MakeFileNameTail();

};
