#pragma once

#include "XGrappledController.h"

class XSwallowedController : public XGrappledController
{
private:
	bool					m_bDisAppear;


public:
	XSwallowedController();
	virtual ~XSwallowedController();



	virtual bool OnStart( _DamageRepeatInfo * pHitInfo);
	virtual void OnFinish();

	void OnDisAppear();
	void OnAppear();
};

