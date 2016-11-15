#pragma once

#include "ext_lib/FastDelegate.h"

class CORETOOL_API CCoreToolEdit : public CEdit
{
public:
	CCoreToolEdit();

	typedef fastdelegate::FastDelegate0< > EDIT_ENTER_HANDLER;

	void SetCallback( EDIT_ENTER_HANDLER );

protected:
	BOOL PreTranslateMessage(MSG* pMsg);

	EDIT_ENTER_HANDLER m_fnCallback;
};

