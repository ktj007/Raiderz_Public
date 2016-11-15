#pragma once


// Update patch file node
class ZUpdatePatchNode
{
protected:
	char		m_szFileName[ _MAX_DIR];
	INT64		m_nSize;
	INT64		m_nCheckSum;
	bool		m_bValidate;
	bool		m_bPatchSuccess;


public:
	ZUpdatePatchNode( const char* pszName, INT64 nSize, INT64 nChecksum)
	{
		strcpy( m_szFileName, pszName);
		m_nSize = nSize;
		m_nCheckSum = nChecksum;
		m_bValidate = true;
		m_bPatchSuccess = true;
	}

	virtual ~ZUpdatePatchNode()
	{

	}

	const char* GetFileName()				{ return m_szFileName; }
	INT64 GetSize()					{ return m_nSize; }
	INT64 GetChecksum()				{ return m_nCheckSum; }
	bool IsValid()							{ return m_bValidate; }
	void SetValid( bool bValid)				{ m_bValidate = bValid; }
	bool IsPatchSuccess()					{ return m_bPatchSuccess; }
	void SetPatchSuccess( bool bSuccess)	{ m_bPatchSuccess = bSuccess; }
	
	bool CheckValid( CString* pstrErrorMsg)
	{
		/*m_bValidate = false;


		// 파일 유무 검사
		WIN32_FIND_DATA FindData;
		HANDLE hFind;
		if( (hFind = FindFirstFile( GetFileName(), &FindData )) == INVALID_HANDLE_VALUE)
		{
			FindClose( hFind );

			char szMsg[ 512];
			sprintf( szMsg, "[ZUpdatePatchNode] Needs to update : %s (not exist)", GetFileName());
			PutLog( szMsg);

			return false;
		}

		
		 Handle close
		FindClose( hFind);


		// CheckSum 구함
		DWORD dwCRC = GetCRC( GetFileName(), pstrErrorMsg);
		if ( dwCRC != GetChecksum())
		{
			 CRC가 다르면 패치 대상 파일
			char szMsg[ 512];
	#ifdef _DEBUG
			sprintf( szMsg, "[ZUpdatePatchNode] Needs to update : %s, remote(%u), local(%u)", GetFileName(), GetChecksum(), dwCRC);
	#else
			sprintf( szMsg, "[ZUpdatePatchNode] Needs to update : %s", GetFileName());
	#endif
			PutLog( szMsg);
		}
		else
		{
			m_bValidate = true;
		}*/
		
		return true;
	}
};
//class ZUpdatePatchList : public list<ZUpdatePatchNode*>{};