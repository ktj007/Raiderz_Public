#ifndef _XGLOBAL_H
#define _XGLOBAL_H

#include "MUID.h"
#include "Mint.h"
#include "MTypes.h"
#include "RTypes.h"
#include "RdeviceD3D.h"
#include "RBackgroundWorker.h"
#include "XScreenDebugger.h"
#include "XConst.h"
#include "MNetClient.h"
#include "XPhysX.h"
#include "XActorLoader.h"
#include "CLangInfo.h"

using namespace mint3;
using namespace rs3;
using namespace minet;

class XGameFrameManager;
class XBaseApplication;
class XSystem;
class XUISystem;
class mint3::Mint;
class XScript;
class rs3::RSceneManager;
class rs3::RDeviceD3D;
class XConfig;
class XObjectManager;
class XNetClient;
class XGame;
class XController;
class XTimer;
class XEffectManager;
class XItemManager;
class XHistory;
class XTalentInfoMgr;
class XItemManager;
class XNPCInfoMgr;
class XFieldInfoMgr;
class XQuestInfoMgr;
class XTalentEffectMgr;
class XRecipeInfoMgr;
class XCameraManager;
class XMotionMgr;
class XBuffInfoMgr;
class CSMeshInfoMgr;
class XResourceManager;
class XTextureMtInfoMgr;
class XBandiCapturer;
class XPhysX;
class XEffectInfoMgr;
class XRealSound;
class XColorTable;
class XEngine;
class XSwordTrailSamplingData;
class XActorLoader;
class XFeedbackSystem;
class XNetwork;
class XFactionInfoMgr;
class XGameState;
class XConditionsInfoMgr;
class XItemConditionsInfoMgr;
class XMsgCommandFacade;
class XItem_DroperInfoMgr;
class XItem_QuestRewarderInfoMgr;
class XItem_RecipeProductInfoMgr;
class XChatChannelManager;
class XInnInfoMgr;
class XModelAnimationInfo;
class XDialogTextInfoMgr;
class XRecipeNPCInfoMgr;
class CLangInfo;

#define __GLOBAL_CHANGE(Type, Value)	Type* Change(Type* a) { Type* pPrev = Value; Value = a; return pPrev; }


/// 전역으로 사용할 클래스를 담아놓는 구조체
///
/// 참고로 전역으로 사용할 변수는 GlobalVar에 넣습니다.
///
class XGlobal 
{
public:
	// core
	rs3::RDeviceD3D*		device;
	rs3::RSceneManager*		smgr;
	rs3::RBackgroundWorker*	bg_worker;
	XActorLoader*			actor_loader;
	mint3::Mint*			mint;
	XNetwork*				net;

	XBaseApplication*		app;
	XSystem*				system;
	XScript*				script;
	XUISystem*				ui;
	XHistory*				history;
	XRealSound*				sound;
	XGameFrameManager*		gfmgr;
	XCameraManager*			camera;
	XEffectManager*			emgr;
	XResourceManager*		res;
	XPhysX*					phx;
	XBandiCapturer*			bandi_capturer;
	XEngine*				engine;
	XFeedbackSystem*		feedback;
	XMsgCommandFacade*		msg;

	MFileSystem*			filesys;

	XGlobal() {
		SetNULL();
	}
	virtual ~XGlobal() {}

	virtual void SetNULL()
	{
		device = NULL;
		smgr = NULL;
		bg_worker = NULL;
		actor_loader = NULL;
		mint = NULL;
		net = NULL;

		app = NULL;
		system = NULL;
		script = NULL;
		ui = NULL;
		history = NULL;
		sound = NULL;
		camera = NULL;
		emgr = NULL;
		res = NULL;
		bandi_capturer = NULL;
		engine = NULL;
		feedback = NULL;
		msg = NULL;
	}

	__GLOBAL_CHANGE(rs3::RDeviceD3D,	device);
	__GLOBAL_CHANGE(rs3::RSceneManager,	smgr);
	__GLOBAL_CHANGE(rs3::RBackgroundWorker,	bg_worker);
	__GLOBAL_CHANGE(XActorLoader,		actor_loader);
	__GLOBAL_CHANGE(mint3::Mint,		mint);
	__GLOBAL_CHANGE(XNetwork,			net);
	__GLOBAL_CHANGE(XEngine,			engine);

	__GLOBAL_CHANGE(XBaseApplication,	app);
	__GLOBAL_CHANGE(XSystem,			system);
	__GLOBAL_CHANGE(XScript,			script);
	__GLOBAL_CHANGE(XUISystem,			ui);
	__GLOBAL_CHANGE(XHistory,			history);
	__GLOBAL_CHANGE(XRealSound,			sound);
	__GLOBAL_CHANGE(XEffectManager,		emgr);
	__GLOBAL_CHANGE(XResourceManager,	res);
	__GLOBAL_CHANGE(XGameFrameManager,	gfmgr);
	__GLOBAL_CHANGE(XCameraManager,		camera);
	__GLOBAL_CHANGE(XFeedbackSystem,	feedback);
	__GLOBAL_CHANGE(XMsgCommandFacade,	msg);
};

