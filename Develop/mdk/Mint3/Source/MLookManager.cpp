#include "stdafx.h"
#include "MFrame.h"
#include "MBFrameLook.h"
#include "MButton.h"
#include "MBButtonLook.h"
#include "MEdit.h"
#include "MBEditLook.h"
#include "MListBox.h"
#include "MBListBoxLook.h"
#include "MBScrollBarLook.h"
#include "MBSpinCtrlLook.h"
#include "MBTextAreaLook.h"
#include "MBGroupLook.h"
#include "MBTreeViewLook.h"
#include "MBPopupBoxLook.h"
#include "MLookManager.h"

namespace mint3 {

MLookManager::MLookManager()
{
}

MLookManager::~MLookManager()
{
	ClearLooks();
}

void MLookManager::ClearLooks()
{
#define CLEAR(x) while(!x.empty()) { delete (*x.begin()).second; x.erase(x.begin()); }

	CLEAR(m_FrameLookMap);
	CLEAR(m_ButtonLookMap);
	CLEAR(m_EditLookMap);
	CLEAR(m_ListBoxLookMap);
	CLEAR(m_ScrollBarLookMap);
	CLEAR(m_SpinCtrlLookMap);
	CLEAR(m_TextAreaLookMap);
	CLEAR(m_GroupLookMap);
	CLEAR(m_TreeViewLookMap);
	CLEAR(m_PopupBoxLookMap);
}


} // namespace mint3