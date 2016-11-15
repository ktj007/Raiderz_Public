#include "StdAfx.h"
#include "XMapSignAreaChecker.h"
#include "XGameState.h"
#include "XCharacter.h"

XMapSignAreaChecker::XMapSignAreaChecker(void)
{
	m_CheckRegulator.SetTickTime(3.0f);
}

XMapSignAreaChecker::~XMapSignAreaChecker(void)
{
}

void XMapSignAreaChecker::UpdateTouchAreaTrigger(XObject* pOwner, float fDelta )
{
	PFC_THISFUNC;

	if (pOwner == NULL) return;
	if (!m_CheckRegulator.IsReady(fDelta)) return;

	XFieldInfo* pFieldInfo = gg.currentgamestate->GetWorld()->GetInfo();
	if (pFieldInfo==NULL) return;

	RMatrix matWorld = pOwner->GetWorldTransform();
	vec3 vMyPos = pOwner->GetActor()->GetInteractionAABB().GetCenter();
	vec3 temppos = vec3(matWorld._41 + vMyPos.x, matWorld._42 + vMyPos.y, matWorld._43 + vMyPos.z);

	AREA_INFO_VECTOR& vecArea = pFieldInfo->m_Areas;
	for (AREA_INFO_VECTOR::iterator itor = vecArea.begin(); itor != vecArea.end(); ++itor)
	{
		AREA_INFO& info = (*itor);
		vector<XCylinder>&	veccylinder = info.veccylinder;

		if(veccylinder.size() == 0) continue;

		RBoundingBox bbox;
		bbox.Initialize();
		for (vector<XCylinder>::iterator itor2 = veccylinder.begin(); itor2 != veccylinder.end(); ++itor2)
		{
			XCylinder& Cylionderinfo = (*itor2);

			XCylinder cylinder;
			cylinder = Cylionderinfo;
			cylinder.radius += AREA_BUFFER_RADIUS;

			bbox.Add(cylinder.GetAABB());
		}

		if (!bbox.Contains(temppos))
		{
			info.bEnterLocal = false;
			continue;
		}

		bool bEnter = false;
		for (vector<XCylinder>::iterator itor2 = veccylinder.begin(); itor2 != veccylinder.end(); ++itor2)
		{
			XCylinder& Cylionderinfo = (*itor2);
			if (Cylionderinfo.CollisionPoint(temppos))
			{
				if (bEnter == false )
				{
					bEnter = true;
					break;
				}
			}
			else
			{
				XCylinder cylinder;
				cylinder = Cylionderinfo;
				cylinder.radius += AREA_BUFFER_RADIUS;
				if(!cylinder.CollisionPoint(temppos))
				{
					bEnter = false;
				}
				else
				{
					if (info.bEnterLocal == true)
					{
						bEnter = true;
						break;
					}
				}
			}
		}

		if (bEnter)
		{
			if(info.bEnterLocal == false)
			{
				info.bEnterLocal = true;

				if(gvar.Game.bGameStart == true)
				{
					global.script->GetGlueGameEvent().OnGameEvent( "MAPSIGN", MLocale::ConvUTF16ToAnsi(info.GetName()));
				}

				if(global.script) global.script->GetGlueFieldEvent().OnAreaEnter(pFieldInfo, info.nID);
			}
		}
		else
		{
			info.bEnterLocal = false;

			if(global.script) global.script->GetGlueFieldEvent().OnAreaLeave(pFieldInfo, info.nID);
		}
	}
}
