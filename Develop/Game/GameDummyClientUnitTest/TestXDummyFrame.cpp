#include "stdafx.h"
#include "XDummyFrame.h"

SUITE(XDummyFrame)
{
	TEST(XDummyFrame)
	{
		XDummyFrame frame;
		CHECK_EQUAL(DUMMYCLIENT_FRAME_ID, frame.GetID());
		
// 		XDummyFrame();
// 		virtual ~XDummyFrame();
// 
// 		virtual bool Open(int nPrevFrameID, const wchar_t* args) override;
// 
// 		virtual void Close(int nNextFrameID) override;
// 		virtual void Update(float fDelta) override;
// 		virtual bool OnEvent(MEvent& e) override;
// 		virtual void OnRender() override;
// 		virtual int GetID() override	{ return DUMMYCLIENT_FRAME_ID; }
	}
}