#pragma once

#include "XBaseApplication.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// 어플리케이션 클래스
//
class XGameApp : public XBaseApplication
{
public:
							XGameApp();
	virtual					~XGameApp();

	virtual const wchar_t*			GetProductVersion();
	virtual const wchar_t*			GetSVNRevision();
	virtual const wchar_t*			GetSVNDate();
};

