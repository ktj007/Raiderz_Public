#include "stdafx.h"
#include "../../../sdk/pathengine/i_pathengine.h"
#include "MPathPreprocessFile.h"

#include <stdio.h>
#include <assert.h>
#include <MMSystem.h>
#include "MFileSystem.h"

class cFileOutputStream : public iOutputStream
{
	FILE* _file;

public:
	cFileOutputStream(const char* name)
	{
		_file = fopen(name, "wb");
		assert(_file);
	}
	~cFileOutputStream()
	{
		int errorOccurred = fclose(_file);
		assert(!errorOccurred);
	}
	void put(const char* data, unsigned long dataSize)
	{
		size_t written = fwrite(data, 1, dataSize, _file);
		assert(written == dataSize);
	}
};

DWORD MPathFindPreprocess::CURRENT_VERSION	= 0x00010001;
DWORD MPathFindPreprocess::CURRENT_HEADER	= 0x00505245;

const int MD5_STRING_LENGTH = 16;

void MPathFindPreprocess::GenerateFile(iMesh *mesh, const char *pcStringMD5, iShape *shape
									   ,const char *pathfindFilename
									   ,const char *collisionFilename
									   ,const char *const* CollisionAttributes /* = NULL  */
									   ,const char *const* PathfindAttribute /* = NULL */)
{
	mesh->generateCollisionPreprocessFor(shape, CollisionAttributes);
	mesh->generatePathfindPreprocessFor(shape, PathfindAttribute);
	
	PATH_HEADER header;
	header.dwHeader = CURRENT_HEADER;
	header.dwVersion = CURRENT_VERSION;

	PATH_SHAPEINFO shapeInfo;
	shapeInfo.dwShapeSize = (DWORD)shape->size();
	shapeInfo.pVertex2DiArray = new long[shapeInfo.dwShapeSize*2];
	_ASSERT(shapeInfo.pVertex2DiArray);

	for ( DWORD di = 0; di<shapeInfo.dwShapeSize; ++di )
	{
		tSigned32 x, y;
		shape->vertex(di, x, y);
		shapeInfo.pVertex2DiArray[di*2+0] = x;
		shapeInfo.pVertex2DiArray[di*2+1] = y;
	}

	// collision file save
	{
		cFileOutputStream collisionFile(collisionFilename);
		collisionFile.put((const char*)&header, sizeof(header));
		collisionFile.put(pcStringMD5, MD5_STRING_LENGTH*sizeof(char));
		collisionFile.put((const char*)&shapeInfo.dwShapeSize, sizeof(shapeInfo.dwShapeSize));
		collisionFile.put((const char*)shapeInfo.pVertex2DiArray, sizeof(long)*shapeInfo.dwShapeSize*2);
		mesh->saveCollisionPreprocessFor(shape,&collisionFile);
	}

	// path-find file save
	{
		cFileOutputStream pathfindFile(pathfindFilename);
		pathfindFile.put((const char*)&header, sizeof(header));
		pathfindFile.put(pcStringMD5, MD5_STRING_LENGTH*sizeof(char));
		pathfindFile.put((const char*)&shapeInfo.dwShapeSize, sizeof(shapeInfo.dwShapeSize));
		pathfindFile.put((const char*)shapeInfo.pVertex2DiArray, sizeof(long)*shapeInfo.dwShapeSize*2);
		mesh->savePathfindPreprocessFor(shape,&pathfindFile);
	}

	delete shapeInfo.pVertex2DiArray;
	shapeInfo.pVertex2DiArray = NULL;
}

bool MPathFindPreprocess::IsValidShapeInfo(iShape *shape, PATH_SHAPEINFO *pShapeInfo)
{
	if ((DWORD)shape->size() != pShapeInfo->dwShapeSize)
		return false;

	// 약간 묘하게 사용, 주의 !!
	long *pVertex2DiArray = (long*)((&pShapeInfo->dwShapeSize)+1);
	for (DWORD di = 0; di<pShapeInfo->dwShapeSize; ++di)
	{
		tSigned32 x, y;
		shape->vertex(di, x, y);
		if ( pVertex2DiArray[di*2+0] != x )
			return false;
		if ( pVertex2DiArray[di*2+1] != y )
			return false;
	}

	return true;
}

