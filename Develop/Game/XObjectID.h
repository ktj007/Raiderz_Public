#pragma once

// XOjbectID 순서는 업데이트 순서입니다. 꼭 지켜야 합니다.
// 1차. 충돌 연관성이 적은 순으로 결정 하였습니다.
// 2차. 다른 오브젝트 정보를 이용하는 순으로 결정 하였습니다. (enum 번호가 클수록 앞쪽 오브젝트 정보를 이용 하는 겁니다.)

enum XObjectID
{
	XOBJ_NA = 0,
	//------------------------------------------------
	XOBJ_NPC,
	XOBJ_BPART_NPC,
	XOBJ_MAGIC_AREA,
	XOBJ_CUTSCENE_PLAYER,

	//------------------------------------------------
	XOBJ_NET_PLAYER,

	//------------------------------------------------
	XOBJ_MY_PLAYER,
	XOBJ_SOUL,

	//------------------------------------------------
	XOBJ_PROJECTILE,
	XOBJ_SWORD_TRAIL,

	//------------------------------------------------
	XOBJ_FEEDBACK,
	XOBJ_MOCK_PLAYER
};

