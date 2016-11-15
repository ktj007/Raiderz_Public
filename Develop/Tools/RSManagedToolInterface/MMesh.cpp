#include "stdafx.h"
#include "MMesh.h"

using namespace RSManaged;
using namespace System;


MMesh::MMesh()
{
	m_pMesh = new rs3::RMesh();
}

MMesh::MMesh(rs3::RMesh* pMesh)
{
	m_pMesh = pMesh;
}

MMesh::~MMesh()
{
	m_pMesh = NULL;
}

bool MMesh::get_Valid()
{
	if (m_pMesh == NULL) return false;
	return true;
}

rs3::RMesh* MMesh::get_NativeMesh()
{
	return m_pMesh;
}
