#pragma once

#include "GGlueField.h"

class GNullGlueField : public GGlueField
{
public:
	GNullGlueField(void);
	virtual ~GNullGlueField(void);

	static void Init();
	static void FIni();

	static GNullGlueField* GetNull();

	virtual bool IsNull() const { return true; }

private:
	static GNullGlueField* m_pNullObj;
};
