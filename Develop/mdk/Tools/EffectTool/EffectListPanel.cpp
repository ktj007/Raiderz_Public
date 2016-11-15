#include "wx/wxprec.h"

#include "EffectListPanel.h"
#include "Config.h"

#include "MainFrame.h"

namespace EffectTool
{

BEGIN_EVENT_TABLE(EffectListPanel, wxPanel)
	EVT_TEXT(EffectListPanel::ID_FILTER_TEXTCTRL, EffectListPanel::OnFilterChanged)
	EVT_LISTBOX(EffectListPanel::ID_EFFECT_LISTBOX, EffectListPanel::OnSelected)
END_EVENT_TABLE()

EffectListPanel::EffectListPanel(wxWindow* parent) :
 wxPanel(parent)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxTextCtrl* searchTextCtrl = new wxTextCtrl(this, ID_FILTER_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(200, wxDefaultSize.y));
	sizer->Add(searchTextCtrl, wxSizerFlags(0).Top().Border());

	wxListBox* effectListBox = new wxListBox(this, ID_EFFECT_LISTBOX);
	sizer->Add(effectListBox, wxSizerFlags(1).Center().Border().Expand());

	SetSizerAndFit(sizer);
}

EffectListPanel::~EffectListPanel()
{
}

void EffectListPanel::Init()
{
	RefleshEffectList();
	RefleshListBox("");
}

void EffectListPanel::RefleshEffectList()
{
	m_effectList.clear();

	BOOL bRet = TRUE;
	wxString filter = CConfig::GetInstance().m_strEffectPath + "*" + EFFECT_FILE_EXT;
	WIN32_FIND_DATA findData = {0};
	HANDLE hDir = FindFirstFile(filter.c_str(), &findData);
	while( (hDir != INVALID_HANDLE_VALUE) && bRet )
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) 
		{
			wxString fileName = findData.cFileName;
			fileName.RemoveLast(EFFECT_FILE_EXT_LENGTH);
			m_effectList.push_back(fileName);
		}
		bRet = FindNextFile(hDir, &findData);
	}
	FindClose(hDir);
}

void EffectListPanel::RefleshListBox(const wxString& filter)
{
	wxListBox* listBox = wxDynamicCast(FindWindow(ID_EFFECT_LISTBOX), wxListBox);
	_ASSERT(listBox);

	wxString lowerCaseFilter = filter.Lower();

	listBox->Clear();
	for(size_t i = 0; i < m_effectList.size(); ++i)
	{
		wxString effectFileName = m_effectList[i];
		effectFileName.LowerCase();

		if( effectFileName.Contains(lowerCaseFilter) )
			listBox->Append(m_effectList[i]);
	}
}

void EffectListPanel::OnFilterChanged(wxCommandEvent& event)
{
	RefleshListBox(event.GetString());
}

void EffectListPanel::OnSelected(wxCommandEvent& event)
{
	MainFrame* mainFrame = wxDynamicCast(GetParent(), MainFrame);
	_ASSERT(mainFrame);

	int selectedIndex = event.GetSelection();
	if(wxNOT_FOUND != selectedIndex)
	{
		mainFrame->CreateEffectSceneNode(event.GetString());
	}
}

}
