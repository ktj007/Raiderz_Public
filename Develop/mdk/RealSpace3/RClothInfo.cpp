#include "stdafx.h"
#include "RClothInfo.h"
#include <fstream>


namespace rs3
{
	char* RClothInfo::FM_VERSION					= "TEST_VERSION";
	char* RClothInfo::FM_PARTICLE					= "PARTICLE";
	char* RClothInfo::FM_TRIANGLE 					= "TRIANGLE";
	char* RClothInfo::FM_CONSTRAINT 				= "CONSTRAINT";
	char* RClothInfo::FM_PARAMETER					= "PARAMETER";
	char* RClothInfo::FM_PARTITIONEDTRIANGLE		= "PARTITIONED";

	RClothInfo::RClothInfo(void)
	{
		//--[ particles ]
		m_pParticles	= NULL;
		m_theNumberOfParticles = 0;

		//--[ triangles ]
		m_pType0_Triangles = NULL;
		m_pType1_Triangles = NULL;
		m_pType2_Triangles = NULL;
		m_pType3_Triangles = NULL;

		m_Type0_TheNumberOfTriangles = 0;
		m_Type1_TheNumberOfTriangles = 0;
		m_Type2_TheNumberOfTriangles = 0;
		m_Type3_TheNumberOfTriangles = 0;

		//--[ constraints ]
		m_pDistanceConstraints = NULL;
		m_theNumberOfDistanceConstraints = 0;

		m_pStretchConstraints = NULL;
		m_theNumberOfStretchConstraints = 0;
	}

	RClothInfo::~RClothInfo(void)
	{
		release();
	}

	RClothParticle* RClothInfo::allocateSpaceForParticles( int theNumberOfParticles )
	{
		_ASSERT(theNumberOfParticles >0);

		releaseSpaceForParticles();

		m_pParticles = new RClothParticle[theNumberOfParticles];
		if( NULL != m_pParticles )
		{
			setTheNumberOfParticles(theNumberOfParticles);
		}
		else
		{
			_ASSERT( false );
		}

		return m_pParticles;
	}

	void RClothInfo::releaseSpaceForParticles( void )
	{
		if( NULL != m_pParticles )
		{
			delete[] m_pParticles;
			m_pParticles = NULL;
		}

		setTheNumberOfParticles(0);
	}

	void RClothInfo::allocateSpaceForTrianglesAndTypes( int type0_TheNumberOfTriangles,int type1_TheNumberOfTriangles,int type2_TheNumberOfTriangles,int type3_TheNumberOfTriangles )
	{
		_ASSERT(type0_TheNumberOfTriangles >=0);
		_ASSERT(type1_TheNumberOfTriangles >=0);
		_ASSERT(type2_TheNumberOfTriangles >=0);
		_ASSERT(type3_TheNumberOfTriangles >=0);

		releaseSpaceForTrianlgesAndTypes();

		m_pType0_Triangles		= new RC_TRIANGLE[type0_TheNumberOfTriangles];
		m_pType1_Triangles		= new RC_TRIANGLE[type1_TheNumberOfTriangles];
		m_pType2_Triangles		= new RC_TRIANGLE[type2_TheNumberOfTriangles];
		m_pType3_Triangles		= new RC_TRIANGLE[type3_TheNumberOfTriangles];

		setTheNumberOfType0_Triangles(type0_TheNumberOfTriangles);
		setTheNumberOfType1_Triangles(type1_TheNumberOfTriangles);
		setTheNumberOfType2_Triangles(type2_TheNumberOfTriangles);
		setTheNumberOfType3_Triangles(type3_TheNumberOfTriangles);
	}

	void RClothInfo::releaseSpaceForTrianlgesAndTypes( void )
	{
		if( NULL != m_pType0_Triangles )
		{
			delete[] m_pType0_Triangles;
			m_pType0_Triangles = NULL;
		}
		if( NULL != m_pType1_Triangles )
		{
			delete[] m_pType1_Triangles;
			m_pType1_Triangles = NULL;
		}
		if( NULL != m_pType2_Triangles )
		{
			delete[] m_pType2_Triangles;
			m_pType2_Triangles = NULL;
		}
		if( NULL != m_pType3_Triangles )
		{
			delete[] m_pType3_Triangles;
			m_pType3_Triangles = NULL;
		}
	}


