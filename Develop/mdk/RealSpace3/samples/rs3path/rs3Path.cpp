#include <windows.h>

#include "crtdbg.h"
#include "RDeviceD3D.h"
#include "RSceneManager.h"
#include "RTerrain.h"
#include "MXml.h"
#include "../sampleApp.h"

#pragma comment(lib, "../../../RealSpace3/lib/RealSpace3d.lib")
#pragma comment(lib, "../../../cml2/lib/cml2d.lib")

using namespace rs3;

#include "d3dx9.h"

typedef vector<int>	vi;
typedef vector<vi>	vvi;

class bmpHeader
{
public:
	char	header[2];
	size_t	fileSize;
	int		reserved;
	int		byteoffset;


	int		headersize;
	size_t	width;
	size_t	height;
	short	bitplane;  
	short	bpp;
	int		comptype;
	size_t	imgsize;
	int		widthres;  
	int		heightres;
	int		color;
	int		coloridx; 

	bmpHeader()
	{
		memset(header, 0, sizeof(bmpHeader));
		header[0] = 'B';
		header[1] = 'M';
		byteoffset = 54;
		headersize = 40;
		bpp = 24;
		bitplane = 1;
	}

	void	loadfromfile(FILE* fp)
	{
		fread(header, 2, 1, fp);
		fread(&fileSize, 4, 1, fp);
		fread(&reserved, 4, 1, fp);
		fread(&byteoffset, 4, 1, fp);
		fread(&headersize, 4, 1, fp);
		fread(&width, 4, 1, fp);
		fread(&height, 4, 1, fp);
		fread(&bitplane, 2, 1, fp);
		fread(&bpp, 2, 1, fp);
		fread(&comptype, 4, 1, fp);
		fread(&imgsize, 4, 1, fp);
		fread(&widthres, 4, 1, fp);
		fread(&heightres, 4, 1, fp);
		fread(&color, 4, 1, fp);
		fread(&coloridx, 4, 1, fp);
	}

	void	savetofile(FILE* fp)
	{
		fwrite(header, 2, 1, fp);
		fwrite(&fileSize, 4, 1, fp);
		fwrite(&reserved, 4, 1, fp);
		fwrite(&byteoffset, 4, 1, fp);
		fwrite(&headersize, 4, 1, fp);
		fwrite(&width, 4, 1, fp);
		fwrite(&height, 4, 1, fp);
		fwrite(&bitplane, 2, 1, fp);
		fwrite(&bpp, 2, 1, fp);
		fwrite(&comptype, 4, 1, fp);
		fwrite(&imgsize, 4, 1, fp);
		fwrite(&widthres, 4, 1, fp);
		fwrite(&heightres, 4, 1, fp);
		fwrite(&color, 4, 1, fp);
		fwrite(&coloridx, 4, 1, fp);
	}
};


struct mrgb
{
	byte r, g, b;
	
	mrgb(){}
	mrgb(byte a, byte bb, byte c) : r(c), g(bb), b(a) {}
};

typedef mrgb(*PackFunc)(int);


mrgb wallsave(int a)
{
	if (a == 0)
		return mrgb(255, 255, 255);
	else
		return mrgb(0, 0, 0);
}

mrgb isoSave(int a)
{
	if (a == 0)
		return mrgb(255, 255, 255);
	else if (a == 1)
		return mrgb(0, 0, 0);
	else
	{
		return mrgb((a * a * a * 235) % 255, (a * a * a * a * 12561) % 255, (a * a * 612) % 255);
	}
}


class myApplication : public sampleApp 
{
	RTerrain*		m_pTerrain;
	
	vvi				WallMap;
	vvi				FIlledMap;	
	int				mainPlaneIdx;
public:
	myApplication() : sampleApp(), m_pTerrain(NULL) {}

	char *GetName() { return "rs3 path sample application"; }
	bool OnCreate();
	void OnDestroy();

	void OnMarkWall();
	void RemoveIsolate();
	void RemoveOtherPlane();

