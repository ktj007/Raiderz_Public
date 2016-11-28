#include "StdAfx.h"
#include "GFakeDBIDGenerator.h"

CID		GFakeDBIDGenerator::m_nCID	= 0;

GFakeDBIDGenerator::GFakeDBIDGenerator(void)
{
}

GFakeDBIDGenerator::~GFakeDBIDGenerator(void)
{
}

CID GFakeDBIDGenerator::NewCID()
{
	m_nCID++;
	return m_nCID;
}