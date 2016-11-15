#pragma once

#include "C_ControlMediatorHelperResource.h"

class CORETOOL_SOULHUNT_API CControlMediatorHelperNewMapResourceImpl : public CControlMediatorHelperNewMapResource
{
public:
	enum NEW_MAP_TYPE
	{
		E_ZONE,
		E_FIELD,
		E_CUTSCENE, 

		E_NONE
	};

	CControlMediatorHelperNewMapResourceImpl(CControlMediator* _pControlMediator, NEW_MAP_TYPE _type)
	{
		m_pControlMediator_ = _pControlMediator;
		m_eNewMapType_ = _type;
	}

	virtual ~CControlMediatorHelperNewMapResourceImpl(){}

	virtual bool IsKeyEventHandlerAcceptable();
	virtual const char* GetNewMapName();
	virtual void OpenNewMapDlg();

protected:
	CControlMediator* m_pControlMediator_;
	NEW_MAP_TYPE m_eNewMapType_;
};
