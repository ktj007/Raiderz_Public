#pragma once

class ZFileTransferUI;

// Update UI infomation
class ZUpdateUIInfo
{
protected:
	ZFileTransferUI*	m_pFileTransferUI;
	INT64		m_nTotalPatchFileSize;
	INT64		m_nCurrPatchedFileSize;
	INT64		m_nTotalPatchFileCount;
	INT64		m_nCurrPatchedFileCount;
	int			m_nPatchFailedCount;


public:
	ZUpdateUIInfo()
	{
		m_nTotalPatchFileSize = 0;
		m_nCurrPatchedFileSize = 0;
		m_nTotalPatchFileCount = 0;
		m_nCurrPatchedFileCount = 0;
		m_nPatchFailedCount = 0;
	};

	void SetTransferUI( ZFileTransferUI* pUI)					{ m_pFileTransferUI = pUI; }

	const char* GetCurrPatchFileName()							{ return m_pFileTransferUI->GetDownloadFileName(); }
	INT64 GetTotalDownloadSize()						{ return m_pFileTransferUI->GetTotalDownloadSize(); }
	INT64 GetCurrDownloadSize()							{ return m_pFileTransferUI->GetCurrDownloadSize(); }

	INT64 GetTotalPatchFileSize()						{ return m_nTotalPatchFileSize; }
	void SetTotalPatchFileSize( INT64 nSize)			{ m_nTotalPatchFileSize = nSize; }
	void AddTotalPatchFileSize( INT64 nSize)			{ m_nTotalPatchFileSize += nSize; }
	void ClearTotalPatchFileSize()								{ m_nTotalPatchFileSize = 0; }

	INT64 GetCurrPatchedFileSize()						{ return m_nCurrPatchedFileSize; }
	void SetCurrPatchedFileSize( INT64 nSize)			{ m_nCurrPatchedFileSize = nSize; }
	void AddCurrPatchedFileSize( INT64 nSize)			{ m_nCurrPatchedFileSize += nSize; }
	void ClearCurrPatchedFileSize()								{ m_nCurrPatchedFileSize = 0; }

	INT64 GetTotalPatchFileCount()						{ return m_nTotalPatchFileCount; }
	void SetTotalPatchFileCount( unsigned long nCount)			{ m_nTotalPatchFileCount = nCount; }
	void AddTotalPatchFileCount( unsigned long nCount)			{ m_nTotalPatchFileCount += nCount; }
	void ClearTotalPatchFileCount()								{ m_nTotalPatchFileCount = 0; }

	INT64 GetCurrPatchedFileCount()						{ return m_nCurrPatchedFileCount; }
	void SetCurrPatchedFileCount( unsigned long nCount)			{ m_nCurrPatchedFileCount = nCount; }
	void AddCurrPatchedFileCount( unsigned long nCount)			{ m_nCurrPatchedFileCount += nCount; }
	void ClearCurrPatchedFileCount()							{ m_nCurrPatchedFileCount = 0; }

	int GetPatchFailedCount()									{ return m_nPatchFailedCount; }
	void SetPatchFailedCount( int nCount)						{ m_nPatchFailedCount = nCount; }
	void AddPatchFailedCount( int nCount)						{ m_nPatchFailedCount += nCount; }
	void ClearPatchFailedCount()								{ m_nPatchFailedCount = 0; }
};