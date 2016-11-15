#pragma once

#include "XBaseResourceGrabber.h"
#include "MTstring.h"

class XFieldResourceGrabber : public XBaseResourceGrabber
{
protected:
	XResourceRefMgr*		m_pResourceRefMgr;

	void GrabResources( vector<tstring> &vecResourceNames, int nResID );
	void ParseResource( MXmlElement* pElement, vector<tstring> &outvecTextureResourceNames );
public:
	XFieldResourceGrabber();
	virtual ~XFieldResourceGrabber();
	void Grab(tstring& strNextZoneResFile);
	void Drop();
};