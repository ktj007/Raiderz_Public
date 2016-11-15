#pragma once

#include "MWidget.h"


using namespace std;
using namespace mint3;


/// class : XUIVideoLook
class XUIVideoLook : public MLookBase
{
/// Member variables
protected:
	MBitmapR3*	m_pBitmap;

/// Member functions
public:
	XUIVideoLook();
	virtual ~XUIVideoLook();

	virtual void OnDrawBorder( MWidget* pWidget, MDrawContext* pDC) override;
	virtual void OnDraw( MWidget* pWidget, MDrawContext* pDC) override;
};



class XBandiVideoPlayer;


/// class : XUIVideo
class XUIVideo : public MWidget
{
protected:
	XBandiVideoPlayer*		bandi_video;


public:
	XUIVideo( const char* szName =NULL, MWidget* pParent =NULL, MListener* pListener =NULL);
	virtual ~XUIVideo();

	bool Open(const char* szName);
	RTexture* GetVideoTexture();

	XBandiVideoPlayer*		GetBandi()	{ return bandi_video; }


protected:
	virtual bool OnEvent( MEvent* pEvent, MListener* pListener) override;
	virtual void OnRun( DWORD dwCurrTime) override;


public:
	void glueOpen(const char* szName)		{ Open(szName);	}


#define MINT_VIDEO			"Video"
	virtual const char* GetClassName() override			{ return MINT_VIDEO;				}
};