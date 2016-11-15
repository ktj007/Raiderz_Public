#pragma once

#include "XGlueFunction.h"
#include "CSNameStringChecker.h"

class XObject;

class XGlueGame : public XGlueFunction
{
private:
	string _GetDisplayKeyString(const string& strVKey);
	string GetNPCTargetMessage(XObject* pObject);

//	int GetEnchantRequirIndex(int nItemID);

public:
	virtual void DeclGlueFunc( MWLua* pLua);


	// System info
	MWLua::ret_int2 GetWindowSize();
	int GetWindowWidth();
	int GetWindowHeight();
	void ShowCursor( bool bShow);
	bool IsShowCursor();
//	void SetCursor(const char* szCursorName);
	MWLua::ret_int2 GetCursorPos()				{ return MWLua::ret_int2( MGetMint()->GetLastCursorPos().x, MGetMint()->GetLastCursorPos().y);	}
	void SetHistory( const char* szKey, const char* szValue);
	const char* GetHistory( const char* szKey);
	double GetUpdateTime()						{ return global.system->GetTimer().GetTime() * 1000.0;			}
	//void ManagedWindow_OnShow(bool bShow, const char* szWidgetName);
	//void ManagedWindow_Pin(bool bPin, const char* szWidgetName);
	int GetVirtualKey( const char* szVKey);
	const char* GetVirtualKeyStr( int nVKey);
//	void MouseVisible(bool bVisible);
	const char* GetString(const char* szKey);
	const char* GetFormatStr(const char* szKey, const char* szArg1, const char* szArg2, const char* szArg3, const char* szArg4, const char* szArg5);
	const char* GetResultString( int nResult );
	void GetResultSound( int nResult );
	void Quit();
	void ExitToCharSelection();
	bool IsServerModeExpo();
//	int				GetMousePosX()			{ return MEvent::GetMouseClientPos().x; }

	// Loading info
	const char* GetLoadingBkgrndImage(int nFieldID = 0);
	const char* GetLoadingFieldDesc();
	const char* GetLoadingTipMsg();
	float GetLoadingProgress();

	// Character info
	const char* GetName();
	int GetRace();
	int GetSex();
	int GetLevel()								{ return gvar.MyInfo.nLevel;					}
	int GetMoney()								{ return gvar.MyInfo.ChrInfo.GetMoney();		}
	int GetHP()									{ return gvar.MyInfo.ChrStatus.nHP;				}
	int GetEN()									{ return gvar.MyInfo.ChrStatus.nEN;				}
	int GetSTA()								{ return gvar.MyInfo.ChrStatus.nSTA;			}
	int GetExp()								{ return gvar.MyInfo.ChrInfo.nXP;				}
	float GetExpRatio()							{ return (gvar.MyInfo.GetExpPercent() * 0.01f);	}
	int GetArmorPoint()							{ return gvar.MyInfo.EquipmentSlot.GetAP();		}
	int GetFatigue()							{ return gvar.MyInfo.nFatigueType;				}
	bool IsMyPlayerMoving();
	MWLua::ret_int4 GetMyPlayerState();
	void SetBattleStance( bool bSet);
	bool IsBattleStance();
	int GetRecievedQuestCount();
	int GetCompletedQuestCount();
	int GetRecievedRecipeCount();
	int GetCompletedRecipeCount();
	int GetKilledEnemyCount();
	int GetKilledEpicEnemyCount();
	int GetCharColorCount( const char* szPart);
	int GetCharColorID( const char* szPart, int i);
	MWLua::ret_int4 GetCharColor( const char* szPart, int id);
	int GetWeaponSet();
	bool IsInQuestPvpArea();
	const char* GetPvpTeamName();

	// Character account info
	int GetCharacterAccountCount()				{ return (int)gvar.Char.m_CharInfos.size();		}
	int GetCharacterAccountMaxCount()			{ return MAX_ACCOUNT_CHAR_COUNT;				}		///< 계정당 생성 가능한 최대 캐릭터 수	
	const char* GetCharacterAccountName( int i);
	int GetCharacterAccountLevel( int i)		{ return gvar.Char.m_CharInfos[ i].nLevel;		}
	const char* GetCharacterAccountLocation( int i);
	bool SetAccountParam( const char* _name, const char* _type, const char* _param);
	const char* GetAccountParam( const char* _name, const char* _type);
	bool DeleteAccountParam( const char* _name);

