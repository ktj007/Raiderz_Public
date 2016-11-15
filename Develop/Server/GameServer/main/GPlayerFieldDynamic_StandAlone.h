#pragma once

#include "GPlayerFieldDynamic.h"

class GPlayerFieldDynamic_StandAlone : public GPlayerFieldDynamic
{
protected:
	virtual void			_EndGateExecute(GDynamicFieldMaster* pFieldGroup) override;

public:
							GPlayerFieldDynamic_StandAlone(GEntityPlayer* pOwner, GPlayerField* pPlayerField);
	virtual					~GPlayerFieldDynamic_StandAlone();

	virtual bool			GateRequest(DYNAMIC_FIELD_TYPE eType, int nFieldGroupID, int nFieldID, vec3 vecDestPos, vec3 vecDestDir, bool bSingleEnter = false) override;
};
