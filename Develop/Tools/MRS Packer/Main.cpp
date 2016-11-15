#include <stdio.h>
#include <io.h>
#include <vector>
#include <string.h>
#include <algorithm>
#include "LzmaEnc.h"
#include "LzmaDec.h"
#include "KeyTable.h"
using namespace std;


// Memory allocator
static void *SzBigAlloc( void*, size_t size)	{ return VirtualAlloc( 0, size, MEM_COMMIT, PAGE_READWRITE);	}
static void SzBigFree( void*, void* address)	{ VirtualFree( address, 0, MEM_RELEASE);						}
static ISzAlloc g_BigAlloc = { SzBigAlloc, SzBigFree };

static void* SzAlloc( void*, size_t size)		{ return malloc( size);		}
static void SzFree( void*, void* address)		{ free( address);			}
static ISzAlloc g_Alloc = { SzAlloc, SzFree };


// Default properties of LZMA
static const unsigned char LZMA_PROPS[] = { 0x5D, 0x00, 0x00, 0x01, 0x00 };


// MFileHeader
struct MFileHeader
{
	unsigned char			padding[64];
	unsigned long			nFileSize;
	unsigned long			nSectorPos;
	unsigned long			nSectorSize;
	unsigned long			nMotherFileNameLen;
	unsigned long			nFileNameLen;
	unsigned short			nCRC;
};


// Encrypt
bool Encrypt( void* pBuff, DWORD nBuffLen)
{
	unsigned char* _buff = (unsigned char*)pBuff;
	_buff[ 0] ^= ( _buff[ nBuffLen - 1] >> 1) ^ g_KeyTable[ 0];
	for ( DWORD i = 1;  i < nBuffLen;  i++)
		_buff[ i] ^= ( _buff[ i - 1] >> 1) ^ g_KeyTable[ i & 0x0000FFFF];

	return true;
}


// Decrypt
bool Decrypt( void* pBuff, DWORD nBuffLen)
{
	unsigned char* _buff = (unsigned char*)pBuff;
	for ( DWORD i = (nBuffLen - 1);  i > 0;  i--)
		_buff[ i] ^= ( _buff[ i - 1] >> 1) ^ g_KeyTable[ i & 0x0000FFFF];
	_buff[ 0] ^= ( _buff[ nBuffLen - 1] >> 1) ^ g_KeyTable[ 0];

	return true;
}


// GetCRC
unsigned short GetCRC( void* pBuff, DWORD nBuffLen)
{
	unsigned char* _buff = (unsigned char*)pBuff;
	unsigned short _crc = 0x3F75;
	for ( DWORD i = 0;  i < nBuffLen;  i++)
		_crc = (_crc << 1) + g_KeyTable[ _buff[ i] & 0x0000FFFF];

	return _crc;
}


// _get_files
void _get_files( const char* _path, vector< string>* _list, bool _include_subdir, bool _filtering_fname, const char* _filter_fname, bool _filtering_ext, const char* _filter_ext)
{
	string _find;
	_find += _path;
	_find += "*.*";

	WIN32_FIND_DATA _find_data;
	HANDLE hFind = FindFirstFile( _find.c_str(), &_find_data);
	if ( hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if ( _find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if ( _include_subdir == false  ||  _find_data.cFileName[ 0] == '.')
					continue;

				string _pathfname;
				_pathfname += _path;
				_pathfname += _find_data.cFileName;
				_pathfname += "/";
				_get_files( _pathfname.c_str(), _list, _include_subdir, _filtering_fname, _filter_fname, _filtering_ext, _filter_ext);
			}
			else
			{
				char _filename[ _MAX_FNAME], _ext[ _MAX_EXT];
				_splitpath_s( _find_data.cFileName, NULL, 0, NULL, 0, _filename, _MAX_FNAME, _ext, _MAX_EXT);

				if ( _filtering_fname == true)
				{
					if ( _strnicmp( _filename, _filter_fname, strlen( _filter_fname)) != 0)
						continue;
				}
				else
				{
					if ( _stricmp( _filename, _filter_fname) != 0)
						continue;
				}

				if ( _filtering_ext == true)
				{
					if ( _strnicmp( _ext, _filter_ext, strlen( _filter_ext)) != 0)
						continue;
				}
				else
				{
					if ( _stricmp( _ext, _filter_ext) != 0)
						continue;
				}


				string _pathfname;
				_pathfname += _path;
				_pathfname += _find_data.cFileName;
				_list->push_back( _pathfname);
			}

		}	while( FindNextFile( hFind, &_find_data) != 0);

		FindClose( hFind);
	}
}