	// Character status
	int GetMaxHP()								{ return gvar.MyInfo.GetMaxHP();				}
	int GetMaxEN()								{ return gvar.MyInfo.GetMaxEN();				}
	int GetMaxSTA()								{ return gvar.MyInfo.GetMaxSTA();				}

	int GetSTR();
	int GetDEX();
	int GetINT();
	int GetCON();
	int GetCHA();

	// Character Modifier
	int GetModSTR()								{ return gvar.MyInfo.PlayerModifier_ExceptTalent.nSTR.CalcDiff(gvar.MyInfo.ChrInfo.nSTR);	}
	int GetModDEX()								{ return gvar.MyInfo.PlayerModifier_ExceptTalent.nDEX.CalcDiff(gvar.MyInfo.ChrInfo.nDEX);	}
	int GetModCON()								{ return gvar.MyInfo.PlayerModifier_ExceptTalent.nCON.CalcDiff(gvar.MyInfo.ChrInfo.nCON);	}
	int GetModINT()								{ return gvar.MyInfo.PlayerModifier_ExceptTalent.nINT.CalcDiff(gvar.MyInfo.ChrInfo.nINT);	}
	int GetModCHA()								{ return gvar.MyInfo.PlayerModifier_ExceptTalent.nCHA.CalcDiff(gvar.MyInfo.ChrInfo.nCHA);	}
	
	int GetModHPMax()							{ return gvar.MyInfo.ActorModifier_ExceptTalent.nHPMax.CalcDiff(gvar.MyInfo.GetHP());}
	int GetModENMax()							{ return gvar.MyInfo.ActorModifier_ExceptTalent.nENMax.CalcDiff(gvar.MyInfo.GetEN());}
	int GetModSTAMax()							{ return gvar.MyInfo.ActorModifier_ExceptTalent.nSTAMax.CalcDiff(gvar.MyInfo.GetSTA());}
	int GetModAP()								{ return gvar.MyInfo.GetModAP();	}

	// Regist
	int	GetFR()									{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_FR]; }
	int	GetCR()									{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_CR]; }
	int	GetLR()									{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_LR]; }
	int	GetPR()									{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_PR]; }
	int	GetHR()									{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_HR]; }
	int	GetUR()									{ return gvar.MyInfo.ChrInfo.Resist.nValue[TRT_UR]; }
	
	int GetModFR()								{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_FR].nMod; }
	int GetModCR()								{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_CR].nMod; }
	int GetModLR()								{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_LR].nMod; }
	int GetModPR()								{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_PR].nMod; }
	int GetModHR()								{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_HR].nMod; }
	int GetModUR()								{ return gvar.MyInfo.ChrStatus.ActorModifier.nResistances[TRT_UR].nMod; }

	// Field
	int GetCurrentFieldID();
//	MWLua::ret_int2 GetFieldPosition();
	int GetFieldListCount();
	int GetFieldID( int i);
	const char* GetFieldName( int nID);
	bool IsDynamicField( int nID);
	bool IsShowFieldMap( int nID);
	bool IsCurrentPvPField();
	bool IsCurrentTutorialField();
	int GetDayTime();
	int GetWeather();
