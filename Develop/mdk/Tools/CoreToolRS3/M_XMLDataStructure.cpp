#include "StdAfx.h"
#include "M_XMLDataStructure.h"
#include "C_XMLResourceList.h"

const std::vector<std::string>* CAttributeTypeExtraManager::GetStringTable( const char* _pSzTableName )
{
	STRING_TYPE_TABLE::iterator itr = m_mapTypeExtraTable.find(_pSzTableName);
	if (itr == m_mapTypeExtraTable.end())
	{
		return NULL;
	}

	return &(itr->second);
}

CAttributeTypeExtraManager::CAttributeTypeExtraManager()
{
	MXml kXml;
	if ( kXml.LoadFile(CXMLResourceList::GetReference().GetPorpertyListPath()))
	{
		MXmlElement* pRoot = kXml.Doc()->RootElement();
		if(pRoot == NULL)
			return;

		MXmlElement* pType = pRoot->FirstChildElement("TYPE");
		if(pType == NULL)
			return;

		MXmlElement* pExtensible = pType->FirstChildElement("EXTENSIBLE");
		if (pExtensible == NULL)
			return;

		MXmlElement* pTypeExtra = pExtensible->FirstChildElement("TYPE_EXTRA");
		while ( pTypeExtra != NULL )
		{
			std::string strTableId;
			_Attribute(strTableId, pTypeExtra, "id");
			_ASSERT(!strTableId.empty());

			std::vector< std::string > vecTable;
			MXmlElement* pCombo = pTypeExtra->FirstChildElement("COMBO");
			while (pCombo != NULL)
			{
				std::string strTableContent;
				_Attribute(strTableContent, pCombo, "id");
				vecTable.push_back(strTableContent);

				pCombo = pCombo->NextSiblingElement();
			}
			m_mapTypeExtraTable[strTableId] = vecTable;

			pTypeExtra = pTypeExtra->NextSiblingElement();
		}
	}
}

//#include <algorithm>
////////////////////////////////////////////////////////////////////////

double SAttributeAtXML::SetAttributeBool(const std::string& _pStrBool)
{
	if( _pStrBool == "true" )
		return 1;
	else if ( _pStrBool == "false" )
		return 0;
	else
		return -1;
}

const char* SAttributeAtXML::GetAttributeBool(float _fBool)
{
	if(_fBool == 0.f)
		return "false";
	else if ( _fBool == 1.f )
		return "true";
	else
		return "";
}

