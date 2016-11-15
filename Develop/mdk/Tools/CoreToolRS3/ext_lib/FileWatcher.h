#pragma once

/*
 GPG 6권 1.10 자산 즉석 적재 에서 가져온것

 가져오면서 수정한 내역
* 파일의 변경뿐만 아니라 생성/삭제/리네임때도 통지되도록 수정
* 클래스의 멤버함수를 콜백으로 등록할수 있도록 FastDelegate 적용
*/


#include <string>
#include <vector>

// #define _WIN32_WINNT 0x0501
#include <windows.h>

#include <time.h>

#include "ext_lib/FastDelegate.h"

class FileWatcher
{
public:
	FileWatcher(const std::string& directoryToWatch);
	~FileWatcher();

	bool Update();

	typedef std::vector< std::string > StringVec;
// 	typedef void (*ChangedFileSetCallback)(const StringVec &);

	typedef fastdelegate::FastDelegate1< const StringVec & > ChangedFileSetCallback;

	void InstallChangedFileSetCallback(ChangedFileSetCallback cb) { m_cb = cb; }

private:
	bool Listen();
	void UpdateReportTiming();
	void ReportChangedFile(const std::string& filename);

	HANDLE m_directoryHandle;
	HANDLE m_notifyEvent;
	OVERLAPPED m_overlappedIo;
	char *m_notifyResults;

	StringVec m_pendingChangedFiles;
	__time64_t m_timeOfFirstPendingChange;
	bool m_timing;

	ChangedFileSetCallback m_cb;

private:
	FileWatcher(const FileWatcher&);
	FileWatcher& operator =(const FileWatcher&);
};