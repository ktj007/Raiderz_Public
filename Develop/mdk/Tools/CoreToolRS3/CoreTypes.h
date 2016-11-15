/// @file CoreTypes.h
#pragma once

#include <map>
#include <list>

// 전방 선언 및 공통으로 쓰이는 타입들 선언

namespace rs3 {
class RSceneNode;
};

class CEntityObject;
class CLogicObject;

/// 베이스 클래스
class CCoreObject
{
public:
	virtual ~CCoreObject() {}
};

typedef std::map<rs3::RSceneNode*, void*>	SCENEOBJECTMAP;
typedef std::list<CEntityObject*>			ENTITY_OBJECTLIST;
typedef std::list<CLogicObject*>			LOGIC_OBJECTLIST;

// 축 선택 타입
enum AXIS_STATE
{
	AXIS_NULL = 0,
	AXIS_X,
	AXIS_Y,
	AXIS_Z,
	AXIS_XY,
	AXIS_YZ,
	AXIS_XZ
};

/** @brief Listenable View에 전달하는 메시지 */
namespace VIEW_MESSAGE {
	enum ID{
		DEVICE_CREATED = 0,		///< 장치가 만들어을 알림
		ACTIVE_VIEW,			///< View가 활성화됨

		PRE_OPEN_DOCUMENT,		///< 문서가 열리기 직전
		SCENE_LOADED,			///< Scene Load
		SCENE_IMPORTED,			///< Scene Imported(Zone일 경우에만 가능)
		LOGIC_LOADED,			///< Logic Load
		PRE_SAVE_DOCUMENT,		///< 문서가 저장되기 직전임을 알림
		POST_SAVE_DOCUMENT,		///< 문서가 저장된 직후임을 알림
		PRE_CLOSE_DOCUMENT,		///< 문서가 닫히기 직전
		POST_CLOSE_DOCUMENT,	///< 문서가 닫힌 직후


		BEFORE_SCENE_CLEAR,		///< Scene Clear 되기 직전
		BEFORE_RES_DESTROY,		///< 프로그램이 종료되어 Resource가 Destroy 되기전

		LOGIC_NEW,				///< LogicObject 추가
		LOGIC_DELETE,			///< LogicObject 삭제
		LOGIC_MOVE,				///< LogicObject 이동
		LOGIC_CHANGED,			///< LogicObject 속성이 바뀌었을때
		LOGIC_SHOW,				///< LogicObject 이 보여질때
		LOGIC_HIDE,				///< LogicObject 이 숨겨질때

		LOGIC_SELECT,			///< LogicObject 선택
		LOGIC_UNSELECT,			///< LogicObject 선택해제

		LOGIC_RENDER,			///< LogicObject 개별 렌더링
		LOGIC_TEXTINFO_RENDER,	///< LogicObject 에 붙을 텍스트들 렌더링

		LOGIC_LAYER_CHANGED,	///< LogicObject 들의 Layer 가 변경되었을때

		SELECTION_CHANGED,		///< Selection 이 바뀌었을때
		SELECTION_CLEAR,		///< Selection 이 모두 없어질때

		UNDO_GENERAL,			///< General Undo Message
		UNDO_MOVE,				///< Move 에 대한 언두
		UNDO_OBJ_NEW_DEL,		///< 오브젝트 추가/삭제 언두

		UNDO_ENTITY_EDIT,		///< 엔티티 편집 언두

		SCENE_BREAK_INSTANCE,	///< SceneNodeInstance 가 Editable 로 바뀌었다

		WB_STATE_CHANGED,		///< WB state 가 바뀌었을때 알리기
		WB_MOUSE_L_DOWN,		///< 마우스가 Down되었을때 뷰에 알리기
		WB_MOUSE_L_UP			///< 마우스가 Up되었을때 뷰에 알리기
	};
};

/** @brief CWorkBench Edit State */
namespace WORKBENCH_STATE {
	enum STATE {
		TERRAIN_HEIGHT = 0,	///< 지형 높이편집 상태
		TERRAIN_AVER,		///< 지형 평균브러시 상태
		TERRAIN_FLAT,		///< 지형 평평하게 상태
		TERRAIN_SPLAT,		///< 지형 스플래팅 상태
		TERRAIN_RELAX,
		TERRAIN_GRASS,

		//일반 오브젝트와 Entity 오브젝트
		//구체적인 구분은 Map Mode에 의해 결정
		OBJECT_NEW,
		OBJECT_SELECT,
		OBJECT_MOVE,
		OBJECT_ROTATE,
		OBJECT_SCALE,

		//기타
		ETC_PATH_SELECT,
		ETC_CAMERA,
		ETC_MAP_VIEW,
		ETC_DEV,
		ETC_DECAL,

		STATE_NUM ///< 워크벤치 스테이트의 총 수
	};
};

/** @brief Map Edit Type */
enum MAP_EDIT_TYPE
{
	MAP_EDIT_TYPE_SCENE = 0,
	MAP_EDIT_TYPE_LOGIC,
	MAP_EDIT_TYPE_GUNZSERVERLOGIC,
	MAP_EDIT_TYPE_NPCTYPE,
	MAP_EDIT_TYPE_AIBEHAVIOR,
	MAP_EDIT_TYPE_ERROR,

	MAP_EDIT_TYPE_EXIT_SUCCESSED,
	MAP_EDIT_TYPE_EXIT_FAILD,
	MAP_EDIT_TYPE_UNKNOWN
};

/** @brief CoreTool View Type */
enum CORE_VIEW_TYPE
{
	CVT_GENERAL = 0,
	CVT_ROLLUP,			// 롤업
	CVT_DIALOG,			// 독립된 다이얼로그
	CVT_FIXED			// 고정된 다이얼로그 - 툴바/상태표시줄
};