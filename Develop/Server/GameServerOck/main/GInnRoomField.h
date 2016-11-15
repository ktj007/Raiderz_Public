#ifndef _G_INN_ROOM_FIELD_H_
#define _G_INN_ROOM_FIELD_H_

#include "GDynamicField.h"
#include "MMemPool.h"

class GInnRoomField : public GDynamicField
{
public:
	GInnRoomField(const MUID& uid, const MUID& uidGroup);
	virtual ~GInnRoomField();
};


class GInnRoomFieldMLeaf : public GInnRoomField, public MMemPool<GInnRoomFieldMLeaf>
{
public:
	GInnRoomFieldMLeaf(const MUID& uid, const MUID& uidGroup) : GInnRoomField(uid, uidGroup) {}
	virtual ~GInnRoomFieldMLeaf() {}
};

#endif//_G_INN_ROOM_FIELD_H_
