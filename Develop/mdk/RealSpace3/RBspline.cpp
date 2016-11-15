#include "StdAfx.h"
#include "RBspline.h"

namespace rs3 {

RBspline::RBspline(void)
:	fS	(0.01F)
,	m_fTime (0.0F)
,	bC(true)
,	iC( 0 )
,	m_fLifeTime( 0.f )
,	iN( 0 )
{
	int iNx	= 10000;

	pCrv = new RVector[iNx];
	pVtx = new RTestVertex[iNx];
	memset(pVtx, 0xAA, iNx * sizeof(RTestVertex));

	mtCd._11 =  -1;	mtCd._12 =  3;	mtCd._13 = -3;	mtCd._14 = 1;
	mtCd._21 =   3;	mtCd._22 = -6;	mtCd._23 =  3;	mtCd._24 = 0;
	mtCd._31 =  -3;	mtCd._32 =  0;	mtCd._33 =  3;	mtCd._34 = 0;
	mtCd._41 =   1;	mtCd._42 =  4;	mtCd._43 =  1;	mtCd._44 = 0;

}

RBspline::~RBspline(void)
{
	Destroy();
}

void RBspline::Destroy()
{
	SAFE_DELETE_ARRAY(	pCrv	);
	SAFE_DELETE_ARRAY(	pVtx	);

	//vPt.clear();
}

void RBspline::Generate(bool loop, std::vector<REmitterVariable> &vecVar)
{
	bC = loop;

	vPt.clear();
	for( int i = 0; i < (int)vecVar.size(); ++i)
	{
		vPt.push_back(vecVar[i].vPos);
	}

	iN = vPt.size();
	iC = 0;
	m_fTime = 0.0f;

	CurveUP();
	VtxUp();
}

void RBspline::Generate(int nCount, vec3* pVec)
{
	bC = false;

	vPt.clear();
	for( int i = 0; i < nCount; ++i)
	{
		vPt.push_back(pVec[i]);
	}

	iN = vPt.size();
	iC = 0;
	m_fTime = 0.0f;

	CurveUP();
	VtxUp();
}

void RBspline::CurveUP()
{
	int			i;
	float		u;
	int			n0;
	int			n1;
	int			n2;
	int			n3;
	RVector		p;
	RVector4	t;
	RVector		pTmp;

	m_fTime = 0;
	iC = 0;	

	for(i=-1; i<iN-2; ++i)
	{
		for(u=0; u<=1.f; u += fS)
		{
			t	= RVector4( u*u*u, u*u, u, 1);

			n0	= (i + 0)%iN;
			n1	= (i + 1)%iN;
			n2	= (i + 2)%iN;
			n3	= (i + 3)%iN;

			if(-1 == i)
			{
				pTmp	= vPt[0]  + ( vPt[0] - vPt[1]);

				p.x  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * pTmp.x
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[0].x
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[1].x
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[2].x;

				p.y  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * pTmp.y
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[0].y
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[1].y
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[2].y;

				p.z  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * pTmp.z
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[0].z
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[1].z
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[2].z;
			}

			else if(iN-3 == i)
			{
				pTmp	= vPt[iN-1]  + ( vPt[iN-1] - vPt[iN-2]);

				p.x  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * vPt[iN-3].x
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[iN-2].x
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[iN-1].x
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * pTmp.x;

				p.y  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * vPt[iN-3].y
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[iN-2].y
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[iN-1].y
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * pTmp.y;

				p.z  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * vPt[iN-3].z
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[iN-2].z
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[iN-1].z
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * pTmp.z;
			}

			else
			{
				p.x  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * vPt[n0].x
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[n1].x
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[n2].x
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[n3].x;

				p.y  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * vPt[n0].y
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[n1].y
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[n2].y
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[n3].y;

				p.z  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * vPt[n0].z
					+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[n1].z
					+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[n2].z
					+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[n3].z;
			}

			p.x /= 6.f;
			p.y /= 6.f;
			p.z /= 6.f;

			pCrv[iC] = p;
			++iC;
		}
	}
}

RVector RBspline::Update(float fElapseTime)
{
	m_fTime += fElapseTime;

	//int index = (int)(m_fTime / fS ) % iC;
	int index = (int)(m_fTime*(iC/m_fLifeTime))%iC;

	return pCrv[index];
}

INT RBspline::VtxUp()
{
	INT i;

	if(iC<2)
		return 1;

	for(i=0; i<iC; ++i)
	{
		pVtx[i].p = pCrv[i];
	}

	return 1;
}

void RBspline::Render(RDevice* pDevice)
{
	if(iN<3 || iC<2)
		return;

	RVector	pos;

	pDevice->SetLightEnable(0, false);
	pDevice->SetLighting(false);

	pDevice->SetTexture(0, R_NONE);
	pDevice->SetFvF(RTestVertexType);
	pDevice->DrawPrimitiveUP(RPT_LINESTRIP, iC-1, pVtx, sizeof(RTestVertex)	);
}

}
