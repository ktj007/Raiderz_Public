#pragma once

// interface class identifier
#define CLSID_APP			0x10

// interface IDs
#define IID_MyIUnknown		0x1000 
#define IID_IDocument		0x1001

// document constants
#define C_DOC_1				0x100
#define C_DOC_2				0x101

#ifdef _DEBUG
#define CORETOOL_DLL_NAME	"CoreTool_d.dll"
#else
#define CORETOOL_DLL_NAME	"CoreTool.dll"
#endif

// instantiate an interface class object
extern "C" BOOL __declspec(dllexport) App_GetClassObject(int nClsid, int nId, void** ppvObj);

// defined instead of MFC's IUnknown
struct MyIUnknown
{
    MyIUnknown() { TRACE("Entering IUnknown ctor %p\n", this); }
    virtual BOOL QueryInterface(int nIid, void** ppvObj) = 0;
    virtual DWORD Release() = 0;
    virtual DWORD AddRef() = 0;
};



//=====================================================
// *** class IDocument ***
// 
//-----------------------------------------------------
// method CreateDocTempl()
//
//		create doc template View/Doc for the given
//		document type
//
//-----------------------------------------------------
// method CreateFrame()
//
//		create MDI frame; must be called after
//		CreateDocTempl()
//	
//=====================================================

struct AFX_EXT_CLASS IDocument : public MyIUnknown
{
    IDocument() { TRACE("Entering IDocument ctor %p\n", this); }
    virtual CDocTemplate* CreateDocTempl( CWinApp*, int ) = 0;
    virtual void CreateFrame() = 0;

	virtual void OnInitInstance() = 0;
	virtual void OnPostCreate() = 0;
	virtual void OnExitInstance() = 0;

	virtual BOOL OnIdle(LONG lCount) = 0;
	virtual bool PreTranslateMessage(MSG* pMsg) = 0;

	virtual int GetExitCode() = 0;
	virtual void OnShellOpenFile() = 0;
};
