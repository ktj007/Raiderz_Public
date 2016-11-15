#include "stdafx.h"

#include "ELMaxMesh.h"
#include "RMaxMaterial.h"

void RMaxMeshFile::Save(BYTE n) { fwrite(&n,sizeof(n),1,m_fp); }

void RMaxMeshFile::Save(char n) { fwrite(&n,sizeof(n),1,m_fp); }

void RMaxMeshFile::Save(WORD n) { fwrite(&n,sizeof(n),1,m_fp); }

void RMaxMeshFile::Save(short n) { fwrite(&n,sizeof(n),1,m_fp); }

void RMaxMeshFile::Save(DWORD n) { fwrite(&n,sizeof(n),1,m_fp); }

void RMaxMeshFile::Save(int n) { fwrite(&n,sizeof(n),1,m_fp); }

void RMaxMeshFile::Save(float f) { fwrite(&f,sizeof(f),1,m_fp); }

void RMaxMeshFile::Save(char* str) 
{	
	int nSize = strlen(str)+1;
		fwrite(&nSize,sizeof(int),1,m_fp);

	if(nSize)
		fwrite(str,nSize,1,m_fp);
}

void RMaxMeshFile::Save(string& str)
{
	int nSize = int(str.size() + 1);
		fwrite(&nSize,sizeof(int),1,m_fp);

	if(nSize)
		fwrite(str.c_str(),nSize,1,m_fp);
}

void RMaxMeshFile::Save(D3DXVECTOR3& v) { fwrite(&v,sizeof(D3DXVECTOR3),1,m_fp); }

void RMaxMeshFile::Save(D3DXVECTOR4& v) { fwrite(&v,sizeof(D3DXVECTOR4),1,m_fp); }

void RMaxMeshFile::Save(D3DXCOLOR& v) { fwrite(&v,sizeof(D3DXCOLOR),1,m_fp); }

void RMaxMeshFile::Save(D3DXMATRIX& m) {	fwrite(&m,sizeof(D3DXMATRIX),1,m_fp); }

void RMaxMeshFile::Save(D3DXVECTOR3* v,int nCnt) { if(nCnt) fwrite(v,sizeof(D3DXVECTOR3),nCnt,m_fp); }

void RMaxMeshFile::Save(D3DXVECTOR4* v,int nCnt) { if(nCnt) fwrite(v,sizeof(D3DXVECTOR4),nCnt,m_fp); }

void RMaxMeshFile::Save(D3DXCOLOR* v,int nCnt) {	if(nCnt) fwrite(v,sizeof(D3DXCOLOR),nCnt,m_fp); }

void RMaxMeshFile::Save(D3DXMATRIX* m,int nCnt) { if(nCnt) fwrite(m,sizeof(D3DXMATRIX),nCnt,m_fp); }

void RMaxMeshFile::Save(void* data,int nSize) 
{ 
	fwrite(data,sizeof(char),nSize,m_fp); 
}

void RMaxMeshFile::Save(RMaxFaceData* p,int nCnt)
{
	if(nCnt) {
//		fwrite(p,sizeof(RFaceData),nCnt,m_fp);
		for(int i=0;i<nCnt;i++) {
			fwrite(&p[i].index,sizeof(RFaceSubData),3,m_fp);
			short nMaterial = p[i].pMaterial->GetIndex();
			fwrite(&nMaterial,sizeof(short),1,m_fp);
		}
	}
}

void RMaxMeshFile::Save(RPhysiqueInfo* p,int nCnt) 
{ 
	Save( nCnt );

	RPhysiqueSubData* pData;

	int nBlendCnt;

	for(int i=0;i<nCnt;i++)
	{
		nBlendCnt = p[i].m_num;

		Save( nBlendCnt );

		for(int j=0;j<nBlendCnt;j++) {

			pData = &p[i].m_Data[j];

//			Save( pData->name );	// pid 를 고유하게 만들면 제거대상..
			Save( pData->cid );
			Save( pData->pid );
			Save( pData->weight );	// 
//			Save( pData->offset );	// 제거대상
		}
	}
}

void RMaxMeshFile::Save(RVisKey* p,int nCnt)
{
	Save(nCnt);

	if(nCnt)
		Save((void*)p,sizeof(RVisKey)*nCnt);
}

void RMaxMeshFile::SaveText(char* name,D3DXCOLOR& c)
{
	fprintf(m_fp,"%s %f %f %f \n",name,c.r ,c.g ,c.b);
}

void RMaxMeshFile::SaveText(char* name,D3DXVECTOR3& v)
{
	fprintf(m_fp,"%s %f %f %f \n",name,v.x ,v.y ,v.z);
}

void RMaxMeshFile::SaveText(char* name,D3DXVECTOR3& v,float f)
{
	fprintf(m_fp,"%s %f %f %f %f\n",name,v.x ,v.y ,v.z,f);
}

void RMaxMeshFile::SaveText(char* name,D3DXMATRIX& c)
{
	fprintf(m_fp,"mat %s\n",name);

	fprintf(m_fp,"%f %f %f %f\n",	c._11,	c._12,	c._13 ,c._14);
	fprintf(m_fp,"%f %f %f %f\n",	c._21,	c._22,	c._23 ,c._24);
	fprintf(m_fp,"%f %f %f %f\n",	c._31,	c._32,	c._33 ,c._34);
	fprintf(m_fp,"%f %f %f %f\n",	c._41,	c._42,	c._43 ,c._44);
}


void RMaxMeshFile::SaveText(char* name,int n1)
{
	fprintf(m_fp,"%s %d\n",name,n1);
}

void RMaxMeshFile::SaveText(char* name,int n1,int n2)
{
	fprintf(m_fp,"%s %d %d\n",name,n1,n2);
}

void RMaxMeshFile::SaveText(char* name,int n1,int n2,int n3)
{
	fprintf(m_fp,"%s %d %d %d \n",name,n1,n2,n3);
}

void RMaxMeshFile::SaveText(char* name,float f1)
{
	fprintf(m_fp,"%s %f\n",name,f1);
}

void RMaxMeshFile::SaveText(char* name,float f1,float f2)
{
	fprintf(m_fp,"%s %f %f\n",name,f1,f2);
}

void RMaxMeshFile::SaveText(char* name,float f1,float f2,float f3)
{
	fprintf(m_fp,"%s %f %f %f \n",name,f1,f2,f3);
}

void RMaxMeshFile::SaveText(char* name,string& s1)
{
	fprintf(m_fp,"%s %s\n",name,s1.c_str());
}

void RMaxMeshFile::SaveText(char* name,char* s1)
{
	fprintf(m_fp,"%s %s\n",name,s1);
}

void RMaxMeshFile::SaveText(char* name,char* s1,char* s2)
{
	fprintf(m_fp,"%s %s %s \n",name,s1,s2);
}

void RMaxMeshFile::SaveText(char* name,char* s1,char* s2,char* s3)
{
	fprintf(m_fp,"%s %s %s %s \n",name,s1,s2,s3);
}

void RMaxMeshFile::SaveText(char* pFormat,...)
{
	char temp[16*1024];	// 16k

	va_list args;

	va_start(args,pFormat);
	vsprintf(temp,pFormat,args);
	va_end(args);

	fprintf(m_fp,temp);
}

