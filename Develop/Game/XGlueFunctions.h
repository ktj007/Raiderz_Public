#pragma once

#include "XGlueGame.h"
#include "XGlueGameOption.h"
#include "XGlueDebugFunction.h"



class XGlueFunctions
{
protected:
	XGlueGame					m_glueGame;
	XGlueGameOption				m_glueGameOption;
	XGlueDebugFunction			m_glueDebug;



public:
	XGlueFunctions()			{}

	void Init( MWLua* pLua);


protected:
	void DeclGlueFunc( MWLua* pLua);
	void SetVars( MWLua* pLua);
};

