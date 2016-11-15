#pragma once

#include "XTimer.h"

class XNPCIcon
{
protected:

	NPC_ICON_TYPE			m_NIT;
	// NPC ICon
	NPC_ICON_STATE			m_NIS;		// 현재 아이콘 상태
	NPC_ICON_STATE			m_PrevNIS;	// 바로 전의 아이콘 상태

	XModuleEffect*			m_pModuleEffect;

	list<NPC_ICON_STATE>	m_NISBuffer;
	XRegulator				m_Regulator;

	void			SetNPCIconEffect( NPC_ICON_STATE NIS );
	void			CheckException();

public:
	XNPCIcon(XModuleEffect* pModuleEffect);
	virtual ~XNPCIcon(void);

	NPC_ICON_STATE	GetNPCIConState() const { return m_NIS; }

	void			SetNPCIConTypeToIconState(NPC_ICON_TYPE NIT );
	void			SetNPCIConState( NPC_ICON_STATE NIS );
	void			SetPreviousNPCIcon();

	void			OnUpdate(float fDelta);
};