bool MPathFindPreprocess::IsVaildHeader(PATH_HEADER *pHeader)
{
	if ( pHeader->dwHeader != CURRENT_HEADER )
		return false;
	if ( pHeader->dwVersion != CURRENT_VERSION )
		return false;
	
	return true;
}

bool MPathFindPreprocess::LoadPreprocess(iPathEngine *pathengine, iMesh *mesh, const char *pcStringMD5
										 , iShape *shape ,const char *pathfindFilename ,const char *collisionFilename)
{
	MFile collisionFile;
	if (!collisionFile.Open(collisionFilename))
		return false;

	MFile pathfindFile;
	if (!pathfindFile.Open(pathfindFilename))
		return false;
	
	char *pPathFindBuffer = NULL;
	char *pCollisionBuffer = NULL;
	char *ptrPathFind = NULL;
	char *ptrCollision = NULL;
	
	unsigned int nPathFindFileLength = pathfindFile.GetLength();
	unsigned int nCollisionFileLength = collisionFile.GetLength();

	DWORD time;
	time = timeGetTime();

	// collision file 헤더 검사
	{
		pCollisionBuffer = new char[nCollisionFileLength];
		collisionFile.ReadAll((void*)pCollisionBuffer, nCollisionFileLength);
		
		char *ptr = pCollisionBuffer;
		PATH_HEADER *pHeader = (PATH_HEADER*)ptr;
		ptr += sizeof(PATH_HEADER);

		if (strncmp(ptr, pcStringMD5, MD5_STRING_LENGTH) != 0)
			goto exceptionCatch;

		ptr += MD5_STRING_LENGTH;

		PATH_SHAPEINFO *pShapeInfo = (PATH_SHAPEINFO*)ptr;
		ptr += sizeof(DWORD);
		ptr += pShapeInfo->dwShapeSize*2*sizeof(long);

		if (!IsVaildHeader(pHeader)
			||!IsValidShapeInfo(shape, pShapeInfo))
		{
			goto exceptionCatch;
		}

		ptrCollision = ptr;
	}

	// path-find file 헤더 검사
	{
		pPathFindBuffer = new char[nPathFindFileLength];
		pathfindFile.ReadAll((void*)pPathFindBuffer, nPathFindFileLength);

		char *ptr = pPathFindBuffer;
		PATH_HEADER *pHeader = (PATH_HEADER*)ptr;
		ptr += sizeof(PATH_HEADER);

		if (strncmp(ptr, pcStringMD5, MD5_STRING_LENGTH) != 0)
			goto exceptionCatch;

		ptr += MD5_STRING_LENGTH;

		PATH_SHAPEINFO *pShapeInfo = (PATH_SHAPEINFO*)ptr;
		ptr += sizeof(DWORD);
		ptr += pShapeInfo->dwShapeSize*2*sizeof(long);

		if (!IsVaildHeader(pHeader)
			||!IsValidShapeInfo(shape, pShapeInfo))
		{
			goto exceptionCatch;
		}

		ptrPathFind = ptr;
	}

	_ASSERT(ptrPathFind && ptrCollision);
	
	int nCollisionHeadSize = (int)(ptrCollision - pCollisionBuffer);
	int nPathFindHeadSize = (int)(ptrPathFind - pPathFindBuffer);
	int nCollisionSize = nCollisionFileLength - nCollisionHeadSize;
	int nPathfindSize = nPathFindFileLength - nPathFindHeadSize;
	if ( !pathengine->collisionPreprocessVersionIsCompatible(ptrCollision, nCollisionSize)
		|| !pathengine->pathfindPreprocessVersionIsCompatible(ptrPathFind, nPathfindSize))
		goto exceptionCatch;

	mesh->loadCollisionPreprocessFor(shape, ptrCollision, nCollisionSize);
	mesh->loadPathfindPreprocessFor(shape, ptrPathFind, nPathfindSize);

	//mlog("collision/path-find load time %d ms\n", timeGetTime() - time);

	delete [] pPathFindBuffer;
	delete [] pCollisionBuffer;

	return true;

exceptionCatch:
	// 버퍼 생성 이후 예외 처리
	delete [] pPathFindBuffer;
	delete [] pCollisionBuffer;

	return false;
}