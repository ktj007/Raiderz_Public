#pragma once

class GNPCAI;

class GNPCAIUpdater
{
public:
	GNPCAIUpdater(GNPCAI* pSubject);
	virtual ~GNPCAIUpdater();

	virtual void Update(float fDelta) = 0;
	virtual float ResetRemainDelta(void)	{ return 0.0f;	}

protected:
	GNPCAI* m_pSubject;
};
