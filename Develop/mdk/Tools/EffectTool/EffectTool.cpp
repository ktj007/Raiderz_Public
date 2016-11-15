// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "MainFrame.h"

namespace EffectTool
{

class MainApp : public wxApp
{
public:
	bool OnInit();
};

DECLARE_APP(MainApp)
IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
	if(false == wxApp::OnInit())
		return false;

	wxFrame* frame = new MainFrame(NULL, wxID_ANY, "Effect Tool", wxDefaultPosition, wxSize(1024, 768));
	SetTopWindow(frame);
	frame->Show();
	return true;
}

}