//	void ShowCraftNPCPos(int nRecipeID, int nIndex);

	// Channel
	int GetCurrentChannel();
	void RequestChannelList();
	int GetChannelListCount();
	int GetChannelID( int i);
	int GetChannelState( int i);
	void MoveToChannel( int nID);
	void CancelMoveToChannel();
	void LeaveDynamicField();
	bool IsChannelChangeable();
	bool IsCombat();

	// Equipped item info
	bool IsEquippedItem( int nItemID );
	int GetEquippedItemID( int nSlot);
	int GetEquippedItemDurability( int nSlot);
	int GetEquippedItemRepairCost( int nSlot);
	int GetEquippedItemEnchantedStoneID( int nSlot, int i);
	void EquipInvenItem( int nSlot, int nInvenIndex);
	void UnequipItem( int nSlot);
	void ExchangeEquippedItem( int nSlot1, int nSlot2);
	int GetEquipItemQuantityByID( int nID);
	bool IsEquippedItemClaimed(int nSlot);

	// Inventory info
	int GetInvenItemCount() const				{ return gvar.MyInfo.Inventory.GetSize();}
	int GetInvenItemMaxCount() const			{ return gvar.MyInfo.Inventory.GetSlotSize();}
	int GetInvenItemID( int nSlotID) const;
	int GetInvenItemQuantity( int nSlotID) const;
	int GetInvenItemQuantityByID(int nItemID);
	int GetInvenItemDurability( int nSlotID) const;
	int GetInvenItemSellCost( int nSlotID);
	int GetInvenItemRepairCost( int nSlotID);
	int GetInvenItemEnchantedStoneID( int nItemIndex, int i);
	bool IsInvenItemEnabled( int nSlotID) const;
	bool IsInvenItemClaimed( int nSlotID) const;
	bool IsInvenNewItem(int nSlotID) const;
	void ClearInvenNewItemFlag(int nSlotID);
	void ClearAllInvenNewItemFlag();
	int GetInvenItemReuseTimeRemaining( int nSlotID ) const;
	void MoveInvenItem( int nSrcSlotID, int nTarSlotID);
	void UseInvenItem( int nSlotID);
	void DropInvenItem( int nSlotID);
	void SellInvenItem( int nSlotID, int nCount);
	void SellInvenVeryCommonItem();

	// Item Info
	const char* GetItemName( int nItemID) const;
	const char* GetItemImage( int nItemID);
	const char* GetItemDesc( int nItemID) const;
	int GetItemType( int nItemID);
	int GetItemWeaponType( int nItemID);
	int GetItemArmorType( int nItemID);
	int GetItemUsableType( int nItemID);
	int GetItemTier( int nItemID);
	int GetItemQuantity( int nItemID) const;
	int GetItemDurability( int nItemID);
	int GetItemBuyingCost( int nItemID );
	int GetItemStackSize( int nItemID);
	int GetItemDropperCount( int nItemID );
	int GetItemEnchantType( int nItemID);
	int GetItemEnchantLevel( int nItemID);
	int GetItemEnchantHoleCount( int nItemID);
	int GetItemEnchantHoleType( int nItemID, int i);
	MWLua::ret_int2 GetItemEnchantAgentIDRange( int nItemID);
	int GetItemUnchantableWeaponTypeCount(int nItemID);
	int GetItemUnchantableWeaponType(int nItemID, int i);
	int GetItemUnchantableArmorSlotCount(int nItemID);
	int GetItemUnchantableArmorSlot(int nItemID, int i);
	const char* GetItemDropperName( int nItemID, int i);
	const char* GetItemPutUpSound( int nItemID);
	const char* GetItemPutDownSound( int nItemID);
	const char* GetItemMakingSound( int nItemID);
	const char* GetMoneyPutUpSound();
	const char* GetMoneyPutDownSound();
	int GetItemDyeColorCount( int nItemID );
	MWLua::ret_int4 GetItemDyeColor( int nItemID, int nIndex);
	bool IsItemUnique( int nItemID);
	bool IsItemQuestRelated( int nItemID);
	bool IsItemRepairable( int nItemID);
	bool IsItemTradable( int nItemID);
	bool IsItemSellable( int nItemID );
	bool IsItemDiscardable( int nItemID);
	bool IsItemReusable( int nItemID);
	bool IsItemRequiredClaim( int nItemID);
	bool IsItemDyeable( int nItemID);
	bool IsItemRecipeMaterial( int nItemID);
	int GetItemSlot( int nItemID);
	int GetItemMaxDurability( int nItemID);
	int GetItemMinDamage( int nItemID);
	int GetItemMaxDamage( int nItemID);
	float GetItemSpellPower( int nItemID);
	int GetItemArmorPoint( int nItemID);
	int GetItemReuseTime( int nItemID);
	int GetItemEquipReqLevel(int nItemID);
	int GetItemEquipReqMaxLevel( int nItemID );
	MWLua::table GetItemEffects( int nItemID );
	int GetItemReqLicense( int nItemID);
	bool HasItemReqLicense( int nItemID, int nLicense);
	int GetItemProductCount( int nItemID);
	int GetItemProductID( int nItemID, int i);
	//SoulHunterZ
	int GetItemExp( int nItemID, bool bEquipSlot = true );
	int GetReqXP ( int nItemID, bool bEquipSlot = true );
	int GetAttuneLvl( int nItemID, bool bEquipSlot = true );


	// Talent Info
	const char* GetTalentName( int nTalentID);
	const char* GetTalentImage( int nTalentID);
	const char* GetTalentPutUpSound( int nTalentID);
	const char* GetTalentPutDownSound( int nTalentID);
	bool DoTalentRequireFocus( int nTalentID);
	int GetTalentRank( int nTalentID);
	int GetTalentStyle( int nTalentID);
	bool IsTalentFocus( int nTalentID);
