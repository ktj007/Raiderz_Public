#pragma once

#include "M_LogicObject.h"
#include <list>
#include <map>
#include <string>

class CEntityObject;
class CControlMediator;

typedef std::map< std::string, std::string> SOUNDFILELIST;

struct SNpcInfo
{
	SNpcInfo(const string& strNpcMeshName, const string& strNpcType, float fScale, DWORD dwColor)
		: m_strNpcMeshName(strNpcMeshName)
		, m_strNpcType(strNpcType)
		, m_fScale(fScale)
		, m_dwColor(dwColor)
	{
	}
	std::string m_strNpcMeshName;
	std::string m_strNpcType;
	float		m_fScale;
	DWORD		m_dwColor;
};

class CORETOOL_API CToolLogicObjectManager
{
public:

	// for entity
	template< typename T_EntityType >
	void InitLogicObjectManager()
	{
		_ASSERT(m_listEntityObject_.empty());
		int nObjNum = (int)T_EntityType::ENTITY_OBJ_NUM;
		m_listEntityObject_.resize(nObjNum);
	}

	template <typename T_EntityType >
	ENTITY_OBJECTLIST* GetEntityObjectList(typename T_EntityType::TYPE _type)
	{
		int nObjNum = (int)T_EntityType::ENTITY_OBJ_NUM;
		int nListObjectSize = (int)m_listEntityObject_.size();
		if ( nObjNum != nListObjectSize)
		{
			_ASSERT(0);
			return NULL;
		}

		return &m_listEntityObject_[_type];
	}

	template <typename T_EntityType >
	void SortEntityObject(typename T_EntityType::TYPE _type)
	{
		std::list<int> listSort;
		ENTITY_OBJECTLIST* plistEntityObject = &m_listEntityObject_[_type];
		ENTITY_OBJECTLIST listCopyEntityObject = m_listEntityObject_[_type];

		for(ENTITY_OBJECTLIST::iterator itr = plistEntityObject->begin(); itr != plistEntityObject->end(); ++itr)
		{
			listSort.push_back((*itr)->GetObjectID());
		}

		listSort.sort();
		plistEntityObject->clear();

		std::list<int>::iterator preSortitr = listSort.begin();

		for(std::list<int>::iterator sortitr = listSort.begin(); sortitr != listSort.end(); )
		{
			//EntityObject ID 순으로 채움
			for(ENTITY_OBJECTLIST::iterator objectitr = listCopyEntityObject.begin(); objectitr != listCopyEntityObject.end(); ++objectitr)
			{
				if(*sortitr == (*objectitr)->GetObjectID())
				{
					plistEntityObject->push_back(*objectitr);
					listCopyEntityObject.erase(objectitr);
					break;
				}
			}

			++sortitr;

			//ID 중복 검사
			if( (sortitr != listSort.end()) && (*preSortitr) == (*sortitr))
			{
				RepeatIDMessage(*sortitr);
				
			}
			preSortitr = sortitr;
		}
	}

	std::vector<ENTITY_OBJECTLIST>& GetEntityObjectRepository(){ return m_listEntityObject_; }

	void						ResetLogicObjectManager();

	void						LoadTablesSafely();
	SElementAtXML*				GetPropertyTable(const char* _pSzName);
	SNpcInfo*					GetNpcInfoFromId(int _id);

	void						PickLogicObjectFrustumForSelection(std::vector<CLogicObject*>& _refVecLogicObject, rs3::RViewFrustum& _refFrustum);
	CLogicObject*				PickLogicObjectForSelection(const rs3::RVector &origin, const rs3::RVector &to);

	void						AddToManager(CLogicObject* _pLogicObject);
	void						RemoveFromManager(CLogicObject* _pLogicObject);

	inline LOGIC_OBJECTLIST&	GetNoneEntityObjectList(){ return m_listNoneEntityObject_; }
	const SOUNDFILELIST&		GetBGMList()	{	return m_mapBGMList; }

	const char*					GetUniqueObjectNameOf( const char* szReferenceName );

	void						ReLoadPropertyAndNpcTable_();

private:
	void						RepeatIDMessage(int nID);

	void						LoadPropertyAndNpcTable_();
	void						LoadSoundEffectTable_();

	void						UpdateElement_(MXmlElement* _pXmlElement, SElementAtXML* _pLogicObjectElement);

	typedef						std::map< std::string, SElementAtXML*> MAP_PROPERTY;
	MAP_PROPERTY				m_mapPropertyTable_;
	typedef						std::map< int, SNpcInfo > MAP_NPCINFO;
	MAP_NPCINFO					m_mapNpcIdInfo_;

	SOUNDFILELIST					m_mapBGMList;
	std::vector<ENTITY_OBJECTLIST>	m_listEntityObject_;
	//EntityObjectList				m_listEntityObject_[CEntityObject::ENTITY_OBJ_NUM];
	LOGIC_OBJECTLIST				m_listNoneEntityObject_;

	friend class CControlMediator;
	CToolLogicObjectManager(CControlMediator* _pControlMediator);
	~CToolLogicObjectManager(void);
	CControlMediator* m_pControlMediator_;
};
