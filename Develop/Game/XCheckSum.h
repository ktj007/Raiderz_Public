#pragma once

#include <stdio.h>
#include <io.h>
#include <vector>
#include <string>
#include <fstream>
using namespace std;




struct _FILEINFO
{
	string			strName;
	DWORD			nCheckSum;
	DWORD			nCheckBit;
	DWORD			nFileSize;

	_FILEINFO() : nCheckSum( 0), nCheckBit( 0), nFileSize( 0)	{}
	_FILEINFO( string _name) : strName( _name), nCheckSum( 0), nCheckBit( 0), nFileSize( 0)		{}
	_FILEINFO( string _name, DWORD _chksum, DWORD _chkbit, DWORD _size) : strName( _name), nCheckSum( _chksum), nCheckBit( _chkbit), nFileSize( _size)	{}
};



class XCheckSum
{
protected:


public:
	XCheckSum();
	virtual ~XCheckSum();


	bool GetFileInfo( const char* szPath, vector< _FILEINFO>* pPathList);
	bool GetFileCheckSum( const char* szFileName, DWORD* pnCheckSum, DWORD* pnCheckBit, DWORD* pnFileSize);


protected:
	void _get_filelist( const char* szPath, vector< _FILEINFO>* pPathList);
};