	void RClothInfo::allocateSpaceForDistanceConstraints( int theNumberOfConstraints )
	{
		_ASSERT(theNumberOfConstraints > 0 );

		releaseSpaceForDistanceConstraints();

		m_pDistanceConstraints = new RClothConstraint[theNumberOfConstraints];
		if( NULL != m_pDistanceConstraints )
		{
			setTheNumberOfDistanceConstraints(theNumberOfConstraints);
		}
		else
		{
			setTheNumberOfDistanceConstraints(0);
		}

	}

	void RClothInfo::releaseSpaceForDistanceConstraints( void )
	{
		if( NULL != m_pDistanceConstraints )
		{
			delete[] m_pDistanceConstraints;
			m_pDistanceConstraints = NULL;
		}

		setTheNumberOfDistanceConstraints(0);
	}

	void RClothInfo::allocateSpaceForStretchConstraints( int theNumberOfConstraints )
	{
		_ASSERT(theNumberOfConstraints > 0 );

		releaseSpaceForStretchConstraints();

		m_pStretchConstraints = new RClothConstraint[theNumberOfConstraints];
		if( NULL != m_pStretchConstraints )
		{
			setTheNumberOfStretchConstraints(theNumberOfConstraints);
		}
		else
		{
			setTheNumberOfStretchConstraints(0);
		}
	}

	void RClothInfo::releaseSpaceForStretchConstraints( void )
	{
		if( NULL != m_pStretchConstraints )
		{
			delete[] m_pStretchConstraints;
			m_pStretchConstraints = NULL;
		}

		setTheNumberOfStretchConstraints(0);
	}

	RClothConstraint& RClothInfo::getDistanceConstraint( int index ) const
	{
		_ASSERT( index < getTheNumberOfDistanceConstraints() );
		return m_pDistanceConstraints[index]; 
	}

	RClothConstraint& RClothInfo::getStretchConstraint( int index ) const
	{
		_ASSERT( index < getTheNumberOfStretchConstraints() );
		return m_pStretchConstraints[index]; 
	}


	void RClothInfo::release( void )
	{
		releaseSpaceForParticles();
		releaseSpaceForTrianlgesAndTypes();
		releaseSpaceForDistanceConstraints();
		releaseSpaceForStretchConstraints();
	}	



	void RClothInfo::calculateAABBOfTheGroup( RAABB *pAABB,unsigned	int numOfGroup )
	{
		_ASSERT( NULL != pAABB );
		_ASSERT(numOfGroup < m_partitionedTriangles.m_theNumberOfTriangleGroup );

		pAABB->Initialize();

		int theNumberOfTriangls = m_partitionedTriangles.m_pTriangleGroups[numOfGroup].m_theNumberOfTriangles;
		RC_TRIANGLE *pTriangles	= m_partitionedTriangles.m_pTriangleGroups[numOfGroup].m_pTriangles;
		RClothParticle* pParticles = getParticles();

		for(int i=0;i<theNumberOfTriangls;i++)
		{
			RC_VERTEX_INDEX &index0 = pTriangles[i].index0;
			RC_VERTEX_INDEX &index1 = pTriangles[i].index1;
			RC_VERTEX_INDEX &index2 = pTriangles[i].index2;

			pAABB->Add(pParticles[index0].p);
			pAABB->Add(pParticles[index1].p);
			pAABB->Add(pParticles[index2].p);
		}
	}

