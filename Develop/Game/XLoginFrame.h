
#pragma once

#include "XGameFrame.h"
#include "XGameFrameID.h"


using namespace rs3;



/// class : XLoginFrame
class XLoginFrame : public XBaseFrame
{
/// Member variables
protected:
	bool			m_bLoadCampaign;
	bool			m_bUpdateOnce;
	bool			m_bLoaded;
	bool			m_bLogined;

/// Member functions
	void			Load();
public:
	XLoginFrame();
	virtual ~XLoginFrame();

	virtual bool Open( int nPrevFrameID, const wchar_t* szArgs) override;
	virtual void Close( int nNextFrameID) override;

	virtual int GetID() override				{ return LOGIN_FRAME_ID;					}

	bool DispatchEvent( MEvent& e);
	bool UpdateOnce( float fDelta );

/// Event handler
public:
	virtual bool OnEvent( MEvent& e) override;
	virtual void Update( float fDelta) override;
	virtual void OnRender() override;
};
