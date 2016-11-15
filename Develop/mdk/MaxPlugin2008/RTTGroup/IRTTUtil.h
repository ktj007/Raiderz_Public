#ifndef __IRTTUTIL_H
#define __IRTTUTIL_H


//Andrzej Chomiak
//2005-12-27


#include <iFnPub.h>


class RTTFaceSelector {
public:
    virtual Class_ID ClassID() = 0;
    virtual void DeleteThis() = 0;
    virtual bool Init(Object *pRTTGroup, INode *pNode, Mesh *pMesh) = 0;
    virtual void Done(Object *pRTTGroup, INode *pNode, Mesh *pMesh) = 0;
    virtual bool Test(Mesh *pMesh, int face) = 0;
};


#define RTTUTIL_INTERFACE       Interface_ID(0x558769bf, 0x1edd6422)


class IRTTUtil : public FPStaticInterface {
public:
    enum {
        kfpFindRTTGroupNodes, 
        kfpRender, 
        kfpGetShowFrameBuffer, 
        kfpSetShowFrameBuffer, 
        kfpGetRenderToFilesOnly, 
        kfpSetRenderToFilesOnly, 
        kfpGetInstanceDuplicateMaps, 
        kfpSetInstanceDuplicateMaps, 
        kfpGetInstanceDuplicateMtls, 
        kfpSetInstanceDuplicateMtls, 
    };

    virtual void FindRTTGroupNodes(Tab<INode *>& rttGroupNodes) = 0;
    virtual bool Render(Tab<INode *>& rttGroupNodes, bool showProgressDlg) = 0;
    virtual bool GetShowFrameBuffer() = 0;
    virtual void SetShowFrameBuffer(bool showFrameBuffer) = 0;
    virtual bool GetRenderToFilesOnly() = 0;
    virtual void SetRenderToFilesOnly(bool renderToFilesOnly) = 0;
    virtual bool GetInstanceDuplicateMaps() = 0;
    virtual void SetInstanceDuplicateMaps(bool instanceDuplicateMaps) = 0;
    virtual bool GetInstanceDuplicateMtls() = 0;
    virtual void SetInstanceDuplicateMtls(bool instanceDuplicateMtls) = 0;

    virtual void RegisterFaceSelector(RTTFaceSelector *pFaceSelector) = 0;
    virtual void UnRegisterFaceSelector(const Class_ID& classID) = 0;
};


#endif