	bool RClothInfo::save( const char* szFileName )
	{
		FILE* pFile = NULL;
		pFile = fopen(szFileName,"wb");	//Opens an empty file for writing. If the given file exists, its contents are destroyed
		if( NULL == pFile )		return false;

		fwrite(FM_VERSION,strlen(FM_VERSION),1,pFile);
		//-parameter
		fwrite(FM_PARAMETER,strlen(FM_PARAMETER),1,pFile);
		fwrite(&m_parameter,sizeof(RClothParameter),1,pFile);	

		//-particles
		fwrite(FM_PARTICLE,strlen(FM_PARTICLE),1,pFile);
		fwrite(&m_theNumberOfParticles,sizeof(int),1,pFile);
		for(int i=0;i<m_theNumberOfParticles;i++)
		{
			fwrite(&m_pParticles[i].oriP,		sizeof(RVector)	,1,pFile);
			fwrite(&m_pParticles[i].oriP,		sizeof(RVector)	,1,pFile);
			fwrite(&m_pParticles[i].oriP,		sizeof(RVector)	,1,pFile);
			fwrite(&m_pParticles[i].v,			sizeof(RVector)	,1,pFile);
			fwrite(&m_pParticles[i].color,		sizeof(RVector)	,1,pFile);
			fwrite(&m_pParticles[i].inverseMass,sizeof(float)	,1,pFile);
		}

		//-triangles
		fwrite(FM_TRIANGLE,strlen(FM_TRIANGLE),1,pFile);
		fwrite(&m_Type0_TheNumberOfTriangles,sizeof(int),1,pFile);
		fwrite(&m_Type1_TheNumberOfTriangles,sizeof(int),1,pFile);
		fwrite(&m_Type2_TheNumberOfTriangles,sizeof(int),1,pFile);
		fwrite(&m_Type3_TheNumberOfTriangles,sizeof(int),1,pFile);

		for(int i=0;i<m_Type0_TheNumberOfTriangles;i++)
		{
			fwrite(&m_pType0_Triangles[i],sizeof(RC_TRIANGLE),1,pFile);
		}
		for(int i=0;i<m_Type1_TheNumberOfTriangles;i++)
		{
			fwrite(&m_pType1_Triangles[i],sizeof(RC_TRIANGLE),1,pFile);
		}
		for(int i=0;i<m_Type2_TheNumberOfTriangles;i++)
		{
			fwrite(&m_pType2_Triangles[i],sizeof(RC_TRIANGLE),1,pFile);
		}
		for(int i=0;i<m_Type3_TheNumberOfTriangles;i++)
		{
			fwrite(&m_pType3_Triangles[i],sizeof(RC_TRIANGLE),1,pFile);
		}


		//-constraints
		fwrite(FM_CONSTRAINT,strlen(FM_CONSTRAINT),1,pFile);
		fwrite(&m_theNumberOfDistanceConstraints,sizeof(int),1,pFile);
		for(int i=0;i<m_theNumberOfDistanceConstraints;i++)
		{
			fwrite(&m_pDistanceConstraints[i],sizeof(RClothConstraint),1,pFile);
		}
		fwrite(&m_theNumberOfStretchConstraints,sizeof(int),1,pFile);	
		for(int i=0;i<m_theNumberOfStretchConstraints;i++)
		{
			fwrite(&m_pStretchConstraints[i],sizeof(RClothConstraint),1,pFile);	
		}

		//-PartitionedTriangles
		fwrite(FM_PARTITIONEDTRIANGLE,strlen(FM_PARTITIONEDTRIANGLE),1,pFile);
		m_partitionedTriangles.save(pFile);

		fclose(pFile);

		return true;
	}

