#include "stdafx.h"
#include "RToken.h"

namespace rs3 {

const RVector2 RUVTransform::DEFAULT_SCALE(1.0f, 1.0f);
const RVector2 RUVTransform::DEFAULT_OFFSET(0.0f, 0.0f);


RUVAnimationInfo::RUVAnimationInfo() :
m_uOffsetIter(0),
m_uScaleIter(0),
m_uRotateIter(0)
{
}

RUVAnimationInfo::~RUVAnimationInfo()
{

}

RUVTransform RUVAnimationInfo::Open(MXmlElement *pElement)
{
	char szTagName[256],szContents[256];

	// offset track을 읽는다.
	vector<RVec3Key> vecOffsetTrack;
	vector<RVec3Key> vecScaleTrack;
	vector<RVec3Key> vecRotateTrack;

	for ( MXmlElement *pTrackElement = pElement->FirstChildElement();
		pTrackElement != NULL; pTrackElement = pTrackElement->NextSiblingElement())
	{
		RVec3Key vFrame;
		vFrame.frame = 0;
		vFrame.x = vFrame.y = vFrame.z = 0.f;

		strcpy(szTagName, pTrackElement->Value());
		if ( szTagName[0] == NULL )		continue;

		pTrackElement->Attribute("time",&vFrame.frame);

		bool bResult = false;
		bResult = _Contents(szContents, pTrackElement);
		_ASSERT(bResult);

		if ( _stricmp(szTagName,RTOK_TEX_OFFSET) == 0 )
		{
			sscanf(szContents,"%f %f",&vFrame.x,&vFrame.y);
			vecOffsetTrack.push_back(vFrame);
		}
		else if ( _stricmp(szTagName,RTOK_TEX_SCALE) == 0 )
		{
			sscanf(szContents,"%f %f",&vFrame.x,&vFrame.y);
			vecScaleTrack.push_back(vFrame);
		}
		else if ( _stricmp(szTagName,RTOK_TEX_ROTATE) == 0 )
		{
			sscanf(szContents,"%f %f %f",&vFrame.x,&vFrame.y,&vFrame.z);
			vecRotateTrack.push_back(vFrame);
		}
	}

	COPY_IF_EXIST ( m_offsetKeyTrack, vecOffsetTrack);
	COPY_IF_EXIST ( m_scaleKeyTrack, vecScaleTrack);
	COPY_IF_EXIST ( m_rotateKeyTrack, vecRotateTrack);

	RVector vTranslate(0, 0, 0);
	RVector vScale(1, 1, 1);

	if ( !m_offsetKeyTrack.empty())
	{
		vTranslate = m_offsetKeyTrack.front();
	}

	if ( !m_scaleKeyTrack.empty())
	{
		vScale = m_scaleKeyTrack.front();
	}

	RUVTransform UVTransform;
	UVTransform.SetOffset(RVector2(vTranslate.x, vTranslate.y));
	UVTransform.SetScale(RVector2(vScale.x, vScale.y));

	return UVTransform;
}

void RUVAnimationInfo::Save(MXmlElement *pElement)
{
	MXmlElement *pAnimation = new MXmlElement(RTOK_TEX_ANIMATION);
	pElement->LinkEndChild(pAnimation);

	char buffer[256]={0};

	for ( int i = 0; i != m_offsetKeyTrack.size(); ++i )
	{
		RVec3Key key = m_offsetKeyTrack.at(i);
		MXmlElement *pOffsetAnimation = new MXmlElement(RTOK_TEX_OFFSET);
		sprintf(buffer,"%5.3f %5.3f", key.x, key.y);

		_SetAttribute(pOffsetAnimation,"time", key.frame);
		_SetContents(pOffsetAnimation, buffer);
		pAnimation->LinkEndChild(pOffsetAnimation);
	}
	for ( int i = 0; i != m_scaleKeyTrack.size(); ++i )
	{
		RVec3Key key = m_scaleKeyTrack.at(i);
		MXmlElement *pScaleAnimation = new MXmlElement(RTOK_TEX_SCALE);
		sprintf(buffer,"%5.3f %5.3f", key.x, key.y);

		_SetContents(pScaleAnimation, buffer);
		_SetAttribute(pScaleAnimation,"time", key.frame);
		pAnimation->LinkEndChild(pScaleAnimation);
	}

	for ( int i = 0; i != m_rotateKeyTrack.size(); ++i )
	{
		RVec3Key key = m_rotateKeyTrack.at(i);
		MXmlElement *pRotateAnimation = new MXmlElement(RTOK_TEX_ROTATE);
		sprintf(buffer,"%5.3f %5.3f %5.3f", key.x, key.y, key.z);

		_SetContents(pRotateAnimation, buffer);
		_SetAttribute(pRotateAnimation,"time", key.frame);
		pAnimation->LinkEndChild(pRotateAnimation);
	}
}

bool RUVAnimationInfo::operator==(RUVAnimationInfo& src)
{
	if(m_offsetKeyTrack.size() != src.GetOffsetKeyTrack().size() ||
		m_scaleKeyTrack.size() != src.GetScaleKeyTrack().size() ||
		m_rotateKeyTrack.size() != src.GetRotateKeyTrack().size() )
		return false;

	for(size_t i=0; i<m_offsetKeyTrack.size(); ++i)
	{
		if(m_offsetKeyTrack.at(i) != src.GetOffsetKeyTrack().at(i))
			return false;
	}

	for(size_t i=0; i<m_scaleKeyTrack.size(); ++i)
	{
		if(m_scaleKeyTrack.at(i) != src.GetScaleKeyTrack().at(i))
			return false;
	}

	for(size_t i=0; i<m_rotateKeyTrack.size(); ++i)
	{
		if(m_rotateKeyTrack.at(i) != src.GetRotateKeyTrack().at(i))
			return false;
	}

	return true;
}

RUVTransform RUVAnimationInfo::GetUVTransform(UINT nFrame)
{
	RVec3Key vOffset;
	RVec3Key vScale;
	vOffset.x = vOffset.y = vOffset.z = 0.f;
	vScale.x = vScale.y = vScale.z = 1.f;

	if ( !m_scaleKeyTrack.empty())
		m_scaleKeyTrack.GetInterpolatedKey(&vScale, nFrame, m_uScaleIter);

	if ( !m_offsetKeyTrack.empty())
		m_offsetKeyTrack.GetInterpolatedKey(&vOffset, nFrame, m_uOffsetIter);

	if ( !m_rotateKeyTrack.empty())
	{
	}

	RUVTransform UVtransform;
	UVtransform.SetOffset(RVector2(vOffset.x, vOffset.y));
	UVtransform.SetScale(RVector2(vScale.x, vScale.y));

	return UVtransform;
}


}