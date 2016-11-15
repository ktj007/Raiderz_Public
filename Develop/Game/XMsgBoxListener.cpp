#include "stdafx.h"
#include "XMsgBox.h"


// XMsgBox Listener
class XMsgBoxListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
	{
// 		switch (nMsg)
// 		{
// 		case MEVENT_MSGBOX_OK:
// 			{
// 				XMsgBox* pMsgBox = (XMsgBox*)pWidget;
// 				if (pMsgBox->GetCustomListener()) 
// 				{
// 					bool bResult = pMsgBox->GetCustomListener()->OnCommand(pWidget, nMsg, szArgs);
// 					pMsgBox->SetCustomListener(NULL);
// 					return bResult;
// 				}
// 
// 				pWidget->Show(false);
// 
// 				return true;
// 			}
// 			break;
// 		}

		return false;
	}
};

XMsgBoxListener	g_MsgBoxListener;

MListener* XGetMsgBoxListener(void)
{
	return &g_MsgBoxListener;
}

// XConfirmMsgBox Listener : Default Listener 일뿐. 물어볼때마다 CustomListener 지정해서 쓸것
class XConfirmMsgBoxListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, MEventMsg nMsg, const char* szArgs)
	{
// 		switch (nMsg)
// 		{
// 		case MEVENT_MSGBOX_OK:
// 			{
// 				XMsgBox* pMsgBox = (XMsgBox*)pWidget;
// 				if (pMsgBox->GetCustomListener()) 
// 				{
// 					bool bResult = pMsgBox->GetCustomListener()->OnCommand(pWidget, nMsg, szArgs);
// 					pMsgBox->SetCustomListener(NULL);
// 					return bResult;
// 				}
// 
// 				pWidget->Show(false);
// 
// 				return true;
// 			}
// 			break;
// 		}
		return false;
	}
};

XConfirmMsgBoxListener	g_CofirmMsgBoxListener;

MListener* XGetConfirmMsgBoxListener(void)
{
	return &g_CofirmMsgBoxListener;
}
