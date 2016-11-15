#ifndef __IRTTGROUP_H
#define __IRTTGROUP_H


//Andrzej Chomiak
//2005-12-24


#include <iFnPub.h>
#include <renderelements.h>


#define RTTGROUP_CLASS_ID       Class_ID(0x27d51fa1, 0x3ba74408)
#define RTTGROUP_INTERFACE      Interface_ID(0x1c8d3da4, 0x2f687874)


class IRTTGroup : public FPMixinInterface {
public:
    enum {
        kfpAddNode, 
        kfpRemoveNode, 
        kfpGetNodes, 
        kfpIsContained, 
        kfpIsBakable, 
        kfpUnwrap, 
        kfpGetManualUpdate, 
        kfpSetManualUpdate, 
        kfpAddBakeElement, 
        kfpRemoveBakeElement, 
        kfpGetBakeElements, 
        kfpGetOutputFileName, 
        kfpCreateBakeElement, 
        kfpSetTargetMapSlot, 
        kfpGetTargetMapSlot, 
        kfpGetOutputPath, 
        kfpSetOutputPath, 
        kfpGetCreateNewBaked, 
        kfpSetCreateNewBaked, 
        kfpGetNewBakedMtl, 
        kfpSetNewBakedMtl, 
        kfpGetMapChannel, 
        kfpSetMapChannel, 
        kfpGetGridSize, 
        kfpSetGridSize, 
        kfpGetThresholdAngle, 
        kfpSetThresholdAngle, 
        kfpGetRotateClusters, 
        kfpSetRotateClusters, 
        kfpGetCheckClusterBorder, 
        kfpSetCheckClusterBorder, 
        kfpGetUseSmGroups, 
        kfpSetUseSmGroups, 
        kfpGetUseMtlIDs, 
        kfpSetUseMtlIDs, 
        kfpGetEdgePadding, 
        kfpSetEdgePadding, 
        kfpGetTextureWidth, 
        kfpSetTextureWidth, 
        kfpGetCWFactor, 
        kfpSetCWFactor, 
        kfpGetGridScale, 
        kfpSetGridScale, 
    };

    virtual int AddNode(INode *pNode) = 0;
    virtual int RemoveNode(INode *pNode) = 0;
    virtual void GetNodes(INodeTab& nodes) = 0;
    virtual bool IsContained(INode *pNode) = 0;
    virtual bool IsBakable(INode *pNode) = 0;

    virtual int Unwrap() = 0;
    virtual bool GetManualUpdate() = 0;
    virtual void SetManualUpdate(bool manualUpdate) = 0;

    virtual int AddBakeElement(MaxBakeElement *pBakeElement) = 0;
    virtual int RemoveBakeElement(MaxBakeElement *pBakeElement) = 0;
    virtual void GetBakeElements(Tab<MaxBakeElement *>& bakeElements) = 0;
    virtual TSTR GetOutputFileName(MaxBakeElement *pBakeElement, int bitmapIndex) = 0;
    virtual MaxBakeElement *CreateBakeElement(const TCHAR *pBEClassName) = 0;
    virtual void SetTargetMapSlot(MaxBakeElement *pBakeElement, Mtl *pMtl, const TCHAR *pName) = 0;
    virtual TSTR GetTargetMapSlot(MaxBakeElement *pBakeElement, Mtl *pMtl) = 0;

    virtual const TCHAR *GetOutputPath() = 0;
    virtual void SetOutputPath(const TCHAR *pOutputPath) = 0;

    virtual bool GetCreateNewBaked() = 0;
    virtual void SetCreateNewBaked(bool createNewBaked) = 0;

    virtual Mtl *GetNewBakedMtl() = 0;
    virtual void SetNewBakedMtl(Mtl *pMtl) = 0;

    virtual int GetMapChannel() = 0;
    virtual void SetMapChannel(int mapChannel) = 0;

    virtual float GetGridSize() = 0;
    virtual void SetGridSize(float gridSize) = 0;

    virtual float GetThresholdAngle() = 0;
    virtual void SetThresholdAngle(float thresholdAngle) = 0;

    virtual bool GetRotateClusters() = 0;
    virtual void SetRotateClusters(bool rotateClusters) = 0;

    virtual bool GetCheckClusterBorder() = 0;
    virtual void SetCheckClusterBorder(bool checkClusterBorder) = 0;

    virtual bool GetUseSmGroups() = 0;
    virtual void SetUseSmGroups(bool useSmGroups) = 0;

    virtual bool GetUseMtlIDs() = 0;
    virtual void SetUseMtlIDs(bool useMtlIDs) = 0;

    virtual int GetEdgePadding() = 0;
    virtual void SetEdgePadding(int edgePadding) = 0;

    virtual int GetTextureWidth() = 0;
    virtual void SetTextureWidth(int textureWidth) = 0;

    virtual float GetCWFactor() = 0;
    virtual void SetCWFactor(float cwFactor) = 0;

    virtual float GetGridScale() = 0;
    virtual void SetGridScale(float gridScale) = 0;
};


#endif
