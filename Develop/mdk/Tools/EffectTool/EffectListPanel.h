#pragma once
#include "wx/panel.h"

#include <vector>
#include <string>

namespace EffectTool
{

typedef std::vector<wxString> EFFECTLIST;

class EffectListPanel : public wxPanel
{
	enum eID
	{
		ID_FILTER_TEXTCTRL = wxID_HIGHEST+1,
		ID_EFFECT_LISTBOX,
	};

public:
	EffectListPanel(wxWindow* parent);
	~EffectListPanel();

	void Init();

private:
	void RefleshEffectList();
	void RefleshListBox(const wxString& filter);

	void OnFilterChanged(wxCommandEvent& event);
	void OnSelected(wxCommandEvent& event);
	
	EFFECTLIST	m_effectList;

	DECLARE_EVENT_TABLE()
};

}