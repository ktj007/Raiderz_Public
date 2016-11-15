#pragma once

namespace mint3 {

class MLookManager
{
private:
protected:

public:
	map<string, MBFrameLook*>		m_FrameLookMap;
	map<string, MButtonLook*>		m_ButtonLookMap;
	map<string, MBEditLook*>		m_EditLookMap;
	map<string, MBListBoxLook*>		m_ListBoxLookMap;
	map<string, MBScrollBarLook*>	m_ScrollBarLookMap;
	map<string, MBSpinCtrlLook*>	m_SpinCtrlLookMap;
	map<string, MBTextAreaLook*>	m_TextAreaLookMap;
	map<string, MBGroupLook*>		m_GroupLookMap;
	map<string, MBTreeViewLook*>	m_TreeViewLookMap;
	map<string, MBPopupBoxLook*>	m_PopupBoxLookMap;

//	map<string, MBLabelLook*>		m_LabelLookMap;
//	map<string, MBGroupLook*>		m_GroupLookMap;
//	map<string, MBListBoxLook*>		m_ListBoxLookMap;
//	map<string, MBScrollBarLook*>	m_ScrollBarLookMap;
//	map<string, MBArrowLook*>		m_ArrowLookMap;
//	map<string, MBThumbLook*>		m_ThumbLookMap;
//	map<string, MBSliderLook*>		m_SliderLookMap;
//	map<string, MAniBitmap*>		m_AniBitmapMap;
//	map<string, MBTextAreaLook*>	m_TextAreaLookMap;
//	map<string, MBTabCtrlLook*>		m_TabCtrlLookMap;
//
//	map<string, MButtonGroup*>		m_ButtonGroupMap;

public:
	MLookManager();
	virtual ~MLookManager();

	void		ClearLooks();
};

} // namespace mint3

