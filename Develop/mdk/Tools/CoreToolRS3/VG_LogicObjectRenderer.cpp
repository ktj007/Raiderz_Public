#include "StdAfx.h"
#include "VG_LogicObjectRenderer.h"
#include "M_LogicObject_SceneNodes.h"
#include "C_ControlMediatorHelperAction.h"
#include "V_LogicObjectRenderer.h"

VG_LogicObjectRenderer::VG_LogicObjectRenderer(void)
{
}

VG_LogicObjectRenderer::~VG_LogicObjectRenderer(void)
{
	_ASSERT( m_mapLogicObjectRenderer.empty() );
}

void VG_LogicObjectRenderer::Listen(CoreMessage& _message)
{
	// object event
	CLogicObject* pLogicObject = _message.pLogicObject;
	if( pLogicObject )
	{
		switch( _message.nID)
		{
		case VIEW_MESSAGE::LOGIC_NEW:		OnLogicObjectNew( pLogicObject );break;
		case VIEW_MESSAGE::LOGIC_DELETE:	OnLogicObjectDelete( pLogicObject );break;
		case VIEW_MESSAGE::LOGIC_MOVE:		OnLogicObjectMove( pLogicObject );break;
		case VIEW_MESSAGE::LOGIC_CHANGED:	OnLogicObjectChanged( pLogicObject );break;
		case VIEW_MESSAGE::LOGIC_SHOW:		OnLogicObjectShow( pLogicObject );break;
		case VIEW_MESSAGE::LOGIC_HIDE:		OnLogicObjectHide( pLogicObject );break;
		}
	}

	// 전역 처리
	switch( _message.nID)
	{
	case VIEW_MESSAGE::LOGIC_RENDER : 
		OnLogicObjectRender(); 
		break;
	case VIEW_MESSAGE::LOGIC_TEXTINFO_RENDER : 
		OnLogicObjectTextInfoRender(); 
		break;
	}
}

void VG_LogicObjectRenderer::OnLogicObjectNew(CLogicObject* pLogicObject)
{
	CLogicObjectRenderer* pObjectRenderer = pLogicObject->CreateRenderer( m_pControlMediator_ );
	if(pObjectRenderer)
	{
		m_mapLogicObjectRenderer.insert( LOGIC_RENDERERMAP::value_type( pLogicObject, pObjectRenderer ) );
		pObjectRenderer->OnAdd( m_pControlMediator_ );
		
		// 초기값이 hide인경우
		if( pLogicObject->GetVisible() == false )
			pObjectRenderer->OnHide();
	}

	// 초기 위치 설정, view쪽으로 통보
	OnLogicObjectMove( pLogicObject );
}

void VG_LogicObjectRenderer::OnLogicObjectDelete(CLogicObject* pLogicObject)
{
	LOGIC_RENDERERMAP::iterator itr = m_mapLogicObjectRenderer.find( pLogicObject );
	if( itr == m_mapLogicObjectRenderer.end() ) return;

	CLogicObjectRenderer* pObjectRenderer = itr->second;
	pObjectRenderer->OnRemove( m_pControlMediator_ );
	delete pObjectRenderer;
	m_mapLogicObjectRenderer.erase( itr );
}

void VG_LogicObjectRenderer::OnLogicObjectMove(CLogicObject* pLogicObject)
{
	LOGIC_RENDERERMAP::iterator itr = m_mapLogicObjectRenderer.find( pLogicObject );
	if( itr == m_mapLogicObjectRenderer.end() ) return;

	CLogicObjectRenderer* pObjectRenderer = itr->second;
	pObjectRenderer->OnChanged();
}

void VG_LogicObjectRenderer::OnLogicObjectChanged(CLogicObject* pLogicObject)
{
	LOGIC_RENDERERMAP::iterator itr = m_mapLogicObjectRenderer.find( pLogicObject );
	if( itr == m_mapLogicObjectRenderer.end() ) return;

	CLogicObjectRenderer* pObjectRenderer = itr->second;
	pObjectRenderer->OnChanged();
}

void VG_LogicObjectRenderer::OnLogicObjectShow(CLogicObject* pLogicObject)
{
	LOGIC_RENDERERMAP::iterator itr = m_mapLogicObjectRenderer.find( pLogicObject );
	if( itr == m_mapLogicObjectRenderer.end() ) return;

	CLogicObjectRenderer* pObjectRenderer = itr->second;
	pObjectRenderer->OnShow();
}

void VG_LogicObjectRenderer::OnLogicObjectHide(CLogicObject* pLogicObject)
{
	LOGIC_RENDERERMAP::iterator itr = m_mapLogicObjectRenderer.find( pLogicObject );
	if( itr == m_mapLogicObjectRenderer.end() ) return;

	CLogicObjectRenderer* pObjectRenderer = itr->second;
	pObjectRenderer->OnHide();
}

void VG_LogicObjectRenderer::OnLogicObjectRender()
{
	LOGIC_RENDERERMAP::iterator itr = m_mapLogicObjectRenderer.begin();
	for( ; itr!= m_mapLogicObjectRenderer.end(); ++itr )
	{
		CLogicObjectRenderer* pObjectRenderer = itr->second;
		pObjectRenderer->OnRender();
	}

	// render selection
	LOGIC_OBJECTLIST *pLogicList = m_pControlMediator_->GetHelperAction()->GetSelectedLogicObject();
	for (LOGIC_OBJECTLIST::const_iterator it = pLogicList->begin(); it != pLogicList->end(); ++it )
	{
		CLogicObject* pLogicObject = *it;

		LOGIC_RENDERERMAP::iterator itr = m_mapLogicObjectRenderer.find( pLogicObject );
		if( itr == m_mapLogicObjectRenderer.end() ) continue;

		// 있으면 그린다
		CLogicObjectRenderer* pObjectRenderer = itr->second;
		pObjectRenderer->OnRenderSelection();
	}
}

void VG_LogicObjectRenderer::OnLogicObjectTextInfoRender()
{
	LOGIC_RENDERERMAP::iterator itr = m_mapLogicObjectRenderer.begin();
	for( ; itr!= m_mapLogicObjectRenderer.end(); ++itr )
	{
		CLogicObjectRenderer* pObjectRenderer = itr->second;
		pObjectRenderer->OnRenderTextInfo();
	}
}
