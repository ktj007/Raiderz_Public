#include "StdAfx.h"
#include "GAIRoomMgr.h"
#include "GFieldInfoLoader.h"

GAIRoomMgr::GAIRoomMgr(void)
{
}

GAIRoomMgr::~GAIRoomMgr(void)
{
}

bool GAIRoomMgr::IsValid( int nOwnerGroup, vec3 vTargetPos )
{
	if (nOwnerGroup == INVALID_ID)
		return true; // 소속된 그룹이 없음

	if (!HasRoom(nOwnerGroup))
		return true;

	map<int, Item>::iterator it = m_mapItems.find(nOwnerGroup);
	Item& item = it->second;
	if (item.boxArea.IsOut(vTargetPos))
		return false; // AI Room 영역을 벗어났음

	return true;
}

void GAIRoomMgr::AddItem( int nGroup, Item item )
{
	RVALID(m_mapItems.find(nGroup) == m_mapItems.end()); // 이미 존재하는 그룹
	m_mapItems.insert(std::make_pair(nGroup, item));
}

bool GAIRoomMgr::Load( wstring strFielname )
{
	MXml kXml;
	if ( !kXml.LoadFile(MLocale::ConvUTF16ToAnsi(strFielname.c_str()).c_str()) )
		return true;

	MXmlHandle docHandle = kXml.DocHandle();
	MXmlElement* pElement = docHandle.FirstChild(FIELD_XML_TAG_MAIET).FirstChildElement().Element();

	for( pElement; pElement != NULL; pElement=pElement->NextSiblingElement() )
	{
		if (!_stricmp(pElement->Value(), FIELD_XML_TAG_ROOM))
		{
			int nID = INVALID_ID;
			_Attribute(&nID,		pElement, FIELD_XML_ATTR_ID);
			DCHECK(nID != INVALID_ID);
			if (nID == INVALID_ID)
				continue;

			float fWidth = 0.0f;
			float fDepth = 0.0f;
			float fHeight = 0.0f;
			float fRotate = 0.0f;
			vec3 vPos;
			_Attribute(&vPos.x,		pElement, FIELD_XML_ATTR_ROOM_X);
			_Attribute(&vPos.y,		pElement, FIELD_XML_ATTR_ROOM_Y);
			_Attribute(&vPos.z,		pElement, FIELD_XML_ATTR_ROOM_Z);
			_Attribute(&fWidth,		pElement, FIELD_XML_ATTR_ROOM_WIDTH);
			_Attribute(&fDepth,		pElement, FIELD_XML_ATTR_ROOM_DEPTH);
			_Attribute(&fHeight,		pElement, FIELD_XML_ATTR_ROOM_HEIGHT);
			_Attribute(&fRotate,		pElement, FIELD_XML_ATTR_ROOM_ROTATE);

			
			MMatrix m;
			m.SetRotationZ(MMath::DegreeToRadian(fRotate));
			vPos.x += fWidth/2;
			vPos.y += fDepth/2;
			vPos.z += fHeight/2;
			GAIRoomMgr::Item item;
			item.boxArea = 
				MOBox(	MVector3(m._11,m._21,m._23),
						MVector3(m._12,m._22,m._32), 
						MVector3(m._13,m._23,m._33), 
						vec3(fWidth/2,fDepth/2,fHeight/2), 
						vPos);

			AddItem(nID, item);
		}
	}

	return true;
}

bool GAIRoomMgr::HasRoom( int nGroup ) const
{
	map<int, Item>::const_iterator it = m_mapItems.find(nGroup);
	return (it != m_mapItems.end()); // 존재하지 않는 그룹	
}