	bool RClothInfo::load( const char* szFileName )
	{
		char szFMTest[255];
		MFile file;
		if( false == file.Open(szFileName) ) return false;

		file.Read(szFMTest,strlen(FM_VERSION));	szFMTest[strlen(FM_VERSION)] = 0;
		if( strcmp(FM_VERSION,szFMTest) )	_ASSERT(false);

		//-parameter
		file.Read(szFMTest,strlen(FM_PARAMETER)); szFMTest[strlen(FM_PARAMETER)] = 0;
		if( strcmp(FM_PARAMETER,szFMTest) )	_ASSERT(false);
		file.Read(&m_parameter,sizeof(RClothParameter));

		//-particles
		file.Read(szFMTest,strlen(FM_PARTICLE)); szFMTest[strlen(FM_PARTICLE)] = 0;
		if( strcmp(FM_PARTICLE,szFMTest) )	_ASSERT(false);

		file.Read(&m_theNumberOfParticles,sizeof(int));
		allocateSpaceForParticles(m_theNumberOfParticles);
		for(int i=0;i<m_theNumberOfParticles;i++)
		{
			file.Read(&m_pParticles[i].oriP ,sizeof(RVector));
			file.Read(&m_pParticles[i].p ,sizeof(RVector));
			file.Read(&m_pParticles[i].newP ,sizeof(RVector));
			file.Read(&m_pParticles[i].v ,sizeof(RVector));
			file.Read(&m_pParticles[i].color ,sizeof(RVector));
			file.Read(&m_pParticles[i].inverseMass ,sizeof(float));
		}

		//-triangles
		file.Read(szFMTest,strlen(FM_TRIANGLE)); szFMTest[strlen(FM_TRIANGLE)] = 0;
		if( strcmp(FM_TRIANGLE,szFMTest) )	_ASSERT(false);

		file.Read(&m_Type0_TheNumberOfTriangles,sizeof(int));	_ASSERT(m_Type0_TheNumberOfTriangles>=0);
		file.Read(&m_Type1_TheNumberOfTriangles,sizeof(int));	_ASSERT(m_Type1_TheNumberOfTriangles>=0);
		file.Read(&m_Type2_TheNumberOfTriangles,sizeof(int));	_ASSERT(m_Type2_TheNumberOfTriangles>=0);
		file.Read(&m_Type3_TheNumberOfTriangles,sizeof(int));	_ASSERT(m_Type3_TheNumberOfTriangles>=0);

		allocateSpaceForTrianglesAndTypes(m_Type0_TheNumberOfTriangles,m_Type1_TheNumberOfTriangles,m_Type2_TheNumberOfTriangles,m_Type3_TheNumberOfTriangles);

		for(int i=0;i<m_Type0_TheNumberOfTriangles;i++)
		{
			file.Read(&m_pType0_Triangles[i],sizeof(RC_TRIANGLE) );
		}
		for(int i=0;i<m_Type1_TheNumberOfTriangles;i++)
		{
			file.Read(&m_pType1_Triangles[i],sizeof(RC_TRIANGLE) );
		}
		for(int i=0;i<m_Type2_TheNumberOfTriangles;i++)
		{
			file.Read(&m_pType2_Triangles[i],sizeof(RC_TRIANGLE) );
		}
		for(int i=0;i<m_Type3_TheNumberOfTriangles;i++)
		{
			file.Read(&m_pType3_Triangles[i],sizeof(RC_TRIANGLE) );
		}

		//-constraints
		file.Read(szFMTest,strlen(FM_CONSTRAINT)); szFMTest[strlen(FM_CONSTRAINT)] = 0;
		if( strcmp(FM_CONSTRAINT,szFMTest) )	_ASSERT(false);

		file.Read(&m_theNumberOfDistanceConstraints,sizeof(int));	_ASSERT(m_theNumberOfDistanceConstraints>0);
		allocateSpaceForDistanceConstraints(m_theNumberOfDistanceConstraints);
		for(int i=0;i<m_theNumberOfDistanceConstraints;i++)
		{
			file.Read(&m_pDistanceConstraints[i],sizeof(RClothConstraint));
		}
		
		file.Read(&m_theNumberOfStretchConstraints,sizeof(int));	_ASSERT(m_theNumberOfStretchConstraints>0);
		allocateSpaceForStretchConstraints(m_theNumberOfStretchConstraints);
		for(int i=0;i<m_theNumberOfStretchConstraints;i++)
		{
			file.Read(&m_pStretchConstraints[i],sizeof(RClothConstraint));	
		}

		//-PartitionedTriangles
		file.Read(szFMTest,strlen(FM_PARTITIONEDTRIANGLE)); szFMTest[strlen(FM_PARTITIONEDTRIANGLE)] = 0;
		if( strcmp(FM_PARTITIONEDTRIANGLE,szFMTest) )	_ASSERT(false);
		m_partitionedTriangles.load(&file);

		file.Close();

		return true;
	}

	bool RClothInfo::isWorldSimulation( const char* szFileName,bool *pbWorldSimulation )
	{
		char szFMTest[255];
		MFile file;
		if( false == file.Open(szFileName) ) return true;

		file.Read(szFMTest,strlen(FM_VERSION));	szFMTest[strlen(FM_VERSION)] = 0;
		if( strcmp(FM_VERSION,szFMTest) )	_ASSERT(false);

		//-parameter
		file.Read(szFMTest,strlen(FM_PARAMETER)); szFMTest[strlen(FM_PARAMETER)] = 0;
		if( strcmp(FM_PARAMETER,szFMTest) )	_ASSERT(false);

		RClothParameter parameter;
		file.Read(&parameter,sizeof(RClothParameter));

		//-worldSimluation;
		*pbWorldSimulation = parameter.getWorldSimulation();

		file.Close();
		return true;
	}


	//--------------------------------------------------------------------------------------------
	void RClothGroupTriangles::releaseATriangleGroup( void )
	{
		if( 0 != m_pTriangles )	
		{
			delete [] m_pTriangles;
			m_pTriangles = 0;
		}
		m_theNumberOfTriangles = 0;
	}

