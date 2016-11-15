#include "StdAfx.h"
#include "RCardinal.h"

// Cardinal curve [7/24/2006 madduck]

namespace rs3 {

RCardinal::RCardinal(void)
:	fT		(0.5f)
,	fS		(0.025f)
,	m_fTime (0.0f)
,	m_fLifeTime (0.0f)
,	iN( 0 )
{
	bC = true;
}

RCardinal::~RCardinal(void)
{
	Destroy();
}

void RCardinal::Destroy()
{
	vPt.clear();
}

void RCardinal::Generate(bool loop, const std::vector<REmitterVariable> &vectorVars, float fLifeTime)
{
	_ASSERT( fLifeTime > FLT_EPSILON );

	bC = loop;

	vPt.resize(vectorVars.size());
	bool bDontMove = true;

	vPt[0] = vectorVars[0].vPos;

	for ( size_t si = 1; si<vectorVars.size(); ++si )
	{
		vPt[si] = vectorVars[si].vPos;
		if ( (vPt[si]-vPt[si-1]).LengthSq() > FLT_EPSILON )
			bDontMove = false;
	}

	iN = static_cast<int>(vPt.size());
	
	m_fLifeTime = fLifeTime;
	m_fTime = 0.0f;

	if ( bDontMove )
	{
		vCurves.resize(1);
		vCurves[0] = vPt[0];
	}
	else if ( vPt.size() > 2 )
	{
		CurveUP();
	}
	else
	{
		// Linear Interpolation
		vCurves.clear();
		for(int i=0; i<iN-1; ++i)
			for(float u=0; u< 1.f; u += fS)
				vCurves.push_back(MMath::LinearInterpolation(vPt[i], vPt[i+1], u));
	}
}

void RCardinal::CurveUP()
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

	mtCd._11 =  -fT;	mtCd._12 =  2-fT;	mtCd._13 =   fT-2;	mtCd._14 =  fT;
	mtCd._21 = 2*fT;	mtCd._22 =  fT-3;	mtCd._23 = 3-2*fT;	mtCd._24 = -fT;
	mtCd._31 =  -fT;	mtCd._32 =     0;	mtCd._33 =     fT;	mtCd._34 =   0;
	mtCd._41 =    0;	mtCd._42 =     1;	mtCd._43 =      0;	mtCd._44 =   0;

	vCurves.clear();

	if( bC )
	{
		for(i=-1; i< (iN-1); ++i)
		{
			for(u=0; u<= 1.f; u += fS)
			{
				t	= RVector4( u*u*u, u*u, u, 1);

				n0	= (iN + i + 0)%iN;
				n1	= (iN + i + 1)%iN;
				n2	= (iN + i + 2)%iN;
				n3	= (iN + i + 3)%iN;

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

				vCurves.push_back(p);
			}
		}
	}
	else
	{
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
					pTmp	= vPt[0]+ ( vPt[0]- vPt[1]);

					p.x  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * pTmp.x
						+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[n1].x
						+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[n2].x
						+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[n3].x;

					p.y  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * pTmp.y
						+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[n1].y
						+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[n2].y
						+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[n3].y;

					p.z  =	(t.x * mtCd._11 + t.y * mtCd._21 + t.z * mtCd._31 + t.w * mtCd._41) * pTmp.z
						+	(t.x * mtCd._12 + t.y * mtCd._22 + t.z * mtCd._32 + t.w * mtCd._42) * vPt[n1].z
						+	(t.x * mtCd._13 + t.y * mtCd._23 + t.z * mtCd._33 + t.w * mtCd._43) * vPt[n2].z
						+	(t.x * mtCd._14 + t.y * mtCd._24 + t.z * mtCd._34 + t.w * mtCd._44) * vPt[n3].z;
				}
				else if(iN-3 == i)
				{
					pTmp	= vPt[iN-1]+ ( vPt[iN-1]- vPt[iN-2]);

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

				vCurves.push_back(p);
			}
		}
	}
}

void RCardinal::Reset()
{
	m_fTime = 0.0f;
}

RVector RCardinal::Update(float fElapseTime)
{
	_ASSERT( vCurves.size() > 0 );
	_ASSERT( m_fLifeTime > FLT_EPSILON );

	m_fTime += fElapseTime;
	int iC = static_cast<int>(vCurves.size());
	int index = (int)(m_fTime*(iC/m_fLifeTime))%iC;

	return vCurves[index];
}

void RCardinal::RenderPath(RDevice *pdevice)
{
	RPFC_THISFUNC;
	int vertexCount = static_cast<int>(vCurves.size());

	pdevice->SetFvF(D3DFVF_XYZ);
	if ( vCurves.size() >= 2 )
		pdevice->DrawPrimitiveUP(RPT_LINESTRIP, vertexCount - 1, (void*)&vCurves[0], sizeof(RVector));

}

}
