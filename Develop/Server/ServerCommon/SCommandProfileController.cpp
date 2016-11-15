#include "StdAfx.h"
#include "SCommandProfileController.h"
#include "SAppHelper.h"
#include "MLocale.h"

SCommandProfileController::SCommandProfileController(minet::MCommandCommunicator* pCC, wstring strLogFolder, wstring strLogFileName, bool bIsAutoSave, int nAutoSaveTickHour)
: m_bIsStarted(false)
, m_pCC(pCC)
, m_strLogFolder(strLogFolder)
, m_strLogFileName(strLogFileName)
, m_bIsAutoSave(bIsAutoSave)
, m_nAutoSaveTickHour(nAutoSaveTickHour)
, m_dElapsedTime(0)
{
	if (m_bIsAutoSave)
	{
		Start();
	}
}

SCommandProfileController::~SCommandProfileController()
{
	if (IsStarted())
	{
		Save();
	}
}

void SCommandProfileController::Start()
{
	m_bIsStarted = true;
	m_pCC->InitProfiler();
}

void SCommandProfileController::Save(bool bIsFinish)
{
	if (m_strLogFileName.empty())
		return;

	wstring strNewLogFileName = m_strLogFileName + MakeFileNameTail();

	if (!m_strLogFolder.empty() &&
		m_strLogFolder != L"./" &&
		m_strLogFolder != L"." &&
		m_strLogFolder != L".\\")
	{
		::CreateDirectory(m_strLogFolder.c_str(), NULL);
	}

	m_pCC->ProfileAnalysis(MLocale::ConvTCHARToAnsi(m_strLogFolder.c_str()).c_str(), MLocale::ConvTCHARToAnsi(strNewLogFileName.c_str()).c_str(), true);

	if (bIsFinish) m_bIsStarted = false;
}

wstring SCommandProfileController::MakeFileNameTail()
{
	return SAppHelper::MakeCurrentDateTimeStringForFileName();
}

void SCommandProfileController::Update( float fDelta )
{
	if (m_bIsAutoSave == false) return;
	if (m_nAutoSaveTickHour <= 0) return;

	m_dElapsedTime += (double)(fDelta);

	if (m_dElapsedTime > (60 * m_nAutoSaveTickHour))
	{
		Save();
		m_pCC->InitProfiler();

		m_dElapsedTime = 0;
	}
}