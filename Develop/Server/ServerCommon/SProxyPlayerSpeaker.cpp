#include "stdafx.h"
#include "SProxyPlayerSpeaker.h"
#include "SProxyPlayerListener.h"
#include "STransData.h"

#if (_MSC_VER >= 1900)
#include <functional>
#endif

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
#if (_MSC_VER >= 1900)
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fn(&SProxyPlayerListener::OnPreAdd), cid));
#else
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnPreAdd), cid));
#endif
}

void SProxyPlayerSpeaker::OnAddComplate(const SProxyPlayer* pPlayer)
{
#if (_MSC_VER >= 1900)
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fn(&SProxyPlayerListener::OnAddComplate), pPlayer));
#else
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnAddComplate), pPlayer));
#endif
}

void SProxyPlayerSpeaker::OnPreRemove(const SProxyPlayer* pPlayer)
{
#if (_MSC_VER >= 1900)
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fn(&SProxyPlayerListener::OnPreRemove), pPlayer));
#else
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnPreRemove), pPlayer));
#endif
}

void SProxyPlayerSpeaker::OnRemoveComplate(CID cid)
{
#if (_MSC_VER >= 1900)
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fn(&SProxyPlayerListener::OnRemoveComplate), cid));
#else
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnRemoveComplate), cid));
#endif
}

void SProxyPlayerSpeaker::OnPreUpdate(const SProxyPlayer* pPlayer, const TD_PROXY_PLAYER_INFO& info)
{
	for_each(m_setListener.begin(), m_setListener.end(), tr1::bind(&SProxyPlayerListener::OnPreUpdate, tr1::placeholders::_1, pPlayer, info));
}

void SProxyPlayerSpeaker::OnUpdateComplate(const SProxyPlayer* pPlayer)
{
#if (_MSC_VER >= 1900)
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fn(&SProxyPlayerListener::OnUpdateComplate), pPlayer));
#else
	for_each(m_setListener.begin(), m_setListener.end(), bind2nd(mem_fun1(&SProxyPlayerListener::OnUpdateComplate), pPlayer));
#endif
}
