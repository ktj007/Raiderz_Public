#include "stdafx.h"
#include "ROccluderSceneNode.h"

namespace rs3 
{

	MImplementRTTI(ROccluderSceneNode, RSceneNode);

	ROccluderSceneNode::ROccluderSceneNode( RRENDERPASS eRenderPass ) 
		: RSceneNode(eRenderPass), m_pOccluderPointsSet(NULL), m_pWorldTransformedOccluderPointsSet(NULL)
	{
		m_LocalNonUniformScale = RVector3(1.f,1.f,1.f);
		SetAutoUpdate(true);
	}

	ROccluderSceneNode::~ROccluderSceneNode( void )
	{
		DestroyOccluder();
	}

	bool ROccluderSceneNode::CreateFromXML( MXmlElement &element, bool bBackgroundCreation )
	{
		_ASSERT( false == HasOccluderPointsSet() );

		//"LOCALSCALE"
		RVector3 LocalScale(1.f,1.f,1.f);
		char szBuffer[256];
		MXmlElement* pElement = element.FirstChildElement("LOCALSCALE");
		if ( pElement && _Contents(szBuffer, pElement) )
			sscanf(szBuffer,"%f %f %f",&LocalScale.x,&LocalScale.y,&LocalScale.z);
		SetLocalScale(LocalScale);
	
		//"OCCLUDERPOINT"
		for(	MXmlElement* pAnOccluderElement = element.FirstChildElement("OCCLUDERPOINT");
			pAnOccluderElement != NULL;
			pAnOccluderElement = pAnOccluderElement->NextSiblingElement("OCCLUDERPOINT")	
		 )
		{
			ROccluderPoints OccluderPoints;
			for(	MXmlElement* pAnOccluderPointElement = pAnOccluderElement->FirstChildElement("P");
				pAnOccluderPointElement != NULL ; 
				pAnOccluderPointElement = pAnOccluderPointElement->NextSiblingElement("P")
				)
			{
				char buffer[1024];
				_Contents(buffer,pAnOccluderPointElement);
				RVector3 Point;
				sscanf(buffer,"%f %f %f",&Point.x,&Point.y,&Point.z);
				OccluderPoints.AddPoint(Point);
			}
			AddOccluderPointsToOccluderPointsSet(OccluderPoints);
		}

		//AABB¼ÂÆÃÇØÁÜ
		SetAABB( GetOCcluderPointsSet()->GetAABB() );
		return true;
	}

	void ROccluderSceneNode::SaveToXML( MXmlElement &element )
	{
		//"LOCALSCALE"
		char szContents[256];
		MXmlElement *pElement = new MXmlElement("LOCALSCALE");
		sprintf(szContents,"%f %f %f", m_LocalNonUniformScale.x, m_LocalNonUniformScale.y, m_LocalNonUniformScale.z);
		_SetContents(pElement, szContents);
		element.LinkEndChild(pElement);

		//"OCCLUDERPOINT"
		char buffer[2048];
		ROccluderPointsSet* pOccluderPointSet = GetOCcluderPointsSet();
		if( NULL == pOccluderPointSet ) return;

		for(int i=0;i<(int)pOccluderPointSet->GetTheNumberOfOccludePoints();i++)
		{
			ROccluderPoints& OccluderPoints = pOccluderPointSet->GetOCcluderPoints(i);

			MXmlElement* pAnOccluderElement = new MXmlElement("OCCLUDERPOINT");
			element.LinkEndChild(pAnOccluderElement);	
			
			for(int j=0;j<(int)OccluderPoints.GetNumberOfPoints();j++)
			{
				MXmlElement* pOccluderPointElement = new MXmlElement("P");
				pAnOccluderElement->LinkEndChild(pOccluderPointElement);
				const MVector3& OccluderPoint = OccluderPoints.GetPoint(j);
				sprintf(buffer,"%f %f %f",OccluderPoint.x,OccluderPoint.y,OccluderPoint.z);
				_SetContents(pOccluderPointElement,buffer);
			}
		}
	}

	RSceneNode* ROccluderSceneNode::Clone()
	{
		ROccluderSceneNode* pNewNode = new ROccluderSceneNode;

		pNewNode->SetTransform(GetLocalTransform());
		pNewNode->SetScale(GetScale());
		pNewNode->SetAABB(GetAABB());

		pNewNode->SetLocalScale(GetLocalScale());
		pNewNode->m_pOccluderPointsSet = new ROccluderPointsSet;
		pNewNode->m_pWorldTransformedOccluderPointsSet = new ROccluderPointsSet;
		*(pNewNode->m_pOccluderPointsSet) = *(this->m_pOccluderPointsSet);
		*(pNewNode->m_pWorldTransformedOccluderPointsSet) = *(this->m_pWorldTransformedOccluderPointsSet);
		return pNewNode;
	}

	void ROccluderSceneNode::OnUpdateTransform()
	{
		UpdateWorldTransformedOccluderPointsSet(GetWorldTransform());
	}

	void ROccluderSceneNode::OnUpdateAABB()
	{
		if( GetWorldTransformedOccluderPointsSet() )
			SetAABB( GetWorldTransformedOccluderPointsSet()->GetAABB() );
	}

	void ROccluderSceneNode::AddOccluderPointsToOccluderPointsSet( const ROccluderPoints& OccluderPoints )
	{
		if(NULL == m_pOccluderPointsSet)
		{
			m_pOccluderPointsSet = new ROccluderPointsSet;
		}
		m_pOccluderPointsSet->AddOccluderPoints(OccluderPoints);
	}

	void ROccluderSceneNode::DestroyOccluder( void )
	{
		SAFE_DELETE(m_pOccluderPointsSet);
		SAFE_DELETE(m_pWorldTransformedOccluderPointsSet);
	}

	void ROccluderSceneNode::UpdateWorldTransformedOccluderPointsSet(const RMatrixA16 &WorldMatrix)
	{
		if( false == HasOccluderPointsSet() ) return;
		if( false == HasWorldTransformedOccluderPointsSet() )
		{
			m_pWorldTransformedOccluderPointsSet = new ROccluderPointsSet;
			*m_pWorldTransformedOccluderPointsSet = *GetOCcluderPointsSet();
		}

		_ASSERT( m_pOccluderPointsSet->GetTheNumberOfOccludePoints() == m_pWorldTransformedOccluderPointsSet->GetTheNumberOfOccludePoints() );

		for(unsigned int i=0;i<m_pOccluderPointsSet->GetTheNumberOfOccludePoints();i++)
		{
			const ROccluderPoints& OccluderPoints = m_pOccluderPointsSet->GetOCcluderPoints(i);
			for(unsigned int j=0;j<OccluderPoints.GetNumberOfPoints();j++)
			{
				RVector3 Out;
				WorldMatrix.TransformVect(OccluderPoints.GetPoint(j)*GetLocalScale(),Out);
				m_pWorldTransformedOccluderPointsSet->GetOCcluderPoints(i).SetPoint(Out,j);
			}
		}
	}

	ROccluderPointsSet* ROccluderSceneNode::GetWorldTransformedOccluderPointsSet( void )
	{	
		if( NULL == m_pWorldTransformedOccluderPointsSet )
			UpdateWorldTransformedOccluderPointsSet(GetWorldTransform());
		return m_pWorldTransformedOccluderPointsSet; 
	}
}