//	const char* GetTalentStyleString( int nTalentID);
	const char* GetTalentCategory( int nTalentID);
	const char* GetTalentDesc( int nTalentID);
	bool IsTalentPassiveType( int nTalentID);
	bool DoTalentRequireWeaponType( int nTalentID, int nWeaponType);
	int GetTalentCostHP( int nTalentID);
	int GetTalentCostEN( int nTalentID);
	int GetTalentCostSP( int nTalentID);
	float GetTalentCastingTime( int nTalentID);
	float GetTalentCoolTime( int nTalentID);
	int GetTalentLearnReqLevel( int nTalentID);
	int GetTalentLearnReqStyleTP( int nTalentID);

	int GetTalentModSTR(int nTalentID );
	int GetTalentModDEX(int nTalentID );
	int GetTalentModCON(int nTalentID );
	int GetTalentModINT(int nTalentID );
	int GetTalentModCHA(int nTalentID );

	int GetTalentModHPMax(int nTalentID );
	int GetTalentModENMax(int nTalentID );
	int GetTalentModSTAMax(int nTalentID );

	// Learned talent info
	int GetTalentPoint()						{ return gvar.MyInfo.Talent.GetTP();									}
	int GetSpentTalentPoint( int nStyle)		{ return gvar.MyInfo.Talent.GetStyleUsedTP( (TALENT_STYLE)(nStyle));	}
	int GetSpentTalentPointAll()				{ return gvar.MyInfo.Talent.GetUsedTP();								}
	int GetLearnedTalentCount();
	int GetLearnedTalentID( int i);
	int GetLastLearnedRankedTalentID( int nTalentID);
	int GetEmotionalTalentCount();
	int GetEmotionalTalentID( int i);
	bool IsLearnedTalent(int nTalentID);

	// Buff info
	int GetBuffCount();
	int GetBuffID( int i);
	const char* GetBuffImage( int nID);
	bool IsDebuff( int nID);
	const char* GetBuffName( int nID);
//	MWLua::table GetBuffEffects( int nBuffID);


	// Palette item info
//	const char* GetPaletteItemName( int i);
//	const char* GetPaletteItemImage( int i);
	int GetPalleteItemType( int index);
	int GetPalleteItemID( int index);
	MWLua::ret_int2 SetPaletteItem( int index, int nType, int nID);
	void ChangePaletteSet( int nSet);
	int GetPaletteSetNum();
	void SetWeaponBindedPaletteSet( int nSet);
	int GetWeaponBindedPaletteSet();


	// Party info
	bool IsPartyJoined();
	bool AmIPartyLeader();
	int GetPartyMemberCount()					{ return (int)gvar.MyInfo.Party.GetMemberCounter();		}
	const char* GetPartyMemberName( int i);
	int GetPartyMemberLevel( int i);
	int GetPartyMemberHP( int i);
	int GetPartyMemberEN( int i);
	int GetPartyMemberBuffCount( int i);
	int GetPartyMemberBuffID( int i, int buff);
	bool IsPartyMemberMe( int i);
	bool IsPartyMemberDead( int i);
	bool IsPartyMemberOffline( int i);
	bool IsPartyMemberDisabled( int i);
	bool IsPartyMemberHightlight( int i);
	bool IsPartyLeader( int i);
	void DisbandPartyMember( int i);
	void LeaveParty();

	// Party setting
	int GetLootingRuleCommon();			// 파티세팅 루팅룰
	int GetLootingRuleRare();
	int GetLootingRuleRareFor();
	void ChangePartySetting(int nCommonRule, int nRareRule, int nRareRuleFor, int nLeaderIndex);
	bool IsAutoParty();
	int GetAutoPartyQuestID();
	int GetAutoPartyState();
	void SetAutoPartyState(int nState);

	// Guild info
	const char* GetGuildName() const;
	const char* GetGuildImage() const			{ return "";									}
	int GetGuildMemberCount() const				{ return gvar.MyInfo.Guild.m_mapMember.size();	}
	bool AmIGuildLeader();
	const char* GetGuildMemberName( int i);
	const char* GetGuildMemberGrade( int i);
	bool IsGuildMemberOffline( int i);
	void CreateGuild( const char* _guildname);
	void DelegateGuildLeader( int i);
	void DisbandGuildMember( int i);
	void LeaveGuild();
	void DestroyGuild();
	bool IsValidateGuildName( const char* szName);
	MWLua::ret_int2 GetGuildNameLengthKor();
	MWLua::ret_int2 GetGuildNameLengthEng();

	// Quest info
	int GetPlayerQuestCount() const				{ return (int)gvar.MyInfo.Quests.GetCount();			}
	int GetPlayerQuestID( int i);
	int GetPlayerQuestObjectiveProgress( int id, int nObjectIndex);
	bool IsPlayerQuestFailed( int id);
	bool IsPlayerQuestCompleted( int id);
	bool IsPlayerQuestObjectiveCompleted( int id, int nObjectIndex);
	bool IsPlayerChallengerQuest( int id);
	const char* GetQuestName( int id);
	const char* GetQuestDesc( int id);
	const char* GetQuestCategory( int id);
	int GetQuestObjectiveCount( int id);
	int GetQuestObjectiveType( int id, int nObjectIndex);
	const char* GetQuestObjectiveText( int id, int nObjectIndex);
	int	GetQuestObjectiveProgress( int id, int nObjectIndex);
	int GetQuestObjectiveIndex( int nQuestID, int nObjectiveID);
	int GetQuestRewardCount( int id);
	int GetQuestRewardType( int id, int nRewardIndex);
	int GetQuestRewardValue( int id, int nRewardIndex);
	int GetQuestRewardAmount( int id, int nRewardIndex);
