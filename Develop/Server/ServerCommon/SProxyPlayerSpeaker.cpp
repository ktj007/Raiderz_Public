#include "stdafx.h"
#include "SProxyPlayerSpeaker.h"
#include "SProxyPlayerListener.h"
#include "STransData.h"

SProxyPlayerSpeaker::SProxyPlayerSpeaker()
{
	// do nothing
}

SProxyPlayerSpeaker::~SProxyPlayerSpeaker()
{
	// do nothing
}

void SProxyPlayerSpeaker::AddListener(SProxyPlayerListener* pListener)
{
	_ASSERT(pListener);
	m_setListener.insert(pListener);
}

void SProxyPlayerSpeaker::RemoveListener(SProxyPlayerListener* pListener)
{
	m_setListener.erase(pListener);
}

void SProxyPlayerSpeaker::OnPreAdd(CID cid)
{
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnPreAdd), cid));
}

void SProxyPlayerSpeaker::OnAddComplate(const SProxyPlayer* pPlayer)
{
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnAddComplate), pPlayer));
}

void SProxyPlayerSpeaker::OnPreRemove(const SProxyPlayer* pPlayer)
{
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnPreRemove), pPlayer));
}

void SProxyPlayerSpeaker::OnRemoveComplate(CID cid)
{
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnRemoveComplate), cid));
}

void SProxyPlayerSpeaker::OnPreUpdate(const SProxyPlayer* pPlayer, const TD_PROXY_PLAYER_INFO& info)
{
	for_each(m_setListener.begin(), m_setListener.end(), tr1::bind(&SProxyPlayerListener::OnPreUpdate, tr1::placeholders::_1, pPlayer, info));
}

void SProxyPlayerSpeaker::OnUpdateComplate(const SProxyPlayer* pPlayer)
{
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnUpdateComplate), pPlayer));
}
