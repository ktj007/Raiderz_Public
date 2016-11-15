#pragma once

#include "XBaseApplication.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// 어플리케이션 클래스
//
class XGameApp : public XBaseApplication
{
protected:
	virtual void			CreateGameFrames() override;
	virtual bool			ParseArguments();
	virtual bool			OnCreate();
	virtual bool			InitGlobalInstance();
public:
							XGameApp();
	virtual					~XGameApp();

	virtual const wchar_t*			GetProductVersion();
	virtual const wchar_t*			GetSVNRevision();
	virtual const wchar_t*			GetSVNDate();
};

