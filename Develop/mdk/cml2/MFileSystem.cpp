#include "stdafx.h"
#include "MFileSystem.h"
#include "LzmaEnc.h"
#include "LzmaDec.h"
#include <algorithm>
#include <shlwapi.h>
#include <io.h>
#include "MLocale.h"


// Size of long value
const static size_t LONG_SIZE = sizeof( long);


// Global instance of file system
static MFileSystem* g_pFileSys = NULL;




// LZMA memory allocator
static void* SzBigAlloc( void*, size_t size)	{ return VirtualAlloc( 0, size, MEM_COMMIT, PAGE_READWRITE);	}
static void SzBigFree( void*, void* address)	{ VirtualFree( address, 0, MEM_RELEASE);						}
static ISzAlloc g_BigAlloc = { SzBigAlloc, SzBigFree };

static void* SzAlloc( void*, size_t size)		{ return malloc( size);		}
static void SzFree( void*, void* address)		{ free( address);			}
static ISzAlloc g_Alloc = { SzAlloc, SzFree };





// _trim_filename
inline string _trim_filename( string str)
{
	// Replace '\' to '/'
	replace( str.begin(), str.end(), '\\', '/');

	// Trim left and right
	string::size_type n;
	n = str.find_first_not_of( " ");		if ( n != string::npos)  str = str.substr( n, str.length());
	n = str.find_last_not_of( " /");		if ( n != string::npos)  str = str.substr( 0, n + 1);

	if ( str == string( "."))				str = "";
	if ( str.compare( 0, 2, "./") == 0)		str.erase( 0, 2);

	return str;
}


// _pick_filename
inline string _pick_filename( string str)
{
	transform( str.begin(), str.end(), str.begin(), tolower);

	string::size_type n = str.find_last_of( "/");
	if ( n != string::npos)		str = str.substr( n + 1, str.length());

	return str;
}


// _get_path
void _get_path( const string& strPath, PATHINDEX* pIndex)
{
	string strFindPath = strPath + string( "*.*");

	WIN32_FIND_DATA _find_data;
	HANDLE hFind = ::FindFirstFile( strFindPath.c_str(), &_find_data);
	if ( hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if ( _find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ( _find_data.cFileName[ 0] == '.')
					continue;


				string _path = strPath + string( _find_data.cFileName) + string( "/");
				pIndex->push_back( _path);

				_get_path( _path, pIndex);
			}

		}	while( ::FindNextFile( hFind, &_find_data) != 0);

		::FindClose( hFind);
	}
}


// _patch_file_data
bool _patch_file_data( HANDLE* pHandle, MCrypt* pCrypt, unsigned char** pData, size_t nFileSize, DWORD nSectorPos, size_t nSectorSize)
{
	if ( *pData != NULL)		return true;


	// Move file pointer
	::SetFilePointer( *pHandle, nSectorPos, 0, FILE_BEGIN);


	// Read file sector
	unsigned char* _compdata = new unsigned char[ nSectorSize];
	DWORD _bytes_read;
	if ( ::ReadFile( *pHandle, _compdata, nSectorSize, &_bytes_read, NULL) == FALSE)
	{
		delete [] _compdata;
		return false;
	}


	// Decrypt data
 	if ( pCrypt->Validate() == false  ||  pCrypt->Decrypt( _compdata, nSectorSize) == false)
	{
		delete [] _compdata;
		return false;
	}


	// Decoding
	CLzmaDec _decryptor;
	LzmaDec_Construct( &_decryptor);

	SRes _result;
	_result = LzmaDec_AllocateProbs( &_decryptor, LZMA_PROPS, LZMA_PROPS_SIZE, &g_Alloc);
	if ( _result == SZ_OK)
	{
		*pData = new unsigned char[ nFileSize];
		_decryptor.dic = *pData;
		_decryptor.dicBufSize = nFileSize;

		SizeT _srclen = nSectorSize;
		ELzmaStatus _status;

		LzmaDec_Init( &_decryptor);
		_result = LzmaDec_DecodeToDic( &_decryptor, nFileSize, (Byte*)_compdata, &_srclen, LZMA_FINISH_END, &_status);
		delete [] _compdata;

		LzmaDec_FreeProbs( &_decryptor, &g_Alloc);
	}


	if ( _result != SZ_OK)
	{
		if ( *pData != NULL)
		{
			delete [] *pData;
			*pData = NULL;
		}
		return false;
	}

	return true;
}







// MCrypt
MCrypt::MCrypt()
{
	m_pKeyTable = NULL;
}


// ~MCrypt
MCrypt::~MCrypt()
{
	Destroy();
}


// Create
bool MCrypt::Create()
{
	// Create key table
	m_pKeyTable = new unsigned char[ 0x10000];
	unsigned int _poly = 0x85F24C5A;
	for ( unsigned int i = 0;  i < 0x10000;  i++)
	{
		unsigned int r = i + 1;
		for ( int j = 0;  j < 8;  j++)
			r = (r >> 1) ^ (_poly & ~(( r & 0x00000001) - 1));

		m_pKeyTable[ i] = (unsigned char)r;
	}

	return true;
}


// Destroy
bool MCrypt::Destroy()
{
	if ( m_pKeyTable != NULL)
	{
		delete [] m_pKeyTable;
		m_pKeyTable = NULL;
	}
	return true;
}


// Encrypt
bool MCrypt::Encrypt( void* pBuff, DWORD nBuffLen)
{
	if ( m_pKeyTable == NULL)	return false;

	// Encrypt
	unsigned char* _buff = (unsigned char*)pBuff;
	_buff[ 0] ^= ( _buff[ nBuffLen - 1] >> 1) ^ m_pKeyTable[ 0];
	for ( DWORD i = 1;  i < nBuffLen;  i++)
		_buff[ i] ^= ( _buff[ i - 1] >> 1) ^ m_pKeyTable[ i & 0x0000FFFF];

	return true;
}


// Decrypt
bool MCrypt::Decrypt( void* pBuff, DWORD nBuffLen)
{
	if ( m_pKeyTable == NULL)	return false;

	// Decrypt
	unsigned char* _buff = (unsigned char*)pBuff;
	for ( DWORD i = (nBuffLen - 1);  i > 0;  i--)
		_buff[ i] ^= ( _buff[ i - 1] >> 1) ^ m_pKeyTable[ i & 0x0000FFFF];
	_buff[ 0] ^= ( _buff[ nBuffLen - 1] >> 1) ^ m_pKeyTable[ 0];

	return true;
}


