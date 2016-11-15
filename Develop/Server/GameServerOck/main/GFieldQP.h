#pragma once

class GField;

class GFieldQP
{
public:
	GFieldQP(GField* pField);
	~GFieldQP();

private:
	GField*	m_pField;
};
