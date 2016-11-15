#pragma once

// 새로운 리소스 로더를 사용할거냐 말거냐
//#define NEW_LOADER

#ifdef NEW_LOADER
#define BACKGROUNDLOAD_OFF
#endif

#ifdef NEW_LOADER
#define NEW_LOADER_TEXTUREMAKING
#endif
#ifdef NEW_LOADER
//#define NEW_LOADER_ACTORLOADING
#endif

#define MODIFY_LOADER
