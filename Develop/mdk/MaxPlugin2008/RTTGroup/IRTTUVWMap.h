#ifndef __IRTTUVWMAP_H
#define __IRTTUVWMAP_H


//Andrzej Chomiak
//2005-02-12


#include <iFnPub.h>


#define RTTUVWMAP_CLASS_ID      Class_ID(0x7a6b18a9, 0x168058b3)
#define RTTUVWMAP_INTERFACE     Interface_ID(0x58bf6c12, 0x22942d11)


class IRTTUVWMap : public FPMixinInterface {
public:
    enum {
        kfpGetMapChannel, 
        kfpGetApplyMtlIDs, 
        kfpSetApplyMtlIDs, 
        kfpGetNewMtlID, 
        kfpNewToOriginalMtlID, 
        kfpGetBitmapIndex, 
    };

    virtual int GetMapChannel() = 0;
    virtual BOOL GetApplyMtlIDs() = 0;
    virtual void SetApplyMtlIDs(BOOL applyMtlIDs) = 0;
    virtual int GetNewMtlID(int faceIndex) = 0;
    virtual int NewToOriginalMtlID(int newMtlID) = 0;
    virtual int GetBitmapIndex(int newMtlID) = 0;
};


#endif