// EncodingStr
SRes EncodingStr( const void* _instream, size_t _insize, void* _outstream, size_t* _outsize)
{
	if ( _instream == NULL  ||  _outstream == NULL)
		return SZ_ERROR_DATA;


	// Encoding
	SRes _result;
	CLzmaEncHandle* _encoder = (CLzmaEncHandle*)LzmaEnc_Create( &g_Alloc);
	if ( _encoder != NULL)
	{
		// Set encoding properties
		CLzmaEncProps _props;
		LzmaEncProps_Init( &_props);
		_props.dictSize = 1 << 16;			// 64 KB
		_props.writeEndMark = 1;
		
		_result = LzmaEnc_SetProps( _encoder, &_props);
		if ( _result == SZ_OK)
		{
			_result = LzmaEnc_MemEncode( _encoder,
										(Byte*)_outstream, _outsize,
										(Byte*)_instream, _insize,
										_props.writeEndMark, NULL, &g_Alloc, &g_BigAlloc);
		}

		LzmaEnc_Destroy( _encoder, &g_Alloc, &g_BigAlloc);
	}
	else
		_result = SZ_ERROR_MEM;


	if ( _result != SZ_OK)
		*_outsize = 0;

	return _result;
}


// EncodingFile
SRes EncodingFile( const char* szDestFileName, const char* szIndexFileName, const char* szSrcFileName, const char* szAliasFileName, size_t nMaxSize =0)
{
	if ( szDestFileName == NULL   ||  szDestFileName[ 0] == 0   ||
		 szSrcFileName == NULL    ||  szSrcFileName[ 0] == 0    ||
		 szAliasFileName == NULL  ||  szAliasFileName[ 0] == 0)
		return SZ_ERROR_DATA;

	SRes nReturn = SZ_OK;

	// Convert maximum file size(to bytes from MB)
	if ( nMaxSize > 0)		nMaxSize *= 1048576;


	// Read file
	unsigned char* _instream = NULL;
	unsigned char* _outstream = NULL;

	HANDLE hFile = ::CreateFile( szSrcFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)		{ nReturn = SZ_ERROR_READ; goto error_encoding; }

	size_t _insize = ::GetFileSize( hFile, NULL);
	_instream = new unsigned char[ _insize];

	DWORD _readnum;
	if ( ::ReadFile( hFile, _instream, _insize, &_readnum, NULL) == FALSE)
	{
		nReturn = SZ_ERROR_READ;
		goto error_encoding;
	}

	::CloseHandle( hFile);


	// Get file CRC
	unsigned short _crc = GetCRC( _instream, _insize);


	// Set output buffer
	size_t _outsize = _insize + max( 1024, ( _insize >> 2));
	_outstream = new unsigned char[ _outsize];
	memset( _outstream, 0x00, _outsize);


	// Encoding data
	SRes result = EncodingStr( _instream, _insize, _outstream, &_outsize);
	if ( result != SZ_OK)		{ nReturn = result; goto error_encoding; }


	// Encrypt data
	Encrypt( _outstream, _outsize);



	// Create file header
	MFileHeader _header;
	_header.nFileSize = _insize;
	_header.nSectorPos = 0;
	_header.nSectorSize = _outsize;
	_header.nMotherFileNameLen = 0;
	_header.nFileNameLen = strlen( szAliasFileName);
	_header.nCRC = _crc;



	// Get mother file name
	char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
	_splitpath_s( szDestFileName, _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);

	char _motherfname[ _MAX_PATH];
	for ( int i = 0;  i < 1000;  i++)
	{
		if ( i == 0)		sprintf_s( _motherfname, "%s%s%s%s",  _drive, _dir, _fname, _ext);
		else				sprintf_s( _motherfname, "%s%s%s.%03d",  _drive, _dir, _fname, i);


		hFile = ::CreateFile( _motherfname, FILE_APPEND_DATA, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if ( hFile == INVALID_HANDLE_VALUE)		{ nReturn = SZ_ERROR_WRITE; goto error_encoding; }

		_header.nSectorPos = ::GetFileSize( hFile, NULL);
		if ( nMaxSize == 0  ||  _header.nSectorPos < nMaxSize)
			break;

		CloseHandle( hFile);
	}


	// Write data
	DWORD _writenum;
	if ( ::WriteFile( hFile, _outstream, _outsize, &_writenum, NULL) == FALSE)
	{
		nReturn = SZ_ERROR_WRITE;
		goto error_encoding;
	}

	::CloseHandle( hFile);


	// Get mother file name
	_splitpath_s( _motherfname, NULL, 0, NULL, 0, _fname, _MAX_FNAME, _ext, _MAX_EXT);
	sprintf_s( _motherfname, "%s%s", _fname, _ext);
	_header.nMotherFileNameLen = strlen( _motherfname);


	// Write header info
	hFile = ::CreateFile( szIndexFileName, FILE_APPEND_DATA, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)
	{
		nReturn = SZ_ERROR_WRITE;
		goto error_encoding;
	}

	::WriteFile( hFile, &_header, sizeof( MFileHeader), &_writenum, NULL);
	::WriteFile( hFile, _motherfname, _header.nMotherFileNameLen, &_writenum, NULL);
	::WriteFile( hFile, szAliasFileName, _header.nFileNameLen, &_writenum, NULL);
	::CloseHandle( hFile);


error_encoding:

	// Release memory
	if ( _instream != NULL)		delete [] _instream;
	if ( _outstream != NULL)	delete [] _outstream;
	return nReturn;
}


// EncodingDir
SRes EncodingDir( const char* szDestFileName, const char* szSrcFileName, const char* szDestPath, size_t nMaxSize =0)
{
	if ( szDestFileName == NULL  ||  szDestFileName[ 0] == 0  ||
		szSrcFileName == NULL   ||  szSrcFileName[ 0] == 0)
		return SZ_ERROR_DATA;


	char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];


	// Get index file name
	string _index;
	_index += szDestFileName;
	_index += ".index";


	// Get filtering info
	_splitpath_s( szSrcFileName, _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);
	bool bFilterFName = false, bFilterExt = false;
	char szFilterFName[ _MAX_FNAME], szFilterExt[ _MAX_EXT];
	strcpy_s( szFilterFName, _fname);
	strcpy_s( szFilterExt, _ext);

	char* _ch;
	_ch = strchr( _fname, '*');
	if ( _ch != NULL)
	{
		bFilterFName = true;
		memset( szFilterFName, 0, _MAX_FNAME);
		strncpy_s( szFilterFName, _fname, (int)(_ch - _fname));
	}

	_ch = strchr( _ext, '*');
	if ( _ch != NULL)
	{
		bFilterExt = true;
		memset( szFilterExt, 0, _MAX_EXT);
		strncpy_s( szFilterExt, _ext, (int)(_ch - _ext));
	}


	// Get start path
	string strPath;
	strPath += _drive;
	strPath += _dir;
	replace( strPath.begin(), strPath.end(), '\\', '/');


	// Get file list
	vector< string> vFileList;
	_get_files( strPath.c_str(), &vFileList, false, bFilterFName, szFilterFName, bFilterExt, szFilterExt);


	// Get target path
	string _target_path;
	if ( szDestPath != NULL  &&  szDestPath[ 0] != 0)
	{
		_target_path = szDestPath;
		replace( _target_path.begin(), _target_path.end(), '\\', '/');

		// Trim left and right
		string::size_type n;
		n = _target_path.find_first_not_of( " ./");
		if ( n != string::npos)  _target_path = _target_path.substr( n, _target_path.length());
		n = _target_path.find_last_not_of( " /");
		if ( n != string::npos)  _target_path = _target_path.substr( 0, n + 1);

		_target_path += "/";
	}


	// Encoding files
	for ( vector< string>::iterator itr = vFileList.begin();  itr != vFileList.end();  itr++)
	{
		_splitpath_s( (*itr).c_str(), _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);

		string _aliasname;
		_aliasname += _target_path;
		_aliasname += _fname;
		_aliasname += _ext;


		printf_s( "Archive : %s", (*itr).c_str());

		SRes _result;
		if ( (_result = EncodingFile( szDestFileName, _index.c_str(), (*itr).c_str(), _aliasname.c_str(), nMaxSize)) != SZ_OK)
			printf_s( " - error %d", _result);

		printf_s( "\n");
	}

	return SZ_OK;
}




