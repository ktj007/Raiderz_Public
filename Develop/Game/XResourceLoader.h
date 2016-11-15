#pragma once


#define ALL_LEVEL		-1
#define MAX_LEVEL		3



struct XResFile
{
	string		strFileName;
	int			nLevel;

	XResFile( const string& _filename, int _level) : strFileName( _filename), nLevel( _level)		{}
};




/// class : XResourceLoader
class XResourceLoader
{
/// Member variables
public:
	list< XResFile>		m_vFileList;


// Member functions
public:
	XResourceLoader();
	virtual ~XResourceLoader();

	bool LoadResource();

	static XResourceLoader* GetInstance();
};





// inline : XGetResourceLoader
inline XResourceLoader* XGetResourceLoader()
{
	return XResourceLoader::GetInstance();
}
