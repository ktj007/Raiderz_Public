#include "stdafx.h"
#include "TestDBTable.h"


TDBTableDef::TDBTableDef(const string& strName) : m_strName(strName)
{

}


TDBTableDef::~TDBTableDef()
{
	Release();
}


bool TDBTableDef::AddColumn(const string& strName
						 , const string& strType)
{
	TDBColumnDef* pDBColumn = new TDBColumnDef();
	if (NULL == pDBColumn)
	{
		return false;
	}

	pDBColumn->strName	= strName;
	pDBColumn->strType	= strType;
	
	m_ColumnVec.push_back(pDBColumn);	

	return true;
}


void TDBTableDef::Release()
{
	for (size_t i = 0; i < m_ColumnVec.size(); ++i)
	{
		delete m_ColumnVec[i];
	}
	
	m_ColumnVec.clear();
}


bool TDBTableDef::operator ==(TDBTableDef& Dest)
{
	return operator==(&Dest);
}


bool TDBTableDef::operator ==(TDBTableDef* pDest)
{
	if (NULL == pDest) return false;

	if (pDest->GetName() != m_strName) 
	{
		mlog("DBTableDef Name: %s != %s\n", pDest->GetName().c_str(), m_strName.c_str());

		return false;
	}
	if (pDest->m_ColumnVec.size() != m_ColumnVec.size()) 
	{
		mlog("DBTableDef(%s) column size: %u != %u\n", m_strName.c_str(), pDest->m_ColumnVec.size(), m_ColumnVec.size());
		return false;
	}

	TDBColumnDef* c1 = NULL;
	TDBColumnDef* c2 = NULL;
	for (size_t i = 0; i < m_ColumnVec.size(); ++i)
	{
		c1 = m_ColumnVec[i];
		c2 = pDest->m_ColumnVec[i];

		if (c1->strName != c2->strName) 
		{
			mlog("DBTableDef(%s) column name: %s != %s\n", m_strName.c_str(), c1->strName.c_str(), c2->strName.c_str());
			return false;
		}
		else if (c1->strType != c2->strType) 
		{
			mlog("DBTableDef(%s) column(%s) type: %s != %s\n", c1->strName.c_str(), m_strName.c_str(), c1->strType.c_str(), c2->strType.c_str());
			return false;
		}
	}

	return true;
}


TDBColumnDef* TDBTableDef::GetColumn(const string& strName)
{
	for (size_t i = 0; i < m_ColumnVec.size(); ++i)
	{
		if (strName == m_ColumnVec[i]->strName)
		{
			return m_ColumnVec[i];
		}
	}

	return NULL;
}


TDBColumnDef* TDBTableDef::GetColumn(const size_t nIndex)
{
	if (nIndex >= m_ColumnVec.size())
	{
		return NULL;
	}

	return m_ColumnVec[nIndex];
}