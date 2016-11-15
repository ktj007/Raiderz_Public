#include "stdafx.h"

#include "EffectList.h"

EffectList::EffectList(void)
{
}

EffectList::~EffectList(void)
{
	while(!empty())
	{
		delete back();
		pop_back();
	}
}

string EffectList::RemoveExtension(const string& strEffectName)
{
	string strReturn;
	// effect.xml 이나 effect.scene.xml 붙어있는것 제거
	const char* EFFECTXML = ".effect.xml";
	if( stricmp(EFFECTXML , strEffectName.substr(strEffectName.length() - strlen(EFFECTXML) ).c_str() ) == 0 )
		strReturn = strEffectName.substr( 0, strEffectName.length() - strlen(EFFECTXML) );

	const char* EFFECTSCENEXML = ".effect.scene.xml";
	if( stricmp(EFFECTSCENEXML , strEffectName.substr(strEffectName.length() - strlen(EFFECTSCENEXML) ).c_str() ) == 0 )
		strReturn = strEffectName.substr( 0, strEffectName.length() - strlen(EFFECTSCENEXML) );

	return strReturn;
}

void EffectList::Add(RMaxMeshNode* pParentMaxMeshNode, const string& strNodeName, const string& strEffectName, bool bStatic, float fScale)
{
	string strRefinedEffectName = RemoveExtension( strEffectName );
	EFFECTDESC *pEffect = new EFFECTDESC;
	pEffect->pParentMaxMeshNode = pParentMaxMeshNode;
	pEffect->parentNodeName = strNodeName;
	pEffect->effectFileName = strRefinedEffectName;
	pEffect->position = MVector3(0,0,0);
	pEffect->direction = MVector3(0,1,0);
	pEffect->scale = MVector3(fScale,fScale,fScale);
	pEffect->bStatic = bStatic;
	push_back( pEffect );
}

// void EffectList::Add(const string& strEffectName, const MVector3& pos, const MVector3& dir) // 특정 위치에 달릴때
// {
// 	EFFECTDESC *pEffect = new EFFECTDESC;
// 	pEffect->parentNodeName = "";
// 	pEffect->effectFileName = strEffectName;
// 	pEffect->position = pos;
// 	pEffect->direction = dir;
// 	push_back( pEffect );
// }


void EffectList::Save(MXmlElement* pElement, bool bStatic)
{
	char szContents[256];

	for(iterator i=begin();i!=end();i++)
	{
		EFFECTDESC* pEffect = *i; 
		if( pEffect->bStatic != bStatic ) continue;

		MXmlElement* pEffectElement = new MXmlElement("EFFECTINSTANCE");
		pElement->LinkEndChild(pEffectElement);

		//////////////////////////////////////////////////////////////////////////
		// common
		MXmlElement* pCommonElement = new MXmlElement("COMMON");
		pEffectElement->LinkEndChild(pCommonElement);

		if( bStatic )
		{
			MXmlElement *pPositionElement = new MXmlElement("POSITION");

			// charmke: StaticMesh익스포트 시 월드 좌표로 익스포트 한다.
			// StaticMesh가 원점이 아니면 위치가 안 맞을 듯.
			D3DXMATRIX& mat = pEffect->pParentMaxMeshNode->m_matBase;
			RVector pos( mat._41, mat._42, mat._43 );
			pos += pEffect->position;

			sprintf(szContents,"%f %f %f", pos.x, pos.y, pos.z);
			_SetContents(pPositionElement, szContents);

			pCommonElement->LinkEndChild(pPositionElement);
		}
		else
		{
			// position
			MXmlElement *pPositionElement = new MXmlElement("POSITION");
			sprintf(szContents,"%f %f %f", pEffect->position.x, pEffect->position.y, pEffect->position.z);
			_SetContents(pPositionElement, szContents);
			pCommonElement->LinkEndChild(pPositionElement);
		}

		MXmlElement *pDirectionElement = new MXmlElement("DIRECTION");
		sprintf(szContents,"%f %f %f", pEffect->direction.x, pEffect->direction.y, pEffect->direction.z);
		_SetContents(pDirectionElement, szContents);
		pCommonElement->LinkEndChild(pDirectionElement);

		MXmlElement *pScaleElement = new MXmlElement("SCALE");
		sprintf(szContents,"%f %f %f", pEffect->scale.x, pEffect->scale.y, pEffect->scale.z);
		_SetContents(pScaleElement, szContents);
		pCommonElement->LinkEndChild(pScaleElement);

		//////////////////////////////////////////////////////////////////////////
		// controller
		if( !bStatic ) // actornode constraint controller
		{
			// CONSTRAINT
			MXmlElement *pControllers = new MXmlElement("CONTROLLERS");
			pEffectElement->LinkEndChild( pControllers );

			MXmlElement *pActorNodeConstraintController = new MXmlElement("ACTORNODECONSTRAINTCONTROLLER");
			pControllers->LinkEndChild( pActorNodeConstraintController );

			MXmlElement *pTarget = new MXmlElement("TARGET");
			_SetContents( pTarget, pEffect->parentNodeName );
			pActorNodeConstraintController->LinkEndChild( pTarget );
		}

		//////////////////////////////////////////////////////////////////////////
		// property
		MXmlElement* pPropertyElement = new MXmlElement("PROPERTY");
		pEffectElement->LinkEndChild(pPropertyElement);

		MXmlElement* pFileName= new MXmlElement("FILENAME");
		pPropertyElement->LinkEndChild(pFileName);
		_SetContents(pFileName,pEffect->effectFileName);

		// 자동시작이 아닐 필요가 있으면 따로 처리해주자, 현재는 항상 자동시작
		MXmlElement* pAutoStart = new MXmlElement("AUTOSTART");
		pPropertyElement->LinkEndChild(pAutoStart);
	}
}