#include "stdafx.h"
#include "XGlueCharEvent.h"

XGlueCharEvent::XGlueCharEvent() : m_pMWLua(NULL)
{

}



void XGlueCharEvent::Init( MWLua* pLua )
{
	m_pMWLua = pLua;
}