// Decoding
SRes DecodingStr( const void* _instream, size_t _insize, void* _outstream, size_t* _outsize)
{
	// Decoding
	SizeT _dstlen = *_outsize;
	SizeT _srclen = _insize;
	ELzmaStatus _status;
	SRes _result = LzmaDecode( (Byte*)_outstream, &_dstlen, (Byte*)_instream, &_srclen, LZMA_PROPS, LZMA_PROPS_SIZE, LZMA_FINISH_END, &_status, &g_Alloc);

	if ( _result != SZ_OK)
	{
		delete [] _outstream;
		*_outsize = 0;
	}

	*_outsize = _dstlen;

	return _result;
}


// DecodingDir
SRes DecodingDir( const char* szSrcFileName, const char* szDestPath)
{
	if ( szSrcFileName == NULL   ||  szSrcFileName[ 0] == 0)
		return SZ_ERROR_DATA;


	// Get destination path
	string strDestPath;
	if ( szDestPath != NULL  &&  szDestPath[ 0] != 0)
	{
		strDestPath = szDestPath;
		replace( strDestPath.begin(), strDestPath.end(), '\\', '/');

		// Trim left and right
		string::size_type n;
		n = strDestPath.find_first_not_of( " ./");
		if ( n != string::npos)  strDestPath = strDestPath.substr( n, strDestPath.length());
		n = strDestPath.find_last_not_of( " /");
		if ( n != string::npos)  strDestPath = strDestPath.substr( 0, n + 1);
	}


	// Decoding files
	HANDLE hFile = ::CreateFile( szSrcFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)		return SZ_ERROR_DATA;

	while ( true)
	{
		// Read file header
		MFileHeader _header;
		DWORD _bytesread;
		if ( ::ReadFile( hFile, &_header, sizeof( MFileHeader), &_bytesread, NULL) == FALSE  ||  _bytesread == 0)
			break;


		// Decrypt header
		if ( Decrypt( &_header, sizeof( MFileHeader)) == false)
			break;

		
		// Read file name
		char _aliasname[ _MAX_PATH];
		memset( _aliasname, 0, _MAX_PATH);
		if ( ::ReadFile( hFile, _aliasname, _header.nFileNameLen, &_bytesread, NULL) == FALSE  ||  _bytesread == 0)
			break;

		
		// Decrypt file name
		if ( Decrypt( _aliasname, _header.nFileNameLen) == false)
			break;


		// Read data
		size_t _sector_size = _header.nSectorSize;
		unsigned char* _comp_data = new unsigned char[ _sector_size];
		if ( ::ReadFile( hFile, _comp_data, _sector_size, &_bytesread, NULL) == FALSE  ||  _bytesread == 0)
		{
			delete [] _comp_data;
			break;
		}


		// Decrypt data
		if ( Decrypt( _comp_data, _sector_size) == false)
		{
			delete [] _comp_data;
			break;
		}
		

		// Decoding data
		size_t _out_size = _header.nFileSize;
		unsigned char* _data = new unsigned char[ _out_size];
		SRes _result = DecodingStr( _comp_data, _sector_size, _data, &_out_size);
		if ( _result != SZ_OK)
		{
			delete [] _comp_data;
			delete [] _data;
			break;
		}


		// Create path
		string _filename;
		if ( strDestPath.empty() == false)
		{
			_filename += strDestPath;
			_filename += "/";
			_filename += _aliasname;
		}
		else
		{
			_filename = _aliasname;
		}

		string::size_type _p;
		_p = _filename.find_last_of( "/");
		if ( _p != string::npos)
		{
			char* _path = _strdup( _filename.substr( 0, _p).c_str());

			string _dir;
			char *_token, *_next_tok;
			_token = strtok_s( _path, "/", &_next_tok);
			while ( _token != NULL)
			{
				_dir += _token;
				_dir += "/";
				if ( _access( _dir.c_str(), 0 ) == -1)
					CreateDirectory( _dir.c_str(), NULL);

				_token = strtok_s( NULL, "/", &_next_tok);
			}

			free( _path);
		}

		
		// Write data
		HANDLE hWriteFile = ::CreateFile( _filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if ( hWriteFile != INVALID_HANDLE_VALUE)
		{
			DWORD _writenum;
			::WriteFile( hWriteFile, _data, _out_size, &_writenum, NULL);
			::CloseHandle( hWriteFile);
		}

		delete [] _comp_data;
		delete [] _data;
	}

	::CloseHandle( hFile);

	return SZ_OK;
}


// MergeIndex
bool MergeIndex( const char* szDestFileName, const char* szSrcFileName)
{
	if ( szDestFileName == NULL  ||  szDestFileName[ 0] == 0  ||
		szSrcFileName == NULL   ||  szSrcFileName[ 0] == 0)
		return SZ_ERROR_DATA;


	char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
	_splitpath_s( szSrcFileName, _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);


	// Get base path
	string strPath;
	strPath += _drive;
	strPath += _dir;
	replace( strPath.begin(), strPath.end(), '\\', '/');


	// Get filtering info
	bool bFilterFName = false, bFilterExt = false;
	char szFilterFName[ _MAX_FNAME], szFilterExt[ _MAX_EXT];
	strcpy_s( szFilterFName, _fname);
	strcpy_s( szFilterExt, _ext);

	char* _ch;
	_ch = strchr( _fname, '*');
	if ( _ch != NULL)
	{
		bFilterFName = true;
		memset( szFilterFName, 0, _MAX_FNAME);
		strncpy_s( szFilterFName, _fname, (int)(_ch - _fname));
	}

	_ch = strchr( _ext, '*');
	if ( _ch != NULL)
	{
		bFilterExt = true;
		memset( szFilterExt, 0, _MAX_EXT);
		strncpy_s( szFilterExt, _ext, (int)(_ch - _ext));
	}


	// Get file list
	vector< string> vFileList;
	_get_files( strPath.c_str(), &vFileList, true, bFilterFName, szFilterFName, bFilterExt, szFilterExt);


	// Get destination file name
	_splitpath_s( szDestFileName, _drive, _MAX_DRIVE, _dir, _MAX_DIR, _fname, _MAX_FNAME, _ext, _MAX_EXT);
	string _dest_fname;
	_dest_fname += strPath;
	_dest_fname += _fname;
	_dest_fname += _ext;

	string _dest_fname_tmp;
	_dest_fname_tmp += _dest_fname;
	_dest_fname_tmp += ".tmp";


	// Merge index
	HANDLE hFile = ::CreateFile( _dest_fname_tmp.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)
		return false;

	for ( vector< string>::iterator itr = vFileList.begin();  itr != vFileList.end();  itr++)
	{
		HANDLE hRead = ::CreateFile( (*itr).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if ( hRead == INVALID_HANDLE_VALUE)
			continue;


		while ( true)
		{
			MFileHeader _header;
			DWORD _readnum;
			if ( ::ReadFile( hRead, &_header, sizeof( MFileHeader), &_readnum, NULL) == FALSE  ||  _readnum == 0)
				break;

			char szMotherFileName[ _MAX_PATH];
			memset( szMotherFileName, 0, _MAX_PATH);
			if ( ::ReadFile( hRead, szMotherFileName, _header.nMotherFileNameLen, &_readnum, NULL) == FALSE  ||  _readnum == 0)
				break;

			char szFileName[ _MAX_PATH];
			memset( szFileName, 0, _MAX_PATH);
			if ( ::ReadFile( hRead, szFileName, _header.nFileNameLen, &_readnum, NULL) == FALSE  ||  _readnum == 0)
				break;


			_splitpath_s( (*itr).c_str(), _drive, _MAX_DRIVE, _dir, _MAX_DIR, NULL, 0, NULL, 0);
			_splitpath_s( szMotherFileName, NULL, 0, NULL, 0, _fname, _MAX_FNAME, _ext, _MAX_EXT);
			char _motherfname[ _MAX_PATH];
			memset( _motherfname, 0, _MAX_PATH);
			sprintf_s( _motherfname, "%s%s%s%s", _drive, _dir, _fname, _ext);
			memcpy( _motherfname, _motherfname + strPath.length(), strlen( _motherfname) - strPath.length() + 1);
			_header.nMotherFileNameLen = strlen( _motherfname);


			DWORD _writenum;
			::WriteFile( hFile, &_header, sizeof( MFileHeader), &_writenum, NULL);
			::WriteFile( hFile, _motherfname, _header.nMotherFileNameLen, &_writenum, NULL);
			::WriteFile( hFile, szFileName, _header.nFileNameLen, &_writenum, NULL);
		}

		::CloseHandle( hRead);
	}


	// Encoding data
	size_t _data_size = ::GetFileSize( hFile, NULL);
	unsigned char* _data = new unsigned char[ _data_size];

	DWORD _num;
	::SetFilePointer( hFile, 0, 0, FILE_BEGIN);
	::ReadFile( hFile, _data, _data_size, &_num, NULL);

	size_t _data_enc_size = _data_size + max( 1024, ( _data_size >> 2));
	unsigned char* _data_enc = new unsigned char[ _data_enc_size];
	EncodingStr( _data, _data_size, _data_enc, &_data_enc_size);


	// Encrypt data
	Encrypt( _data_enc, _data_enc_size);

	::CloseHandle( hFile);



	// Write data
	hFile = ::CreateFile( _dest_fname.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE)
		return false;

	::WriteFile( hFile, &_data_size, sizeof( size_t), &_num, NULL);			// Data size
	::WriteFile( hFile, _data_enc, _data_enc_size, &_num, NULL);			// Data

	delete [] _data;
	delete [] _data_enc;
	::CloseHandle( hFile);

	
	::DeleteFile( _dest_fname_tmp.c_str());
	return true;
}


// main
int main( int numArgs, const char *args[])
{
	// Help
	if ( numArgs <= 1)
	{
		printf( "\nMRS Packer 0.1 beta  Copyright (c) 2010-2020 MAIET Entertainment\n\n");
		printf( "Usage : MRSPacker <command[:size(MB)]> <archive name> <file names> [target dir]\n");
		printf( "  archive : MRSPacker a out.mrf *.* outdir\n");
		printf( "            MRSPacker a:100 out.mrf *.* outdir\n");
		printf( "  extract : MRSPacker x out.mrf outdir\n");
		printf( "  merge   : MRSPacker m index.msf outdir/*.index\n\n");
		printf( "<Commands>\n  a : Add files to archive\n  x : eXtract files\n  m : Merge index file\n");
	}

	// Archive files
	else if ( numArgs >= 4  && ( _stricmp( args[ 1], "a") == 0  ||  _strnicmp( args[ 1], "a:", 2) == 0))
	{
		size_t nMaxSize = 0;
		if ( args[ 1][ 1] == ':')
			nMaxSize = atoi( args[ 1] + 2);

		SRes _result = EncodingDir( args[ 2], args[ 3], (numArgs > 4  ?  args[ 4]  :  ""), nMaxSize);
		if ( _result != SZ_OK)		return 1;
	}

	// Extract files
	else if ( numArgs >= 3  &&  _stricmp( args[ 1], "x") == 0)
	{
		SRes _result = DecodingDir( args[ 2],  (numArgs > 3  ?  args[ 3]  :  ""));
		if ( _result != SZ_OK)		return 1;
	}

	// Merge index
	else if ( numArgs >= 2  &&  _stricmp( args[ 1], "m") == 0)
	{
		SRes _result = MergeIndex( args[ 2],  args[ 3]);
		if ( _result != SZ_OK)		return 1;
	}

	return 0;
}
