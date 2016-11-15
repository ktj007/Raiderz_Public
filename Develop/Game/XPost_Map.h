#ifndef _XPOST_MAP_H
#define _XPOST_MAP_H

#include "XNetClient.h"
#include "XCommandTable.h"

inline void XPostMap_MarkPos(vec3& pos)
{
	XPOSTCMD1(MC_MINIMAP_MARK_POS_REQ, MCommandParameterVector(pos));
}

#endif//_XPOST_MAP_H