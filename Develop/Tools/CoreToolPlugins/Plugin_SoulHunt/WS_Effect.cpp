#include "stdafx.h"
#include "MDebug.h"
#include "WS_Effect.h"
#include "REffectInstance.h"

WS_Effect::WS_Effect()
:XModelEffect()
{
}

WS_Effect::~WS_Effect()
{

}

bool WS_Effect::Create(MUID& uid, const string& str)
{
	m_UID = uid;

	REffectInstanceSceneNode effectInstance;
 	if( !effectInstance.Create( str.c_str() ) )
 	{
#ifdef _DEBUG
 		mlog("CreateEffect ½ÇÆÐ(%s)\r", str.c_str());
#endif
 		return false;
 	}
	effectInstance.GetEffectSceneNode()->RemoveFromParent();

	m_pEffect = effectInstance.GetEffectSceneNode();

	m_strEffectFileName = str;

	return true;
}

void WS_Effect::UpdateXEffect( float fDelta )
{
	if( m_pEffect )
	{
		RSceneNodeUpdateVisitor v;
		m_pEffect->Traverse( &v );
	}

	UpdateDurationTime(fDelta);
	UpdateEffectFader(fDelta);
}