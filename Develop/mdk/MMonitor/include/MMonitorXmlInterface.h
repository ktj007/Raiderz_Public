#ifndef _MMONITORXMLINTERFACE
#define _MMONITORXMLINTERFACE


// 이 해더파일은 MMonitor프로젝트가 고유의 XML관리를 하는 클래스를 만들어 줘야한다.
// 지금은 임시로 이렇게 사용을 한다.


#include <crtdbg.h>


#include <string>


//using std::string;
//
//
//class MMonitorXmlElement : public MXmlElement
//{
//public :
//		MMonitorXmlElement() {}
//		virtual ~MMonitorXmlElement() {}
//public :
//	MMonitorXmlElement() {}
//	virtual ~MMonitorXmlElement() {}
//
//	virtual bool	GetChildNode( const DWORD dwIndex, MMonitorXmlElement*& pOutXmlElement )					{ _ASSERT( 0 ); return false; }
//
//	virtual void	GetAttribute( const DWORD dwIndex, char* szoutAttrName, char* szoutAttrValue )				{ _ASSERT( 0 ); }
//
//	//virtual DWORD	GetChildNodeCount()																			{ _ASSERT( 0 );  return 0; }
//	//virtual DWORD	GetAttributeCount()																			{ _ASSERT( 0 );  return 0; }
//
//	virtual void	GetTagName( char* sOutTagName )																{ _ASSERT( 0 ); }
//	
//	virtual void	GetContents( char* szOutStr )																{ _ASSERT( 0 ); }
//	virtual void	GetContents(int* ipOutValue)																{ _ASSERT( 0 ); }
//	virtual void	GetContents(bool* bpOutValue)																{ _ASSERT( 0 ); }
//	virtual void	GetContents(float* fpOutValue)																{ _ASSERT( 0 ); }
//	virtual void	GetContents(string* pstrValue)																{ _ASSERT( 0 ); }
//
//	virtual bool	GetAttribute(char* sOutText,		const char* sAttrName,	char* sDefaultText = "")		{ _ASSERT( 0 ); return false; }
//	virtual bool	GetAttribute(int* ipOutValue,		const char* sAttrName,	int nDefaultValue = 0)			{ _ASSERT( 0 ); return false; }
//	virtual bool	GetAttribute(bool* bOutValue,		const char* sAttrName,	bool bDefaultValue = false)		{ _ASSERT( 0 ); return false; }
//	virtual bool	GetAttribute(float* fpOutValue,		const char* sAttrName,	float fDefaultValue = 0.0f)		{ _ASSERT( 0 ); return false; }
//	virtual bool	GetAttribute(string* pstrOutValue,	const char* sAttrName,	char* sDefaultValue = "")		{ _ASSERT( 0 ); return false; }
//
//	virtual	void	Release()																					{}
//};



//class MMonitorXmlDocument
//{
//public :
//	MMonitorXmlDocument()																		{}
//	virtual ~MMonitorXmlDocument()																{}
//
//	virtual bool	Create()																	= 0;
//	virtual bool	Destroy()																	= 0;
//
//	virtual bool	LoadFromMemory( const char* szBuffer, const LANGID lanid = LANG_KOREAN )	= 0;
//	virtual bool	LoadFromFile( const char* szFileName )										= 0;
//
//	virtual bool	GetDocumentElement( MMonitorXmlElement*& pOutXmlElement )					= 0;
//};
//
//
//void DeleteMonitorXmlElement( MMonitorXmlElement*& pXmlElement );
//void DeleteMonitorXmlDocument( MMonitorXmlDocument*& pXmlDocument );

#endif