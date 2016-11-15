#include "StdAfx.h"
#include "GPlayerRecipe.h"

GPlayerRecipe::GPlayerRecipe(GEntityPlayer* pOwner)
: m_pOwner(pOwner)
{
}

GPlayerRecipe::~GPlayerRecipe(void)
{
}

void GPlayerRecipe::Insert( int nRecipeID )
{
	m_setRecipeID.insert(nRecipeID);
}

void GPlayerRecipe::Delete( int nRecipeID )
{
	m_setRecipeID.erase(nRecipeID);
}

bool GPlayerRecipe::IsExist( int nRecipeID )
{
	if (m_setRecipeID.end() == m_setRecipeID.find(nRecipeID)) return false;

	return true;
}

bool GPlayerRecipe::IsEmpty()
{
	return m_setRecipeID.empty();
}

const SET_RECIPEID& GPlayerRecipe::GetContainer()
{
	return m_setRecipeID;
}