	void RClothGroupTriangles::allocateForATriangleGroup( unsigned int theNumberOfTriangles )
	{
		_ASSERT( theNumberOfTriangles > 0 );
		_ASSERT( 0 == m_pTriangles );

		releaseATriangleGroup();

		m_theNumberOfTriangles = theNumberOfTriangles;
		m_pTriangles = new RC_TRIANGLE[m_theNumberOfTriangles];
	}

	//--------------------------------------------------------------------------------------------
	void RClothPartitionedTriangles::releaseTriangleGroups( void )
	{
		if( 0 == m_pTriangleGroups ) return;

		for(unsigned int i=0;i<m_theNumberOfTriangleGroup;i++)
		{
			m_pTriangleGroups[i].releaseATriangleGroup();
		}

		if( 0 != m_pTriangleGroups )
		{
			delete[] m_pTriangleGroups;
			m_pTriangleGroups = 0;
		}
		m_theNumberOfTriangleGroup = 0;
	}

	void RClothPartitionedTriangles::allocateForTriangleGroups( unsigned int theNumberOfTriangleGroup )
	{
		_ASSERT( theNumberOfTriangleGroup > 0 );
		_ASSERT( 0 == m_pTriangleGroups );

		releaseTriangleGroups();
		m_theNumberOfTriangleGroup = theNumberOfTriangleGroup;
		m_pTriangleGroups = new RClothGroupTriangles[m_theNumberOfTriangleGroup];
	}	


	bool RClothPartitionedTriangles::save( FILE *pFile )
	{
		unsigned int writeNum = 0;
		writeNum = fwrite(&m_theNumberOfTriangleGroup,sizeof(unsigned int),1,pFile);
		if( writeNum != 1 )	return false;
		
		for(unsigned int i=0;i<m_theNumberOfTriangleGroup;i++)
		{
			writeNum = fwrite(&m_pTriangleGroups[i].m_theNumberOfTriangles,sizeof(unsigned int),1,pFile);
			if( writeNum != 1 )	return false;

			writeNum = fwrite(m_pTriangleGroups[i].m_pTriangles,sizeof(RC_TRIANGLE),m_pTriangleGroups[i].m_theNumberOfTriangles,pFile);
			if( writeNum != m_pTriangleGroups[i].m_theNumberOfTriangles )	return false;
		}

		return true;
	}

	bool RClothPartitionedTriangles::load( FILE *pFile )
	{
		unsigned int writeNum = 0;
		writeNum = fread(&m_theNumberOfTriangleGroup,sizeof(unsigned int),1,pFile);
		if( writeNum != 1 )	return false;
		_ASSERT( m_theNumberOfTriangleGroup > 0);
		allocateForTriangleGroups(m_theNumberOfTriangleGroup);

		for(unsigned int i=0;i<m_theNumberOfTriangleGroup;i++)
		{
			writeNum = fread(&m_pTriangleGroups[i].m_theNumberOfTriangles,sizeof(unsigned int),1,pFile);
			if( writeNum != 1 )	return false;
			_ASSERT( m_pTriangleGroups[i].m_theNumberOfTriangles > 0 );

			m_pTriangleGroups[i].allocateForATriangleGroup(m_pTriangleGroups[i].m_theNumberOfTriangles);
			writeNum = fread(m_pTriangleGroups[i].m_pTriangles,sizeof(RC_TRIANGLE),m_pTriangleGroups[i].m_theNumberOfTriangles,pFile);
			if( writeNum != m_pTriangleGroups[i].m_theNumberOfTriangles )	return false;
		}

		return true;
	}

	bool RClothPartitionedTriangles::load( MFile *pFile )
	{
		_ASSERT( NULL != pFile );

		pFile->Read(&m_theNumberOfTriangleGroup,sizeof(unsigned int));
		_ASSERT( m_theNumberOfTriangleGroup > 0);

		allocateForTriangleGroups(m_theNumberOfTriangleGroup);

		for(unsigned int i=0;i<m_theNumberOfTriangleGroup;i++)
		{
			pFile->Read(&m_pTriangleGroups[i].m_theNumberOfTriangles,sizeof(unsigned int));
			_ASSERT( m_pTriangleGroups[i].m_theNumberOfTriangles > 0 );

			m_pTriangleGroups[i].allocateForATriangleGroup(m_pTriangleGroups[i].m_theNumberOfTriangles);
			pFile->Read(m_pTriangleGroups[i].m_pTriangles,sizeof(RC_TRIANGLE) * m_pTriangleGroups[i].m_theNumberOfTriangles);
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------

}