//	const char* GetQuestRewardText( int id, int nRewardIndex);				// 삭제 예정
//	const char* GetQuestRewardImage( int id, int nRewardIndex);				// 삭제 예정
	int GetQuestSelectableRewardCount(  int id);
	int GetQuestSelectableRewardType( int id, int nRewardIndex);
	int GetQuestSelectableRewardValue( int id, int nRewardIndex);
	int GetQuestSelectableRewardAmount( int id, int nRewardIndex);
//	const char* GetQuestSelectableRewardText( int id, int nRewardIndex);	// 삭제 예정
//	const char* GetQuestSelectableRewardImage( int id, int nRewardIndex);	// 삭제 예정
	bool IsQuestShareable( int nQuestID);
	void IndicateQuest( int id);
	void ShareQuest( int id);
	void AcceptShareQuest( int id);
	void GiveUpQuest( int id);
	bool IsAutoPartyQuest(int id);

	// Faction
	int GetFactionCount() const;
	int GetFactionID( int i);
	const char* GetFactionName( int nFactionID);
	int GetFactionValue( int i);
	const char* GetFactionDesc(int i);

	// Shop
	int GetShopItemCount() const;
	int GetShopItemID( int i) const;
	bool IsBuyableShopItem( int nID) const;
	bool IsUsableShopItem( int nID) const;
	float GetBuyDiscountRatio();
	float GetSellDiscountRatio();
	void RepairAllItem();
	void RepairInvenItem( int i);
	void RepairEquippedItem( int nSlot);
	void BuyItem( int nItemID, int nCount);
	bool IsRepairableShop();

	// Journal indicator
	int GetJournalCount();
	int GetJournalType( int i);
	int GetJournalID( int i);
	bool AddJournal( int _type, int _id);
	bool RemoveJournal( int _type, int _id);
	bool IsJournalIndicated( int _type, int _id);

	// Change campaign scene
	void ChangeCampaignScene( int nScene);

	// Login
	bool IsDirectLogin();
	bool IsAlreadyDirectLogined();
	void WaitDirectLogin(bool bWait);
	int GetServerCount();
	const char* GetServerName( int i);
	const char* GetServerAddress( int i);
	MWLua::ret_int2 GetServerState( int i);
	bool GetServerActive( int i);
	int GetServerType( int i);
	void LoginConnect( const char* szServer, const char* szLoginID, const char* szPassword);
	void LoginDisconnect();

	// Character select
	void SelChangedCharacter( int i);
	void SelectCharacter( int i);
	void DeleteCharacter( int i);
	void CreateNewCharacter( const char* szName, MWLua::table t);
	void ChangeNewCharacterModel( MWLua::table t);
	bool IsValidateCharName( const char* szName);
	MWLua::ret_int2 GetCharacterNameLengthKor();
	MWLua::ret_int2 GetCharacterNameLengthEng();

	// Looting
	int GetLootingItemCount() const					{ return gvar.MyInfo.LootingItem.GetSize();										}
	int GetLootingItemID( unsigned int i) const;
	int GetLootingItemType( unsigned int i) const;
	int GetLootingItemQuantity( unsigned int i) const;