HTREEITEM SAttributeAtXML::UpdateMFCTreeItem(CTreeCtrl& _refTreeCtrl, HTREEITEM _hParentToUpdate, SAttributeAtXML* _pMultiSelected)
{
	char szBuffer[256];
	szBuffer[0] = '\0';

	if(_pMultiSelected != NULL)
	{
		if(m_strAttrName == "id")
		{
			int nID = (int)m_vGenericNum[0];
			//id가 같다면
			if( nID == (int)_pMultiSelected->m_vGenericNum[0] )
			{
				sprintf(szBuffer,"id : %d", nID);
			}
			else
			{
				std::string strRootName( _refTreeCtrl.GetItemText(_hParentToUpdate).GetString() );
				int find_token = strRootName.find(" :");
				std::string strPureRootName( strRootName.substr(0, find_token) );
				std::string strMultiSelectedId( strRootName.substr(find_token, strRootName.length()) );
				sprintf(szBuffer,", %d", nID);
				strMultiSelectedId = strMultiSelectedId + szBuffer;
				_refTreeCtrl.SetItemText(_hParentToUpdate, (strPureRootName + strMultiSelectedId).c_str() );
			}
		}
		else
		{
			CString cstrBeforeTreeInfo(_refTreeCtrl.GetItemText(_hParentToUpdate));
			m_hTreeItem = _hParentToUpdate;
			UpdateMFCTreeItem(_refTreeCtrl);
			CString cstrAfterTreeInfo(_refTreeCtrl.GetItemText(_hParentToUpdate));

			//같지 않다면
			if(cstrBeforeTreeInfo.Compare(cstrAfterTreeInfo) != 0)
			{
				sprintf(szBuffer,"%s : _", m_strAttrName.c_str());
				_refTreeCtrl.SetItemText(_hParentToUpdate, szBuffer);
			}
		}
	}
	else
	{
		switch(m_eAttrType)
		{
			case E_UINT:
			{
				if(m_vGenericNum[0] >= 0 )
				{
					sprintf(szBuffer,"%s : %d", m_strAttrName.c_str(), (int)(m_vGenericNum[0]));
					if(_hParentToUpdate == NULL)
					{
						_refTreeCtrl.SetItemText(m_hTreeItem, szBuffer);
						return NULL;
					}
					else
					{
						m_hTreeItem = _refTreeCtrl.InsertItem( szBuffer, _hParentToUpdate, TVI_LAST);
						return m_hTreeItem;
					}
				}
				// Empty
				else if (m_vGenericNum[0] == SYM_EMPTY)
				{
					_refTreeCtrl.SetItemText(m_hTreeItem, "지워진 속성");
					return NULL;
				}
				// none symbol 추가
				else if( m_vGenericNum[0] == SYM_NONE )
				{
					sprintf(szBuffer,"%s : none", m_strAttrName.c_str());
					if(_hParentToUpdate == NULL)
					{
						_refTreeCtrl.SetItemText(m_hTreeItem, szBuffer);
						return NULL;
					}
					else
					{
						m_hTreeItem = _refTreeCtrl.InsertItem( szBuffer, _hParentToUpdate, TVI_LAST);
						return m_hTreeItem;
					}
				}
			}
			break;
			case E_FLOAT:
			{
				sprintf(szBuffer,"%s : %.2f", m_strAttrName.c_str(), m_vGenericNum[0]);
				if(_hParentToUpdate == NULL)
				{
					_refTreeCtrl.SetItemText(m_hTreeItem, szBuffer);
					return NULL;
				}
				else
				{
					m_hTreeItem = _refTreeCtrl.InsertItem( szBuffer, _hParentToUpdate, TVI_LAST);
					return m_hTreeItem;
				}
			}
			break;
			case E_BOOL:
			{
				float fBool = m_vGenericNum[0];
				if(fBool != -1.f)
				{
					sprintf(szBuffer,"%s : %s", m_strAttrName.c_str(), GetAttributeBool(fBool));
					if(_hParentToUpdate == NULL)
					{
						_refTreeCtrl.SetItemText(m_hTreeItem, szBuffer);
						return NULL;
					}
					else
					{
						m_hTreeItem = _refTreeCtrl.InsertItem( szBuffer, _hParentToUpdate, TVI_LAST);
						return m_hTreeItem;
					}
				}
			}
			break;
			case E_STRING:
			{
				if(!m_strGenericString.empty() )
				{
					sprintf(szBuffer,"%s : %s", m_strAttrName.c_str(), m_strGenericString.c_str());
					if(_hParentToUpdate == NULL)
					{
						_refTreeCtrl.SetItemText(m_hTreeItem, szBuffer);
						return NULL;
					}
					else
					{
						m_hTreeItem = _refTreeCtrl.InsertItem( szBuffer, _hParentToUpdate, TVI_LAST);
						return m_hTreeItem;
					}
				}
			}
			break;
			case E_RADIUS:
			{
				sprintf(szBuffer,"%s : %.2f", m_strAttrName.c_str(), m_vGenericNum[0]);
				if(_hParentToUpdate == NULL)
				{
					_refTreeCtrl.SetItemText(m_hTreeItem, szBuffer);
					return NULL;
				}
				else
				{
					m_hTreeItem = _refTreeCtrl.InsertItem( szBuffer, _hParentToUpdate, TVI_LAST);
					return m_hTreeItem;
				}
			}
			break;
			case E_POS:
			case E_DIR:
			case E_UP:
			{
				sprintf(szBuffer,"%s : %.2f, %.2f, %.2f", m_strAttrName.c_str(), m_vGenericNum[0], m_vGenericNum[1], m_vGenericNum[2]);
				if(_hParentToUpdate == NULL)
				{
					_refTreeCtrl.SetItemText(m_hTreeItem, szBuffer);
					return NULL;
				}
				else
				{
					m_hTreeItem = _refTreeCtrl.InsertItem( szBuffer, _hParentToUpdate, TVI_LAST);
					return m_hTreeItem;
				}
			}
			break;
			//case E_POS:
			//{
			//	sprintf(szBuffer,"%s : %.1f, %.1f, %.1f", m_strAttrName.c_str(), m_vGenericNum[0], m_vGenericNum[1], m_vGenericNum[2]);
			//	if(_hParentToUpdate == NULL)
			//	{
			//		_refTreeCtrl.SetItemText(m_hTreeItem, szBuffer);
			//		return NULL;
			//	}
			//	else
			//	{
			//		m_hTreeItem = _refTreeCtrl.InsertItem( szBuffer, _hParentToUpdate, TVI_LAST);
			//		return m_hTreeItem;
			//	}
			//}
			//break;
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////

void SElementAtXML::DestroyElement()
{
	//Delete Attribute
	if(!m_vecAttribute.empty())
	{
		for(AttributeList::iterator itr = m_vecAttribute.begin(); itr != m_vecAttribute.end(); ++itr)
			delete (*itr);
	}

	//Delete Element
	if(m_vecChildElement.empty())
	{
		delete this;
	}
	else
	{
		for(ElementList::iterator itr = m_vecChildElement.begin(); itr != m_vecChildElement.end(); ++itr)
			(*itr)->DestroyElement();

		delete this;
	}

}

SElementAtXML::SElementAtXML(void)
{
	m_pdElementID = NULL;
}
SElementAtXML::~SElementAtXML(void)
{
}
