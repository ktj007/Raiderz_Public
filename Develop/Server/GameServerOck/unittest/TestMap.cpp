#include "stdafx.h"
#include "SUnitTest.h"
#include "GSharedField.h"
#include "GPathFinder.h"
#include "MPath.h"
#include "MAgent.h"
#include "GGlobal.h"
#include "GFieldMgr.h"

SUITE(Map)
{
	TEST3(TestTeressPlainPick)
	{
		const int TERESS_PLANE_FIELD_ID = 107;
		GSharedField* pTeressPlane = gmgr.pFieldMgr->GetSharedField(TERESS_PLANE_FIELD_ID, 0);
		if (pTeressPlane == NULL) return;	// 테스트로 필드를 로딩안 할 수도 있다.

		vec3 vOrigin = vec3(30000.0f, 30000.0f, 10000.0f);
		vec3 vTo = vec3(30000.0f, 30000.0f, -500.0f);
		vec3 vPickPos = vec3::ZERO;

		CHECK(pTeressPlane->Pick(vOrigin, vTo, &vPickPos) == true);

		vOrigin = vec3(17000.0f ,37000.0f, -2000000.0f);
		vTo = vec3(17000.0f, 37000.0f ,-2100000.0f);
		vPickPos = vec3::ZERO;

		CHECK(pTeressPlane->Pick(vOrigin, vTo, &vPickPos) == false);
	}



	TEST3(TestRenPath)
	{
		return;

		const int ZONE_ID = 110;
		GField* pField = gmgr.pFieldMgr->GetSharedField(ZONE_ID, 0);
		if (pField == NULL) return;

		MAgent agent;
		vec3 vStart, vEnd;
		MPath outPath;

		vStart = vec3(28603.00f, 34878.00f, 1503.92f);
		vEnd = vec3(25522.00f, 32714.00f, 1110.87f);

		UNITTEST_TIME_CONSTRAINT(200);
//		pField->GetGroundVerticalPos(vStart);

		bool bResult = gsys.pPathFinder->NewPlaceAgent(agent, vStart, ZONE_ID, pField->GetPathMesh());

		//float height = 0.0f;
		//PATHENGINE_RESULT_TYPE nPathResultType =
		//	gsys.pPathFinder->IsValidPosition(&height,
		//		ZONE_ID, pField->GetPathContext(), vEnd, strShape);
		//		CHECK_EQUAL(nPathResultType, PATH_SUCCESS);

		GPathFinder::QueryResult nResult = gsys.pPathFinder->FindPath(outPath, pField->GetPathMesh(), agent, vEnd);
		CHECK(nResult != GPathFinder::QUERY_FAILED);


	}

#if 0
	TEST3(TestRenPath3)
	{
		const int ZONE_ID = 110;
		GField* pField = gmgr.pFieldMgr->GetSharedFieldChannel(ZONE_ID, 0);
		if (pField == NULL) return;

		SPathMesh* pPMesh = gsys.pPathFinder->GetMPathFinder().GetMesh(ZONE_ID);
		iMesh* mesh = pPMesh->m_pMesh;
		iShape* shape = gsys.pPathFinder->GetMPathFinder().GetShape(wstring("small"));
		MContext Context;

		gsys.pPathFinder->NewContext(Context, ZONE_ID);

		vector<MAgent*>	vecAgents;
		size_t index = -1;

		//FILE* fp;
		//if( fopen_s( &fp, "agents.txt", "r" ) == 0 )
		//{
		//	char szBuf[1024] = "";

		//	int i = 0;
		//	while ( fgets( szBuf, 1024, fp ) != NULL)
		//	{
		//		cPosition pos;
		//		sscanf_s(szBuf, "%d %d %d", &pos.x, &pos.y, &pos.cell);

		//		MAgent* agent = new MAgent();
		//		vecAgents.push_back(agent);

		//		agent->m_pAgent = mesh->placeAgent(shape, pos);
		//		Context.AddAgent(*agent);
		//		
		//		agent->m_pAgent->moveTo(pos);
		//	}
		//	fclose( fp );
		//}

		//for (size_t i = 0; i < vecAgents.size(); i++)
		//{
		//	cPosition pos = vecAgents[i]->m_pAgent->getPosition();
		//	if (pos.x == 45894 && pos.y == 13922 && pos.cell == 618)
		//	{
		//		index = i;
		//		break;
		//	}
		//}
		//CHECK(index != -1);

		{
			vec3 vPos = vec3(45460.0f, 13678.0f, 0.0f);

			//vPos = vec3(45731.0f, 13810.0f, 0.0f);

			pField->GetGroundVerticalPos(vPos);
			vPos.z += 100.0f;

			cPosition cPos;

			cPos = mesh->positionFor3DPointF((const float*)&vPos);

			MAgent* agent = new MAgent();
			vecAgents.push_back(agent);

			agent->m_pAgent = mesh->placeAgent(shape, cPos);
			Context.AddAgent(*agent);

			agent->m_pAgent->moveTo(cPos);
		}
		{
			vec3 vPos = vec3(45894.0f, 13922.0f, 0.0f);
			pField->GetGroundVerticalPos(vPos);
			vPos.z += 100.0f;

			cPosition cPos;

			cPos = mesh->positionFor3DPointF((const float*)&vPos);

			MAgent* agent = new MAgent();
			vecAgents.push_back(agent);

			agent->m_pAgent = mesh->placeAgent(shape, cPos);
			Context.AddAgent(*agent);

			agent->m_pAgent->moveTo(cPos);
		}

		index = 1;

		MPath path;
		vec3 vGoalPos = vec3(45204.72f, 13312.31f, -76.92f);
		vGoalPos = vec3(45280.0f, 13422.0f, -191.0f);
		bool bFind = gsys.pPathFinder->FindPath(path, ZONE_ID, *(vecAgents[index]), vGoalPos, wstring("small"));
		CHECK(bFind == true);

		for (size_t i = 0; i < vecAgents.size(); i++)
		{
			delete vecAgents[i];
		}
		vecAgents.clear();
	}


	TEST3(TestRenPath4)
	{
		const int ZONE_ID = 110;
		GField* pField = gmgr.pFieldMgr->GetSharedFieldChannel(ZONE_ID, 0);
		if (pField == NULL) return;

		SPathMesh* pPMesh = gsys.pPathFinder->GetMPathFinder().GetMesh(ZONE_ID);
		iMesh* mesh = pPMesh->m_pMesh;
		iShape* shape = gsys.pPathFinder->GetMPathFinder().GetShape(wstring("small"));
		MContext Context;

		gsys.pPathFinder->NewContext(Context, ZONE_ID);

		vector<MAgent*>	vecAgents;
		size_t index = -1;

		{
			vec3 vPos = vec3(47665.0f, 16258.0f, 0.0f);

			//vPos = vec3(45731.0f, 13810.0f, 0.0f);

			pField->GetGroundVerticalPos(vPos);
			vPos.z += 100.0f;

			cPosition cPos;

			cPos = mesh->positionFor3DPointF((const float*)&vPos);

			MAgent* agent = new MAgent();
			vecAgents.push_back(agent);

			agent->m_pAgent = mesh->placeAgent(shape, cPos);
			Context.AddAgent(*agent);

			agent->m_pAgent->moveTo(cPos);
		}
		{
			vec3 vPos = vec3(46642.0f, 15244.0f, -380.0f);
			pField->GetGroundVerticalPos(vPos);
			vPos.z += 100.0f;

			cPosition cPos;

			cPos = mesh->positionFor3DPointF((const float*)&vPos);

			MAgent* agent = new MAgent();
			vecAgents.push_back(agent);

			agent->m_pAgent = mesh->placeAgent(shape, cPos);
			Context.AddAgent(*agent);

			agent->m_pAgent->moveTo(cPos);
		}

		index = 0;

		MPath path;
		vec3 vGoalPos = vec3(46281.26f, 14894.86f, -232.22f);

		bool bFind = gsys.pPathFinder->FindPath(path, ZONE_ID, *(vecAgents[index]), vGoalPos, wstring("small"));
		CHECK(bFind == true);

		for (size_t i = 0; i < vecAgents.size(); i++)
		{
			delete vecAgents[i];
		}
		vecAgents.clear();
	}
#endif

}
