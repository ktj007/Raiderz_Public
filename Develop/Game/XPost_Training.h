#ifndef _XPOST_TRAINING_H
#define _XPOST_TRAINING_H

#include "XNetClient.h"
#include "XCommandTable.h"

inline void XPostTraining_Train(int nTalentID)
{
	XPOSTCMD1(MC_TRAINING_TRAIN_REQ, MCommandParameterInt(nTalentID));
}

#endif//_XPOST_TRAINING_H