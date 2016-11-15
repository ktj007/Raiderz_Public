#include "stdafx.h"
#include "XCheckSum.h"




XCheckSum::XCheckSum()
{
}


XCheckSum::~XCheckSum()
{
}


void XCheckSum::_get_filelist( const char* szPath, vector< _FILEINFO>* pPathList)
{
	char _path[ MAX_PATH];
	sprintf_s( _path, "%s\\*.*", szPath);

	long hFile;
	struct _finddata_t file_t;

	if ( (hFile = _findfirst( _path, &file_t)) != -1L ) 
	{
		do
		{
			// Sub directory
			if ( file_t.attrib & _A_SUBDIR)
			{
				if ( strcmp( file_t.name, "." ) == 0)		continue;
				if ( strcmp( file_t.name, "..") == 0)		continue;
				if ( strcmp( file_t.name, ".svn") == 0)		continue;

				char _subdir[ MAX_PATH];
				sprintf_s( _subdir, "%s\\%s", szPath, file_t.name);
				_get_filelist( _subdir, pPathList);
			}

			else if ( file_t.attrib & (_A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_ARCH))
			{
				char _drive[ _MAX_DRIVE], _dir[ _MAX_DIR], _fname[ _MAX_FNAME], _ext[ _MAX_EXT];
				_splitpath_s( file_t.name, _drive, _dir, _fname, _ext);

				if ( _stricmp( _ext, ".MRS") != 0)			continue;

				char _pathname[ MAX_PATH];
				sprintf_s( _pathname, "%s\\%s", szPath, file_t.name);

				DWORD _checksum = 0, _checkbit = 0, _filesize =0;
				GetFileCheckSum( _pathname, &_checksum, &_checkbit, &_filesize);
				pPathList->push_back( _FILEINFO( _pathname, _checksum, _checkbit, _filesize));
			}

		}	while( _findnext( hFile, &file_t) == 0);

		_findclose( hFile);
	}
}


bool XCheckSum::GetFileInfo( const char* szPath, vector< _FILEINFO>* pPathList)
{
	_get_filelist( szPath, pPathList);


	size_t _len = strlen( szPath);
	for ( vector< _FILEINFO>::iterator itr = pPathList->begin();  itr != pPathList->end();  itr++)
	{
		string _pathname = (*itr).strName;

		size_t i = 0;
		while ( (i = _pathname.find( "\\")) != wstring::npos)
			_pathname.replace( i, 1, "/");

		(*itr).strName = _pathname.substr( _len + 1, _pathname.length());
	}

	return true;
}


bool XCheckSum::GetFileCheckSum( const char* szFileName, DWORD* pnCheckSum, DWORD* pnCheckBit, DWORD* pnFileSize)
{
	DWORD dwCheckSum = 0;
	unsigned char dwCheckBit = 0;
	DWORD dwFileSize = 0;


	FILE* pFile;
	fopen_s( &pFile, szFileName, "rb");
	if ( pFile == NULL)		return false;


	fseek( pFile, 0, SEEK_END);

	dwFileSize = ftell( pFile);
	fseek( pFile, 0, SEEK_SET);

	unsigned char* _buff = new unsigned char[ dwFileSize];
	fread( _buff, 1 , dwFileSize, pFile);

	fclose( pFile);


	DWORD _gap = max( 1,  (int)( (double)dwFileSize / 100.0));
	for ( DWORD i = 0;  i < dwFileSize;  i += _gap)
	{
		dwCheckSum += _buff[ i];
		dwCheckBit ^= _buff[ i];
	}

	delete [] _buff;


	if ( pnCheckSum != NULL)		*pnCheckSum = dwCheckSum;
	if ( pnCheckBit != NULL)		*pnCheckBit = dwCheckBit;
	if ( pnFileSize != NULL)		*pnFileSize = dwFileSize;

	return true;
}
