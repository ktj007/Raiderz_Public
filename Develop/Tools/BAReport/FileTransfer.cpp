#include "stdafx.h"
#include "FileTransfer.h"



CFileTransfer::CFileTransfer()
{
	m_bRun = FALSE;
	m_hThread = NULL;
	m_bDeleteSrcFile = false;
	m_pTransferCallback = NULL;
}

CFileTransfer::~CFileTransfer()
{
	if (m_hThread) {
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread = NULL;
	}

	m_bRun = false;
}

void CFileTransfer::Destroy()
{
	if (m_hThread) {
		WaitForSingleObject(m_hThread, INFINITE);
		m_hThread = NULL;
	}

	m_bRun = false;
}

void CFileTransfer::Log(const wchar_t* pszMessage)
{
	TRACE("[MapTransLOG] %s \n", pszMessage);
}

void CFileTransfer::PutFileQueue(CString strAddress, int nPort, CString strUserName, CString strPassword, CString strFileName, CString strRemoteFileName)
{
	CFtpItem* pItem = new CFtpItem;
	pItem->strAddress = strAddress;
	pItem->nPort = nPort;
	pItem->strUserName = strUserName;
	pItem->strPassword = strPassword;
	pItem->strFileName = strFileName;
	pItem->strRemoteFileName = strRemoteFileName;

	m_FtpItemQueue.push_back(pItem);
}

bool CFileTransfer::BeginTransfer()
{
	CWinThread* pWinThread = AfxBeginThread(ThreadProc, this, THREAD_PRIORITY_NORMAL);
	m_hThread = pWinThread->m_hThread;
	return true;
}

void CFileTransfer::EndTransfer()
{
	m_bRun = false;
	m_hThread = NULL;

	// DO TRANSFER FINISH //////////////////////
	if (m_pTransferCallback)
		m_pTransferCallback(REASON_END, 0, 0);
}

UINT ThreadProc(LPVOID pParam) 
{
	CFileTransfer* pTrans = (CFileTransfer*)pParam;
	pTrans->m_bRun = true;
	CFtpItem* pItem = NULL;
	
	for (size_t i = 0; i < pTrans->m_FtpItemQueue.size(); i++)
	{
		pItem = pTrans->m_FtpItemQueue[i];

		TRY {
			CFtpConnection* pFtpConnection = pTrans->m_INetSession.GetFtpConnection((LPCTSTR)pItem->strAddress, (LPCTSTR)pItem->strUserName, (LPCTSTR)pItem->strPassword, pItem->nPort);
			if (pFtpConnection == NULL) {
				pTrans->Log(L"[ERROR] FtpConnection Error");
			}
			if (pTrans->PutFile(pFtpConnection, pTrans->m_pTransferCallback, pItem->strFileName, pItem->strRemoteFileName) == false)
				pTrans->Log(L"[ERROR] Transfer failed");
			if (pFtpConnection) {
				pFtpConnection->Close();
				delete pFtpConnection;
				pFtpConnection = NULL;
			}
		} CATCH(CInternetException, e) {
			TCHAR szError[256];
			e->GetErrorMessage(szError, 256);
			pTrans->Log(szError);
			break;
		}
		END_CATCH

		delete pItem;
	}
	pTrans->m_FtpItemQueue.clear();


	pTrans->Log(L"SHUTDOWN THREAD");
	pTrans->EndTransfer();
	return 0;
}

bool CFileTransfer::PutFile(CFtpConnection* pFtpConnection, TRANSFERCALLBACK* pStatusCallback, CString strLocal, CString strRemote)
{
	bool bCleanup = false;
	CFile LocalFile;
	CFileException eFile;
	if (!LocalFile.Open((LPCTSTR)strLocal, CFile::modeRead|CFile::typeBinary, &eFile)) {
		TRACE("Can't Read the file\n");
		return false;
	}
	DWORD dwLocalSize = LocalFile.GetLength();
	
	CInternetFile* pRemoteFile = (CInternetFile*)pFtpConnection->OpenFile((LPCTSTR)strRemote, GENERIC_WRITE, INTERNET_FLAG_TRANSFER_BINARY);
	if (pRemoteFile == NULL) {
		TRACE("Can't Create remote file\n");
		return false;
	}

	DWORD dwTotalSent = 0;
	char szBuf[4096];
	while(m_bRun) {
		int nRead = LocalFile.Read(szBuf, 4096);
		if (nRead == 0) {
			bCleanup = true;
			LocalFile.Close();
			if (m_bDeleteSrcFile) DeleteFile(strLocal);
			

			pRemoteFile->Close();
			delete pRemoteFile;
			return true;
		}
		pRemoteFile->Write(szBuf, nRead);
		dwTotalSent += nRead;
		TRACE("Sent : %d \n", dwTotalSent);
		if (pStatusCallback)
			pStatusCallback(REASON_PROGRESS, dwTotalSent, dwLocalSize);
	}

	

	if (bCleanup == false) {
		LocalFile.Close();
		if (m_bDeleteSrcFile) DeleteFile(strLocal);

		pRemoteFile->Close();
		delete pRemoteFile;
		return true;
	}
	return true;
}

void CFileTransfer::SetDeleteSrcFile(bool bVal)
{
	// 사실은 쓰레드 락을 걸어줘야 한다.
	m_bDeleteSrcFile = bVal;
}

std::wstring CFileTransfer::GetTextFileNameInQueue()
{
	for (size_t i = 0; i < m_FtpItemQueue.size(); i++)
	{
		CFtpItem* pItem = m_FtpItemQueue[i];
		int n = pItem->strFileName.Find(L".txt");
		if (n > 0)
		{
			std::wstring ret = (LPCTSTR)pItem->strFileName;
			return ret;
		}
	}
	return L"";
}