	void SaveToBMP(const vvi& dat, const char* filename, PackFunc f);
} g_App;


bool myApplication::OnCreate()
{
	MXml kXml;

	if ( !kXml.LoadFile("../../../Game/Runtime/Data/Map/Cemetery/Cemetery.xml", NULL))
	{
		return false;
	}

	MXmlElement *pRoot = kXml.Doc()->FirstChildElement("ROOT");
	MXmlElement *pScene = pRoot->FirstChildElement("SCENE");
	MXmlElement *pTerrain = pScene->FirstChildElement("RTERRAIN");
	MXmlElement *pProp = pTerrain->FirstChildElement("PROPERTY");

	m_pTerrain = new RTerrain;
	m_pTerrain->SetPath("../../../Game/Runtime/Data/Map/Cemetery/");
	m_pTerrain->CreateFromXML(*pProp);


	OnMarkWall();
	RemoveIsolate();
	RemoveOtherPlane();

	return false;
}


void myApplication::OnDestroy()
{
	SAFE_DELETE(m_pTerrain);
}



void myApplication::OnMarkWall()
{
	RVector p1, p2, p3, p4;

	float x1, x2, y1, y2;
	RVector v1, v2, l1, l2, upvec;
	float d1, d2;

	upvec.Set(0, 0, 1);

	WallMap = vvi(m_pTerrain->GetHeightMap(), vi(m_pTerrain->GetWidthMap()));

	for (int i=0; i<m_pTerrain->GetHeightMap(); i++)
	{
		for (int j=0; j<m_pTerrain->GetWidthMap(); j++)
		{
			if (i == 255)
			{
				i = i;
			}
			x1 = j * m_pTerrain->GetXScale();
			x2 = (j + 1) * m_pTerrain->GetXScale() - 0.1;
			y1 = i * m_pTerrain->GetYScale();
			y2 = (i + 1) * m_pTerrain->GetYScale() - 0.1;
			
			
			m_pTerrain->GetPickVertical(p1, x1, y1);
			m_pTerrain->GetPickVertical(p2, x1, y2);
			m_pTerrain->GetPickVertical(p3, x2, y1);
			m_pTerrain->GetPickVertical(p4, x2, y2);


			l1 = p1 - p2;
			l2 = p1 - p3;
			v1 = l1.CrossProduct(l2);

			l1 = p4 - p2;
			l2 = p4 - p3;
			v2 = l1.CrossProduct(l2);

			if (v1.z < 0) v1 = -v1;
			if (v2.z < 0) v2 = -v2;

			v1 = v1.Normalize();
			v2 = v2.Normalize();

			d1 = upvec.DotProduct(v1);
			d2 = upvec.DotProduct(v1);

			if (d1 > d2) d1 = d2;

			if (d1 < cos(DegreeToRadian(40))) 
				WallMap[i][j] = 1;
			else
				WallMap[i][j] = 0;
		}
	}

	SaveToBMP(WallMap, "wallmap.bmp", wallsave);
}


#include <queue>

