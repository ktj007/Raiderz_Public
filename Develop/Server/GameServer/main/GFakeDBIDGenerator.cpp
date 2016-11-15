#include "StdAfx.h"
#include "GFakeDBIDGenerator.h"

int		GFakeDBIDGenerator::m_nCID	= 0;

GFakeDBIDGenerator::GFakeDBIDGenerator(void)
{
}

GFakeDBIDGenerator::~GFakeDBIDGenerator(void)
{
}

int GFakeDBIDGenerator::NewCID()
{
	m_nCID++;
	return m_nCID;
}