// 	bool IsGettableLootingItem(int i);
// 	bool IsRollableLootingItem(int i);
// 	bool IsMasterLootableLootingItem(int i);
	void LootItem( unsigned int i);
	void LootAllItems();
	void RequestLootingEnd();

	// Assign looting
// 	int GetItemIDLootingLooter();
// 	int GetItemCountLootingLooter();
// 	int GetItemnQuantityLootingLooter();
// 	void ReqAssignItem(int nIndex, bool bRandom);
	int GetAssignLootingCount();
	int GetAssignLootingID( int nItemIndex);
	int GetAssignLootingQuantity( int nItemIndex);
	int GetLooterCount(int nItemIndex);
	const char* GetLooterName( int nItemIndex, int nLooterIndex);
	void DoAssignLooting( int nItemIndex, int nWho);

	// Accept quest
	void AcceptQuest( int nID);
	void RejectQuest();
	void SetNPCIconQuestBegin();

	// Quest Reward Dialog
	void AcceptQuestRewardItem( int nQuestID, int nSelectedReward);
	void RejectQuestRewardItem();

	// Enemy Info
	int GetEnemyCount();
	UIID GetEnemyID();
	const char* GetEnemyName();
	int GetEnemyLevel();
	int GetEnemyGrade();
	int GetEnemyHP();
	int GetEnemyMaxHP();
	bool IsShowEnemyInfo();

	// Target Info
	UIID GetTargetID();
	UIID _GetTargetID(UIID uiid);
	const char* GetTargetName();
	const char* _GetTargetName(UIID uiid);
	int GetTargetLevel();
	int _GetTargetLevel(UIID uiid);
	bool _IsLockOnTarget();
	bool IsShowTargetInfo();
	bool _IsShowTargetInfo(UIID uiid);
	int GetTargetGrade();
	int _GetTargetGrade(UIID uiid);
	int GetTargetHP();
	int _GetTargetHP(UIID uiid);
	int GetTargetMaxHP();
	int _GetTargetMaxHP(UIID uiid);
	int GetTargetInteractType();
	MWLua::ret_int4 GetTargetAttackableType();
	const char* GetTargetMessage();
	const char* GetDisplayKeyString(const char* szVKey);

	// Mail box
	int GetMailCount();
	bool IsExistUnreadMail();
	bool IsMailBoxFull();
	const char* GetMailSender( int i);
	const char* GetMailTitle( int i);
	const char* GetMailContent( int i);
	int GetMailRemaindDay( int i);
	bool IsMailRead( int i);
	int GetMailAttachedMoney( int i);
	int GetMailAttachedThumbItemID( int i);
	int GetMailAttachedItemCount( int i);
	int GetMailAttachedItemID( int i, int item);
	int GetMailAttachedItemQuantity( int i, int item);
	int GetMailAttachedItemIndex( int i, int item);
	void AttachItemToSendMail( int i, int qty);
	void DetachItemToSendMail( int i, int qty);
	void ClearSendMailAttachedItems();
	bool ReadMail(int i);
	bool SendMail( const char* szReceiver, const char* szTitle, const char* szContent, int nMoney);
	bool DeleteMail( int i);
	void TakeMailMoney( int i);
	void TakeMailItem( int i, int item);
	const char* GetMailErrorMsg( int _errcode);

	// Rebirth
	int GetRebirthRemainTime();
	void RequireRebirth( int nSelect );
	int GetSoulBindFieldID();
	int GetCheckPointFieldID();
	int GetRebirthItemID();

	// Interaction with PC
	const char* GetInteractionPlayerName() const;
	const bool IsInteractionPlayerPartyLeader() const;
	void InviteToParty();
	void InviteToGuild();
	void RequestToTrade();
	void ChallengeToDuel();
	void ChallengeToPartyDuel();
	const char* GetRequesterName() const;
	void ConfirmInviteToParty( bool bAccept);
	void ConfirmInviteToGuild( bool bAccept);
	void ConfirmRequestToTrade( bool bAccept);
	void ConfirmChallengeToDuel( bool bAccept);

	// Interaction with NPC
	const char* GetNpcName() const;
	const char* GetNpcWelcomeMsg() const;
	int GetCountNpcAnswer();
	const char* GetNpcAnswer( int i);
	void SelectNpcAnswer( int i);
	void RequestNpcInteractionEnd();

	// Dialog with NPC
	const char* GetNpcDialogMessage() const;
	int GetCountNpcDialogAnswer();
	const char* GetNpcDialogAnswer( int i);
	void SelectNpcDialogAnswer( int i);

	// Message
	void		Narration(const char* szMsg);

	// 투기장
	bool IsBattleArena();
	MWLua::ret_int2	GetCurrentBattleArenaScore();

	// Crafting
	int GetCraftingRecipeCount();
	int GetCraftingRecipeID( int i);
