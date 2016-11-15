#pragma once

#include "XGameFrame.h"
#include "XBirdDummyAgent.h"
#include "XGameFrameID.h"

class XDummyMaster;
class XDummyPattern;
class XDummyHandler_LoginFlood;
class XDummyUI;

class XDummyFrame : public XBaseFrame
{
private:
	XDummyMaster*		m_pDummyMaster;
	XDummyUI*			m_pUI;
	string				m_strDummyClientFileName;

	//map<string, XDummyPattern *> m_mapDummyClientTestList;

public:
	XDummyFrame();
	virtual ~XDummyFrame();

	virtual bool Open(int nPrevFrameID, const wchar_t* args) override;

	virtual void Close(int nNextFrameID) override;
	virtual void Update(float fDelta) override;
	virtual bool OnEvent(MEvent& e) override;
	virtual void OnRender() override;
	virtual int GetID() override	{ return DUMMYCLIENT_FRAME_ID; }

};