// GetCRC
unsigned short MCrypt::GetCRC( void* pBuff, DWORD nBuffLen)
{
	if ( m_pKeyTable == NULL)	return 0;

	// Get crc
	unsigned char* _buff = (unsigned char*)pBuff;
	unsigned short _crc = 0x3F75;
	for ( DWORD i = 0;  i < nBuffLen;  i++)
		_crc = (_crc << 1) + m_pKeyTable[ _buff[ i] & 0x0000FFFF];

	return _crc;
}


// Validate
bool MCrypt::Validate()
{
	if ( m_pKeyTable == NULL)	return false;

	unsigned int _index = rand() & 0x00010000;
	unsigned int _poly = 0x85F24C5A;
	unsigned int r = _index + 1;
	for ( int j = 0;  j < 8;  j++)
		r = (r >> 1) ^ (_poly & ~(( r & 1) - 1));

	if ( m_pKeyTable[ _index] != (r & 0x000000FF))
		return false;

	return true;
}








// MFile
MFile::MFile()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_pData = NULL;
	m_nFilePos = 0;
	m_nFileSize = 0;
	m_nSectorPos = 0;
	m_nSectorSize = 0;
	m_bCompressed = false;
	m_bAllowAccUncompFile = false;
}


// ~MFile
MFile::~MFile()
{
	Close();
}

void MFile::FileFindAndChangeState( const char* szInFileName, string& strOutFileName )
{
	strOutFileName = _trim_filename( szInFileName);
	strOutFileName = _pick_filename( strOutFileName );

	g_pFileSys->_find_file_and_change_state( strOutFileName );
}