// 	int GetCraftingItemID( int i);
 	int GetCraftingItemCost( int i);
// 	int GetCraftingItemQuantity( int i);
// 	int GetCraftingRecipeMaterialCount( int i);
// 	int GetCraftingRecipeMaterialID( int nRecipeIndex, int nMaterialIndex);
// 	int GetCraftingRecipeMaterialQuantity( int nRecipeIndex, int nMaterialIndex);
	bool ObtainCraftingRecipe( int i);
	bool MakeCraftingItem( int i);
	bool IsEnabledCraftingItem( int i);
	bool IsMakableCraftingItem( int i);
// 	int				GetRecipeCount();
// 	int				GetProductItemID(int nRecipeID);
// 	int				GetProductItemIDByIndex(int nIndex);
// 	int				GetPriceMakeItem(int nRecipeID);
// 	int				GetPriceMakeItemByIndex(int nIndex);
// 	MWLua::table	GetMaterialInfoTable(int nRecipeID);
// 	MWLua::table	GetMaterialInfoTableByIndex(int nIndex);
// 	void			CraftingItem( int nIndex);
// 	bool			IsCraftableItem(int nIndex);
// 	void			GetRecipe(int nIndex);
// 	int				GetMakeItemAmountByIndex(int nIndex);

	// Recipe
	int GetRecipeCount();
	int GetRecipeID( int i);
	int GetRecipeItemID( int nID);
	int GetRecipeItemCost( int nID);
	int GetRecipeItemQuantity( int nID);	
	const char* GetRecipeItemName( int nID );
	int GetRecipeMaterialCount( int nID);
	int GetRecipeMaterialID( int nID, int nMaterialIndex);
	int GetRecipeMaterialQuantity( int nID, int nMaterialIndex);
	const char* GetRecipeConditionText( int nID);
	int GetRecipeArtisanCount( int nID);
	const char* GetRecipeArtisanName( int nID, int nIndex);
	int GetRecipeArtisanFieldID( int nID, int nIndex);
	void RemoveRecipe( int nID);
	bool IsHaveRecipe( int nID);


	// 아 진짜 이런 어처구니 없는 함수는 대체 머임?
	string GetConditionText( int nID );

	// 상점
// 	MWLua::table GetBuyAllItemTable();
// 	MWLua::table GetBuyableItemTable();
// 	void BuyItem( int nItemID, int nCount );
// 	bool IsRepairer();
// 	void ReqRepairItemIndex(int index);
// 	void ReqRepairAll();
// 	void BuyItem( MWLua::table t);

	// Trade
	void CancelTrade();
	int GetMaxTradableItemCount();
	int GetMyTradeItemCount();
	int GetMyTradeItemID( int i);
	int GetMyTradeItemQuantity( int i);
	int GetMyTradeMoney();
	int GetOtherTradeItemCount();
	int GetOtherTradeItemID( int i);
	int GetOtherTradeItemQuantity( int i);
	int GetOtherTradeMoney();
	void PutUpTradeItem( int i, int nQuantity);
	void PutDownTradeItem( int i);
	void PutUpMoney( int nSilver);
	void PutDownMoney( int nSilver);
	void ReadyToTrade( bool bReady);
	bool IsReadyToTrade();
	bool IsOtherReadyToTrade();
	// Recipe Journal
