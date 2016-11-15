#include "StdAfx.h"
#include "CSModEffector.h"

void CSModEffector::AddActorModifier(CSActorModifier& actorModifier, const CSActorModifier& Modifier)
{
	actorModifier += Modifier;
}

void CSModEffector::EraseActorModifier(CSActorModifier& actorModifier, const CSActorModifier& Modifier)
{
	actorModifier -= Modifier;
}

void CSModEffector::AddPlayerModifier(CSPlayerModifier& playerModifier, const CSPlayerModifier& Modifier)
{
	playerModifier += Modifier;
}

void CSModEffector::ErasePlayerModifier(CSPlayerModifier& playerModifier, const CSPlayerModifier& Modifier)
{
	playerModifier -= Modifier;
}