// Open
bool MFile::Open( const char* szFileName, bool bAllowAccessUncompFile)
{
	if ( g_pFileSys == NULL  ||  m_hFile != INVALID_HANDLE_VALUE  ||  szFileName == NULL  ||  szFileName[ 0] == NULL)
		return false;


	if ( true == g_pFileSys->IsAllowedAccessUncompFile() || 
		 true == IsAllowedAccessUncompFile()				)
		 bAllowAccessUncompFile = true;


	// Open file
	bool _result = false;
	string _filename = _trim_filename( szFileName);

	const MFileInfo* pInfo = NULL;
	switch ( g_pFileSys->GetAccessMode())
	{
	case MFILEACCESS_GENERIC :
		{
			// Read unencrypted file
			if ( g_pFileSys->_find_fileinfo( _filename, &pInfo) == true)
			{
				m_hFile = ::CreateFile( pInfo->strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if ( m_hFile == INVALID_HANDLE_VALUE)
				{
					((MFileInfo*)pInfo)->bExist = false;

					if ( g_pFileSys->_find_fileinfo_from_disk( _filename, &pInfo) == false)
						break;

					m_hFile = ::CreateFile( pInfo->strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if ( m_hFile == INVALID_HANDLE_VALUE)
						break;
				}


				m_strFileName = pInfo->strFileName;
				m_nFilePos = 0;
				m_nFileSize = ::GetFileSize( m_hFile, NULL);
				m_nSectorPos = 0;
				m_nSectorSize = 0;

				_result = true;
			}

			break;
		}

	case MFILEACCESS_ENCRYPTED :
		{
			// Read encrypted file
			if ( g_pFileSys->_find_fileinfo( _filename, &pInfo) == true)
			{
				m_hFile = ::CreateFile( pInfo->strMotherFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if ( m_hFile != INVALID_HANDLE_VALUE)
				{
					m_strFileName = pInfo->strFileName;
					m_nFilePos = 0;
					m_nFileSize = pInfo->nFileSize;
					m_nSectorPos = pInfo->nSectorPos;
					m_nSectorSize = pInfo->nSectorSize;
					m_bCompressed = true;

					_result = true;
				}
			}

			// Read uncompressed file
			if ( _result == false  &&  bAllowAccessUncompFile == true)
			{
				m_hFile = ::CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if ( m_hFile != INVALID_HANDLE_VALUE)
				{
					m_strFileName = szFileName;
					m_nFilePos = 0;
					m_nFileSize = ::GetFileSize( m_hFile, NULL);
					m_nSectorPos = 0;
					m_nSectorSize = 0;
					m_bCompressed = false;

					_result = true;
				}
			}

			break;
		}
	}

#ifdef _DEBUG
	if ( g_pFileSys->GetAccessMode() == MFILEACCESS_ENCRYPTED  &&  _result == false)
	{
		char _buff[ 512];
#if _MSC_VER < 1400
		sprintf( _buff, "Cannot open file : %s\n", szFileName);
#else
		sprintf_s( _buff, "Cannot open file : %s\n", szFileName);
#endif
	}
#endif


	// Call event handler
	if ( _result == true)
		g_pFileSys->OnOpenFile( m_strFileName.c_str(), pInfo);

	return _result;
}


// Close
bool MFile::Close()
{
	if ( g_pFileSys == NULL  ||  m_hFile == INVALID_HANDLE_VALUE)
		return false;


	// Call event handler
	g_pFileSys->OnCloseFile( m_strFileName.c_str());


	// Release data
	if ( m_pData != NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
	}


	// Close file handle
	if ( m_hFile != NULL)
	{
		::CloseHandle( m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}


	// Reset variables
	m_strFileName.clear();
	m_nFilePos = 0;
	m_nFileSize = 0;
	m_nSectorPos = 0;
	m_nSectorSize = 0;
	m_bCompressed = false;

	return true;
}


// GetLength
size_t MFile::GetLength() const
{
	if ( g_pFileSys == NULL  ||  m_hFile == INVALID_HANDLE_VALUE)
		return 0;

	return m_nFileSize;
}


// GetFileName
const string& MFile::GetFileName() const
{
	return m_strFileName;
}


// Seek
DWORD MFile::Seek( long nOffset, int nOrigin)
{
	if ( g_pFileSys == NULL  ||  m_hFile == INVALID_HANDLE_VALUE)
		return 0;


	if ( m_bCompressed == false)
	{
		DWORD _pos;

		switch ( nOrigin)
		{
		case MFile::BEGIN :			_pos = SetFilePointer( m_hFile, nOffset, 0, FILE_BEGIN);		break;
		case MFile::CURRENT :		_pos = SetFilePointer( m_hFile, nOffset, 0, FILE_CURRENT);		break;
		case MFile::END :			_pos = SetFilePointer( m_hFile, nOffset, 0, FILE_END);			break;
		default :					return 0;
		}

		return _pos;
	}
	else
	{
		LONGLONG _pos;

		switch ( nOrigin)
		{
		case MFile::BEGIN :			_pos = nOffset;							break;
		case MFile::CURRENT :		_pos = m_nFilePos + nOffset;			break;
		case MFile::END :			_pos = m_nFileSize + nOffset;			break;
		default :					return 0;
		}

		if ( _pos > m_nFileSize)	_pos = m_nFileSize;
		else if ( _pos < 0)			_pos = 0;

		m_nFilePos = (DWORD)_pos;
		return m_nFilePos;
	}

	return 0;
}


// Read
bool MFile::Read( void* pBuffer, size_t nBytesToRead, size_t* pnBytesRead)
{
	if ( g_pFileSys == NULL  ||  m_hFile == INVALID_HANDLE_VALUE)
	{
		if ( pnBytesRead != NULL)			*pnBytesRead = 0;
		return false;
	}
	
	if ( nBytesToRead == 0)
	{
		if ( pnBytesRead != NULL)			*pnBytesRead = 0;
		return true;
	}

	if (  pBuffer == NULL)
	{
		if ( pnBytesRead != NULL)			*pnBytesRead = 0;
		return false;
	}


	// Copy data to buffer
	if ( m_bCompressed == false)
	{
		DWORD _bytes_read;
		if ( ::ReadFile( m_hFile, pBuffer, nBytesToRead, &_bytes_read, NULL) == TRUE)
		{
			if ( pnBytesRead != NULL)	*pnBytesRead = _bytes_read;
			return true;
		}
	}
	else
	{
		if ( _patch_file_data( &m_hFile, &g_pFileSys->m_Crypt, &m_pData, m_nFileSize, m_nSectorPos, m_nSectorSize) == true)
		{
			size_t _bytes_read = min( nBytesToRead, m_nFileSize - m_nFilePos);
			memcpy( pBuffer, m_pData + m_nFilePos, _bytes_read);

			m_nFilePos += _bytes_read;
			if ( pnBytesRead != NULL)		*pnBytesRead = _bytes_read;
			return true;
		}
	}

	if ( pnBytesRead != NULL)				*pnBytesRead = 0;
	return false;
}


// ReadAll
bool MFile::ReadAll( void* pBuffer, size_t nBufferSize, size_t* pnBytesRead)
{
	Seek( MFile::BEGIN, 0);

	bool _result = Read( pBuffer, m_nFileSize, pnBytesRead);

	Seek( MFile::BEGIN, 0);

	return _result;
}


// ReadBufferAll
bool MFile::ReadBufferAll( char** pBuffer, size_t* pnBytesRead)
{
	char* _buff = new char[ m_nFileSize];

	if ( ReadAll( _buff, m_nFileSize, pnBytesRead) == true)
	{
		*pBuffer = _buff;
		return true;
	}

	delete [] _buff;
	*pBuffer = NULL;
	return false;
}


// ReadStringAll
bool MFile::ReadStringAll( char** pBuffer, size_t* pnBytesRead)
{
	char* _buff = new char[ m_nFileSize + 1];
	_buff[ m_nFileSize] = 0;

	if ( ReadAll( _buff, m_nFileSize, pnBytesRead) == true)
	{
		*pBuffer = _buff;
		return true;
	}

	delete [] _buff;
	*pBuffer = NULL;
	return false;
}


// ReadStringAll
bool MFile::ReadStringAll( string& strBuffer, size_t* pnBytesRead)
{
	strBuffer.clear();
	strBuffer.resize( m_nFileSize);

	if ( ReadAll( &strBuffer[ 0], m_nFileSize, pnBytesRead) == true)
		return true;

	strBuffer.clear();
	return false;
}














// MFileSystem
MFileSystem::MFileSystem()
{
	m_PathIndex.push_back( "");				// Root path

	m_bCreate = false;
	m_nAccessMode = MFILEACCESS_GENERIC;
	m_bAllowAccUncompFile = true;
	m_bDetectedInvalidFile = false;

	m_Thread.hThread = NULL;
	m_Thread.hEventDestroy = NULL;
	m_Thread.bExit = true;

	m_cbDetectedInvalidFile = NULL;
}


// ~MFileSystem
MFileSystem::~MFileSystem()
{
	Destroy();
}


// Create
bool MFileSystem::Create( const char* szCurrentDirectory, int nAccessMode)
{
	if ( m_bCreate == true  ||  g_pFileSys != NULL)
		return false;

	g_pFileSys = this;


	// Success create file system
	m_bCreate = true;
	m_nAccessMode = nAccessMode;
	m_bAllowAccUncompFile = (nAccessMode == MFILEACCESS_ENCRYPTED)  ?  false : true;
	m_bDetectedInvalidFile = false;
	m_cbDetectedInvalidFile = NULL;


	// Create crypt
	m_Crypt.Create();


	// Set current directory
	::SetCurrentDirectory( szCurrentDirectory);


	// Create index file name
	char _dir[ _MAX_PATH];
	::GetModuleFileName( NULL, _dir, _MAX_PATH);
	m_strIndexFName = string( _dir) + string( ".index");


	// Read cached file index
	if ( m_nAccessMode == MFILEACCESS_GENERIC)
		_read_cached_fileindex( m_strIndexFName.c_str());


	// Create critical section
	::InitializeCriticalSection( &m_csLock);


	// Create thread
/*	if ( m_nAccessMode == MFILEACCESS_ENCRYPTED)
	{
		m_Thread.hEventDestroy = NULL;
		m_Thread.bExit = false;
		m_Thread.hThread = ::CreateThread( NULL, 0, _thread_filesystem, this, 0, NULL);
		::SetThreadPriority( m_Thread.hThread, THREAD_PRIORITY_IDLE);
	}
*/

	// Call event handler
	OnCreate();

	return true;
}


// Destroy
bool MFileSystem::Destroy()
{
	if ( m_bCreate == false)	return false;


	// Call event handler
	OnDestroy();


	// Delete thread
/*	if ( m_Thread.hThread != NULL)
	{
		m_Thread.bExit = true;

		if ( ::WaitForSingleObject( m_Thread.hEventDestroy, 3000) == WAIT_TIMEOUT)
			::TerminateThread( m_Thread.hThread, 0);

		::CloseHandle( m_Thread.hEventDestroy);
		m_Thread.hEventDestroy = NULL;

		::CloseHandle( m_Thread.hThread);
		m_Thread.hThread = NULL;
	}
*/

	// Delete critical section
	:: DeleteCriticalSection( &m_csLock);


	// Write cached file index
	if ( m_nAccessMode == MFILEACCESS_GENERIC)
		_write_cached_fileindex( m_strIndexFName.c_str());


	// Clear variables
	m_bCreate = false;
	m_PathIndex.clear();
	m_CheckFileList.clear();
	m_strIndexFName.clear();
	m_bAllowAccUncompFile = true;
	m_bDetectedInvalidFile = false;
	m_cbDetectedInvalidFile = NULL;

	if ( m_FileIndex.empty() == false)
	{
		for ( FILEINDEX::iterator itr = m_FileIndex.begin();  itr != m_FileIndex.end();  itr++)
			delete (*itr).second;

		m_FileIndex.clear();
	}
	

	g_pFileSys = NULL;

	return true;
}


// GetInstance
MFileSystem* MFileSystem::GetInstance()
{
	return g_pFileSys;
}


// Read file index
bool MFileSystem::ReadFileIndex( const char* szFileName)
{
	if ( m_bCreate == false  ||  m_nAccessMode == MFILEACCESS_GENERIC  ||  szFileName == NULL  ||  szFileName[ 0] == 0)
		return false;


	// File info
	size_t _data_size = 0, _data_enc_size = 0;
	unsigned char *_data = NULL, *_data_enc = NULL;


	// Open file
	HANDLE hFile = ::CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)		goto error_readfileindex;


	// Read data
	DWORD _bytes_read;
	if ( ::ReadFile( hFile, &_data_size, sizeof( size_t), &_bytes_read, NULL) == FALSE  ||  _bytes_read == 0)
		goto error_readfileindex;

	_data_enc_size = ::GetFileSize( hFile, NULL) - sizeof( size_t);
	_data_enc = new unsigned char[ _data_enc_size];
	if ( ::ReadFile( hFile, _data_enc, _data_enc_size, &_bytes_read, NULL) == FALSE  ||  _bytes_read == 0)
		goto error_readfileindex;


	// Decrypt data
	if ( m_Crypt.Decrypt( _data_enc, _data_enc_size) == false)
		goto error_readfileindex;


	// Decoding data
	CLzmaDec _decryptor;
	LzmaDec_Construct( &_decryptor);

	SRes _result;
	_result = LzmaDec_AllocateProbs( &_decryptor, LZMA_PROPS, LZMA_PROPS_SIZE, &g_Alloc);
	if ( _result != SZ_OK)		goto error_readfileindex;

	_data = new unsigned char[ _data_size];
	_decryptor.dic = _data;
	_decryptor.dicBufSize = _data_size;
	LzmaDec_Init( &_decryptor);

	ELzmaStatus _status;
	_result = LzmaDec_DecodeToDic( &_decryptor, _data_size, (Byte*)_data_enc, &_data_enc_size, LZMA_FINISH_END, &_status);
	if ( _result != SZ_OK)		goto error_readfileindex;

	LzmaDec_FreeProbs( &_decryptor, &g_Alloc);


	// Read file index
	DWORD _pos = 0;
	DWORD _headersize = sizeof( MFileHeader);
	while ( _pos < _data_size)
	{
		// Header
		MFileHeader _header;

		memcpy( &_header, _data + _pos, _headersize);

		// Mother file name
		char _motherfname[ _MAX_PATH];
		_motherfname[ _header.nMotherFileNameLen] = 0;
		memcpy( _motherfname, _data + _pos + _headersize, _header.nMotherFileNameLen);

		// File name
		char _filename[ _MAX_PATH];
		_filename[ _header.nFileNameLen] = 0;
		memcpy( _filename, _data + _pos + _headersize + _header.nMotherFileNameLen, _header.nFileNameLen);


		_pos += _headersize + _header.nMotherFileNameLen + _header.nFileNameLen;


		// Get alias name
		char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
#if _MSC_VER < 1400
		_splitpath( _motherfname, _drive, _dir, _fname, _ext);
#else
		_splitpath_s( _motherfname, _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);
#endif
		string _pathfname;
		_pathfname += _drive;
		_pathfname += _dir;
		_pathfname += _fname;
		_pathfname += "/";
		_pathfname += _filename;


		// Create file info
		MFileInfo* _info = new MFileInfo();
		_info->bExist = true;
		_info->strMotherFileName = _motherfname;
		_info->strFileName = _pathfname;
		_info->nCRC = _header.nCRC;
		_info->nFileSize = _header.nFileSize;
		_info->nSectorPos = _header.nSectorPos;
		_info->nSectorSize = _header.nSectorSize;
		m_FileIndex.insert( FILEINDEX::value_type( _pick_filename( _pathfname), _info));
	}


	if ( hFile != INVALID_HANDLE_VALUE)		::CloseHandle( hFile);
	if ( _data != NULL)			delete [] _data;
	if ( _data_enc != NULL)		delete [] _data_enc;

	return true;



error_readfileindex :

	if ( hFile != INVALID_HANDLE_VALUE)		::CloseHandle( hFile);
	if ( _data != NULL)			delete [] _data;
	if ( _data_enc != NULL)		delete [] _data_enc;

	return false;
}


// SetForcedAccessCompFile
bool MFileSystem::SetAllowAccessUncompFile( bool bAllow)
{
	if ( m_bAllowAccUncompFile != bAllow)
		m_bAllowAccUncompFile = bAllow;

	return m_bAllowAccUncompFile;
}


// IsForcedAccessCompFile
bool MFileSystem::IsAllowedAccessUncompFile() const
{
	return m_bAllowAccUncompFile;
}


// _lock
void MFileSystem::_lock()
{
	if ( m_bCreate == false)	return;

	EnterCriticalSection( &m_csLock);
}


// _unlock
void MFileSystem::_unlock()
{
	if ( m_bCreate == false)	return;

	LeaveCriticalSection( &m_csLock);
}


// _read_cached_fileindex
bool MFileSystem::_read_cached_fileindex( const char* szFileName)
{
	bool _result = false;

	HANDLE hFile = ::CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)
		return false;


	size_t _size = ::GetFileSize( hFile, NULL);
	char* _buff = new char[ _size + 1];
	_buff[ _size] = 0;

	DWORD _readnum;
	if ( ::ReadFile( hFile, _buff, _size, &_readnum, NULL) == TRUE)
	{
		char* _token;
#if _MSC_VER < 1400
		_token = strtok( _buff, "\n");
#else
		char* _next_tok;
		_token = strtok_s( _buff, "\n", &_next_tok);
#endif

		while ( _token != NULL)
		{
			// Inset file info
			MFileInfo* pInfo = new MFileInfo();
			pInfo->bExist = true;
			pInfo->strFileName = _token;
			pInfo->nCRC = 0;
			pInfo->nFileSize = 0;
			pInfo->nSectorPos = 0;
			pInfo->nSectorSize = 0;
			m_FileIndex.insert( FILEINDEX::value_type( _pick_filename( _token), pInfo));


#if _MSC_VER < 1400
			_token = strtok( NULL, "\n");
#else
			_token = strtok_s( NULL, "\n", &_next_tok);
#endif
		}

		_result = true;
	}

	if ( _buff != NULL)		delete [] _buff;

	::CloseHandle( hFile);

	return _result;
}


// _write_cached_fileindex
bool MFileSystem::_write_cached_fileindex( const char* szFileName)
{
	HANDLE hFile = ::CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)
		return false;

	for ( FILEINDEX::iterator itr = m_FileIndex.begin();  itr != m_FileIndex.end();  itr++)
	{
		if ( (*itr).second->bExist == false)
			continue;

		string _str;
		_str += (*itr).second->strFileName;
		_str += "\n";

		DWORD _writenum;
		::WriteFile( hFile, _str.c_str(), _str.length(), &_writenum, NULL);
	}

	::CloseHandle( hFile);

	return true;
}

bool MFileSystem::_find_file_and_change_state( const string& _strOutFileName )
{
	// Have 'dir + filename + ext'
	if ( _strOutFileName.find( "/") != string::npos)
	{
		pair< FILEINDEX::iterator, FILEINDEX::iterator> _range = m_FileIndex.equal_range( _strOutFileName);
		for ( FILEINDEX::iterator itr = _range.first;  itr != _range.second;  itr++)
		{
			if ( _stricmp( _strOutFileName.c_str(), (*itr).second->strFileName.c_str()) != 0)
				continue;
			(*itr).second->bExist = true;
			return true;
		}
	}
	else
	{
		FILEINDEX::iterator itr = m_FileIndex.find( _strOutFileName);
		if ( itr != m_FileIndex.end())
		{
			(*itr).second->bExist = true;
			return true;
		}
	}

	return false;
}


// _find_fileinfo
bool MFileSystem::_find_fileinfo( const string& strFileName, const MFileInfo** pInfo)
{
	if ( m_bCreate == false)
	{
		*pInfo = NULL;
		return false;
	}


	// Enter critical section
	_lock();


	// Find file info
	bool bFind = false;
	bool bSearchFile = true;
	string _filename = _pick_filename( strFileName);


	// Have 'dir + filename + ext'
	if ( strFileName.find( "/") != string::npos)
	{
		pair< FILEINDEX::iterator, FILEINDEX::iterator> _range = m_FileIndex.equal_range( _filename);
		for ( FILEINDEX::iterator itr = _range.first;  itr != _range.second;  itr++)
		{
			if ( _stricmp( strFileName.c_str(), (*itr).second->strFileName.c_str()) != 0)
				continue;

			if ( (*itr).second->bExist == true)
			{
				*pInfo = (*itr).second;
				bFind = true;
			}

			bSearchFile = false;
			break;
		}
	}

	// Have only 'filename + ext'
	else
	{
		FILEINDEX::iterator itr = m_FileIndex.find( _filename);
		if ( itr != m_FileIndex.end())
		{
			if ( (*itr).second->bExist == true)
			{
				*pInfo = (*itr).second;
				bFind = true;
			}

			bSearchFile = false;
		}
	}


	// If not exist file index
	if ( bSearchFile == true)
	{
		const MFileInfo* _info;
		if ( _find_file_from_disk( strFileName, &_info) == true)
		{
			*pInfo = _info;
			bFind = true;
		}
	}

	if ( bFind == false)
		*pInfo = NULL;


	// Leave critical section
	_unlock();


	return bFind;
}


// _find_fileinfo_from_disk
bool MFileSystem::_find_fileinfo_from_disk( const string& strFileName, const MFileInfo** pInfo)
{
	if ( m_bCreate == false)
	{
		*pInfo = NULL;
		return false;
	}


	// Enter critical section
	_lock();


	// Find file from disk
	bool bFind = _find_file_from_disk( strFileName, pInfo);


	// Leave critical section
	_unlock();

	return bFind;
}


// _find_file_from_disk
bool MFileSystem::_find_file_from_disk( const string& strFileName, const MFileInfo** pInfo)
{
	if ( m_bCreate == false  ||  m_nAccessMode != MFILEACCESS_GENERIC)
	{
		*pInfo = NULL;
		return false;
	}


	bool bFind = false;
	string _filename = _pick_filename( strFileName);


	// Have 'dir + filename + ext'
	if ( strFileName.find( "/") != string::npos)
	{
		// Modify file index
		pair< FILEINDEX::iterator, FILEINDEX::iterator> _range = m_FileIndex.equal_range( _filename);
		for ( FILEINDEX::iterator itr = _range.first;  itr != _range.second;  )
		{
			if ( (*itr).second->bExist == false  ||  (*itr).second->strFileName == strFileName)
			{
				delete (*itr).second;
				itr = m_FileIndex.erase( itr);
			}
			else
				itr++;
		}

		
		// Find file
		if ( _access( strFileName.c_str(), 0) != -1)
		{
			// Add file index
			MFileInfo* _info = new MFileInfo();
			_info->bExist = true;
			_info->strFileName = strFileName;
			_info->nCRC = 0;
			_info->nFileSize = 0;
			_info->nSectorPos = 0;
			_info->nSectorSize = 0;
			m_FileIndex.insert( FILEINDEX::value_type( _filename, _info));

			*pInfo = _info;
			bFind = true;
		}
	}

	// Have only 'filename + ext'
	else
	{
		// Modify file index
		pair< FILEINDEX::iterator, FILEINDEX::iterator> _range = m_FileIndex.equal_range( _filename);
		for ( FILEINDEX::iterator itr = _range.first;  itr != _range.second;  )
		{
			if ( (*itr).second->bExist == false)
			{
				delete (*itr).second;
				itr = m_FileIndex.erase( itr);
			}
			else
				itr++;
		}


		// Find file
		for ( PATHINDEX::iterator itr = m_PathIndex.begin();  itr != m_PathIndex.end();  itr++)
		{
			string _pathfilename = (*itr) + strFileName;
			if ( _access( _pathfilename.c_str(), 0) != -1)
			{
				// Add file index
				MFileInfo* _info = new MFileInfo();
				_info->bExist = true;
				_info->strFileName = _pathfilename;
				_info->nCRC = 0;
				_info->nFileSize = 0;
				_info->nSectorPos = 0;
				_info->nSectorSize = 0;
				m_FileIndex.insert( FILEINDEX::value_type( _filename, _info));

				*pInfo = _info;
				bFind = true;

				break;
			}
		}
	}

	
	// Not exist file
	if ( bFind == false)
	{
		MFileInfo* _info = new MFileInfo();
		_info->bExist = false;
		_info->strFileName = strFileName;
		_info->nCRC = 0;
		_info->nFileSize = 0;
		_info->nSectorPos = 0;
		_info->nSectorSize = 0;
		m_FileIndex.insert( FILEINDEX::value_type( _filename, _info));

		*pInfo = NULL;
	}

	return bFind;
}


// GetFileIndex
const FILEINDEX* MFileSystem::GetFileIndex()
{
	return &m_FileIndex;
}


// SetPath
void MFileSystem::SetPath( const char* szPath, bool bIncludeSubDir)
{
	m_PathIndex.clear();
	m_PathIndex.push_back( "");				// Root path

	AddPath( szPath, bIncludeSubDir);
}


// AddPath
void MFileSystem::AddPath( const char* szPath, bool bIncludeSubDir)
{
	if ( m_bCreate == false  ||  m_nAccessMode != MFILEACCESS_GENERIC)
		return;


	char* _path = _strdup( szPath);
	char* _token;
#if _MSC_VER < 1400
	_token = strtok( _path, ";,");
#else
	char* _next_tok;
	_token = strtok_s( _path, ";,", &_next_tok);
#endif

	while ( _token != NULL)
	{
		string strPath = _trim_filename( _token);
		if ( strPath.empty() == false)
			strPath += '/';

		m_PathIndex.push_back( strPath);


		if ( bIncludeSubDir == true)
			_get_path( strPath, &m_PathIndex);


#if _MSC_VER < 1400
		_token = strtok( NULL, ";,");
#else
		_token = strtok_s( NULL, ";,", &_next_tok);
#endif
	}

	free( _path);
}


// IsExistFile
bool MFileSystem::IsExistFile( const char* szFileName)
{
	if ( m_bCreate == false)	return false;


	// Enter critical section
	_lock();


	bool bExist = false;
	string _trimname = _trim_filename( szFileName);
	string _filename = _pick_filename( _trimname);

	switch ( m_nAccessMode)
	{
	case MFILEACCESS_GENERIC :
		{
			FILEINDEX::iterator itr = m_FileIndex.find( _filename);
			if ( itr != m_FileIndex.end())
			{
				MFileInfo* pInfo = (*itr).second;
				if ( pInfo->bExist == true  &&  _access( pInfo->strFileName.c_str(), 0) != -1)
					bExist = true;

				break;
			}

			const MFileInfo* pInfo;
			bExist = _find_file_from_disk( _trimname, &pInfo);

			break;
		}

	case MFILEACCESS_ENCRYPTED :
		{
			FILEINDEX::iterator itr = m_FileIndex.find( _filename);
			if ( itr != m_FileIndex.end())
				bExist = true;

			break;
		}
	}


	// Leave critical section
	_unlock();

	return bExist;
}


// GetAccessMode
int MFileSystem::GetAccessMode() const
{
	return m_nAccessMode;
}



// _thread
unsigned long WINAPI MFileSystem::_thread_filesystem( LPVOID lpParam)
{
	MFileSystem* pFileSystem = (MFileSystem*)lpParam;

	// Call event handler
	pFileSystem->_on_create_thread();


	// Run thread
	while ( pFileSystem->_is_exit_thread() == false)
	{
		if ( pFileSystem->_on_run_thread() == false)
			break;


		// Sleep to relax CPU usage
		Sleep( 10);
	}


	// Call event handler
	pFileSystem->_on_destroy_thread();

	return 0;
}


// _is_exit_thread
bool MFileSystem::_is_exit_thread() const
{
	return m_Thread.bExit;
}


// _on_create_thread
void MFileSystem::_on_create_thread()
{
	m_Thread.bExit = false;
	m_Thread.hEventDestroy = CreateEvent( NULL, FALSE, FALSE, NULL);
}


// _on_destroy_thread
void MFileSystem::_on_destroy_thread()
{
	SetEvent( m_Thread.hEventDestroy);
}


// _on_run_thread
bool MFileSystem::_on_run_thread()
{
	// Enter critical section
	_lock();

	
	// Get file info
	const MFileInfo* pInfo = NULL;
	if ( m_CheckFileList.empty() == false)
	{
		pInfo = *m_CheckFileList.begin();

		m_CheckFileList.pop_front();
	}


	// Leave critical section
	_unlock();


	// Get CRC
	if ( pInfo != NULL  &&  pInfo->bExist == true)
	{
		HANDLE hFile = ::CreateFile( pInfo->strMotherFileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if ( hFile != INVALID_HANDLE_VALUE)
		{
			unsigned char* pData = NULL;
			if ( _patch_file_data( &hFile, &m_Crypt, &pData, pInfo->nFileSize, pInfo->nSectorPos, pInfo->nSectorSize) == true)
			{
				unsigned short _crc = m_Crypt.GetCRC( pData, pInfo->nFileSize);
				if ( _crc != pInfo->nCRC  &&  m_bDetectedInvalidFile == false)
				{
					m_bDetectedInvalidFile = true;
					OnDetectedInvalidFile();
				}

				delete [] pData;
			}

			::CloseHandle( hFile);
		}
	}

	return true;
}


// SetDetectInvalidFileCallback
void MFileSystem::SetDetectInvalidFileCallback( MCALLBACK_DETECTEDINVALIDFILE _callback)
{
	m_cbDetectedInvalidFile = _callback;
}


// OnCreate
void MFileSystem::OnCreate()
{
}


// OnDestroy
void MFileSystem::OnDestroy()
{
}


// OnOpenFile
void MFileSystem::OnOpenFile( const char* szFileName, const MFileInfo* pInfo)
{
/*	if ( pInfo != NULL  &&  m_nAccessMode == MFILEACCESS_ENCRYPTED)
	{
		_lock();

		m_CheckFileList.push_back( pInfo);

		_unlock();
	}
*/
}


// OnCloseFile
void MFileSystem::OnCloseFile( const char* szFileName)
{
}


// OnDetectedInvalidFile
void MFileSystem::OnDetectedInvalidFile()
{
	if ( m_cbDetectedInvalidFile != NULL)
		m_cbDetectedInvalidFile();
}


// Get file system instance
MFileSystem* MGetFileSystem()
{
	return MFileSystem::GetInstance();
}


// MCreateFileSystem
MFileSystem* MCreateFileSystem( const char* szCurDirectory, int nAccessMode, const char* szPath)
{
	static MFileSystem _filesystem;
	if ( _filesystem.Create( szCurDirectory, nAccessMode) == false)
		return NULL;

	if ( szPath != NULL  &&  szPath[ 0] != NULL)
		_filesystem.SetPath( szPath);
	else
		_filesystem.SetPath( "./");

	return &_filesystem;
}

MFileSystem* MCreateFileSystem( int nAccessMode, const char* szPath)
{
	char _dir[ _MAX_PATH];
	::GetModuleFileName( NULL, _dir, _MAX_PATH);
	PathRemoveFileSpec( _dir);

	return MCreateFileSystem( _dir, nAccessMode, szPath);
}
















// MGetPureFileNameExt
string MGetPureFileNameExt( const string& szFileName)
{
	char drive[ _MAX_DRIVE], dir[ _MAX_DIR], fname[ _MAX_FNAME], ext[ _MAX_EXT];
#if _MSC_VER < 1400
	_splitpath( szFileName.c_str(), drive, dir, fname, ext);
#else
	_splitpath_s( szFileName.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
#endif

	string _result;
	_result += fname;
	_result += ext;
	return _result;
}


// MGetPureFileName
string MGetPureFileName( const string& szFileName)
{
	char drive[ _MAX_DRIVE], dir[ _MAX_DIR], fname[ _MAX_FNAME], ext[ _MAX_EXT];
#if _MSC_VER < 1400
	_splitpath( szFileName.c_str(), drive, dir, fname, ext);
#else
	_splitpath_s( szFileName.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
#endif

	string _result;
	_result = fname;
	return _result;
}


// MGetPureExtension
string MGetPureExtension( const string& szFileName)
{
	char drive[ _MAX_DRIVE], dir[ _MAX_DIR], fname[ _MAX_FNAME], ext[ _MAX_EXT];
#if _MSC_VER < 1400
	_splitpath( szFileName.c_str(), drive, dir, fname, ext);
#else
	_splitpath_s( szFileName.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
#endif

	string _result;
	_result = ext;
	return _result;
}


// MGetRemovePathName
string MGetRemovePathName( const string& strPathName)
{
	string::size_type _pos = strPathName.find_last_of( "/\\");
	if ( _pos != string::npos)		return strPathName.substr( _pos + 1);
	return strPathName;
}


// MGetRemovePathName
string MGetRemoveRelativeName( string& strPathName)
{
	string szSearchFor("Data/Model/");
	string replacestr("");
	size_t found = strPathName.find( szSearchFor);
	if(found != string::npos)
	{
	strPathName.replace(found, szSearchFor.length(), replacestr);
	}
	return strPathName;
}


// MGetPathReplacedBackSlashToSlash
string MGetPathReplacedBackSlashToSlash( const string& strPath)
{
	string _result = strPath;
	replace( _result.begin(), _result.end(), '\\', '/');

	return _result;
}


// MGetPurePath
string MGetPurePath( const string& strPath)
{
	char drive[ _MAX_DRIVE], dir[ _MAX_DIR], fname[ _MAX_FNAME], ext[ _MAX_EXT];
#if _MSC_VER < 1400
	_splitpath( strPath.c_str(), drive, dir, fname, ext);
#else
	_splitpath_s( strPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
#endif

	string _path;
	_path += drive;
	_path += dir;
	replace( _path.begin(), _path.end(), '\\', '/');

	if ( _path.empty() == false  &&  _path.at( _path.length() - 1) != '/')
		_path += "/";

	return _path;
}


// MGetPathFileNameFromRelativePath
string MGetPathFileNameFromRelativePath( const string& strBasePath, const string& strRelative)
{
	string _base_path;
	{
		char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
#if _MSC_VER < 1400
		_splitpath( strBasePath.c_str(), _drive, _dir, _fname, _ext);
#else
		_splitpath_s( strBasePath.c_str(), _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);
#endif
		_base_path += _drive;
		_base_path += _dir;
		replace( _base_path.begin(), _base_path.end(), '\\', '/');

		string::size_type n;
		n = _base_path.find_last_not_of( "/");
		if ( n != string::npos)  _base_path = _base_path.substr( 0, n + 1);
	}

	string _rel_path;
	string _filename;
	{
		char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
#if _MSC_VER < 1400
		_splitpath( strRelative.c_str(), _drive, _dir, _fname, _ext);
#else
		_splitpath_s( strRelative.c_str(), _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);
#endif
		_rel_path += _drive;
		_rel_path += _dir;
		replace( _rel_path.begin(), _rel_path.end(), '\\', '/');

		_filename += _fname;
		_filename += _ext;
	}


	string _path = _base_path;
	bool _result = true;
	while ( _rel_path.compare( 0, 3, "../") == 0)
	{
		string::size_type n;
		n = _path.find_last_of( "/");
		if ( n == string::npos)
		{
			_path = _base_path;
			if ( _path.empty() == false  &&  _path.at( _path.length() - 1) != '/')
				_path += "/";

			_path +=_filename;
			return _path;
		}

		_path = _path.substr( 0, n);
		_rel_path.erase( 0, 3);
	}

	if ( _path.empty() == false)	_path += string( "/");
	_path += _rel_path;

	if ( _path.empty() == false  &&  _path.at( _path.length() - 1) != '/')
		_path += "/";

	_path +=_filename;
	return _path;
}


// MGetRelativePathFileName
int _get_dir_depth(const char *pDir)
{
	int nDepth = 0;
	int nDirLen = strlen(pDir);
	for(int i=0; i<nDirLen; i++){
		if(pDir[i]=='\\') nDepth++;
	}
	if(pDir[0]=='\\') nDepth--;
	if(pDir[nDirLen-1]=='\\') nDepth--;
	return (nDepth+1);
}

int _get_dir_as_depth(char *pDepthDir, const char *pDir, int nDepth)
{
	int nStartDir = 0;
	int nEndDir = 0;
	int nDepthCount = 0;

	int nCount = 0;
	if(pDir[0]=='\\') nCount++;
	int nDepthChecked = FALSE;
	for(int i=nCount; i<(int)strlen(pDir); i++){
		if(pDir[i]=='\\'){
			nDepthCount++;
		}
		if(nDepth==nDepthCount && nDepthChecked==FALSE){
			nStartDir = i;
			nDepthChecked = TRUE;
		}
		if(nDepth+1==nDepthCount){
			nEndDir = i-1;
			break;
		}
	}

	memcpy(pDepthDir, pDir+nStartDir, nEndDir-nStartDir+1);
	pDepthDir[nEndDir-nStartDir+1] = 0;

	return nStartDir;
}

int _get_len_as_depth(char *pDir, int nDepth)
{
	int nDepthCount = 0;

	int nCount = 0;
	if(pDir[0]=='\\') nCount++;

	if(nDepth==0) return nCount;

	for(int i=nCount; i<(int)strlen(pDir); i++){
		if(pDir[i]=='\\'){
			nDepthCount++;
		}
		if(nDepth==nDepthCount){
			return i+1;
		}
	}

	return -1;
}

string MGetRelativePathFileName( const string& strBasePath, const string& strFileName)
{
	string _path;

	char szBasePath[256], szPath[256];
	_fullpath(szBasePath, strBasePath.c_str(), 256);
	_fullpath(szPath, strFileName.c_str(), 256);

	_strlwr(szPath);
	_strlwr(szBasePath);

	if(szBasePath[0]!=szPath[0])
	{
		_path = szPath;
		replace( _path.begin(), _path.end(), '\\', '/');
		return _path;
	}

	char szBaseDir[256], szBaseDrive[4], szBaseFileName[256], szBaseExt[16];
	char szDir[256], szDrive[4], szFileName[256], szExt[16];
	_splitpath(szBasePath, szBaseDrive, szBaseDir, szBaseFileName, szBaseExt);
	_splitpath(szPath, szDrive, szDir, szFileName, szExt);

	char szBaseDepthDir[256];
	char szDepthDir[256];
	int nEqualDepth = 0;
	int nEqualLen = 0;

	int nBaseDepth = _get_dir_depth(szBaseDir);
	int nDepth = _get_dir_depth(szDir);
	//nEqualDepth = min(nBaseDepth, nDepth);
	for( int i=0; i<min(nBaseDepth, nDepth); i++){
		_get_dir_as_depth(szBaseDepthDir, szBaseDir, i);
		_get_dir_as_depth(szDepthDir, szDir, i);
		if(strcmp(szDepthDir, szBaseDepthDir)==0){
			nEqualDepth++;
		}
		else{
			break;
		}
	}

	nEqualLen = _get_len_as_depth(szDir, nEqualDepth);

	// Depth의 차이만큼 "../"넣기
	char szTemp[256]={0,};
	char *pUp = "..\\";
	int nNextPos = 0;
	for( int i=0; i<nBaseDepth-nEqualDepth; i++)
	{
		memcpy(szTemp+nNextPos, pUp, 4);
		nNextPos +=3;
	}
	memcpy(szTemp+nNextPos, szDir+nEqualLen, strlen(szDir)-nEqualLen);

	_path.clear();
	_path += szTemp;
	_path += szFileName;
	_path += szExt;
	replace( _path.begin(), _path.end(), '\\', '/');

	return _path;
}


// MGetCurrentDirectoryW
wstring MGetCurrentDirectoryW()
{
	wchar_t szBuffer[ _MAX_PATH];
	GetCurrentDirectoryW( sizeof( szBuffer), szBuffer);

	wstring _result;
	_result += szBuffer;
	_result += L'\\';
	return _result;
}


// MGetCurrentDirectoryA
string MGetCurrentDirectoryA()
{
	char szBuffer[ _MAX_PATH];
	GetCurrentDirectoryA( sizeof( szBuffer), szBuffer);

	string _result;
	_result += szBuffer;
	_result += '\\';
	return _result;
}


// MIsExistFileW
bool MIsExistFileW( const wchar_t* szFileName)
{
	string strFileName = MLocale::ConvUTF16ToAnsi(szFileName);
	return MIsExistFileA( strFileName.c_str());
}


// MIsExistFileA
bool MIsExistFileA( const char* szFileName)
{
	if ( g_pFileSys == NULL)
		return  ( _access( szFileName, 0) != -1)  ?  true : false;

	else
		return g_pFileSys->IsExistFile( szFileName);

	return false;
}
