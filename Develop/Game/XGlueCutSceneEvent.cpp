#include "stdafx.h"
#include "XGlueCutSceneEvent.h"

XGlueCutSceneEvent::XGlueCutSceneEvent() : m_pMWLua(NULL)
{

}

void XGlueCutSceneEvent::Init( MWLua* pLua )
{
	m_pMWLua = pLua;
}

XGlueCutSceneEvent::~XGlueCutSceneEvent()
{
	m_pMWLua = NULL;
}
