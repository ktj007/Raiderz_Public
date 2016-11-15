#pragma once
#include "wx/frame.h"
#include "wx/aui/aui.h"

#include "MDebug.h"

namespace rs3
{
	class RSceneNode;
}
using namespace rs3;

namespace EffectTool
{

class MainFrame : public wxFrame
{
	enum eID
	{
		ID_MAINFRAME = wxID_HIGHEST+1,

		ID_MENU_VIEW_EFFECTLIST,
		ID_MENU_VIEW_EFFECTSCENE,
	};

public:
	MainFrame(wxWindow* parent, wxWindowID id, const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
	~MainFrame();

	bool CreateEffectSceneNode(const wxString& fileName);
	void RefreshEffectSceneTreeCtrl(RSceneNode* pSceneNode);

	static void	MLogCallBack(const char* szText, MLogLevel nLevel = MLOG_LEVEL_DEBUG);

private:
	wxAuiManager* GetAuiManager();
	wxAuiPaneInfo& GetPanel(const wxString& name);

	void ShowPanel(const wxString& name, bool show);

	void OnQuit( wxCommandEvent& event );
	void OnViewEffectList( wxCommandEvent& event );
	void OnViewEffectScene( wxCommandEvent& event );

	void CreateMenuBar();

	DECLARE_EVENT_TABLE()
};

}