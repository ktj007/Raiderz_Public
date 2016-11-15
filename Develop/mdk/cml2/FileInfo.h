//
//	Misc Functions about File
//
//                                              written by lee jang ho
//
//////////////////////////////////////////////////////////////////////
#ifndef _FILEINFO_H
#define _FILEINFO_H

#include <windows.h>
#include <time.h>
#include "RLib.h"

// 파일의 최근 업데이트된 시간 얻기
CML2_API BOOL GetLastUpdate(const char *pFileName, FILETIME *ft);

// 폴더의 최근 업데이트된 시간 얻기
CML2_API BOOL GetLastDirectoryUpdate(const char* pDirectoryName, FILETIME *ft);

// 파일명을 공통 명칭으로 변경
CML2_API void TrimFilePathName( char* szFileName);

// 파일이 업데이트 되었는가?
//		pFileName
//		ot			:	비교할 시간 ( 보통 오픈했을 때 시간 )
CML2_API BOOL IsModifiedOutside(const char *pFileName, FILETIME ot);

// Extension 제거
CML2_API BOOL RemoveExtension(char *pRemoveExt, const char *pFileName);

// Extension 교체
CML2_API void ReplaceExtension(char *pTargetName, const char *pSourceName, char *pExt);

// 두 경로 붙이기
CML2_API void GetMergedPath( char* szOutPath, const char* szBasePath, const char* szRelPath);

// 상대 경로 얻기
CML2_API void GetRelativePath(char *pRelativePath, const char *pBasePath, const char *pPath);

// 절대 경로 얻기
CML2_API void GetFullPath(char *pFullPath, const char *pBasePath, const char *pRelativePath);
//또 절대 경로 얻기... current directory를 기준으로...
CML2_API void GetFullPath(char *pFullPath, const char *pRelativePath);

// path 만 얻어내기..
CML2_API void GetPurePath(char *pPurePath,const char *pFilename);
// path 와 extension을 제거한 순수한 파일이름 얻기.
CML2_API void GetPureFilename(char *pPureFilename,const char *pFilename);
// extension 을 얻어낸다
CML2_API void GetPureExtension(char *pPureExtension,const char *pFilename);
// 파일이름 + extension 을 얻어낸다
CML2_API void GetPureFilenameExt(char *pPureFilenameExt,const char *pFilename);

// 절대 경로인가? ( 네트워크 경로는 지원하지 않음 )
CML2_API BOOL IsFullPath(const char *pPath);

// 헤더 읽기 ( 헤더의 시작 int는 헤더 전체 크기를 가지고 있다. )
CML2_API BOOL ReadHeader(HANDLE hFile, void *pHeader, int nHeaderSize);

// File의 CheckSum을 구한다. (날짜, 사이즈, 파일의 내용)
CML2_API DWORD GetFileCheckSum(char* pszFileName);

// 파일이 존재하는가?
CML2_API bool IsExist(const char *filename);
CML2_API bool IsExist(const wchar_t *filename);

// 부모 디렉토리 이름을 얻는다.
CML2_API void GetParentDirectory(char* pszFileName);

// 주어진 경로까지의 디렉토리를 생성한다.
CML2_API bool MakePath(const char* pszFileName);

// time_t 를 FILETIME 으로 변환 Code from MSDN
CML2_API void time_tToFILETIME(time_t t, LPFILETIME pft);

// 파일의 WriteTime 을 설정한다
CML2_API BOOL MSetFileTime(LPCTSTR lpszPath, FILETIME ft);

CML2_API bool GetMyDocumentsPath(char* path);

CML2_API bool CreatePathA(char* path);
CML2_API bool CreatePathW(wchar_t* path);

#ifdef _UNICODE
#define CreatePath		CreatePathW
#else
#define CreatePath		CreatePathA
#endif


// '\\' 를 '/'로 대체한다
CML2_API void ReplaceBackSlashToSlash(char* szPath);

// 끝에 / 를 더한다
CML2_API void AddSlash(char *szPath);

#ifdef WIN32
#pragma comment(lib, "Shlwapi.lib")
#endif

#endif	// _FILEINFO_H
