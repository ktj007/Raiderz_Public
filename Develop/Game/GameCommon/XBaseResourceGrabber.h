#pragma once

class XResourceRefMgr;

class XBaseResourceGrabber
{
public:
	XBaseResourceGrabber() {}
	virtual ~XBaseResourceGrabber() {}
};

class XBaseAlwaysResourceGrabber : public XBaseResourceGrabber
{
protected:
	XResourceRefMgr*		m_pResourceRefMgr;
	void ReserveAddRef(const TCHAR* szResName);
public:
	XBaseAlwaysResourceGrabber();
	virtual ~XBaseAlwaysResourceGrabber();
	void Create();
	void Destroy(RDevice* pDevice);

	void OnCreateResource( RResource* pResource );
	virtual void Grab();
};