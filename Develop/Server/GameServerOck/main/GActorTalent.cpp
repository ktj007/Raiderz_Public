#include "StdAfx.h"
#include "GActorTalent.h"
#include "GEntityActor.h"

GActorTalent::GActorTalent(GEntityActor* pOwner)
: m_pOwner(pOwner), m_vUseTalentPos(vec3::ZERO)
{
	m_pOwner->AttachObserver(this);
}

GActorTalent::~GActorTalent(void)
{
	m_pOwner->DetachObserver(this);
}

void GActorTalent::OnUseTalent( GEntityActor* pUser, GTalentInfo* pTalentInfo )
{
	m_vUseTalentPos = m_pOwner->GetPos();
}

void GActorTalent::OnFinishTalent( GTalentInfo* pTalentInfo )
{
	m_vUseTalentPos = vec3::ZERO;
}

bool GActorTalent::HasUseTalentPos() const
{
	return m_vUseTalentPos != vec3::ZERO;
}

vec3 GActorTalent::GetUseTalentPos() const
{
	return m_vUseTalentPos;
}