//	MWLua::table GetRecipeTable()	{ return gvar.MyInfo.RecipeInfo.GetRecipeTable();	};
//	void GiveUpRecipe( int nRecipeID );


	void PlaySound(const char* szSoundName);
	void PlayBGM(const char* szSoundName);
	void CheckBGM();
	void ReleaseHoldMyCharacter();
	void OutputGuideMsg( int nGuideMsg);
	void ReleaseGuideMsg( int nGuideMsg);

	//Field
	void	ChangeWeather(int nWeather);
	void	ChangeCurrentWeather();

	void	ChangeCustomEnv( const char* szEnvname);
	void	ChangeCustomTimeWeather( const char* szEnvname, int nTime, int nWeather);
	void	ChangeCustomWeather( const char* szEnvname, int nWeather);
	void	ChangeCustomTime( const char* szEnvname, int nTime);

	// Challenger Quest
	int		GetChallengerQuestCount();
	int		GetChallengerQuestID( int nIndex );
	int		GetChallengerQuestEnableType( int nIndex );
	const char* GetChallengerQuestConditionText(int nID);		

	// Enchant
	bool	OpenEnchantItem( int nStoneIndex);
	bool	BeginEnchantItem( int nItemType, int nItemIndex);
	void	CancelEnchantItem();
	float	GetEnchantSuccessPercent( int nStoneID);
	void	DoEnchantingItem( int nItemType, int nItemIndex, int nAgentIndex);
// 	void	EnchantStart(int nSlotID, int nAgentSlotID, bool bEquip);
// 	void	EnchantCancel();
// 	int		GetEnchantItemColor(int nItemID, int nSlotID, int nIndex, bool bEquip);
// 	int		GetEnchantStoneItemColor(int nID);
// 	int		GetEnchantMaxCount(int nID);
// 	int		GetEnchantItemBuffID(int nSlotID, int nIndex, bool bEquip);
// 	int		GetEnchantItemBuffItemID(int nSlotID, int nIndex, bool bEquip);
// 	float	GetEnchantSuccessPercent(int nItemID);
// 	const char* GetEnchantRequirAgentItemName(int nItemID);
// 	int		GetEnchantRequirAgentItemID(int nItemID);
// 	int		GetEnchantRequirLimiteLevel(int nItemID);
// 	int		GetEnchantAgentItemUseLevel(int nItemID);
// 	int		GetEnchantItemPosition(int nSlotID, int nStoneItemID, bool bEquip);
// 	int		GetEnchantAgentAutoPush(int nItemID);
// 	int		GetEnchantCount(int nSlotID, bool bEquip);
// 	bool	CheckEnchantItemType(int nSlotID, int nEnchantStoneID, bool bEquip);
// 	bool    CheckEnchantItemColor(int nSlotID, int nEnchantStoneID, bool bEquip);
// 	void	CheckEnchantItemReq(int nSlotID, int nEnchantAgentSlotID, bool bEquip);
// 	bool EnchantUseInvenItem(int nSlotID);
// 	void EnchantSelectItem(int nSlotID, bool bEquip);


	// Dye
	bool	OpenDyeItem( int nDyeItemIndex);
	bool	BeginDyeItem( int nItemType, int nItemIndex, int nDyeItemIndex);
	void	CancelDyeItem();
	void	DoDyeingItem( int nItemType, int nItemIndex, int nDyeItemIndex);
// 	void	UseDyeItem(SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID, int nDyeSlotID);
// 	void	CandelDyeItem();
//	bool	DyeingItem(SH_ITEM_SLOT_TYPE nTargetSlotType, int nTargetSlotID);
//	MWLua::ret_int4 GetDyeColor(int nIndex);

	// IsDirectLoginToLoginFrame
	bool		IsDirectLoginToLoginFrame();
	// 튜토리얼
	void		HelpTrigger(int nHelpID);
	void		HelpTriggerString(const char* szEventString);
	const char*	GetHelpImg( int nHelpID, int nImgIndex );
	const char*	GetHelpMsg( int nHelpID );
	const char*	GetEventTrigger( int nHelpID );
	float		GetHelpTimer( int nHelpID );
	void		FinishedHelp( int nHelpID );
	void		LockCameraToNPC( int nNPCID, const char* szBoneName, float fTimeSec);


	// Cross Hair
	const char* GetCurrentCrossHairName();
	const char* GetCurrentCrossHairPickName();
	const char* GetCurrentCrossHairAwaysName();

	// string
	const char* Format(const char* format, const char* param0, const char* param1, const char* param2, const char* param3, const char* param4, const char* param5, const char* param6);
};
