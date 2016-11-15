#include "stdafx.h"
#include "TestDBTableStatus.h"
#include "MXml.h"
#include "DBTestGlobal.h"

TestDBTableStatus::TestDBTableStatus()
{

}


TestDBTableStatus::~TestDBTableStatus()
{
	Release();
}


bool TestDBTableStatus::Load(const char* szFileName)
{
	MXml mx;
	if (!mx.LoadFile(szFileName))
	{
		return false;
	}

	MXmlHandle		docHandle	= mx.DocHandle();
	MXmlElement*	pElement	= docHandle.FirstChild("maiet").FirstChildElement().Element();

	for (; NULL != pElement; pElement=pElement->NextSiblingElement())
	{
		if (0 == _stricmp("TABLE", pElement->Value()))
		{
			string strTName;

			if (!_Attribute(strTName, pElement, "name", &mx))
			{
				return false;
			}

			TDBTableDef* pDBTable = new TDBTableDef(strTName);

			MXmlElement* pColElement = pElement->FirstChildElement();
			for (; NULL != pColElement; pColElement = pColElement->NextSiblingElement())
			{
				if (0 == _stricmp("COLUMN", pColElement->Value()))
				{
					string strColName;
					string strDataType;

					_Attribute(strColName, pColElement, "name", &mx);
					_Attribute(strDataType, pColElement, "data_type", &mx);

					if (!pDBTable->AddColumn(strColName, strDataType))
					{
						return false;
					}
				}
			}

			m_DBTableVec.push_back(pDBTable);
		}		
	}

	return true;
}


TDBTableDef* TestDBTableStatus::GetTable(const string& strName)
{
	for (size_t i = 0; i < m_DBTableVec.size(); ++i)
	{
		if (strName == m_DBTableVec[i]->GetName())
		{
			return m_DBTableVec[i];
		}
	}

	return NULL;
}


void TestDBTableStatus::Release()
{
	for (size_t i = 0; i < m_DBTableVec.size(); ++i)
	{
		m_DBTableVec[i]->Release();
		delete m_DBTableVec[i];
	}

	m_DBTableVec.clear();
}