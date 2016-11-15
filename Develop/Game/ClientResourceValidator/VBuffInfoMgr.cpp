#include "stdafx.h"
#include "VBuffInfoMgr.h"

VBuffInfoMgr::VBuffInfoMgr()
: XBuffInfoMgr()
{

}

VBuffInfoMgr::~VBuffInfoMgr()
{

}

void VBuffInfoMgr::ParseBuffInfo( MXmlElement* pElement, MXml* pXml )
{
	XBuffInfoMgr::ParseBuff(pElement, pXml);
}

void VBuffInfoMgr::ParseBuffEffectInfo( MXmlElement* pElement, MXml* pXml )
{
	XBuffInfoMgr::ParseBuffEffect(pElement, pXml);
}