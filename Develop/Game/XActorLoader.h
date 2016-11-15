#ifndef _XActorLoader_h
#define _XActorLoader_h

//#include "XCharacter.h"

class XCharacter;

// 백그라운드로 액터를 생성/삭제 한다
class XActorLoader
{
public:
	XActorLoader(void);
	~XActorLoader();

	XCharacter*			CreateActor();
	void				DeleteActor(XCharacter* pActor, bool bBackgroundCreation = true);

	void				Destroy();
};

#endif//_RActorLoaderUnit_h