class XGlobalInfo
{
public:
	XItemManager*				item;
	XNPCInfoMgr*				npc;
	XFieldInfoMgr*				field;
	XQuestInfoMgr*				quest;	
	XTalentInfoMgr*				talent;
	XTalentEffectMgr*			talent_effect;
	XRecipeInfoMgr*				recipe;
	CSMeshInfoMgr*				mesh_info;
	XMotionMgr*					motion;
	XBuffInfoMgr*				buff;
	XTextureMtInfoMgr*			textureMt;
	XEffectInfoMgr*				effect_Info;
	XColorTable*				colorTable;
	XSwordTrailSamplingData*	swordtrail_sampling;
	XFactionInfoMgr*			faction;
	XModelAnimationInfo*		model_info;

	XConditionsInfoMgr*			cond;
	XItemConditionsInfoMgr*		cond_item;
	XItem_DroperInfoMgr*		item_droper;
	XItem_QuestRewarderInfoMgr*	item_questrewarder;
	XItem_RecipeProductInfoMgr*	item_recipeproduct;

	XChatChannelManager*		chat_channel;	
	XInnInfoMgr*				inn;
	XDialogTextInfoMgr*			dialog_text;

	XRecipeNPCInfoMgr*			recipenpc;

	CLangInfo*					lang;

	XGlobalInfo()
	{
		SetNull();
	}
	virtual ~XGlobalInfo() {}
	virtual void SetNull()
	{
		talent = NULL;
		talent_effect = NULL;
		item = NULL;
		npc = NULL;
		field = NULL;
		quest = NULL;
		recipe = NULL;
		motion = NULL;
		buff = NULL;
		mesh_info = NULL;
		textureMt = NULL;
		effect_Info = NULL;
		colorTable = NULL;
		swordtrail_sampling = NULL;
		faction = NULL;
		cond = NULL;
		cond_item = NULL;
		item_droper = NULL;
		item_questrewarder = NULL;
		item_recipeproduct = NULL;
		chat_channel = NULL;
		inn = NULL;
		model_info = NULL;
		dialog_text = NULL;
		recipenpc = NULL;
		lang = NULL;
	}

	__GLOBAL_CHANGE(XItemManager,				item);
	__GLOBAL_CHANGE(XNPCInfoMgr,				npc);
	__GLOBAL_CHANGE(XFieldInfoMgr,				field);
	__GLOBAL_CHANGE(XQuestInfoMgr,				quest);
	__GLOBAL_CHANGE(XTalentInfoMgr,				talent);
	__GLOBAL_CHANGE(XTalentEffectMgr,			talent_effect);
	__GLOBAL_CHANGE(XRecipeInfoMgr,				recipe);
	__GLOBAL_CHANGE(CSMeshInfoMgr,				mesh_info);
	__GLOBAL_CHANGE(XMotionMgr,					motion);
	__GLOBAL_CHANGE(XBuffInfoMgr,				buff);
	__GLOBAL_CHANGE(XTextureMtInfoMgr,			textureMt);
	__GLOBAL_CHANGE(XEffectInfoMgr,				effect_Info);
	__GLOBAL_CHANGE(XColorTable,				colorTable);
	__GLOBAL_CHANGE(XSwordTrailSamplingData,	swordtrail_sampling);
	__GLOBAL_CHANGE(XFactionInfoMgr,			faction);
	__GLOBAL_CHANGE(XConditionsInfoMgr,			cond);
	__GLOBAL_CHANGE(XItemConditionsInfoMgr,		cond_item);
	__GLOBAL_CHANGE(XItem_DroperInfoMgr,		item_droper);
	__GLOBAL_CHANGE(XItem_QuestRewarderInfoMgr,	item_questrewarder);
	__GLOBAL_CHANGE(XItem_RecipeProductInfoMgr,	item_recipeproduct);
	__GLOBAL_CHANGE(XChatChannelManager,		chat_channel);
	__GLOBAL_CHANGE(XInnInfoMgr,				inn);
	__GLOBAL_CHANGE(XModelAnimationInfo,		model_info);
	__GLOBAL_CHANGE(XDialogTextInfoMgr,			dialog_text);
	__GLOBAL_CHANGE(XRecipeNPCInfoMgr,			recipenpc);
	__GLOBAL_CHANGE(CLangInfo,					lang);
};


class XGlobalGame
{
public:
	XGame*					game;
	XGameState*				currentgamestate;
	XController*			controller;
	XObjectManager*			omgr;

	//
	XGlobalGame() { SetNull(); }
	virtual ~XGlobalGame() {}
	virtual void SetNull()
	{
		game = NULL;
		currentgamestate = NULL;
		controller = NULL;
		omgr = NULL;
	}

	__GLOBAL_CHANGE(XGame,			game);
	__GLOBAL_CHANGE(XGameState,		currentgamestate);
	__GLOBAL_CHANGE(XController,	controller);
	__GLOBAL_CHANGE(XObjectManager,	omgr);
};


extern XGlobal global;
extern XGlobalInfo info;
extern XGlobalGame gg;

//////////////////////////////////////////////////////////////////////////

#endif