void myApplication::RemoveIsolate()
{
	FIlledMap = WallMap;
	size_t w = FIlledMap[0].size();
	size_t h = FIlledMap.size();
	int nowcolor;

	nowcolor = -1;
	for (size_t i=0; i<h; i++)
	{
		for (size_t j=0; j<w; j++)
		{
			if (FIlledMap[i][j] == 1)
			{
				typedef pair<size_t, size_t> coord;
				queue<coord> q;
				vector<coord> processed;
				coord c;
				static const int dx[4] = { 0, 0, -1, 1 };
				static const int dy[4] = { -1, 1, 0, 0 };

				q.push(make_pair(i, j));
				FIlledMap[i][j] = nowcolor;

				do
				{
					c = q.front();
					q.pop();
					processed.push_back(c);

					for (int k=0; k<4; k++)	
					{
						int nx = c.second + dx[k];
						int ny = c.first + dy[k];
						if ((nx >= 0) && (ny >= 0) && (nx < w) && (ny < h))
						{
							if (FIlledMap[ny][nx] == 1)	
							{
								q.push(make_pair(ny, nx));
								FIlledMap[ny][nx] = nowcolor;
							}
						}
					}
				}
				while(!q.empty());

				if (processed.size() < 5)
				{
					for (int k=0; k<processed.size(); k++)
						FIlledMap[processed[k].first][processed[k].second] = 0;
				}

				nowcolor--;
			}
		}
	}


	for (size_t i=0; i<h; i++)
	{
		for (size_t j=0; j<w; j++)
		{
			if (FIlledMap[i][j] < 0) FIlledMap[i][j] = 1;
		}
	}

	nowcolor = 2;
	int m = 0;
	for (size_t i=0; i<h; i++)
	{
		for (size_t j=0; j<w; j++)
		{
			if (FIlledMap[i][j] == 0)
			{
				typedef pair<size_t, size_t> coord;
				queue<coord> q;
				vector<coord> processed;
				coord c;
				static const int dx[4] = { 0, 0, -1, 1 };
				static const int dy[4] = { -1, 1, 0, 0 };

				q.push(make_pair(i, j));
				FIlledMap[i][j] = nowcolor;

				do
				{
					c = q.front();
					q.pop();
					processed.push_back(c);

					for (int k=0; k<4; k++)	
					{
						int nx = c.second + dx[k];
						int ny = c.first + dy[k];
						if ((nx >= 0) && (ny >= 0) && (nx < w) && (ny < h))
						{
							if (FIlledMap[ny][nx] == 0)
							{
								q.push(make_pair(ny, nx));
								FIlledMap[ny][nx] = nowcolor;
							}
						}
					}
				}
				while(!q.empty());

				if (m < processed.size())
				{
					m = processed.size();
					mainPlaneIdx = nowcolor;
				}

				if (processed.size() < 15)
				{
					for (int k=0; k<processed.size(); k++)
						FIlledMap[processed[k].first][processed[k].second] = 1;
				}

				nowcolor++;
			}
		}
	}


	SaveToBMP(FIlledMap, "isomap.bmp", isoSave);
}


void myApplication::RemoveOtherPlane()
{
	size_t w = FIlledMap[0].size();
	size_t h = FIlledMap.size();

	for (size_t i=0; i<h; i++)
	{
		for (size_t j=0; j<w; j++)
		{
			if (FIlledMap[i][j] != mainPlaneIdx)
				FIlledMap[i][j] = 1;
		}
	}
	SaveToBMP(FIlledMap, "mainplane.bmp", isoSave);
}


void myApplication::SaveToBMP(const vvi& dat, const char* filename, PackFunc f)
{
	size_t pitch = dat[0].size() * 3;
	size_t hei = dat.size();
	size_t len = (hei * pitch);
	int offset = 0;
	byte* buf = new byte[len];
	
	bmpHeader h;
	h.fileSize = len + 54;
	h.imgsize = len;
	h.width = dat[0].size();
	h.height = dat.size();

	mrgb c;

	for (size_t i=0; i<dat.size(); i++)
	{
		for (size_t j=0; j<dat[0].size(); j++)
		{
			c = f(dat[i][j]);
			buf[(hei - i - 1) * pitch + (j * 3) + 0] = c.r;
			buf[(hei - i - 1) * pitch + (j * 3) + 1] = c.g;
			buf[(hei - i - 1) * pitch + (j * 3) + 2] = c.b;
		}
	}

	FILE* fp = fopen(filename, "wb");

	h.savetofile(fp);
	fwrite(buf, len, 1, fp);

	fclose(fp);

	delete[] buf;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	rs3::config.SetShaderPath("../../Shaders");
	rs3::config.SetTreePath("./data");
	rs3::config.SetTexturePath("./;./data/;./data/terrain/");

	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	return g_App.Run();
}